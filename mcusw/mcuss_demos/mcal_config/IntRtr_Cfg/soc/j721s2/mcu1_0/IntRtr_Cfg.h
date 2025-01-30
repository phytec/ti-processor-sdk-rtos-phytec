/*
*
* Copyright (c) 2024 Texas Instruments Incorporated
*
* All rights reserved not granted herein.
*
* Limited License.
*
* Texas Instruments Incorporated grants a world-wide, royalty-free, non-exclusive
* license under copyrights and patents it now or hereafter owns or controls to make,
* have made, use, import, offer to sell and sell ("Utilize") this software subject to the
* terms herein.  With respect to the foregoing patent license, such license is granted
* solely to the extent that any such patent is necessary to Utilize the software alone.
* The patent license shall not apply to any combinations which include this software,
* other than combinations with devices manufactured by or for TI ("TI Devices").
* No hardware patent is licensed hereunder.
*
* Redistributions must preserve existing copyright notices and reproduce this license
* (including the above copyright notice and the disclaimer and (if applicable) source
* code license limitations below) in the documentation and/or other materials provided
* with the distribution
*
* Redistribution and use in binary form, without modification, are permitted provided
* that the following conditions are met:
*
* *       No reverse engineering, decompilation, or disassembly of this software is
* permitted with respect to any software provided in binary form.
*
* *       any redistribution and use are licensed by TI for use only with TI Devices.
*
* *       Nothing shall obligate TI to provide you with source code for the software
* licensed and provided to you in object code.
*
* If software source code is provided to you, modification and redistribution of the
* source code are permitted provided that the following conditions are met:
*
* *       any redistribution and use of the source code, including any resulting derivative
* works, are licensed by TI for use only with TI Devices.
*
* *       any redistribution and use of any object code compiled from the source code
* and any resulting derivative works, are licensed by TI for use only with TI Devices.
*
* Neither the name of Texas Instruments Incorporated nor the names of its suppliers
*
* may be used to endorse or promote products derived from this software without
* specific prior written permission.
*
* DISCLAIMER.
*
* THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
* OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

/**
 *  \file     IntRtr_Cfg.h
 *
 *  \brief    Defines the router configurations used in example / demo
 *              applications.
 *
 */

#ifndef INTRTR_CFG_H_
#define INTRTR_CFG_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "Std_Types.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */


/* ========================================================================== */
/*                          J721S2 SoC Specifics                               */
/* ========================================================================== */
/** \brief Interrupt ID's for J721S2 SoC
 *
 */
#define INTRTR_CFG_TOTAL_INPUT_INT_TYPE_PULSE_SUPPORTED    (104U)
/**< Total number of inputs interrupts supported */
#define INTRTR_CFG_TOTAL_OUTPUT_INT_TYPE_PULSE_SUPPORTED   (48U)
/**< Total number of output interrupts supported */

#define INTRTR_CFG_TOTAL_INPUT_INT_TYPE_LVL_SUPPORTED    (320U)
/**< Total number of inputs interrupts supported */
#define INTRTR_CFG_TOTAL_OUTPUT_INT_TYPE_LVL_SUPPORTED   (64U)
/**< Total number of output interrupts supported */

#define NAVSS0_INTR_ROUTER0_TOTAL_INPUT_INT_SUPPORTED   (440U)
/**< Total number of inputs interrupts supported */
#define NAVSS0_INTR_ROUTER0_TOTAL_OUTPUT_INT_SUPPORTED  (512U)
/**< Total number of inputs interrupts supported */

#define INTRTR_CFG_START_LEVEL_INT_NUMBER \
                                    (CSLR_MCU_ARMSS0_CPU0_INTR_MAIN2MCU_LVL_INTRTR0_OUTL_0)
/**< Total number of LEVEL output interrupts supported */
#define INTRTR_CFG_START_PULSE_INT_NUMBER \
                                    (CSLR_MCU_ARMSS0_CPU0_INTR_MAIN2MCU_PULSE_INTRTR0_OUTL_0)
/**< Total number of PULSE output interrupts supported */

#define INTRTR_CFG_MAIN_DOMAIN_MCSPI_1_OUT_INT_NO   (0U)
/**< Main domain McSPI instance 1 interrupt is routed on output int no */
#define INTRTR_CFG_MAIN_DOMAIN_MCSPI_2_OUT_INT_NO   (1U)
/**< Main domain McSPI instance 2 interrupt is routed on output int no */
#define INTRTR_CFG_MAIN_DOMAIN_MCSPI_3_OUT_INT_NO   (2U)
/**< Main domain McSPI instance 3 interrupt is routed on output int no */
#define INTRTR_CFG_MAIN_DOMAIN_MCSPI_4_OUT_INT_NO   (3U)
/**< Main domain McSPI instance 4 interrupt is routed on output int no */

#define INTRTR_CFG_MAIN_DOMAIN_TIMER_1_OUT_INT_NO   (4U)
/**< Main domain Timer instance 1 interrupt is routed on output int no */
#define INTRTR_CFG_MAIN_DOMAIN_TIMER_2_OUT_INT_NO   (5U)
/**< Main domain Timer instance 2 interrupt is routed on output int no */
#define INTRTR_CFG_MAIN_DOMAIN_TIMER_3_OUT_INT_NO   (6U)
/**< Main domain Timer instance 3 interrupt is routed on output int no */
#define INTRTR_CFG_MAIN_DOMAIN_TIMER_4_OUT_INT_NO   (7U)
/**< Main domain Timer instance 4 interrupt is routed on output int no */
#define INTRTR_CFG_MAIN_DOMAIN_TIMER_5_OUT_INT_NO   (8U)
/**< Main domain Timer instance 5 interrupt is routed on output int no */
#define INTRTR_CFG_MAIN_DOMAIN_TIMER_6_OUT_INT_NO   (9U)
/**< Main domain Timer instance 6 interrupt is routed on output int no */
#define INTRTR_CFG_MAIN_DOMAIN_TIMER_7_OUT_INT_NO   (10U)
/**< Main domain Timer instance 7 interrupt is routed on output int no */
#define INTRTR_CFG_MAIN_DOMAIN_TIMER_8_OUT_INT_NO   (11U)
/**< Main domain Timer instance 8 interrupt is routed on output int no */
#define INTRTR_CFG_MAIN_DOMAIN_TIMER_9_OUT_INT_NO   (12U)
/**< Main domain Timer instance 9 interrupt is routed on output int no */
#define INTRTR_CFG_MAIN_DOMAIN_TIMER_10_OUT_INT_NO  (13U)
/**< Main domain Timer instance 10 interrupt is routed on output int no */
#define INTRTR_CFG_MAIN_DOMAIN_TIMER_11_OUT_INT_NO  (14U)
/**< Main domain Timer instance 11 interrupt is routed on output int no */
#define INTRTR_CFG_MAIN_DOMAIN_TIMER_12_OUT_INT_NO  (15U)
/**< Main domain Timer instance 12 interrupt is routed on output int no */
#define INTRTR_CFG_MAIN_DOMAIN_TIMER_13_OUT_INT_NO  (16U)
/**< Main domain Timer instance 13 interrupt is routed on output int no */
#define INTRTR_CFG_MAIN_DOMAIN_TIMER_14_OUT_INT_NO  (17U)
/**< Main domain Timer instance 14 interrupt is routed on output int no */
#define INTRTR_CFG_MAIN_DOMAIN_TIMER_15_OUT_INT_NO  (18U)
/**< Main domain Timer instance 15 interrupt is routed on output int no */
#define INTRTR_CFG_MAIN_DOMAIN_TIMER_16_OUT_INT_NO  (19U)
/**< Main domain Timer instance 16 interrupt is routed on output int no */
#define INTRTR_CFG_MAIN_DOMAIN_TIMER_17_OUT_INT_NO  (20U)
/**< Main domain Timer instance 17 interrupt is routed on output int no */
#define INTRTR_CFG_MAIN_DOMAIN_TIMER_18_OUT_INT_NO  (21U)
/**< Main domain Timer instance 18 interrupt is routed on output int no */
#define INTRTR_CFG_MAIN_DOMAIN_TIMER_19_OUT_INT_NO  (22U)
/**< Main domain Timer instance 19 interrupt is routed on output int no */
#define INTRTR_CFG_MAIN_DOMAIN_TIMER_20_OUT_INT_NO  (23U)
/**< Main domain Timer instance 20 interrupt is routed on output int no */
/* EPWM numbers will be repeated to accomodate the VIM intr number changes in PDK sysfw */
#define INTRTR_CFG_MAIN_DOMAIN_EPWM_0_OUT_INT_NO    (0U)
/**< Main domain EPWM instance 0 interrupt is routed on output int no */
#define INTRTR_CFG_MAIN_DOMAIN_EPWM_1_OUT_INT_NO    (1U)
/**< Main domain EPWM instance 1 interrupt is routed on output int no */
#define INTRTR_CFG_MAIN_DOMAIN_EPWM_2_OUT_INT_NO    (2U)
/**< Main domain EPWM instance 2 interrupt is routed on output int no */
#define INTRTR_CFG_MAIN_DOMAIN_EPWM_3_OUT_INT_NO    (3U)
/**< Main domain EPWM instance 3 interrupt is routed on output int no */
#define INTRTR_CFG_MAIN_DOMAIN_EPWM_4_OUT_INT_NO    (4U)
/**< Main domain EPWM instance 4 interrupt is routed on output int no */
#define INTRTR_CFG_MAIN_DOMAIN_EPWM_5_OUT_INT_NO    (5U)
/**< Main domain EPWM instance 5 interrupt is routed on output int no */

#define INTRTR_CFG_MBX_CLST0_USR1_437_MPU_1_0_TO_MCU_1_0_376    (376U)
/**< J721S2 Main domain MAILBOX 0, Cluster 0 and User 1 */
#define INTRTR_CFG_MBX_CLST7_USR0_408_MCU_2_0_TO_MCU_1_0_377    (377U)
/**< J721S2 Main domain MAILBOX 0, Cluster 7 and User 0 */
#define INTRTR_CFG_MBX_CLST1_USR2_434_MPU_1_0_TO_MCU_2_1_248    (248U)
#define INTRTR_CFG_MBX_CLST1_USR2_434_MCU_1_1_TO_MCU_2_1_249    (249U)
#define INTRTR_CFG_MBX_CLST1_USR2_434_MCU_2_0_TO_MCU_2_1_250    (250U)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */


#ifdef __cplusplus
}
#endif

#endif  /* #ifndef INTRTR_CFG_H_ */
