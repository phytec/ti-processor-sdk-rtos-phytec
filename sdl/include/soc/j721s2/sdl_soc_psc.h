/********************************************************************
*
* SOC PSC PD and LPSC ID definitions. header file
*
* Copyright (C) 2015-2019 Texas Instruments Incorporated.
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
#ifndef SDL_SOC_PSC_H_
#define SDL_SOC_PSC_H_

#include <sdlr.h>
#include <tistdtypes.h>
#ifdef __cplusplus
extern "C"
{
#endif

/*
* Auto-generated PSC definitions File
*/

/*
* PD Indices:
*/

/*
*  WKUP PD
*/

#define SDL_WKUP_PD_WKUP                                                                           (0U)
#define SDL_WKUP_PD_MCU_PULSAR                                                                     (1U)
/*
*  MAIN PD
*/

#define SDL_MAIN_GP_CORE_CTL                                                                       (0U)
#define SDL_MAIN_PD_MCANSS                                                                         (1U)
#define SDL_MAIN_PD_DSS                                                                            (2U)
#define SDL_MAIN_PD_ICSS                                                                           (3U)
#define SDL_MAIN_PD_9GSS                                                                           (4U)
#define SDL_MAIN_PD_SERDES_0                                                                       (5U)
#define SDL_MAIN_PD_SERDES_1                                                                       (6U)
#define SDL_MAIN_PD_SERDES_2                                                                       (7U)
#define SDL_MAIN_PD_SERDES_3                                                                       (8U)
#define SDL_MAIN_PD_SERDES_4                                                                       (9U)
#define SDL_MAIN_PD_SERDES_5                                                                       (10U)
#define SDL_MAIN_PD_TIMER                                                                          (11U)
#define SDL_MAIN_PD_C71X_0                                                                         (12U)
#define SDL_MAIN_PD_C71X_1                                                                         (13U)
#define SDL_MAIN_PD_A72_CLUSTER_0                                                                  (14U)
#define SDL_MAIN_PD_A72_0                                                                          (15U)
#define SDL_MAIN_PD_A72_1                                                                          (16U)
#define SDL_MAIN_PD_A72_CLUSTER_1                                                                  (17U)
#define SDL_MAIN_PD_A72_2                                                                          (18U)
#define SDL_MAIN_PD_A72_3                                                                          (19U)
#define SDL_MAIN_PD_GPUCOM                                                                         (20U)
#define SDL_MAIN_PD_GPUCORE                                                                        (21U)
#define SDL_MAIN_PD_C66X_0                                                                         (22U)
#define SDL_MAIN_PD_C66X_1                                                                         (23U)
#define SDL_MAIN_PD_PULSAR_0                                                                       (24U)
#define SDL_MAIN_PD_PULSAR_1                                                                       (25U)
#define SDL_MAIN_PD_DECODE_0                                                                       (26U)
#define SDL_MAIN_PD_ENCODE_0                                                                       (27U)
#define SDL_MAIN_PD_DMPAC                                                                          (28U)
#define SDL_MAIN_PD_VPAC                                                                           (29U)
#define SDL_MAIN_PD_A72_CL0_2                                                                      (30U)
#define SDL_MAIN_PD_A72_CL0_3                                                                      (31U)
#define SDL_MAIN_PD_A72_CL1_2                                                                      (32U)
#define SDL_MAIN_PD_A72_CL1_3                                                                      (33U)
#define SDL_MAIN_PD_VPAC_1                                                                         (34U)
#define SDL_MAIN_PD_ENCODE_1                                                                       (35U)
#define SDL_MAIN_PD_DSI_1                                                                          (36U)
#define SDL_MAIN_PD_CPSW2                                                                          (37U)
#define SDL_MAIN_PD_DDR2                                                                           (38U)
#define SDL_MAIN_PD_PULSAR_2                                                                       (39U)
#define SDL_MAIN_PD_SPARE4                                                                         (40U)
#define SDL_MAIN_PD_SPARE5                                                                         (41U)
#define SDL_MAIN_PD_SPARE6                                                                         (42U)
/*
* LPSC Modules:
*/

/*
*  WKUP LPSC
*/

#define SDL_WKUP_LPSC_WKUP_ALWAYSON                                                                (0U)
#define SDL_WKUP_LPSC_DMSC                                                                         (1U)
#define SDL_WKUP_LPSC_DEBUG2DMSC                                                                   (2U)
#define SDL_WKUP_LPSC_WKUP_GPIO                                                                    (3U)
#define SDL_WKUP_LPSC_WKUPMCU2MAIN                                                                 (4U)
#define SDL_WKUP_LPSC_MAIN2WKUPMCU                                                                 (5U)
#define SDL_WKUP_LPSC_MCU_TEST                                                                     (6U)
#define SDL_WKUP_LPSC_MCU_DEBUG                                                                    (7U)
#define SDL_WKUP_LPSC_MCU_MCAN_0                                                                   (8U)
#define SDL_WKUP_LPSC_MCU_MCAN_1                                                                   (9U)
#define SDL_WKUP_LPSC_MCU_OSPI_0                                                                   (10U)
#define SDL_WKUP_LPSC_MCU_OSPI_1                                                                   (11U)
#define SDL_WKUP_LPSC_MCU_HYPERBUS                                                                 (12U)
#define SDL_WKUP_LPSC_MCU_I3C_0                                                                    (13U)
#define SDL_WKUP_LPSC_MCU_I3C_1                                                                    (14U)
#define SDL_WKUP_LPSC_MCU_ADC_0                                                                    (15U)
#define SDL_WKUP_LPSC_MCU_ADC_1                                                                    (16U)
#define SDL_WKUP_LPSC_WKUP_SPARE0                                                                  (17U)
#define SDL_WKUP_LPSC_WKUP_SPARE1                                                                  (18U)
#define SDL_WKUP_LPSC_MCU_R5_0                                                                     (19U)
#define SDL_WKUP_LPSC_MCU_R5_1                                                                     (20U)
#define SDL_WKUP_LPSC_MCU_PULSAR_PBIST_0                                                           (21U)
/*
*  MAIN LPSC
*/

#define SDL_MAIN_LPSC_MAIN_ALWAYSON                                                                (0U)
#define SDL_MAIN_LPSC_MAIN_TEST                                                                    (1U)
#define SDL_MAIN_LPSC_MAIN_PBIST                                                                   (2U)
#define SDL_MAIN_LPSC_PER_AUDIO                                                                    (3U)
#define SDL_MAIN_LPSC_PER_ATL                                                                      (4U)
#define SDL_MAIN_LPSC_PER_MLB                                                                      (5U)
#define SDL_MAIN_LPSC_PER_MOTOR                                                                    (6U)
#define SDL_MAIN_LPSC_PER_MISCIO                                                                   (7U)
#define SDL_MAIN_LPSC_PER_GPMC                                                                     (8U)
#define SDL_MAIN_LPSC_PER_VPFE                                                                     (9U)
#define SDL_MAIN_LPSC_PER_VPE                                                                      (10U)
#define SDL_MAIN_LPSC_PER_SPARE0                                                                   (11U)
#define SDL_MAIN_LPSC_PER_SPARE1                                                                   (12U)
#define SDL_MAIN_LPSC_MAIN_DEBUG                                                                   (13U)
#define SDL_MAIN_LPSC_EMIF_DATA_0                                                                  (14U)
#define SDL_MAIN_LPSC_EMIF_CFG_0                                                                   (15U)
#define SDL_MAIN_LPSC_EMIF_DATA_1                                                                  (16U)
#define SDL_MAIN_LPSC_EMIF_CFG_1                                                                   (17U)
#define SDL_MAIN_LPSC_PER_SPARE2                                                                   (18U)
#define SDL_MAIN_LPSC_CC_TOP_PBIST                                                                 (19U)
#define SDL_MAIN_LPSC_USB_0                                                                        (20U)
#define SDL_MAIN_LPSC_USB_1                                                                        (21U)
#define SDL_MAIN_LPSC_USB_2                                                                        (22U)
#define SDL_MAIN_LPSC_MMC4B_0                                                                      (23U)
#define SDL_MAIN_LPSC_MMC4B_1                                                                      (24U)
#define SDL_MAIN_LPSC_MMC8B_0                                                                      (25U)
#define SDL_MAIN_LPSC_UFS_0                                                                        (26U)
#define SDL_MAIN_LPSC_UFS_1                                                                        (27U)
#define SDL_MAIN_LPSC_PCIE_0                                                                       (28U)
#define SDL_MAIN_LPSC_PCIE_1                                                                       (29U)
#define SDL_MAIN_LPSC_PCIE_2                                                                       (30U)
#define SDL_MAIN_LPSC_PCIE_3                                                                       (31U)
#define SDL_MAIN_LPSC_SAUL                                                                         (32U)
#define SDL_MAIN_LPSC_PER_I3C                                                                      (33U)
#define SDL_MAIN_LPSC_MAIN_MCANSS_0                                                                (34U)
#define SDL_MAIN_LPSC_MAIN_MCANSS_1                                                                (35U)
#define SDL_MAIN_LPSC_MAIN_MCANSS_2                                                                (36U)
#define SDL_MAIN_LPSC_MAIN_MCANSS_3                                                                (37U)
#define SDL_MAIN_LPSC_MAIN_MCANSS_4                                                                (38U)
#define SDL_MAIN_LPSC_MAIN_MCANSS_5                                                                (39U)
#define SDL_MAIN_LPSC_MAIN_MCANSS_6                                                                (40U)
#define SDL_MAIN_LPSC_MAIN_MCANSS_7                                                                (41U)
#define SDL_MAIN_LPSC_MAIN_MCANSS_8                                                                (42U)
#define SDL_MAIN_LPSC_MAIN_MCANSS_9                                                                (43U)
#define SDL_MAIN_LPSC_MAIN_MCANSS_10                                                               (44U)
#define SDL_MAIN_LPSC_MAIN_MCANSS_11                                                               (45U)
#define SDL_MAIN_LPSC_MAIN_MCANSS_12                                                               (46U)
#define SDL_MAIN_LPSC_MAIN_MCANSS_13                                                               (47U)
#define SDL_MAIN_LPSC_DSS                                                                          (48U)
#define SDL_MAIN_LPSC_DSS_PBIST                                                                    (49U)
#define SDL_MAIN_LPSC_DSI                                                                          (50U)
#define SDL_MAIN_LPSC_EDP_0                                                                        (51U)
#define SDL_MAIN_LPSC_EDP_1                                                                        (52U)
#define SDL_MAIN_LPSC_CSIRX_0                                                                      (53U)
#define SDL_MAIN_LPSC_CSIRX_1                                                                      (54U)
#define SDL_MAIN_LPSC_CSIRX_2                                                                      (55U)
#define SDL_MAIN_LPSC_CSITX_0                                                                      (56U)
#define SDL_MAIN_LPSC_TX_DPHY_0                                                                    (57U)
#define SDL_MAIN_LPSC_CSIRX_PHY_0                                                                  (58U)
#define SDL_MAIN_LPSC_CSIRX_PHY_1                                                                  (59U)
#define SDL_MAIN_LPSC_CSIRX_PHY_2                                                                  (60U)
#define SDL_MAIN_LPSC_ICSSG_0                                                                      (61U)
#define SDL_MAIN_LPSC_ICSSG_1                                                                      (62U)
#define SDL_MAIN_LPSC_9GSS                                                                         (63U)
#define SDL_MAIN_LPSC_SERDES_0                                                                     (64U)
#define SDL_MAIN_LPSC_SERDES_1                                                                     (65U)
#define SDL_MAIN_LPSC_SERDES_2                                                                     (66U)
#define SDL_MAIN_LPSC_SERDES_3                                                                     (67U)
#define SDL_MAIN_LPSC_SERDES_4                                                                     (68U)
#define SDL_MAIN_LPSC_SERDES_5                                                                     (69U)
#define SDL_MAIN_LPSC_DMTIMER_0                                                                    (70U)
#define SDL_MAIN_LPSC_DMTIMER_1                                                                    (71U)
#define SDL_MAIN_LPSC_DMTIMER_2                                                                    (72U)
#define SDL_MAIN_LPSC_DMTIMER_3                                                                    (73U)
#define SDL_MAIN_LPSC_C71X_0                                                                       (74U)
#define SDL_MAIN_LPSC_C71X_0_PBIST                                                                 (75U)
#define SDL_MAIN_LPSC_C71X_1                                                                       (76U)
#define SDL_MAIN_LPSC_C71X_1_PBIST                                                                 (77U)
#define SDL_MAIN_LPSC_A72_CLUSTER_0                                                                (78U)
#define SDL_MAIN_LPSC_A72_CLUSTER_0_PBIST                                                          (79U)
#define SDL_MAIN_LPSC_A72_0                                                                        (80U)
#define SDL_MAIN_LPSC_A72_1                                                                        (81U)
#define SDL_MAIN_LPSC_A72_CLUSTER_1                                                                (82U)
#define SDL_MAIN_LPSC_A72_CLUSTER_1_PBIST                                                          (83U)
#define SDL_MAIN_LPSC_A72_2                                                                        (84U)
#define SDL_MAIN_LPSC_A72_3                                                                        (85U)
#define SDL_MAIN_LPSC_GPUCOM                                                                       (86U)
#define SDL_MAIN_LPSC_GPUPBIST                                                                     (87U)
#define SDL_MAIN_LPSC_GPUCORE                                                                      (88U)
#define SDL_MAIN_LPSC_C66X_0                                                                       (89U)
#define SDL_MAIN_LPSC_C66X_PBIST_0                                                                 (90U)
#define SDL_MAIN_LPSC_C66X_1                                                                       (91U)
#define SDL_MAIN_LPSC_C66X_PBIST_1                                                                 (92U)
#define SDL_MAIN_LPSC_PULSAR_0_R5_0                                                                (93U)
#define SDL_MAIN_LPSC_PULSAR_0_R5_1                                                                (94U)
#define SDL_MAIN_LPSC_PULSAR_PBIST_0                                                               (95U)
#define SDL_MAIN_LPSC_PULSAR_1_R5_0                                                                (96U)
#define SDL_MAIN_LPSC_PULSAR_1_R5_1                                                                (97U)
#define SDL_MAIN_LPSC_PULSAR_PBIST_1                                                               (98U)
#define SDL_MAIN_LPSC_DECODE_0                                                                     (99U)
#define SDL_MAIN_LPSC_DECODE_PBIST                                                                 (100U)
#define SDL_MAIN_LPSC_ENCODE_0                                                                     (101U)
#define SDL_MAIN_LPSC_ENCODE_PBIST                                                                 (102U)
#define SDL_MAIN_LPSC_DMPAC                                                                        (103U)
#define SDL_MAIN_LPSC_SDE                                                                          (104U)
#define SDL_MAIN_LPSC_DMPAC_PBIST                                                                  (105U)
#define SDL_MAIN_LPSC_VPAC                                                                         (106U)
#define SDL_MAIN_LPSC_VPAC_PBIST                                                                   (107U)
#define SDL_MAIN_LPSC_A72_CLSTR0_CORE2                                                             (108U)
#define SDL_MAIN_LPSC_A72_CLSTR0_CORE3                                                             (109U)
#define SDL_MAIN_LPSC_A72_CLSTR1_CORE2                                                             (110U)
#define SDL_MAIN_LPSC_A72_CLSTR1_CORE3                                                             (111U)
#define SDL_MAIN_LPSC_VPAC_1                                                                       (112U)
#define SDL_MAIN_LPSC_VPAC_1_PBIST                                                                 (113U)
#define SDL_MAIN_LPSC_ENCODE_1                                                                     (114U)
#define SDL_MAIN_LPSC_ENCODE_1_PBIST                                                               (115U)
#define SDL_MAIN_LPSC_CSITX_1                                                                      (116U)
#define SDL_MAIN_LPSC_TX_DPHY_1                                                                    (117U)
#define SDL_MAIN_LPSC_DSI_1_PBIST                                                                  (118U)
#define SDL_MAIN_LPSC_CPSW_2                                                                       (119U)
#define SDL_MAIN_LPSC_EMIF_DATA_2                                                                  (120U)
#define SDL_MAIN_LPSC_EMIF_CFG_2                                                                   (121U)
#define SDL_MAIN_LPSC_PULSAR_2_R5_0                                                                (122U)
#define SDL_MAIN_LPSC_PULSAR_2_R5_1                                                                (123U)
#define SDL_MAIN_LPSC_PULSAR_2_PBIST                                                               (124U)
#define SDL_MAIN_LPSC_SPARE_4                                                                      (125U)
#define SDL_MAIN_LPSC_SPARE_5                                                                      (126U)
#define SDL_MAIN_LPSC_SPARE_6                                                                      (127U)

#ifdef __cplusplus
}
#endif
#endif /* SDL_SOC_PSC_H_ */

