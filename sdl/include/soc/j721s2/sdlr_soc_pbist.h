/********************************************************************
*
* SOC PBIST PROPERTIES. header file
*
* Copyright (C) 2015-2023 Texas Instruments Incorporated.
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
#ifndef SDLR_SOC_PBIST_H_
#define SDLR_SOC_PBIST_H_

#include <sdlr.h>
#include <tistdtypes.h>
#ifdef __cplusplus
extern "C"
{
#endif

/*
* Auto-generated SDL definitions for SoC PBIST Instances:
*/


/* Properties of PBIST instances in: PBIST3 */
#define SDL_PBIST3_NUM_TEST_VECTORS                                                                (2U)
#define SDL_PBIST3_ALGO_BITMAP_0                                                                   (0x0000000000000005U)
#define SDL_PBIST3_MEM_BITMAP_0                                                                    (0x0000000000000155U)
#define SDL_PBIST3_ALGO_BITMAP_1                                                                   (0x000000000000000AU)
#define SDL_PBIST3_MEM_BITMAP_1                                                                    (0x0000000003553400U)
#define SDL_PBIST3_FAIL_INSERTION_TEST_VECTOR_CA0                                                  (0x00000000U)
#define SDL_PBIST3_FAIL_INSERTION_TEST_VECTOR_CA1                                                  (0x000001FFU)
#define SDL_PBIST3_FAIL_INSERTION_TEST_VECTOR_CA2                                                  (0x000001FFU)
#define SDL_PBIST3_FAIL_INSERTION_TEST_VECTOR_CA3                                                  (0x00000000U)
#define SDL_PBIST3_FAIL_INSERTION_TEST_VECTOR_CL0                                                  (0x0000007FU)
#define SDL_PBIST3_FAIL_INSERTION_TEST_VECTOR_CL1                                                  (0x00000003U)
#define SDL_PBIST3_FAIL_INSERTION_TEST_VECTOR_CL2                                                  (0x00000008U)
#define SDL_PBIST3_FAIL_INSERTION_TEST_VECTOR_CL3                                                  (0x000001FFU)
#define SDL_PBIST3_FAIL_INSERTION_TEST_VECTOR_CMS                                                  (0x00000004U)
#define SDL_PBIST3_FAIL_INSERTION_TEST_VECTOR_CSR                                                  (0x00000003U)
#define SDL_PBIST3_FAIL_INSERTION_TEST_VECTOR_I0                                                   (0x00000001U)
#define SDL_PBIST3_FAIL_INSERTION_TEST_VECTOR_I1                                                   (0x00000004U)
#define SDL_PBIST3_FAIL_INSERTION_TEST_VECTOR_I2                                                   (0x00000008U)
#define SDL_PBIST3_FAIL_INSERTION_TEST_VECTOR_I3                                                   (0x00000000U)
#define SDL_PBIST3_FAIL_INSERTION_TEST_VECTOR_RAMT                                                 (0x21002028U)

/* Properties of PBIST instances in: PBIST2 */
#define SDL_PBIST2_NUM_TEST_VECTORS                                                                (1U)
#define SDL_PBIST2_ALGO_BITMAP_0                                                                   (0x0000000000000003U)
#define SDL_PBIST2_MEM_BITMAP_0                                                                    (0x00000000000CCCCCU)
#define SDL_PBIST2_FAIL_INSERTION_TEST_VECTOR_CA0                                                  (0x00000000U)
#define SDL_PBIST2_FAIL_INSERTION_TEST_VECTOR_CA1                                                  (0x000001FFU)
#define SDL_PBIST2_FAIL_INSERTION_TEST_VECTOR_CA2                                                  (0x000001FFU)
#define SDL_PBIST2_FAIL_INSERTION_TEST_VECTOR_CA3                                                  (0x00000000U)
#define SDL_PBIST2_FAIL_INSERTION_TEST_VECTOR_CL0                                                  (0x0000007FU)
#define SDL_PBIST2_FAIL_INSERTION_TEST_VECTOR_CL1                                                  (0x00000003U)
#define SDL_PBIST2_FAIL_INSERTION_TEST_VECTOR_CL2                                                  (0x00000008U)
#define SDL_PBIST2_FAIL_INSERTION_TEST_VECTOR_CL3                                                  (0x000001FFU)
#define SDL_PBIST2_FAIL_INSERTION_TEST_VECTOR_CMS                                                  (0x00000000U)
#define SDL_PBIST2_FAIL_INSERTION_TEST_VECTOR_CSR                                                  (0x20000000U)
#define SDL_PBIST2_FAIL_INSERTION_TEST_VECTOR_I0                                                   (0x00000001U)
#define SDL_PBIST2_FAIL_INSERTION_TEST_VECTOR_I1                                                   (0x00000004U)
#define SDL_PBIST2_FAIL_INSERTION_TEST_VECTOR_I2                                                   (0x00000008U)
#define SDL_PBIST2_FAIL_INSERTION_TEST_VECTOR_I3                                                   (0x00000000U)
#define SDL_PBIST2_FAIL_INSERTION_TEST_VECTOR_RAMT                                                 (0x011D2528U)

/* Properties of PBIST instances in: PBIST1 */
#define SDL_PBIST1_NUM_TEST_VECTORS                                                                (2U)
#define SDL_PBIST1_ALGO_BITMAP_0                                                                   (0x0000000000000005U)
#define SDL_PBIST1_MEM_BITMAP_0                                                                    (0x0000000000000001U)
#define SDL_PBIST1_ALGO_BITMAP_1                                                                   (0x000000000000000AU)
#define SDL_PBIST1_MEM_BITMAP_1                                                                    (0x000000000000000CU)
#define SDL_PBIST1_FAIL_INSERTION_TEST_VECTOR_CA0                                                  (0x00000000U)
#define SDL_PBIST1_FAIL_INSERTION_TEST_VECTOR_CA1                                                  (0x000000D7U)
#define SDL_PBIST1_FAIL_INSERTION_TEST_VECTOR_CA2                                                  (0x000000D7U)
#define SDL_PBIST1_FAIL_INSERTION_TEST_VECTOR_CA3                                                  (0x00000057U)
#define SDL_PBIST1_FAIL_INSERTION_TEST_VECTOR_CL0                                                  (0x0000006BU)
#define SDL_PBIST1_FAIL_INSERTION_TEST_VECTOR_CL1                                                  (0x00000001U)
#define SDL_PBIST1_FAIL_INSERTION_TEST_VECTOR_CL2                                                  (0x00000007U)
#define SDL_PBIST1_FAIL_INSERTION_TEST_VECTOR_CL3                                                  (0x0000007FU)
#define SDL_PBIST1_FAIL_INSERTION_TEST_VECTOR_CMS                                                  (0x00000000U)
#define SDL_PBIST1_FAIL_INSERTION_TEST_VECTOR_CSR                                                  (0x0000007FU)
#define SDL_PBIST1_FAIL_INSERTION_TEST_VECTOR_I0                                                   (0x00000001U)
#define SDL_PBIST1_FAIL_INSERTION_TEST_VECTOR_I1                                                   (0x00000002U)
#define SDL_PBIST1_FAIL_INSERTION_TEST_VECTOR_I2                                                   (0x00000006U)
#define SDL_PBIST1_FAIL_INSERTION_TEST_VECTOR_I3                                                   (0x00000006U)
#define SDL_PBIST1_FAIL_INSERTION_TEST_VECTOR_RAMT                                                 (0x03002028U)

/* Properties of PBIST instances in: PBIST8 */
#define SDL_PBIST8_NUM_TEST_VECTORS                                                                (2U)
#define SDL_PBIST8_ALGO_BITMAP_0                                                                   (0x0000000000000005U)
#define SDL_PBIST8_MEM_BITMAP_0                                                                    (0x0000000000002849U)
#define SDL_PBIST8_ALGO_BITMAP_1                                                                   (0x000000000000000AU)
#define SDL_PBIST8_MEM_BITMAP_1                                                                    (0x0000000000028000U)
#define SDL_PBIST8_FAIL_INSERTION_TEST_VECTOR_CA0                                                  (0x00000000U)
#define SDL_PBIST8_FAIL_INSERTION_TEST_VECTOR_CA1                                                  (0x0000083FU)
#define SDL_PBIST8_FAIL_INSERTION_TEST_VECTOR_CA2                                                  (0x0000083FU)
#define SDL_PBIST8_FAIL_INSERTION_TEST_VECTOR_CA3                                                  (0x0000003FU)
#define SDL_PBIST8_FAIL_INSERTION_TEST_VECTOR_CL0                                                  (0x0000020FU)
#define SDL_PBIST8_FAIL_INSERTION_TEST_VECTOR_CL1                                                  (0x00000003U)
#define SDL_PBIST8_FAIL_INSERTION_TEST_VECTOR_CL2                                                  (0x0000000BU)
#define SDL_PBIST8_FAIL_INSERTION_TEST_VECTOR_CL3                                                  (0x000007FFU)
#define SDL_PBIST8_FAIL_INSERTION_TEST_VECTOR_CMS                                                  (0x00000000U)
#define SDL_PBIST8_FAIL_INSERTION_TEST_VECTOR_CSR                                                  (0x00010000U)
#define SDL_PBIST8_FAIL_INSERTION_TEST_VECTOR_I0                                                   (0x00000001U)
#define SDL_PBIST8_FAIL_INSERTION_TEST_VECTOR_I1                                                   (0x00000004U)
#define SDL_PBIST8_FAIL_INSERTION_TEST_VECTOR_I2                                                   (0x0000000AU)
#define SDL_PBIST8_FAIL_INSERTION_TEST_VECTOR_I3                                                   (0x00000005U)
#define SDL_PBIST8_FAIL_INSERTION_TEST_VECTOR_RAMT                                                 (0x00104038U)

/* Properties of PBIST instances in: PBIST10 */
#define SDL_PBIST10_NUM_TEST_VECTORS                                                               (1U)
#define SDL_PBIST10_ALGO_BITMAP_0                                                                  (0x0000000000000003U)
#define SDL_PBIST10_MEM_BITMAP_0                                                                   (0x00000000000CCCCCU)
#define SDL_PBIST10_FAIL_INSERTION_TEST_VECTOR_CA0                                                 (0x00000000U)
#define SDL_PBIST10_FAIL_INSERTION_TEST_VECTOR_CA1                                                 (0x000001FFU)
#define SDL_PBIST10_FAIL_INSERTION_TEST_VECTOR_CA2                                                 (0x000001FFU)
#define SDL_PBIST10_FAIL_INSERTION_TEST_VECTOR_CA3                                                 (0x00000000U)
#define SDL_PBIST10_FAIL_INSERTION_TEST_VECTOR_CL0                                                 (0x0000007FU)
#define SDL_PBIST10_FAIL_INSERTION_TEST_VECTOR_CL1                                                 (0x00000003U)
#define SDL_PBIST10_FAIL_INSERTION_TEST_VECTOR_CL2                                                 (0x00000008U)
#define SDL_PBIST10_FAIL_INSERTION_TEST_VECTOR_CL3                                                 (0x000001FFU)
#define SDL_PBIST10_FAIL_INSERTION_TEST_VECTOR_CMS                                                 (0x00000000U)
#define SDL_PBIST10_FAIL_INSERTION_TEST_VECTOR_CSR                                                 (0x20000000U)
#define SDL_PBIST10_FAIL_INSERTION_TEST_VECTOR_I0                                                  (0x00000001U)
#define SDL_PBIST10_FAIL_INSERTION_TEST_VECTOR_I1                                                  (0x00000004U)
#define SDL_PBIST10_FAIL_INSERTION_TEST_VECTOR_I2                                                  (0x00000008U)
#define SDL_PBIST10_FAIL_INSERTION_TEST_VECTOR_I3                                                  (0x00000000U)
#define SDL_PBIST10_FAIL_INSERTION_TEST_VECTOR_RAMT                                                (0x011D2528U)

/* Properties of PBIST instances in: PBIST5 */
#define SDL_PBIST5_NUM_TEST_VECTORS                                                                (2U)
#define SDL_PBIST5_ALGO_BITMAP_0                                                                   (0x0000000000000005U)
#define SDL_PBIST5_MEM_BITMAP_0                                                                    (0x000000000003AA4DU)
#define SDL_PBIST5_ALGO_BITMAP_1                                                                   (0x000000000000000AU)
#define SDL_PBIST5_MEM_BITMAP_1                                                                    (0x0000099225E00000U)
#define SDL_PBIST5_FAIL_INSERTION_TEST_VECTOR_CA0                                                  (0x00000000U)
#define SDL_PBIST5_FAIL_INSERTION_TEST_VECTOR_CA1                                                  (0x0000007FU)
#define SDL_PBIST5_FAIL_INSERTION_TEST_VECTOR_CA2                                                  (0x0000007FU)
#define SDL_PBIST5_FAIL_INSERTION_TEST_VECTOR_CA3                                                  (0x00000000U)
#define SDL_PBIST5_FAIL_INSERTION_TEST_VECTOR_CL0                                                  (0x0000001FU)
#define SDL_PBIST5_FAIL_INSERTION_TEST_VECTOR_CL1                                                  (0x00000003U)
#define SDL_PBIST5_FAIL_INSERTION_TEST_VECTOR_CL2                                                  (0x00000006U)
#define SDL_PBIST5_FAIL_INSERTION_TEST_VECTOR_CL3                                                  (0x0000007FU)
#define SDL_PBIST5_FAIL_INSERTION_TEST_VECTOR_CMS                                                  (0x00000004U)
#define SDL_PBIST5_FAIL_INSERTION_TEST_VECTOR_CSR                                                  (0x00000004U)
#define SDL_PBIST5_FAIL_INSERTION_TEST_VECTOR_I0                                                   (0x00000001U)
#define SDL_PBIST5_FAIL_INSERTION_TEST_VECTOR_I1                                                   (0x00000004U)
#define SDL_PBIST5_FAIL_INSERTION_TEST_VECTOR_I2                                                   (0x00000006U)
#define SDL_PBIST5_FAIL_INSERTION_TEST_VECTOR_I3                                                   (0x00000000U)
#define SDL_PBIST5_FAIL_INSERTION_TEST_VECTOR_RAMT                                                 (0x28022828U)

/* Properties of PBIST instances in: PBIST11 */
#define SDL_PBIST11_NUM_TEST_VECTORS                                                               (2U)
#define SDL_PBIST11_ALGO_BITMAP_0                                                                  (0x0000000000000005U)
#define SDL_PBIST11_MEM_BITMAP_0                                                                   (0x000000002A9552AAU)
#define SDL_PBIST11_ALGO_BITMAP_1                                                                  (0x000000000000000AU)
#define SDL_PBIST11_MEM_BITMAP_1                                                                   (0x0002AAA540000000U)
#define SDL_PBIST11_FAIL_INSERTION_TEST_VECTOR_CA0                                                 (0x00000000U)
#define SDL_PBIST11_FAIL_INSERTION_TEST_VECTOR_CA1                                                 (0x0000001FU)
#define SDL_PBIST11_FAIL_INSERTION_TEST_VECTOR_CA2                                                 (0x0000001FU)
#define SDL_PBIST11_FAIL_INSERTION_TEST_VECTOR_CA3                                                 (0x00000000U)
#define SDL_PBIST11_FAIL_INSERTION_TEST_VECTOR_CL0                                                 (0x0000001FU)
#define SDL_PBIST11_FAIL_INSERTION_TEST_VECTOR_CL1                                                 (0x00000000U)
#define SDL_PBIST11_FAIL_INSERTION_TEST_VECTOR_CL2                                                 (0x00000004U)
#define SDL_PBIST11_FAIL_INSERTION_TEST_VECTOR_CL3                                                 (0x0000001FU)
#define SDL_PBIST11_FAIL_INSERTION_TEST_VECTOR_CMS                                                 (0x00000000U)
#define SDL_PBIST11_FAIL_INSERTION_TEST_VECTOR_CSR                                                 (0x00000010U)
#define SDL_PBIST11_FAIL_INSERTION_TEST_VECTOR_I0                                                  (0x00000001U)
#define SDL_PBIST11_FAIL_INSERTION_TEST_VECTOR_I1                                                  (0x00000001U)
#define SDL_PBIST11_FAIL_INSERTION_TEST_VECTOR_I2                                                  (0x00000004U)
#define SDL_PBIST11_FAIL_INSERTION_TEST_VECTOR_I3                                                  (0x00000000U)
#define SDL_PBIST11_FAIL_INSERTION_TEST_VECTOR_RAMT                                                (0x20042C28U)

/* Properties of PBIST instances in: A72SS0_CORE0_PBIST_WRAP */
#define SDL_A72SS0_CORE0_PBIST_WRAP_NUM_TEST_VECTORS                                               (1U)
#define SDL_A72SS0_CORE0_PBIST_WRAP_ALGO_BITMAP_0                                                  (0x0000000000000003U)
#define SDL_A72SS0_CORE0_PBIST_WRAP_MEM_BITMAP_0                                                   (0x0FFFC000FF00FF00U)
#define SDL_A72SS0_CORE0_PBIST_WRAP_FAIL_INSERTION_TEST_VECTOR_CA0                                 (0x00000000U)
#define SDL_A72SS0_CORE0_PBIST_WRAP_FAIL_INSERTION_TEST_VECTOR_CA1                                 (0x000003FFU)
#define SDL_A72SS0_CORE0_PBIST_WRAP_FAIL_INSERTION_TEST_VECTOR_CA2                                 (0x000003FFU)
#define SDL_A72SS0_CORE0_PBIST_WRAP_FAIL_INSERTION_TEST_VECTOR_CA3                                 (0x00000000U)
#define SDL_A72SS0_CORE0_PBIST_WRAP_FAIL_INSERTION_TEST_VECTOR_CL0                                 (0x000000FFU)
#define SDL_A72SS0_CORE0_PBIST_WRAP_FAIL_INSERTION_TEST_VECTOR_CL1                                 (0x00000003U)
#define SDL_A72SS0_CORE0_PBIST_WRAP_FAIL_INSERTION_TEST_VECTOR_CL2                                 (0x00000009U)
#define SDL_A72SS0_CORE0_PBIST_WRAP_FAIL_INSERTION_TEST_VECTOR_CL3                                 (0x000003FFU)
#define SDL_A72SS0_CORE0_PBIST_WRAP_FAIL_INSERTION_TEST_VECTOR_CMS                                 (0x00000000U)
#define SDL_A72SS0_CORE0_PBIST_WRAP_FAIL_INSERTION_TEST_VECTOR_CSR                                 (0x00000003U)
#define SDL_A72SS0_CORE0_PBIST_WRAP_FAIL_INSERTION_TEST_VECTOR_I0                                  (0x00000001U)
#define SDL_A72SS0_CORE0_PBIST_WRAP_FAIL_INSERTION_TEST_VECTOR_I1                                  (0x00000004U)
#define SDL_A72SS0_CORE0_PBIST_WRAP_FAIL_INSERTION_TEST_VECTOR_I2                                  (0x00000009U)
#define SDL_A72SS0_CORE0_PBIST_WRAP_FAIL_INSERTION_TEST_VECTOR_I3                                  (0x00000000U)
#define SDL_A72SS0_CORE0_PBIST_WRAP_FAIL_INSERTION_TEST_VECTOR_RAMT                                (0x02004040U)

/* Properties of PBIST instances in: PBIST7 */
#define SDL_PBIST7_NUM_TEST_VECTORS                                                                (2U)
#define SDL_PBIST7_ALGO_BITMAP_0                                                                   (0x000000000000000AU)
#define SDL_PBIST7_MEM_BITMAP_0                                                                    (0x0000000000000800U)
#define SDL_PBIST7_ALGO_BITMAP_1                                                                   (0x0000000000000005U)
#define SDL_PBIST7_MEM_BITMAP_1                                                                    (0x00000000000002A4U)
#define SDL_PBIST7_FAIL_INSERTION_TEST_VECTOR_CA0                                                  (0x00000000U)
#define SDL_PBIST7_FAIL_INSERTION_TEST_VECTOR_CA1                                                  (0x00000BFFU)
#define SDL_PBIST7_FAIL_INSERTION_TEST_VECTOR_CA2                                                  (0x00000BFFU)
#define SDL_PBIST7_FAIL_INSERTION_TEST_VECTOR_CA3                                                  (0x000003FFU)
#define SDL_PBIST7_FAIL_INSERTION_TEST_VECTOR_CL0                                                  (0x0000017FU)
#define SDL_PBIST7_FAIL_INSERTION_TEST_VECTOR_CL1                                                  (0x00000007U)
#define SDL_PBIST7_FAIL_INSERTION_TEST_VECTOR_CL2                                                  (0x0000000BU)
#define SDL_PBIST7_FAIL_INSERTION_TEST_VECTOR_CL3                                                  (0x000007FFU)
#define SDL_PBIST7_FAIL_INSERTION_TEST_VECTOR_CMS                                                  (0x00000000U)
#define SDL_PBIST7_FAIL_INSERTION_TEST_VECTOR_CSR                                                  (0x00000010U)
#define SDL_PBIST7_FAIL_INSERTION_TEST_VECTOR_I0                                                   (0x00000001U)
#define SDL_PBIST7_FAIL_INSERTION_TEST_VECTOR_I1                                                   (0x00000008U)
#define SDL_PBIST7_FAIL_INSERTION_TEST_VECTOR_I2                                                   (0x0000000AU)
#define SDL_PBIST7_FAIL_INSERTION_TEST_VECTOR_I3                                                   (0x00000009U)
#define SDL_PBIST7_FAIL_INSERTION_TEST_VECTOR_RAMT                                                 (0x01041438U)

/* Properties of PBIST instances in: MCU_PBIST0 */
#define SDL_MCU_PBIST0_NUM_TEST_VECTORS                                                            (2U)
#define SDL_MCU_PBIST0_ALGO_BITMAP_0                                                               (0x0000000000000014U)
#define SDL_MCU_PBIST0_MEM_BITMAP_0                                                                (0x0000000000006080U)
#define SDL_MCU_PBIST0_ALGO_BITMAP_1                                                               (0x000000000000000AU)
#define SDL_MCU_PBIST0_MEM_BITMAP_1                                                                (0x000000000000001AU)
#define SDL_MCU_PBIST0_FAIL_INSERTION_TEST_VECTOR_CA0                                              (0x00000000U)
#define SDL_MCU_PBIST0_FAIL_INSERTION_TEST_VECTOR_CA1                                              (0x00001FFFU)
#define SDL_MCU_PBIST0_FAIL_INSERTION_TEST_VECTOR_CA2                                              (0x00001FFFU)
#define SDL_MCU_PBIST0_FAIL_INSERTION_TEST_VECTOR_CA3                                              (0x00000000U)
#define SDL_MCU_PBIST0_FAIL_INSERTION_TEST_VECTOR_CL0                                              (0x000003FFU)
#define SDL_MCU_PBIST0_FAIL_INSERTION_TEST_VECTOR_CL1                                              (0x00000007U)
#define SDL_MCU_PBIST0_FAIL_INSERTION_TEST_VECTOR_CL2                                              (0x0000000CU)
#define SDL_MCU_PBIST0_FAIL_INSERTION_TEST_VECTOR_CL3                                              (0x00001FFFU)
#define SDL_MCU_PBIST0_FAIL_INSERTION_TEST_VECTOR_CMS                                              (0x00000001U)
#define SDL_MCU_PBIST0_FAIL_INSERTION_TEST_VECTOR_CSR                                              (0x00000003U)
#define SDL_MCU_PBIST0_FAIL_INSERTION_TEST_VECTOR_I0                                               (0x00000001U)
#define SDL_MCU_PBIST0_FAIL_INSERTION_TEST_VECTOR_I1                                               (0x00000008U)
#define SDL_MCU_PBIST0_FAIL_INSERTION_TEST_VECTOR_I2                                               (0x0000000CU)
#define SDL_MCU_PBIST0_FAIL_INSERTION_TEST_VECTOR_I3                                               (0x00000000U)
#define SDL_MCU_PBIST0_FAIL_INSERTION_TEST_VECTOR_RAMT                                             (0x01004028U)

/* Properties of PBIST instances in: COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0 */
#define SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_NUM_TEST_VECTORS                                  (1U)
#define SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_ALGO_BITMAP_0                                     (0x0000000000000003U)
#define SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_MEM_BITMAP_0                                      (0x0000000000000234U)
#define SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CA0                    (0x00000000U)
#define SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CA1                    (0x000000FFU)
#define SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CA2                    (0x000000FFU)
#define SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CA3                    (0x00000000U)
#define SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CL0                    (0x0000003FU)
#define SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CL1                    (0x00000003U)
#define SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CL2                    (0x00000007U)
#define SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CL3                    (0x000000FFU)
#define SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CMS                    (0x00000000U)
#define SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CSR                    (0x00000003U)
#define SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_I0                     (0x00000001U)
#define SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_I1                     (0x00000004U)
#define SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_I2                     (0x00000007U)
#define SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_I3                     (0x00000000U)
#define SDL_COMPUTE_CLUSTER0_C71SS1_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_RAMT                   (0x0800400CU)

/* Properties of PBIST instances in: PBIST4 */
#define SDL_PBIST4_NUM_TEST_VECTORS                                                                (2U)
#define SDL_PBIST4_ALGO_BITMAP_0                                                                   (0x0000000000000005U)
#define SDL_PBIST4_MEM_BITMAP_0                                                                    (0x000000011D338D33U)
#define SDL_PBIST4_ALGO_BITMAP_1                                                                   (0x000000000000000AU)
#define SDL_PBIST4_MEM_BITMAP_1                                                                    (0x0000219C00000000U)
#define SDL_PBIST4_FAIL_INSERTION_TEST_VECTOR_CA0                                                  (0x00000000U)
#define SDL_PBIST4_FAIL_INSERTION_TEST_VECTOR_CA1                                                  (0x0000003FU)
#define SDL_PBIST4_FAIL_INSERTION_TEST_VECTOR_CA2                                                  (0x0000003FU)
#define SDL_PBIST4_FAIL_INSERTION_TEST_VECTOR_CA3                                                  (0x00000000U)
#define SDL_PBIST4_FAIL_INSERTION_TEST_VECTOR_CL0                                                  (0x0000000FU)
#define SDL_PBIST4_FAIL_INSERTION_TEST_VECTOR_CL1                                                  (0x00000003U)
#define SDL_PBIST4_FAIL_INSERTION_TEST_VECTOR_CL2                                                  (0x00000005U)
#define SDL_PBIST4_FAIL_INSERTION_TEST_VECTOR_CL3                                                  (0x0000003FU)
#define SDL_PBIST4_FAIL_INSERTION_TEST_VECTOR_CMS                                                  (0x00000000U)
#define SDL_PBIST4_FAIL_INSERTION_TEST_VECTOR_CSR                                                  (0x00000003U)
#define SDL_PBIST4_FAIL_INSERTION_TEST_VECTOR_I0                                                   (0x00000001U)
#define SDL_PBIST4_FAIL_INSERTION_TEST_VECTOR_I1                                                   (0x00000004U)
#define SDL_PBIST4_FAIL_INSERTION_TEST_VECTOR_I2                                                   (0x00000005U)
#define SDL_PBIST4_FAIL_INSERTION_TEST_VECTOR_I3                                                   (0x00000000U)
#define SDL_PBIST4_FAIL_INSERTION_TEST_VECTOR_RAMT                                                 (0x40004028U)

/* Properties of PBIST instances in: COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0 */
#define SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_NUM_TEST_VECTORS                                  (1U)
#define SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_ALGO_BITMAP_0                                     (0x0000000000000003U)
#define SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_MEM_BITMAP_0                                      (0x0000000000000234U)
#define SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CA0                    (0x00000000U)
#define SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CA1                    (0x000000FFU)
#define SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CA2                    (0x000000FFU)
#define SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CA3                    (0x00000000U)
#define SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CL0                    (0x0000003FU)
#define SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CL1                    (0x00000003U)
#define SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CL2                    (0x00000007U)
#define SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CL3                    (0x000000FFU)
#define SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CMS                    (0x00000000U)
#define SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CSR                    (0x00000003U)
#define SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_I0                     (0x00000001U)
#define SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_I1                     (0x00000004U)
#define SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_I2                     (0x00000007U)
#define SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_I3                     (0x00000000U)
#define SDL_COMPUTE_CLUSTER0_C71SS0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_RAMT                   (0x0800400CU)

/* Properties of PBIST instances in: MCU_PBIST1 */
#define SDL_MCU_PBIST1_NUM_TEST_VECTORS                                                            (2U)
#define SDL_MCU_PBIST1_ALGO_BITMAP_0                                                               (0x0000000000000014U)
#define SDL_MCU_PBIST1_MEM_BITMAP_0                                                                (0x000000001003D134U)
#define SDL_MCU_PBIST1_ALGO_BITMAP_1                                                               (0x0000000000000028U)
#define SDL_MCU_PBIST1_MEM_BITMAP_1                                                                (0x0010A91000000002U)
#define SDL_MCU_PBIST1_FAIL_INSERTION_TEST_VECTOR_CA0                                              (0x00000000U)
#define SDL_MCU_PBIST1_FAIL_INSERTION_TEST_VECTOR_CA1                                              (0x0000002FU)
#define SDL_MCU_PBIST1_FAIL_INSERTION_TEST_VECTOR_CA2                                              (0x0000002FU)
#define SDL_MCU_PBIST1_FAIL_INSERTION_TEST_VECTOR_CA3                                              (0x0000000FU)
#define SDL_MCU_PBIST1_FAIL_INSERTION_TEST_VECTOR_CL0                                              (0x0000000BU)
#define SDL_MCU_PBIST1_FAIL_INSERTION_TEST_VECTOR_CL1                                              (0x00000003U)
#define SDL_MCU_PBIST1_FAIL_INSERTION_TEST_VECTOR_CL2                                              (0x00000005U)
#define SDL_MCU_PBIST1_FAIL_INSERTION_TEST_VECTOR_CL3                                              (0x0000001FU)
#define SDL_MCU_PBIST1_FAIL_INSERTION_TEST_VECTOR_CMS                                              (0x00000002U)
#define SDL_MCU_PBIST1_FAIL_INSERTION_TEST_VECTOR_CSR                                              (0x00003000U)
#define SDL_MCU_PBIST1_FAIL_INSERTION_TEST_VECTOR_I0                                               (0x00000001U)
#define SDL_MCU_PBIST1_FAIL_INSERTION_TEST_VECTOR_I1                                               (0x00000004U)
#define SDL_MCU_PBIST1_FAIL_INSERTION_TEST_VECTOR_I2                                               (0x00000004U)
#define SDL_MCU_PBIST1_FAIL_INSERTION_TEST_VECTOR_I3                                               (0x00000003U)
#define SDL_MCU_PBIST1_FAIL_INSERTION_TEST_VECTOR_RAMT                                             (0x000C4028U)

/* Properties of PBIST instances in: MCU_PBIST2 */
#define SDL_MCU_PBIST2_NUM_TEST_VECTORS                                                            (1U)
#define SDL_MCU_PBIST2_ALGO_BITMAP_0                                                               (0x0000000000000006U)
#define SDL_MCU_PBIST2_MEM_BITMAP_0                                                                (0x0000000000199998U)
#define SDL_MCU_PBIST2_FAIL_INSERTION_TEST_VECTOR_CA0                                              (0x00000000U)
#define SDL_MCU_PBIST2_FAIL_INSERTION_TEST_VECTOR_CA1                                              (0x000001FFU)
#define SDL_MCU_PBIST2_FAIL_INSERTION_TEST_VECTOR_CA2                                              (0x000001FFU)
#define SDL_MCU_PBIST2_FAIL_INSERTION_TEST_VECTOR_CA3                                              (0x00000000U)
#define SDL_MCU_PBIST2_FAIL_INSERTION_TEST_VECTOR_CL0                                              (0x0000007FU)
#define SDL_MCU_PBIST2_FAIL_INSERTION_TEST_VECTOR_CL1                                              (0x00000003U)
#define SDL_MCU_PBIST2_FAIL_INSERTION_TEST_VECTOR_CL2                                              (0x00000008U)
#define SDL_MCU_PBIST2_FAIL_INSERTION_TEST_VECTOR_CL3                                              (0x000001FFU)
#define SDL_MCU_PBIST2_FAIL_INSERTION_TEST_VECTOR_CMS                                              (0x00000000U)
#define SDL_MCU_PBIST2_FAIL_INSERTION_TEST_VECTOR_CSR                                              (0x00010000U)
#define SDL_MCU_PBIST2_FAIL_INSERTION_TEST_VECTOR_I0                                               (0x00000001U)
#define SDL_MCU_PBIST2_FAIL_INSERTION_TEST_VECTOR_I1                                               (0x00000004U)
#define SDL_MCU_PBIST2_FAIL_INSERTION_TEST_VECTOR_I2                                               (0x00000008U)
#define SDL_MCU_PBIST2_FAIL_INSERTION_TEST_VECTOR_I3                                               (0x00000000U)
#define SDL_MCU_PBIST2_FAIL_INSERTION_TEST_VECTOR_RAMT                                             (0x04102528U)

/* Properties of PBIST instances in: PBIST0 */
#define SDL_PBIST0_NUM_TEST_VECTORS                                                                (2U)
#define SDL_PBIST0_ALGO_BITMAP_0                                                                   (0x000000000000000AU)
#define SDL_PBIST0_MEM_BITMAP_0                                                                    (0x0000000000001800U)
#define SDL_PBIST0_ALGO_BITMAP_1                                                                   (0x0000000000000005U)
#define SDL_PBIST0_MEM_BITMAP_1                                                                    (0x00000000000001AAU)
#define SDL_PBIST0_FAIL_INSERTION_TEST_VECTOR_CA0                                                  (0x00000000U)
#define SDL_PBIST0_FAIL_INSERTION_TEST_VECTOR_CA1                                                  (0x000001FFU)
#define SDL_PBIST0_FAIL_INSERTION_TEST_VECTOR_CA2                                                  (0x000001FFU)
#define SDL_PBIST0_FAIL_INSERTION_TEST_VECTOR_CA3                                                  (0x00000000U)
#define SDL_PBIST0_FAIL_INSERTION_TEST_VECTOR_CL0                                                  (0x0000007FU)
#define SDL_PBIST0_FAIL_INSERTION_TEST_VECTOR_CL1                                                  (0x00000003U)
#define SDL_PBIST0_FAIL_INSERTION_TEST_VECTOR_CL2                                                  (0x00000008U)
#define SDL_PBIST0_FAIL_INSERTION_TEST_VECTOR_CL3                                                  (0x000001FFU)
#define SDL_PBIST0_FAIL_INSERTION_TEST_VECTOR_CMS                                                  (0x00000001U)
#define SDL_PBIST0_FAIL_INSERTION_TEST_VECTOR_CSR                                                  (0x00000003U)
#define SDL_PBIST0_FAIL_INSERTION_TEST_VECTOR_I0                                                   (0x00000001U)
#define SDL_PBIST0_FAIL_INSERTION_TEST_VECTOR_I1                                                   (0x00000004U)
#define SDL_PBIST0_FAIL_INSERTION_TEST_VECTOR_I2                                                   (0x00000008U)
#define SDL_PBIST0_FAIL_INSERTION_TEST_VECTOR_I3                                                   (0x00000000U)
#define SDL_PBIST0_FAIL_INSERTION_TEST_VECTOR_RAMT                                                 (0x05002028U)

/* Properties of PBIST instances in: AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0 */
#define SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_NUM_TEST_VECTORS                                (2U)
#define SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_ALGO_BITMAP_0                                   (0x0000000000000005U)
#define SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_MEM_BITMAP_0                                    (0x00000000B0CC6666U)
#define SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_ALGO_BITMAP_1                                   (0x000000000000000AU)
#define SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_MEM_BITMAP_1                                    (0x1158C98C00000000U)
#define SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_FAIL_INSERTION_TEST_VECTOR_CA0                  (0x00000000U)
#define SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_FAIL_INSERTION_TEST_VECTOR_CA1                  (0x0000001FU)
#define SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_FAIL_INSERTION_TEST_VECTOR_CA2                  (0x0000001FU)
#define SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_FAIL_INSERTION_TEST_VECTOR_CA3                  (0x00000000U)
#define SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_FAIL_INSERTION_TEST_VECTOR_CL0                  (0x0000001FU)
#define SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_FAIL_INSERTION_TEST_VECTOR_CL1                  (0x00000000U)
#define SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_FAIL_INSERTION_TEST_VECTOR_CL2                  (0x00000004U)
#define SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_FAIL_INSERTION_TEST_VECTOR_CL3                  (0x0000001FU)
#define SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_FAIL_INSERTION_TEST_VECTOR_CMS                  (0x00000000U)
#define SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_FAIL_INSERTION_TEST_VECTOR_CSR                  (0x00003E00U)
#define SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_FAIL_INSERTION_TEST_VECTOR_I0                   (0x00000001U)
#define SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_FAIL_INSERTION_TEST_VECTOR_I1                   (0x00000001U)
#define SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_FAIL_INSERTION_TEST_VECTOR_I2                   (0x00000004U)
#define SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_FAIL_INSERTION_TEST_VECTOR_I3                   (0x00000000U)
#define SDL_AEP_GPU_BXS464_WRAP0_DFT_EMBED_PBIST_0_FAIL_INSERTION_TEST_VECTOR_RAMT                 (0x0F092038U)

/* Properties of PBIST instances in: COMPUTE_CLUSTER0_PBIST_WRAP_0 */
#define SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_NUM_TEST_VECTORS                                         (2U)
#define SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_ALGO_BITMAP_0                                            (0x0000000000000005U)
#define SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_MEM_BITMAP_0                                             (0x00000000000107E5U)
#define SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_ALGO_BITMAP_1                                            (0x000000000000000AU)
#define SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_MEM_BITMAP_1                                             (0x001A000000000000U)
#define SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CA0                           (0x00000000U)
#define SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CA1                           (0x0000043FU)
#define SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CA2                           (0x0000043FU)
#define SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CA3                           (0x0000003FU)
#define SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CL0                           (0x0000010FU)
#define SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CL1                           (0x00000003U)
#define SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CL2                           (0x0000000AU)
#define SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CL3                           (0x000003FFU)
#define SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CMS                           (0x00000001U)
#define SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_CSR                           (0x00000004U)
#define SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_I0                            (0x00000001U)
#define SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_I1                            (0x00000004U)
#define SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_I2                            (0x00000009U)
#define SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_I3                            (0x00000005U)
#define SDL_COMPUTE_CLUSTER0_PBIST_WRAP_0_FAIL_INSERTION_TEST_VECTOR_RAMT                          (0x54021020U)

#ifdef __cplusplus
}
#endif
#endif /* SDLR_SOC_PBIST_H_ */

