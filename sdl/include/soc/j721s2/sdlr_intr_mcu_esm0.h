/********************************************************************
*
* MCU_ESM0 INTERRUPT MAP. header file
*
* Copyright (C) 2015-2020 Texas Instruments Incorporated.
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
#ifndef SDLR_MCU_ESM0_INTERRUPT_MAP_H_
#define SDLR_MCU_ESM0_INTERRUPT_MAP_H_

#include <sdlr.h>
#include <tistdtypes.h>
#ifdef __cplusplus
extern "C"
{
#endif

/*
* List of intr sources for receiver: MCU_ESM0
*/

#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_PLLFRACF2_SSMOD0_LOCKLOSS_IPCFG_0                          (0U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_PLLFRACF2_SSMOD1_LOCKLOSS_IPCFG_0                          (1U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_PLLFRACF2_SSMOD2_LOCKLOSS_IPCFG_0                          (2U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_ADC12FCC0_ECC_CORRECTED_ERR_LEVEL_0                        (10U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_ADC12FCC0_ECC_UNCORRECTED_ERR_LEVEL_0                      (11U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_ADC12FCC1_ECC_CORRECTED_ERR_LEVEL_0                        (12U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_ADC12FCC1_ECC_UNCORRECTED_ERR_LEVEL_0                      (13U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_CPSW0_ECC_SEC_PEND_0                                       (14U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_CPSW0_ECC_DED_PEND_0                                       (15U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_MCAN0_MCANSS_ECC_CORR_LVL_INT_0                            (16U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_MCAN0_MCANSS_ECC_UNCORR_LVL_INT_0                          (17U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_MCAN1_MCANSS_ECC_CORR_LVL_INT_0                            (18U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_MCAN1_MCANSS_ECC_UNCORR_LVL_INT_0                          (19U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_I3C0_PCLK_ECC_UNCORR_LVL_0                                 (20U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_I3C0_SCLK_ECC_UNCORR_LVL_0                                 (21U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_I3C1_PCLK_ECC_UNCORR_LVL_0                                 (22U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_I3C1_SCLK_ECC_UNCORR_LVL_0                                 (23U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_FSS0_OSPI_ECC_AGGR_0_OSPI_ECC_CORR_LVL_INTR_0              (24U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_FSS0_OSPI_ECC_AGGR_0_OSPI_ECC_UNCORR_LVL_INTR_0            (25U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_FSS0_OSPI_ECC_AGGR_1_OSPI_ECC_CORR_LVL_INTR_0              (26U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_FSS0_OSPI_ECC_AGGR_1_OSPI_ECC_UNCORR_LVL_INTR_0            (27U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_FSS0_HYPERBUS_ECC_AGGR_0_HPB_ECC_CORR_LEVEL_0              (28U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_FSS0_HYPERBUS_ECC_AGGR_0_HPB_ECC_UNCORR_LEVEL_0            (29U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_FSS0_MISC_0_ECC_INTR_ERR_PEND_0                            (30U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_FSS0_FSAS_0_ECC_INTR_ERR_PEND_0                            (31U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_R5FSS0_CORE0_ECC_CORRECTED_LEVEL_0                         (32U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_R5FSS0_CORE0_ECC_UNCORRECTED_LEVEL_0                       (33U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_R5FSS0_CORE1_ECC_CORRECTED_LEVEL_0                         (34U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_R5FSS0_CORE1_ECC_UNCORRECTED_LEVEL_0                       (35U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_R5FSS0_CORE0_EXP_INTR_0                                    (36U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_R5FSS0_CORE1_EXP_INTR_0                                    (37U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_MASTER_SAFETY_GASKET0_TIMED_OUT_0                          (39U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_NAVSS0_MODSS_ECC_AGGR0_ECC_CORRECTED_ERR_LEVEL_0           (40U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_NAVSS0_MODSS_ECC_AGGR0_ECC_UNCORRECTED_ERR_LEVEL_0         (41U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_NAVSS0_UDMASS_ECC_AGGR0_ECC_CORRECTED_ERR_LEVEL_0          (42U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_NAVSS0_UDMASS_ECC_AGGR0_ECC_UNCORRECTED_ERR_LEVEL_0        (43U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MASTER_SAFETY_GASKET16_TIMED_OUT_0                             (44U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MASTER_SAFETY_GASKET17_TIMED_OUT_0                             (45U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MASTER_SAFETY_GASKET18_TIMED_OUT_0                             (46U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MASTER_SAFETY_GASKET19_TIMED_OUT_0                             (47U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_TIMEOUT_64B3_TRANS_ERR_LVL_0                               (48U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_TIMEOUT_64B4_TRANS_ERR_LVL_0                               (49U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_R5FSS0_COMMON0_ECC_SE_TO_ESM_0_0                           (50U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_R5FSS0_COMMON0_ECC_DE_TO_ESM_0_0                           (51U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_R5FSS0_COMMON0_ECC_SE_TO_ESM_1_0                           (52U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_R5FSS0_COMMON0_ECC_DE_TO_ESM_1_0                           (53U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_TIMER0_INTR_PEND_0                                         (54U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_TIMER1_INTR_PEND_0                                         (55U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_TIMER2_INTR_PEND_0                                         (56U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_TIMER3_INTR_PEND_0                                         (57U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_TIMER4_INTR_PEND_0                                         (58U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_TIMER5_INTR_PEND_0                                         (59U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_TIMER6_INTR_PEND_0                                         (60U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_TIMER7_INTR_PEND_0                                         (61U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_TIMER8_INTR_PEND_0                                         (62U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_TIMER9_INTR_PEND_0                                         (63U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_MSRAM_1MB0_ECC_CORR_LEVEL_0                                (64U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_MSRAM_1MB0_ECC_UNCORR_LEVEL_0                              (65U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_SA3_SS0_SA_UL_0_SA_UL_ECC_CORR_LEVEL_0                     (66U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_SA3_SS0_SA_UL_0_SA_UL_ECC_UNCORR_LEVEL_0                   (67U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_SA3_SS0_DMSS_ECCAGGR_0_DMSS_ECC_DED_PEND_0                 (68U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_SA3_SS0_DMSS_ECCAGGR_0_DMSS_ECC_SEC_PEND_0                 (69U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCUM_MCU_ECC_AGGR0_CORR_LEVEL_0                                (70U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCUM_MCU_ECC_AGGR0_UNCORR_LEVEL_0                              (71U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_PBIST1_DFT_PBIST_SAFETY_ERROR_0                            (73U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_PBIST0_DFT_PBIST_SAFETY_ERROR_0                            (74U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_PBIST2_DFT_PBIST_SAFETY_ERROR_0                            (75U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_TIMEOUT_64B2_TRANS_ERR_LVL_0                               (76U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_VDC_INFRA_VBUSP_32B_SRC_SAFEG0_INFRA_SAFEG_TRANS_ERR_LVL_0 (77U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_VDC_SOC_FW_VBUSP_32B_SRC_SAFEG1_SOC_FW_SAFEG_TRANS_ERR_LVL_0 (78U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_I3C0_I3C_NONFATAL__INT_0                                   (80U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_I3C0_I3C_FATAL__INT_0                                      (81U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_I3C1_I3C_NONFATAL__INT_0                                   (82U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_I3C1_I3C_FATAL__INT_0                                      (83U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MASTER_SAFETY_GASKET20_TIMED_OUT_0                             (84U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MASTER_SAFETY_GASKET21_TIMED_OUT_0                             (85U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_DCC0_INTR_ERR_LEVEL_0                                      (86U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_DCC1_INTR_ERR_LEVEL_0                                      (87U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MCU_DCC2_INTR_ERR_LEVEL_0                                      (88U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MASTER_SAFETY_GASKET22_TIMED_OUT_0                             (89U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_MASTER_SAFETY_GASKET23_TIMED_OUT_0                             (90U)
#define SDLR_MCU_ESM0_ESM_LVL_EVENT_GLUELOGIC_ESM_MAIN_ERR_GLUE_ERR_I_N_0                          (95U)
#define SDLR_MCU_ESM0_ESM_PLS_EVENT0_MCU_R5FSS0_COMMON0_SELFTEST_ERR_PULSE_0                       (96U)
#define SDLR_MCU_ESM0_ESM_PLS_EVENT1_MCU_R5FSS0_COMMON0_SELFTEST_ERR_PULSE_0                       (96U)
#define SDLR_MCU_ESM0_ESM_PLS_EVENT2_MCU_R5FSS0_COMMON0_SELFTEST_ERR_PULSE_0                       (96U)
#define SDLR_MCU_ESM0_ESM_PLS_EVENT0_MCU_R5FSS0_COMMON0_COMPARE_ERR_PULSE_0                        (97U)
#define SDLR_MCU_ESM0_ESM_PLS_EVENT1_MCU_R5FSS0_COMMON0_COMPARE_ERR_PULSE_0                        (97U)
#define SDLR_MCU_ESM0_ESM_PLS_EVENT2_MCU_R5FSS0_COMMON0_COMPARE_ERR_PULSE_0                        (97U)
#define SDLR_MCU_ESM0_ESM_PLS_EVENT0_MCU_R5FSS0_COMMON0_BUS_MONITOR_ERR_PULSE_0                    (98U)
#define SDLR_MCU_ESM0_ESM_PLS_EVENT1_MCU_R5FSS0_COMMON0_BUS_MONITOR_ERR_PULSE_0                    (98U)
#define SDLR_MCU_ESM0_ESM_PLS_EVENT2_MCU_R5FSS0_COMMON0_BUS_MONITOR_ERR_PULSE_0                    (98U)
#define SDLR_MCU_ESM0_ESM_PLS_EVENT0_MCU_R5FSS0_COMMON0_VIM_COMPARE_ERR_PULSE_0                    (99U)
#define SDLR_MCU_ESM0_ESM_PLS_EVENT1_MCU_R5FSS0_COMMON0_VIM_COMPARE_ERR_PULSE_0                    (99U)
#define SDLR_MCU_ESM0_ESM_PLS_EVENT2_MCU_R5FSS0_COMMON0_VIM_COMPARE_ERR_PULSE_0                    (99U)
#define SDLR_MCU_ESM0_ESM_PLS_EVENT0_MCU_R5FSS0_CCM_COMPARE_STAT_PULSE_INTR_0                      (100U)
#define SDLR_MCU_ESM0_ESM_PLS_EVENT1_MCU_R5FSS0_CCM_COMPARE_STAT_PULSE_INTR_0                      (100U)
#define SDLR_MCU_ESM0_ESM_PLS_EVENT2_MCU_R5FSS0_CCM_COMPARE_STAT_PULSE_INTR_0                      (100U)
#define SDLR_MCU_ESM0_ESM_PLS_EVENT0_MCU_RTI0_INTR_WWD_0                                           (104U)
#define SDLR_MCU_ESM0_ESM_PLS_EVENT1_MCU_RTI0_INTR_WWD_0                                           (104U)
#define SDLR_MCU_ESM0_ESM_PLS_EVENT2_MCU_RTI0_INTR_WWD_0                                           (104U)
#define SDLR_MCU_ESM0_ESM_PLS_EVENT0_MCU_RTI1_INTR_WWD_0                                           (105U)
#define SDLR_MCU_ESM0_ESM_PLS_EVENT1_MCU_RTI1_INTR_WWD_0                                           (105U)
#define SDLR_MCU_ESM0_ESM_PLS_EVENT2_MCU_RTI1_INTR_WWD_0                                           (105U)

#ifdef __cplusplus
}
#endif
#endif /* SDLR_MCU_ESM0_INTERRUPT_MAP_H_ */

