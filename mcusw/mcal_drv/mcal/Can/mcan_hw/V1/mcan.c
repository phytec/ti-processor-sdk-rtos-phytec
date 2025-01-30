/********************************************************************
 *   Copyright (C) Texas Instruments Incorporated 2019-2020
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
 *  \file     mcan.c
 *
 *  \brief    This file contains the implementation of the APIs present in the
 *            device abstraction layer file of MCAN.
 *            This also contains some related macros.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/**
 * \brief This is to disable HW_SYNC_BARRIER for J7 due to performance
 *        requirement
 */

#define MEM_BARRIER_DISABLE

#include "Std_Types.h"
#include <stdint.h>
#include <string.h>
#include <hw_include/lld_types.h>
#include <hw_include/hw_types.h>
#include <hw_include/lld_mcan.h>
#include <hw_include/arch/lld_arch.h>
#include "Can_Cfg.h"
/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 * \brief  MCAN MSG RAM BANK number for ECC AGGR.
 */
#define MCANSS_MSG_RAM_NUM                                       (1U)

/**
 * \brief  Maximum Number of Rx Buffers.
 */
#define MCANSS_RX_BUFFER_MAX                                     (64U)

/**
 * \brief  Maximum Number of Tx Buffers.
 */
#define MCANSS_TX_BUFFER_MAX                                     (32U)

/**
 * \brief  Macro for standard Message ID filter.
 */
#define MCANSS_STD_ID_FILTER_SIZE_WORDS                          (1U)

/**
 * \brief  Macro for extended Message ID filter.
 */
#define MCANSS_EXT_ID_FILTER_SIZE_WORDS                          (2U)

/**
 * \brief  Macro for Tx Event FIFO element size.
 */
#define MCANSS_TX_EVENT_FIFO_SIZE_WORDS                          (2U)


/**
 * \brief  Macro for Interrupt Line enable mask.
 */
#define MCANSS_INTR_LINE_EN_MASK   ((MCAN_ILE_EINT0_MASK | MCAN_ILE_EINT1_MASK))

/**
 * \brief  Mask and shift for Tx Buffers elements.
 */
#define MCANSS_TX_BUFFER_ELEM_ID_SHIFT                           (0U)
#define MCANSS_TX_BUFFER_ELEM_ID_MASK                            (0x1FFFFFFFU)
#define MCANSS_TX_BUFFER_ELEM_RTR_SHIFT                          (29U)
#define MCANSS_TX_BUFFER_ELEM_RTR_MASK                           (0x20000000U)
#define MCANSS_TX_BUFFER_ELEM_XTD_SHIFT                          (30U)
#define MCANSS_TX_BUFFER_ELEM_XTD_MASK                           (0x40000000U)
#define MCANSS_TX_BUFFER_ELEM_ESI_SHIFT                          (31U)
#define MCANSS_TX_BUFFER_ELEM_ESI_MASK                           (0x80000000U)
#define MCANSS_TX_BUFFER_ELEM_DLC_SHIFT                          (16U)
#define MCANSS_TX_BUFFER_ELEM_DLC_MASK                           (0x000F0000U)
#define MCANSS_TX_BUFFER_ELEM_BRS_SHIFT                          (20U)
#define MCANSS_TX_BUFFER_ELEM_BRS_MASK                           (0x00100000U)
#define MCANSS_TX_BUFFER_ELEM_FDF_SHIFT                          (21U)
#define MCANSS_TX_BUFFER_ELEM_FDF_MASK                           (0x00200000U)
#define MCANSS_TX_BUFFER_ELEM_EFC_SHIFT                          (23U)
#define MCANSS_TX_BUFFER_ELEM_EFC_MASK                           (0x00800000U)
#define MCANSS_TX_BUFFER_ELEM_MM_SHIFT                           (24U)
#define MCANSS_TX_BUFFER_ELEM_MM_MASK                            (0xFF000000U)

/**
 * \brief  Mask and shift for Rx Buffers elements.
 */
#define MCANSS_RX_BUFFER_ELEM_ID_SHIFT                           (0U)
#define MCANSS_RX_BUFFER_ELEM_ID_MASK                            (0x1FFFFFFFU)
#define MCANSS_RX_BUFFER_ELEM_RTR_SHIFT                          (29U)
#define MCANSS_RX_BUFFER_ELEM_RTR_MASK                           (0x20000000U)
#define MCANSS_RX_BUFFER_ELEM_XTD_SHIFT                          (30U)
#define MCANSS_RX_BUFFER_ELEM_XTD_MASK                           (0x40000000U)
#define MCANSS_RX_BUFFER_ELEM_ESI_SHIFT                          (31U)
#define MCANSS_RX_BUFFER_ELEM_ESI_MASK                           (0x80000000U)
#define MCANSS_RX_BUFFER_ELEM_RXTS_SHIFT                         (0U)
#define MCANSS_RX_BUFFER_ELEM_RXTS_MASK                          (0x0000FFFFU)
#define MCANSS_RX_BUFFER_ELEM_DLC_SHIFT                          (16U)
#define MCANSS_RX_BUFFER_ELEM_DLC_MASK                           (0x000F0000U)
#define MCANSS_RX_BUFFER_ELEM_BRS_SHIFT                          (20U)
#define MCANSS_RX_BUFFER_ELEM_BRS_MASK                           (0x00100000U)
#define MCANSS_RX_BUFFER_ELEM_FDF_SHIFT                          (21U)
#define MCANSS_RX_BUFFER_ELEM_FDF_MASK                           (0x00200000U)
#define MCANSS_RX_BUFFER_ELEM_FIDX_SHIFT                         (24U)
#define MCANSS_RX_BUFFER_ELEM_FIDX_MASK                          (0x7F000000U)
#define MCANSS_RX_BUFFER_ELEM_ANMF_SHIFT                         (31U)
#define MCANSS_RX_BUFFER_ELEM_ANMF_MASK                          (0x80000000U)

/**
 * \brief  Mask and shift for Standard Message ID Filter Elements.
 */
#define MCANSS_STD_ID_FILTER_SFID2_SHIFT                         (0U)
#define MCANSS_STD_ID_FILTER_SFID2_MASK                          (0x000003FFU)
#define MCANSS_STD_ID_FILTER_SFID1_SHIFT                         (16U)
#define MCANSS_STD_ID_FILTER_SFID1_MASK                          (0x03FF0000U)
#define MCANSS_STD_ID_FILTER_SFEC_SHIFT                          (27U)
#define MCANSS_STD_ID_FILTER_SFEC_MASK                           (0x38000000U)
#define MCANSS_STD_ID_FILTER_SFT_SHIFT                           (30U)
#define MCANSS_STD_ID_FILTER_SFT_MASK                            (0xC0000000U)

/**
 * \brief  Extended Message ID Filter Element.
 */
#define MCANSS_EXT_ID_FILTER_EFID2_SHIFT                        (0U)
#define MCANSS_EXT_ID_FILTER_EFID2_MASK                         (0x1FFFFFFFU)
#define MCANSS_EXT_ID_FILTER_EFID1_SHIFT                        (0U)
#define MCANSS_EXT_ID_FILTER_EFID1_MASK                         (0x1FFFFFFFU)
#define MCANSS_EXT_ID_FILTER_EFEC_SHIFT                         (29U)
#define MCANSS_EXT_ID_FILTER_EFEC_MASK                          (0xE0000000U)
#define MCANSS_EXT_ID_FILTER_EFT_SHIFT                          (30U)
#define MCANSS_EXT_ID_FILTER_EFT_MASK                           (0xC0000000U)

/**
 * \brief  Mask and shift for Tx Event FIFO elements.
 */
#define MCANSS_TX_EVENT_FIFO_ELEM_ID_SHIFT                      (0U)
#define MCANSS_TX_EVENT_FIFO_ELEM_ID_MASK                       (0x1FFFFFFFU)
#define MCANSS_TX_EVENT_FIFO_ELEM_RTR_SHIFT                     (29U)
#define MCANSS_TX_EVENT_FIFO_ELEM_RTR_MASK                      (0x20000000U)
#define MCANSS_TX_EVENT_FIFO_ELEM_XTD_SHIFT                     (30U)
#define MCANSS_TX_EVENT_FIFO_ELEM_XTD_MASK                      (0x40000000U)
#define MCANSS_TX_EVENT_FIFO_ELEM_ESI_SHIFT                     (31U)
#define MCANSS_TX_EVENT_FIFO_ELEM_ESI_MASK                      (0x80000000U)

#define MCANSS_TX_EVENT_FIFO_ELEM_TXTS_SHIFT                    (0U)
#define MCANSS_TX_EVENT_FIFO_ELEM_TXTS_MASK                     (0x0000FFFFU)
#define MCANSS_TX_EVENT_FIFO_ELEM_DLC_SHIFT                     (16U)
#define MCANSS_TX_EVENT_FIFO_ELEM_DLC_MASK                      (0x000F0000U)
#define MCANSS_TX_EVENT_FIFO_ELEM_BRS_SHIFT                     (20U)
#define MCANSS_TX_EVENT_FIFO_ELEM_BRS_MASK                      (0x00100000U)
#define MCANSS_TX_EVENT_FIFO_ELEM_FDF_SHIFT                     (21U)
#define MCANSS_TX_EVENT_FIFO_ELEM_FDF_MASK                      (0x00200000U)
#define MCANSS_TX_EVENT_FIFO_ELEM_ET_SHIFT                      (22U)
#define MCANSS_TX_EVENT_FIFO_ELEM_ET_MASK                       (0x00C00000U)
#define MCANSS_TX_EVENT_FIFO_ELEM_MM_SHIFT                      (24U)
#define MCANSS_TX_EVENT_FIFO_ELEM_MM_MASK                       (0xFF000000U)


/**
 * \brief  Defined baseaddress as macros for preprocessors and static constants only.
 */
 #if defined (SOC_J721E) ||  defined (SOC_J7200) ||  defined (SOC_J721S2) || defined (SOC_J784S4)
#define CSL_MCU_MCAN0_MSGMEM_RAM_BASE                          (0x40500000UL)
#define CSL_MCU_MCAN0_SS_BASE                                  (0x40520000UL)
#define CSL_MCU_MCAN0_CFG_BASE                                 (0x40528000UL)
#define CSL_MCU_MCAN1_MSGMEM_RAM_BASE                          (0x40540000UL)
#define CSL_MCU_MCAN1_SS_BASE                                  (0x40560000UL)
#define CSL_MCU_MCAN1_CFG_BASE                                 (0x40568000UL)
#define CSL_MCAN0_SS_BASE                                      (0x2700000UL)
#define CSL_MCAN0_MSGMEM_RAM_BASE                              (0x2708000UL)
#define CSL_MCAN0_CFG_BASE                                     (0x2701000UL)
#define CSL_MCAN1_SS_BASE                                      (0x2710000UL)
#define CSL_MCAN1_MSGMEM_RAM_BASE                              (0x2718000UL)
#define CSL_MCAN1_CFG_BASE                                     (0x2711000UL)
#define CSL_MCAN2_SS_BASE                                      (0x2720000UL)
#define CSL_MCAN2_CFG_BASE                                     (0x2721000UL)
#define CSL_MCAN2_MSGMEM_RAM_BASE                              (0x2728000UL)
#define CSL_MCAN3_SS_BASE                                      (0x2730000UL)
#define CSL_MCAN3_CFG_BASE                                     (0x2731000UL)
#define CSL_MCAN3_MSGMEM_RAM_BASE                              (0x2738000UL)
#define CSL_MCAN4_SS_BASE                                      (0x2740000UL)
#define CSL_MCAN4_CFG_BASE                                     (0x2741000UL)
#define CSL_MCAN4_MSGMEM_RAM_BASE                              (0x2748000UL)
#define CSL_MCAN5_SS_BASE                                      (0x2750000UL)
#define CSL_MCAN5_CFG_BASE                                     (0x2751000UL)
#define CSL_MCAN5_MSGMEM_RAM_BASE                              (0x2758000UL)
#define CSL_MCAN6_SS_BASE                                      (0x2760000UL)
#define CSL_MCAN6_CFG_BASE                                     (0x2761000UL)
#define CSL_MCAN6_MSGMEM_RAM_BASE                              (0x2768000UL)
#define CSL_MCAN7_SS_BASE                                      (0x2770000UL)
#define CSL_MCAN7_CFG_BASE                                     (0x2771000UL)
#define CSL_MCAN7_MSGMEM_RAM_BASE                              (0x2778000UL)
#define CSL_MCAN8_SS_BASE                                      (0x2780000UL)
#define CSL_MCAN8_CFG_BASE                                     (0x2781000UL)
#define CSL_MCAN8_MSGMEM_RAM_BASE                              (0x2788000UL)
#define CSL_MCAN9_SS_BASE                                      (0x2790000UL)
#define CSL_MCAN9_CFG_BASE                                     (0x2791000UL)
#define CSL_MCAN9_MSGMEM_RAM_BASE                              (0x2798000UL)
#define CSL_MCAN10_SS_BASE                                     (0x27a0000UL)
#define CSL_MCAN10_CFG_BASE                                    (0x27a1000UL)
#define CSL_MCAN10_MSGMEM_RAM_BASE                             (0x27a8000UL)
#define CSL_MCAN11_SS_BASE                                     (0x27b0000UL)
#define CSL_MCAN11_CFG_BASE                                    (0x27b1000UL)
#define CSL_MCAN11_MSGMEM_RAM_BASE                             (0x27b8000UL)
#define CSL_MCAN12_SS_BASE                                     (0x27c0000UL)
#define CSL_MCAN12_CFG_BASE                                    (0x27c1000UL)
#define CSL_MCAN12_MSGMEM_RAM_BASE                             (0x27c8000UL)
#define CSL_MCAN13_SS_BASE                                     (0x27d0000UL)
#define CSL_MCAN13_CFG_BASE                                    (0x27d1000UL)
#define CSL_MCAN13_MSGMEM_RAM_BASE                             (0x27d8000UL)
#endif
#if defined (SOC_J7200)
#define CSL_MCAN14_SS_BASE                                     (0x2680000UL)
#define CSL_MCAN14_CFG_BASE                                    (0x2681000UL)
#define CSL_MCAN14_MSGMEM_RAM_BASE                             (0x2688000UL)
#define CSL_MCAN15_SS_BASE                                     (0x2690000UL)
#define CSL_MCAN15_CFG_BASE                                    (0x2691000UL)
#define CSL_MCAN15_MSGMEM_RAM_BASE                             (0x2698000UL)
#define CSL_MCAN16_SS_BASE                                     (0x26a0000UL)
#define CSL_MCAN16_CFG_BASE                                    (0x26a1000UL)
#define CSL_MCAN16_MSGMEM_RAM_BASE                             (0x26a8000UL)
#define CSL_MCAN17_SS_BASE                                     (0x26b0000UL)
#define CSL_MCAN17_CFG_BASE                                    (0x26b1000UL)
#define CSL_MCAN17_MSGMEM_RAM_BASE                             (0x26b8000UL)
#endif

#if defined (SOC_AM62X)
#define CSL_MCU_MCAN0_MSGMEM_RAM_BASE                          (0x4e00000UL)
#define CSL_MCU_MCAN0_SS_BASE                                  (0x4e09000UL)
#define CSL_MCU_MCAN0_CFG_BASE                                 (0x4e08000UL)
#define CSL_MCU_MCAN1_MSGMEM_RAM_BASE                          (0x4e10000UL)
#define CSL_MCU_MCAN1_SS_BASE                                  (0x4e19000UL)
#define CSL_MCU_MCAN1_CFG_BASE                                 (0x4e18000UL)
#define CSL_MCAN0_SS_BASE                                      (0x20700000UL)
#define CSL_MCAN0_MSGMEM_RAM_BASE                              (0x20708000UL)
#define CSL_MCAN0_CFG_BASE                                     (0x20701000UL)
#endif

#if defined (SOC_AM62AX) 
#define CSL_MCU_MCAN0_MSGMEM_RAM_BASE                          (0x4e00000UL)
#define CSL_MCU_MCAN0_SS_BASE                                  (0x4e09000UL)
#define CSL_MCU_MCAN0_CFG_BASE                                 (0x4e08000UL)
#define CSL_MCU_MCAN1_MSGMEM_RAM_BASE                          (0x4e10000UL)
#define CSL_MCU_MCAN1_SS_BASE                                  (0x4e19000UL)
#define CSL_MCU_MCAN1_CFG_BASE                                 (0x4e18000UL)
#endif

#if defined (SOC_AM62PX) || defined (SOC_J722S)
#define CSL_MCU_MCAN0_MSGMEM_RAM_BASE                          (0x4e00000UL)
#define CSL_MCU_MCAN0_SS_BASE                                  (0x4e09000UL)
#define CSL_MCU_MCAN0_CFG_BASE                                 (0x4e08000UL)
#define CSL_MCU_MCAN1_MSGMEM_RAM_BASE                          (0x4e10000UL)
#define CSL_MCU_MCAN1_SS_BASE                                  (0x4e19000UL)
#define CSL_MCU_MCAN1_CFG_BASE                                 (0x4e18000UL)
#endif

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

typedef struct MCAN_OffsetAddr_t {
    uint32     mcanSsOffset;
    uint32     mcanCfgOffset;
} MCAN_OffsetAddr;

/* User will provide only the MsgRam Base address.
 * All Offsets are calculated compared to MsgRam Base address. */
static const MCAN_OffsetAddr mcuMcanOffsetAddr = {
    .mcanSsOffset       = ((uint32) CSL_MCU_MCAN0_SS_BASE         - (uint32) CSL_MCU_MCAN0_MSGMEM_RAM_BASE),
    .mcanCfgOffset      = ((uint32) CSL_MCU_MCAN0_CFG_BASE        - (uint32) CSL_MCU_MCAN0_MSGMEM_RAM_BASE),
};
/* Offsets are same for MCU MCAN0 and MCU MCAN1 instances. */
#if (((CSL_MCU_MCAN0_SS_BASE - CSL_MCU_MCAN0_MSGMEM_RAM_BASE)           != (CSL_MCU_MCAN1_SS_BASE - CSL_MCU_MCAN1_MSGMEM_RAM_BASE))         || \
     ((CSL_MCU_MCAN0_CFG_BASE - CSL_MCU_MCAN0_MSGMEM_RAM_BASE)          != (CSL_MCU_MCAN1_CFG_BASE - CSL_MCU_MCAN1_MSGMEM_RAM_BASE)))
     #error Offsets assumed donot match for MCU MCAN1
#endif

#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4) || defined (SOC_AM62X)
/* User will provide only the MsgRam Base address.
 * All Offsets are calculated compared to MsgRam Base address. */
static const MCAN_OffsetAddr mainMcanOffsetAddr  = {
    .mcanSsOffset       = ((uint32) CSL_MCAN0_SS_BASE         - (uint32) CSL_MCAN0_MSGMEM_RAM_BASE),
    .mcanCfgOffset      = ((uint32) CSL_MCAN0_CFG_BASE        - (uint32) CSL_MCAN0_MSGMEM_RAM_BASE),
};
#endif 

#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
/* Offsets are same for all main domain instances MCAN0 to MCAN13. For J7VCL it is MCAN0 to MCAN17 */
#if (((CSL_MCAN0_SS_BASE            - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN1_SS_BASE           - CSL_MCAN1_MSGMEM_RAM_BASE))   || \
     ((CSL_MCAN0_CFG_BASE           - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN1_CFG_BASE          - CSL_MCAN1_MSGMEM_RAM_BASE)))
     #error Offsets assumed donot match for MCAN1
#endif

#if (((CSL_MCAN0_SS_BASE            - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN2_SS_BASE           - CSL_MCAN2_MSGMEM_RAM_BASE))   || \
     ((CSL_MCAN0_CFG_BASE           - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN2_CFG_BASE          - CSL_MCAN2_MSGMEM_RAM_BASE)))
     #error Offsets assumed donot match for MCAN2
#endif
#if (((CSL_MCAN0_SS_BASE            - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN3_SS_BASE           - CSL_MCAN3_MSGMEM_RAM_BASE))   || \
     ((CSL_MCAN0_CFG_BASE           - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN3_CFG_BASE          - CSL_MCAN3_MSGMEM_RAM_BASE)))
     #error Offsets assumed donot match for MCAN3
#endif
#if (((CSL_MCAN0_SS_BASE            - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN4_SS_BASE           - CSL_MCAN4_MSGMEM_RAM_BASE))   || \
     ((CSL_MCAN0_CFG_BASE           - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN4_CFG_BASE          - CSL_MCAN4_MSGMEM_RAM_BASE)))
     #error Offsets assumed donot match for MCAN4
#endif
#if (((CSL_MCAN0_SS_BASE            - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN5_SS_BASE           - CSL_MCAN5_MSGMEM_RAM_BASE))   || \
     ((CSL_MCAN0_CFG_BASE           - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN5_CFG_BASE          - CSL_MCAN5_MSGMEM_RAM_BASE)))
     #error Offsets assumed donot match for MCAN5
#endif
#if (((CSL_MCAN0_SS_BASE            - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN6_SS_BASE           - CSL_MCAN6_MSGMEM_RAM_BASE))   || \
     ((CSL_MCAN0_CFG_BASE           - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN6_CFG_BASE          - CSL_MCAN6_MSGMEM_RAM_BASE)))
     #error Offsets assumed donot match for MCAN6
#endif
#if (((CSL_MCAN0_SS_BASE            - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN7_SS_BASE           - CSL_MCAN7_MSGMEM_RAM_BASE))   || \
     ((CSL_MCAN0_CFG_BASE           - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN7_CFG_BASE          - CSL_MCAN7_MSGMEM_RAM_BASE)))
     #error Offsets assumed donot match for MCAN7
#endif
#if (((CSL_MCAN0_SS_BASE            - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN8_SS_BASE           - CSL_MCAN8_MSGMEM_RAM_BASE))   || \
     ((CSL_MCAN0_CFG_BASE           - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN8_CFG_BASE          - CSL_MCAN8_MSGMEM_RAM_BASE)))
     #error Offsets assumed donot match for MCAN8
#endif
#if (((CSL_MCAN0_SS_BASE            - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN9_SS_BASE           - CSL_MCAN9_MSGMEM_RAM_BASE))   || \
     ((CSL_MCAN0_CFG_BASE           - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN9_CFG_BASE          - CSL_MCAN9_MSGMEM_RAM_BASE)))
     #error Offsets assumed donot match for MCAN9
#endif
#if (((CSL_MCAN0_SS_BASE            - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN10_SS_BASE           - CSL_MCAN10_MSGMEM_RAM_BASE))   || \
     ((CSL_MCAN0_CFG_BASE           - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN10_CFG_BASE          - CSL_MCAN10_MSGMEM_RAM_BASE)))
     #error Offsets assumed donot match for MCAN10
#endif
#if (((CSL_MCAN0_SS_BASE            - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN11_SS_BASE           - CSL_MCAN11_MSGMEM_RAM_BASE))   || \
     ((CSL_MCAN0_CFG_BASE           - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN11_CFG_BASE          - CSL_MCAN11_MSGMEM_RAM_BASE)))
     #error Offsets assumed donot match for MCAN11
#endif
#if (((CSL_MCAN0_SS_BASE            - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN12_SS_BASE           - CSL_MCAN12_MSGMEM_RAM_BASE))   || \
     ((CSL_MCAN0_CFG_BASE           - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN12_CFG_BASE          - CSL_MCAN12_MSGMEM_RAM_BASE)))
     #error Offsets assumed donot match for MCAN12
#endif
#if (((CSL_MCAN0_SS_BASE            - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN13_SS_BASE           - CSL_MCAN13_MSGMEM_RAM_BASE))   || \
     ((CSL_MCAN0_CFG_BASE           - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN13_CFG_BASE          - CSL_MCAN13_MSGMEM_RAM_BASE)))
     #error Offsets assumed donot match for MCAN13
#endif
#if defined (SOC_J7200)
#if (((CSL_MCAN0_SS_BASE            - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN14_SS_BASE           - CSL_MCAN14_MSGMEM_RAM_BASE))   || \
     ((CSL_MCAN0_CFG_BASE           - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN14_CFG_BASE          - CSL_MCAN14_MSGMEM_RAM_BASE)))
     #error Offsets assumed donot match for MCAN14
#endif
#if (((CSL_MCAN0_SS_BASE            - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN15_SS_BASE           - CSL_MCAN15_MSGMEM_RAM_BASE))   || \
     ((CSL_MCAN0_CFG_BASE           - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN15_CFG_BASE          - CSL_MCAN15_MSGMEM_RAM_BASE)))
     #error Offsets assumed donot match for MCAN15
#endif
#if (((CSL_MCAN0_SS_BASE            - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN16_SS_BASE           - CSL_MCAN16_MSGMEM_RAM_BASE))   || \
     ((CSL_MCAN0_CFG_BASE           - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN16_CFG_BASE          - CSL_MCAN16_MSGMEM_RAM_BASE)))
     #error Offsets assumed donot match for MCAN16
#endif
#if (((CSL_MCAN0_SS_BASE            - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN17_SS_BASE           - CSL_MCAN17_MSGMEM_RAM_BASE))   || \
     ((CSL_MCAN0_CFG_BASE           - CSL_MCAN0_MSGMEM_RAM_BASE)    != (CSL_MCAN17_CFG_BASE          - CSL_MCAN17_MSGMEM_RAM_BASE)))
     #error Offsets assumed donot match for MCAN17
#endif
#endif
#endif

static const MCAN_OffsetAddr* MCAN_getOffsetAddr (uint32 baseAddr);
/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

/**
 * \brief   This API will unblock write access to write protected registers.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 *
 * \return  None.
 */
static void MCAN_writeProtectedRegAccessUnlock(uint32 baseAddr);

/**
 * \brief   This API will block write access to write protected registers.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 *
 * \return  None.
 */
static void MCAN_writeProtectedRegAccessLock(uint32 baseAddr);

/**
 * \brief   This API will load the register from ECC memory bank.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 * \param   regOffset       Offset of the register to read.
 *
 * \return  None.
 */
static void MCAN_eccLoadRegister(uint32 baseAddr, uint32 regOffset);

/**
 * \brief   This API will read the message object from Message RAM. This uses
 *          the MCAN_RxBufElementNoCpy structure which has the data field as
 *          a pointer. Needed to avoid one copy of the data/payload in the MCAL
 *          CAN driver and replace it by a pointer assignment.
 *          Note that as the data is a pointer here hence corruption of data is
 *          possible in case you exceed the payload size.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 * \param   elemAddr        Address of the message object.
 * \param   elem            Message Object.
 *                          Refer struct #MCAN_RxBufElementNoCpy.
 *
 * \return  None.
 */
static void MCAN_readMsgNoCpy(uint32           baseAddr,
                         uint32           elemAddr,
                         MCAN_RxBufElementNoCpy *elem);


/**
 * \brief   This API will write the message object to Message RAM. This uses
 *          the MCAN_TxBufElementNoCpy structure which has the data field as
 *          a pointer. Needed to avoid one copy of the data/payload in the MCAL
 *          CAN driver and replace it by a pointer assignment.
 *          Note that as the data is a pointer here hence corruption of data is
 *          possible in case you exceed the payload size.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 * \param   elemAddr        Address of the message object.
 * \param   elem            Message Object.
 *                          Refer struct #MCAN_TxBufElementNoCpy.
 *
 * \return  None.
 */
static void MCAN_writeMsgNoCpy(uint32                 baseAddr,
                          uint32                 elemAddr,
                          const MCAN_TxBufElementNoCpy *elem);

/**
 * \brief   This API will return ECC Configuration Register Base Address.
 *
 * \param   baseAddr        Base Address of the MCAN Registers.
 *
 * \return  offset
 *                          ECC Configuration Register Base Address.
 */
static uint32 MCAN_getECCRegionAddr(uint32 baseAddr);
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* payload depending on 'dlc'  field. */
static const uint32 gDataSize[16]  = {0,  1,  2,  3,  4,  5,  6, 7, 8,
                                        12, 16, 20, 24, 32, 48, 64};
/* message object stored in Message RAM. */
static const uint32 gMsgObjSize[8] = {4, 5, 6, 7, 8, 10, 14, 18};

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

static uint32 MCAN_SsAddr(uint32 baseAddr);
static uint32 MCAN_SsAddr(uint32 baseAddr)
{
    uint64_t mcanSsAddr = (uint64_t)NULL;
    const MCAN_OffsetAddr *offsetAddr = MCAN_getOffsetAddr(baseAddr);
    if (offsetAddr != NULL)
    {
        mcanSsAddr = (uint64_t) baseAddr + (uint64_t)offsetAddr->mcanSsOffset;
    }
    return ((uint32) mcanSsAddr);
}

static uint32 MCAN_CfgAddr(uint32 baseAddr);
static uint32 MCAN_CfgAddr(uint32 baseAddr)
{
    uint64_t mcanCfgAddr = (uint64_t)NULL;
    const MCAN_OffsetAddr *offsetAddr = MCAN_getOffsetAddr(baseAddr);
    if (offsetAddr != NULL)
    {
        mcanCfgAddr = (uint64_t) baseAddr + (uint64_t)offsetAddr->mcanCfgOffset;
    }
    return ((uint32) mcanCfgAddr);
}



uint32 MCAN_isMemInitDone(uint32 baseAddr)
{
    uint32 memInit;
    uint32 state;

    memInit = HW_RD_FIELD32(MCAN_SsAddr(baseAddr) + MCAN_MCANSS_STAT,
                            MCAN_MCANSS_STAT_MEM_INIT_DONE);
    if (1U == memInit)
    {
        state = (uint32) TRUE;
    }
    else
    {
        state = (uint32) FALSE;
    }
    return state;
}

void MCAN_setOpMode(uint32 baseAddr, uint32 mode)
{

    HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_CCCR, MCAN_CCCR_INIT, mode);
}

uint32 MCAN_getOpMode(uint32 baseAddr)
{

    return (HW_RD_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_CCCR, MCAN_CCCR_INIT));
}

int32_t MCAN_init(uint32 baseAddr, const MCAN_InitParams *initParams)
{
    int32_t  status;
    uint32 regVal;

    /* Configure MCAN wakeup and clock stop controls */
    regVal = HW_RD_REG32(MCAN_SsAddr(baseAddr) + MCAN_MCANSS_CTRL);
    HW_SET_FIELD32(regVal,
                   MCAN_MCANSS_CTRL_WAKEUPREQEN,
                   initParams->wkupReqEnable);
    HW_SET_FIELD32(regVal,
                   MCAN_MCANSS_CTRL_AUTOWAKEUP,
                   initParams->autoWkupEnable);

    HW_SET_FIELD32(regVal,
                   MCAN_MCANSS_CTRL_EMUEN,
                   initParams->emulationEnable);
    HW_SET_FIELD32(regVal,
                   MCAN_MCANSS_CTRL_EMUFACK,
                   initParams->emulationFAck);
    HW_SET_FIELD32(regVal,
                   MCAN_MCANSS_CTRL_CLKFACK,
                   initParams->clkStopFAck);

    HW_WR_REG32(MCAN_SsAddr(baseAddr) + MCAN_MCANSS_CTRL, regVal);

    MCAN_writeProtectedRegAccessUnlock(baseAddr);

    /* Configure MCAN mode(FD vs Classic CAN operation) and controls */
    regVal = HW_RD_REG32(MCAN_CfgAddr(baseAddr) + MCAN_CCCR);
    HW_SET_FIELD32(regVal,
                   MCAN_CCCR_FDOE,
                   initParams->fdMode);
    HW_SET_FIELD32(regVal,
                   MCAN_CCCR_BRSE,
                   initParams->brsEnable);
    HW_SET_FIELD32(regVal,
                   MCAN_CCCR_TXP,
                   initParams->txpEnable);
    HW_SET_FIELD32(regVal,
                   MCAN_CCCR_EFBI,
                   initParams->efbi);
    HW_SET_FIELD32(regVal,
                   MCAN_CCCR_PXHD,
                   initParams->pxhddisable);
    HW_SET_FIELD32(regVal,
                   MCAN_CCCR_DAR,
                   initParams->darEnable);
    HW_WR_REG32(MCAN_CfgAddr(baseAddr) + MCAN_CCCR, regVal);

        /* CAN_CoverageGap_41: The driver code shall not utilize complete
         * feature of the hardware to implement the Autosar requirements. 
           So, MC/DC for this function MCAN_init() is not covered. 
         */
    if ((MCAN_TDCR_TDCF_MAX >= initParams->tdcConfig.tdcf) &&
        (MCAN_TDCR_TDCO_MAX >= initParams->tdcConfig.tdco) &&
        (MCAN_RWD_WDC_MAX >= initParams->wdcPreload))
    {
        /* Configure Transceiver Delay Compensation */
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_TDCR,
                      MCAN_TDCR_TDCF,
                      initParams->tdcConfig.tdcf);
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_TDCR,
                      MCAN_TDCR_TDCO,
                      initParams->tdcConfig.tdco);
        /* Configure MSG RAM watchdog counter preload value */
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_RWD,
                      MCAN_RWD_WDC,
                      initParams->wdcPreload);
        /* Enable/Disable Transceiver Delay Compensation */
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_DBTP,
                      MCAN_DBTP_TDC,
                      initParams->tdcEnable);
        status = LLD_STW_SOK;
    }
    else
    {
        status = LLD_STW_EFAIL;
    }

    MCAN_writeProtectedRegAccessLock(baseAddr);

    return status;
}

int32_t MCAN_config(uint32 baseAddr, const MCAN_ConfigParams *configParams)
{
    int32_t status;

    MCAN_writeProtectedRegAccessUnlock(baseAddr);

    /* Configure MCAN control registers */
    HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_CCCR,
                  MCAN_CCCR_MON,
                  configParams->monEnable);
    HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_CCCR,
                  MCAN_CCCR_ASM,
                  configParams->asmEnable);
    /* Configure Global Filter */
    HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_GFC,
                  MCAN_GFC_RRFE,
                  configParams->filterConfig.rrfe);
    HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_GFC,
                  MCAN_GFC_RRFS,
                  configParams->filterConfig.rrfs);
    HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_GFC,
                  MCAN_GFC_ANFE,
                  configParams->filterConfig.anfe);
    HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_GFC,
                  MCAN_GFC_ANFS,
                  configParams->filterConfig.anfs);

        /* CAN_CoverageGap_42: The driver code shall not utilize 
         * complete feature of the hardware to implement the Autosar 
         * requirements.So, MC/DC for this function MCAN_config() is not covered.
         */
    if ((MCAN_TSCC_TCP_MAX >= configParams->tsPrescalar) &&
        (MCAN_TOCC_TOP_MAX >= configParams->timeoutPreload))
    {
        /* Configure Time-stamp counter */
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_TSCC,
                      MCAN_TSCC_TSS,
                      configParams->tsSelect);
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_TSCC,
                      MCAN_TSCC_TCP,
                      (configParams->tsPrescalar - 1U));
        /* Configure Time-out counter */
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_TOCC,
                      MCAN_TOCC_TOS,
                      configParams->timeoutSelect);
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_TOCC,
                      MCAN_TOCC_TOP,
                      configParams->timeoutPreload);
        /* Enable Time-out counter */
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_TOCC,
                      MCAN_TOCC_ETOC,
                      configParams->timeoutCntEnable);
        status = LLD_STW_SOK;
    }
    else
    {
        status = LLD_STW_EFAIL;
    }

    MCAN_writeProtectedRegAccessLock(baseAddr);

    return status;
}

void MCAN_eccConfig(uint32                    baseAddr,
                    const MCAN_ECCConfigParams *configParams)
{
    uint32 regVal;
    uint32 eccAggrBaseAddr;

    eccAggrBaseAddr = MCAN_getECCRegionAddr (baseAddr);

    MCAN_eccLoadRegister(baseAddr, MCAN_ECC_AGGR_CONTROL);
    regVal = HW_RD_REG32(eccAggrBaseAddr + MCAN_ECC_AGGR_CONTROL);
    HW_SET_FIELD32(regVal,
                   MCAN_ECC_AGGR_CONTROL_ECC_CHECK,
                   configParams->enableChk);
    HW_SET_FIELD32(regVal,
                   MCAN_ECC_AGGR_CONTROL_ECC_ENABLE,
                   configParams->enable);
    HW_SET_FIELD32(regVal,
                   MCAN_ECC_AGGR_CONTROL_ENABLE_RMW,
                   configParams->enableRdModWr);
    HW_WR_REG32(eccAggrBaseAddr + MCAN_ECC_AGGR_CONTROL, regVal);
}

int32_t MCAN_setBitTime(uint32                    baseAddr,
                        const MCAN_BitTimingParams *configParams)
{
    int32_t status;

    MCAN_writeProtectedRegAccessUnlock(baseAddr);

        /* CAN_CoverageGap_43:  The driver code shall not utilize
         * complete feature of the hardware to implement the Autosar 
         * requirements.So, MC/DC for this function MCAN_setBitTime() is not covered. 
         */
    if ((MCAN_NBTP_NSJW_MAX >= configParams->nomSynchJumpWidth) &&
        (MCAN_NBTP_NTSEG2_MAX >= configParams->nomTimeSeg2) &&
        (MCAN_NBTP_NTSEG1_MAX >= configParams->nomTimeSeg1) &&
        (MCAN_NBTP_NBRP_MAX >= configParams->nomRatePrescalar))
    {
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_NBTP,
                      MCAN_NBTP_NSJW,
                      configParams->nomSynchJumpWidth);
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_NBTP,
                      MCAN_NBTP_NTSEG2,
                      configParams->nomTimeSeg2);
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_NBTP,
                      MCAN_NBTP_NTSEG1,
                      configParams->nomTimeSeg1);
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_NBTP,
                      MCAN_NBTP_NBRP,
                      configParams->nomRatePrescalar);
        status = LLD_STW_SOK;
    }
    else
    {
        status = LLD_STW_EFAIL;
    }
    if (LLD_STW_SOK == status)
    {
        /* CAN_CoverageGap_44:  The driver code shall not utilize
         * complete feature of the hardware to implement the Autosar 
         * requirements.So, MC/DC for this function MCAN_setBitTime() is not covered. 
         */
        if ((MCAN_DBTP_DSJW_MAX >= configParams->dataSynchJumpWidth) &&
            (MCAN_DBTP_DTSEG2_MAX >= configParams->dataTimeSeg2) &&
            (MCAN_DBTP_DTSEG1_MAX >= configParams->dataTimeSeg1) &&
            (MCAN_DBTP_DBRP_MAX >= configParams->dataRatePrescalar))
        {
            HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_DBTP,
                          MCAN_DBTP_DSJW,
                          configParams->dataSynchJumpWidth);
            HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_DBTP,
                          MCAN_DBTP_DTSEG2,
                          configParams->dataTimeSeg2);
            HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_DBTP,
                          MCAN_DBTP_DTSEG1,
                          configParams->dataTimeSeg1);
            HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_DBTP,
                          MCAN_DBTP_DBRP,
                          configParams->dataRatePrescalar);
            status = LLD_STW_SOK;
        }
        else
        {
            status = LLD_STW_EFAIL;
        }
    }

    MCAN_writeProtectedRegAccessLock(baseAddr);
    return status;
}

int32_t MCAN_msgRAMConfig(uint32                       baseAddr,
                          const MCAN_MsgRAMConfigParams *msgRAMConfigParams)
{
    int32_t  status;
    uint32 elemNum = 0U;

    MCAN_writeProtectedRegAccessUnlock(baseAddr);

    /* Configure Message Filters section */
    if (0U != msgRAMConfigParams->lss)
    {
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_SIDFC,
                      MCAN_SIDFC_FLSSA,
                      (msgRAMConfigParams->flssa >> 2U));
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_SIDFC,
                      MCAN_SIDFC_LSS,
                      msgRAMConfigParams->lss);
    }
    if (0U != msgRAMConfigParams->lse)
    {
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_XIDFC,
                      MCAN_XIDFC_FLESA,
                      (msgRAMConfigParams->flesa >> 2U));
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_XIDFC,
                      MCAN_XIDFC_LSE,
                      msgRAMConfigParams->lse);
    }
    /* Configure Rx FIFO 0 section */
    if (0U != msgRAMConfigParams->rxFIFO0size)
    {
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_RXF0C,
                      MCAN_RXF0C_F0SA,
                      (msgRAMConfigParams->rxFIFO0startAddr >> 2U));
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_RXF0C,
                      MCAN_RXF0C_F0S,
                      msgRAMConfigParams->rxFIFO0size);
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_RXF0C,
                      MCAN_RXF0C_F0WM,
                      msgRAMConfigParams->rxFIFO0waterMark);
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_RXF0C,
                      MCAN_RXF0C_F0OM,
                      msgRAMConfigParams->rxFIFO0OpMode);
        /* Configure Rx FIFO0 elements size */
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_RXESC,
                      MCAN_RXESC_F0DS,
                      msgRAMConfigParams->rxFIFO0ElemSize);
    }
    /* Configure Rx FIFO 1 section */
    if (0U != msgRAMConfigParams->rxFIFO1size)
    {
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_RXF1C,
                      MCAN_RXF1C_F1SA,
                      (msgRAMConfigParams->rxFIFO1startAddr >> 2U));
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_RXF1C,
                      MCAN_RXF1C_F1S,
                      msgRAMConfigParams->rxFIFO1size);
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_RXF1C,
                      MCAN_RXF1C_F1WM,
                      msgRAMConfigParams->rxFIFO1waterMark);
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_RXF1C,
                      MCAN_RXF1C_F1OM,
                      msgRAMConfigParams->rxFIFO1OpMode);
        /* Configure Rx FIFO1 elements size */
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_RXESC,
                      MCAN_RXESC_F1DS,
                      msgRAMConfigParams->rxFIFO1ElemSize);
    }
    /* Configure Rx Buffer Start Address */
    HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_RXBC,
                  MCAN_RXBC_RBSA,
                  (msgRAMConfigParams->rxBufStartAddr >> 2U));
    /* Configure Rx Buffer elements size */
    HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_RXESC,
                  MCAN_RXESC_RBDS,
                  msgRAMConfigParams->rxBufElemSize);
    /* Configure Tx Event FIFO section */
    if (0U != msgRAMConfigParams->txEventFIFOSize)
    {
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_TXEFC,
                      MCAN_TXEFC_EFSA,
                      (msgRAMConfigParams->txEventFIFOStartAddr >> 2U));
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_TXEFC,
                      MCAN_TXEFC_EFS,
                      msgRAMConfigParams->txEventFIFOSize);
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_TXEFC,
                      MCAN_TXEFC_EFWM,
                      msgRAMConfigParams->txEventFIFOWaterMark);
    }
    /* Configure Tx Buffer and FIFO/Q section */
    elemNum = msgRAMConfigParams->txBufNum + msgRAMConfigParams->txFIFOSize;

     /* CAN_CoverageGap_45: The driver code shall not utilize complete feature
      * of the hardware to implement the Autosar requirements.So, MC/DC for this 
      * function MCAN_msgRAMConfig is not covered.
      */

    if ((MCANSS_TX_BUFFER_MAX >= elemNum) &&
        ((0U != msgRAMConfigParams->txBufNum) ||
         (0U != msgRAMConfigParams->txFIFOSize)))
    {
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_TXBC,
                      MCAN_TXBC_TBSA,
                      (msgRAMConfigParams->txStartAddr >> 2U));
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_TXBC,
                      MCAN_TXBC_NDTB,
                      msgRAMConfigParams->txBufNum);
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_TXBC,
                      MCAN_TXBC_TFQS,
                      msgRAMConfigParams->txFIFOSize);
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_TXBC,
                      MCAN_TXBC_TFQM,
                      msgRAMConfigParams->txBufMode);
        /* Configure Tx Buffer/FIFO0/FIFO1 elements size */
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_TXESC,
                      MCAN_TXESC_TBDS,
                      msgRAMConfigParams->txBufElemSize);
        status = LLD_STW_SOK;
    }
    else
    {
        status = LLD_STW_EFAIL;
    }

    MCAN_writeProtectedRegAccessLock(baseAddr);

    return status;
}


void MCAN_writeMsgRamNoCpy(uint32                 baseAddr,
                      uint32                 memType,
                      uint32                 bufNum,
                      const MCAN_TxBufElementNoCpy *elem)
{
    uint32 startAddr = 0U, elemSize = 0U, elemAddr = 0U;
    uint32 idx       = 0U, enableMod = 0U;

    if (MCAN_MEM_TYPE_BUF == memType)
    {
        idx       = bufNum;
        enableMod = 1U;
    }
    if (MCAN_MEM_TYPE_FIFO == memType)
    {
        idx       = HW_RD_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_TXFQS, MCAN_TXFQS_TFQPI);
        enableMod = 1U;
    }
    if (1U == enableMod)
    {
        startAddr = HW_RD_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_TXBC,
                                  MCAN_TXBC_TBSA);
        elemSize = HW_RD_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_TXESC,
                                 MCAN_TXESC_TBDS);
        startAddr = (uint32) (startAddr << 2U);
        elemSize  = gMsgObjSize[elemSize];
        elemSize *= 4U;
        elemAddr  = startAddr + (elemSize * idx);
        elemAddr += MCAN_MCAN_MSG_MEM;
        MCAN_writeMsgNoCpy(baseAddr, elemAddr, elem);
    }
}

int32_t MCAN_txBufAddReq(uint32 baseAddr, uint32 bufNum)
{
    int32_t  status;
    uint32 regVal;

    if (MCANSS_TX_BUFFER_MAX > bufNum)
    {
        regVal  = HW_RD_REG32(MCAN_CfgAddr(baseAddr) + MCAN_TXBAR);
        regVal |= ((uint32) 1U << bufNum);
        HW_WR_REG32(MCAN_CfgAddr(baseAddr) + MCAN_TXBAR, regVal);
        status = LLD_STW_SOK;
    }
    else
    {
        status = LLD_STW_EFAIL;
    }
    return status;
}

void  MCAN_getNewDataStatus(uint32              baseAddr,
                            MCAN_RxNewDataStatus *newDataStatus)
{

    newDataStatus->statusLow  = HW_RD_REG32(MCAN_CfgAddr(baseAddr) + MCAN_NDAT1);
    newDataStatus->statusHigh = HW_RD_REG32(MCAN_CfgAddr(baseAddr) + MCAN_NDAT2);
}

void  MCAN_clearNewDataStatus(uint32                    baseAddr,
                              const MCAN_RxNewDataStatus *newDataStatus)
{

    HW_WR_REG32(MCAN_CfgAddr(baseAddr) + MCAN_NDAT1, newDataStatus->statusLow);
    HW_WR_REG32(MCAN_CfgAddr(baseAddr) + MCAN_NDAT2, newDataStatus->statusHigh);
}


void MCAN_readMsgRamNoCpy(uint32           baseAddr,
                     uint32           memType,
                     uint32           bufNum,
                     uint32           fifoNum,
                     MCAN_RxBufElementNoCpy *elem)
{
    uint32 startAddr = 0U, elemSize = 0U, elemAddr = 0U;
    uint32 enableMod = 0U, idx = 0U;

    if (MCAN_MEM_TYPE_BUF == memType)
    {
        startAddr = HW_RD_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_RXBC,
                                  MCAN_RXBC_RBSA);
        elemSize = HW_RD_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_RXESC,
                                 MCAN_RXESC_RBDS);
        idx       = bufNum;
        enableMod = 1U;
    }
    if (MCAN_MEM_TYPE_FIFO == memType)
    {
        switch (fifoNum)
        {
            case MCAN_RX_FIFO_NUM_0:
                startAddr = HW_RD_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_RXF0C,
                                          MCAN_RXF0C_F0SA);
                elemSize = HW_RD_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_RXESC,
                                         MCAN_RXESC_F0DS);
                idx = HW_RD_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_RXF0S,
                                    MCAN_RXF0S_F0GI);
                enableMod = 1U;
                break;
            case MCAN_RX_FIFO_NUM_1:
                startAddr = HW_RD_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_RXF1C,
                                          MCAN_RXF1C_F1SA);
                elemSize = HW_RD_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_RXESC,
                                         MCAN_RXESC_F1DS);
                idx = HW_RD_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_RXF1S,
                                    MCAN_RXF1S_F1GI);
                enableMod = 1U;
                break;
            default:
                /* Invalid option */
                break;
        }
    }
    if (1U == enableMod)
    {
        startAddr = (uint32) (startAddr << 2U);
        elemSize  = gMsgObjSize[elemSize];
        elemSize *= 4U;
        elemAddr  = startAddr + (elemSize * idx);
        elemAddr += MCAN_MCAN_MSG_MEM;
        MCAN_readMsgNoCpy(baseAddr, elemAddr, elem);
    }
}


void MCAN_addStdMsgIDFilter(uint32                          baseAddr,
                            uint32                          filtNum,
                            const MCAN_StdMsgIDFilterElement *elem)
{
    uint32 startAddr, elemAddr, regVal;

    startAddr = HW_RD_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_SIDFC,
                              MCAN_SIDFC_FLSSA);
    startAddr = (uint32) (startAddr << 2U);
    elemAddr  = startAddr + (filtNum * MCANSS_STD_ID_FILTER_SIZE_WORDS * 4U);
    elemAddr += MCAN_MCAN_MSG_MEM;

    regVal  = 0U;
    regVal |= (uint32) (elem->sfid2 << MCANSS_STD_ID_FILTER_SFID2_SHIFT);
    regVal |= (uint32) (elem->sfid1 << MCANSS_STD_ID_FILTER_SFID1_SHIFT);
    regVal |= (uint32) (elem->sfec << MCANSS_STD_ID_FILTER_SFEC_SHIFT);
    regVal |= (uint32) (elem->sft << MCANSS_STD_ID_FILTER_SFT_SHIFT);
    HW_WR_REG32(baseAddr + elemAddr, regVal);
}

void MCAN_addExtMsgIDFilter(uint32                          baseAddr,
                            uint32                          filtNum,
                            const MCAN_ExtMsgIDFilterElement *elem)
{
    uint32 startAddr, elemAddr, regVal;

    startAddr = HW_RD_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_XIDFC,
                              MCAN_XIDFC_FLESA);
    startAddr = (uint32) (startAddr << 2U);
    elemAddr  = startAddr + (filtNum * MCANSS_EXT_ID_FILTER_SIZE_WORDS * 4U);
    elemAddr += MCAN_MCAN_MSG_MEM;

    regVal  = 0U;
    regVal |= (uint32) (elem->efid1 << MCANSS_EXT_ID_FILTER_EFID1_SHIFT);
    regVal |= (uint32) (elem->efec << MCANSS_EXT_ID_FILTER_EFEC_SHIFT);
    HW_WR_REG32(baseAddr + elemAddr, regVal);

    elemAddr += 4U;
    regVal    = 0U;
    regVal   |= (uint32) (elem->efid2 << MCANSS_EXT_ID_FILTER_EFID2_SHIFT);
    regVal   |= (uint32) (elem->eft << MCANSS_EXT_ID_FILTER_EFT_SHIFT);
    HW_WR_REG32(baseAddr + elemAddr, regVal);
}

void MCAN_lpbkModeEnable(uint32 baseAddr,
                         uint32 lpbkMode,
                         uint32 enable)
{
    MCAN_writeProtectedRegAccessUnlock(baseAddr);

    if (TRUE == enable)
    {
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_CCCR, MCAN_CCCR_TEST, 0x1U);
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_TEST,
                      MCAN_TEST_LBCK,
                      enable);
        if (MCAN_LPBK_MODE_INTERNAL == lpbkMode)
        {
            HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_CCCR,
                          MCAN_CCCR_MON,
                          0x1U);
        }
    }
    else
    {
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_TEST,
                      MCAN_TEST_LBCK,
                      enable);
        HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_CCCR, MCAN_CCCR_TEST, 0x0U);
        if (MCAN_LPBK_MODE_INTERNAL == lpbkMode)
        {
            HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_CCCR,
                          MCAN_CCCR_MON,
                          0x0U);
        }
    }
    MCAN_writeProtectedRegAccessLock(baseAddr);
}


void  MCAN_getProtocolStatus(uint32             baseAddr,
                             MCAN_ProtocolStatus *protStatus)
{
    uint32 regVal;

    regVal = HW_RD_REG32(MCAN_CfgAddr(baseAddr) + MCAN_PSR);
    protStatus->lastErrCode   = HW_GET_FIELD(regVal, MCAN_PSR_LEC);
    protStatus->act           = HW_GET_FIELD(regVal, MCAN_PSR_ACT);
    protStatus->errPassive    = HW_GET_FIELD(regVal, MCAN_PSR_EP);
    protStatus->warningStatus = HW_GET_FIELD(regVal, MCAN_PSR_EW);
    protStatus->busOffStatus  = HW_GET_FIELD(regVal, MCAN_PSR_BO);
    protStatus->dlec          = HW_GET_FIELD(regVal, MCAN_PSR_DLEC);
    protStatus->resi          = HW_GET_FIELD(regVal, MCAN_PSR_RESI);
    protStatus->rbrs          = HW_GET_FIELD(regVal, MCAN_PSR_RBRS);
    protStatus->rfdf          = HW_GET_FIELD(regVal, MCAN_PSR_RFDF);
    protStatus->pxe           = HW_GET_FIELD(regVal, MCAN_PSR_PXE);
    protStatus->tdcv          = HW_GET_FIELD(regVal, MCAN_PSR_TDCV);
}

void MCAN_enableIntr(uint32 baseAddr, uint32 intrMask, uint32 enable)
{
    uint32 regVal;

    if (TRUE == enable)
    {
        regVal  = HW_RD_REG32(MCAN_CfgAddr(baseAddr) + MCAN_IE);
        regVal |= intrMask;
        HW_WR_REG32(MCAN_CfgAddr(baseAddr) + MCAN_IE, regVal);
    }
    else
    {
        regVal  = HW_RD_REG32(MCAN_CfgAddr(baseAddr) + MCAN_IE);
        regVal &= ~intrMask;
        HW_WR_REG32(MCAN_CfgAddr(baseAddr) + MCAN_IE, regVal);
    }
}

void MCAN_selectIntrLine(uint32 baseAddr,
                         uint32 intrMask,
                         uint32 lineNum)
{
    uint32 regVal;

    if (MCAN_INTR_LINE_NUM_0 == lineNum)
    {
        regVal  = HW_RD_REG32(MCAN_CfgAddr(baseAddr) + MCAN_ILS);
        regVal &= ~intrMask;
        HW_WR_REG32(MCAN_CfgAddr(baseAddr) + MCAN_ILS, regVal);
    }
    else
    {
        regVal  = HW_RD_REG32(MCAN_CfgAddr(baseAddr) + MCAN_ILS);
        regVal |= intrMask;
        HW_WR_REG32(MCAN_CfgAddr(baseAddr) + MCAN_ILS, regVal);
    }
}


void MCAN_enableIntrLine(uint32 baseAddr,
                         uint32 lineNum,
                         uint32 enable)
{
    uint32 regVal, tempLineNum;

    tempLineNum = lineNum & MCANSS_INTR_LINE_EN_MASK;
    regVal   = HW_RD_REG32(MCAN_CfgAddr(baseAddr) + MCAN_ILE);
    regVal  &= ~((uint32) 0x1U << tempLineNum);
    regVal  |= (uint32) (enable << tempLineNum);
    HW_WR_REG32(MCAN_CfgAddr(baseAddr) + MCAN_ILE, regVal);
}

uint32 MCAN_getIntrStatus(uint32 baseAddr)
{

    return (HW_RD_REG32(MCAN_CfgAddr(baseAddr) + MCAN_IR));
}

void MCAN_clearIntrStatus(uint32 baseAddr, uint32 intrMask)
{

    HW_WR_REG32(MCAN_CfgAddr(baseAddr) + MCAN_IR, intrMask);
}


void MCAN_getRxFIFOStatus(uint32           baseAddr,
                          MCAN_RxFIFOStatus *fifoStatus)
{
    uint32 regVal;

    switch (fifoStatus->num)
    {
        case MCAN_RX_FIFO_NUM_0:
            regVal = HW_RD_REG32(MCAN_CfgAddr(baseAddr) + MCAN_RXF0S);
            fifoStatus->fillLvl  = HW_GET_FIELD(regVal, MCAN_RXF0S_F0FL);
            fifoStatus->getIdx   = HW_GET_FIELD(regVal, MCAN_RXF0S_F0GI);
            fifoStatus->putIdx   = HW_GET_FIELD(regVal, MCAN_RXF0S_F0PI);
            fifoStatus->fifoFull = HW_GET_FIELD(regVal, MCAN_RXF0S_F0F);
            fifoStatus->msgLost  = HW_GET_FIELD(regVal, MCAN_RXF0S_RF0L);
            break;
        case MCAN_RX_FIFO_NUM_1:
            regVal = HW_RD_REG32(MCAN_CfgAddr(baseAddr) + MCAN_RXF1S);
            fifoStatus->fillLvl  = HW_GET_FIELD(regVal, MCAN_RXF1S_F1FL);
            fifoStatus->getIdx   = HW_GET_FIELD(regVal, MCAN_RXF1S_F1GI);
            fifoStatus->putIdx   = HW_GET_FIELD(regVal, MCAN_RXF1S_F1PI);
            fifoStatus->fifoFull = HW_GET_FIELD(regVal, MCAN_RXF1S_F1F);
            fifoStatus->msgLost  = HW_GET_FIELD(regVal, MCAN_RXF1S_RF1L);
            break;
        default:
            /* Invalid option */
            break;
    }
}

int32_t MCAN_writeRxFIFOAck(uint32 baseAddr,
                            uint32 fifoNum,
                            uint32 idx)
{
    int32_t  status;
    uint32 size;

    switch (fifoNum)
    {
        case MCAN_RX_FIFO_NUM_0:
            size = HW_RD_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_RXF0C,
                                 MCAN_RXF0C_F0S);
            if (size >= idx)
            {
                HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_RXF0A,
                              MCAN_RXF0A_F0AI,
                              idx);
                status = LLD_STW_SOK;
            }
            else
            {
                status = LLD_STW_EFAIL;
            }
            break;
        case MCAN_RX_FIFO_NUM_1:
            size = HW_RD_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_RXF1C,
                                 MCAN_RXF1C_F1S);
            if (size >= idx)
            {
                HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_RXF1A,
                              MCAN_RXF1A_F1AI,
                              idx);
                status = LLD_STW_SOK;
            }
            else
            {
                status = LLD_STW_EFAIL;
            }
            break;
        default:
            status = LLD_STW_EFAIL;
            break;
    }

    return status;
}


uint32 MCAN_getTxBufReqPend(uint32 baseAddr)
{

    return (HW_RD_REG32(MCAN_CfgAddr(baseAddr) + MCAN_TXBRP));
}

int32_t MCAN_txBufCancellationReq(uint32 baseAddr, uint32 buffNum)
{
    int32_t  status;
    uint32 regVal;

    if (MCANSS_TX_BUFFER_MAX > buffNum)
    {
        regVal  = HW_RD_REG32(MCAN_CfgAddr(baseAddr) + MCAN_TXBCR);
        regVal |= ((uint32) 1U << buffNum);
        HW_WR_REG32(MCAN_CfgAddr(baseAddr) + MCAN_TXBCR, regVal);
        status = LLD_STW_SOK;
    }
    else
    {
        status = LLD_STW_EFAIL;
    }
    return status;
}

uint32 MCAN_getTxBufTransmissionStatus(uint32 baseAddr)
{

    return (HW_RD_REG32(MCAN_CfgAddr(baseAddr) + MCAN_TXBTO));
}

uint32 MCAN_txBufCancellationStatus(uint32 baseAddr)
{

    return (HW_RD_REG32(MCAN_CfgAddr(baseAddr) + MCAN_TXBCF));
}

int32_t MCAN_txBufTransIntrEnable(uint32 baseAddr,
                                  uint32 bufNum,
                                  uint32 enable)
{
    int32_t  status;
    uint32 regVal;

    if (MCANSS_TX_BUFFER_MAX > bufNum)
    {
        if (TRUE == enable)
        {
            regVal  = HW_RD_REG32(MCAN_CfgAddr(baseAddr) + MCAN_TXBTIE);
            regVal |= ((uint32) 1U << bufNum);
            HW_WR_REG32(MCAN_CfgAddr(baseAddr) + MCAN_TXBTIE, regVal);
        }
        else
        {
            regVal  = HW_RD_REG32(MCAN_CfgAddr(baseAddr) + MCAN_TXBTIE);
            regVal &= ~((uint32) 0x1U << bufNum);
            HW_WR_REG32(MCAN_CfgAddr(baseAddr) + MCAN_TXBTIE, regVal);
        }
        status = LLD_STW_SOK;
    }
    else
    {
        status = LLD_STW_EFAIL;
    }
    return status;
}


/* ========================================================================== */
/*                          Advance Functions                                 */
/* ========================================================================== */


uint32 MCAN_getClkStopAck(uint32 baseAddr)
{

    return (HW_RD_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_CCCR, MCAN_CCCR_CSA));
}

/* ========================================================================== */
/*                          Internal Functions                                */
/* ========================================================================== */

static void MCAN_writeProtectedRegAccessUnlock(uint32 baseAddr)
{

    HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_CCCR, MCAN_CCCR_CCE, 0x1U);
}

static void MCAN_writeProtectedRegAccessLock(uint32 baseAddr)
{

    HW_WR_FIELD32(MCAN_CfgAddr(baseAddr) + MCAN_CCCR, MCAN_CCCR_CCE, 0x0U);
}

static void MCAN_eccLoadRegister(uint32 baseAddr, uint32 regOffset)
{
    uint32 regVal = 0U, offset;
    uint32 eccAggrBaseAddr;

    eccAggrBaseAddr = MCAN_getECCRegionAddr (baseAddr);
    offset  = regOffset & 0xFFU;
    regVal |= ((uint32)MCANSS_MSG_RAM_NUM << MCAN_ECC_AGGR_VECTOR_SHIFT);
    regVal |= (offset << MCAN_ECC_AGGR_VECTOR_RD_SVBUS_ADDRESS_SHIFT);
    regVal |= ((uint32)1U << MCAN_ECC_AGGR_VECTOR_RD_SVBUS_SHIFT);
    HW_WR_REG32(eccAggrBaseAddr + MCAN_ECC_AGGR_VECTOR, regVal);
    while (MCAN_ECC_AGGR_VECTOR_RD_SVBUS_DONE_MASK !=
           (HW_RD_REG32(eccAggrBaseAddr + MCAN_ECC_AGGR_VECTOR) &
            MCAN_ECC_AGGR_VECTOR_RD_SVBUS_DONE_MASK))
    {}
}

static void MCAN_readMsgNoCpy(uint32           baseAddr,
                         uint32           elemAddr,
                         MCAN_RxBufElementNoCpy *elem)
{
    uint32 regVal = 0U, loopCnt = 0U;
    uint32 tempElemAddr = elemAddr;

    regVal   = HW_RD_REG32(baseAddr + tempElemAddr);
    elem->id = (uint32) ((regVal & MCANSS_RX_BUFFER_ELEM_ID_MASK)
                           >> MCANSS_RX_BUFFER_ELEM_ID_SHIFT);
    elem->rtr = (uint32) ((regVal & MCANSS_RX_BUFFER_ELEM_RTR_MASK)
                            >> MCANSS_RX_BUFFER_ELEM_RTR_SHIFT);
    elem->xtd = (uint32) ((regVal & MCANSS_RX_BUFFER_ELEM_XTD_MASK)
                            >> MCANSS_RX_BUFFER_ELEM_XTD_SHIFT);
    elem->esi = (uint32) ((regVal & MCANSS_RX_BUFFER_ELEM_ESI_MASK)
                            >> MCANSS_RX_BUFFER_ELEM_ESI_SHIFT);

    tempElemAddr  += 4U;
    regVal     = HW_RD_REG32(baseAddr + tempElemAddr);
    elem->rxts = (uint32) ((regVal & MCANSS_RX_BUFFER_ELEM_RXTS_MASK)
                             >> MCANSS_RX_BUFFER_ELEM_RXTS_SHIFT);
    elem->dlc = (uint32) ((regVal & MCANSS_RX_BUFFER_ELEM_DLC_MASK)
                            >> MCANSS_RX_BUFFER_ELEM_DLC_SHIFT);
    elem->brs = (uint32) ((regVal & MCANSS_RX_BUFFER_ELEM_BRS_MASK)
                            >> MCANSS_RX_BUFFER_ELEM_BRS_SHIFT);
    elem->fdf = (uint32) ((regVal & MCANSS_RX_BUFFER_ELEM_FDF_MASK)
                            >> MCANSS_RX_BUFFER_ELEM_FDF_SHIFT);
    elem->fidx = (uint32) ((regVal & MCANSS_RX_BUFFER_ELEM_FIDX_MASK)
                             >> MCANSS_RX_BUFFER_ELEM_FIDX_SHIFT);
    elem->anmf = (uint32) ((regVal & MCANSS_RX_BUFFER_ELEM_ANMF_MASK)
                             >> MCANSS_RX_BUFFER_ELEM_ANMF_SHIFT);
    tempElemAddr += 4U;

    /* Dest, Src, num_of_bytes */
    loopCnt = gDataSize[elem->dlc];

    /*
     * sizeof(void *) == sizeof(uint64_t) in case of 64 bit architecture while
     * sizeof(void *) == sizeof(uint32) in case of 32 bit architecture
     * typecasting to (void *) will give an error in case if proper typecasting
     * of (baseAddr + tempElemAddr) is not done before.
     */
#ifndef __aarch64__
    memcpy((void *)elem->data, (void *)(baseAddr + tempElemAddr), loopCnt);
#else
    memcpy((void *)elem->data, (void *)((uint64_t)(baseAddr + tempElemAddr)), loopCnt);
#endif
}

static void MCAN_writeMsgNoCpy(uint32                 baseAddr,
                          uint32                 elemAddr,
                          const MCAN_TxBufElementNoCpy *elem)
{
    uint32 regVal = 0, loopCnt = 0U;
    uint32 tempElemAddr = elemAddr;

    regVal  = 0U;
    regVal |= (((uint32) (elem->id << MCANSS_TX_BUFFER_ELEM_ID_SHIFT)) |
               ((uint32) (elem->rtr << MCANSS_TX_BUFFER_ELEM_RTR_SHIFT)) |
               ((uint32) (elem->xtd << MCANSS_TX_BUFFER_ELEM_XTD_SHIFT)) |
               ((uint32) (elem->esi << MCANSS_TX_BUFFER_ELEM_ESI_SHIFT)));
    HW_WR_REG32(baseAddr + tempElemAddr, regVal);
    tempElemAddr += 4U;

    regVal  = 0U;
    regVal |= ((uint32) (elem->dlc << MCANSS_TX_BUFFER_ELEM_DLC_SHIFT)) |
              ((uint32) (elem->brs << MCANSS_TX_BUFFER_ELEM_BRS_SHIFT)) |
              ((uint32) (elem->fdf << MCANSS_TX_BUFFER_ELEM_FDF_SHIFT)) |
              ((uint32) (elem->efc << MCANSS_TX_BUFFER_ELEM_EFC_SHIFT)) |
              ((uint32) (elem->mm << MCANSS_TX_BUFFER_ELEM_MM_SHIFT));
    HW_WR_REG32(baseAddr + tempElemAddr, regVal);
    tempElemAddr += 4U;

    loopCnt = 0U;
    /* Framing words out of the payload bytes and writing it to message RAM */
    
    /* CAN_CoverageGap_46: The driver code shall not utilize complete feature 
     * of the hardware to implement the Autosar requirements.So, MC/DC for this 
     * function MCAN_writeMsgNoCpy() is not covered.
     */
    
    while ((4U <= (gDataSize[elem->dlc] - loopCnt)) &&
           (0U != (gDataSize[elem->dlc] - loopCnt)))
    {
        regVal  = 0U;
        regVal |= ((uint32)elem->data[loopCnt] |
                   ((uint32)elem->data[(loopCnt + 1U)] << 8U) |
                   ((uint32)elem->data[(loopCnt + 2U)] << 16U) |
                   ((uint32)elem->data[(loopCnt + 3U)] << 24U));
        HW_WR_REG32(baseAddr + tempElemAddr, regVal);
        tempElemAddr += 4U;
        loopCnt  += 4U;
    }
    /* Framing a word out of remaining payload bytes and writing it to
     * message RAM */
    if (0U < (gDataSize[elem->dlc] - loopCnt))
    {
        regVal  = 0U;
        regVal |= ((uint32)elem->data[loopCnt] |
                   ((uint32)elem->data[(loopCnt + 1U)] << 8U) |
                   ((uint32)elem->data[(loopCnt + 2U)] << 16U) |
                   ((uint32)elem->data[(loopCnt + 3U)] << 24U));
        HW_WR_REG32(baseAddr + tempElemAddr, regVal);
    }

#if defined (BUILD_MCU)
    /**
     * The MCAN MSG RAM is configured to be non-shareable device memory instead of
     * strongly ordered memory. Add a dsb instruction after the last write
     * to be sure that before exiting the function the writes have indeed
     * done.
     * DSB instruction specific to R5 hence guard by BUILD_MCU flag
     */
    //CSL_armR5Dsb();
#endif
}

static uint32 MCAN_getECCRegionAddr(uint32 baseAddr)
{
    uint64_t eccAggrBase = 0U;
    uint8_t         baseAddrIndex=0;
    for(baseAddrIndex=0; (baseAddrIndex < (uint8_t) CAN_MAX_CONTROLLER) ; baseAddrIndex++ )
    {
        if(Can_MsgmemRAMBaseAddr[baseAddrIndex] == baseAddr )
        {
            eccAggrBase = Can_MCANECCAggrRegBaseAddr[baseAddrIndex];
            break;
        }
    }
    return (uint32) eccAggrBase;
}

static const MCAN_OffsetAddr* MCAN_getOffsetAddr (uint32 baseAddr)
{
    const MCAN_OffsetAddr *offsetAddr;
    uint8         baseAddrIndex=0;
    Std_ReturnType status = (Std_ReturnType) E_NOT_OK;

    for(baseAddrIndex=0; (baseAddrIndex < (uint8) CAN_MAX_CONTROLLER); baseAddrIndex++ )
    {
        if(Can_MsgmemRAMBaseAddr[baseAddrIndex] == baseAddr )
        {
            if((baseAddr == Can_MsgmemRAMBaseAddr[0]) || (baseAddr == Can_MsgmemRAMBaseAddr[1]))
            {
                offsetAddr = &mcuMcanOffsetAddr;
                status = (Std_ReturnType)E_OK;
            }
            else
            {
             #if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4) || defined (SOC_AM62X)
                offsetAddr = &mainMcanOffsetAddr;
                status = (Std_ReturnType)E_OK;
            #endif
            }
        }

        if(status == (Std_ReturnType)E_OK)
        {
           break;
        }
    }
    if (status == E_NOT_OK)
    {
        offsetAddr = NULL;
    }
    return offsetAddr;
}
