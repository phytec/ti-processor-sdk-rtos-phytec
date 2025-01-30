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
 *  \file vhwa_m2mMscPriv.h
 *
 *  \brief MSC Private header file
 *
 */

#ifndef VHWA_M2M_MSC_PRIV_H_
#define VHWA_M2M_MSC_PRIV_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/csl/soc.h>
#include <ti/osal/osal.h>

#include <ti/csl/csl_fvid2_dataTypes.h>
#include <ti/drv/fvid2/include/fvid2_drvMgr.h>

#include <ti/drv/vhwa/src/csl/include/csl_msc.h>
#include <ti/drv/vhwa/src/csl/include/csl_hts.h>
#include <ti/drv/vhwa/src/csl/include/csl_lse.h>
#include <ti/drv/vhwa/src/csl/include/csl_vpactop.h>

#include <ti/drv/vhwa/include/vhwa_common.h>
#include <ti/drv/vhwa/include/vhwa_m2mMsc.h>

#include <ti/drv/vhwa/src/drv/vhwa_utils.h>
#include <ti/drv/vhwa/src/drv/vhwa_cfg.h>

#include <vhwa_vpac_priv.h>


#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
/** \brief Maximum number of DMA channel per handle */
#define VHWA_M2M_MSC_MAX_DMA_CH         (MSC_MAX_OUTPUT + \
                                        (VHWA_M2M_MSC_MAX_INST * VHWA_M2M_MSC_MAX_IN_CHANNEL))

/** \brief Maximum possible number of MSC Input. */
#define VHWA_M2M_MSC_INPUT_MAX           (2u)

/** \brief Minimum input lines required in SL2 */
#define MSC_M2M_MIN_IN_SL2_LINES         (6u)
/** \brief Minimum output lines required in SL2 */
#define MSC_M2M_MIN_OUT_SL2_LINES        (2u)

/** \brief Maximum supported Queue Length per instance */
#define VHWA_MSC_QUEUE_LEN_PER_INST      (15U)

/** \brief Total Transfer Record Descriptors */
#define MAX_SUPPORTED_WIDTH_HEIGHT       (8191u)

/** \brief Default width - Used to initialize configuration parameters */
#define VHWA_MSC_DEF_WIDTH               (64U)
/** \brief Default Height - Used to initialize configuration parameters */
#define VHWA_MSC_DEF_HEIGHT              (32U)

/** \brief Maximum loop count - used while dequeuing UDMA */
#define VHWA_M2M_MSC_MAX_WAIT_LOOP_CNT   (5000u)

/** \brief Total Transfer Record Descriptors */
#define VHWA_MSC_UDMA_NUM_TR_DESC       (1U)

/**
 *  \brief UDMA TR packet descriptor memory.
 *  This contains the CSL_UdmapCppi5TRPD + Padding to sizeof(CSL_UdmapTR15) +
 *  one Type_15 TR (CSL_UdmapTR15) + one TR response of 4 bytes.
 *  Since CSL_UdmapCppi5TRPD is less than CSL_UdmapTR15, size is just two times
 *  CSL_UdmapTR15 for alignment.
 */
#define VHWA_MSC_UDMA_TRPD_SIZE               (sizeof(CSL_UdmapTR15) + \
    (sizeof(CSL_UdmapTR15) * VHWA_MSC_UDMA_NUM_TR_DESC) + \
    (VHWA_MSC_UDMA_NUM_TR_DESC * 4U) + \
    (UDMA_CACHELINE_ALIGNMENT - (VHWA_MSC_UDMA_NUM_TR_DESC * 4U)))


/**
 * \brief IOCTL for getting the MSC SL2 parameters
 *        This IOCTL require pointer to structure Vhwa_M2mMscSl2Params
 * \return  FVID2_SOK on success, else error code.
 */
#define IOCTL_VHWA_MSC_GET_SL2_PARAMS   (VHWA_IOCTL_FC_MSC_IOCTL_BASE)

/**
 * \brief IOCTL To get the params
 *        This IOCTL require pointer to structure Vhwa_M2mMscFcGetPrms
 * \return  FVID2_SOK on success, else error code.
 */
#define IOCTL_VHWA_MSC_FC_GET_PARAMS   (IOCTL_VHWA_MSC_GET_SL2_PARAMS + 1U)

/**
 * \brief IOCTL for updating MSC config using Flex-Connect
 *        This IOCTL require pointer to structure Vhwa_M2mMscFcConPrms
 * \return  FVID2_SOK on success, else error code.
 */
#define IOCTL_VHWA_MSC_FC_CONN_PARAMS   (IOCTL_VHWA_MSC_FC_GET_PARAMS + 1U)

/**
 * \brief IOCTL for updating MSC SL2 config using Flex-Connect
 *        This IOCTL require pointer to structure Vhwa_M2mMscFcUpdatePrms
 * \return  FVID2_SOK on success, else error code.
 */
#define IOCTL_VHWA_MSC_UPDATE_FC_PARAMS     (IOCTL_VHWA_MSC_FC_CONN_PARAMS + 1U)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  \brief Structure containing the channel parameters
 */
typedef struct
{
    uint32_t  buffEnable;

    uint8_t  *trMem[VHWA_M2M_MSC_MAX_COMP];
    /**< Pointer to TR memory of this channel */
    uint32_t sl2Pitch;
    /**< Pitch required in SL2 for this channel */

    uint32_t width;
    uint32_t ccsf;
    uint32_t height;
} Vhwa_M2mMscInChParams;

/**
 *  \brief Structure containing the channel parameters
 */
typedef struct
{
    uint32_t  buffEnable;

    uint8_t  *trMem[VHWA_M2M_MSC_MAX_COMP];
    /**< Pointer to TR memory of this channel */
    uint32_t sl2Pitch;
    /**< Pitch required in SL2 for this channel */
} Vhwa_M2mMscOutChParams;

typedef struct
{
    /* Parameters used to Update the MSC HTS, LSE and TR */
    uint32_t                outputIdx;
    /**< Output channel Index */
    uint64_t                sl2Addr;
    uint32_t                sl2Depth;
    uint32_t                sl2Pitch;
    uint32_t                ctrlIdx;
    /**< MSC Producer Control index */

    /* HTS Specific parameters */
    uint32_t                htsConsId;
    uint32_t                htsThreshold;
    uint32_t                htsDmaProdPreLoad;
    uint32_t                htsDmaProdPostLoad;
    uint32_t                htsDmaProdCntDec;
    uint32_t                enableMaskSel;
    uint32_t                maskSel;
} Vhwa_M2mMscFcConPrms;

typedef struct
{
    uint32_t                isFlexConnect;
    uint32_t                inDmaEnable[VHWA_M2M_MSC_MAX_IN_CHANNEL];
    uint32_t                outDmaEnable[MSC_MAX_OUTPUT];
} Vhwa_M2mMscFcUpdatePrms;

typedef struct
{
    /* Parameters used to configure the previous Node */
    CSL_HtsSchConfig *htsCfg;

    uint32_t         inSl2Pitch[VHWA_M2M_MSC_MAX_IN_CHANNEL];
} Vhwa_M2mMscFcGetPrms;

/**
 *  \brief Structure containing the SL2 parameters
 *         These paramenters are are used to configure the TR to transfer image
 *         data to SL2
 */
typedef struct
{
    uint64_t                sl2StartAddr;
    /**< SL2 Start Address allocated to MSC driver */
    uint32_t                sl2MemSize;
    /**< SL2 Memory Size allocated to MSC driver */

    uint64_t                inSl2Addr[VHWA_M2M_MSC_MAX_INST][VHWA_M2M_MSC_MAX_IN_CHANNEL];
    /**< SL2 address for each input buffer */
    uint32_t                inWidthInBytes[VHWA_M2M_MSC_MAX_INST][VHWA_M2M_MSC_MAX_IN_CHANNEL];
    /**< SL2 input buffer width in bytes */
    uint32_t                inSl2BuffDepth[VHWA_M2M_MSC_MAX_INST][VHWA_M2M_MSC_MAX_IN_CHANNEL];
    /**< SL2 buffer depth for each input buffer */

    uint64_t                outSl2Addr[MSC_MAX_OUTPUT];
    /**< SL2 address for each output buffer */
    uint32_t                outWidthInBytes[MSC_MAX_OUTPUT];
    /**< SL2 output buffer width in bytes */
    uint32_t                outSl2BuffDepth[MSC_MAX_OUTPUT];
    /**< SL2 buffer depth for each input buffer */
} Vhwa_M2mMscSl2Params;

typedef struct
{
    uint32_t                isFlexConnect;
    uint32_t                inDmaEnable[VHWA_M2M_MSC_MAX_IN_CHANNEL];
    uint32_t                outDmaEnable[MSC_MAX_OUTPUT];
} Vhwa_M2mMscFcStatus;

/**
 *  \brief Structure containiing configuration paramenter for each opened handle
 */
typedef struct
{
    uint32_t                 isUsed;
    /**< Flag to indicate if this object is free or not */

    uint32_t                 instId;
    /**< Instance ID for the request */

    uint32_t                 scalarUsed;
    /**< Binary Map storing scalar instances used */

    Vhwa_M2mMscCreatePrms    createPrms;
    /**< Create argument parameters for MSC driver */

    Vhwa_M2mMscInChParams    inChPrms[VHWA_M2M_MSC_INPUT_MAX];
    /**< MSC channel parameters for the input buffers */

    Vhwa_M2mMscOutChParams   outChPrms[MSC_MAX_OUTPUT];
    /**< MSC channel parameters for the output buffers */

    CSL_LseConfig            lseCfg[VHWA_M2M_MSC_MAX_COMP];
    /**< LSE Configuration for Luma and Chroma components*/

    Vhwa_M2mMscParams        mscPrms;
    /**< MSC Configuration for this handle */

    CSL_MscConfig           cslMscCfg[VHWA_M2M_MSC_MAX_COMP];
    /**< Structure to store Configuration for Luma and Chroma components */

    uint32_t                 vsSchFactor[MSC_MAX_OUTPUT][VHWA_M2M_MSC_MAX_COMP];
    /**< Variable to store veritcal Scaling factor for Luma and Chroma */

    CSL_HtsSchConfig         htsCfg[VHWA_M2M_MSC_MAX_COMP];
    /**< HTS Configuration for Luma and Chroma components */

    uint32_t                 idx;
    /**< The index of this handle object in the array,
         used in allocating UDMA memory */

    uint32_t                 numIter;
    /**< When input is YUV420, it requires two passes of MSC,
         one for luma and one for chroma, This variable stores
         number of passes required
     */

    uint32_t                 numInChUsed;
    /**< When input is YUV420, and dual channels are supported
     */

    uint32_t                curIterCnt;
    /**< Current iteration count used to process Luma and chroma components */

    uint64_t                 perf[VHWA_M2M_MSC_MAX_COMP];
    /**< Variables storing last performance numbers in clock cycles
         for both luma and chroma */

    uint32_t                 swOvhd[VHWA_M2M_MSC_MAX_COMP];
    /**< Variable to store the Software overhead in clock cycles */

    volatile uint32_t        numPendReq;
    /**< Number of pending requests to be dequeued by application once
     *   submitted to driver. */

    Msc_ErrEventParams      eePrms;
    /**< Parameters for the Error Events */

    Fvid2UtilsLinkListObj   *outQ;
    /**< Queue object to put the processed output requests. */

    Fvid2UtilsLinkListObj   outLlObj;
    /**< Linked List object for outQ. */

    Vhwa_M2mMscPsaSign       psa;
    /**< PSA Signature of the last submission */

    Fvid2_DrvCbParams       fdmCbPrms;
    /**< Structure containing frame completion callback parameters */

    Fvid2_FrameList         inFrmList;
    /**< Input frame list parameters */

    Fvid2_FrameList         outFrmList;
    /**< Output frame list parameters */

    uint32_t                isCfgUpdated;
    /**< Flag to know if config is updated or not. */

    Vhwa_M2mMscFcStatus     fcStatus;
    /**< Flexconnet related config structure */

}Vhwa_M2mMscHandleObj;

/**
 *  \brief Structure defining the queue object used in queue/dequeue operation.
 *   Instead of creating frame objects, this is used so that any other
 *   information could be queued/dequeued along with the frame.
 *   Already qElem is added to avoid dynamic allocation of Queue nodes.
 */
typedef struct
{
    Fvid2Utils_QElem       qElem;
    /**< FVID2 Utils queue element used in node addition. */
    Vhwa_M2mMscHandleObj   *hObj;
    /**< Handle Object reference for this Queue object */
    Fvid2_FrameList         inFrmList;
    /**< FVID2 frame list for store list of input frames. */
    Fvid2_FrameList         outFrmList;
    /**< FVID2 frame list for store list of output frames. */
} Vhwa_M2mMscQueueObj;

/**
 *  \brief Structure containiing configuration paramenter for each MSC instance
 */
typedef struct
{
    SemaphoreP_Handle       lockSem;
    /**< Semaphore to protect function calls and other memory allocation. */

    SemaphoreP_Handle       hwaLock;
    /**< Semaphore to protect MSC HWA.
     *   Only one processing request can be submitted to the MSC HW.
     *   This lock makes sure that except submitted one, all other
     *   requests are waiting on this semaphore.
     *   There is no longer support for submission from ISR and so
     *   no ISR protection in processRequest. */

    uint32_t                openCnt;
    /**< Keeps track of number of opens,
         used to initialize common configuration only once
         for example, enabling MSC at the top level */

    uint32_t                pipeline;
    /**< HTS Pipeline number,
         Same pipeline is used for all handles,
         Allocated on the first open */

    HwiP_Handle             intrHandle;
    /**< Handle to Hardware Interrupt */

    /**
     * Initially all the queue elements will be in freeQ.
     *
     * For every application queue operation,      freeQ -> inQueue
     * For every request submitted to hardware,    inQueue  -> activeQObj
     * For every request completion from hardware, activeQObj  -> outQ
     * For every application dequeue operation,    outQ -> freeQ
     */
    Vhwa_M2mMscQueueObj     *activeQObj;
    /**< Flag to indicate if any request is active and already
     *   submitted to hardware. */
    Vhwa_M2mMscHandleObj   *lastHndlObj;
    /**< Pointer to the handle object, whose request was submitted
     *   and completed */

    Fvid2UtilsLinkListObj   *freeQ;
    /**< Queue for queueing all the free queue objects for this handle. */
    Fvid2UtilsLinkListObj   freeLlObj;
    /**< Linked List object for inQueue. */
    Vhwa_M2mMscQueueObj     mscQObj[VHWA_MSC_QUEUE_LEN_PER_INST];
    /**< MSC queue objects */

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
    /**< Keeps track of Invalid HTS status */
    uint32_t errNoActQObj;
    uint32_t errNoLastHndlObj;
}Vhwa_M2mMscInstObj;

/**
 *  \brief Structure containiing paramenter common across both instance
 */
typedef struct
{
    uint32_t                initDone;
    /**< Variable to check if Init is done */

    uint32_t                isSl2AllocDone;
    /**< Flag to check if SL2 Alloc is done or not */

    SemaphoreP_Handle       scLockSem[MSC_MAX_OUTPUT];
    /**< Semaphore to protect function calls and other memory allocation for each scalar output */

    struct Udma_ChObj       inChObj[VHWA_M2M_MSC_MAX_INST][VHWA_M2M_MSC_MAX_IN_CHANNEL];
    /**< UDMA Channel object for input channels */
    Udma_ChHandle           inChHandle[VHWA_M2M_MSC_MAX_INST][VHWA_M2M_MSC_MAX_IN_CHANNEL];
    /**< UDMA Channel handles for input channels */
    uint32_t                inComplRingNum[VHWA_M2M_MSC_MAX_INST][VHWA_M2M_MSC_MAX_IN_CHANNEL];
    /**< UDMA Completion ring number for input channels */

    struct Udma_ChObj       outChObj[MSC_MAX_OUTPUT];
    /**< UDMA Channel object for output channels */
    Udma_ChHandle           outChHandle[MSC_MAX_OUTPUT];
    /**< UDMA Channel handles for output channels */
    uint32_t                outComplRingNum[MSC_MAX_OUTPUT];
    /**< UDMA Completion ring number for output channels */

    Vhwa_M2mMscInitParams   mscInitPrms;
    /**< Variable to store the driver init parameters */

    Vhwa_M2mMscSl2Params    sl2Prms;
    /**< Variable to store sl2 parameters */

    Vhwa_M2mMscQueueObj    *qConfigObj;
    /**< Common queue config object pointer
         This pointer is used to store the next pending request which couldn't
         be submitted due to conflict */

    Msc_SocInfo             socInfo;
    /**< Variable to store register address map */

    uint32_t                mscProdStatus;
    /**< Variable to store the MSC Prod channel used */

    Vhwa_M2mMscSl2AllocPrms sl2AllocPrms;
    /**< Local copy of SL2 alloc parameters */
}Vhwa_M2mMscCommonObj;

/* ========================================================================== */
/*                           Function Declarations                            */
/* ========================================================================== */

int32_t Vhwa_m2mMscUdmaChInit(Vhwa_M2mMscCommonObj *commonObj);

int32_t Vhwa_m2mMscUdmaChDeInit(const Vhwa_M2mMscCommonObj *commonObj);

void Vhwa_mscM2mSetTrDesc(Vhwa_M2mMscHandleObj *hObj,
                          const Vhwa_M2mMscCommonObj *comObj);

int32_t Vhwa_m2mMscStartCh(const Vhwa_M2mMscInstObj *instObj,
    const Vhwa_M2mMscCommonObj *comObj);

int32_t Vhwa_m2mMscStopCh(const Vhwa_M2mMscCommonObj *comObj);

int32_t Vhwa_mscM2mSubmitRing(const Vhwa_M2mMscInstObj *instObj,
                              const Vhwa_M2mMscHandleObj *hObj,
                              const Vhwa_M2mMscCommonObj *comObj,
                              uint32_t compCnt);

int32_t Vhwa_m2mMscAllocUdmaMem(Vhwa_M2mMscHandleObj *hObj);

void Vhwa_m2mMscSetAddress(Vhwa_M2mMscHandleObj *hObj,
                           const Fvid2_FrameList *inFrmList,
                           const Fvid2_FrameList *outFrmList);

int32_t Vhwa_m2mMscPopRings(Vhwa_M2mMscHandleObj *hObj,
                            const Vhwa_M2mMscCommonObj *comObj,
                            uint32_t compCnt);

int32_t Vhwa_m2mMscSetConfigInHW(const Vhwa_M2mMscQueueObj *qObj,
                                Vhwa_M2mMscCommonObj *comObj,
                                uint32_t intCnt);
int32_t Vhwa_m2mMscSetFrameSize(const Vhwa_M2mMscQueueObj *qObj,
                                Vhwa_M2mMscCommonObj *comObj,
                                uint32_t intCnt);

int32_t Vhwa_m2mMscSubmitRequest(Vhwa_M2mMscInstObj *instObj,
                                Vhwa_M2mMscQueueObj *qObj,
                                Vhwa_M2mMscCommonObj *comObj,
                                uint32_t intCnt);

int32_t Vhwa_m2mMscRegisterIsr(Vhwa_M2mMscInstObj *instObj,
                               const Vhwa_M2mMscCommonObj *comObj,
                               uint32_t instId);

void Vhwa_m2mMscUnregisterIsr(Vhwa_M2mMscInstObj *instObj,
                              const Vhwa_M2mMscCommonObj *comObj,
                               uint32_t instId);

uint32_t Vhwa_m2mMscCalcHorzSizeInBytes(uint32_t width, uint32_t ccsf,
                                        uint32_t dataFmt);

void Vhwa_m2mMscUpdatePipeline(uint32_t instId, uint32_t htsPipeline);

Vhwa_M2mMscHandleObj *Vhwa_m2mMscGetHandleObj(uint32_t instance, uint32_t cnt);

#ifdef __cplusplus
}
#endif

#endif  /** VHWA_M2M_MSC_PRIV_H_ */
