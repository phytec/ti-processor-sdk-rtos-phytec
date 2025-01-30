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
 *  \defgroup DRV_LDC_MODULE LDC Module
 *            LDC Module
 *
 */
/**
 *  \ingroup  DRV_LDC_MODULE
 *  \defgroup DRV_LDC_MODULE_INTERFACE LDC Interface
 *            Interface file for LDC M2M FVID2 driver
 *
 *  @{
 */
/**
 *  \file vhwa_m2mLdc.h
 *
 *  \brief  Defines the structures / control operations that could be used to
 *              configure / control LDC M2M driver
 *
 *          Typical Application for the LDC M2M driver is as shown below.
 *
 *          Vhwa_m2mLdcInit
 *               ||
 *          Vhwa_m2mLdcAllocSl2
 *               ||
 *          FVID2_Create
 *               ||
 *          IOCTL_VHWA_M2M_LDC_SET_CONFIG (sets LDC configuration)
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


#ifndef VHWA_M2M_LDC_H_
#define VHWA_M2M_LDC_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/drv/vhwa/include/ldc_cfg.h>
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
 *  \anchor VhwaM2mLdc_InstanceId
 *  \name VHWA LDC Instance ID 
 *
 *  VHWA VPAC LDC Instance ID's
 *
 *  @{
 */
#if defined (SOC_J784S4)
    /** \brief VPAC 0 LDC Instance 0  */
    #define VHWA_M2M_VPAC_0_LDC_DRV_INST_ID_0             (0U)
    /** \brief VPAC 1 LDC Instance 0  */
    #define VHWA_M2M_VPAC_1_LDC_DRV_INST_ID_0             (1U)
    /** \brief Total number of LDC instances */
    #define VHWA_M2M_LDC_DRV_NUM_INST                     (VHWA_M2M_VPAC_1_LDC_DRV_INST_ID_0 + 1U)
#else
    /** \brief VPAC 0 LDC Instance 0  */
    #define VHWA_M2M_VPAC_0_LDC_DRV_INST_ID_0             (0U)
    /** \brief Total number of LDC instances */
    #define VHWA_M2M_LDC_DRV_NUM_INST                     (VHWA_M2M_VPAC_0_LDC_DRV_INST_ID_0 + 1U)
#endif
/* @} */

/**< Default Instance ID for the LDC M2m driver [For backward compatibility] */
#define VHWA_M2M_LDC_DRV_INST_ID            (VHWA_M2M_VPAC_0_LDC_DRV_INST_ID_0)

/** \brief Max Number of handles supported for each instance of LDC M2M Driver */
#define VHWA_M2M_LDC_MAX_HANDLES            (8u)

/**
 *  \anchor VhwaM2MLdc_Ioctls
 *  \name   Ioctls for the LDC memory to memory driver
 *  \brief  Input/Output control MACRO's for LDC memory to memory module
 *
 *  @{
 */
/**
 * \brief IOCTL for setting LDC configuration.
 *        This IOCTL takes LDC Configuration #Ldc_Config as input
 *
 * \return  FVID2_SOK on success, else error code.
 */
#define IOCTL_VHWA_M2M_LDC_SET_PARAMS       (VHWA_IOCTL_M2M_LDC_IOCTL_BASE)

/**
 * \brief IOCTL for getting module's performance numbers
 *        for the last frame submitted.
 *
 * \return  FVID2_SOK on success, else error code.
 */
#define IOCTL_VHWA_M2M_LDC_GET_PERFORMANCE  (IOCTL_VHWA_M2M_LDC_SET_PARAMS + 1U)

/**
 * \brief IOCTL for getting LSE PSA Signature
 *
 * \return  FVID2_SOK on success, else error code.
 */
#define IOCTL_VHWA_M2M_LDC_GET_PSA_SIGN     (                                  \
    IOCTL_VHWA_M2M_LDC_GET_PERFORMANCE + 1U)

/**
 * \brief IOCTL for enabling error events and
 *        registering callbacks for the same.
 *        This IOCTL pointer to #Ldc_ErrEventParams as input
 *
 * \return  FVID2_SOK on success, else error code.
 */
#define IOCTL_VHWA_M2M_LDC_REGISTER_ERR_CB  (IOCTL_VHWA_M2M_LDC_GET_PSA_SIGN + \
    1U)

/**
 * \brief IOCTL for enabling and setting parameters for
 *        LDC Read bandwidth limiter. This ioctl sets the configurations
 *        directly in the registers, so it gets used for all open
 *        handles. This IOCTL takes pointer to #Ldc_RdBwLimitConfig as input
 *
 * \return  FVID2_SOK on success, else error code.
 */
#define IOCTL_VHWA_M2M_LDC_SET_RD_BW_LIMIT  (                                  \
    IOCTL_VHWA_M2M_LDC_REGISTER_ERR_CB + 1U)

/**
 * \brief IOCTL for enabling and setting HTS limiter.
 *        This is used to slowed down the HTS by introducing clock cycles
 *        between internal start signals.
 *        This IOCTL takes pointer to #Vhwa_HtsLimiter as input
 *
 * \return  FVID2_SOK on success, else error code.
 */
#define IOCTL_VHWA_M2M_LDC_SET_HTS_LIMIT    (                                  \
    IOCTL_VHWA_M2M_LDC_SET_RD_BW_LIMIT + 1U)

/**
 * \brief IOCTL to sync start each module
 *        This IOCTL doesn't configure any register and only enable pipeline
 *        to start processing
 *
 * \return  FVID2_SOK on success, else error code.
 */
#define IOCTL_VHWA_M2M_LDC_SYNC_START    (                                  \
                                    IOCTL_VHWA_M2M_LDC_SET_HTS_LIMIT + 1U)
/** @} */


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

typedef struct
{
    Vhwa_GetTimeStamp  getTimeStamp;
    /**< Function pointer to get timestamp */

    uint32_t                    enablePsa;
    /**< Flag to enable/Disble LSE PSA Module */
} Vhwa_M2mLdcCreateArgs;

/**
 *  struct Vhwa_M2mLdcInitParams
 *  \brief Init Parameters for LDC M2M Driver
 */
typedef struct
{
    Udma_DrvHandle              udmaDrvHndl;
    /**< UDMA driver handle, used for configuring UTC */
    Vhwa_IrqInfo                irqInfo;
    /**< IRQ Information */
} Vhwa_M2mLdcInitParams;

/**
 *  struct Vhwa_m2mLdcSl2AllocPrms
 *  \brief Used for allocating SL2 memory
 */
typedef struct
{
    /** These parameters are used by the driver to allocate memory, used
     *  by all handles
     *  Ensure to allocate max of all handles requirement.
     */
    uint32_t                    enableOutput[LDC_MAX_OUTPUT];
    /**< Flag to indicate if the output is enabled,
     *   Memory is allocated for this output only if it is enabled,
     *   If memory is not allocated for a given output, it cannot be enabled
     *   later using IOCTL_VHWA_M2M_LDC_SET_CONFIG ioctl */
    uint32_t                    outCcsf[LDC_MAX_OUTPUT];
    /**< Output storage format for both the outputs */
    uint32_t                    maxNumBlocks[LDC_MAX_OUTPUT];
    /**< Maximum number of block to be allocated in SL2 memory,
         Minimum 2 blocks are required for each output */
    uint32_t                    maxBlockWidth;
    /**< Maximum output block width for both the outputs,
     *   Block size is same for all outputs */
    uint32_t                    maxBlockHeight;
    /**< Maximum output block height for both the outputs,
     *   Block size is same for all outputs */
} Vhwa_M2mLdcSl2AllocPrms;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief Initializes LDC Init Parameters
 *
 *  \param ldcInitPrms      LDC Init Parameters
 *                          This parameter should not be 0.
 *  \return                 Returns 0 on success else returns error value
 */
int32_t Vhwa_m2mLdcInit(Vhwa_M2mLdcInitParams *ldcInitPrms);

/**
 *  \brief DeInitializes LDC Init Parameters
 *
 */
void Vhwa_m2mLdcDeInit(void);

/**
 *  \brief Function to allocate Sl2 memory for the output buffers.
 *
 *  \param sl2allocPrms Pointer to a #Vhwa_M2mLdcSl2AllocPrms structure
 *                      containing the SL2 allocation parameters
 *
 *  \return FVID2_SOK if successful, else suitable error code
 */
int32_t Vhwa_m2mLdcAllocSl2(const Vhwa_M2mLdcSl2AllocPrms *sl2allocPrms);

/**
 *  \brief Function to free allocated SL2.
 *
 */
void Vhwa_m2mLdcFreeSl2(void);

/**
 *  VhwaM2mLdcCfg_Init
 *  \brief This function should be used to initialize variable of type
 *          #Vhwa_M2mLdcInitParams.
 *
 *  \param ldcCfg   A pointer of type Vhwa_M2mLdcInitParams
 *  \return         None
 */
static inline void Vhwa_m2mLdcInitParamsInit(Vhwa_M2mLdcInitParams *ldcCfg);

/**
 *  Vhwa_M2mLdcCreateArgsInit
 *  \brief This function should be used to initialize variable of type
 *          #Vhwa_M2mLdcCreateArgs.
 *
 *  \param createArgs   A pointer of type Vhwa_M2mLdcCreateArgs
 *  \return             None
 */
static inline void Vhwa_M2mLdcCreateArgsInit(Vhwa_M2mLdcCreateArgs *createArgs);

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

static inline void Vhwa_m2mLdcInitParamsInit(Vhwa_M2mLdcInitParams *initPrms)
{
    if (NULL != initPrms)
    {
        Ldc_getIrqInfo(&initPrms->irqInfo);
        initPrms->udmaDrvHndl = NULL;
    }
}


static inline void Vhwa_M2mLdcSl2AllocPrmsInit(Vhwa_M2mLdcSl2AllocPrms *sl2Prms)
{
    uint32_t cnt;

    if (NULL != sl2Prms)
    {
        for (cnt = 0u; cnt < LDC_MAX_OUTPUT; cnt ++)
        {
            if (0u == cnt)
            {
                sl2Prms->enableOutput[cnt] = (uint32_t)UTRUE;
                sl2Prms->outCcsf[cnt] = FVID2_CCSF_BITS16_PACKED;
            }
            else
            {
                sl2Prms->enableOutput[cnt] = (uint32_t)UTRUE;
                sl2Prms->outCcsf[cnt] = FVID2_CCSF_BITS8_PACKED;
            }
            sl2Prms->maxNumBlocks[cnt] = 2U;
        }
        sl2Prms->maxBlockWidth = 64U;
        sl2Prms->maxBlockHeight = 64U;
    }
}

static inline void Vhwa_M2mLdcCreateArgsInit(Vhwa_M2mLdcCreateArgs *createArgs)
{
    if (NULL != createArgs)
    {
        createArgs->getTimeStamp = NULL;
        createArgs->enablePsa = (uint32_t)UFALSE;
    }
}

#ifdef __cplusplus
}
#endif

#endif /* VHWA_M2M_LDC_H_ */

/** @} */
