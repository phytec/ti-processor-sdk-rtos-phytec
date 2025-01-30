/*
 *   Copyright (c) Texas Instruments Incorporated 2019-2021
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
 *  \file     ecc_test_func.c
 *
 *  \brief    This file contains ECC SDL Function test code for R5 core.
 *
 *  \details  ECC SDL API module tests
 **/

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <stdint.h>
#include <soc.h>
#include <sdl_types.h>
#include <src/sdl/sdl_ecc.h>
#include <ti/drv/uart/UART_stdio.h>
#include <sdl_cache.h>
#include <interrupt.h>
#include <sdl_exception.h>
#include <sdl_arm_r5_pmu.h>
#include <common/ecc_test_main.h>
#include <sdl_esm.h>

/* Osal API header files */
#include <ti/osal/HwiP.h>
#include <ti/osal/TimerP.h>
/* ========================================================================== */
/*                                Macros                                      */
/* ========================================================================== */
#define ATCM_BASE_ADDR  (0x00000000U)
#define ATCM_SIZE       (0x00008000U)
#define TEST_AREA_SIZE  (0x00000400U)

#define ATCM_FILL_PATTERN (0xffffffffU)

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

SDL_ESM_config ECC_Test_esmInitConfig_WKUP =
{
    .esmErrorConfig = {0u, 8u}, /* Self test error config */
    .enableBitmap = {0xffffffffu, 0x00180003u, 0xffffffffu,
                },
     /**< All events enable: except clkstop events for unused clocks */
    .priorityBitmap = {0xffffffffu, 0x00180003u, 0xffffffffu,
                        },
    /**< All events high priority: except clkstop events for unused clocks */
    .errorpinBitmap = {0xffffffffu, 0x00180003u, 0xffffffffu,
                      },
    /**< All events high priority: except clkstop for unused clocks
     *   and selftest error events */
};
#if defined (SOC_J721E)||(SOC_J7200)
SDL_ESM_config ECC_Test_esmInitConfig_MCU =
{
    .esmErrorConfig = {0u, 3u}, /* Self test error config */
    .enableBitmap = {0xffffffffu, 0xff03ffffu, 0x7fffffffu, 0x00000007u,
                },
     /**< All events enable: except timer and self test  events, and Main ESM output */
    /* Temporarily disabling vim compare error as well*/
    .priorityBitmap = {0xffffffffu, 0xff03ffffu, 0x7fffffffu, 0x00000007u,
                        },
    /**< All events high priority: except timer, selftest error events, and Main ESM output */
    .errorpinBitmap = {0xffffffffu, 0xff03ffffu, 0x7fffffffu, 0x00000007u,
                      },
    /**< All events high priority: except timer, selftest error events, and Main ESM output */
};

SDL_ESM_config ECC_Test_esmInitConfig_MAIN =
{
    .esmErrorConfig = {1u, 8u}, /* Self test error config */
    .enableBitmap = {0x00000000u, 0xfffffffbu, 0x7fffffffu, 0xffffffffu,
                 0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                 0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                 0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                 0x00000000u, 0x00000000u, 0x00000000u, 0x00003c0fu,
                 0xffffffffu,
                },
     /**< All events enable: except clkstop events for unused clocks
      *   and PCIE events */
    .priorityBitmap = {0x00000000u, 0xfffffffbu, 0x7fffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                         0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                         0xffffffffu,
                        },
    /**< All events high priority: except clkstop events for unused clocks
     *   and PCIE events */
    .errorpinBitmap = {0x00000000u, 0xfffffffbu, 0x7fffffffu, 0xffffffffu,
                       0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                       0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                       0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                       0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                       0xffffffffu,
                      },
    /**< All events high priority: except clkstop for unused clocks
     *   and PCIE events */
};
#endif

#if defined (SOC_J721S2) 
SDL_ESM_config ECC_Test_esmInitConfig_MCU =
{
    .esmErrorConfig = {0u, 3u}, /* Self test error config */
    .enableBitmap = {0xfffffffbu, 0xff33ffffu, 0x7fffffffu, 0x00000007u,
                },
     /**< All events enable: except timer and self test  events, and Main ESM output */
    /* Temporarily disabling vim compare error as well*/
    .priorityBitmap = {0xfffffffbu, 0xff33ffffu, 0x7fffffffu, 0x00000007u,
                        },
    /**< All events high priority: except timer, selftest error events, and Main ESM output */
    .errorpinBitmap = {0xfffffffbu, 0xff33ffffu, 0x7fffffffu, 0x00000007u,
                      },
    /**< All events high priority: except timer, selftest error events, and Main ESM output */
};


SDL_ESM_config ECC_Test_esmInitConfig_MAIN =
{
    .esmErrorConfig = {1u, 8u}, /* Self test error config */
    .enableBitmap = {0x00000000u, 0xffffffdbu, 0x7fffffffu, 0xffffffffu,
                 0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                 0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                 0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                 0xfffbfeffu,
                },
     /**< All events enable: except clkstop events for unused clocks
      *   and PCIE events */
    .priorityBitmap = {0x00000000u, 0xffffffdbu, 0x7fffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                         0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                         0xfffbfeffu,
                        },
    /**< All events high priority: except clkstop events for unused clocks
     *   and PCIE events */
    .errorpinBitmap = {0x00000000u, 0xffffffdbu, 0x7fffffffu, 0xffffffffu,
                       0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                       0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                       0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                       0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                       0xfffbfeffu,
                      },
    /**< All events high priority: except clkstop for unused clocks
     *   and PCIE events */
};
#endif

#if defined (SOC_J784S4)
SDL_ESM_config ECC_Test_esmInitConfig_MCU =
{
    .esmErrorConfig = {0u, 3u}, /* Self test error config */
    .enableBitmap = {0xfffffffbu, 0xff33ffffu, 0x7fffffffu, 0x00000007u,
                },
     /**< All events enable: except timer and self test  events, and Main ESM output */
    /* Temporarily disabling vim compare error as well*/
    .priorityBitmap = {0xfffffffbu, 0xff33ffffu, 0x7fffffffu, 0x00000007u,
                        },
    /**< All events high priority: except timer, selftest error events, and Main ESM output */
    .errorpinBitmap = {0xfffffffbu, 0xff33ffffu, 0x7fffffffu, 0x00000007u,
                      },
    /**< All events high priority: except timer, selftest error events, and Main ESM output */
};


SDL_ESM_config ECC_Test_esmInitConfig_MAIN =
{
     .esmErrorConfig = {1u, 8u}, /* Self test error config */
     .enableBitmap = {0x00000000u, 0xffffffdbu, 0x7fffffffu, 0xffffffffu,
                   0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                   0xffffffffu, 0xfffffffbu, 0xffffffffu, 0xffffffffu,
                   0xffff7fffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                   0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                   0xfffbfeffu,
                  },
     /**< All events enable: except clkstop events for unused clocks
      *   and PCIE events */
     .priorityBitmap = {0x00000000u, 0xffffffdbu, 0x7fffffffu, 0xffffffffu,
                   0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                   0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                   0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                   0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                   0xfffbfeffu,
                  },
    /**< All events high priority: except clkstop events for unused clocks
     *   and PCIE events */
     .errorpinBitmap = {0x00000000u, 0xffffffdbu, 0x7fffffffu, 0xffffffffu,
                   0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                   0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                   0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                   0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                   0xfffbfeffu,
                  },
    /**< All events high priority: except clkstop for unused clocks
     *   and PCIE events */
};
#endif

extern int32_t SDL_ESM_applicationCallbackFunction(SDL_ESM_Inst esmInstType,
                                                   SDL_ESM_IntType esmIntType,
                                                   uint32_t grpChannel,
                                                   uint32_t index,
                                                   uint32_t intSrc,
                                                   void *arg);										   

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */
extern int32_t ECC_Test_UTILS_TCMEnable(void);

void ECC_Test_UTILS_initATCM(void)
{
    memset(ATCM_BASE_ADDR, ATCM_FILL_PATTERN, (ATCM_SIZE-TEST_AREA_SIZE));
}

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/* Defines */
#define MCU_R5F_MAX_MEM_SECTIONS   (7u)

/* Function prototypes */
void ECC_Test_copyResetVector(void);
int32_t ECC_Test_EsmInitHandlerInit(SDL_ESM_Inst esmInstType);
int32_t ECC_Test_init (void);
void ECC_Test_VIMDEDInterruptHandler(void);

static SDL_ECC_MemSubType ECC_Test_R5FCoresubMemTypeList[MCU_R5F_MAX_MEM_SECTIONS] =
{
    SDL_ECC_R5F_MEM_SUBTYPE_ATCM0_BANK0_VECTOR_ID,
    SDL_ECC_R5F_MEM_SUBTYPE_ATCM0_BANK1_VECTOR_ID,
    SDL_ECC_R5F_MEM_SUBTYPE_KS_VIM_RAM_VECTOR_ID,
    SDL_ECC_R5F_MEM_SUBTYPE_B0TCM0_BANK0_VECTOR_ID,
    SDL_ECC_R5F_MEM_SUBTYPE_B0TCM0_BANK1_VECTOR_ID,
    SDL_ECC_R5F_MEM_SUBTYPE_B1TCM0_BANK0_VECTOR_ID,
    SDL_ECC_R5F_MEM_SUBTYPE_B1TCM0_BANK1_VECTOR_ID,
};

static SDL_ECC_InitConfig_t ECC_Test_R5FCoreECCInitConfig =
{
    .numRams = MCU_R5F_MAX_MEM_SECTIONS,
    /**< Number of Rams ECC is enabled  */
    .pMemSubTypeList = &(ECC_Test_R5FCoresubMemTypeList[0]),
    /**< Sub type list  */
};

#ifdef SOC_J721E
#define MCU_CBASS_MAX_MEM_SECTIONS (3u)
#define MAIN_MSMC_AGGR0_MAX_MEM_SECTIONS (5u)
static SDL_ECC_MemSubType ECC_Test_MCUCBASSsubMemTypeList[MCU_CBASS_MAX_MEM_SECTIONS] =
{
    SDL_MCU_CBASS_ECC_AGGR0_IMCU_COR_FW_VBUSP_32B_SOC_FW_SAFEG_WR_RAMECC_RAM_ID,
    SDL_MCU_CBASS_ECC_AGGR0_IMCU_COR_FW_VBUSP_32B_SOC_FW_SAFEG_RD_RAMECC_RAM_ID,
    SDL_MCU_CBASS_ECC_AGGR0_IMCU_COR_FW_VBUSP_32B_SOC_FW_SAFEG_EDC_CTRL_RAM_ID
};


static SDL_ECC_InitConfig_t ECC_Test_MCUCBASSECCInitConfig =
{
    .numRams = MCU_CBASS_MAX_MEM_SECTIONS,
    /**< Number of Rams ECC is enabled  */
    .pMemSubTypeList = &(ECC_Test_MCUCBASSsubMemTypeList[0]),
    /**< Sub type list  */
};

static SDL_ECC_MemSubType ECC_Test_MAINMSMC_A0subMemTypeList[MAIN_MSMC_AGGR0_MAX_MEM_SECTIONS] =
{
    SDL_COMPUTE_CLUSTER0_MSMC_ECC_AGGR0_DRU_CBASS_MMR_CFG_EDC_RAM_ID,
    SDL_COMPUTE_CLUSTER0_MSMC_ECC_AGGR0_MSMC_MMR_BUSECC_RAM_ID,
	SDL_COMPUTE_CLUSTER0_MSMC_ECC_AGGR0_RMW2_CACHE_TAG_PIPE_BUSECC_RAM_ID,
    SDL_COMPUTE_CLUSTER0_MSMC_ECC_AGGR0_CLEC_SRAM_RAMECC_RAM_ID,
	SDL_COMPUTE_CLUSTER0_MSMC_ECC_AGGR0_CLEC_J7ES_CLEC_EDC_CTRL_BUSECC_RAM_ID,
};

static SDL_ECC_InitConfig_t ECC_Test_MAINMSMCA0ECCInitConfig =
{
    .numRams = MAIN_MSMC_AGGR0_MAX_MEM_SECTIONS,
    /**< Number of Rams ECC is enabled  */
    .pMemSubTypeList = &(ECC_Test_MAINMSMC_A0subMemTypeList[0]),
    /**< Sub type list  */
};

#endif

#ifdef SOC_J7200
#define MCU_NAVSS0_MAX_MEM_SECTIONS (1u)
#define MAIN_MSMC_AGGR0_MAX_MEM_SECTIONS (2u)
#define SDL_MMCSD0_MAX_MEM_SECTIONS (2u)
static SDL_ECC_MemSubType ECC_Test_MCU_NAVSS0subMemTypeList[MCU_NAVSS0_MAX_MEM_SECTIONS] =
{
    SDL_VC_MAIN_HC_ECC_AGGR5_J7VC_HC2_CBASS_J7VC_RC_TO_HC2_VBUSM_M2M_BRIDGE_J7VC_HC2_CBASS_J7VC_RC_TO_HC2_VBUSM_M2M_BRIDGE_SRC_EDC_CTRL_BUSECC_RAM_ID,
};


static SDL_ECC_InitConfig_t ECC_Test_MCU_NAVSS0ECCInitConfig =
{
    .numRams = MCU_NAVSS0_MAX_MEM_SECTIONS,
    /**< Number of Rams ECC is enabled  */
    .pMemSubTypeList = &(ECC_Test_MCU_NAVSS0subMemTypeList[0]),
    /**< Sub type list  */
};

static SDL_ECC_MemSubType ECC_Test_SDL_MMCSD0subMemTypeList[ MCU_NAVSS0_MAX_MEM_SECTIONS] =
{
    SDL_MCU_VC_MCU_ECC_AGGR0_IMCU_COR_FW_VBUSP_32B_SOC_FW_SAFEG_EDC_CTRL_RAM_ID,
};

static SDL_ECC_InitConfig_t ECC_Test_SDL_MMCSD0ECCInitConfig =
{
    .numRams =  MCU_NAVSS0_MAX_MEM_SECTIONS,
    /**< Number of Rams ECC is enabled  */
    .pMemSubTypeList = &(ECC_Test_SDL_MMCSD0subMemTypeList[0]),
    /**< Sub type list  */
};

static SDL_ECC_MemSubType ECC_Test_SDL_NAVSS0_1subMemTypeList[ SDL_MMCSD0_MAX_MEM_SECTIONS] =
{
    SDL_NAVSS0_NAVSS256VCL_UDMASS_UDMASS_ECCAGGR0_EDC_CTRL_RAM_ID,
	SDL_NAVSS0_NAVSS256VCL_UDMASS_UDMAP0_TPCFG_PSTATE_RAM_ID
};

static SDL_ECC_InitConfig_t ECC_Test_NAVSS0_1InitConfig =
{
    .numRams = 1u,
    /**< Number of Rams ECC is enabled  */
    .pMemSubTypeList = &(ECC_Test_SDL_NAVSS0_1subMemTypeList[0]),
    /**< Sub type list  */
};


static SDL_ECC_MemSubType ECC_Test_MAINMSMC_A0subMemTypeList[MAIN_MSMC_AGGR0_MAX_MEM_SECTIONS] =
{
    SDL_COMPUTE_CLUSTER0_MSMC_MMR_BUSECC_RAM_ID,
	SDL_COMPUTE_CLUSTER0_POSTARB_PIPE_CFG_BUSECC_RAM_ID
};

static SDL_ECC_InitConfig_t ECC_Test_MAINMSMCA0ECCInitConfig =
{
    .numRams = MAIN_MSMC_AGGR0_MAX_MEM_SECTIONS,
    /**< Number of Rams ECC is enabled  */
    .pMemSubTypeList = &(ECC_Test_MAINMSMC_A0subMemTypeList[0]),
    /**< Sub type list  */
};

#endif


#ifdef SOC_J721S2
#define MCU_NAVSS0_MAX_MEM_SECTIONS (1u)
#define MAIN_MSMC_AGGR0_MAX_MEM_SECTIONS (4u)
#define SDL_MMCSD0_MAX_MEM_SECTIONS (2u)

static SDL_ECC_MemSubType ECC_Test_MAINMSMC_A0subMemTypeList[MAIN_MSMC_AGGR0_MAX_MEM_SECTIONS] =
{
	SDL_COMPUTE_CLUSTER0_COMPUTE_CLUSTER_J7AE_MSMC_EN_DRU_MSMC_EN_4MB_WRAP_MSMC_4MB_MSMC_WRAP_ECC_AGGR0_DRU0_CBASS_MMR_CFG_EDC_RAM_ID,
    SDL_COMPUTE_CLUSTER0_COMPUTE_CLUSTER_J7AE_P0_A72_DUAL_1MB_A72_DUAL_MID_ECC_AGGR_CPU1_A72_J7_1MB_WRAP_A72_DUAL_MID_ECC_AGGR_CPU1_EDC_CTRL_RAM_ID,
	SDL_COMPUTE_CLUSTER0_COMPUTE_CLUSTER_J7AE_P0_A72_DUAL_1MB_A72_DUAL_MID_ECC_AGGR_CPU1_CPU1_IF_DATA_SPRAM_BANK0_ECC_SVBUS_RAM_ID,
	SDL_COMPUTE_CLUSTER0_COMPUTE_CLUSTER_J7AE_MSMC_EN_DRU_MSMC_EN_4MB_WRAP_MSMC_4MB_MSMC_WRAP_ECC_AGGR0_DRU0_CBASS_MMR_FW_CH_EDC_RAM_ID
};

static SDL_ECC_InitConfig_t ECC_Test_MAINMSMCA0ECCInitConfig =
{
    .numRams = MAIN_MSMC_AGGR0_MAX_MEM_SECTIONS,
    /**< Number of Rams ECC is enabled  */
    .pMemSubTypeList = &(ECC_Test_MAINMSMC_A0subMemTypeList[0]),
    /**< Sub type list  */
};


static SDL_ECC_MemSubType ECC_Test_WKUP_VTM0subMemTypeList[MCU_NAVSS0_MAX_MEM_SECTIONS] =
{
    SDL_WKUP_VTM0_K3VTM_N16FFC_ECCAGGR_K3VTM_N16FFC_CFG_CBASS_VBUSP_P2P_BRIDGE_EDC_CTRL_0_RAM_ID,
};


static SDL_ECC_InitConfig_t ECC_Test_WKUP_VTM0ECCInitConfig =
{
    .numRams = MCU_NAVSS0_MAX_MEM_SECTIONS,
    /**< Number of Rams ECC is enabled  */
    .pMemSubTypeList = &(ECC_Test_WKUP_VTM0subMemTypeList[0]),
    /**< Sub type list  */
};

static SDL_ECC_MemSubType ECC_Test_SDL_NAVSS0_1subMemTypeList[ SDL_MMCSD0_MAX_MEM_SECTIONS] =
{
    SDL_NAVSS0_NAVSS512J7AM_MODSS_ECCAGGR0_NAVSS512J7AM_MODSS_ECCAGGR0_EDC_CTRL_RAM_ID,
	SDL_NAVSS0_NAVSS512J7AM_MODSS_ECCAGGR0_NAVSS512J7AM_MODSS_SPINLOCK0_EDC_CTRL_0_RAM_ID      
};

static SDL_ECC_InitConfig_t ECC_Test_NAVSS0_1InitConfig =
{
    .numRams = 1u,
    /**< Number of Rams ECC is enabled  */
    .pMemSubTypeList = &(ECC_Test_SDL_NAVSS0_1subMemTypeList[0]),
    /**< Sub type list  */
};

static SDL_ECC_MemSubType ECC_Test_SDL_MMCSD0subMemTypeList[ MCU_NAVSS0_MAX_MEM_SECTIONS] =
{
    SDL_MCUM_MCU_ECC_AGGR0_J7AM_MCU_ECC_AGGR_IMCU_COR_FW_VBUSP_32B_SOC_FW_SAFEG_EDC_CTRL_RAM_ID,
};

static SDL_ECC_InitConfig_t ECC_Test_SDL_MMCSD0ECCInitConfig =
{
    .numRams =  MCU_NAVSS0_MAX_MEM_SECTIONS,
    /**< Number of Rams ECC is enabled  */
    .pMemSubTypeList = &(ECC_Test_SDL_MMCSD0subMemTypeList[0]),
    /**< Sub type list  */
};

#endif

#ifdef SOC_J784S4
#define MCU_NAVSS0_MAX_MEM_SECTIONS (1u)
#define SDL_MMCSD0_MAX_MEM_SECTIONS (2u)
#define MAIN_MSMC_AGGR0_MAX_MEM_SECTIONS (4u)

static SDL_ECC_MemSubType ECC_Test_MAINMSMC_A0subMemTypeList[MAIN_MSMC_AGGR0_MAX_MEM_SECTIONS] =
{ 
    SDL_COMPUTE_CLUSTER0_COMPUTE_CLUSTER_J7AHP_MSMC_EN_DRU_MSMC_EN_8MB_WRAP_MSMC_8MB_MSMC_WRAP_ECC_AGGR0_DRU0_CBASS_MMR_CFG_EDC_RAM_ID,
    SDL_COMPUTE_CLUSTER0_COMPUTE_CLUSTER_J7AHP_MSMC_EN_DRU_MSMC_EN_8MB_WRAP_MSMC_8MB_MSMC_WRAP_ECC_AGGR0_VBUSP_DMSC_CBASS_DRU0_MMR_FW_BRIDGE_DST_BUSECC_RAM_ID,
    SDL_COMPUTE_CLUSTER0_COMPUTE_CLUSTER_J7AHP_MSMC_EN_DRU_MSMC_EN_8MB_WRAP_MSMC_8MB_MSMC_WRAP_ECC_AGGR0_MSMC_LAMBDA3_WRAP_MSMC_QOR_MSMC_CORE_EMIF_PIPE_EMIF0_SLAVE_PIPE_EMIF_SLV_EDC_CTRL_0_RAM_ID,
	SDL_COMPUTE_CLUSTER0_COMPUTE_CLUSTER_J7AHP_MSMC_EN_DRU_MSMC_EN_8MB_WRAP_MSMC_8MB_MSMC_WRAP_ECC_AGGR0_MSMC_LAMBDA3_WRAP_MSMC_QOR_MSMC_CORE_EMIF_PIPE_EMIF0_DP_TO_VBUSMC_EDC_CTRL_0_RAM_ID,
};

static SDL_ECC_InitConfig_t ECC_Test_MAINMSMCA0ECCInitConfig =
{
    .numRams = MAIN_MSMC_AGGR0_MAX_MEM_SECTIONS,
    /**< Number of Rams ECC is enabled  */
    .pMemSubTypeList = &(ECC_Test_MAINMSMC_A0subMemTypeList[0]),
    /**< Sub type list  */
};

static SDL_ECC_MemSubType ECC_Test_WKUP_VTM0subMemTypeList[MCU_NAVSS0_MAX_MEM_SECTIONS] =
{
	SDL_WKUP_VTM0_K3VTM_N16FFC_ECCAGGR_K3VTM_N16FFC_CFG_CBASS_VBUSP_P2P_BRIDGE_EDC_CTRL_0_RAM_ID,
};

static SDL_ECC_InitConfig_t ECC_Test_WKUP_VTM0ECCInitConfig =
{
    .numRams = MCU_NAVSS0_MAX_MEM_SECTIONS,
    /**< Number of Rams ECC is enabled  */
    .pMemSubTypeList = &(ECC_Test_WKUP_VTM0subMemTypeList[0]),
    /**< Sub type list  */
};

static SDL_ECC_MemSubType ECC_Test_SDL_NAVSS0_1subMemTypeList[ SDL_MMCSD0_MAX_MEM_SECTIONS] =
{
    SDL_NAVSS0_NAVSS512J7AM_MODSS_ECCAGGR0_NAVSS512J7AM_MODSS_ECCAGGR0_EDC_CTRL_RAM_ID,
	SDL_NAVSS0_NAVSS512J7AM_MODSS_ECCAGGR0_NAVSS512J7AM_MODSS_SPINLOCK0_EDC_CTRL_0_RAM_ID      
};

static SDL_ECC_InitConfig_t ECC_Test_NAVSS0_1InitConfig =
{
    .numRams = 1u,
    /**< Number of Rams ECC is enabled  */
    .pMemSubTypeList = &(ECC_Test_SDL_NAVSS0_1subMemTypeList[0]),
    /**< Sub type list  */
};

static SDL_ECC_MemSubType ECC_Test_SDL_MMCSD0subMemTypeList[ MCU_NAVSS0_MAX_MEM_SECTIONS] =
{
    SDL_MCUM_MCU_ECC_AGGR0_J7AM_MCU_ECC_AGGR_IMCU_COR_FW_VBUSP_32B_SOC_FW_SAFEG_EDC_CTRL_RAM_ID,
};

static SDL_ECC_InitConfig_t ECC_Test_SDL_MMCSD0ECCInitConfig =
{
    .numRams =  MCU_NAVSS0_MAX_MEM_SECTIONS,
    /**< Number of Rams ECC is enabled  */
    .pMemSubTypeList = &(ECC_Test_SDL_MMCSD0subMemTypeList[0]),
    /**< Sub type list  */
};

#endif


static void timerFxn(uintptr_t arg)
{
    return;
}


static void VIM_Test_startTimer(void)
{
    TimerP_Params  timerParams;
    TimerP_Handle  timerHandle;

    /* Initialize the parameters */
    TimerP_Params_init(&timerParams);
    timerParams.startMode = (uint32_t)TimerP_StartMode_USER;
    timerParams.periodType = (uint32_t)TimerP_PeriodType_MICROSECS;
    timerParams.period    = 1000000u;
    timerHandle = TimerP_create(TimerP_ANY, (TimerP_Fxn)&timerFxn, &timerParams);

    if ( timerHandle != NULL_PTR)
    {
        /* start the timer */
        (void)TimerP_start(timerHandle);
    }

}

HwiP_Handle ECC_Test_EsmHiHwiPHandle;
HwiP_Handle ECC_Test_EsmLoHwiPHandle;
HwiP_Handle ECC_Test_EsmCfgHwiPHandle;

extern void *_resetvectors;
#define VECTOR_SIZE 0x100
#define APP_MAX_PROFILE_SIZE 16

void ECC_Test_cpuCacheInv(const void * addr, int32_t size)
{
    SDL_armR5CacheInv(addr, size);
}

void ECC_Test_cpuCacheWbInv(const void * addr, int32_t size)
{
    SDL_armR5CacheWbInv(addr, size);
}

void ECC_Test_copyResetVector(void)
{
    /* Invalidate cache */
    ECC_Test_cpuCacheInv(0, VECTOR_SIZE);

    /* copy the vectors into 0 location */
    /* Vectors are always assumed to be at 0 for R5 */
    memcpy(0, &_resetvectors, VECTOR_SIZE);

    /* Write back cache */
    ECC_Test_cpuCacheWbInv(0, VECTOR_SIZE);
}

/* This is the list of exception handle and the parameters */
const SDL_R5ExptnHandlers ECC_Test_R5ExptnHandlers =
{
    .udefExptnHandler = &SDL_EXCEPTION_undefInstructionExptnHandler,
    .swiExptnHandler = &SDL_EXCEPTION_swIntrExptnHandler,
    .pabtExptnHandler = &SDL_EXCEPTION_prefetchAbortExptnHandler,
    .dabtExptnHandler = &SDL_EXCEPTION_dataAbortExptnHandler,
    .irqExptnHandler = &SDL_EXCEPTION_irqExptnHandler,
    .fiqExptnHandler = &SDL_EXCEPTION_fiqExptnHandler,
    .udefExptnHandlerArgs = ((void *)0u),
    .swiExptnHandlerArgs = ((void *)0u),
    .pabtExptnHandlerArgs = ((void *)0u),
    .dabtExptnHandlerArgs = ((void *)0u),
    .irqExptnHandlerArgs = ((void *)0u),
};

void ECC_Test_undefInstructionExptnCallback(void)
{
    UART_printf("\n Undefined Instruction exception");
}

void ECC_Test_swIntrExptnCallback(void)
{
    UART_printf("\n Software interrupt exception");
}

void ECC_Test_prefetchAbortExptnCallback(void)
{
    UART_printf("\n Prefetch Abort exception");
}
void ECC_Test_dataAbortExptnCallback(void)
{
    UART_printf("\n Data Abort exception");
}
void ECC_Test_irqExptnCallback(void)
{
    UART_printf("\n Irq exception");
}

void ECC_Test_fiqExptnCallback(void)
{
    UART_printf("\n Fiq exception");
}

void ECC_Test_exceptionInit(void)
{

    SDL_EXCEPTION_CallbackFunctions_t exceptionCallbackFunctions =
            {
             .udefExptnCallback = ECC_Test_undefInstructionExptnCallback,
             .swiExptnCallback = ECC_Test_swIntrExptnCallback,
             .pabtExptnCallback = ECC_Test_prefetchAbortExptnCallback,
             .dabtExptnCallback = ECC_Test_dataAbortExptnCallback,
             .irqExptnCallback = ECC_Test_irqExptnCallback,
             .fiqExptnCallback = ECC_Test_fiqExptnCallback,
            };

    /* Initialize SDL exception handler */
    SDL_EXCEPTION_init(&exceptionCallbackFunctions);
    /* Register SDL exception handler */
    Intc_RegisterExptnHandlers(&ECC_Test_R5ExptnHandlers);

    return;
}
static uint32_t arg;
/*********************************************************************
* @fn      ECC_Test_init
*
* @brief   Initializes Software Diagostics Test Framework
*
* @param   None
*
* @return    0 : Success; < 0 for failures
*/
int32_t ECC_Test_init (void)
{
    int32_t retValue=0;
    void *ptr = (void *)&arg;
    SDL_ErrType_t result;
    SDL_ECC_staticRegs staticRegs;

    /* Enable ATCM and BTCM */
    ECC_Test_UTILS_TCMEnable();

    /* Initialize ATCM */
    ECC_Test_UTILS_initATCM();

    Board_initCfg boardCfg;

    boardCfg = BOARD_INIT_PINMUX_CONFIG |
               BOARD_INIT_UART_STDIO;
    Board_init(boardCfg);

    UART_printf("\nECC_Test_init: UART ready to print, proceeding with ECC_Test init \n");

    UART_printf("\nECC_Test_init: Profile init complete \n");

    /* Call Init for ECC_Test software structures */

    /* Copy reset vector to 0 location */
    ECC_Test_copyResetVector();

    UART_printf("\nECC_Test_init: Copy Reset vector complete \n");

    /* Initialise exception handler */
    ECC_Test_exceptionInit();

    UART_printf("\nECC_Test_init: Exception init complete \n");

    /* Timer is used to trigger the interrupt vector used for VIM DED Handler error */
    VIM_Test_startTimer();

    /* Register VIM DED interrupt handler */
    SDL_ECC_registerVIMDEDHandler(&ECC_Test_VIMDEDInterruptHandler);

    UART_printf("\nECC_Test_init: register VIM DED Handler complete \n");

    if (retValue == 0) {
        /* Initialize MCU ESM module */
        result = SDL_ESM_init(SDL_ESM_INST_MCU_ESM0, &ECC_Test_esmInitConfig_MCU, SDL_ESM_applicationCallbackFunction, ptr);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("ECC_Test_init: Error initializing MCU ESM: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nECC_Test_init: Init MCU ESM complete \n");
        }
    }

    if (retValue == 0) {
        /* Initialize WKUP ESM module */
        result = SDL_ESM_init(SDL_ESM_INST_WKUP_ESM0, &ECC_Test_esmInitConfig_WKUP,SDL_ESM_applicationCallbackFunction,ptr);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("ECC_Test_init: Error initializing WKUP ESM: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nECC_Test_init: Init WKUP ESM complete \n");
        }
    }

    if (retValue == 0) {
        /* Initialize MAIN ESM module */
        result = SDL_ESM_init(SDL_ESM_INST_MAIN_ESM0, &ECC_Test_esmInitConfig_MAIN,SDL_ESM_applicationCallbackFunction,ptr);
        if (result != SDL_PASS) {
            /* print error and quit */
            UART_printf("ECC_Test_init: Error initializing MAIN ESM: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nECC_Test_init: Init MAIN ESM complete \n");
        }
    }

    if (retValue == 0) {
        /* Initialize ECC */
        result = SDL_ECC_init(SDL_ECC_MEMTYPE_MCU_R5F0_CORE, &ECC_Test_R5FCoreECCInitConfig);
        if (result != SDL_PASS) {
            /* print error and quit */
            UART_printf("ECC_Test_init: Error initializing R5F core ECC: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nECC_Test_init: R5F Core ECC Init complete \n");
        }
    }

    if (retValue == 0) {
        /* Read back the static registers */
        result = SDL_ECC_getStaticRegisters(SDL_ECC_MEMTYPE_MCU_R5F0_CORE, &staticRegs);
        if (result != SDL_PASS) {
            /* print error and quit */
            UART_printf("ECC_Test_init: Error reading the static registers: result = %d\n");

            retValue = -1;
        } else {
            UART_printf("\nECC_Test_init: R5F Memtype Register Readback successful \n");
        }
    }
	
#if defined (SOC_J721E)
    if (retValue == 0) {
        /* Initialize ECC */
        result = SDL_ECC_init(SDL_ECC_MEMTYPE_MCU_CBASS_ECC_AGGR0, &ECC_Test_MCUCBASSECCInitConfig);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("ECC_Test_init: Error initializing R5F core ECC: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nECC_Test_init: MCU CBASS ECC Init complete \n");
        }
    }
	
	if (retValue == 0) {
        /* Read back the static registers */
        result = SDL_ECC_getStaticRegisters(SDL_ECC_MEMTYPE_MCU_CBASS_ECC_AGGR0, &staticRegs);
        if (result != SDL_PASS) {
            /* print error and quit */
            UART_printf("ECC_Test_init: Error reading the static registers: result = %d\n");

            retValue = -1;
        } else {
            UART_printf("\nECC_Test_init: R5F Memtype Register Readback successful \n");
        }
    }
	
#endif

#if defined (SOC_J7200)
    if (retValue == 0) {
        /* Initialize ECC */
        result = SDL_ECC_init(SDL_ECC_VC_MAIN_HC_ECC_AGGR5_ECC_AGGR, &ECC_Test_MCU_NAVSS0ECCInitConfig);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("ECC_Test_init: Error initializing MCU_NAVSS0 ECC: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nECC_Test_init: MCU_NAVSS0 ECC Init complete \n");
        }
    }
	
	
	
	if (retValue == 0) {
        /* Initialize ECC */
        result = SDL_ECC_init(SDL_ECC_NAVSS0_1_ECC_AGGR, &ECC_Test_NAVSS0_1InitConfig);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("ECC_Test_init: Error initializing MAIN ECC_NAVSS0_1  ECC: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nECC_Test_init: MAIN ECC_NAVSS0_1 ECC Init complete \n");
        }
    }
	
	if (retValue == 0) {
        /* Initialize ECC */
        result = SDL_ECC_init(SDL_ECC_MCU_VC_MCU_ECC_AGGR0, &ECC_Test_SDL_MMCSD0ECCInitConfig);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("ECC_Test_init: Error initializing MCU_NAVSS0 ECC: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nECC_Test_init: MCU_NAVSS0 ECC Init complete \n");
        }
    }
	
	if (retValue == 0) {
        /* Read back the static registers */
        result = SDL_ECC_getStaticRegisters(SDL_ECC_VC_MAIN_HC_ECC_AGGR5_ECC_AGGR, &staticRegs);
        if (result != SDL_PASS) {
            /* print error and quit */
            UART_printf("ECC_Test_init: Error reading the MCU_NAVSS0 ECC static registers: result = %d\n");

            retValue = -1;
        } else {
            UART_printf("\nECC_Test_init: MCU_NAVSS0 Memtype Register Readback successful \n");
        }
    }
	
#endif

    /* Initialize an ECC aggregator type that requires mapping.
     * This example only shows MSMC_AGGR0 instance.*/
    if (retValue == 0) {
        /* Initialize ECC */
        result = SDL_ECC_init(SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0, &ECC_Test_MAINMSMCA0ECCInitConfig);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("ECC_Test_init: Error initializing MAIN_MSMC ECC: result = %d\n", result);
 
            retValue = -1;
        } else {
            UART_printf("\nECC_Test_init: MAIN_MSMC ECC Init complete \n");
        }
    }

    /* Test the path for MSMC mapping */
    if (retValue == 0) {
        /* Read back the static registers */
        result = SDL_ECC_getStaticRegisters(SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0, &staticRegs);
        if (result != SDL_PASS) {
            /* print error and quit */
            UART_printf("ECC_Test_init: Error reading the static registers: result = %d\n");
	
            retValue = -1;
        } else {
            UART_printf("\nECC_Test_init: MAIN_MSMC Memtype Register Readback successful \n");
        }
    }

#if defined (SOC_J721S2) || defined (SOC_J784S4)
	if (retValue == 0) {
        /* Initialize ECC */
        result = SDL_ECC_init(SDL_NAVSS0_1_ECC_AGGR, &ECC_Test_NAVSS0_1InitConfig);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("ECC_Test_init: Error initializing MAIN ECC_NAVSS0_1  ECC: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nECC_Test_init: MAIN ECC_NAVSS0_1 ECC Init complete \n");
        }
    }
	
	if (retValue == 0) {
        /* Initialize ECC */
        result = SDL_ECC_init(SDL_MCU_NAVSS0_0_ECC_AGGR, &ECC_Test_WKUP_VTM0ECCInitConfig);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("ECC_Test_init: Error initializing MCU_NAVSS0 ECC: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nECC_Test_init: MCU_NAVSS0 ECC Init complete \n");
        }
    }
	
	if (retValue == 0) {
        /* Initialize ECC */
        result = SDL_ECC_init(SDL_MCUM_MCU_ECC_AGGR0_ECC_AGGR, &ECC_Test_SDL_MMCSD0ECCInitConfig);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("ECC_Test_init: Error initializing MCU_NAVSS0 ECC: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nECC_Test_init: MCU_NAVSS0 ECC Init complete \n");
        }
    }
	
	if (retValue == 0) {
        /* Initialize ECC */
        result = SDL_ECC_init(SDL_WKUP_VTM0_ECC_AGGR, &ECC_Test_WKUP_VTM0ECCInitConfig);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("ECC_Test_init: Error initializing MCU_NAVSS0 ECC: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nECC_Test_init: MCU_NAVSS0 ECC Init complete \n");
        }
    }
	
#endif


    if (retValue == 0) {
        /* Initialize ECC callbacks within the MCU ESM */
        result = SDL_ECC_initEsm(SDL_ESM_INST_MCU_ESM0);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("ECC_Test_init: Error initializing ECC callback for MCU ESM: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nECC_Test_init: ECC Callback Init complete for MCU ESM \n");
        }
    }

    if (retValue == 0) {
        /* Initialize ECC callbacks within the Main ESM */
        result = SDL_ECC_initEsm(SDL_ESM_INST_MAIN_ESM0);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("ECC_Test_init: Error initializing ECC callback for Main ESM: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nECC_Test_init: ECC Callback Init complete for Main ESM \n");
        }
    }

    if (retValue == 0) {
        /* Initialize ECC callbacks within the WKUP ESM */
        result = SDL_ECC_initEsm(SDL_ESM_INST_WKUP_ESM0);
        if (result != SDL_PASS) {
            /* print error and quit */
             UART_printf("ECC_Test_init: Error initializing ECC callback for WKUP ESM: result = %d\n", result);

            retValue = -1;
        } else {
            UART_printf("\nECC_Test_init: ECC Callback Init complete for WKUP ESM \n");
        }
    }

    if (retValue == 0) {
        /* Initialize VIM ECC memory ; This is specifically for VIM in lockstep*/
        /* All other memories are auto initialized by hardware */
        result = SDL_ECC_initMemory(SDL_ECC_MEMTYPE_MCU_R5F0_CORE, SDL_ECC_R5F_MEM_SUBTYPE_KS_VIM_RAM_VECTOR_ID);
        if (result != SDL_PASS) {
             /* print error and quit */
              UART_printf("ECC_Test_init: Error initializing ECC memory: retValue = %d\n", retValue);
             retValue = -1;
         } else {
             UART_printf("\nECC_Test_init: ECC Memory init complete \n");
         }
    }

    if (retValue == 0) {
        result = SDL_ECC_initMemory(SDL_ECC_MEMTYPE_MCU_R5F0_CORE, 1U);
        if (result != SDL_PASS) {
             /* print error and quit */
              UART_printf("ECC_Test_init: Error initializing ECC memory: retValue = %d\n", retValue);
             retValue = -1;
         } else {
             UART_printf("\nECC_Test_init: ECC Memory init complete \n");
         }
    }

    if (retValue == 0) {
        result = SDL_ECC_initMemory(SDL_ECC_MEMTYPE_MCU_R5F0_CORE, 28U);
        if (result != SDL_PASS) {
             /* print error and quit */
              UART_printf("ECC_Test_init: Error initializing ECC memory: retValue = %d\n", retValue);
             retValue = -1;
         } else {
             UART_printf("\nECC_Test_init: ECC Memory init complete \n");
         }
    }

    return retValue;
}

uint32_t SDL_getTime(void)
{
    return(SDL_R5PMU_readCntr(SDL_ARM_R5_PMU_CYCLE_COUNTER_NUM));
}

/* Local functions */
void ECC_Test_readVIMRAM(uint32_t offset);
int32_t ECC_Test_ECC_waitDEDTrigger(uint64_t timeCount);

void ECC_Test_ECC_dummyFunction(void);


__attribute__((section(".ecc_test_ecc_testcodesection"))) void ECC_Test_ECC_dummyFunction(void)
{
    volatile int i;

    i = 10;
    return;
}

/*********************************************************************
 * @fn      ECC_Test_runECC1BitInjectTest
 *
 * @brief   Execute ECC 1 bit inject test
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t ECC_Test_runECC1BitInjectTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;
    volatile uint32_t testLocationValue;

    UART_printf("\n ATCM Single bit error inject: test starting");

    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0x00);

    /* Run one shot test for ATCM 1 bit error */
    injectErrorConfig.flipBitMask = 0x10;
    result = SDL_ECC_injectError(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
                                 SDL_ECC_R5F_MEM_SUBTYPE_ATCM0_BANK0_VECTOR_ID,
                                 SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE,
                                 &injectErrorConfig);


    if (result != SDL_PASS ) {
        UART_printf("\n ATCM Single bit error inject at pErrMem 0x%p test failed",
                    injectErrorConfig.pErrMem);
        retVal = -1;
    } else {
        /* Access the memory where injection is expected */
        testLocationValue = injectErrorConfig.pErrMem[0];

        UART_printf("\n ATCM Single bit error inject at pErrMem 0x%p: test complete",
                    injectErrorConfig.pErrMem);
    }

    return retVal;
}

/*********************************************************************
 * @fn      ECC_Test_runECC2BitInjectTest
 *
 * @brief   Execute ECC 2 bit Inject test
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t ECC_Test_runECC2BitInjectTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;
    volatile uint32_t testLocationValue;

    UART_printf("\n ATCM Double bit error inject: starting");

    /* Run one shot test for ATCM 2 bit error */
    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0x200u);

    injectErrorConfig.flipBitMask = 0x101;
    result = SDL_ECC_injectError(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
                                 SDL_ECC_R5F_MEM_SUBTYPE_ATCM0_BANK0_VECTOR_ID,
                                 SDL_INJECT_ECC_ERROR_FORCING_2BIT_ONCE,
                                 &injectErrorConfig);

    /* Access the memory where injection is expected */
    testLocationValue = injectErrorConfig.pErrMem[0];

    if (result != SDL_PASS ) {
        UART_printf("\n ATCM Double bit error inject: at pErrMem 0x%p: fixed location once test failed",
                    injectErrorConfig.pErrMem);
       retVal = -1;
    } else {
        UART_printf("\n ATCM Double bit error inject: pErrMem 0x%p fixed location once test complete",
                    injectErrorConfig.pErrMem);
    }

    return retVal;
}


void ECC_Test_readVIMRAM(uint32_t offset)
{

    uint32_t *vimAddr = (uint32_t *)0x40F82000;
    volatile uint32_t tmpValue;

    tmpValue = vimAddr[offset >> 2u];

}

/*********************************************************************
 * @fn      ECC_Test_runECC1BitVIMRAMInjectTest
 *
 * @brief   Execute ECC 1 bit inject test on VIM RAM
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t ECC_Test_runECC1BitVIMRAMInjectTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;
    volatile uint32_t testLocationValue;

    UART_printf("\n VIM RAM Single bit error inject: test starting");

    /* Note the address is Actual error address */
    injectErrorConfig.pErrMem = (uint32_t *)(0x40F82000u);

    /* Run one shot test for ATCM 1 bit error */
    injectErrorConfig.flipBitMask = 0x10;
    result = SDL_ECC_injectError(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
                                 SDL_ECC_R5F_MEM_SUBTYPE_KS_VIM_RAM_VECTOR_ID,
                                 SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE,
                                 &injectErrorConfig);


    if (result != SDL_PASS ) {
        UART_printf("\n VIM RAM Single bit error inject at pErrMem 0x%p test failed",
                    injectErrorConfig.pErrMem);
        retVal = -1;
    } else {
        /* Access the memory where injection is expected */
        testLocationValue = injectErrorConfig.pErrMem[0];

        /* Note this should result in actual error call back */
        UART_printf("\n VIM RAM Single bit error inject at pErrMem 0x%p: test complete",
                    injectErrorConfig.pErrMem);
    }

    return retVal;
}


/*********************************************************************
 * @fn      ECC_Test_runECC2BitVIMRAMInjectTest
 *
 * @brief   Execute ECC 2 bit Inject test with VIM RAM
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t ECC_Test_runECC2BitVIMRAMInjectTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;
    volatile uint32_t testLocationValue;

    UART_printf("\n VIM RAM Double bit error inject: starting");

    /* Run one shot test for VIM RAM 2 bit error */
    /* Note the address is actual address */
    injectErrorConfig.pErrMem = (uint32_t *)(0x40F82004u);

    injectErrorConfig.flipBitMask = 0x101;
    result = SDL_ECC_injectError(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
                                 SDL_ECC_R5F_MEM_SUBTYPE_KS_VIM_RAM_VECTOR_ID,
                                 SDL_INJECT_ECC_ERROR_FORCING_2BIT_ONCE,
                                 &injectErrorConfig);


    if (result != SDL_PASS ) {
        UART_printf("\n VIM RAM Double bit error inject: at pErrMem 0x%p: fixed location once test failed",
                    injectErrorConfig.pErrMem);
       retVal = -1;
    } else {
        /* Access the memory where injection is expected */
        testLocationValue = injectErrorConfig.pErrMem[0];

        UART_printf("\n VIM RAM Double bit error inject: pErrMem 0x%p fixed location once test complete",
                    injectErrorConfig.pErrMem);
    }

    return retVal;
}

/*********************************************************************
 * @fn      ECC_Test_runECC2BitVIMRAMDEDvector
 *
 * @brief   Execute ECC 2 bit Inject test with VIM RAM,but expect to
 *          trigger VIM DED vector
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t ECC_Test_runECC2BitVIMRAMDEDvector(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;

    UART_printf("\n VIM RAM Double bit error inject: starting");

    /* Run one shot test for VIM RAM 2 bit error */
    /* This is expected to trigger the DED vector */
    injectErrorConfig.pErrMem = (uint32_t *)(0x40F82098u);

    injectErrorConfig.flipBitMask = 0x101;
    result = SDL_ECC_injectError(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
                                 SDL_ECC_R5F_MEM_SUBTYPE_KS_VIM_RAM_VECTOR_ID,
                                 SDL_INJECT_ECC_ERROR_FORCING_2BIT_ONCE,
                                 &injectErrorConfig);


    if (result != SDL_PASS ) {
        UART_printf("\n VIM RAM Double bit error inject: at pErrMem 0x%p: fixed location once test failed",
                    injectErrorConfig.pErrMem);
       retVal = -1;
    } else {
        UART_printf("\n VIM RAM Double bit error inject: pErrMem 0x%p fixed location once test complete",
                    injectErrorConfig.pErrMem);
    }
    UART_printf("\n waiting for DED Vector Trigger");
    if (retVal == 0) {
        retVal = ECC_Test_ECC_waitDEDTrigger(0x100000000ull);
    }
    if (retVal == 0) {
        UART_printf("\n DED Vector Trigger: Success");
    } else {
        UART_printf("\n DED Vector Trigger: Fail");
    }
    return retVal;
}


/*********************************************************************
 * @fn      ECC_Test_runECC1BitVIMRAMSelfTest
 *
 * @brief   Execute ECC 1 bit self test on VIM RAM
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t ECC_Test_runECC1BitVIMRAMSelfTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;
    volatile uint32_t testLocationValue;

    UART_printf("\n VIM RAM Single bit error self test: starting");

    /* Note the address is Actual error address */
    injectErrorConfig.pErrMem = (uint32_t *)(0x40F82000u);

    /* Run one shot test for ATCM 1 bit error */
    injectErrorConfig.flipBitMask = 0x10;
    result = SDL_ECC_selfTest(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
                              SDL_ECC_R5F_MEM_SUBTYPE_KS_VIM_RAM_VECTOR_ID,
                              SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE,
                              &injectErrorConfig,
                              100000);


    if (result != SDL_PASS ) {
        UART_printf("\n VIM RAM Single bit error self test: at pErrMem 0x%p test failed",
                    injectErrorConfig.pErrMem);
        retVal = -1;
    } else {
        /* Access the memory where injection is expected */
        testLocationValue = injectErrorConfig.pErrMem[0];

        /* Note this should result in actual error call back */
        UART_printf("\n VIM RAM Single bit error self test: at pErrMem 0x%p: test complete",
                    injectErrorConfig.pErrMem);
    }

    return retVal;
}


/*********************************************************************
 * @fn      ECC_Test_runECC2BitVIMRAMSelfTest
 *
 * @brief   Execute ECC 2 bit Self test with VIM RAM
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t ECC_Test_runECC2BitVIMRAMSelfTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;
    volatile uint32_t testLocationValue;

    UART_printf("\n VIM RAM Double bit error self test: starting");

    /* Run one shot test for VIM RAM 2 bit error */
    /* Note the address is actual address */
    injectErrorConfig.pErrMem = (uint32_t *)(0x40F82004u);

    injectErrorConfig.flipBitMask = 0x101;
    result = SDL_ECC_selfTest(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
                              SDL_ECC_R5F_MEM_SUBTYPE_KS_VIM_RAM_VECTOR_ID,
                              SDL_INJECT_ECC_ERROR_FORCING_2BIT_ONCE,
                              &injectErrorConfig,
                              100000);


    if (result != SDL_PASS ) {
        UART_printf("\n VIM RAM Double bit error self test: at pErrMem 0x%p: fixed location once test failed",
                    injectErrorConfig.pErrMem);
       retVal = -1;
    } else {
        /* Access the memory where injection is expected */
        testLocationValue = injectErrorConfig.pErrMem[0];

        UART_printf("\n VIM RAM Double bit error self test: pErrMem 0x%p fixed location once test complete",
                    injectErrorConfig.pErrMem);
    }

    return retVal;
}

/*********************************************************************
 * @fn      ECC_Test_runECC1BitSelfTest
 *
 * @brief   Execute ECC 1 bit Self test
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t ECC_Test_runECC1BitSelfTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;

    UART_printf("\n ATCM Single bit error self test: starting");

    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0x100);

    /* Run one shot test for ATCM 1 bit error */
    injectErrorConfig.flipBitMask = 0x10;
							 
    result = SDL_ECC_selfTest(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
                              SDL_ECC_R5F_MEM_SUBTYPE_ATCM0_BANK0_VECTOR_ID,
                              SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE,
                              &injectErrorConfig,
                              100000);

    if (result != SDL_PASS ) {
         UART_printf("\n ATCM Single bit error self test at pErrMem 0x%p test failed",
                     injectErrorConfig.pErrMem);
        retVal = -1;
    } else {

        UART_printf("\n ATCM Single bit error self test at pErrMem 0x%p: test complete",
                    injectErrorConfig.pErrMem);
    }

    return retVal;
}

int32_t ECC_Test_runECC_nonAccessable1BitSelfTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;

    UART_printf("\n ATCM non accessable Single bit error self test: starting");

    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0x100);

    /* Run one shot test for ATCM 1 bit error */
    injectErrorConfig.flipBitMask = 0x10;
    result = SDL_ECC_selfTest(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
                              0U,
                              SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE,
                              &injectErrorConfig,
                              100000);

    if (result != SDL_EFAIL ) {
         UART_printf("\n ATCM non accessable Single bit error self test at pErrMem 0x%p test failed",
                     injectErrorConfig.pErrMem);
        retVal = -1;
    } else {

        UART_printf("\n ATCM non accessable Single bit error self test at pErrMem 0x%p: test complete",
                    injectErrorConfig.pErrMem);
    }

    return retVal;
}

int32_t ECC_Test_runECC1BitNeg1SelfTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;

    UART_printf("\n ATCM Single bit error negative-1 self test: starting");

    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0x100);

    /* To get EFAIL  passing invalid chkgrp*/
    injectErrorConfig.chkGrp = 50U;
    result = SDL_ECC_selfTest(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
                              28U,
                              SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE,
                              &injectErrorConfig,
                              100000);


    if (result != SDL_EFAIL ) {
         UART_printf("\n ATCM Single bit error negative-1 self test at pErrMem 0x%p test failed",
                     injectErrorConfig.pErrMem);
        retVal = -1;
    } else {

        UART_printf("\n ATCM Single bit error negative-1 self test at pErrMem 0x%p: test complete",
                    injectErrorConfig.pErrMem);
    }

    return retVal;
}

/*********************************************************************
 * @fn      ECC_Test_runECC2BitSelfTest
 *
 * @brief   Execute ECC 2 bit Self Test
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t ECC_Test_runECC2BitSelfTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;

    UART_printf("\n ATCM Double bit error self test: starting");

    /* Run one shot test for ATCM 2 bit error */
    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0x400u);

    injectErrorConfig.flipBitMask = 0x101;
    result = SDL_ECC_selfTest(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
                              SDL_ECC_R5F_MEM_SUBTYPE_ATCM0_BANK0_VECTOR_ID,
                              SDL_INJECT_ECC_ERROR_FORCING_2BIT_ONCE,
                              &injectErrorConfig,
                              1000);


    if (result != SDL_PASS ) {
        UART_printf("\n ATCM Double bit error self test: at pErrMem 0x%p: fixed location once test failed",
                    injectErrorConfig.pErrMem);
        retVal = -1;
    } else {
        UART_printf("\n ATCM Double bit error self test: pErrMem 0x%p fixed location once test complete",
                    injectErrorConfig.pErrMem);
    }

    return retVal;
}

/*********************************************************************
 * @fn      ECC_Test_runECC1BitB0TCM0Bank0SelfTest
 *
 * @brief   Execute ECC single bit error test on BTCM0 Bank 0 memory
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t ECC_Test_runECC1BitB0TCM0Bank0SelfTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;

    UART_printf("\n B0TCM0 Bank0 Single bit error self test: starting");

    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0x41010100u);

    /* Run one shot test for BTCM0 1 bit error */
    injectErrorConfig.flipBitMask = 0x20;
    result = SDL_ECC_selfTest(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
                              SDL_ECC_R5F_MEM_SUBTYPE_B0TCM0_BANK0_VECTOR_ID,
                              SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE,
                              &injectErrorConfig,
                              100000);


    if (result != SDL_PASS ) {
        UART_printf("\n B0TCM0 Bank0 Single bit error self test at pErrMem 0x%p test failed",
                    injectErrorConfig.pErrMem);
       retVal = -1;
    } else {
        UART_printf("\n B0TCM0 Bank0 Single bit error self test at pErrMem 0x%p: test complete",
                    injectErrorConfig.pErrMem);
    }

    return retVal;
}


/*********************************************************************
 * @fn      ECC_Test_runECC2BitB0TCM0Bank0SelfTest
 *
 * @brief   Execute ECC Double bit error test on BTCM0 Bank 0 memory
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t ECC_Test_runECC2BitB0TCM0Bank0SelfTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;

    UART_printf("\n B0TCM0 Bank0 Double bit error self test: starting");

    /* Run one shot test for BTCM0 2 bit error */
    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0x41010200u);

    injectErrorConfig.flipBitMask = 0x201;
    result = SDL_ECC_selfTest(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
                              SDL_ECC_R5F_MEM_SUBTYPE_B0TCM0_BANK0_VECTOR_ID,
                              SDL_INJECT_ECC_ERROR_FORCING_2BIT_ONCE,
                              &injectErrorConfig,
                              1000);


    if (result != SDL_PASS ) {
        UART_printf("\n B0TCM0 Bank0 Double bit error self test: at pErrMem 0x%p: fixed location once test failed",
                    injectErrorConfig.pErrMem);
       retVal = -1;
    } else {
        UART_printf("\n B0TCM0 Bank0 Double bit error self test: pErrMem 0x%p fixed location once test complete",
                    injectErrorConfig.pErrMem);
    }

    return retVal;
}

/*********************************************************************
 * @fn      ECC_Test_runECC1BitB0TCM0Bank1SelfTest
 *
 * @brief   Execute ECC Single bit error test on BTCM0 Bank 1 memory
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t ECC_Test_runECC1BitB0TCM0Bank1SelfTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;

    UART_printf("\n B0TCM0 Bank1 Single bit error self test: starting");

    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0x41010104u);

    /* Run one shot test for BTCM0 1 bit error */
    injectErrorConfig.flipBitMask = 0x20;
    result = SDL_ECC_selfTest(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
                              SDL_ECC_R5F_MEM_SUBTYPE_B0TCM0_BANK1_VECTOR_ID,
                              SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE,
                              &injectErrorConfig,
                              100000);


    if (result != SDL_PASS ) {
        UART_printf("\n B0TCM0 Bank1 Single bit error self test at pErrMem 0x%p test failed",
                    injectErrorConfig.pErrMem);
        retVal = -1;
    } else {
        UART_printf("\n B0TCM0 Bank1 Single bit error self test at pErrMem 0x%p: test complete",
                    injectErrorConfig.pErrMem);
    }

    return retVal;
}

/*********************************************************************
 * @fn      ECC_Test_runECC2BitB0TCM0Bank1SelfTest
 *
 * @brief   Execute ECC Double bit error test on BTCM0 Bank 1 memory
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t ECC_Test_runECC2BitB0TCM0Bank1SelfTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;

    UART_printf("\n B0TCM0 Bank1 Double bit error self test: starting");

    /* Run one shot test for BTCM0 2 bit error */
    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0x41010204u);

    injectErrorConfig.flipBitMask = 0x201;
    result = SDL_ECC_selfTest(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
                              SDL_ECC_R5F_MEM_SUBTYPE_B0TCM0_BANK1_VECTOR_ID,
                              SDL_INJECT_ECC_ERROR_FORCING_2BIT_ONCE,
                              &injectErrorConfig,
                              1000);


    if (result != SDL_PASS ) {
        UART_printf("\n B0TCM0 Bank1 Double bit error self test: at pErrMem 0x%p: fixed location once test failed",
                    injectErrorConfig.pErrMem);
        retVal = -1;
    } else {
        UART_printf("\n B0TCM0 Bank1 Double bit error self test: pErrMem 0x%p fixed location once test complete",
                    injectErrorConfig.pErrMem);
    }

    return retVal;
}

/*********************************************************************
 * @fn      ECC_Test_runECC1BitB1TCM0Bank0SelfTest
 *
 * @brief   Execute ECC Double bit error test on B1TCM0 Bank 0 memory
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t ECC_Test_runECC1BitB1TCM0Bank0SelfTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;

    UART_printf("\n B1TCM0 Bank0 Single bit error self test: starting");

     /* Note the address is relative to start of ram */
     injectErrorConfig.pErrMem = (uint32_t *)(0x41010108u);

     /* Run one shot test for BTCM0 1 bit error */
     injectErrorConfig.flipBitMask = 0x20;
     result = SDL_ECC_selfTest(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
                               SDL_ECC_R5F_MEM_SUBTYPE_B1TCM0_BANK0_VECTOR_ID,
                               SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE,
                               &injectErrorConfig,
                               100000);


     if (result != SDL_PASS ) {
         UART_printf("\n B1TCM0 Bank0 Single bit error self test at pErrMem 0x%p test failed",
                     injectErrorConfig.pErrMem);
        retVal = -1;
    } else {
        UART_printf("\n B1TCM0 Bank0 Single bit error self test at pErrMem 0x%p: test complete",
                    injectErrorConfig.pErrMem);
    }

    return retVal;
}

/*********************************************************************
 * @fn      ECC_Test_runECC2BitB1TCM0Bank0SelfTest
 *
 * @brief   Execute ECC Double bit error test on B1TCM0 Bank 0 memory
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t ECC_Test_runECC2BitB1TCM0Bank0SelfTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;

    UART_printf("\n B1TCM0 Bank0 Double bit error self test: starting");

    /* Run one shot test for BTCM0 2 bit error */
    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0x41010208u);

    injectErrorConfig.flipBitMask = 0x201;
    result = SDL_ECC_selfTest(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
                              SDL_ECC_R5F_MEM_SUBTYPE_B1TCM0_BANK0_VECTOR_ID,
                              SDL_INJECT_ECC_ERROR_FORCING_2BIT_ONCE,
                              &injectErrorConfig,
                              1000);


    if (result != SDL_PASS ) {
        UART_printf("\n B1TCM0 Bank0 Double bit error self test: at pErrMem 0x%p: fixed location once test failed",
                    injectErrorConfig.pErrMem);
       retVal = -1;
    } else {
        UART_printf("\n B1TCM0 Bank0 Double bit error self test: pErrMem 0x%p fixed location once test complete",
                    injectErrorConfig.pErrMem);
    }

    return retVal;
}

/*********************************************************************
 * @fn      ECC_Test_runECC1BitB1TCM0Bank1SelfTest
 *
 * @brief   Execute ECC Single bit error test on B1TCM0 Bank 1 memory
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t ECC_Test_runECC1BitB1TCM0Bank1SelfTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;

    UART_printf("\n B1TCM0 Bank1 Single bit error self test: starting");

    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0x4101010cu);

    /* Run one shot test for BTCM0 1 bit error */
    injectErrorConfig.flipBitMask = 0x20;
    result = SDL_ECC_selfTest(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
                              SDL_ECC_R5F_MEM_SUBTYPE_B1TCM0_BANK1_VECTOR_ID,
                              SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE,
                              &injectErrorConfig,
                              100000);


    if (result != SDL_PASS ) {
        UART_printf("\n B1TCM0 Bank1 Single bit error self test at pErrMem 0x%p test failed",
                    injectErrorConfig.pErrMem);
       retVal = -1;
    } else {
        UART_printf("\n B1TCM0 Bank1 Single bit error self test at pErrMem 0x%p: test complete",
                    injectErrorConfig.pErrMem);
    }

    return retVal;
}

/*********************************************************************
 * @fn      ECC_Test_runECC2BitB1TCM0Bank1SelfTest
 *
 * @brief   Execute ECC Single bit error test on B1TCM0 Bank 1 memory
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t ECC_Test_runECC2BitB1TCM0Bank1SelfTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;

    UART_printf("\n B1TCM0 Bank1 Double bit error self test: starting");

    /* Run one shot test for BTCM0 2 bit error */
    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0x4101020cu);

    injectErrorConfig.flipBitMask = 0x201;
    result = SDL_ECC_selfTest(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
                              SDL_ECC_R5F_MEM_SUBTYPE_B1TCM0_BANK1_VECTOR_ID,
                              SDL_INJECT_ECC_ERROR_FORCING_2BIT_ONCE,
                              &injectErrorConfig,
                              1000);


    if (result != SDL_PASS ) {
        UART_printf("\n B1TCM0 Bank1 Double bit error self test: at pErrMem 0x%p: fixed location once test failed",
                    injectErrorConfig.pErrMem);
        retVal = -1;
    } else {
        UART_printf("\n B1TCM0 Bank1 Double bit error self test: pErrMem 0x%p fixed location once test complete",
                    injectErrorConfig.pErrMem);
    }

    return retVal;
}


/*********************************************************************
 * @fn      ECC_Test_runECC1BitCBASSSelfTest
 *
 * @brief   Execute ECC Single bit error self test on CBASS ECC aggregator
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t ECC_Test_runECC1BitCBASSSelfTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;
    uint32_t subType,mainMem;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;

    UART_printf("\n CBASS Single bit error self test: starting");

    memset(&injectErrorConfig, 0, sizeof(injectErrorConfig));

    /* Run one shot test for CBASS 2 bit error */
    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0u);

    injectErrorConfig.flipBitMask = 0x1;
    injectErrorConfig.chkGrp = 0x0;
#if defined(SOC_J721E)	
	mainMem = SDL_ECC_MEMTYPE_MCU_CBASS_ECC_AGGR0;
    subType = SDL_MCU_CBASS_ECC_AGGR0_IMCU_COR_FW_VBUSP_32B_SOC_FW_SAFEG_EDC_CTRL_RAM_ID;
#elif defined (SOC_J7200)
	mainMem = SDL_ECC_VC_MAIN_HC_ECC_AGGR5_ECC_AGGR;
    subType = SDL_VC_MAIN_HC_ECC_AGGR5_J7VC_HC2_CBASS_J7VC_RC_TO_HC2_VBUSM_M2M_BRIDGE_J7VC_HC2_CBASS_J7VC_RC_TO_HC2_VBUSM_M2M_BRIDGE_SRC_EDC_CTRL_BUSECC_RAM_ID;
#elif defined (SOC_J721S2)
	mainMem = SDL_WKUP_VTM0_ECC_AGGR;
    subType = SDL_WKUP_VTM0_K3VTM_N16FFC_ECCAGGR_K3VTM_N16FFC_CFG_CBASS_VBUSP_P2P_BRIDGE_EDC_CTRL_0_RAM_ID;
#elif defined (SOC_J784S4)
	mainMem = SDL_WKUP_VTM0_ECC_AGGR;
    subType = SDL_WKUP_VTM0_K3VTM_N16FFC_ECCAGGR_K3VTM_N16FFC_CFG_CBASS_VBUSP_P2P_BRIDGE_EDC_CTRL_0_RAM_ID;
#endif
    result = SDL_ECC_selfTest(mainMem,
                              subType,
                              SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE,
                              &injectErrorConfig,
                              1000);


    if (result != SDL_PASS ) {
        UART_printf("\n CBASS  Single bit error self test: Subtype %d: test failed",
                    subType);
        retVal = -1;
    } else {
        UART_printf("\n CBASS Single bit error self test: Subtype 0x%p test complete",
                    subType);
    }
    return retVal;
}

/*********************************************************************
 * @fn      ECC_Test_runECC1BitCBASSInjectTest
 *
 * @brief   Execute ECC Single bit error inject test on CBASS ECC aggregator
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t ECC_Test_runECC1BitCBASSInjectTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;
    uint32_t subType, mainMem;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;

    memset(&injectErrorConfig, 0, sizeof(injectErrorConfig));

    UART_printf("\n CBASS Single bit error inject test: starting");

    /* Run one shot test for CBASS 2 bit error */
    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0u);

    injectErrorConfig.flipBitMask = 0x1;
    injectErrorConfig.chkGrp = 0x0;
#if defined(SOC_J721E)	
	mainMem = SDL_ECC_MEMTYPE_MCU_CBASS_ECC_AGGR0;
    subType = SDL_MCU_CBASS_ECC_AGGR0_IMCU_COR_FW_VBUSP_32B_SOC_FW_SAFEG_EDC_CTRL_RAM_ID;
#elif defined (SOC_J7200)
	mainMem = SDL_ECC_VC_MAIN_HC_ECC_AGGR5_ECC_AGGR;
    subType = SDL_VC_MAIN_HC_ECC_AGGR5_J7VC_HC2_CBASS_J7VC_RC_TO_HC2_VBUSM_M2M_BRIDGE_J7VC_HC2_CBASS_J7VC_RC_TO_HC2_VBUSM_M2M_BRIDGE_SRC_EDC_CTRL_BUSECC_RAM_ID;
#elif defined (SOC_J721S2)
	mainMem = SDL_WKUP_VTM0_ECC_AGGR;
    subType = SDL_WKUP_VTM0_K3VTM_N16FFC_ECCAGGR_K3VTM_N16FFC_CFG_CBASS_VBUSP_P2P_BRIDGE_EDC_CTRL_0_RAM_ID;
#elif defined (SOC_J784S4)
	mainMem = SDL_WKUP_VTM0_ECC_AGGR;
    subType = SDL_WKUP_VTM0_K3VTM_N16FFC_ECCAGGR_K3VTM_N16FFC_CFG_CBASS_VBUSP_P2P_BRIDGE_EDC_CTRL_0_RAM_ID;
#endif
    result = SDL_ECC_injectError(mainMem,
                              subType,
                              SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE,
                              &injectErrorConfig
                              );


    if (result != SDL_PASS ) {
        UART_printf("\n CBASS  Single bit error inject test: Subtype %d: test failed",
                    subType);
        retVal = -1;
    } else {
        UART_printf("\n CBASS Single bit error inject test: Subtype 0x%p test complete",
                    subType);
    }

    return retVal;
}


/*********************************************************************
 * @fn      ECC_Test_runECC2BitCBASSSelfTest
 *
 * @brief   Execute ECC Double bit error self test on CBASS ECC aggregator
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t ECC_Test_runECC2BitCBASSSelfTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;
    uint32_t subType, mainMem;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;

    UART_printf("\n CBASS Double bit error self test: starting");

    memset(&injectErrorConfig, 0, sizeof(injectErrorConfig));

    /* Run one shot test for CBASS 2 bit error */
    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0u);

    injectErrorConfig.flipBitMask = 0x3;
    injectErrorConfig.chkGrp = 0x4;

#if defined(SOC_J721E)	
	mainMem = SDL_ECC_MEMTYPE_MCU_CBASS_ECC_AGGR0;
    subType = SDL_MCU_CBASS_ECC_AGGR0_IMCU_COR_FW_VBUSP_32B_SOC_FW_SAFEG_EDC_CTRL_RAM_ID;
#elif defined (SOC_J7200)
	mainMem = SDL_ECC_MCU_VC_MCU_ECC_AGGR0;
    subType = SDL_MCU_VC_MCU_ECC_AGGR0_IMCU_COR_FW_VBUSP_32B_SOC_FW_SAFEG_EDC_CTRL_RAM_ID;
#elif defined (SOC_J721S2)
	mainMem = SDL_MCUM_MCU_ECC_AGGR0_ECC_AGGR;
    subType = SDL_MCUM_MCU_ECC_AGGR0_J7AM_MCU_ECC_AGGR_IMCU_COR_FW_VBUSP_32B_SOC_FW_SAFEG_EDC_CTRL_RAM_ID;
#elif defined (SOC_J784S4)
	mainMem = SDL_MCUM_MCU_ECC_AGGR0_ECC_AGGR;
    subType = SDL_MCUM_MCU_ECC_AGGR0_J7AM_MCU_ECC_AGGR_IMCU_COR_FW_VBUSP_32B_SOC_FW_SAFEG_EDC_CTRL_RAM_ID;
#endif
    result = SDL_ECC_selfTest(mainMem,
                              subType,
                              SDL_INJECT_ECC_ERROR_FORCING_2BIT_ONCE,
                              &injectErrorConfig,
							  10000000);


    if (result != SDL_PASS ) {
        UART_printf("\n CBASS  Double bit error self test: Subtype %d: fixed location once test failed",
                    subType);
        retVal = -1;
    } else {
        UART_printf("\n CBASS Double bit error self test: Subtype 0x%p fixed location once test complete",
                    subType);
    }

    return retVal;
}

/*********************************************************************
 * @fn      ECC_Test_runECC2BitCBASSInjectTest
 *
 * @brief   Execute ECC Doule bit error inject test on CBASS ECC aggregator
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t ECC_Test_runECC2BitCBASSInjectTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;
    uint32_t subType, mainMem;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;

    memset(&injectErrorConfig, 0, sizeof(injectErrorConfig));

    UART_printf("\n CBASS Double bit error inject test: starting");

    /* Run one shot test for CBASS 2 bit error */
    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0u);

    injectErrorConfig.flipBitMask = 0x5;
    injectErrorConfig.chkGrp = 0x4;
	
#if defined(SOC_J721E)	
	mainMem = SDL_ECC_MEMTYPE_MCU_CBASS_ECC_AGGR0;
    subType = SDL_MCU_CBASS_ECC_AGGR0_IMCU_COR_FW_VBUSP_32B_SOC_FW_SAFEG_EDC_CTRL_RAM_ID;
#elif defined (SOC_J7200)
	mainMem = SDL_ECC_MCU_VC_MCU_ECC_AGGR0;
    subType = SDL_MCU_VC_MCU_ECC_AGGR0_IMCU_COR_FW_VBUSP_32B_SOC_FW_SAFEG_EDC_CTRL_RAM_ID;
#elif defined (SOC_J721S2)
	mainMem = SDL_MCUM_MCU_ECC_AGGR0_ECC_AGGR;
    subType = SDL_MCUM_MCU_ECC_AGGR0_J7AM_MCU_ECC_AGGR_IMCU_COR_FW_VBUSP_32B_SOC_FW_SAFEG_EDC_CTRL_RAM_ID;
#elif defined (SOC_J784S4)
	mainMem = SDL_MCUM_MCU_ECC_AGGR0_ECC_AGGR;
    subType = SDL_MCUM_MCU_ECC_AGGR0_J7AM_MCU_ECC_AGGR_IMCU_COR_FW_VBUSP_32B_SOC_FW_SAFEG_EDC_CTRL_RAM_ID;
#endif

    result = SDL_ECC_injectError(mainMem,
                              subType,
                              SDL_INJECT_ECC_ERROR_FORCING_2BIT_ONCE,
                              &injectErrorConfig);


    if (result != SDL_PASS ) {
        UART_printf("\n CBASS  Double bit error inject test: Subtype %d: test failed",
                    subType);
        retVal = -1;
    } else {
        UART_printf("\n CBASS Double bit error inject test: Subtype 0x%p test complete",
                    subType);
    }

    return retVal;
}

/*********************************************************************
 * @fn      ECC_Test_runECC1BitMSMCRAMSelfTest
 *
 * @brief   Execute ECC Single bit error self test on MSMC_BUSECC_RAM aggregator
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
#if defined (SOC_J721E) || (SOC_J7200) || (SOC_J721S2) || (SOC_J784S4)
int32_t ECC_Test_runECC1BitMSMCRAMSelfTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;
    uint32_t subType;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;

    UART_printf("\n SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0 Single bit error self test: starting");

    memset(&injectErrorConfig, 0, sizeof(injectErrorConfig));

    /* Run one shot test for SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0 1 bit error */
    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0u);

    injectErrorConfig.flipBitMask = 0x1;
    injectErrorConfig.chkGrp = 0x4;
#if defined(SOC_J721E)	
    subType = SDL_COMPUTE_CLUSTER0_MSMC_ECC_AGGR0_MSMC_MMR_BUSECC_RAM_ID;
#elif defined (SOC_J7200)
    subType = SDL_COMPUTE_CLUSTER0_MSMC_MMR_BUSECC_RAM_ID;
#elif defined (SOC_J721S2)
    subType = SDL_COMPUTE_CLUSTER0_COMPUTE_CLUSTER_J7AE_MSMC_EN_DRU_MSMC_EN_4MB_WRAP_MSMC_4MB_MSMC_WRAP_ECC_AGGR0_DRU0_CBASS_MMR_FW_CH_EDC_RAM_ID;
#elif defined (SOC_J784S4)
    subType = SDL_COMPUTE_CLUSTER0_COMPUTE_CLUSTER_J7AHP_MSMC_EN_DRU_MSMC_EN_8MB_WRAP_MSMC_8MB_MSMC_WRAP_ECC_AGGR0_MSMC_LAMBDA3_WRAP_MSMC_QOR_MSMC_CORE_EMIF_PIPE_EMIF0_DP_TO_VBUSMC_EDC_CTRL_0_RAM_ID;
#endif
    result = SDL_ECC_selfTest(SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0,
                              subType,
                              SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE,
                              &injectErrorConfig,
                              1000);


    if (result != SDL_PASS ) {
        UART_printf("\n SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0  Single bit error self test: Subtype %d: test failed",
                    subType);
        retVal = -1;
    } else {
        UART_printf("\n SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0 Single bit error self test: Subtype 0x%p test complete",
                    subType);
    }

    return retVal;
}
#endif

/*********************************************************************
 * @fn      ECC_Test_runECC2BitMSMCRAMSelfTest
 *
 * @brief   Execute ECC Double bit error self test on MSMC_BUSECC_RAM ECC aggregator
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t ECC_Test_runECC2BitMSMCRAMSelfTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;
    uint32_t subType, mainMem;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;

    UART_printf("\n MSMC_BUSECC_RAM Double bit error self test: starting");

    memset(&injectErrorConfig, 0, sizeof(injectErrorConfig));
	/* Run one shot test for MSMC_BUSECC_RAM 2 bit error */
    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0u);

    
#if defined(SOC_J721E)	
    injectErrorConfig.flipBitMask = 0x3;
    injectErrorConfig.chkGrp = 0x1;
	mainMem = SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0;
    subType = SDL_COMPUTE_CLUSTER0_MSMC_ECC_AGGR0_MSMC_MMR_BUSECC_RAM_ID;
#elif defined (SOC_J7200)
    injectErrorConfig.flipBitMask = 0x3;
    injectErrorConfig.chkGrp = 1u;
	mainMem = SDL_ECC_NAVSS0_1_ECC_AGGR;
    subType = SDL_NAVSS0_NAVSS256VCL_UDMASS_UDMASS_ECCAGGR0_EDC_CTRL_RAM_ID;
#elif defined (SOC_J721S2)
    injectErrorConfig.flipBitMask = 0x3;
    injectErrorConfig.chkGrp = 1u;
	mainMem = SDL_NAVSS0_1_ECC_AGGR;
    subType = SDL_NAVSS0_NAVSS512J7AM_MODSS_ECCAGGR0_NAVSS512J7AM_MODSS_ECCAGGR0_EDC_CTRL_RAM_ID;	
#elif defined (SOC_J784S4)
    injectErrorConfig.flipBitMask = 0x3;
    injectErrorConfig.chkGrp = 1u;
	mainMem = SDL_NAVSS0_1_ECC_AGGR;
	subType = SDL_NAVSS0_NAVSS512J7AM_MODSS_ECCAGGR0_NAVSS512J7AM_MODSS_ECCAGGR0_EDC_CTRL_RAM_ID;
#endif

    result = SDL_ECC_selfTest(mainMem,
                              subType,
                              SDL_INJECT_ECC_ERROR_FORCING_2BIT_ONCE,
                              &injectErrorConfig,
							  10000);			  

	#if defined(SOC_J721E)	
    if (result != SDL_PASS ) {
        UART_printf("\n SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0  Double bit error self test: Subtype %d: fixed location once test failed",
                    subType);
        retVal = -1;
    } else {
        UART_printf("\n SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0 Double bit error self test: Subtype 0x%p fixed location once test complete",
                    subType);
    }
	#elif defined (SOC_J7200)
	if (result != SDL_PASS ) {
        UART_printf("\n SDL_ECC_NAVSS0_1_ECC_AGGR  Double bit error self test: Subtype %d: fixed location once test failed",
                    subType);
        retVal = -1;
    } else {
        UART_printf("\n SDL_ECC_NAVSS0_1_ECC_AGGR Double bit error self test: Subtype 0x%p fixed location once test complete",
                    subType);
    }
	#elif defined (SOC_J721S2)
	if (result != SDL_PASS ) {
        UART_printf("\n SDL_NAVSS0_1_ECC_AGGR  Double bit error self test: Subtype %d: fixed location once test failed",
                    subType);
        retVal = -1;
    } else {
        UART_printf("\n SDL_NAVSS0_1_ECC_AGGR Double bit error self test: Subtype 0x%p fixed location once test complete",
                    subType);
    }
	#elif defined (SOC_J784S4)
	if (result != SDL_PASS ) {
        UART_printf("\n SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0  Double bit error self test: Subtype %d: fixed location once test failed",
                    subType);
        retVal = -1;
    } else {
        UART_printf("\n SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0 Double bit error self test: Subtype 0x%p fixed location once test complete",
                    subType);
    }
	#endif

    return retVal;
}

#if defined(SOC_J721E)	
/*********************************************************************
 * @fn      ECC_Test_runECC1BitMSMC_CACHE_selfTest
 *
 * @brief   Execute ECC Single bit error self test on CACHE_TAG_PIPE_BUSECC aggregator
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t ECC_Test_runECC1BitMSMC_CACHE_selfTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;
    uint32_t subType;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;

    UART_printf("\n CACHE_TAG_PIPE_BUSECC Single bit error self test: starting");

    memset(&injectErrorConfig, 0, sizeof(injectErrorConfig));

    /* Run one shot test for CACHE_TAG_PIPE_BUSECC 1 bit error */
    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0u);
	

    injectErrorConfig.flipBitMask = 0x1;
    injectErrorConfig.chkGrp = 0x1;
    subType = SDL_COMPUTE_CLUSTER0_MSMC_ECC_AGGR0_RMW2_CACHE_TAG_PIPE_BUSECC_RAM_ID;


    result = SDL_ECC_selfTest(SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0,
                              subType,
                              SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE,
                              &injectErrorConfig,
                              1000);


    if (result != SDL_PASS ) {
        UART_printf("\n CACHE_TAG_PIPE_BUSECC  Single bit error self test: Subtype %d: test failed",
                    subType);
        retVal = -1;
    } else {
        UART_printf("\n CACHE_TAG_PIPE_BUSECC Single bit error self test: Subtype 0x%p test complete",
                    subType);
    }

    return retVal;
}


/*********************************************************************
 * @fn      ECC_Test_runECC2BitMSMC_CACHE_selfTest
 *
 * @brief   Execute ECC Double bit error self test on CACHE_TAG_PIPE_BUSECC ECC aggregator
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t ECC_Test_runECC2BitMSMC_CACHE_selfTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;
    uint32_t subType;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;

    UART_printf("\n CACHE_TAG_PIPE_BUSECC Double bit error self test: starting");

    memset(&injectErrorConfig, 0, sizeof(injectErrorConfig));
	/* Run one shot test for CACHE_TAG_PIPE_BUSECC 2 bit error */
    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0u);

	injectErrorConfig.flipBitMask = 0x5;
    injectErrorConfig.chkGrp = 0x1;
    subType = SDL_COMPUTE_CLUSTER0_MSMC_ECC_AGGR0_RMW2_CACHE_TAG_PIPE_BUSECC_RAM_ID;

    result = SDL_ECC_selfTest(SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0,
                              subType,
                              SDL_INJECT_ECC_ERROR_FORCING_2BIT_ONCE,
                              &injectErrorConfig,
                              1000);

    if (result != SDL_PASS ) {
        UART_printf("\n CACHE_TAG_PIPE_BUSECC  Double bit error self test: Subtype %d: fixed location once test failed",
                    subType);
        retVal = -1;
    } else {
        UART_printf("\n CACHE_TAG_PIPE_BUSECC Double bit error self test: Subtype 0x%p fixed location once test complete",
                    subType);
    }

    return retVal;
}

/*********************************************************************
 * @fn      ECC_Test_runECC1BitMSMCCLECselfTest
 *
 * @brief   Execute ECC Single bit error self test on CLEC_EDC_CTRL_BUSECC aggregator
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t ECC_Test_runECC1BitMSMCCLECselfTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;
   uint32_t subType;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;

    UART_printf("\n CLEC_EDC_CTRL_BUSECC Single bit error self test: starting");

    memset(&injectErrorConfig, 0, sizeof(injectErrorConfig));

    /* Run one shot test for CLEC_EDC_CTRL_BUSECC 1 bit error */
    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0u);

    injectErrorConfig.flipBitMask = 0x1;
    injectErrorConfig.chkGrp = 0x0;
	
    subType = SDL_COMPUTE_CLUSTER0_MSMC_ECC_AGGR0_CLEC_J7ES_CLEC_EDC_CTRL_BUSECC_RAM_ID;

    result = SDL_ECC_selfTest(SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0,
                              subType,
                              SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE,
                              &injectErrorConfig,
                              1000);


    if (result != SDL_PASS ) {
        UART_printf("\n CLEC_EDC_CTRL_BUSECC  Single bit error self test: Subtype %d: test failed",
                    subType);
        retVal = -1;
    } else {
        UART_printf("\n CLEC_EDC_CTRL_BUSECC Single bit error self test: Subtype 0x%p test complete",
                    subType);
    }

    return retVal;
}

/*********************************************************************
 * @fn      ECC_Test_runECC2BitMSMCCLECselfTest
 *
 * @brief   Execute ECC Double bit error self test on CLEC_EDC_CTRL_BUSECC ECC aggregator
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t ECC_Test_runECC2BitMSMCCLECselfTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;
   uint32_t subType;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;

    UART_printf("\n CLEC_EDC_CTRL_BUSECC Double bit error self test: starting");

    memset(&injectErrorConfig, 0, sizeof(injectErrorConfig));
	/* Run one shot test for CLEC_EDC_CTRL_BUSECC 2 bit error */
    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0u);
	
	injectErrorConfig.flipBitMask = 0x5;
    injectErrorConfig.chkGrp = 0x0;
    subType = SDL_COMPUTE_CLUSTER0_MSMC_ECC_AGGR0_CLEC_J7ES_CLEC_EDC_CTRL_BUSECC_RAM_ID;

    result = SDL_ECC_selfTest(SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0,
                              subType,
                              SDL_INJECT_ECC_ERROR_FORCING_2BIT_ONCE,
                              &injectErrorConfig,
                              1000);

    if (result != SDL_PASS ) {
        UART_printf("\n CLEC_EDC_CTRL_BUSECC  Double bit error self test: Subtype %d: fixed location once test failed",
                    subType);
        retVal = -1;
    } else {
        UART_printf("\n CLEC_EDC_CTRL_BUSECC Double bit error self test: Subtype 0x%p fixed location once test complete",
                    subType);
    }

    return retVal;
}
#endif
static bool ECC_Test_ECC_DEDTriggerFlag = false;

/*********************************************************************
 *
 * @brief   indicate DED trigger
 *
 *
 * @return  0 : Success; < 0 for failures
 */
void ECC_Test_ECC_indicateDEDTrigger(void)
{
    ECC_Test_ECC_DEDTriggerFlag = true;
}


int32_t ECC_Test_ECC_waitDEDTrigger(uint64_t timeCount)
{
    int32_t retVal = 0;
    uint32_t startTimeStamp, curTimeStamp, lastTimeStamp, deltaTime;
    uint64_t elapsedTime = 0u;

    startTimeStamp = SDL_getTime();
    lastTimeStamp = startTimeStamp;
    while(ECC_Test_ECC_DEDTriggerFlag != true) {
        curTimeStamp = SDL_getTime();
        deltaTime = curTimeStamp-lastTimeStamp;
        lastTimeStamp = curTimeStamp;
        elapsedTime += deltaTime;
        if (elapsedTime > timeCount) {
            break;
        }
    }

    if (ECC_Test_ECC_DEDTriggerFlag == true ) {
        ECC_Test_ECC_DEDTriggerFlag = false;
    } else {
        retVal = -1;
    }
    return retVal;
}

/*********************************************************************
 * @fn      ECC_Test_runECC2BitCodeInjectTest
 *
 * @brief   Execute ECC 2 bit Inject error on program section
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t ECC_Test_runECC2BitCodeInjectTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;

    UART_printf("\n ATCM Double bit Program section error inject: starting");

    /* Error injection on program memory */
    injectErrorConfig.pErrMem = (uint32_t *)(&ECC_Test_ECC_dummyFunction);

    injectErrorConfig.flipBitMask = 0x101;

    result = SDL_ECC_injectError(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
                                 SDL_ECC_R5F_MEM_SUBTYPE_ATCM0_BANK0_VECTOR_ID,
                                 SDL_INJECT_ECC_ERROR_FORCING_2BIT_ONCE,
                                 &injectErrorConfig);


    /* Call progrem for the ECC error to take effect */
    ECC_Test_ECC_dummyFunction();

    if (result != SDL_PASS ) {
        UART_printf("\n ATCM Double bit Program section error inject: at pErrMem 0x%p: fixed location once test failed",
                    injectErrorConfig.pErrMem);
       retVal = -1;
    } else {
        UART_printf("\n ATCM Double bit Program section error inject: pErrMem 0x%p fixed location once test complete",
                    injectErrorConfig.pErrMem);
    }

    return retVal;
}

int32_t ECC_Test_runECC_ErrMemInjectTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;
    volatile uint32_t testLocationValue;

    UART_printf("\n Error inject: EFAIL test starting");

    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0x00);

    /* Run one shot test for ATCM 1 bit error */
    injectErrorConfig.flipBitMask = 0x10;
    result = SDL_ECC_injectError(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
                                 SDL_ECC_R5F_MEM_SUBTYPE_B0TCM0_BANK0_VECTOR_ID,
                                 SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE,
                                 &injectErrorConfig);


    if (result != SDL_EFAIL ) {
        UART_printf("\n Error inject at pErrMem 0x%p EFAIL test failed",
                    injectErrorConfig.pErrMem);
        retVal = -1;
    } else {
        /* Access the memory where injection is expected */
        testLocationValue = injectErrorConfig.pErrMem[0];

        UART_printf("\n Error inject at pErrMem 0x%p: EFAIL test complete",
                    injectErrorConfig.pErrMem);
    }

    return retVal;
}

int32_t ECC_Test_runECC1BitNrowInjectTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;
    volatile uint32_t testLocationValue;

    UART_printf("\n ATCM Single bit N row error inject: test starting");

    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0x00);

    /* Run one shot test for ATCM 1 bit N row error */
    injectErrorConfig.flipBitMask = 0x10;
    result = SDL_ECC_injectError(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
                                 SDL_ECC_R5F_MEM_SUBTYPE_ATCM0_BANK0_VECTOR_ID,
                                 SDL_INJECT_ECC_ERROR_FORCING_1BIT_N_ROW_ONCE,
                                 &injectErrorConfig);


    if (result != SDL_PASS ) {
        UART_printf("\n ATCM Single bit N row error inject at pErrMem 0x%p test failed",
                    injectErrorConfig.pErrMem);
        retVal = -1;
    } else {
        /* Access the memory where injection is expected */
        testLocationValue = injectErrorConfig.pErrMem[0];

        UART_printf("\n ATCM Single bit N row error inject at pErrMem 0x%p: test complete",
                    injectErrorConfig.pErrMem);
    }

    return retVal;
}

int32_t ECC_Test_runECC1BitRepeatInjectTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;
    volatile uint32_t testLocationValue;

    UART_printf("\n ATCM Single bit Repeat error inject: test starting");

    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0x00);

    /* Run one shot test for ATCM 1 bit Repeat error */
    injectErrorConfig.flipBitMask = 0x10;
    result = SDL_ECC_injectError(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
                                 SDL_ECC_R5F_MEM_SUBTYPE_ATCM0_BANK0_VECTOR_ID,
                                 SDL_INJECT_ECC_ERROR_FORCING_1BIT_REPEAT,
                                 &injectErrorConfig);


    if (result != SDL_PASS ) {
        UART_printf("\n ATCM Single bit Repeat error inject at pErrMem 0x%p test failed",
                    injectErrorConfig.pErrMem);
        retVal = -1;
    } else {
        /* Access the memory where injection is expected */
        testLocationValue = injectErrorConfig.pErrMem[0];

        UART_printf("\n ATCM Single bit Repeat error inject at pErrMem 0x%p: test complete",
                    injectErrorConfig.pErrMem);
    }

    return retVal;
}

int32_t ECC_Test_runECC2BitNrowInjectTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;
    volatile uint32_t testLocationValue;

    UART_printf("\n ATCM Double bit N row error inject: test starting");

    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0x00);

    /* Run one shot test for ATCM 2 bit N row error */
    injectErrorConfig.flipBitMask = 0x101;
    result = SDL_ECC_injectError(SDL_ECC_MEMTYPE_MCU_R5F0_CORE,
                                 SDL_ECC_R5F_MEM_SUBTYPE_ATCM0_BANK0_VECTOR_ID,
                                 SDL_INJECT_ECC_ERROR_FORCING_2BIT_N_ROW_ONCE,
                                 &injectErrorConfig);


    if (result != SDL_PASS ) {
        UART_printf("\n ATCM Double bit N row error inject at pErrMem 0x%p test failed",
                    injectErrorConfig.pErrMem);
        retVal = -1;
    } else {
        /* Access the memory where injection is expected */
        testLocationValue = injectErrorConfig.pErrMem[0];

        UART_printf("\n ATCM Double bit N row error inject at pErrMem 0x%p: test complete",
                    injectErrorConfig.pErrMem);
    }

    return retVal;
}

#if defined (SOC_J7200) 
int32_t ECC_Test_runECC2BitRepeatInjectTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;
   uint32_t subType, mainMem;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;

    UART_printf("\n NAVSS0_1 Double bit repeat error inject: starting");

    memset(&injectErrorConfig, 0, sizeof(injectErrorConfig));
	/* Run one shot test for NAVSS0_1 2 bit error */
    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0u);

	injectErrorConfig.flipBitMask = 0x3;
	mainMem = SDL_ECC_NAVSS0_1_ECC_AGGR;
    subType = SDL_NAVSS0_NAVSS256VCL_UDMASS_UDMAP0_TPCFG_PSTATE_RAM_ID;

    result = SDL_ECC_injectError(mainMem,
                                subType,
                                SDL_INJECT_ECC_ERROR_FORCING_2BIT_REPEAT,
                                 &injectErrorConfig);

    if (result != SDL_PASS ) {
        UART_printf("\n NAVSS0_1  Double bit repeat error inject: Subtype %d: fixed location once test failed",
                    subType);
        retVal = -1;
    } else {
        UART_printf("\n NAVSS0_1 Double bit repeat error inject: Subtype 0x%p fixed location once test complete",
                    subType);
    }

    return retVal;
}
#endif

#if defined (SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J784S4)
int32_t ECC_Test_runECC2BitRepeatInjectTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;
   uint32_t subType, mainMem;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;

    UART_printf("\n MSMC_BUSECC_RAM Double bit repeat error inject: starting");

    memset(&injectErrorConfig, 0, sizeof(injectErrorConfig));
	/* Run one shot test for MSMC_BUSECC_RAM 2 bit error */
    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0u);
	
	injectErrorConfig.flipBitMask = 0x5;
	
	#if defined (SOC_J721E)
    subType = SDL_COMPUTE_CLUSTER0_MSMC_ECC_AGGR0_DRU_CBASS_MMR_CFG_EDC_RAM_ID;
	mainMem = SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0;
	
	#elif defined (SOC_J721S2)
	subType = SDL_COMPUTE_CLUSTER0_COMPUTE_CLUSTER_J7AE_MSMC_EN_DRU_MSMC_EN_4MB_WRAP_MSMC_4MB_MSMC_WRAP_ECC_AGGR0_DRU0_CBASS_MMR_CFG_EDC_RAM_ID;
	mainMem = SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0;
	
	#elif defined (SOC_J784S4)
	subType = SDL_COMPUTE_CLUSTER0_COMPUTE_CLUSTER_J7AHP_MSMC_EN_DRU_MSMC_EN_8MB_WRAP_MSMC_8MB_MSMC_WRAP_ECC_AGGR0_DRU0_CBASS_MMR_CFG_EDC_RAM_ID;
	mainMem = SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0;
	
	#endif

    result = SDL_ECC_injectError(mainMem,
                                subType,
                                SDL_INJECT_ECC_ERROR_FORCING_2BIT_REPEAT,
                                 &injectErrorConfig);

    if (result != SDL_PASS ) {
        UART_printf("\n MSMC_BUSECC_RAM  Double bit repeat error inject: Subtype %d: fixed location once test failed",
                    subType);
        retVal = -1;
    } else {
        UART_printf("\n MSMC_BUSECC_RAM Double bit repeat error inject: Subtype 0x%p fixed location once test complete",
                    subType);
    }

    return retVal;
}
#endif

 /*********************************************************************
 * @fn      ECC_Test_runECCSEC_DED_MSMCRAMSelfTest
 *
 * @brief   Execute ECC Single bit error self test on MSMC_BUSECC_RAM aggregator
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
#if defined (SOC_J721E) || (SOC_J7200) || (SOC_J721S2)|| (SOC_J784S4)
int32_t ECC_Test_runECCSEC_DED_MSMCRAMSelfTest(void)
{
    SDL_ErrType_t result;
    int32_t retVal=0;
    uint32_t subType, i;

    SDL_ECC_InjectErrorConfig_t injectErrorConfig;

    UART_printf("\n MSMC_BUSECC_RAM Single bit error self test: starting");

    memset(&injectErrorConfig, 0, sizeof(injectErrorConfig));

    /* Run one shot test for MSMC_BUSECC_RAM 1 bit error */
    /* Note the address is relative to start of ram */
    injectErrorConfig.pErrMem = (uint32_t *)(0u);

    injectErrorConfig.flipBitMask = 0x5;
#if defined(SOC_J721E)	
    subType = SDL_COMPUTE_CLUSTER0_MSMC_ECC_AGGR0_DRU_CBASS_MMR_CFG_EDC_RAM_ID;
#elif defined (SOC_J7200)
    subType = SDL_COMPUTE_CLUSTER0_MSMC_MMR_BUSECC_RAM_ID;
#elif defined (SOC_J721S2)
    subType = SDL_COMPUTE_CLUSTER0_COMPUTE_CLUSTER_J7AE_MSMC_EN_DRU_MSMC_EN_4MB_WRAP_MSMC_4MB_MSMC_WRAP_ECC_AGGR0_DRU0_CBASS_MMR_CFG_EDC_RAM_ID;
#elif defined (SOC_J784S4)
    subType = SDL_COMPUTE_CLUSTER0_COMPUTE_CLUSTER_J7AHP_MSMC_EN_DRU_MSMC_EN_8MB_WRAP_MSMC_8MB_MSMC_WRAP_ECC_AGGR0_MSMC_LAMBDA3_WRAP_MSMC_QOR_MSMC_CORE_EMIF_PIPE_EMIF0_DP_TO_VBUSMC_EDC_CTRL_0_RAM_ID;
#endif
	for(i = SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE; i<= SDL_INJECT_ECC_ERROR_FORCING_2BIT_N_ROW_REPEAT; i++)
	{
		result = SDL_ECC_selfTest(SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0,
                                  subType,
                                  i,
                                  &injectErrorConfig,
                                  1000);

	}
    if (result == SDL_PASS ) {
        UART_printf("\n ECC_Test_runECCSEC_DED_MSMCRAMSelfTest  Single bit error self test: Subtype %d: test failed",
                    subType);
		retVal = -1;
    } else {
        UART_printf("\n ECC_Test_runECCSEC_DED_MSMCRAMSelfTest Single bit error self test: Subtype 0x%p test complete",
                    subType);
		retVal = 0;
    }
	
    return retVal;
}
#endif

__attribute__((interrupt, section(".text.hwi"), target("arm"), aligned(32))) void ECC_Test_VIMDEDInterruptHandler(void);

/*********************************************************************
* @fn      ECC_Test_VIMDEDInterruptHandler
*
* @brief   Sample DED Interrupt handler
*
* @param   None
*
* @return   None
*/
void ECC_Test_VIMDEDInterruptHandler(void)
{
    /* Action to be taken by customer */
    UART_printf("\n  VIM DED error happened \n");
    UART_printf("  Take action \n");

    /* clear appropriate interrupt events */

    ECC_Test_ECC_indicateDEDTrigger();
}


static int32_t ECC_sdlFuncTest(void)
{
    int32_t result;
    int32_t retVal = 0;
    SDL_ECC_InjectErrorConfig_t injectErrorConfig;
    memset(&injectErrorConfig, 0, sizeof(injectErrorConfig));
    injectErrorConfig.pErrMem = (uint32_t *)(0u);
    injectErrorConfig.flipBitMask = 0x3;

    UART_printf("\n ECC SDL API tests: starting");

    result = ECC_Test_runECC1BitInjectTest();
    if (result != SDL_PASS) {
        retVal = -1;
        UART_printf("\n ECC_Test_runECC1BitInjectTest has failed...");
    }

    if (retVal == 0) {
        result = ECC_Test_runECC2BitInjectTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC2BitInjectTest has failed...");
        }
    }

    if (retVal == 0) {
        result = ECC_Test_runECC1BitSelfTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC1BitSelfTest has failed...");
        }
    }

    if (retVal == 0) {
        result = ECC_Test_runECC_nonAccessable1BitSelfTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC_nonAccessable1BitSelfTest has failed...");
        }
    }

    if (retVal == 0) {
        result = ECC_Test_runECC1BitNeg1SelfTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC1BitNeg1SelfTest has failed...");
        }
    }

    if (retVal == 0) {
        result = ECC_Test_runECC2BitSelfTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC2BitSelfTest has failed...");
        }
    }

    if (retVal == 0) {
        result = ECC_Test_runECC1BitB0TCM0Bank0SelfTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC1BitB0TCM0Bank0SelfTest has failed...");
        }
    }

    if (retVal == 0) {
        result = ECC_Test_runECC2BitB0TCM0Bank0SelfTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC2BitB0TCM0Bank0SelfTest has failed...");
        }
    }

    if (retVal == 0) {
        result = ECC_Test_runECC1BitB0TCM0Bank1SelfTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC1BitB0TCM0Bank1SelfTest has failed...");
        }
    }

    if (retVal == 0) {
        result = ECC_Test_runECC2BitB0TCM0Bank1SelfTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC2BitB0TCM0Bank1SelfTest has failed...");
        }
    }

    if (retVal == 0) {
        result = ECC_Test_runECC1BitB1TCM0Bank0SelfTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC1BitB1TCM0Bank0SelfTest has failed...");
        }
    }

    if (retVal == 0) {
        result = ECC_Test_runECC2BitB1TCM0Bank0SelfTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC2BitB1TCM0Bank0SelfTest has failed...");
        }
    }

    if (retVal == 0) {
        result = ECC_Test_runECC1BitB1TCM0Bank1SelfTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC1BitB1TCM0Bank1SelfTest has failed...");
        }
    }

    if (retVal == 0) {
        result = ECC_Test_runECC2BitB1TCM0Bank1SelfTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC2BitB1TCM0Bank1SelfTest has failed...");
        }
    }

    if (retVal == 0) {
        result = ECC_Test_runECC2BitCodeInjectTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC2BitCodeInjectTest has failed...");
        }
    }

    if (retVal == 0) {
        result = ECC_Test_runECC2BitCBASSSelfTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC2BitCBASSSelfTest has failed...");
        }
    }
	
    if (retVal == 0) {
        result = ECC_Test_runECC2BitCBASSInjectTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC2BitCBASSInjectTest has failed...");
        }
    }

    if (retVal == 0) {
        result = ECC_Test_runECC1BitCBASSSelfTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC1BitCBASSSelfTest has failed...");
        }
    }

    if (retVal == 0) {
        result = ECC_Test_runECC1BitCBASSInjectTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC1BitCBASSInjectTest has failed...");
        }
    }

    if (retVal == 0) {
        result = ECC_Test_runECC1BitMSMCRAMSelfTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC1BitMSMCRAMSelfTest has failed...");
        }
    }

    if (retVal == 0) {
        result = ECC_Test_runECC2BitMSMCRAMSelfTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC2BitMSMCRAMSelfTest has failed...");
        }
    }
#if defined (SOC_J721E) 
    if (retVal == 0) {
        result = ECC_Test_runECC1BitMSMC_CACHE_selfTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC1BitMSMC_CACHE_selfTest has failed...");
        }
    }


    if (retVal == 0) {
        result = ECC_Test_runECC2BitMSMC_CACHE_selfTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC2BitMSMC_CACHE_selfTest has failed...");
        }
    }

    if (retVal == 0) {
        result = ECC_Test_runECC1BitMSMCCLECselfTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC1BitMSMCCLECselfTest has failed...");
        }
    }

    if (retVal == 0) {
        result = ECC_Test_runECC2BitMSMCCLECselfTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC2BitMSMCCLECselfTest has failed...");
        }
    }
#endif	
    if (retVal == 0) {
        result = ECC_Test_runECC_ErrMemInjectTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC_ErrMemInjectTest has failed...");
        }
    }
	
    if (retVal == 0) {
        result = ECC_Test_runECC1BitNrowInjectTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC1BitNrowInjectTest has failed...");
        }
    }
	
    if (retVal == 0) {
        result = ECC_Test_runECC1BitRepeatInjectTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC1BitRepeatInjectTest has failed...");
        }
    }
	
    if (retVal == 0) {
        result = ECC_Test_runECC2BitNrowInjectTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC2BitNrowInjectTest has failed...");
        }
    }
    
    if (retVal == 0) {
        result = ECC_Test_runECC2BitRepeatInjectTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC2BitRepeatInjectTest has failed...");
        }
    }

    if (retVal == 0) {
        result = ECC_Test_runECC1BitVIMRAMInjectTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC1BitVIMRAMInjectTest has failed...");
        }
    }

    if (retVal == 0) {
        result = ECC_Test_runECC2BitVIMRAMInjectTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC2BitVIMRAMInjectTest has failed...");
        }
    }

    if (retVal == 0) {
        result = ECC_Test_runECC1BitVIMRAMSelfTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC1BitVIMRAMSelfTest has failed...");
        }
    }

    if (retVal == 0) {
        result = ECC_Test_runECC2BitVIMRAMSelfTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC2BitVIMRAMSelfTest has failed...");
        }
    }
    
    if (retVal == 0) {
        result = ECC_Test_runECCSEC_DED_MSMCRAMSelfTest();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECCSEC_DED_MSMCRAMSelfTest has failed...");
        }
    }
    /* This needs to be last as it is destructive */
    if (retVal == 0) {
        result = ECC_Test_runECC2BitVIMRAMDEDvector();
        if (result != SDL_PASS) {
            retVal = -1;
            UART_printf("\n ECC_Test_runECC2BitVIMRAMDEDvector has failed...");
        }
    }

    if ( retVal == 0) {
        UART_printf("\n ECC SDL API tests: success");
    } else {
        UART_printf("\n ECC SDL API tests: failed");
    }

    return retVal;
}

/* ECC Function module test */
int32_t ECC_funcTest(void)
{
    int32_t testResult;

    testResult = ECC_Test_init();

    if (testResult != 0)
    {
        UART_printf("\n ECC SDL API tests: unsuccessful");
        return SDL_EFAIL;
    }

    testResult = ECC_sdlFuncTest();


    return (testResult);
}

/* Nothing past this point */
