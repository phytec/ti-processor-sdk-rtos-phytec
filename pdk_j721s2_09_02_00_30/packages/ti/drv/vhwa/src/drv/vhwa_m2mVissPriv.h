/* =============================================================================
 *   Copyright (c) Texas Instruments Incorporated 2019
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
 *  \file vhwa_m2mVissPriv.h
 *
 *  \brief VISS Private header file
 *
 */

#ifndef VHWA_M2M_VISS_PRIV_H_
#define VHWA_M2M_VISS_PRIV_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/osal/osal.h>

#include <ti/csl/csl_fvid2_dataTypes.h>
#include <ti/drv/fvid2/include/fvid2_drvMgr.h>

#include <ti/drv/vhwa/src/csl/include/csl_fcp.h>
#include <ti/drv/vhwa/src/csl/include/csl_rawfe.h>
#include <ti/drv/vhwa/src/csl/include/csl_glbce.h>
#include <ti/drv/vhwa/src/csl/include/csl_nsf4.h>
#include <ti/drv/vhwa/src/csl/include/csl_h3a.h>
#include <ti/drv/vhwa/src/csl/include/csl_hts.h>
#include <ti/drv/vhwa/src/csl/include/csl_lse.h>
#include <ti/drv/vhwa/src/csl/include/csl_glbce.h>
#include <ti/drv/vhwa/src/csl/include/csl_ee.h>
#include <ti/drv/vhwa/src/csl/include/csl_cac.h>
#include <ti/drv/vhwa/src/csl/include/csl_vpactop.h>
#include <ti/drv/vhwa/src/csl/include/csl_visstop.h>

#include <ti/drv/vhwa/include/vhwa_m2mViss.h>

#include <ti/drv/vhwa/src/drv/vhwa_cfg.h>
#include <ti/drv/vhwa/src/drv/vhwa_utils.h>

#include <ti/drv/vhwa/soc/vhwa_soc.h>

#include <vhwa_vpac_priv.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
/** \brief Maximum loop count - used while dequeuing UDMA */
#define VHWA_M2M_VISS_MAX_WAIT_LOOP_CNT         (2000u)

/**
 *  \brief Number of ring entries - we can prime this much memcpy operations
 */
#define VHWA_M2M_VISS_UDMA_RING_ENTRIES         (16u)

/**
 *  \brief Size (in bytes) of each ring entry (Size of pointer - 64-bit)
 */
#define VHWA_M2M_VISS_UDMA_RING_ENTRY_SIZE      (sizeof(uint64_t))

/**
 *  \brief Total ring memory
 */
#define VHWA_M2M_VISS_UDMA_RING_MEM_SIZE        (                              \
    VHWA_M2M_VISS_UDMA_RING_ENTRIES * VHWA_M2M_VISS_UDMA_RING_ENTRY_SIZE)

/**
 *  \brief Total Transfer Record Descriptors
 */
#define VHWA_M2M_VISS_UDMA_NUM_TR_DESC          (1U)

/**
 *  \brief Total Config Transfer Record Descriptors
 */
#define VHWA_M2M_VISS_UDMA_CONFIG_NUM_TR_DESC   (uint32_t)(BUFF_ID_MAXBUFID)


/**
 *  \brief UDMA TR packet descriptor memory.
 *  This contains the CSL_UdmapCppi5TRPD + Padding to sizeof(CSL_UdmapTR15) +
 *  one Type_15 TR (CSL_UdmapTR15) + one TR response of 4 bytes.
 *  Since CSL_UdmapCppi5TRPD is less than CSL_UdmapTR15, size is just two times
 *  CSL_UdmapTR15 for alignment.
 */
#define VHWA_M2M_VISS_UDMA_TRPD_SIZE            (sizeof(CSL_UdmapTR15) +       \
    (sizeof(CSL_UdmapTR15) * VHWA_M2M_VISS_UDMA_NUM_TR_DESC) +                 \
    (VHWA_M2M_VISS_UDMA_NUM_TR_DESC * 4U) +                                    \
    (UDMA_CACHELINE_ALIGNMENT - (VHWA_M2M_VISS_UDMA_NUM_TR_DESC * 4U)))

/**
 *  \brief UDMA TR packet descriptor memory.
 *  This contains the CSL_UdmapCppi5TRPD + Padding to sizeof(CSL_UdmapTR15) +
 *  one Type_15 TR (CSL_UdmapTR15) + one TR response of 4 bytes.
 *  Since CSL_UdmapCppi5TRPD is less than CSL_UdmapTR15, size is just two times
 *  CSL_UdmapTR15 for alignment.
 */
#define VHWA_M2M_VISS_UDMA_CONFIG_TRPD_SIZE      (sizeof(CSL_UdmapTR15) +        \
    (sizeof(CSL_UdmapTR15) * VHWA_M2M_VISS_UDMA_CONFIG_NUM_TR_DESC) +            \
    (VHWA_M2M_VISS_UDMA_CONFIG_NUM_TR_DESC * 4U) +                               \
    (UDMA_CACHELINE_ALIGNMENT - (VHWA_M2M_VISS_UDMA_CONFIG_NUM_TR_DESC * 4U)))

/**
 *  \brief Minimum input lines required in SL2
 */
#define VHWA_M2M_VISS_MIN_INPUT_DEPTH           (3u)

/**
 *  \brief Minimum output lines required in SL2
 */
#define VHWA_M2M_VISS_MIN_OUTPUT_DEPTH          (2u)

/**<
 *  \brief Maximum number of input DMA channels
 */
#define VHWA_M2M_VISS_MAX_IN_DMA_CH             (VHWA_M2M_VISS_MAX_INPUTS)

/**<
 *  \brief Maximum number of output DMA channels
 */
#define VHWA_M2M_VISS_MAX_OUT_DMA_CH            (VHWA_M2M_VISS_MAX_OUTPUTS)

/**<
 *  \brief Maximum number of DMA channels
 */
#define VHWA_M2M_VISS_MAX_DMA_CH                (VHWA_M2M_VISS_MAX_IN_DMA_CH + \
    VHWA_M2M_VISS_MAX_OUT_DMA_CH)

/**<
 *  \brief Maximum number of DMA channels
 */
#define VHWA_M2M_VISS_MAX_CONFIG_DMA_CH			(1U)

#define VHWA_M2M_VISS_LUMA_MODE                 (1U)
#define VHWA_M2M_VISS_CHROMA_MODE               (2U)

#define VHWA_M2M_VISS_RED_COLOR_MODE            (1U)
#define VHWA_M2M_VISS_GREEN_COLOR_MODE          (2U)
#define VHWA_M2M_VISS_BLUE_COLOR_MODE           (4U)

#define VHWA_M2M_VISS_GLBCE_TIMEOUT_ITER        (1000)

/**
 * \brief IOCTL for updating VISS SL2 config using Flex-Connect
 *        This IOCTL require pointer to structure Vhwa_M2mVissFcConPrms
 * \return  FVID2_SOK on success, else error code.
 */
#define IOCTL_VHWA_VISS_FC_CONN_PARAMS   (VHWA_IOCTL_FC_VISS_IOCTL_BASE)

/**
 * \brief IOCTL for updating VISS SL2 config using Flex-Connect
 *        This IOCTL require pointer to structure Vhwa_M2mVissFcUpdatePrms
 * \return  FVID2_SOK on success, else error code.
 */
#define IOCTL_VHWA_VISS_FC_UPDATE_PARAMS   (IOCTL_VHWA_VISS_FC_CONN_PARAMS+1U)

/**
 * \brief IOCTL for getting the VISS SL2 parameters
 *        This IOCTL require pointer to structure Vhwa_M2mVissSl2Prms
 * \return  FVID2_SOK on success, else error code.
 */
#define IOCTL_VHWA_VISS_GET_SL2_PARAMS      (IOCTL_VHWA_VISS_FC_UPDATE_PARAMS+1U)

/**<
 *  \brief Maximum Horizontal blanking required for VISS; Referred from VISS_Top_Specification_1.85, 2.2.12 Blanking Requirements
 */
#define VHWA_VISS_MAX_HORZ_BLANKING              (35u)


#if (VHWA_VISS_UTC_CH_END < VHWA_M2M_VISS_MAX_DMA_CH)
#error "VISS: Number of UTC Ch are less than required by VISS"
#endif

/**
 * HTS HWA0 register size, in word size.
 */
#define BUFF_OBJ_HTS_HWA0_REG_SIZE                                        \
    (((CSL_HTS_HWA0_PROD6_COUNT - CSL_HTS_HWA0_SCHEDULER_CONTROL) / 4U) + 1U)

/**
 * HTS DMA 0 to 4 register size, in word size.
 */
#define BUFF_OBJ_HTS_DMA_0_4_REG_SIZE                                     \
    (((CSL_HTS_DMA4_PROD0_COUNT - CSL_HTS_DMA0_SCHEDULER_CONTROL) / 4U) + 1U)

/**
 * HTS DMA 240 to 245 register siz, in word size.
 */
#define BUFF_OBJ_HTS_DMA240_245_REG_SIZE                                        \
    (((CSL_HTS_DMA245_CONS0_CONTROL - CSL_HTS_DMA240_SCHEDULER_CONTROL) / 4U) + 1U)

/**
 * decompLut1Data size in word size.
 */
#define	BUFF_OBJ_DECOMP_LUT1_SIZE			((RFE_COMP_DECOMP_LUT_SIZE / 2U) + 1U)

/**
 * decompLut2Data size in word size.
 */
#define	BUFF_OBJ_DECOMP_LUT2_SIZE			((RFE_COMP_DECOMP_LUT_SIZE / 2U) + 1U)

/**
 * decompLut3Data size in word size.
 */
#define	BUFF_OBJ_DECOMP_LUT3_SIZE			((RFE_COMP_DECOMP_LUT_SIZE / 2U) + 1U)

/**
 * compLutData size in word size.
 */
#define	BUFF_OBJ_COMP_LUT_SIZE				((RFE_COMP_DECOMP_LUT_SIZE / 2U) + 1U)

/**
 * h3aLutData size in word size.
 */
#define	BUFF_OBJ_H3A_LUT_SIZE				((RFE_H3A_COMP_LUT_SIZE / 2U) + 1U)

/**
 * gammaLut1Data size in word size.
 */
#define	BUFF_GAMMA_LUT1_SIZE				((FCP_GAMMA_LUT_SIZE / 2U) + 1U)

/**
 * gammaLut2Data size in word size.
 */
#define	BUFF_GAMMA_LUT2_SIZE				((FCP_GAMMA_LUT_SIZE / 2U) + 1U)

/**
 * gammaLut3Data size in word size.
 */
#define	BUFF_GAMMA_LUT3_SIZE				((FCP_GAMMA_LUT_SIZE / 2U) + 1U)

/**
 * y8C8LutData size in word size.
 */
#define	BUFF_Y8C8_LUT_SIZE					((FCP_RGB_YUVSAT_LUT_SIZE / 2U) + 1U)

/**
 * c8G8LutData size in word size.
 */
#define	BUFF_C8G8_LUT_SIZE					((FCP_RGB_YUVSAT_LUT_SIZE / 2U) + 1U)

/**
 * s8B8LutData size in word size.
 */
#define	BUFF_S8B8_LUT_SIZE					((FCP_RGB_YUVSAT_LUT_SIZE / 2U) + 1U)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

typedef struct
{
    /* Parameters used to Update every conenction */
    uint32_t                outputIdx;
    /**< Output channel Index */
    uint64_t                sl2Addr;
    uint32_t                sl2Depth;
    uint32_t                sl2Pitch;
    uint32_t                ctrlIdx;
    /**< VISS Producer Control index */

    /* HTS Specific parameters */
    uint32_t                htsConsId;
    uint32_t                htsThreshold;
    uint32_t                htsDmaProdPreLoad;
    uint32_t                htsDmaProdPostLoad;
    uint32_t                htsDmaProdCntDec;
    uint32_t                enableMaskSel;
    uint32_t                maskSel;
} Vhwa_M2mVissFcConPrms;


typedef struct
{
    /* Parameters common across all connection paths */
    /* Only 5 output needs to be updated */
    uint32_t                dmaConsEnable[VHWA_M2M_VISS_MAX_OUTPUTS];

    /* DMA specific parameters */
    uint32_t                isFlexConnect;
} Vhwa_M2mVissFcUpdatePrms;


typedef struct
{
    uint32_t                isFlexConnect;
    uint32_t                outDmaEnable[VHWA_M2M_VISS_MAX_OUT_DMA_CH];
} Vhwa_M2mVissFcStatus;

typedef struct
{
    uint32_t                sl2MemSize;
    /**< SL2 Memory Size allocated to MSC driver */

    uint64_t                inSl2Addr[VHWA_M2M_VISS_MAX_IN_DMA_CH];
    /**< SL2 address for each input buffer */
    uint32_t                inWidthInBytes[VHWA_M2M_VISS_MAX_IN_DMA_CH];
    /**< SL2 input buffer width in bytes */
    uint32_t                inSl2BuffDepth[VHWA_M2M_VISS_MAX_IN_DMA_CH];
    /**< SL2 buffer depth for each input buffer */

    uint64_t                outSl2Addr[VHWA_M2M_VISS_MAX_OUT_DMA_CH];
    /**< SL2 address for each output buffer */
    uint32_t                outWidthInBytes[VHWA_M2M_VISS_MAX_OUT_DMA_CH];
    /**< SL2 output buffer width in bytes */
    uint32_t                outSl2BuffDepth[VHWA_M2M_VISS_MAX_OUT_DMA_CH];
    /**< SL2 buffer depth for each input buffer */
}Vhwa_M2mVissSl2Prms;

typedef struct
{
    CSL_viss_topRegs            *topRegs;
    /**< VISS Top Register Address */
    CSL_lseRegs                 *lseRegs;
    /**< VISS LSE Register Address */
    CSL_htsRegs                 *htsRegs;
    /**< HTS Register Address */

    /* RAW FE modules */
    CSL_rawfe_cfgRegs           *rfeRegs;
    /**< RFE Register Address */
    uint32_t                    *dpcLutAddr;
    /**< DPC Lut Address */
    uint32_t                    *lscLutAddr;
    /**< LSC Lut Address */
    uint32_t                    *decompLut1Addr;
    /**< Decompanding LUT address for the Long Exposure input */
    uint32_t                    *decompLut2Addr;
    /**< Decompanding LUT address for the Short Exposure input */
    uint32_t                    *decompLut3Addr;
    /**< Decompanding LUT address for the Very Short Exposure input */
    uint32_t                    *compLutAddr;
    /**< Companding LUT Address */
    uint32_t                    *h3aLutAddr;
    /**< H3A Lut Address */
    CSL_rawfe_h3a_cfgRegs       *h3aRegs;
    /**< VISS H3A Register Address */

    CSL_glbceRegs               *glbceRegs;
    /**< VISS GLBCE Register Address */
    CSL_glbce_statmemRegs       *glbceStatsMem;
    /**< VISS GLBCE Statics Memory Address */
    CSL_nsf4vRegs               *nsf4Regs;
    /**< NSF4 Register Address */
    CSL_flexcfaRegs             *cfaRegs;
    /**< CFA Register Address */

    /* Flex CC Modules */
    CSL_flexcc_cfgRegs          *fcpRegs;
    /**< FCP Register Address */
    uint32_t                    *gammaLut1Addr;
    /**< Gamma/Contrast Lut1 Address */
    uint32_t                    *gammaLut2Addr;
    /**< Gamma/Contrast Lut2 Address */
    uint32_t                    *gammaLut3Addr;
    /**< Gamma/Contrast Lut3 Address */
    uint32_t                    *y8C8LutAddr;
    /**< RGB Lut Address */
    uint32_t                    *c8G8LutAddr;
    /**< RGB Lut Address */
    uint32_t                    *s8B8LutAddr;
    /**< RGB Lut Address */

    CSL_flexeeRegs              *eeRegs;
    /**< Edge Enhancer Register address */

    CSL_flexcc_histRegs         *histRegs;
    /**< Histogram data Register address */

#if defined VHWA_VPAC_IP_REV_VPAC3
    CSL_rawhist_lutRegs         *nsf4LutRegs;
    /**< NSF4 Histgram Lut Register Address */
    CSL_rawhist_dataRegs        *nsf4HistRegs;
    /**< NSF4 Histgram memory Register Address */
    CSL_flexcfa_dlutsRegs       *cfaDLutRegs;
    /**< CFA Decompanding Lut Register Address */
    CSL_cacRegs                 *cacRegs;
    /**< CAC Register Address */
    CSL_cac_lutRegs             *cacLutRegs;
    /**< CAC Register Address */

    /* FCP 2 registers */
    CSL_flexcfa_dlutsRegs       *fcp2CfaDLutRegs;
    /**< CFA Decompanding Lut Register Address */
    CSL_flexcfaRegs             *fcp2CfaRegs;
    /**< CFA Register Address */

    /* Flex CC Modules */
    CSL_flexcc_cfgRegs          *fcp2Regs;
    /**< FCP Register Address */
    uint32_t                    *fcp2GammaLut1Addr;
    /**< Gamma/Contrast Lut1 Address */
    uint32_t                    *fcp2GammaLut2Addr;
    /**< Gamma/Contrast Lut2 Address */
    uint32_t                    *fcp2GammaLut3Addr;
    /**< Gamma/Contrast Lut3 Address */
    uint32_t                    *fcp2Y8C8LutAddr;
    /**< RGB Lut Address */
    uint32_t                    *fcp2C8G8LutAddr;
    /**< RGB Lut Address */
    uint32_t                    *fcp2S8B8LutAddr;
    /**< RGB Lut Address */
    CSL_flexeeRegs              *fcp2EeRegs;
    /**< Edge Enhancer Register address */
    CSL_flexcc_histRegs         *fcp2HistRegs;
    /**< Histogram data Register address */
#endif
} Vhwa_M2mVissRegAddr;

/**
 * This structure is used to store the initial register data.
 */
typedef struct
{
    CSL_viss_topRegs            topRegs;
    /**< VISS Top Register Data */
    CSL_lseRegs                 lseRegs;
    /**< VISS LSE Register Data */
    CSL_htsRegs                 htsRegs;
    /**< HTS Register Data */

    /* RAW FE modules */
    CSL_rawfe_cfgRegs           rfeRegs;
    /**< RFE Register Data */
    uint32_t                    dpcLutData[RFE_DPC_LUT_SIZE];
    /**< DPC Lut Data */
    uint32_t                    lscLutData[RFE_LSC_TBL_SIZE];
    /**< LSC Lut Data */
    uint32_t                    decompLut1Data[BUFF_OBJ_DECOMP_LUT1_SIZE];
    /**< Decompanding LUT Data for the Long Exposure input */
    uint32_t                    decompLut2Data[BUFF_OBJ_DECOMP_LUT2_SIZE];
    /**< Decompanding LUT Data for the Short Exposure input */
    uint32_t                    decompLut3Data[BUFF_OBJ_DECOMP_LUT3_SIZE];
    /**< Decompanding LUT Data for the Very Short Exposure input */
    uint32_t                    compLutData[BUFF_OBJ_COMP_LUT_SIZE];
    /**< Companding LUT Data */
    uint32_t                    h3aLutData[BUFF_OBJ_H3A_LUT_SIZE];
    /**< H3A Lut Address */
    CSL_rawfe_h3a_cfgRegs       h3aRegs;
    /**< VISS H3A Register Data */

    CSL_glbceRegs               glbceRegs;
    /**< VISS GLBCE Register Data */
    CSL_glbce_statmemRegs       glbceStatsMem;
    /**< VISS GLBCE Statics Memory Data */
    CSL_nsf4vRegs               nsf4Regs;
    /**< NSF4 Register Data */

    CSL_flexcfaRegs             cfaRegs;
    /**< CFA Register Data */

    /* Flex CC Modules */
    CSL_flexcc_cfgRegs          fcpRegs;
    /**< FCP Register Data */
    uint32_t                    gammaLut1Data[BUFF_GAMMA_LUT1_SIZE];
    /**< Gamma/Contrast Lut1 Data */
    uint32_t                    gammaLut2Data[BUFF_GAMMA_LUT2_SIZE];
    /**< Gamma/Contrast Lut2 Data */
    uint32_t                    gammaLut3Data[BUFF_GAMMA_LUT3_SIZE];
    /**< Gamma/Contrast Lut3 Data */
    uint32_t                    y8C8LutData[BUFF_Y8C8_LUT_SIZE];
    /**< RGB Lut Data */
    uint32_t                    c8G8LutData[BUFF_C8G8_LUT_SIZE];
    /**< RGB Lut Data */
    uint32_t                    s8B8LutData[BUFF_S8B8_LUT_SIZE];
    /**< RGB Lut Data */

    CSL_flexeeRegs              eeRegs;
    /**< Edge Enhancer Register Data */

    CSL_flexcc_histRegs         histRegs;
    /**< Histogram data Register Data */

#if defined VHWA_VPAC_IP_REV_VPAC3
    CSL_rawhist_lutRegs         nsf4LutRegs;
    /**< NSF4 Histgram Lut Register Address */
    CSL_rawhist_dataRegs        nsf4HistRegs;
    /**< NSF4 Histgram memory Register Address */
    CSL_flexcfa_dlutsRegs       cfaDLutRegs;
    /**< CFA Decompanding Lut Register Address */
    CSL_cacRegs                 cacRegs;
    /**< CAC Register Address */
    CSL_cac_lutRegs             cacLutRegs;
    /**< CAC Register Address */

    /* FCP 2 registers */
    CSL_flexcfa_dlutsRegs       fcp2CfaDLutRegs;
    /**< CFA Decompanding Lut Register Address */
    CSL_flexcfaRegs             fcp2CfaRegs;
    /**< CFA Register Address */

    /* Flex CC Modules */
    CSL_flexcc_cfgRegs          fcp2Regs;
    /**< FCP Register Address */
    uint32_t                    fcp2GammaLut1Data[BUFF_GAMMA_LUT1_SIZE];
    /**< Gamma/Contrast Lut1 Address */
    uint32_t                    fcp2GammaLut2Data[BUFF_GAMMA_LUT2_SIZE];
    /**< Gamma/Contrast Lut2 Address */
    uint32_t                    fcp2GammaLut3Data[BUFF_GAMMA_LUT3_SIZE];
    /**< Gamma/Contrast Lut3 Address */
    uint32_t                    fcp2Y8C8LutData[BUFF_Y8C8_LUT_SIZE];
    /**< RGB Lut Address */
    uint32_t                    fcp2C8G8LutData[BUFF_C8G8_LUT_SIZE];
    /**< RGB Lut Address */
    uint32_t                    fcp2S8B8LutData[BUFF_S8B8_LUT_SIZE];
    /**< RGB Lut Address */
    CSL_flexeeRegs              fcp2EeRegs;
    /**< Edge Enhancer Register address */
    CSL_flexcc_histRegs         fcp2HistRegs;
    /**< Histogram data Register address */
#endif
} Vhwa_M2mVissSaveRegData;

/*
 * Enum to identify the buffer object.
 */
typedef enum
{
    BUFF_ID_TOP_REGS = 0,
    BUFF_ID_LSE_REGS,
    BUFF_ID_HTS_HWA0_REGS,
    BUFF_ID_HTS_DMA_0_4_REGS,
    BUFF_ID_HTS_DMA_240_245_REGS,
    BUFF_ID_RFE_REGS,
    BUFF_ID_DPC_LUT_ADDR,
    BUFF_ID_LSC_SUT_ADDR,
    BUFF_ID_DECOMP_LUT1_ADDR,
    BUFF_ID_DECOMP_LUT2_ADDR,
    BUFF_ID_DECOMP_LUT3_ADDR,
    BUFF_ID_COMP_LUT_ADDR,
    BUFF_ID_H3A_LUT_ADDR,
    BUFF_ID_H3A_REGS,
    BUFF_ID_GLBCE_REGS,
    BUFF_ID_GLBCE_STATS_MEM,
    BUFF_ID_NSF4_REGS,
    BUFF_ID_CFA_REGS,
    BUFF_ID_FCP_REGS,
    BUFF_ID_GAMMA_LUT1_ADDR,
    BUFF_ID_GAMMA_LUT2_ADDR,
    BUFF_ID_GAMMA_LUT3_ADDR,
    BUFF_ID_Y8C8_LUT_ADDR,
    BUFF_ID_C8G8_LUT_ADDR,
    BUFF_ID_S8B8_LUT_ADDR,
    BUFF_ID_EE_REGS,
    BUFF_ID_HIST_REGS,
#if defined VHWA_VPAC_IP_REV_VPAC3
    BUFF_ID_NSF4_HIST_LUT_REGS,
    BUFF_ID_NSF4_HIST_REGS,
    BUFF_ID_CFA_DLUT_REGS,
    BUFF_ID_CAC_REGS,
    BUFF_ID_CAC_LUT_REGS,
    BUFF_ID_FCP2_CFA_DLUT_REGS,
    BUFF_ID_FCP2_CFA_REGS,
    BUFF_ID_FCP2_REGS,
    BUFF_ID_FCP2_GAMMA_LUT1_ADDR,
    BUFF_ID_FCP2_GAMMA_LUT2_ADDR,
    BUFF_ID_FCP2_GAMMA_LUT3_ADDR,
    BUFF_ID_FCP2_Y8C8_LUT_ADDR,
    BUFF_ID_FCP2_C8G8_LUT_ADDR,
    BUFF_ID_FCP2_S8B8_LUT_ADDR,
    BUFF_ID_FCP2_EE_REGS,
    BUFF_ID_FCP2_HIST_REGS,
#endif
    BUFF_ID_MAXBUFID
} Vhwa_M2mVissConfigBuffId;

/* buffer object to identify the buffer */
typedef struct
{
    /* buffer id, this point to the enum corresponding to the buffer component */
    uint8_t bufferID;
    /* source  pointer, pointing to buffer address */
    uint32_t *srcPtr;
    /* destination pointer, actual register address */
    uint32_t *dstPtr;
    /* length of the buffer */
    uint32_t length;
    /* flag to check if this object being used */
    bool isUsed;
    /* flag to check if this object being modified for the frame */
    bool isModified;
} Vhwa_M2mVissConfigBuffobj;

typedef struct
{
    uint32_t                    isEnabled;
    /**< Flag to indicate if the channel is enabled or not */

    /* Image Information */
    uint32_t                    widthInBytes;
    /**< Width of the frame, in bytes,
     *   This is the number of bytes copied */
    uint32_t                    width;
    /**< Width of the frame, storing for easy access */
    uint32_t                    height;
    /**< Height of the frame */
    uint32_t                    ccsf;
    /**< Storage format for this channel,
     *   Storing for each channel, for easy access.
     *   Mainly used for setting it in LSE */
    uint32_t                    dataFmt;
    /**< Locally storing data format, so that YUV422 can be available */
    uint32_t                    imgPitch;
    /**< Pitch for this channel */

    /* Sl2 Information */
    uint32_t                    sl2Pitch;
    /**< Pitch in SL2 memory for this channel */
    uint32_t                    sl2Depth;
    /**< Number of lines in SL2 for this channel */

    /* Below information is used for calculating blanking */
    uint32_t                    extraBlanking;
    /**< Additional blanking required for this channel,
     *  in addition to total blanking, eg chroma channel requires
     *  addition one line in blanking */
    uint32_t                    isLumaCh;
    /**< Flag to see if this is luma channel,
     *   LSE Init skip is calculated only for the luma channel */

    uint32_t                    frmIdx;
    /**< Input frame Idx, essentially #VhwaM2mViss_OutputIdx,
      *  Stored here for the easy access */
    uint32_t                    frmAddrIdx;
    /**< Input Fvid2_Frame has multiple addresses, this idx
     *   tells exactly which index is used for this channel */

    /* Addresses */
    uint64_t                    sl2Addr;
    /**< SL2 Memory Address */
    uint8_t                    *trMem;
    /**< Pointer to TX TR Descr Memory */
    uint32_t                   *trRespMem;
    /**< Pointer to TX TR Response Descr Memory */
    uint64_t                    bufAddr;
    /**< Local Storing buffer address to be set in TR
     *   Buffer address from the Fvid2_Frame is temporarily stored
     *   here, so that Entire Fvid2_Frame does not require to be passed
     *   to the udma layer. */

    uint32_t                   maxVertBlanking;

    uint32_t                   thr;

    uint32_t                   vPipe;

} Vhwa_M2mVissChParams;

typedef struct
{
    uint32_t                    isUsed;
    /**< Flag to indicate if this object is free or not */

    uint32_t                    isPrmsSet;
    /**< Flag to indicate if common parameters are set */

    /* Fvid2 Parameters */
    Vhwa_M2mVissCreateArgs      createArgs;
    /** Create Arguments */
    Fvid2_DrvCbParams           cbPrms;
    /** Callback parameters, copied locally */

    SemaphoreP_Handle           lock;
    /**< Semaphore to protect function calls and other memory allocation. */

    /* Input and Output channel information */
    Vhwa_M2mVissChParams        inChPrms[VHWA_M2M_VISS_MAX_IN_DMA_CH];
    /**< Input Channel parameters */
    Vhwa_M2mVissChParams        outChPrms[VHWA_M2M_VISS_MAX_OUT_DMA_CH];
    /**< Input Channel parameters */

    Vhwa_M2mVissSl2Prms         sl2Prms;
    /**< SL2 Parameters */

    /* Used for calculating blanking information */
    uint32_t                    maxLseInitSkip, minLseInitSkip;

    /**< Maximum Init Skip, used to set post load in HTS */
    uint32_t                    maxLseHorzBlanking;
    /**< Max Horizontal blanking in LSE */
    uint32_t                    maxLseVertBlanking;
    /**< Max Vertical blanking in LSE */

    uint32_t                    thr;
    /**< Threshold to be added in post load and depth */
    uint32_t                    totalSl2Size;
    /**< Total SL2 memory required for this handle */

    Fvid2UtilsLinkListObj       doneQObj;
    /**< Done Queue, queue containing queue objects,
     *   for which VISS processing is completed */
    Fvid2UtilsLinkListObj      *doneQ;
    /**< Pointer to Done Queue */

    uint32_t                    numPendReq;
    /**< Count of pending requests in driver,
     *   Does allow closing of driver, if this is not 0. */
    uint32_t                    hIdx;
    /**< Index of this handle in the global pool #gM2mVissHandleObj */
    uint64_t                    perf;
    /**< Variables storing last performance numbers in clock cycles
         for both luma and chroma */

    CSL_LseConfig               lseCfg;
    /**< LSE Configuration for this handle */
    CSL_HtsSchConfig            htsCfg;
    /**< HTS Configuration for this handle */
    Vhwa_M2mVissParams          vsPrms;
    /**< VISS Parameters */
    CslNsf4v_FrameConfig        nsf4vCfg;
    /**< Frame Information for Noise Filter */
    CslFcp_FrameConfig          fcpCfg;
    /**< Frame Information for FCP/CFA */
    CslRfe_FrameConfig          rfeCfg;
    /**< Frame Information for RFE */
    CslEe_FrameConfig           eeCfg;
    /**< Frame Information for Edge Enhancer */
    CslFcp_OutputSelect         fcpOutCfg;
    /**< Difference mux outputs FCP */
    uint32_t                    h3aModule;
    /**< H3A Module to be enabled */
    uint32_t                    eeMux;
    /**< Mux Value of the Edge Enhancer */
    Vhwa_M2mVissRegAddr         buffRegAddrs;
    /**< VISS register address to create UDMA buffer to write configuration */
    Vhwa_M2mVissConfigBuffobj   bufferObjHolder[BUFF_ID_MAXBUFID];
    /**< Buffer object holder index with buffer id to access */
    bool                        appBuffInitDone;
    /**< Flag to check application buffer is valid or not */
    Vhwa_M2mVissConfigAppBuff   appBufInfo;
    /**< Application buffer info */
    bool                        sameAsPrevHandle;
    /**< Flag to check if current handle is same as previous handle */
    uint32_t                    numOfConfigTrSubmitted;
    /**< Number of configuration TR submitted to ring */
    bool                        configInHwDone;
    /**< Number of configuration TR submitted to ring */
    /**< Structure containing register base addresses */

    Vhwa_M2mVissFcStatus        fcStatus;

#if defined VHWA_VPAC_IP_REV_VPAC3
    /* Used for calculating blanking information */
    uint32_t                    maxLseInitSkipMV, minLseInitSkipMV;

    uint32_t                    maxLseVertBlankingMV;
    /**< Max Vertical blanking in LSE for MV pipe */

    CslFcp_FrameConfig          fcp2Cfg;
    /**< Frame Information for FCP2 */

    CslEe_FrameConfig           fcp2EeCfg;                  /* TODO update for this */
    /**< Frame Information for Edge Enhancer */
    CslCac_FrameConfig          cacCfg;
    /**< Frame Information for CAC */

    CslFcp_OutputSelect         fcp2OutCfg;
    /**< Difference mux outputs FCP2 */

    uint32_t                    fcp2EeMux;
    /**< Mux Value of the Edge Enhancer */

    CslViss_OutputSelect        vissTopOutMux;

    uint32_t                    mvPipeDelay;
#endif
} Vhwa_M2mVissHandleObj;

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
    Fvid2Utils_QElem            qElem;
    /**< FVID2 Utils queue element used in node addition. */
    Vhwa_M2mVissHandleObj      *hObj;
    /**< Handle Object reference for this Queue object */
    Fvid2_FrameList             inFrmList;
    /**< FVID2 frame list for store list of input frames. */
    Fvid2_FrameList             outFrmList;
    /**< FVID2 frame list for store list of output frames. */
} Vhwa_M2mVissQueueObj;


typedef struct
{
    uint32_t                    initDone;
    /**< Flag to know if initialization for this instance is done or not */
    uint32_t                    isRegistered;
    /**< Flag to indicate if the VISS driver is registered
     *   to FVID2 layer */
    uint32_t                    isSl2AllocDone;
    /**< Flag to indicate if the SL2 memory is allocated */

    struct Udma_ChObj           utcChObj[VHWA_M2M_VISS_MAX_DMA_CH];
    /**< UDMA Channel object for input channels */
    Udma_ChHandle               utcChHndl[VHWA_M2M_VISS_MAX_DMA_CH];
    /**< UDMA Channel handles for input channels */
    Udma_RingHandle             inFqRingHndl[VHWA_M2M_VISS_MAX_IN_DMA_CH];
    /**< Forward Queue's Ring Handle */
    Udma_RingHandle             inCqRingHndl[VHWA_M2M_VISS_MAX_IN_DMA_CH];
    /**< Completion Queue's Ring Handle */
    uint32_t                    inCqRingNum[VHWA_M2M_VISS_MAX_IN_DMA_CH];
    /**< UDMA Completion ring number for input channels */
    uint32_t                    inUtcCh[VHWA_M2M_VISS_MAX_IN_DMA_CH];
    /**< Local UTC channel number, used for setting in HTS */
    Udma_RingHandle             outFqRingHndl[VHWA_M2M_VISS_MAX_OUT_DMA_CH];
    /**< Forward Queue's Ring Handle */
    Udma_RingHandle             outCqRingHndl[VHWA_M2M_VISS_MAX_OUT_DMA_CH];
    /**< Completion Queue's Ring Handle */
    uint32_t                    outCqRingNum[VHWA_M2M_VISS_MAX_OUT_DMA_CH];
    /**< UDMA Completion ring number for input channels */
    uint32_t                    outUtcCh[VHWA_M2M_VISS_MAX_OUT_DMA_CH];
    /**< Local UTC channel number, used for setting in HTS */

    Vhwa_M2mVissInitParams      initPrms;
    /**< Init parameters, containing udma handle */

    uint32_t                    openCnt;
    /**< Keeps track of number of opens,
     *   used to initialize common configuration only once
     *   for example, enabling VISS at the top level*/

    uint32_t                    pipeline;
    /**< HTS Pipeline number,
         Same pipeline is used for all handles,
         Allocated on the first open */

    /* Soc Information */
    Viss_SocInfo                socInfo;
    /**< Soc Information, like base address, interrupt number,
     *   INTD irq numbers etc. */
    uint32_t                    irqNum;
    /**< Core IRQ Number to be used for VISS,
     *   Used internally for isr registration,
     *   Storing it locally for easy access */
    uint32_t                    vhwaIrqNum;
    /**< VHWA IRQ Number,
     *   Refer to #VHWA_IrqNum for valid number.
     *   Used to enable the irq in INTD number,
     *   Storing it locally for easy access */
    HwiP_Handle                 intrHndl;
    /**< Handle to Hardware Interrupt */
    Vhwa_M2mVissRegAddr         regAddrs;
    /**< Structure containing register base addresses */

    /* Sl2 Information */
    Vhwa_M2mVissSl2Params       sl2AllocPrms;
    /**< Sl2 Alloc parameters,
     *   Initialized to default at init,
     *   If SL2 Alloc API is not called, default values are used
     *   for SL2 allocation  */
    uint64_t                    sl2Addr;
    /**< Allocated SL2 Address,
     *   Stored here for easy access. */
    uint32_t                    sl2Size;
    /**< Allocated Total SL2 Size,
     *   Stored here for easy access. */

    SemaphoreP_Handle           lock;
    /**< Semaphore to protect instance object's variables. */
    SemaphoreP_Handle           hwaLock;
    /**< Semaphore to protect VISS HWA.
     *   Only one processing request can be submitted to the VISS HW.
     *   This lock makes sure that except submitted one, all other
     *   requests are waiting on this semaphore.
     *   There is no longer support for submission from ISR and so
     *   no ISR protection in processRequest. */

    Vhwa_M2mVissHandleObj      *lastHndlObj;
    /**< Pointer to the handle object, whose request was submitted
     *   and completed */
    Vhwa_M2mVissHandleObj      *curHndlObj;
    /**< Current Active object */

    /* Queue parameters */
    Fvid2UtilsLinkListObj       freeQObj;
    /**< Free Queue, queue containing free queue objcts */
    Fvid2UtilsLinkListObj      *freeQ;
    /**< Pointer to Free Queue */
    Fvid2UtilsLinkListObj       reqQObj;
    /**< Request queue, containing list of pending requests */
    Fvid2UtilsLinkListObj      *reqQ;
    /**< Pointer to request Queue */
    Vhwa_M2mVissQueueObj        vissQObj[VHWA_M2M_VISS_UDMA_RING_ENTRIES];
    /**< Viss queue objects */
    Vhwa_M2mVissQueueObj       *actQObj;
    /**< Pointer points to queue object, whose request is submitted to
     * the HW and not yet completed */

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
    struct Udma_ChObj           configChObj;
    /* configuration channel object */
    Udma_ChHandle               configChHandle;
    /* UDMA handle for channel configuration */
    Udma_ChPrms                 configChPrms;
    Udma_RingHandle             configFqRingHndl;
    /**< Forward Queue's Ring Handle */
    Udma_RingHandle             configCqRingHndl;
    /**< Completion Queue's Ring Handle */
    uint32_t                    configCqRingNum;
    /**< UDMA Completion ring number for input channels */
    uint32_t                    ConfigcCh;
    /**< Config channel number */
    struct Udma_EventObj        configUdmaCqEventObj;
    /* config UDMA channel completion queue event object */
    SemaphoreP_Handle           configCqEventSemaphore;
    /**< Semaphore inform completion of transfer. */
    uint8_t                     *configTxTrMem;
    /**< Pointer to TX TR Descr Memory */
    uint8_t                     *configTxTrRespMem;
    /**< Pointer to TX TR response Memory */
    Vhwa_M2mVissSaveRegData     regData;
    /**< Structure used to store the initial register data */
    Vhwa_M2mVissConfigBuffobj   bufferObjHolder[BUFF_ID_MAXBUFID];
    /**< Buffer object holder index with buffer id to access */
} Vhwa_M2mVissInstObj;


/* ========================================================================== */
/*                           Function Declarations                            */
/* ========================================================================== */

int32_t Vhwa_m2mVissUdmaInit(Vhwa_M2mVissInstObj *instObj,
    Vhwa_M2mVissInitParams *initPrms);
int32_t Vhwa_m2mVissUdmaDeInit(const Vhwa_M2mVissInstObj *instObj);
int32_t Vhwa_m2mVissStartCh(const Vhwa_M2mVissInstObj *instObj);
int32_t Vhwa_m2mVissStartConfigCh(const Vhwa_M2mVissInstObj *instObj);
int32_t Vhwa_m2mVissStopCh(const Vhwa_M2mVissInstObj *instObj);

int32_t Vhwa_m2mVissSubmitRing(Vhwa_M2mVissInstObj *instObj,
    Vhwa_M2mVissHandleObj *hObj);
int32_t Vhwa_m2mVissAllocUdmaMem(Vhwa_M2mVissInstObj *instObj,
        Vhwa_M2mVissHandleObj *hObj);
int32_t Vhwa_m2mVissAllocConfigUdmaMem(Vhwa_M2mVissInstObj *instObj);
void Vhwa_m2mVissSetAddress(Vhwa_M2mVissHandleObj *hObj);
int32_t Vhwa_m2mVissPopRings(Vhwa_M2mVissInstObj *instObj,
    Vhwa_M2mVissHandleObj *hObj);
void Vhwa_m2mVissSetTrDesc(const Vhwa_M2mVissInstObj *instObj,
    Vhwa_M2mVissHandleObj *hObj);
void Vhwa_m2mVissSetH3aTrDesc(const Vhwa_M2mVissInstObj *instObj,
    Vhwa_M2mVissHandleObj *hObj);

uint32_t Vhwa_m2mVissCalcHorzSizeInBytes(uint32_t width, uint32_t ccsf,
    uint32_t dataFmt);

int32_t Vhwa_m2mVissSetRfeConfig(Vhwa_M2mVissInstObj *instObj,
		Vhwa_M2mVissHandleObj *hObj, const Rfe_Control *rfeCtrl);
int32_t Vhwa_m2mVissSetGlbceConfig(Vhwa_M2mVissInstObj *instObj,
    	Vhwa_M2mVissHandleObj *hObj, const Glbce_Control *ctrl);
int32_t Vhwa_m2mVissGetGlbceConfig(Vhwa_M2mVissInstObj *instObj,
    const Vhwa_M2mVissHandleObj *hObj, Glbce_Control *ctrl);
int32_t Vhwa_m2mVissSetNsf4Config(Vhwa_M2mVissInstObj *instObj,
    	Vhwa_M2mVissHandleObj *hObj, const Nsf4v_Config *nsf4Cfg);
int32_t Vhwa_m2mVissSetH3aConfig(Vhwa_M2mVissInstObj *instObj,
    	Vhwa_M2mVissHandleObj *hObj, const H3a_Config *h3aCfg);
int32_t Vhwa_m2mVissSetFcpConfig(Vhwa_M2mVissInstObj *instObj,
    Vhwa_M2mVissHandleObj *hObj, const Fcp_Control *ctrl);
void Vhwa_m2mVissSetHtsCfg(const Vhwa_M2mVissInstObj *instObj,
    Vhwa_M2mVissHandleObj *hObj, const Vhwa_M2mVissParams *vsPrms);
void Vhwa_m2mVissSetLseCfg(const Vhwa_M2mVissInstObj *instObj,
    Vhwa_M2mVissHandleObj *hObj, const Vhwa_M2mVissParams *vsPrms);
int32_t Vhwa_memVissSetVissSizeAndPath(Vhwa_M2mVissInstObj *instObj,
    Vhwa_M2mVissHandleObj *hObj);
void Vhwa_m2mVissStopModules(Vhwa_M2mVissInstObj *instObj,
    const Vhwa_M2mVissHandleObj *hObj);
void Vhwa_m2mVissGlbceStart(Vhwa_M2mVissInstObj *instObj,
    Vhwa_M2mVissHandleObj *hObj);
void Vhwa_m2mVissNsf4Init(Vhwa_M2mVissInstObj *instObj,
    	Vhwa_M2mVissHandleObj *hObj);
int32_t Vhwa_m2mVissSetH3aParams(Vhwa_M2mVissInstObj *instObj,
    Vhwa_M2mVissHandleObj *hObj, H3a_Config *h3aCfg);
void Vhwa_m2mVissGlbceInit(Vhwa_M2mVissInstObj *instObj,
    	Vhwa_M2mVissHandleObj *hObj);
int32_t Vhwa_memVissSetVissStartModules(Vhwa_M2mVissInstObj *instObj,
    		Vhwa_M2mVissHandleObj *hObj);
int32_t Vhwa_m2mVissReadHistogram(Vhwa_M2mVissInstObj *instObj,
    const Vhwa_M2mVissHandleObj *hObj, Fcp_HistData *histData);

int32_t Vhwa_m2mVissRegisterIsr(Vhwa_M2mVissInstObj *instObj);
void Vhwa_m2mVissUnregisterIsr(Vhwa_M2mVissInstObj *instObj);

void Vhwa_m2mVissInitSl2Params(Vhwa_M2mVissSl2Params *sl2Prms);

int32_t Vhwa_m2mVissSetInChParams(Vhwa_M2mVissInstObj *instObj,
    Vhwa_M2mVissHandleObj *hObj, Vhwa_M2mVissParams *vsPrms);
int32_t Vhwa_m2mVissSetOutChParams(Vhwa_M2mVissInstObj *instObj,
    Vhwa_M2mVissHandleObj *hObj, Vhwa_M2mVissParams *vsPrms);

void Vhwa_m2mVissInitAddresses(Vhwa_M2mVissInstObj *instObj);

uint32_t vhwa_m2mCalAppBufSize(void);
int32_t Vhwa_m2mVissProcessAppBuf(Vhwa_M2mVissInstObj *instObj,
        Vhwa_M2mVissHandleObj *hObj);
void Vhwa_m2mVissInitBuffAddresses(Vhwa_M2mVissHandleObj *hObj,
        uint32_t *bufferPtr);
void Vhwa_m2mVissInitBuffObject(Vhwa_M2mVissInstObj *instObj,
        Vhwa_M2mVissHandleObj *hObj);
void Vhwa_m2mVissInitSaveBuffObject(Vhwa_M2mVissInstObj *instObj);
void Vhwa_m2mVissInitRestoreBuffObject(Vhwa_M2mVissInstObj *instObj,
        Vhwa_M2mVissHandleObj *hObj);
void Vhwa_m2mVissSetConfigTRDesc(Vhwa_M2mVissInstObj *instObj,
        Vhwa_M2mVissConfigBuffobj *bufferObjHolder, bool sameAsPrevHandle,
        uint32_t numOfTr);
uint32_t Vhwa_m2mVissCalcNumOfTr(Vhwa_M2mVissHandleObj *hObj);
void Vhwa_m2mVissUpdateModifiedBufObj(Vhwa_M2mVissHandleObj *hObj);
void Vhwa_m2mVissResetModifiedFlag(Vhwa_M2mVissHandleObj *hObj);
int32_t Vhwa_m2mVissPopConfigRings(Vhwa_M2mVissInstObj *instObj);
int32_t Vhwa_m2mVissSubmitConfigUDMARing(Vhwa_M2mVissInstObj *instObj,
        const uint8_t *trMem);
int32_t Vhwa_m2mVissSubmitConfUDMABuf(Vhwa_M2mVissInstObj *instObj,
        Vhwa_M2mVissHandleObj *hObj);
int32_t Vhwa_m2mVissCheckConfUDMAComp(Vhwa_M2mVissInstObj *instObj,
        Vhwa_M2mVissHandleObj *hObj);
void Vhwa_m2mVissSetConfigTransferRecord(
        Vhwa_M2mVissConfigBuffobj *bufferObjHolder, bool sameAsPrevHandle,
        CSL_UdmapTR15 *pTr);
void Vhwa_m2mVissmakeTrpd(void *pTrpd, uint32_t trType, uint32_t trCnt,
        uint32_t cqRingNum);
void Vhwa_m2mVissConfgUdmaEvenCb(Udma_EventHandle eventHandle,
        uint32_t eventType, void *appData);
int32_t Vhwa_m2mVissSubmitSaveRestoreConfigUDMA(Vhwa_M2mVissInstObj *instObj,
        Vhwa_M2mVissHandleObj *hObj, bool saveConfig);

void Vhwa_m2mVissUpdatePipeline(uint32_t htsPipeline);

#if defined VHWA_VPAC_IP_REV_VPAC3
int32_t Vhwa_m2mVissSetFcp2Config(Vhwa_M2mVissInstObj *instObj,
    Vhwa_M2mVissHandleObj *hObj, const Fcp_Control *ctrl);
void Vhwa_m2mVissCacInit(Vhwa_M2mVissInstObj *instObj,
    const Vhwa_M2mVissHandleObj *hObj);
int32_t Vhwa_m2mVissSetCacConfig(Vhwa_M2mVissInstObj *instObj,
            Vhwa_M2mVissHandleObj *hObj, const Cac_Config *cfg);

int32_t Vhwa_m2mVissGetNsf4Histogram(Vhwa_M2mVissInstObj *instObj,
    const Vhwa_M2mVissHandleObj *hObj, Nsf4_Histogram *histData);
#endif

Vhwa_M2mVissHandleObj *Vhwa_m2mVissGetHandleObj(uint32_t cnt);

#ifdef __cplusplus
}
#endif

#endif  /* VHWA_M2M_VISS_PRIV_H_ */
