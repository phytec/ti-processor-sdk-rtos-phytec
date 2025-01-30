/*
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
 *  \ingroup  DRV_VHWA_MODULE
 *  \defgroup DRV_DOF_MODULE DOF Module
 *            DOF Module
 *
 */
/**
 *  \ingroup  DRV_DOF_MODULE
 *  \defgroup DRV_DOF_MODULE_INTERFACE DOF Interface
 *            Interface file for DOF M2M FVID2 driver
 *
 *  @{
 */
/**
 *  \file vhwa_m2mDof.h
 *
 *  \brief  Defines the structures / control operations that could be used to
 *          configure / control DOF M2M driver
 */

#ifndef VHWA_M2M_DOF_H_
#define VHWA_M2M_DOF_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <ti/drv/vhwa/include/dof_cfg.h>
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
 *  \anchor Vhwa_DofIoctl
 *  \name   VHWA DOF IOCTL macros
 *  \brief  Input/Output control MACRO's for DOF driver
 *
 *  @{
 */
/** \brief IOCTL for setting DOF configuration */
#define VHWA_M2M_IOCTL_DOF_SET_PARAMS                 \
                                (VHWA_IOCTL_DOF_IOCTL_BASE)
/** \brief IOCTL for setting Confidence score parameters */
#define VHWA_M2M_IOCTL_DOF_SET_CONF_SCORE_PARAMS      \
                                (VHWA_M2M_IOCTL_DOF_SET_PARAMS  + 1u)
/** \brief IOCTL for setting bandwidth limiter */
/* TODO rename don't call BW limiter */
#define VHWA_M2M_IOCTL_DOF_SET_HTS_LIMIT             \
                                (VHWA_M2M_IOCTL_DOF_SET_CONF_SCORE_PARAMS + 1u)
/** \brief IOCTL for setting next Pyramid level to be processsed this IOCTL
           should be called bfore process request */
#define VHWA_M2M_IOCTL_DOF_SET_NEXT_PYR               \
                                (VHWA_M2M_IOCTL_DOF_SET_HTS_LIMIT + 1u)
/** \brief IOCTL for getting IP performance */
#define VHWA_M2M_IOCTL_DOF_GET_PERFORMANCE            \
                                (VHWA_M2M_IOCTL_DOF_SET_NEXT_PYR + 1u)
/** \brief IOCTL for getting the PSA signature */
#define VHWA_M2M_IOCTL_DOF_GET_PSA_SIGN               \
                                (VHWA_M2M_IOCTL_DOF_GET_PERFORMANCE + 1u)
/** \brief IOCTL for getting flow vector histogram */
#define VHWA_M2M_IOCTL_DOF_GET_HISTOGRAM              \
                                (VHWA_M2M_IOCTL_DOF_GET_PSA_SIGN + 1u)
/** \brief IOCTL for registering the Error Callback */
#define VHWA_M2M_IOCTL_DOF_REGISTER_ERR_CB            \
                                (VHWA_M2M_IOCTL_DOF_GET_HISTOGRAM + 1u)

/**
 * \brief IOCTL to sync start each module
 *        This IOCTL doesn't configure any register and only enable pipeline
 *        to start processing
 */
#define VHWA_M2M_IOCTL_DOF_SYNC_START            \
                                (VHWA_M2M_IOCTL_DOF_REGISTER_ERR_CB + 1u)
 /** @} */

/** Maximim number of inputs DOF buffers */
#define VHWA_M2M_DOF_MAX_IN_BUFFER              (5U)

/** Maximim number of output DOF buffers */
#define VHWA_M2M_DOF_MAX_OUT_BUFFER             (1U)

/** Maximim number of total DOF buffers */
#define VHWA_M2M_DOF_MAX_BUFFER            (VHWA_M2M_DOF_MAX_IN_BUFFER + \
                                            VHWA_M2M_DOF_MAX_OUT_BUFFER)

/**< Instance ID for the DOF M2m driver */
#define VHWA_M2M_DOF_DRV_INST_ID            (0U)

/** \brief Max Number of handles supported by DOF M2M Driver */
#define VHWA_M2M_DOF_MAX_HANDLES            (4u)

/**
 *  \anchor Vhwa_DofIOIndex
 *  \name   VHWA DOF IO buffer indexes
 *  \brief  Input/Output buffer MACRO's for DOF driver
 *
 *  @{
 */
#define DOF_INPUT_REFERENCE_IMG     (0U)
#define DOF_INPUT_CURRENT_IMG       (1U)
#define DOF_INPUT_TEMPORAL_PRED     (2U)
#define DOF_INPUT_PYRAMID_PRED      (3U)
#define DOF_INPUT_SOF               (4U)
#define DOF_OUTPUT                  (5U)
/* Below two buffer will be used Driver internally and are not supposed to be
   used by application */
#define DOF_INPUT_FOCO_REF_IMG      (6U)
#define DOF_INPUT_FOCO_CURR_IMG     (7U)

/** @} */

/**
 *  \anchor Vhwa_DofDefaultInitPrms
 *  \name   VHWA DOF Default Init Paramters
 *  \brief  VHWA DOF Default Init Paramters for Sl2
 *
 *  @{
 */
#define DOF_DEFAULT_IMG_WIDTH       (1312U)
#define DOF_DEFAULT_CCSF            (FVID2_CCSF_BITS12_UNPACKED16)
#define DOF_DEFAULT_REF_BUFF_DEPTH  (2U)
#define DOF_DEFAULT_CUR_BUFF_DEPTH  (2U)
#define DOF_DEFAULT_FV_BUFF_DEPTH   (4U)
#define DOF_DEFAULT_TOP_SR          (48U)
#define DOF_DEFAULT_BOT_SR          (48U)
/** @} */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct Vhwa_M2mDofPrms
 *  \brief Connfiguration parameters of DOF.
 */
typedef struct
{
    uint32_t            tPrmdLvl;
    /**< Total number of pyramid */

    uint32_t            skipTempPredLine;
    /**< Enter Value 1 if alternate line is to be skipped 0 for other wise */

    uint32_t            flowVectorHeight;
    /**< Height of the flow vector in case of SOF is used */

    uint32_t            tPredictor;
    /**< Predictor used by the top layer possible values
            * DOF_PREDICTOR_DELEY_LEFT    */
    uint32_t            mPredictor1;
    /**< Predictor used by the middle pyramidal layer, possible values
            * DOF_PREDICTOR_DELEY_LEFT
            * DOF_PREDICTOR_PYR_LEFT
            * DOF_PREDICTOR_PYR_COLOCATED
      */
    uint32_t            mPredictor2;
    /**< Predictor used by the middle pyramidal layer, possible values
            * DOF_PREDICTOR_DELEY_LEFT
            * DOF_PREDICTOR_PYR_LEFT
            * DOF_PREDICTOR_PYR_COLOCATED
      */
    uint32_t            bPredictor1;
    /**< Predictor used by the base layer, possible values
            * DOF_PREDICTOR_DELEY_LEFT
            * DOF_PREDICTOR_PYR_LEFT
            * DOF_PREDICTOR_PYR_COLOCATED
            * DOF_PREDICTOR_TEMPORAL
      */
    uint32_t            bPredictor2;
    /**< Predictor used by the base layer, possible values
            * DOF_PREDICTOR_DELEY_LEFT
            * DOF_PREDICTOR_PYR_LEFT
            * DOF_PREDICTOR_PYR_COLOCATED
            * DOF_PREDICTOR_TEMPORAL
      */

    Fvid2_Format        inOutImgFmt[DOF_MAX_PYR_LVL_SUPPORTED][VHWA_M2M_DOF_MAX_BUFFER];
    /**< Input Frame format, describing input frames storage format
         Following parametes are used from this
         pitch = line offset for the input data, used based on the data format
         dataFormat = Input DataFormat, Only used for Current frame,
                      same value is used for reference frame
         sfcc = Storage format, Only used for Current frame, same value is used
                for reference frame
     */

    Dof_Config          coreCfg;
    /**< DOF configuration */

    Vhwa_FocoPrms       focoPrms;
    /**< DOF FOCO Parameters */

} Vhwa_M2mDofPrms;

/**
 *  struct Vhwa_M2mDofCreateArgs
 *  \brief Parameters required to create a new handle
 */
typedef struct
{
    Vhwa_GetTimeStamp  getTimeStamp;
    /**< Function pointer to get timestamp */

    uint32_t enablePsa;
    /**< Flag to enable/Disble DOF PSA Module */
}Vhwa_M2mDofCreateArgs;

/**
 *  struct Vhwa_M2mDofInitParams
 *  \brief Initialization Parameters for DOF M2M Driver
 */
typedef struct
{
    Udma_DrvHandle     udmaDrvHndl;
    /**< UDMA driver handle */
    Vhwa_IrqInfo       irqInfo;
    /**< IRQ Information */
}Vhwa_M2mDofInitParams;

/**
 *  struct Vhwa_m2mDofSl2AllocPrms
 *  \brief Init Parameters required to allocate DOF driver
 */
typedef struct
{
    /** These parameters are used by the driver to allocater SL2 memory, used
     *  all driver handles.
     *  Ensure to allocate max of all handles requirement.
     */
    uint32_t        maxImgWidth;
    /**< Maximun image width */
    uint32_t        inCcsf;
    /**< CCSF for input image */
    uint32_t        refBuffDepth;
    /**< Input buffer depth in SL2 for reference image, final depth will be
         calculated based on tob and bottom searc range.
         Minumum configurable depth is 2 */
    uint32_t        currBuffDepth;
    /**< Input buffer depth in SL2 for Current image
         Minumum configurable depth is 2 */
    uint32_t        fvBuffDepth;
    /**< Flow vector buffer depth in Sl2 */
    uint32_t        topSR;
    /**< Top search range */
    uint32_t        botSR;
    /**< Bottom search range */
} Vhwa_M2mDofSl2AllocPrms;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief Vhwa_M2mDofInitParams structure init function.
 *
 *  \param  dofInitPrms  Pointer to #Vhwa_M2mDofInitParams structure.
 *
 *  \return None
 */
static inline void Vhwa_m2mDofInitParamsInit(
                                Vhwa_M2mDofInitParams *dofInitPrms);

/**
 *  \brief Function to initialize the DOF configuration parameters to default
 *
 *  \param dofCfg   Pointer to a #Vhwa_M2mDofPrms containing DOF configuration
 *
 *  \return         None
 */
static inline void Vhwa_m2mDofConfigInit(Vhwa_M2mDofPrms *dofCfg);

/**
 *  \brief Function to initialize the DOF create argument parameters to default
 *
 *  \param createArgs   Pointer to a #Vhwa_M2mDofCreateArgs containing
 *                      DOF create arguments
 *
 *  \return         None
 */
static inline void Vhwa_m2mDofCreateArgsInit(Vhwa_M2mDofCreateArgs *createArgs);

/**
 *  \brief DOF initialization function.
 *   This function initializes the DOF hardware and drivers.
 *   This function should be called before calling any driver APIs and
 *   only once.
 *
 *  \param pInitPrms   Pointer to a #Vhwa_M2mDofInitParams structure
 *                     containing the DOF driver initialization parameters
 *
 *  \return FVID2_SOK if successful, else suitable error code
 */
int32_t Vhwa_m2mDofInit(Vhwa_M2mDofInitParams *pInitPrms);

/**
 *  \brief DOF de-initialization function.
 *   This function un-initializes the DOF hardware and drivers.
 *   This function should be called during system shutdown if
 *   Vhwa_m2mDofDeInit() was called by the application.
 *
 *  \return None
 */
void Vhwa_m2mDofDeInit(void);

/**
 *  \brief Function to allocate Sl2 memory for input and output buffers.
 *         Both DOF and SDE SL2 memory should be freed before reallocating.
 *
 *  \param sl2allocPrms Pointer to a #Vhwa_M2mDofSl2AllocPrms structure
 *                      containing the SL2 allocation parameters
 *
 *  \return FVID2_SOK if successful, else suitable error code
 */
int32_t Vhwa_m2mDofAllocSl2(const Vhwa_M2mDofSl2AllocPrms *sl2allocPrms);

/**
 *  \brief Function to free allocated SL2.
 *
 *  \return FVID2_SOK if successful, else suitable error code
 */
int32_t Vhwa_m2mDofFreeSl2(void);

/**
 *  \brief Function to Initialize the confidence score with default value.
 *
 *  \param csPrms       Pointer to a #Dof_ConfScoreParam structure which needs
 *                      to be initialized
 *
 *  \return None
 */
void Vhwa_m2mConfScoreParamInit(Dof_ConfScoreParam *csPrms);

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

static inline void Vhwa_m2mDofInitParamsInit(
                                Vhwa_M2mDofInitParams *dofInitPrms)
{
    if (NULL != dofInitPrms)
    {
        Dof_getIrqInfo(&dofInitPrms->irqInfo);
        dofInitPrms->udmaDrvHndl = NULL;
    }
}

static inline void Vhwa_m2mDofConfigInit(Vhwa_M2mDofPrms *dofCfg)
{
    if (NULL != dofCfg)
    {
        (void)memset(dofCfg, 0, sizeof(Vhwa_M2mDofPrms));
    }
}

static inline void Vhwa_M2mDofSl2AllocPrmsInit(Vhwa_M2mDofSl2AllocPrms *sl2Prms)
{
    sl2Prms->maxImgWidth = DOF_DEFAULT_IMG_WIDTH;
    sl2Prms->inCcsf = DOF_DEFAULT_CCSF;
    sl2Prms->refBuffDepth = DOF_DEFAULT_REF_BUFF_DEPTH;
    sl2Prms->currBuffDepth = DOF_DEFAULT_CUR_BUFF_DEPTH;
    sl2Prms->fvBuffDepth = DOF_DEFAULT_FV_BUFF_DEPTH;
    sl2Prms->topSR = DOF_DEFAULT_TOP_SR;
    sl2Prms->botSR = DOF_DEFAULT_BOT_SR;
}

static inline void Vhwa_m2mDofCreateArgsInit(Vhwa_M2mDofCreateArgs *createArgs)
{
    if (NULL != createArgs)
    {
        (void)memset(createArgs, 0, sizeof(Vhwa_M2mDofCreateArgs));
    }
}

#ifdef __cplusplus
}
#endif

#endif /* VHWA_M2M_DOF_H_ */

/** @} */
