/*
 * SDL ECC
 *
 * Software Diagnostics Library module for ECC
 *
 *  Copyright (c) Texas Instruments Incorporated 2018-2023
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
 * @file  sdl_ecc.h
 *
 * @brief
 *  Header file contains enumerations, structure definitions and function
 *  declarations for SDL ECC interface.
 *  ============================================================================
 */

#ifndef INCLUDE_SDL_ECC_H_
#define INCLUDE_SDL_ECC_H_

#include <stdint.h>
#include <stdbool.h>

#include "sdl_common.h"
#include <src/ip/sdl_ip_ecc.h>
#include <src/ip/sdl_esm.h>

#if defined (SOC_J721E)
#define SDL_MCU_R5F0_SUB_MEMORY     SDL_MCU_R5FSS0_CORE0_ECC_AGGR_KSBUS_VBUSM2AXI0_EDC_CTRL_RAM_ID
#endif /* SOC_J721E */

#if defined (SOC_J7200)
#define SDL_MCU_R5F0_SUB_MEMORY     SDL_MCU_R5FSS0_PULSAR_SL_KSBUS_VBUSM2AXI0_EDC_CTRL_0_RAM_ID
#endif /* SOC_J7200 */

#if defined (SOC_J721S2)
#define SDL_MCU_R5F0_SUB_MEMORY     SDL_MCU_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_PULSAR_SL_KSBUS_VBUSM2AXI0_EDC_CTRL_0_RAM_ID
#endif /* SOC_J721S2 */

#if defined (SOC_J784S4)
#define SDL_MCU_R5F0_SUB_MEMORY     SDL_MCU_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_PULSAR_SL_KSBUS_VBUSM2AXI0_EDC_CTRL_0_RAM_ID
#endif /* SOC_J784S4 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * @defgroup SDL_ECC_API Error Correction Code(ECC)
 * @ingroup SDL_MODULE
 *   Provides the APIs for ECC.
 *
 */

/**
@defgroup SDL_ECC_DATASTRUCT  SDL ECC Data Structures
@ingroup SDL_ECC_API
*/
/**
@defgroup SDL_ECC_FUNCTION  SDL ECC Functions
@ingroup SDL_ECC_API
*/
/**
@defgroup SDL_ECC_ENUM SDL ECC Enumerated Data Types
@ingroup SDL_ECC_API
*/
/**
@defgroup SDL_ECC_MACROS SDL ECC Macro defines
@ingroup SDL_ECC_API
*/

/** ===========================================================================
 *  \addtogroup SDL_ECC_ENUM
 *  @{
 * ============================================================================
 */

/**
 * \brief This enumerator defines the different ECC aggregator types
 *
 */
typedef enum {
    SDL_ECC_AGGR_TYPE_INJECT_ONLY = 1,
    /**<  Ecc aggregator inject only */
    SDL_ECC_AGGR_TYPE_FULL_FUNCTION = 2,
    /**<  Ecc aggregator full funtionality */
} SDL_ECC_AggregatorType;


/**
 * \brief      ECC Inject error types
 *
 */
typedef enum {
    /** No error */
    SDL_INJECT_ECC_NO_ERROR = 0,
    /** 1-Bit ECC Error forcing once */
    SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE = 1,
    /** 2-Bit ECC Error forcing once */
    SDL_INJECT_ECC_ERROR_FORCING_2BIT_ONCE = 2,
    /** 1-Bit ECC Error Force once on next any Ram read */
    SDL_INJECT_ECC_ERROR_FORCING_1BIT_N_ROW_ONCE = 3,
    /** 2-Bit ECC Error Force once on  next Ram read */
    SDL_INJECT_ECC_ERROR_FORCING_2BIT_N_ROW_ONCE = 4,
    /** 1-Bit ECC Error forcing repeat */
    SDL_INJECT_ECC_ERROR_FORCING_1BIT_REPEAT = 5,
    /** 2-Bit ECC Error forcing repeat */
    SDL_INJECT_ECC_ERROR_FORCING_2BIT_REPEAT = 6,
    /** 1-Bit ECC Error Force repeat on next any Ram read */
    SDL_INJECT_ECC_ERROR_FORCING_1BIT_N_ROW_REPEAT = 7,
    /** 2-Bit ECC Error Force repeat on  next Ram read */
    SDL_INJECT_ECC_ERROR_FORCING_2BIT_N_ROW_REPEAT = 8,
} SDL_ECC_InjectErrorType;


/**
 * \brief This enumerator defines the different ECC RAM ID types
 *
 */
typedef enum {
    SDL_ECC_RAM_ID_TYPE_WRAPPER = 0,
    /**<  Ecc RAM ID Wrapper type */
    SDL_ECC_RAM_ID_TYPE_INTERCONNECT = 1,
    /**<  Ecc RAM ID Interconnect/CBASS type */
} SDL_ECC_RamIdType;

/* @} */

/**
 *  \addtogroup SDL_ECC_MACROS
 *  @{
 */

/**
 * \brief This enumerator indicate ECC memory type
 *
 *
 */
typedef uint32_t SDL_ECC_MemType;

#if defined (SOC_J721E)

    #define SDL_ECC_MEMTYPE_MCU_R5F0_CORE 								(0u)
    #define SDL_ECC_MEMTYPE_MCU_R5F1_CORE 								(1u)
    #define SDL_ECC_MEMTYPE_MCU_ADC0 									(2u)
    #define SDL_ECC_MEMTYPE_MCU_ADC1 									(3u)
    #define SDL_ECC_MEMTYPE_MCU_CPSW0 									(4u)
    #define SDL_ECC_MEMTYPE_MCU_FSS0_HPB0  								(5u)
    #define SDL_ECC_MEMTYPE_MCU_FSS0_OSPI0 								(6u)
    #define SDL_ECC_MEMTYPE_MCU_FSS0_OSPI1 								(7u)

    #define SDL_ECC_MEMTYPE_MCU_MCAN0     								(8u)
    #define SDL_ECC_MEMTYPE_MCU_MCAN1 									(9u)
    #define SDL_ECC_MEMTYPE_MCU_MSRAM0 									(10u)
    #define SDL_ECC_MEMTYPE_MCU_NAVSS0 									(11u)

    #define SDL_ECC_MEMTYPE_MCU_PSRAM0 									(12u)
    #define SDL_ECC_MEMTYPE_MCU_CBASS_ECC_AGGR0 			 			(13u)

    #define SDL_MCAN0_MCANSS_MSGMEM_WRAP_ECC_AGGR           			(14u)
    #define SDL_MCAN1_MCANSS_MSGMEM_WRAP_ECC_AGGR           			(15u)
    #define SDL_MCAN2_MCANSS_MSGMEM_WRAP_ECC_AGGR           			(16u)
    #define SDL_MCAN3_MCANSS_MSGMEM_WRAP_ECC_AGGR           			(17u)
    #define SDL_MCAN4_MCANSS_MSGMEM_WRAP_ECC_AGGR           			(18u)
    #define SDL_MCAN5_MCANSS_MSGMEM_WRAP_ECC_AGGR           			(19u)
    #define SDL_MCAN6_MCANSS_MSGMEM_WRAP_ECC_AGGR           			(20u)
    #define SDL_MCAN7_MCANSS_MSGMEM_WRAP_ECC_AGGR           			(21u)
    #define SDL_MCAN8_MCANSS_MSGMEM_WRAP_ECC_AGGR           			(22u)
    #define SDL_MCAN9_MCANSS_MSGMEM_WRAP_ECC_AGGR           			(23u)
    #define SDL_MCAN10_MCANSS_MSGMEM_WRAP_ECC_AGGR          			(24u)
    #define SDL_MCAN11_MCANSS_MSGMEM_WRAP_ECC_AGGR          			(25u)
    #define SDL_MCAN12_MCANSS_MSGMEM_WRAP_ECC_AGGR          			(26u)
    #define SDL_MCAN13_MCANSS_MSGMEM_WRAP_ECC_AGGR          			(27u)

    #define SDL_MSRAM_512K0_MSRAM16KX256E_ECC_AGGR          			(28u)
    #define SDL_PCIE0_ECC_AGGR_CORE_AXI_0                               (29u)
    #define SDL_PCIE0_ECC_AGGR_CORE_0                                   (30u)
    #define SDL_PCIE1_ECC_AGGR_CORE_AXI_0                               (31u)
    #define SDL_PCIE1_ECC_AGGR_CORE_0                                   (32u)
    #define SDL_PCIE2_ECC_AGGR_CORE_AXI_0                               (33u)
    #define SDL_PCIE2_ECC_AGGR_CORE_0                                   (34u)
    #define SDL_PCIE3_ECC_AGGR_CORE_AXI_0                               (35u)
    #define SDL_PCIE3_ECC_AGGR_CORE_0                                   (36u)

    #define SDL_I3C0_I3C_S_ECC_AGGR   									(37u)
    #define SDL_I3C0_I3C_P_ECC_AGGR   									(38u)
    #define SDL_MCU_I3C0_I3C_P_ECC_AGGR   								(39u)
    #define SDL_MCU_I3C0_I3C_S_ECC_AGGR   								(40u)
    #define SDL_MCU_I3C1_I3C_P_ECC_AGGR   								(41u)
    #define SDL_MCU_I3C1_I3C_S_ECC_AGGR   								(42u)
    #define SDL_PRU_ICSSG0_ICSS_G_CORE_BORG_ECC_AGGR   					(43u)
    #define SDL_PRU_ICSSG1_ICSS_G_CORE_BORG_ECC_AGGR   					(44u)

    #define SDL_CBASS_ECC_AGGR0   										(45u)
    #define SDL_MAIN_RC_ECC_AGGR0 										(46u)


    #define SDL_NAVSS_NBSS_ECC_AGGR0_NAVSS512L_NBSS_PHYS_ECC_AGGR   	(47u)
    #define SDL_DMPAC0_ECC_AGGR   										(48u)
    #define SDL_MAIN_HC_ECC_AGGR0  								 		(49u)
    #define SDL_VPAC0_ECC_AGGR											(50u)
    #define SDL_VPAC0_VISS_ECC_AGGR        								(51u)
    #define SDL_VPAC0_LDC_ECC_AGGR         								(52u)
    #define SDL_R5FSS0_CORE0_ECC_AGGR      								(53u)
    #define SDL_R5FSS1_CORE0_ECC_AGGR      								(54u)
    #define SDL_R5FSS0_CORE1_ECC_AGGR      								(55u)
    #define SDL_R5FSS1_CORE1_ECC_AGGR      								(56u)
    #define SDL_NAVSS_VIRTSS_ECC_AGGR0   								(57u)

    #define SDL_MCU_SA2_UL0_SA2_UL_SA2_UL_ECC_AGGR  					(58u)
    #define SDL_SA2_UL0_SA2_UL_SA2_UL_ECC_AGGR   						(59u)

    #define SDL_MLB0_MLBSS2P0_MLBDIM_WRAP_ECC_AGGR   					(60u)
    #define SDL_MAIN_AC_ECC_AGGR0  									    (61u)

    #define SDL_WKUP_VTM0_K3VTM_NC_ECCAGGR   	 						(62u)
    #define SDL_MMCSD0_EMMC8SSC_ECC_AGGR_RXMEM  						(63u)
    #define SDL_MMCSD0_EMMC8SSC_ECC_AGGR_TXMEM  						(64u)
    #define SDL_MMCSD1_EMMCSD4SS_ECC_AGGR_RXMEM 						(65u)
    #define SDL_MMCSD1_EMMCSD4SS_ECC_AGGR_TXMEM 						(66u)
    #define SDL_MMCSD2_EMMCSD4SS_ECC_AGGR_RXMEM 						(67u)
    #define SDL_MMCSD2_EMMCSD4SS_ECC_AGGR_TXMEM 						(68u)
    #define SDL_DSS_EDP0_K3_DSS_EDP_MHDPTX_WRAPPER_ECC_AGGR_CORE 		(69u)
    #define SDL_DSS_EDP0_K3_DSS_EDP_MHDPTX_WRAPPER_ECC_AGGR_PHY  		(70u)
    #define SDL_DSS_EDP0_K3_DSS_EDP_MHDPTX_WRAPPER_ECC_AGGR_DSC  		(71u)
    #define SDL_UFS0_UFSHCI2P1SSC_HCLK_ECC_AGGR  						(72u)
    #define SDL_CSI_RX_IF0_ECC_AGGR_0 									(73u)
    #define SDL_CSI_RX_IF1_ECC_AGGR_0 									(74u)
    #define SDL_NAVSS0_MODSS_ECC_AGGR0  								(75u)
    #define SDL_USB0_USB3P0SSC_USB3P0SS_CORE_A_ECC_AGGR   				(76u)
    #define SDL_USB1_USB3P0SSC_USB3P0SS_CORE_A_ECC_AGGR   				(77u)
    #define SDL_NAVSS0_VIRTSS_ECC_AGGR0 								(78u)
    #define SDL_NAVSS0_NBSS_ECC_AGGR0   								(79u)
    #define SDL_IDOM1_ECC_AGGR0 										(80u)
    #define SDL_IDOM1_ECC_AGGR1 										(81u)
    #define SDL_IDOM0_ECC_AGGR0_IDOM0_ECC_AGGR   						(82u)
    #define SDL_IDOM0_ECC_AGGR1_IDOM0_ECC_AGGR   						(83u)
    #define SDL_CSI_TX_IF0_CSI_TX_IF_ECC_AGGR   						(84u)
    #define SDL_CSI_TX_IF0_CSI_TX_IF_ECC_AGGR_BYTE 					    (85u)
    #define SDL_WKUP_CBASS_ECC_AGGR0_WAKEUP_ECC_AGGR 					(86u)
    #define SDL_DSS_DSI0_K3_DSS_DSI_TOP_ECC_AGGR_SYS 					(87u)
    #define SDL_MCU_NAVSS0_UDMASS_ECC_AGGR0 							(88u)
    #define SDL_PDMA5_PDMA_MAIN_MCAN_ECCAGGR 							(89u)
    #define SDL_PSRAMECC0_PSRAM256X32EC_ECC_AGGR						(90u)
    #define SDL_NAVSS0_UDMASS_ECC_AGGR0                                 (91u)


    #define SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0 							(92u)
    #define SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR1 							(93u)
    #define SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR2 							(94u)
    #define SDL_COMPUTE_CLUSTER0_A72SS0_COMMON_ECC_AGGR 				(95u)
    #define SDL_COMPUTE_CLUSTER0_A72SS0_CORE0_ECC_AGGR 					(96u)
    #define SDL_COMPUTE_CLUSTER0_A72SS0_CORE1_ECC_AGGR 					(97u)
    #define SDL_COMPUTE_CLUSTER0_DDR32SSC_EW_BRCTL_SC_ECC_AGGR_VBUS 	(98u)
    #define SDL_COMPUTE_CLUSTER0_DDR32SSC_EW_BRCTL_SC_ECC_AGGR_CTL  	(99u)
    #define SDL_COMPUTE_CLUSTER0_DDR32SSC_EW_BRCTL_SC_ECC_AGGR_CFG  	(100u)
    #define SDL_COMPUTE_CLUSTER0_C71SS0_ECC_AGGR 						(101u)

#define SDL_ECC_MEMTYPE_MAX (SDL_COMPUTE_CLUSTER0_C71SS0_ECC_AGGR + 1U)


#endif

#if defined (SOC_J7200)

#define SDL_ECC_MEMTYPE_MCU_R5F0_CORE                                   (0u)
#define SDL_ECC_MEMTYPE_MCU_R5F1_CORE                                   (1u)
#define SDL_ECC_PDMA10_ECC_AGGR                                         (2u)
#define SDL_ECC_PCIE1_1_ECC_AGGR                                        (3u)
#define SDL_ECC_MCU_NAVSS0_0_ECC_AGGR                                   (4u)
#define SDL_ECC_MCU_NAVSS0_1_ECC_AGGR                                   (5u)
#define SDL_ECC_PCIE1_0_ECC_AGGR                                        (6u)
#define SDL_ECC_USB0_ECC_AGGR                                           (7u)
#define SDL_ECC_CPSW0_ECC_AGGR                                          (8u)
#define SDL_ECC_MMCSD0_1_ECC_AGGR                                       (9u)
#define SDL_ECC_MMCSD0_0_ECC_AGGR                                       (10u)
#define SDL_ECC_MMCSD1_0_ECC_AGGR                                       (11u)
#define SDL_ECC_MMCSD1_1_ECC_AGGR                                       (12u)
#define SDL_ECC_MSRAM_512K0_ECC_AGGR                                    (13u)
#define SDL_ECC_MCAN8_ECC_AGGR                                          (14u)
#define SDL_ECC_MCAN9_ECC_AGGR                                          (15u)
#define SDL_ECC_MCAN10_ECC_AGGR                                         (16u)
#define SDL_ECC_MCAN11_ECC_AGGR                                         (17u)
#define SDL_ECC_MCAN12_ECC_AGGR                                         (18u)
#define SDL_ECC_MCAN13_ECC_AGGR                                         (19u)
#define SDL_ECC_MCAN14_ECC_AGGR                                         (20u)
#define SDL_ECC_MCAN15_ECC_AGGR                                         (21u)
#define SDL_ECC_MCAN16_ECC_AGGR                                         (22u)
#define SDL_ECC_MCAN17_ECC_AGGR                                         (23u)
#define SDL_R5FSS0_CORE0_ECC_AGGR                                       (24u)  
#define SDL_ECC_I3C0_1_ECC_AGGR                                         (25u)
#define SDL_ECC_I3C0_0_ECC_AGGR                                         (26u)
#define SDL_ECC_MCAN0_ECC_AGGR                                          (27u)
#define SDL_ECC_MCAN1_ECC_AGGR                                          (28u)
#define SDL_ECC_MCAN2_ECC_AGGR                                          (29u)
#define SDL_ECC_MCAN3_ECC_AGGR                                          (30u)
#define SDL_ECC_MCAN4_ECC_AGGR                                          (31u)
#define SDL_ECC_MCAN5_ECC_AGGR                                          (32u)
#define SDL_ECC_MCAN6_ECC_AGGR                                          (33u)
#define SDL_ECC_MCAN7_ECC_AGGR                                          (34u)
#define SDL_ECC_IDOM0_ECC_AGGR16                                        (35u)
#define SDL_ECC_IDOM1_ECC_AGGR17                                        (36u)
#define SDL_ECC_VC_MAIN_RC_ECC_AGGR4_ECC_AGGR   						(37u)
#define SDL_ECC_VC_MAIN_HC_ECC_AGGR5_ECC_AGGR   						(38u)
#define SDL_ECC_VC_MAIN_SPI_G0_MAIN_0_ECCAGGR6   						(39u)
#define SDL_ECC_VCL_NAVSS256_VIRTSS_PHYS_ECC_AGGR10_ECC_AGGR   			(40u)
#define SDL_ECC_VC_NAVSS256_NBSS_PHYS_ECC_AGGR11_ECC_AGGR   			(41u)
#define SDL_ECC_NAVSS0_0_ECC_AGGR   									(42u)
#define SDL_ECC_NAVSS0_1_ECC_AGGR   									(43u)
#define SDL_ECC_NAVSS0_3_ECC_AGGR   									(44u)
#define SDL_ECC_NAVSS0_2_ECC_AGGR   									(45u)
#define SDL_ECC_PDMA5_ECC_AGGR 								         	(46u)
#define SDL_ECC_PDMA9_ECC_AGGR 									    	(47u)
#define SDL_ECC_MCU_MCAN0_ECC_AGGR   									(48u)
#define SDL_ECC_MCU_MCAN1_ECC_AGGR  								 	(49u)
#define SDL_ECC_MCU_ADC0_ECC_AGGR										(50u)
#define SDL_ECC_MCU_CPSW0_ECC_AGGR         								(51u)
#define SDL_ECC_MCU_MSRAM_1MB0_ECC_AGGR      							(52u)
#define SDL_ECC_MCU_SA2_UL0_ECC_AGGR      								(53u)
#define SDL_ECC_MCU_I3C0_1_ECC_AGGR      								(54u)
#define SDL_ECC_MCU_I3C0_0_ECC_AGGR      								(55u)
#define SDL_ECC_MCU_I3C1_1_ECC_AGGR   									(56u)
#define SDL_ECC_MCU_I3C1_0_ECC_AGGR   									(57u)
#define SDL_ECC_WKUP_VTM0_ECC_AGGR  									(58u)
#define SDL_ECC_MCU_FSS0_0_ECC_AGGR   									(59u)
#define SDL_ECC_MCU_FSS0_1_ECC_AGGR   	 						        (60u)
#define SDL_ECC_MCU_VC_MCU_ECC_AGGR0  									(61u)
#define SDL_R5FSS0_CORE1_ECC_AGGR  										(62u)  
#define SDL_ECC_PSRAMECC0_ECC_AGGR 										(63u)  
#define SDL_ECC_PSRAM2KECC0_ECC_AGGR 									(64u)
#define SDL_ECC_VCL_MAIN_INFRA_ECC_AGGR0_0_ECC_AGGR        				(65u)

#define SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0        					        (66u)  
#define SDL_ECC_COMPUTE_CLUSTER0_2_ECC_AGGR							    (67u)
#define SDL_ECC_COMPUTE_CLUSTER0_4_ECC_AGGR 							(68u)
#define SDL_ECC_COMPUTE_CLUSTER0_5_ECC_AGGR							    (69u)
#define SDL_ECC_COMPUTE_CLUSTER0_1_ECC_AGGR	 						    (70u)
#define SDL_ECC_COMPUTE_CLUSTER0_0_ECC_AGGR							    (71u)
#define SDL_DDR0_0_ECC_AGGR											    (72u)
#define SDL_DDR0_1_ECC_AGGR											    (73u)
#define SDL_DDR0_2_ECC_AGGR  											(74u)
#define SDL_ECC_COMPUTE_CLUSTER0_6_ECC_AGGR  							(75u)


#define SDL_ECC_MEMTYPE_MAX (SDL_ECC_COMPUTE_CLUSTER0_6_ECC_AGGR + 1U)


/* Max entries based on max mem type */
#define SDL_ECC_AGGREGATOR_MAX_LOW_ENTRIES    (SDL_ECC_VCL_MAIN_INFRA_ECC_AGGR0_0_ECC_AGGR+1U)

#define SDL_ECC_AGGREGATOR_MAX_HIGH_ENTRIES (SDL_ECC_COMPUTE_CLUSTER0_6_ECC_AGGR - \
                                           SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0 + 1u)

#endif

#if defined (SOC_J721S2)

#define   SDL_ECC_MEMTYPE_MCU_R5F0_CORE          (0U)        
#define   SDL_ECC_MEMTYPE_MCU_R5F1_CORE          (1U)        
#define   SDL_IVC_DOM1_ECC_AGGR17_ECC_AGGR       (2U)
#define   SDL_WKUP_VTM0_ECC_AGGR                 (3U)
#define   SDL_MSRAM_512K0_ECC_AGGR               (4U)
#define   SDL_MSRAM_512K1_ECC_AGGR               (5U)
#define   SDL_PSRAMECC0_ECC_AGGR                 (6U)
#define   SDL_MCU_MSRAM_1MB0_ECC_AGGR            (7U)
#define   SDL_ECC_AGGR4_ECC_AGGR                 (8U)
#define   SDL_USB0_ECC_AGGR                      (9U)
#define   SDL_MCU_I3C1_0_ECC_AGGR                (10U)
#define   SDL_MCU_I3C1_1_ECC_AGGR                (11U)
#define   SDL_VPAC0_0_ECC_AGGR                   (12U)
#define   SDL_VPAC0_1_ECC_AGGR                   (13U)
#define   SDL_VPAC0_ECC_AGGR                     (14U)
#define   SDL_MCAN16_ECC_AGGR                    (15U)
#define   SDL_PCIE1_0_ECC_AGGR                   (16U)
#define   SDL_PCIE1_1_ECC_AGGR                   (17U) 
#define   SDL_IVC_DOM0_ECC_AGGR16_ECC_AGGR       (18U) 
#define   SDL_PDMA6_ECC_AGGR                     (19U)
#define   SDL_ECC_AGGR5_ECC_AGGR                 (20U)
#define   SDL_DSS_DSI0_ECC_AGGR                  (21U)
#define   SDL_ECC_AGGR0_ECC_AGGR                 (22U)
#define   SDL_CSI_RX_IF0_ECC_AGGR                (23U)
#define   SDL_MCAN5_ECC_AGGR                     (24U)
#define   SDL_PSRAM2KECC0_ECC_AGGR               (25U)
#define   SDL_PDMA7_ECC_AGGR                     (26U)
#define   SDL_IVC_DOM0_ECC_AGGR18_ECC_AGGR       (27U) 
#define   SDL_MCU_NAVSS0_0_ECC_AGGR              (28U) 
#define   SDL_MCU_NAVSS0_1_ECC_AGGR              (29U)
#define   SDL_DSS_EDP0_2_ECC_AGGR                (30U)
#define   SDL_DSS_EDP0_0_ECC_AGGR                (31U)
#define   SDL_DSS_EDP0_1_ECC_AGGR                (32U)
#define   SDL_MCAN10_ECC_AGGR                    (33U)
#define   SDL_SA2_UL0_ECC_AGGR                   (34U)
#define   SDL_MMCSD0_0_ECC_AGGR                  (35U)
#define   SDL_MMCSD0_1_ECC_AGGR                  (36U)
#define   SDL_ECC_AGGR10_ECC_AGGR                (37U)
#define   SDL_CSI_TX_IF_V2_1_0_ECC_AGGR          (38U)
#define   SDL_CSI_TX_IF_V2_1_1_ECC_AGGR          (39U)
#define   SDL_CSI_TX_IF_V2_0_0_ECC_AGGR          (40U)
#define   SDL_CSI_TX_IF_V2_0_1_ECC_AGGR          (41U)
#define   SDL_MCU_MCAN0_ECC_AGGR                 (42U)
#define   SDL_MCU_ADC12FCC1_ECC_AGGR             (43U)
#define   SDL_MCU_ADC12FCC0_ECC_AGGR             (44U)
#define   SDL_CSI_RX_IF1_ECC_AGGR                (45U)
#define   SDL_NAVSS0_0_ECC_AGGR                  (46U)
#define   SDL_NAVSS0_1_ECC_AGGR                  (47U)
#define   SDL_NAVSS0_2_ECC_AGGR                  (48U)
#define   SDL_NAVSS0_3_ECC_AGGR                  (49U)
#define   SDL_MAIN_IP_ECC_AGGR0_ECC_AGGR         (50U)
#define   SDL_MCU_MCAN1_ECC_AGGR                 (51U)
#define   SDL_CPSW1_ECC_AGGR                     (52U)
#define   SDL_MMCSD1_0_ECC_AGGR                  (53U)
#define   SDL_MMCSD1_1_ECC_AGGR                  (54U)
#define   SDL_MCUM_MCU_ECC_AGGR0_ECC_AGGR        (55U)
#define   SDL_MCU_FSS0_0_ECC_AGGR                (56U)
#define   SDL_MCU_FSS0_1_ECC_AGGR                (57U)
#define   SDL_MCU_FSS0_2_ECC_AGGR                (58U)
#define   SDL_WKUP_SMS0_TIFS_ECC_AGGR_0_ECC_AGGR (59U)
#define   SDL_MCU_CPSW0_ECC_AGGR                 (60U)
#define   SDL_MCU_I3C0_0_ECC_AGGR                (61U) 
#define   SDL_MCU_I3C0_1_ECC_AGGR                (62U)
#define   SDL_R5FSS0_0_ECC_AGGR                  (63U)
#define   SDL_R5FSS0_1_ECC_AGGR                  (64U)
#define   SDL_R5FSS1_0_ECC_AGGR                  (65U)
#define   SDL_R5FSS1_1_ECC_AGGR                  (66U)
#define   SDL_DMPAC0_ECC_AGGR                    (67U)
#define   SDL_ECC_AGGR9_ECC_AGGR                 (68U)
#define   SDL_ECC_AGGR6_ECC_AGGR                 (69U)
#define   SDL_MCAN9_ECC_AGGR                     (70U)
#define   SDL_MCAN8_ECC_AGGR                     (71U)
#define   SDL_MCAN1_ECC_AGGR                     (72U)
#define   SDL_MCAN3_ECC_AGGR                     (73U)
#define   SDL_MCAN2_ECC_AGGR                     (74U)
#define   SDL_MCAN7_ECC_AGGR                     (75U)
#define   SDL_MCAN6_ECC_AGGR                     (76U)
#define   SDL_DSS_DSI1_ECC_AGGR                  (77U)
#define   SDL_IVC_DOM1_ECC_AGGR19_ECC_AGGR       (78U)
#define   SDL_WKUP_SMS0_HSM_ECC_AGGR_0_ECC_AGGR  (79U)
#define   SDL_PDMA5_ECC_AGGR                     (80U)
#define   SDL_MCAN11_ECC_AGGR                    (81U) 
#define   SDL_MCAN13_ECC_AGGR                    (82U)
#define   SDL_MCAN12_ECC_AGGR                    (83U)
#define   SDL_MCAN15_ECC_AGGR                    (84U)
#define   SDL_MCAN14_ECC_AGGR                    (85U)
#define   SDL_MCAN17_ECC_AGGR                    (86U)
#define   SDL_MCAN0_ECC_AGGR                     (87U)
#define   SDL_ECC_AGGR11_ECC_AGGR                (88U)
#define   SDL_MCAN4_ECC_AGGR                     (89U)
#define   SDL_WKUP_ECC_AGGR0_ECC_AGGR            (90U)
#define   SDL_MCU_SA3_SS0_1_ECC_AGGR             (91U)

#define   SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0        (92U)
#define   SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR1        (93U)
#define   SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR2        (94U)
#define   SDL_ECC_MEMTYPE_A72_COREPAC            (95U)
#define   SDL_ECC_MEMTYPE_A72_1                  (96U)
#define   SDL_ECC_MEMTYPE_C7X_1                  (97U)
#define   SDL_DDR0_0_ECC_AGGR                    (98U)
#define   SDL_DDR1_0_ECC_AGGR                    (99U)
#define   SDL_DDR0_1_ECC_AGGR                    (100U)
#define   SDL_DDR1_1_ECC_AGGR                    (101U)
#define   SDL_DDR0_2_ECC_AGGR                    (102U)
#define   SDL_DDR1_2_ECC_AGGR                    (103U)
#define   SDL_ECC_MEMTYPE_C7X_0                  (104U)
#define   SDL_ECC_MEMTYPE_A72_0                  (105U)
#define   SDL_ECC_MEMTYPE_GIC                    (106U)
#define   SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR3        (107U)

#define SDL_ECC_MEMTYPE_MAX (SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR3 + 1U)

/* Max entries based on max mem type */
#define SDL_ECC_AGGREGATOR_MAX_LOW_ENTRIES    (SDL_MCU_SA3_SS0_1_ECC_AGGR+1U)

#define SDL_ECC_AGGREGATOR_MAX_HIGH_ENTRIES (SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR3 - \
                                           SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0 + 1u)

#endif

#if defined (SOC_J784S4)

#define   SDL_ECC_MEMTYPE_MCU_R5F0_CORE                       (0U)	/* MCU_R5FSS0_0 */
#define   SDL_ECC_MEMTYPE_MCU_R5F1_CORE                       (1U)	/* MCU_R5FSS0_1 */
#define   SDL_PCIE0_0_ECC_AGGR                                (2U)
#define   SDL_PCIE0_1_ECC_AGGR                                (3U)
#define   SDL_PCIE3_0_ECC_AGGR                                (4U)
#define   SDL_PCIE3_1_ECC_AGGR                                (5U)
#define   SDL_PCIE2_0_ECC_AGGR                                (6U)
#define   SDL_PCIE2_1_ECC_AGGR                                (7U)
#define   SDL_MCU_CPSW0_ECC_AGGR                              (8U)
#define   SDL_WKUP_SMS0_TIFS_ECC_AGGR_0_ECC_AGGR              (9U)
#define   SDL_WKUP_SMS0_HSM_ECC_AGGR_0_ECC_AGGR               (10U)
#define   SDL_MSRAM_512K1_ECC_AGGR                            (11U)
#define   SDL_MSRAM_512K2_ECC_AGGR                            (12U)
#define   SDL_PDMA6_ECC_AGGR                                  (13U)
#define   SDL_DSS_DSI0_ECC_AGGR                               (14U)
#define   SDL_MCU_MSRAM_1MB0_ECC_AGGR                         (15U)
#define   SDL_ECC_AGGR4_ECC_AGGR                              (16U)
#define   SDL_USB0_ECC_AGGR                                   (17U)
#define   SDL_VPAC1_0_ECC_AGGR                                (18U)
#define   SDL_VPAC1_1_ECC_AGGR                                (19U) 
#define   SDL_VPAC1_2_ECC_AGGR                                (20U)
#define   SDL_ECC_AGGR6_ECC_AGGR                              (21U)
#define   SDL_MCAN16_ECC_AGGR                                 (22U)
#define   SDL_UFS0_ECC_AGGR                                   (23U)
#define   SDL_IVC_DOM0_ECC_AGGR20_ECC_AGGR                    (24U)
#define   SDL_PCIE1_0_ECC_AGGR                                (25U)
#define   SDL_PCIE1_1_ECC_AGGR                                (26U)
#define   SDL_ECC_AGGR5_ECC_AGGR                              (27U)
#define   SDL_IVC_DOM0_ECC_AGGR16_ECC_AGGR                    (28U)
#define   SDL_VPAC0_0_ECC_AGGR                                (29U)
#define   SDL_VPAC0_1_ECC_AGGR                                (30U)
#define   SDL_VPAC0_2_ECC_AGGR                                (31U)
#define   SDL_MCU_MCAN0_ECC_AGGR                              (32U)
#define   SDL_PSRAMECC0_ECC_AGGR                              (33U)
#define   SDL_MCU_MCAN1_ECC_AGGR                              (34U)
#define   SDL_MCU_SA3_SS0_0_ECC_AGGR                          (35U)
#define   SDL_MCU_SA3_SS0_1_ECC_AGGR                          (36U)
#define   SDL_MMCSD1_0_ECC_AGGR                               (37U)
#define   SDL_MMCSD1_1_ECC_AGGR                               (38U)
#define   SDL_MCAN4_ECC_AGGR                                  (39U)
#define   SDL_IVC_DOM1_ECC_AGGR21_ECC_AGGR                    (40U)
#define   SDL_IVC_DOM1_ECC_AGGR17_ECC_AGGR                    (41U)
#define   SDL_MCU_I3C1_0_ECC_AGGR                             (42U)
#define   SDL_MCU_I3C1_1_ECC_AGGR                             (43U)
#define   SDL_ECC_AGGR0_ECC_AGGR                              (44U)
#define   SDL_CSI_RX_IF2_ECC_AGGR                             (45U)
#define   SDL_CSI_RX_IF1_ECC_AGGR                             (46U)
#define   SDL_CSI_RX_IF0_ECC_AGGR                             (47U)
#define   SDL_PSRAM2KECC0_ECC_AGGR                            (48U)
#define   SDL_PDMA7_ECC_AGGR                                  (49U)
#define   SDL_ECC_AGGR11_ECC_AGGR                             (50U)
#define   SDL_IVC_DOM0_ECC_AGGR18_ECC_AGGR                    (51U)
#define   SDL_MCU_NAVSS0_0_ECC_AGGR                           (52U)
#define   SDL_MCU_NAVSS0_1_ECC_AGGR                           (53U)
#define   SDL_DSS_EDP0_0_ECC_AGGR                             (54U)
#define   SDL_DSS_EDP0_1_ECC_AGGR                             (55U)
#define   SDL_DSS_EDP0_2_ECC_AGGR                             (56U)
#define   SDL_MCAN10_ECC_AGGR        			              (57U)
#define   SDL_SA2_UL0_ECC_AGGR                                (58U)
#define   SDL_MMCSD0_0_ECC_AGGR                               (59U)
#define   SDL_MMCSD0_1_ECC_AGGR                               (60U)
#define   SDL_ECC_AGGR10_ECC_AGGR                             (61U)
#define   SDL_CSI_TX_IF_V2_1_0_ECC_AGGR                       (62U)
#define   SDL_CSI_TX_IF_V2_1_1_ECC_AGGR                       (63U) 
#define   SDL_CSI_TX_IF_V2_0_0_ECC_AGGR                       (64U)
#define   SDL_CSI_TX_IF_V2_0_1_ECC_AGGR                       (65U)
#define   SDL_NAVSS0_0_ECC_AGGR                               (66U)
#define   SDL_NAVSS0_1_ECC_AGGR                               (67U)
#define   SDL_NAVSS0_2_ECC_AGGR                               (68U)
#define   SDL_NAVSS0_3_ECC_AGGR                               (69U)
#define   SDL_MCU_ADC12FCC1_ECC_AGGR                          (70U)
#define   SDL_MCU_ADC12FCC0_ECC_AGGR                          (71U)
#define   SDL_MCAN14_ECC_AGGR                                 (72U)
#define   SDL_MAIN_IP_ECC_AGGR0_ECC_AGGR                      (73U)
#define   SDL_WKUP_VTM0_ECC_AGGR                              (74U)
#define   SDL_MSRAM_512K0_ECC_AGGR                            (75U)
#define   SDL_CPSW1_ECC_AGGR                                  (76U)
#define   SDL_MCUM_MCU_ECC_AGGR0_ECC_AGGR                     (77U)
#define   SDL_MCU_FSS0_0_ECC_AGGR                             (78U)
#define   SDL_MCU_FSS0_1_ECC_AGGR                             (79U)
#define   SDL_MCU_FSS0_2_ECC_AGGR                             (80U)
#define   SDL_MCU_I3C0_0_ECC_AGGR                             (81U)
#define   SDL_MCU_I3C0_1_ECC_AGGR                             (82U) 
#define   SDL_R5FSS0_0_ECC_AGGR                               (83U)
#define   SDL_R5FSS0_1_ECC_AGGR                               (84U)
#define   SDL_R5FSS1_0_ECC_AGGR                               (85U)
#define   SDL_R5FSS1_1_ECC_AGGR                               (86U)
#define   SDL_R5FSS2_0_ECC_AGGR                               (87U)
#define   SDL_R5FSS2_1_ECC_AGGR                               (88U)
#define   SDL_PDMA5_ECC_AGGR                                  (89U)
#define   SDL_MCAN5_ECC_AGGR                                  (90U)
#define   SDL_MCAN9_ECC_AGGR                                  (91U)
#define   SDL_MCAN8_ECC_AGGR                                  (92U)
#define   SDL_MCAN1_ECC_AGGR                                  (93U)
#define   SDL_MCAN3_ECC_AGGR                                  (94U)
#define   SDL_MCAN2_ECC_AGGR                                  (95U)
#define   SDL_MCAN7_ECC_AGGR                                  (96U)                   
#define   SDL_MCAN6_ECC_AGGR                                  (97U)              
#define   SDL_DSS_DSI1_ECC_AGGR                               (98U)                
#define   SDL_IVC_DOM1_ECC_AGGR19_ECC_AGGR                    (99U)
#define   SDL_MCAN11_ECC_AGGR                                 (100U)
#define   SDL_MCAN13_ECC_AGGR                                 (101U)
#define   SDL_MCAN12_ECC_AGGR                                 (102U)
#define   SDL_MCAN15_ECC_AGGR                                 (103U)
#define   SDL_MCAN17_ECC_AGGR                                 (104U)
#define   SDL_ECC_AGGR9_ECC_AGGR                              (105U)
#define   SDL_MCAN0_ECC_AGGR                                  (106U)             
#define   SDL_WKUP_ECC_AGGR0_ECC_AGGR                         (107U)          
#define   SDL_DMPAC0_ECC_AGGR                                 (108U)
#define   SDL_CPSW_9XUSSM0_ECC_AGGR                           (109U)

#define   SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0                     (110U)
#define   SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR1                     (111U)
#define   SDL_COMPUTE_CLUSTER0_MSMC_ECC_AGGR2                 (112U)
#define   SDL_COMPUTE_CLUSTER0_MSMC_ECC_AGGR3                 (113U)
#define   SDL_COMPUTE_CLUSTER0_MSMC_ECC_AGGR4                 (114U)
#define   SDL_COMPUTE_CLUSTER0_MSMC_ECC_AGGR5                 (115U)
#define   SDL_COMPUTE_CLUSTER0_GICSS_ECC_AGGR                 (116U)
#define   SDL_COMPUTE_CLUSTER0_AW4_ECC0_AGGR                  (117U)
#define   SDL_COMPUTE_CLUSTER0_AW4_ECC1_AGGR                  (118U)
#define   SDL_COMPUTE_CLUSTER0_DSP0_ECC_AGGR                  (119U)
#define   SDL_DDR0_0_ECC_AGGR                                 (120U)
#define   SDL_DDR0_1_ECC_AGGR                                 (121U)
#define   SDL_DDR0_2_ECC_AGGR                                 (122U)
#define   SDL_DDR1_0_ECC_AGGR                                 (123U)
#define   SDL_DDR1_1_ECC_AGGR                                 (124U)
#define   SDL_DDR1_2_ECC_AGGR                                 (125U)
#define   SDL_DDR2_0_ECC_AGGR                                 (126U)
#define   SDL_DDR2_1_ECC_AGGR                                 (127U)
#define   SDL_DDR2_2_ECC_AGGR                                 (128U)
#define   SDL_DDR3_0_ECC_AGGR                                 (129U)
#define   SDL_DDR3_1_ECC_AGGR                                 (130U)
#define   SDL_DDR3_2_ECC_AGGR                                 (131U)
#define   SDL_COMPUTE_CLUSTER0_AW5_ECC0_AGGR                  (132U)
#define   SDL_COMPUTE_CLUSTER0_AW5_ECC1_AGGR                  (133U)
#define   SDL_COMPUTE_CLUSTER0_DSP1_ECC_AGGR                  (134U)
#define   SDL_COMPUTE_CLUSTER0_AW6_ECC0_AGGR                  (135U)
#define   SDL_COMPUTE_CLUSTER0_AW6_ECC1_AGGR                  (136U)
#define   SDL_COMPUTE_CLUSTER0_DSP2_ECC_AGGR                  (137U)
#define   SDL_COMPUTE_CLUSTER0_AW7_ECC0_AGGR                  (138U)
#define   SDL_COMPUTE_CLUSTER0_AW7_ECC1_AGGR                  (139U)
#define   SDL_COMPUTE_CLUSTER0_DSP3_ECC_AGGR                  (140U)
#define   SDL_COMPUTE_CLUSTER0_MSMC2_ECC_AGGR0_AGGR           (141U)
#define   SDL_COMPUTE_CLUSTER0_MSMC2_ECC_AGGR1_AGGR           (142U)
#define   SDL_COMPUTE_CLUSTER0_CFG0_ARM_COREPAC_ECC_AGGR      (143U)
#define   SDL_COMPUTE_CLUSTER0_CFG0_ARM_CORE0_ECC_AGGR        (144U)
#define   SDL_COMPUTE_CLUSTER0_CFG0_ARM_CORE1_ECC_AGGR        (145U)
#define   SDL_COMPUTE_CLUSTER0_CFG0_ARM_CORE2_ECC_AGGR        (146U)
#define   SDL_COMPUTE_CLUSTER0_CFG0_ARM_CORE3_ECC_AGGR        (147U)
#define   SDL_COMPUTE_CLUSTER0_CFG1_ARM_COREPAC_ECC_AGGR      (148U)
#define   SDL_COMPUTE_CLUSTER0_CFG1_ARM_CORE0_ECC_AGGR        (149U)
#define   SDL_COMPUTE_CLUSTER0_CFG1_ARM_CORE1_ECC_AGGR        (150U)
#define   SDL_COMPUTE_CLUSTER0_CFG1_ARM_CORE2_ECC_AGGR        (151U)
#define   SDL_COMPUTE_CLUSTER0_CFG1_ARM_CORE3_ECC_AGGR        (152U) 

#define SDL_ECC_MEMTYPE_MAX (SDL_COMPUTE_CLUSTER0_CFG1_ARM_CORE3_ECC_AGGR + 1U)

/* Max entries based on max mem type */
#define SDL_ECC_AGGREGATOR_MAX_LOW_ENTRIES    (SDL_CPSW_9XUSSM0_ECC_AGGR+1U)

#define SDL_ECC_AGGREGATOR_MAX_HIGH_ENTRIES (SDL_COMPUTE_CLUSTER0_CFG1_ARM_CORE3_ECC_AGGR - \
                                           SDL_ECC_MEMTYPE_MAIN_MSMC_AGGR0 + 1u)

#endif

/**
 * \brief This enumerator indicate ECC memory Sub Type
 *
 */
typedef uint32_t SDL_ECC_MemSubType;

#if defined (SOC_J721E)

/* The following are the memory sub type for Memory type
   SDL_ECC_MEMTYPE_MCU_R5F0_CORE & SDL_ECC_MEMTYPE_MCU_R5F1_CORE */
/* Keeping for backward-compatibility. Recommend to use RAM_ID directly from sdlr_soc_ecc_aggr.h file */

/** \brief Select memory subtype ATCM0 BANK0 */
#define SDL_ECC_R5F_MEM_SUBTYPE_ATCM0_BANK0_VECTOR_ID (SDL_MCU_R5FSS0_CORE0_ECC_AGGR_PULSAR_SL_ATCM0_BANK0_RAM_ID)
/** \brief Select memory subtype ATCM0 BANK1 */
#define SDL_ECC_R5F_MEM_SUBTYPE_ATCM0_BANK1_VECTOR_ID (SDL_MCU_R5FSS0_CORE0_ECC_AGGR_PULSAR_SL_ATCM0_BANK1_RAM_ID)
/** \brief Select memory subtype B0TCM0 BANK0 */
#define SDL_ECC_R5F_MEM_SUBTYPE_B0TCM0_BANK0_VECTOR_ID (SDL_MCU_R5FSS0_CORE0_ECC_AGGR_PULSAR_SL_B0TCM0_BANK0_RAM_ID)
/** \brief Select memory subtype B0TCM0 BANK1 */
#define SDL_ECC_R5F_MEM_SUBTYPE_B0TCM0_BANK1_VECTOR_ID (SDL_MCU_R5FSS0_CORE0_ECC_AGGR_PULSAR_SL_B0TCM0_BANK1_RAM_ID)
/** \brief Select memory subtype B1TCM0 BANK0 */
#define SDL_ECC_R5F_MEM_SUBTYPE_B1TCM0_BANK0_VECTOR_ID (SDL_MCU_R5FSS0_CORE0_ECC_AGGR_PULSAR_SL_B1TCM0_BANK0_RAM_ID)
/** \brief Select memory subtype B1TCM0 BANK1 */
#define SDL_ECC_R5F_MEM_SUBTYPE_B1TCM0_BANK1_VECTOR_ID (SDL_MCU_R5FSS0_CORE0_ECC_AGGR_PULSAR_SL_B1TCM0_BANK1_RAM_ID)
/** \brief Select memory subtype VIM RAM */
#define SDL_ECC_R5F_MEM_SUBTYPE_KS_VIM_RAM_VECTOR_ID (SDL_MCU_R5FSS0_CORE0_ECC_AGGR_CPU0_KS_VIM_RAMECC_RAM_ID)

#endif

#if defined (SOC_J7200)

/* The following are the memory sub type for Memory type
   SDL_ECC_MEMTYPE_MCU_R5F0_CORE & SDL_ECC_MEMTYPE_MCU_R5F1_CORE */
/* Keeping for backward-compatibility. Recommend to use RAM_ID directly from sdlr_soc_ecc_aggr.h file */

/** \brief Select memory subtype ATCM0 BANK0 */
#define SDL_ECC_R5F_MEM_SUBTYPE_ATCM0_BANK0_VECTOR_ID (SDL_MCU_R5FSS0_PULSAR_SL_ATCM0_BANK0_RAM_ID)
/** \brief Select memory subtype ATCM0 BANK1 */
#define SDL_ECC_R5F_MEM_SUBTYPE_ATCM0_BANK1_VECTOR_ID (SDL_MCU_R5FSS0_PULSAR_SL_ATCM0_BANK1_RAM_ID)
/** \brief Select memory subtype B0TCM0 BANK0 */
#define SDL_ECC_R5F_MEM_SUBTYPE_B0TCM0_BANK0_VECTOR_ID (SDL_MCU_R5FSS0_PULSAR_SL_B0TCM0_BANK0_RAM_ID)
/** \brief Select memory subtype B0TCM0 BANK1 */
#define SDL_ECC_R5F_MEM_SUBTYPE_B0TCM0_BANK1_VECTOR_ID (SDL_MCU_R5FSS0_PULSAR_SL_B0TCM0_BANK1_RAM_ID)
/** \brief Select memory subtype B1TCM0 BANK0 */
#define SDL_ECC_R5F_MEM_SUBTYPE_B1TCM0_BANK0_VECTOR_ID (SDL_MCU_R5FSS0_PULSAR_SL_B1TCM0_BANK0_RAM_ID)
/** \brief Select memory subtype B1TCM0 BANK1 */
#define SDL_ECC_R5F_MEM_SUBTYPE_B1TCM0_BANK1_VECTOR_ID (SDL_MCU_R5FSS0_PULSAR_SL_B1TCM0_BANK1_RAM_ID)
/** \brief Select memory subtype VIM RAM */
#define SDL_ECC_R5F_MEM_SUBTYPE_KS_VIM_RAM_VECTOR_ID (SDL_MCU_R5FSS0_CPU0_KS_VIM_RAMECC_RAM_ID)

#endif

#if defined (SOC_J721S2) || defined (SOC_J784S4)

/* The following are the memory sub type for Memory type
   SDL_ECC_MEMTYPE_MCU_R5F0_CORE & SDL_ECC_MEMTYPE_MCU_R5F1_CORE */
/* Keeping for backward-compatibility. Recommend to use RAM_ID directly from sdlr_soc_ecc_aggr.h file */

/** \brief Select memory subtype ATCM0 BANK0 */
#define SDL_ECC_R5F_MEM_SUBTYPE_ATCM0_BANK0_VECTOR_ID (SDL_MCU_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_PULSAR_SL_ATCM0_BANK0_RAM_ID)
/** \brief Select memory subtype ATCM0 BANK1 */
#define SDL_ECC_R5F_MEM_SUBTYPE_ATCM0_BANK1_VECTOR_ID (SDL_MCU_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_PULSAR_SL_ATCM0_BANK1_RAM_ID)
/** \brief Select memory subtype B0TCM0 BANK0 */
#define SDL_ECC_R5F_MEM_SUBTYPE_B0TCM0_BANK0_VECTOR_ID (SDL_MCU_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_PULSAR_SL_B0TCM0_BANK0_RAM_ID)
/** \brief Select memory subtype B0TCM0 BANK1 */
#define SDL_ECC_R5F_MEM_SUBTYPE_B0TCM0_BANK1_VECTOR_ID (SDL_MCU_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_PULSAR_SL_B0TCM0_BANK1_RAM_ID)
/** \brief Select memory subtype B1TCM0 BANK0 */
#define SDL_ECC_R5F_MEM_SUBTYPE_B1TCM0_BANK0_VECTOR_ID (SDL_MCU_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_PULSAR_SL_B1TCM0_BANK0_RAM_ID)
/** \brief Select memory subtype B1TCM0 BANK1 */
#define SDL_ECC_R5F_MEM_SUBTYPE_B1TCM0_BANK1_VECTOR_ID (SDL_MCU_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_PULSAR_SL_B1TCM0_BANK1_RAM_ID)
/** \brief Select memory subtype VIM RAM */
#define SDL_ECC_R5F_MEM_SUBTYPE_KS_VIM_RAM_VECTOR_ID (SDL_MCU_R5FSS0_PULSAR_SL_CPU0_ECC_AGGR_CPU0_KS_VIM_RAMECC_RAM_ID)

#endif

/** /brief Format of ECC error Call back function */
typedef void (*SDL_ECC_ErrorCallback_t) (uint32_t errorSrc, uint32_t address);

/** /brief Format of VIM DED vector function */
typedef void (*SDL_ECC_VIMDEDVector_t) (void);

/* @} */

/**
 *  \addtogroup SDL_ECC_DATASTRUCT
 *  @{
 */

/**
 * \brief This structure defines the elements of ECC  Init configuration
 *
 */
typedef struct SDL_ECC_InitConfig_s
{
    uint32_t numRams;
    /**< Max number of memory sections ECC is enabled on R5F,
         the memory sections include ATCM, VIM, BTCM RAMs */
    SDL_ECC_MemSubType *pMemSubTypeList;
    /**< Pointer to list of Vector ID types of ECC enabled memory sections */
} SDL_ECC_InitConfig_t;

/**
 * \brief This structure defines the inject error configuration
 *
 */
typedef struct SDL_ECC_InjectErrorConfig_s
{
    uint32_t *pErrMem;
    /**< Address to inject error */
    uint64_t errMem64;
    /**< Address to inject error for accessible mem where baseaddress is > 64-bits */
    uint32_t flipBitMask;
    /**< Bit location to flip bits */
    uint32_t chkGrp;
    /**< Group checker (for Interconnect RAM ID's only) */
}  SDL_ECC_InjectErrorConfig_t;

/**
 * \brief This structure defines the error status information
 *
 */
typedef struct SDL_ECC_ErrorInfo_s
{
    SDL_ECC_MemType eccMemType;
    /**< ECC Memory type */
    SDL_ECC_MemSubType memSubType;
    /**< Memory subtype */
    SDL_Ecc_AggrIntrSrc intrSrc;
    /**< Interrupt source */
    uint32_t bitErrCnt;
    /**< bit error count for the interrupt source. 0-2 Number of errors, 3 means 3 or more errors. */
    uint32_t injectBitErrCnt;
    /**< inject bit error count for the interrupt source (valid for EDC only). 0-2 Number of errors, 3 means 3 or more errors. */
    uint32_t bitErrorGroup;
    /**< bit error group. Indicates the Checker Group where the error occurred. */
    uint64_t bitErrorOffset;
    /**< bit error offset */
} SDL_ECC_ErrorInfo_t;

/* @} */

/**
 *  \addtogroup SDL_ECC_FUNCTION
 *  @{
 */

/** ============================================================================*
 *
 * \brief   Initializes an  module for usage with ECC module
 *
 * \param   esmInstType: Instance of
 *
 * \return  SDL_PASS : Success; SDL_FAIL for failures
 */
int32_t SDL_ECC_initEsm (const SDL_ESM_Inst esmInstType);

/** ============================================================================*
 *
 * \brief   Initializes ECC module for ECC detection
 *
 * \param  eccMemType ECC memory type
 * \param  pECCInitConfig     Pointer to Ecc init configuration
 *
 * \return  SDL_PASS : Success; SDL_FAIL for failuresn
 */
int32_t SDL_ECC_init (SDL_ECC_MemType eccMemType,
                        const SDL_ECC_InitConfig_t *pECCInitConfig);

/** ============================================================================
 *
 * \brief   Initializes Memory to be ready for ECC error detection.
 *          Assumes ECC is already enabled.
 *
 * \param  eccMemType ECC memory type
 * \param  memSubType: Memory subtype
 *
 * \return  SDL_PASS : Success; SDL_FAIL for failures
 */
int32_t SDL_ECC_initMemory (SDL_ECC_MemType eccMemType,
                               SDL_ECC_MemSubType memSubType);

/** ============================================================================
 *
 * \brief   Runs self test by injecting and error and monitor response
 *          Assumes ECC is already enabled.
 *
 * \param  eccMemType ECC memory type
 * \param  memSubType: Memory subtype
 * \param  errorType: ECC Self test type
 * \param  pECCErrorConfig: Pointer to Error configuration
 * \param  selfTestTimeOut: Number of retries before time out
 *
 * \return  SDL_PASS : Success; SDL_FAIL for failures
 */
int32_t SDL_ECC_selfTest(SDL_ECC_MemType eccMemType,
                            SDL_ECC_MemSubType memSubType,
                            SDL_ECC_InjectErrorType errorType,
                            const SDL_ECC_InjectErrorConfig_t *pECCErrorConfig,
                            uint32_t selfTestTimeOut);

/** ============================================================================
 *
 * \brief   Injects ECC error at specified location
 *          Assumes ECC is already enabled.
 *
 * \param  eccMemType ECC memory type
 * \param  memSubType: Memory subtype
 * \param  errorType: ECC error type
 * \param  pECCErrorConfig: Pointer to Error configuration
 *
 * \return  SDL_PASS : Success; SDL_FAIL for failures
 */
int32_t SDL_ECC_injectError(SDL_ECC_MemType eccMemType,
                               SDL_ECC_MemSubType memSubType,
                               SDL_ECC_InjectErrorType errorType,
                               const SDL_ECC_InjectErrorConfig_t *pECCErrorConfig);

/** ============================================================================
 *
 * \brief   Gets the static registers for the specified ECC instance.
 *
 * \param  eccMemType ECC memory type
 * \param  pStaticRegs: Pointer to Static registers structure
 *
 * \return  SDL_PASS : Success; SDL_FAIL for failures
 */
int32_t SDL_ECC_getStaticRegisters(SDL_ECC_MemType eccMemType,
                                   SDL_ECC_staticRegs *pStaticRegs);

/** ============================================================================
 *
 * \brief   Retrieves the ECC error information for the specified memtype and
 *          interrupt source.
 *
 * \param   eccMemType ECC memory type
 * \param   intrSrc: interrupt source
 * \param   pErrorInfo: Pointer to the Error Information
 *
 * \return  SDL_PASS : Success; SDL_FAIL for failures
 */
int32_t SDL_ECC_getErrorInfo(SDL_ECC_MemType eccMemType,
                                SDL_Ecc_AggrIntrSrc intrSrc,
                                SDL_ECC_ErrorInfo_t *pErrorInfo);

/** ============================================================================
 *
 * \brief   Acknowledge the ECC interrupt
 *
 * \param   eccMemType ECC memory type
 * \param   intrSrc: interrupt source
 *
 * \return  SDL_PASS : Success; SDL_FAIL for failures
 */
int32_t SDL_ECC_ackIntr(SDL_ECC_MemType eccMemType,
                        SDL_Ecc_AggrIntrSrc intrSrc);

/** ============================================================================
 *
 * \brief   Retrieves the ECC error information for the specified ESM error. If
 *          it isn't an ECC error or the ECC error is not supported an error is
 *          returned.
 *
 * \param   instance ESM instance
 * \param   intSrc: ESM interrupt number
 * \param   eccMemType: Pointer to the ECC memory type
 * \param   intrSrcType: Pointer to the interrupt source type
 *
 * \return  SDL_PASS : Success; SDL_FAIL for failures
 */
int32_t SDL_ECC_getESMErrorInfo(SDL_ESM_Inst instance, uint32_t intSrc,
                                SDL_ECC_MemType *eccMemType, SDL_Ecc_AggrIntrSrc *intrSrcType);

/** ============================================================================
 *
 * \brief   Clears N pending interrupts for the specified memtype, subtype and
 *          interrupt source.
 *
 * \param   eccMemType ECC memory type
 * \param   memSubType: Memory subtype
 * \param   intrSrc: interrupt source
 * \param   subType: error subtype (valid for EDC types only)
 * \param   numEvents: number of pending interrupts to clear
 *
 * \return  SDL_PASS : Success; SDL_FAIL for failures
 */
int32_t SDL_ECC_clearNIntrPending(SDL_ECC_MemType eccMemType, SDL_ECC_MemSubType memSubType,
                                  SDL_Ecc_AggrIntrSrc intrSrc,
                                  SDL_Ecc_AggrEDCErrorSubType subType, uint32_t numEvents);

/** ============================================================================
 *
 * \brief   Register Handler for VIM DED ECC error
 *
 * \param  VIMDEDHandler: This is function to be called when an Double bit ECC
 *                        error happens on the VIM RAM. NOTE: That the regular
 *                        vector load will be replaced with this registered
 *                        function.
 *
 * \return  None
 */
void SDL_ECC_registerVIMDEDHandler(SDL_ECC_VIMDEDVector_t VIMDEDHandler);

/** ============================================================================
 *
 * \brief   Application provided external callback function for ECC handling
 *          Called inside the reference functions when ECC errors occur.
 *          NOTE: This is application supplied and not part of the SDL
 *          If not supplied by application this will result in an linker error
 *
 * \param  eccMemType: ECC Memory Type
 * \param  errorSrc: Error source for the ECC error event.
 * \param  address: Address at which the ECC error occurred.
 * \param  ramId: RAM ID at which the ECC error occurred.
 * \param  bitErrorOffset: Offset at which the ECC error occurred.
 * \param  bitErrorGroup: group checker that reported the error
 *         (Interconnect ECC type only).
 *
 *
 * \return  None
 */
void SDL_ECC_applicationCallbackFunction(SDL_ECC_MemType eccMemType,
                                         uint32_t errorSrc,
                                         uint32_t address,
                                         uint32_t ramId,
                                         uint64_t bitErrorOffset,
                                         uint32_t bitErrorGroup);

/* @} */

#ifdef __cplusplus
}
#endif  /* extern "C" */

#endif /* INCLUDE_SDL_ECC_H_ */
