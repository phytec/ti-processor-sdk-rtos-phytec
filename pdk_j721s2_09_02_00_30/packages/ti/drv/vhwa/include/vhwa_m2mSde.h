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
 *  \defgroup DRV_SDE_MODULE SDE Module
 *            SDE Module
 *
 */
/**
 *  \ingroup  DRV_SDE_MODULE
 *  \defgroup DRV_SDE_MODULE_INTERFACE SDE Interface
 *            Interface file for SDE M2M FVID2 driver
 *
 *  @{
 */
/**
 *  \file vhwa_m2mSde.h
 *
 *  \brief  Defines the structures / control operations that could be used to
 *          configure / control SDE M2M driver
 */


#ifndef VHWA_M2M_SDE_H_
#define VHWA_M2M_SDE_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <ti/drv/vhwa/include/sde_cfg.h>
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
 *  \anchor Vhwa_SdeIoctl
 *  \name   VHWA SDE IOCTL macros
 *  \brief  Input/Output control MACRO's for SDE driver
 *
 *  @{
 */
/** \brief IOCTL for setting SDE configuration */
#define VHWA_M2M_IOCTL_SDE_SET_PARAMS                 \
                                (VHWA_IOCTL_SDE_IOCTL_BASE)
/** \brief IOCTL for setting bandwidth limiter */
#define VHWA_M2M_IOCTL_SDE_SET_HTS_LIMIT             \
                                (VHWA_M2M_IOCTL_SDE_SET_PARAMS + 1u)
/** \brief IOCTL for getting IP performance */
#define VHWA_M2M_IOCTL_SDE_GET_PERFORMANCE            \
                                (VHWA_M2M_IOCTL_SDE_SET_HTS_LIMIT + 1u)
/** \brief IOCTL for getting the PSA signature */
#define VHWA_M2M_IOCTL_SDE_GET_PSA_SIGN               \
                                (VHWA_M2M_IOCTL_SDE_GET_PERFORMANCE + 1u)
/** \brief IOCTL for getting flow vector histogram */
#define VHWA_M2M_IOCTL_SDE_GET_HISTOGRAM              \
                                (VHWA_M2M_IOCTL_SDE_GET_PSA_SIGN + 1u)
/** \brief IOCTL for registering the Error Callback */
#define VHWA_M2M_IOCTL_SDE_REGISTER_ERR_CB            \
                                (VHWA_M2M_IOCTL_SDE_GET_HISTOGRAM + 1u)

/**
 * \brief IOCTL to sync start each module
 *        This IOCTL doesn't configure any register and only enable pipeline
 *        to start processing
 */
#define VHWA_M2M_IOCTL_SDE_SYNC_START            \
                                (VHWA_M2M_IOCTL_SDE_REGISTER_ERR_CB + 1u)
 /** @} */

/** Maximim number of inputs SDE buffers */
#define VHWA_M2M_SDE_MAX_IN_BUFFER              (2U)

/** Maximim number of output SDE buffers */
#define VHWA_M2M_SDE_MAX_OUT_BUFFER             (1U)

/** Maximim number of total SDE buffers */
#define VHWA_M2M_SDE_MAX_BUFFER            (VHWA_M2M_SDE_MAX_IN_BUFFER + \
                                            VHWA_M2M_SDE_MAX_OUT_BUFFER)

/**< Instance ID for the SDE M2m driver */
#define VHWA_M2M_SDE_DRV_INST_ID            (0U)

/** \brief Max Number of handles supported by SDE M2M Driver */
#define VHWA_M2M_SDE_MAX_HANDLES            (4u)

/**
 *  \anchor Vhwa_SdeIOIndex
 *  \name   VHWA SDE IO buffer indexes
 *  \brief  Input/Output buffer MACRO's for SDE driver
 *
 *  @{
 */
#define SDE_INPUT_REFERENCE_IMG     (0U)
#define SDE_INPUT_BASE_IMG          (1U)
#define SDE_OUTPUT                  (2U)
/* Below two buffer will be used Driver internally and are not supposed to be
   used by application */
#define SDE_INPUT_FOCO_REF_IMG      (3U)
#define SDE_INPUT_FOCO_BASE_IMG     (4U)
/** @} */

/**
 *  \anchor Vhwa_SdeDefaultInitPrms
 *  \name   VHWA SDE Default Init Paramters
 *  \brief  VHWA SDE Default Init Paramters for Sl2
 *
 *  @{
 */
#define SDE_DEFAULT_IMG_WIDTH       (1280U)
#define SDE_DEFAULT_CCSF            (FVID2_CCSF_BITS12_UNPACKED16)
#define SDE_DEFAULT_DIS_BUFF_DEPTH  (2U)
#define SDE_DEFAULT_SR              (SDE_SR_128)
/** @} */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct Vhwa_M2mSdePrms
 *  \brief Connfiguration parameters of SDE.
 */
typedef struct
{
    Fvid2_Format        inOutImgFmt[VHWA_M2M_SDE_MAX_BUFFER];
    /**< Input Frame format, describing input frames storage format
         Following parametes are used from this
         pitch = line offset for the input data, used based on the data format
         dataFormat = Input DataFormat, Only used for Curr
         sfcc = Storage format */
    Sde_Config          sdeCfg;
    /**< SDE configuration */

    Vhwa_FocoPrms       focoPrms;
    /**< SDE FOCO Parameters */
}Vhwa_M2mSdePrms;

/**
 *  struct Vhwa_M2mSdeCreateArgs
 *  \brief Parameters required to create a new handle
 */
typedef struct
{
    Vhwa_GetTimeStamp  getTimeStamp;
    /**< Function pointer to get timestamp */

    uint32_t enablePsa;
    /**< Flag to enable/Disble SDE PSA Module */
}Vhwa_M2mSdeCreateArgs;

/**
 *  struct Vhwa_M2mSdeInitParams
 *  \brief Initialization Parameters for SDE M2M Driver
 */
typedef struct
{
    Udma_DrvHandle     udmaDrvHndl;
    /**< UDMA driver handle */
    Vhwa_IrqInfo       irqInfo;
    /**< IRQ Information */
}Vhwa_M2mSdeInitParams;

/**
 *  struct Vhwa_m2mSdeSl2AllocPrms
 *  \brief Init Parameters required to allocate SDE driver
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
    uint32_t        searchRange;
    /**< Search range refer to \ref SDE_DispSearchRange */
    uint32_t        disBuffDepth;
    /**< Maximum disparity buffer depth */
} Vhwa_M2mSdeSl2AllocPrms;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief Vhwa_M2mSdeInitParams structure init function.
 *
 *  \param  sdeInitPrms  Pointer to #Vhwa_M2mSdeInitParams structure.
 *
 *  \return None
 */
static inline void Vhwa_M2mSdeInitParamsInit(
                                Vhwa_M2mSdeInitParams *sdeInitPrms);

/**
 *  \brief Function to initialize the SDE configuration parameters to default
 *
 *  \param sdePrms   Pointer to a #Vhwa_M2mSdePrms containing SDE configuration
 *
 *  \return         None
 */
static inline void Vhwa_M2mSdePrmsInit(Vhwa_M2mSdePrms *sdePrms);

/**
 *  \brief Function to initialize the SDE create argument parameters to default
 *
 *  \param createArgs   Pointer to a #Vhwa_M2mSdeCreateArgs containing
 *                      SDE create arguments
 *
 *  \return             None
 */
static inline void Vhwa_M2mSdeCreateArgsInit(Vhwa_M2mSdeCreateArgs *createArgs);

/**
 *  \brief SDE initialization function.
 *   This function initializes the SDE hardware and drivers.
 *   This function should be called before calling any driver APIs and
 *   only once.
 *
 *  \param pInitPrms   Pointer to a #Vhwa_M2mSdeInitParams structure
 *                     containing the SDE driver initialization parameters
 *
 *  \return FVID2_SOK if successful, else suitable error code
 */
int32_t Vhwa_m2mSdeInit(Vhwa_M2mSdeInitParams *pInitPrms);

/**
 *  \brief SDE de-initialization function.
 *   This function un-initializes the SDE hardware and drivers.
 *   This function should be called during system shutdown if
 *   Vhwa_m2mSdeDeInit() was called by the application.
 *
 *  \return None
 */
void Vhwa_m2mSdeDeInit(void);

/**
 *  \brief Function to allocate Sl2 memory for input and output buffers.
 *         Both DOF and SDE SL2 memory should be freed before reallocating.
 *
 *  \param sl2allocPrms Pointer to a #Vhwa_M2mSdeSl2AllocPrms structure
 *                      containing the SL2 allocation parameters
 *
 *  \return FVID2_SOK if successful, else suitable error code
 */
int32_t Vhwa_m2mSdeAllocSl2(const Vhwa_M2mSdeSl2AllocPrms *sl2allocPrms);

/**
 *  \brief Function to free allocated SL2.
 *
 *  \return FVID2_SOK if successful, else suitable error code
 */
int32_t Vhwa_m2mSdeFreeSl2(void);

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

static inline void Vhwa_M2mSdeInitParamsInit(
                                Vhwa_M2mSdeInitParams *sdeInitPrms)
{
    if (NULL != sdeInitPrms)
    {
        Sde_getIrqInfo(&sdeInitPrms->irqInfo);
        sdeInitPrms->udmaDrvHndl = NULL;
    }
}

static inline void Vhwa_M2mSdePrmsInit(Vhwa_M2mSdePrms *sdePrms)
{
    if (NULL != sdePrms)
    {
        (void)memset(sdePrms, 0, sizeof(Vhwa_M2mSdePrms));
    }
}

static inline void Vhwa_M2mSdeCreateArgsInit(Vhwa_M2mSdeCreateArgs *createArgs)
{
    if (NULL != createArgs)
    {
        createArgs->getTimeStamp = NULL;
        createArgs->enablePsa = 0;
    }
}

static inline void Vhwa_M2mSdeSl2AllocPrmsInit(Vhwa_M2mSdeSl2AllocPrms *prms)
{
    if(NULL != prms)
    {
        prms->maxImgWidth  = SDE_DEFAULT_IMG_WIDTH;
        prms->inCcsf       = SDE_DEFAULT_CCSF;
        prms->searchRange  = SDE_DEFAULT_SR;
        prms->disBuffDepth = SDE_DEFAULT_DIS_BUFF_DEPTH;
    }
}

#ifdef __cplusplus
}
#endif

#endif /* VHWA_M2M_SDE_H_ */

/* @} */
