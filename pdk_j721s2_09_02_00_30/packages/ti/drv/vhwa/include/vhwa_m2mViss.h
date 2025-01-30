/*
 *   Copyright (c) Texas Instruments Incorporated 2019
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
 *  \ingroup  DRV_VHWA_MODULE
 *  \defgroup DRV_VISS_MODULE VISS Module
 *            VISS Module
 *
 */
/**
 *  \ingroup  DRV_VISS_MODULE
 *  \defgroup DRV_VISS_MODULE_INTERFACE VISS Interface
 *            Interface file for VISS M2M FVID2 driver
 *
 *  @{
 */
/**
 *  \file vhwa_m2mViss.h
 *
 *  \brief  Defines the structures / control operations that could be used to
 *              configure / control VISS M2M driver
 *
 *          Typical Application for the VISS M2M driver is as shown below.
 *
 *          Vhwa_m2mVissInit
 *               ||
 *          Vhwa_m2mVissAllocSl2 (Optional, if not called, first open allocates)
 *               ||
 *          FVID2_Create
 *               ||
 *          sets VISS configuration
 *               ||
 *          FVID2_ProcessReq
 *               ||
 *          Wait for completion callback
 *               ||
 *          FVID2_GetProcessedReq
 *               ||
 *          FVID2_close
 *               ||
 *          FVID2_deInit
 */

#ifndef VHWA_M2M_VISS_H_
#define VHWA_M2M_VISS_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/drv/vhwa/include/rawfe_cfg.h>
#include <ti/drv/vhwa/include/fcp_cfg.h>
#include <ti/drv/vhwa/include/nsf4_cfg.h>
#include <ti/drv/vhwa/include/h3a_cfg.h>
#include <ti/drv/vhwa/include/glbce_cfg.h>
#if defined VHWA_VPAC_IP_REV_VPAC3
#include <ti/drv/vhwa/include/cac_cfg.h>
#endif

#include <ti/drv/udma/udma.h>
#include <ti/drv/vhwa/include/vhwa_common.h>
#include <ti/drv/vhwa/soc/vhwa_soc.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \anchor VhwaM2mViss_Ioctls
 *  \name   Ioctls for the VISS memory to memory driver
 *  \brief  Input/Output control MACRO's for VISS memory to memory module
 *
 *  @{
 */
/**
 * \brief IOCTL for setting VISS configuration.
 *        This IOCTL is used to select the number input, outputs and
 *        any internal path related configuration, it has flags
 *        to enable/disable, which alters blanking requirement.
 *        It is not used for confiugring individual VISS sub-modules.
 *        Typically used just after the create to select inputs, outputs
 *        and paths. Refer to #Vhwa_M2mVissParams
 *
 * \return  FVID2_SOK on success, else error code.
 */
#define IOCTL_VHWA_M2M_VISS_SET_PARAMS          (VHWA_IOCTL_M2M_VISS_IOCTL_BASE)

/**
 * \brief IOCTL for enabling error events and registering callbacks for the
 *        same. Refer to #Ldc_ErrEventParams
 *
 * \return  FVID2_SOK on success, else error code.
 */
#define IOCTL_VHWA_M2M_VISS_REGISTER_ERR_CB     (                              \
    IOCTL_VHWA_M2M_VISS_SET_PARAMS + 1U)

/**
 * \brief IOCTL for enabling and setting HTS limiter.
 *        This is used to slowed down the HTS by introducing clock cycles
 *        between internal start signals. Refer to #Vhwa_HtsLimiter
 *
 * \return  FVID2_SOK on success, else error code.
 */
#define IOCTL_VHWA_M2M_VISS_SET_HTS_LIMIT       (                              \
    IOCTL_VHWA_M2M_VISS_REGISTER_ERR_CB + 1U)

/**
 * \brief IOCTL to sync start each module
 *        This IOCTL doesn't configure any register and only enable pipeline
 *        to start processing
 *
 * \return  FVID2_SOK on success, else error code.
 */
#define IOCTL_VHWA_M2M_VISS_SYNC_START       (                              \
                IOCTL_VHWA_M2M_VISS_SET_HTS_LIMIT + 1U)


/**
 * \brief IOCTL for getting module's performance numbers
 *        for the last frame submitted.
 *
 * \return  FVID2_SOK on success, else error code.
 */
#define IOCTL_VHWA_M2M_VISS_GET_PERFORMANCE     (                           \
                IOCTL_VHWA_M2M_VISS_SYNC_START + 1U)

/**
 * \brief Get Config UDMA buffer information.
 *
 * \return  FVID2_SOK on success, else error code.
 */
#define IOCTL_VHWA_M2M_VISS_GET_BUFF_INFO     (                           \
				IOCTL_VHWA_M2M_VISS_GET_PERFORMANCE + 1U)

/**
 * \brief Set Config UDMA buffer information.
 *
 * \return  FVID2_SOK on success, else error code.
 */
#define IOCTL_VHWA_M2M_VISS_SET_BUFF_INFO     (                           \
				IOCTL_VHWA_M2M_VISS_GET_BUFF_INFO + 1U)

/** @} */

/**
 *  \anchor VhwaM2mViss_InstanceId
 *  \name VHWA VISS Instance ID 
 *
 *  VHWA VPAC VISS Instance ID's
 *
 *  @{
 */
#if defined (SOC_J784S4)
    /** \brief VPAC 0 VISS Instance 0  */
    #define VHWA_M2M_VPAC_0_VISS_DRV_INST_ID_0             (0U)
    /** \brief VPAC 1 VISS Instance 0  */
    #define VHWA_M2M_VPAC_1_VISS_DRV_INST_ID_0             (1U)
    /** \brief Total number of VISS instances */
    #define VHWA_M2M_VISS_DRV_NUM_INST                     (VHWA_M2M_VPAC_1_VISS_DRV_INST_ID_0 + 1U)
#else
    /** \brief VPAC 0 VISS Instance 0  */
    #define VHWA_M2M_VPAC_0_VISS_DRV_INST_ID_0             (0U)
    /** \brief Total number of VISS instances */
    #define VHWA_M2M_VISS_DRV_NUM_INST                     (VHWA_M2M_VPAC_0_VISS_DRV_INST_ID_0 + 1U)
#endif
/* @} */

/**
 *  \brief Default VISS Driver Instance ID [For backward compatibility]
 */
#define VHWA_M2M_VISS_DRV_INST0                 (VHWA_M2M_VPAC_0_VISS_DRV_INST_ID_0)

/**
 *  \brief Maximum VISS driver Instances [For backward compatibility]
 */
#define VHWA_M2M_VISS_DRV_MAX_INST              (VHWA_M2M_VISS_DRV_NUM_INST)

/**
 * \brief Max Number of handles supported  for each instance of VISS M2M Driver
 */
#define VHWA_M2M_VISS_MAX_HANDLES               (8u)

/*
 *  \brief Index for input channel0, ie companded/V short input
 */
#define VHWA_M2M_VISS_INPUT_IDX0                (0U)
/*
 *  \brief Index for input channel1, ie short input
 */
#define VHWA_M2M_VISS_INPUT_IDX1                (1U)
/*
 *  \brief Index for input channel2, ie long input
 */
#define VHWA_M2M_VISS_INPUT_IDX2                (2U)
/*
 *  \brief Max Input Channels
 */
#define VHWA_M2M_VISS_MAX_INPUTS                (3U)


/**
 *  \anchor VhwaM2mViss_OutputIdx
 *  \name   Ioctls for the VISS memory to memory driver
 *  \brief  Input/Output control MACRO's for VISS memory to memory module
 *
 *  @{
 */
#define VHWA_M2M_VISS_OUT_YUV420_12B_IDX        (0U)
#define VHWA_M2M_VISS_OUT_GREY_12B_IDX          (0U)
#define VHWA_M2M_VISS_OUT_COLOR_12B_IDX         (0U)
#define VHWA_M2M_VISS_OUT_LUMA_12B_IDX          (0U)
#define VHWA_M2M_VISS_OUT_YUV422_12B_IDX        (0U)
#define VHWA_M2M_VISS_OUT_CFA_C1_12B_IDX        (1U)
#define VHWA_M2M_VISS_OUT_CHROMA_12B_IDX        (1U)
#define VHWA_M2M_VISS_OUT_YUV420_8B_IDX         (2U)
#define VHWA_M2M_VISS_OUT_YUV422_8B_IDX         (2U)
#define VHWA_M2M_VISS_OUT_RGB888_8B_IDX         (2U)
#define VHWA_M2M_VISS_OUT_GREY_8B_IDX           (2U)
#define VHWA_M2M_VISS_OUT_COLOR_8B_IDX          (2U)
#define VHWA_M2M_VISS_OUT_CFA_C2_12B_IDX        (2U)
#define VHWA_M2M_VISS_OUT_LUMA_8B_IDX           (2U)
#define VHWA_M2M_VISS_OUT_RED_8B_IDX            (2U)
#define VHWA_M2M_VISS_OUT_RGI_8B_IDX            (2U)
#define VHWA_M2M_VISS_OUT_CFA_C3_12B_IDX        (3U)
#define VHWA_M2M_VISS_OUT_CHROMA_8B_IDX         (3U)
#define VHWA_M2M_VISS_OUT_GREEN_8B_IDX          (3U)
#define VHWA_M2M_VISS_OUT_SATURA_8B_IDX         (4U)
#define VHWA_M2M_VISS_OUT_CFA_C4_12B_IDX        (4U)
#define VHWA_M2M_VISS_OUT_BLUE_8B_IDX           (4U)
#define VHWA_M2M_VISS_OUT_H3A_IDX               (5U)
/*
 *  \brief Max Output Channels
 */
#define VHWA_M2M_VISS_MAX_OUTPUTS               (6U)
/** @} */

/**
 *  \anchor Vhwa_M2mVissInputMode
 *  \name   VISS input mode
 *  \brief  Decides the number of input channels required,
 *          essentially decides the merge mode.
 *
 *          Caution: This macro values are used for internal calculations.
 *
 *  @{
 */
/**< VISS Single Frame Input Mode */
#define VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT   (1U)
/**< VISS Two Frame Merge Mode */
#define VHWA_M2M_VISS_MODE_TWO_FRAME_MERGE      (2U)
/**< VISS Three Frame Merge Mode */
#define VHWA_M2M_VISS_MODE_THREE_FRAME_MERGE    (3U)
/**< Max VISS Mode */
#define VHWA_M2M_VISS_MODE_MAX                  (4U)
/** @} */

/**
 *  \anchor Vhwa_M2mVissEdgeEnhancerMode
 *  \name   VISS Luma Interpolation type
 *  \brief  Enum to define Luma interpolation type. For all other components,
 *         i.e. chroma or bayer, bilinear interpolation is used.
 *
 *  @{
 */
/**< Edge Enhancer is disabled */
#define VHWA_M2M_VISS_EE_DISABLE                (0x0U)
/**< Edge Enhancer is enabled on Luma12 bit output for HV */
#define VHWA_M2M_VISS_EE_ON_LUMA12              (0x1U)
/**< Edge Enhancer is enabled on Luma8 bit output for HV */
#define VHWA_M2M_VISS_EE_ON_LUMA8               (0x2U)
#if defined VHWA_VPAC_IP_REV_VPAC3
/**< Edge Enhancer is enabled on Luma12 bit output for MV */
#define VHWA_M2M_VISS_EE_ON_MV_LUMA12           (0x4U)
/**< Edge Enhancer is enabled on Luma8 bit output for MV */
#define VHWA_M2M_VISS_EE_ON_MV_LUMA8            (0x8U)
#endif
/**< Max value, used for error checking */
#define VHWA_M2M_VISS_EE_MAX                    (0xFFFFU)
/** @} */

#if defined VHWA_VPAC_IP_REV_VPAC3
/**
 *  \anchor Vhwa_M2mVissPipeMode
 *  \name   VISS HV/MV Pipeline
 *  \brief  MACRO to use the MV pipeline and select input for MV pipeline
 *
 *  @{
 */
 /**< HV output pipeline */
#define VHWA_VISS_PIPE_HV                       (0u)
/**< MV output pipeline */
#define VHWA_VISS_PIPE_MV                       (1u)

/**< Use RFE output for MV pipeline */
#define VHWA_VISS_MV_PIPE_INPUT_RFE             (0u)
/**< Use NSF4 output for MV pipeline */
#define VHWA_VISS_MV_PIPE_INPUT_NSF4            (1u)
/**< Use GLBC output for MV pipeline */
#define VHWA_VISS_MV_PIPE_INPUT_GLBC            (2u)
/**< Use CAC output for MV pipeline */
#define VHWA_VISS_MV_PIPE_INPUT_CAC             (3u)
/** @} */
#endif
/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct Vhwa_m2mVissInitParams
 *  \brief Init Parameters for VISS M2M Driver
 */
typedef struct
{
    Udma_DrvHandle              udmaDrvHndl;
    /**< UDMA driver handle, used for configuring UTC */
    Vhwa_IrqInfo                irqInfo;
    /**< IRQ Information */
    bool						configThroughUdmaFlag;
    /**< this should be enabled when configuration through UDMA is enabled */
} Vhwa_M2mVissInitParams;

typedef struct
{
    Vhwa_GetTimeStamp  getTimeStamp;
    /**< Function pointer to get timestamp */

    uint32_t                    enablePsa;
    /**< Flag to enable/Disble LSE PSA Module */
} Vhwa_M2mVissCreateArgs;

typedef struct
{
    /* Following parameters are used by the driver for the
       SL2 memory allocation */
    uint32_t                    maxInWidth[VHWA_M2M_VISS_MAX_INPUTS];
    /**< Maximum input image width,
     *   If set to 0, memory for that channel is not allocated.
     *   Depth and CCSF is common for all three input channels,
     *   But interface for max Input width is provided, so that
     *   internal memory can be allocated for less than 3 channels */
    uint32_t                    inCcsf;
    /**< Input image storage format,
     *   It is common for all input channels */
    uint32_t                    inDepth;
    /**< Buffer Depth
     *   It is common for all input channels */

    uint32_t                    maxOutWidth[VHWA_M2M_VISS_MAX_OUTPUTS];
    /**< Maximum input image width */
    uint32_t                    outCcsf[VHWA_M2M_VISS_MAX_OUTPUTS];
    /**< Output image storage format */
    uint32_t                    outDepth[VHWA_M2M_VISS_MAX_OUTPUTS];
    /**< Buffer Depth */
} Vhwa_M2mVissSl2Params;

typedef struct
{
    uint32_t                    enable;
    /**< Flag to enable/disble H3A output,
     *   Used it here, so that driver allocates/reserves resources for H3A
     *   AF/AEWB config can be set later on using SET_CONFIG ioctl.
     *   Request can be submitted only if H3A Set config is called
     *   when this flag is set. ie driver does not allow submitting request
     *   without setting h3a config, in case it is enabled here. */
    Fvid2_Format                fmt;
    /**< Output Format */
#if defined VHWA_VPAC_IP_REV_VPAC3
    uint32_t                    vPipeline;
    /**< Vision pipeline: Human vision or Machine vision
            VHWA_VISS_PIPE_HV
            VHWA_VISS_PIPE_MV
     */
#endif
} Vhwa_M2mVissOutputParams;

/**
 *  struct Vhwa_M2mVissParams
 *  \brief Connfiguration parameters of VISS.
 */
typedef struct
{
    uint32_t                    inputMode;
    /**< VISS Mode, decides the number of inputs,
     *   Refer \ref Vhwa_M2mVissInputMode for valid values */

    Fvid2_Format                inFmt;
    /**< Input Frame format, describing input frames storage format
     *   Used for configuring input frame format for all three inputs
     *   Frame size for all three inputs are used from this format
     *   Used in all VISS Mode \ref Vhwa_M2mVissInputMode
     *
     *   Following parametes are used from this
     *   width      = width of the input frame,
     *                max width supports is 4096 pixels
     *   height     = height of the input frame
     *   pitch      = line offset for the input data,
     *                used based on the data format
     *   dataFormat = Input DataFormat, only Bayer formats supports as of now
     *   sfcc       = Storage format
     *
     *   In case of multi-exposure input, the only parameter that can
     *   change among exposure is pitch/line offset. In this case
     *   pitch[n] is used to specify pitch of n exposures. */

    Vhwa_M2mVissOutputParams    outPrms[VHWA_M2M_VISS_MAX_OUTPUTS];
    /**< Output Frame Parameters, used to enable output and also to describe
     *   output frames storage format.
     *   Output format can be indexede using index \ref VhwaM2mViss_OutputIdx
     *   and be enabled in the output outPrms structure. the same instance has
     *   fvid2_format which describes the storage format for the
     *   corresponding output.
     *
     *   Following parametes are used from this
     *   width      = width of the output frame,
     *                must be same as input frame width
     *   height     = height of the input frame
     *                must be same as input frame height
     *   pitch      = line offset for the input data, used
     *                based on the data format
     *   dataFormat = DataFormat,
     *                Supported data formats
     *                  Output0: YUV420, Grey, RAW, Luma Only
     *                  Output1: RAW, Chroma Only
     *                  Output2: YUV420, YUV422, RGB, Grey, RAW, Luma only, RED
     *                  Output3: RAW, Chroma Only, Green
     *                  Output4: Saturation, RAW, BLUE
     *   sfcc       = Storage format
     *
     *   Not used for describing frame format for H3A output, H3A outputs
     *   8bit linear data. Driver internally ignores H3A output format */

    uint32_t                    enableGlbce;
    /**< Flag to include GLBCE in the pipeline */
    uint32_t                    enableNsf4;
    /**< Flag to include NSF4 in the pipeline */
    uint32_t                    edgeEnhancerMode;
    /**< Enables Edge Enhancer on Luma12 output,
     *   Mainly used in calculating vertical blanking,
     *   Refer \ref Vhwa_M2mVissEdgeEnhancerMode For valid values */
    uint32_t                    enableDpc;
    /**< Enable DPC
     *   Mainly used in calculating vertical blanking */
#if defined VHWA_VPAC_IP_REV_VPAC3
    uint32_t                    enableCac;
    /**< Flag to include CAC in the pipeline */

    uint32_t                    enableMVPipe;

    uint32_t                    mvPipeInSel;
    /**< MV pipe input select
     *      VHWA_VISS_MV_PIPE_INPUT_RFE
     *      VHWA_VISS_MV_PIPE_INPUT_NSF4
     *      VHWA_VISS_MV_PIPE_INPUT_GLBC */
#endif
    uint32_t                    chromaMode;
    /**< Chroma Mode, 0: 420 1: 422
     *   Used only when CHROMA_ONLY output format selected.
     *   to determine chroma downsampling,
     *   Refer \ref Fcp_ChromaMode for valid values */
} Vhwa_M2mVissParams;

/**
 * Structure contains the application buffer information to store
 * configuration, which will be written through UDMA.
 */
typedef struct
{
	/**< Buffer pointer passed from the application */
	uint32_t	*bufferPtr;
	/**< length of application buffer in number of bytes bytes */
	uint32_t	length;
	/**< configuration through UDMA is enabled or not */
	bool        configThroughUdmaFlag;
	/**< set to TRUE if cache policy of MPU for allocated buffer is cache write back */
	bool        isCacheWriteBackPolicy;
} Vhwa_M2mVissConfigAppBuff;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief This function should be used to initialize variable of type
 *          #Vhwa_M2mVissInitParams.
 *
 *  \param prms     A pointer of type Vhwa_M2mVissInitParams
 *
 *  \return         None
 */
static inline void Vhwa_m2mVissInitParamsInit(Vhwa_M2mVissInitParams *prms);

/**
 *  \brief This function should be used to initialize variable of type
 *          #Vhwa_M2mVissCreateArgs.
 *
 *  \param prms     A pointer of type #Vhwa_M2mVissCreateArgs
 *
 *  \return         None
 */
static inline void Vhwa_m2mVissCreateArgsInit(Vhwa_M2mVissCreateArgs *prms);

/**
 *  \brief This function should be used to initialize variable of type
 *          #Vhwa_M2mVissParams.
 *
 *  \param prms     A pointer of type #Vhwa_M2mVissParams
 *
 *  \return         None
 */
static inline void Vhwa_m2mVissParamsInit(Vhwa_M2mVissParams *prms);

/**
 *  \brief This function should be used to initialize variable of type
 *          #Vhwa_M2mVissSl2Params.
 *
 *  \param prms     A pointer of type #Vhwa_M2mVissSl2Params
 *
 *  \return         None
 */
static inline void Vhwa_m2mVissSl2ParamsInit(Vhwa_M2mVissSl2Params *prms);

/**
 *  \brief Function to initialize VISS with provided init parameters
 *
 *  \param initPrms         Init Parameters containing base address
 *                          and utc channel parameters
 *                          This parameter should not be 0.
 *  \return                 Returns 0 on success else returns error value
 */
int32_t Vhwa_m2mVissInit(Vhwa_M2mVissInitParams *initPrms);

/**
 *  \brief Function to deinitialize, cleans up the internal data structures
 */
void Vhwa_m2mVissDeInit(void);

/**
 *  \brief Function to allocate Sl2 memory for the output buffers.
 *
 *  \param sl2AllocPrms Pointer to a #Vhwa_M2mVissSl2Params structure
 *                      containing the SL2 allocation parameters
 *
 *  \return FVID2_SOK if successful, else suitable error code
 */
int32_t Vhwa_m2mVissAllocSl2(const Vhwa_M2mVissSl2Params *sl2AllocPrms);

/**
 *  \brief Function to free allocated SL2.
 *
 */
void Vhwa_m2mVissFreeSl2(void);

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

static inline void Vhwa_m2mVissInitParamsInit(Vhwa_M2mVissInitParams *prms)
{
    if (NULL != prms)
    {
        prms->udmaDrvHndl = NULL;
        Viss_getIrqInfo(&prms->irqInfo);
        prms->configThroughUdmaFlag = false;
    }
}

static inline void Vhwa_m2mVissCreateArgsInit(Vhwa_M2mVissCreateArgs *prms)
{
    if (NULL != prms)
    {
        prms->getTimeStamp = NULL;
        prms->enablePsa = (uint32_t)UFALSE;
    }
}

static inline void Vhwa_m2mVissParamsInit(Vhwa_M2mVissParams *prms)
{
    if (NULL != prms)
    {
        prms->inputMode = VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT;
        prms->inFmt.width = 1280U;
        prms->inFmt.height = 720U;
        prms->inFmt.pitch[0] = 1280U * 2U;
        prms->inFmt.dataFormat = FVID2_DF_RAW;
        prms->inFmt.ccsFormat = FVID2_CCSF_BITS12_UNPACKED16;

        prms->outPrms[VHWA_M2M_VISS_OUT_YUV420_8B_IDX].enable = (uint32_t)UTRUE;
        prms->outPrms[VHWA_M2M_VISS_OUT_YUV420_8B_IDX].fmt.width = 1280U;
        prms->outPrms[VHWA_M2M_VISS_OUT_YUV420_8B_IDX].fmt.height = 720U;
        prms->outPrms[VHWA_M2M_VISS_OUT_YUV420_8B_IDX].fmt.pitch[0U] = 1280U;
        prms->outPrms[VHWA_M2M_VISS_OUT_YUV420_8B_IDX].fmt.pitch[1U] = 1280U;
        prms->outPrms[VHWA_M2M_VISS_OUT_YUV420_8B_IDX].fmt.dataFormat =
            FVID2_DF_YUV420SP_UV;
        prms->outPrms[VHWA_M2M_VISS_OUT_YUV420_8B_IDX].fmt.ccsFormat =
            FVID2_CCSF_BITS8_PACKED;

        prms->enableGlbce = (uint32_t)UFALSE;
        prms->enableNsf4 = (uint32_t)UFALSE;
#if defined VHWA_VPAC_IP_REV_VPAC3
        prms->enableCac = (uint32_t)UFALSE;
#endif
        prms->edgeEnhancerMode = VHWA_M2M_VISS_EE_DISABLE;
        prms->enableDpc = (uint32_t)UFALSE;
        prms->chromaMode = FCP_CHROMA_MODE_420;
    }
}

static inline void Vhwa_m2mVissSl2ParamsInit(Vhwa_M2mVissSl2Params *prms)
{
    uint32_t cnt;

    if (NULL != prms)
    {
        for (cnt = 0U; cnt < VHWA_M2M_VISS_MAX_INPUTS; cnt ++)
        {
            prms->maxInWidth[cnt] = 1920U;
        }
        prms->inCcsf = FVID2_CCSF_BITS12_UNPACKED16;
        prms->inDepth = 3U;
        for (cnt = 0U; cnt < VHWA_M2M_VISS_MAX_OUTPUTS; cnt ++)
        {
            if (VHWA_M2M_VISS_OUT_H3A_IDX == cnt)
            {
                prms->maxOutWidth[cnt] = 1024U;
            }
            else
            {
                prms->maxOutWidth[cnt] = 1920U;
            }

#if defined VHWA_VPAC_IP_REV_VPAC
            if (cnt < VHWA_M2M_VISS_OUT_YUV420_8B_IDX)
            {
                prms->outCcsf[cnt] = FVID2_CCSF_BITS12_UNPACKED16;
            }
            else
            {
                prms->outCcsf[cnt] = FVID2_CCSF_BITS8_PACKED;
            }
#elif defined VHWA_VPAC_IP_REV_VPAC3
            prms->outCcsf[cnt] = FVID2_CCSF_BITS12_UNPACKED16;
#endif
            prms->outDepth[cnt] = 2U;
        }
    }
}

#ifdef __cplusplus
}
#endif

#endif /* VHWA_M2M_VISS_H_ */

/* @} */
