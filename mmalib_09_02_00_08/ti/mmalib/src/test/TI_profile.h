/*******************************************************************************
**+--------------------------------------------------------------------------+**
**|                            ****                                          |**
**|                            ****                                          |**
**|                            ******o***                                    |**
**|                      ********_///_****                                   |**
**|                      ***** /_//_/ ****                                   |**
**|                       ** ** (__/ ****                                    |**
**|                           *********                                      |**
**|                            ****                                          |**
**|                            ***                                           |**
**|                                                                          |**
**|        Copyright (c) 2007 - 2017 Texas Instruments Incorporated          |**
**|                                                                          |**
**|              All rights reserved not granted herein.                     |**
**|                                                                          |**
**|                         Limited License.                                 |**
**|                                                                          |**
**|  Texas Instruments Incorporated grants a world-wide, royalty-free,       |**
**|  non-exclusive license under copyrights and patents it now or            |**
**|  hereafter owns or controls to make, have made, use, import, offer to    |**
**|  sell and sell ("Utilize") this software subject to the terms herein.    |**
**|  With respect to the foregoing patent license, such license is granted   |**
**|  solely to the extent that any such patent is necessary to Utilize the   |**
**|  software alone.  The patent license shall not apply to any              |**
**|  combinations which include this software, other than combinations       |**
**|  with devices manufactured by or for TI ('TI Devices').  No hardware     |**
**|  patent is licensed hereunder.                                           |**
**|                                                                          |**
**|  Redistributions must preserve existing copyright notices and            |**
**|  reproduce this license (including the above copyright notice and the    |**
**|  disclaimer and (if applicable) source code license limitations below)   |**
**|  in the documentation and/or other materials provided with the           |**
**|  distribution                                                            |**
**|                                                                          |**
**|  Redistribution and use in binary form, without modification, are        |**
**|  permitted provided that the following conditions are met:               |**
**|                                                                          |**
**|    *  No reverse engineering, decompilation, or disassembly of this      |**
**|  software is permitted with respect to any software provided in binary   |**
**|  form.                                                                   |**
**|                                                                          |**
**|    *  any redistribution and use are licensed by TI for use only with    |**
**|  TI Devices.                                                             |**
**|                                                                          |**
**|    *  Nothing shall obligate TI to provide you with source code for      |**
**|  the software licensed and provided to you in object code.               |**
**|                                                                          |**
**|  If software source code is provided to you, modification and            |**
**|  redistribution of the source code are permitted provided that the       |**
**|  following conditions are met:                                           |**
**|                                                                          |**
**|    *  any redistribution and use of the source code, including any       |**
**|  resulting derivative works, are licensed by TI for use only with TI     |**
**|  Devices.                                                                |**
**|                                                                          |**
**|    *  any redistribution and use of any object code compiled from the    |**
**|  source code and any resulting derivative works, are licensed by TI      |**
**|  for use only with TI Devices.                                           |**
**|                                                                          |**
**|  Neither the name of Texas Instruments Incorporated nor the names of     |**
**|  its suppliers may be used to endorse or promote products derived from   |**
**|  this software without specific prior written permission.                |**
**|                                                                          |**
**|  DISCLAIMER.                                                             |**
**|                                                                          |**
**|  THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY      |**
**|  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE       |**
**|  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR      |**
**|  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL TI AND TI'S LICENSORS BE      |**
**|  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR     |**
**|  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF    |**
**|  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR         |**
**|  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,   |**
**|  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE    |**
**|  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,       |**
**|  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                      |**
**+--------------------------------------------------------------------------+**
*******************************************************************************/

#ifndef TI_PROFILE_H_
#define TI_PROFILE_H_ 1

/* Memory utility includes */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "TI_test.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Global test case index */
extern int    test_index;       /* Index of current active test */
extern int    act_kernel;       /* Active kernel type for test and profile */

/* Kernel routine type enumeration (must not use explicit numbering) */
#define  TI_PROFILE_KERNEL_OPT          0         /* Kernel type optimized (C-intrinsics, assembly) */
#define  TI_PROFILE_KERNEL_OPT_WARM     1         /* Kernel type optimized (C-intrinsics, assembly) with warm instruction cache */
#define  TI_PROFILE_KERNEL_OPT_WARMWRB  2         /* Kernel type optimized (C-intrinsics, assembly) with warm instruction cache, warm output cache */
#define  TI_PROFILE_KERNEL_CN           3         /* Kernel type natural C */
#define  TI_PROFILE_KERNEL_INIT         4         /* Kernel init for C6x, C7x */
#define  TI_PROFILE_KERNEL_CNINIT       5         /* Kernel init for natural C */
#define  TI_PROFILE_KERNEL_CNT          6

/* Profiling defines */
#define TI_PROFILE_MAX_TESTS     32
#define TI_PROFILE_MAX_ORDER      4
#define TI_PROFILE_MAX_MODES      6
#define TI_PROFILE_FORMULA        0
#define TI_PROFILE_RANGE          1
#define TI_PROFILE_FORMULA_RANGE  2

/* Profiling macros */
#ifdef _HOST_BUILD
    #define PROFILE_INIT
    #define PROFILE_READ   0
#else
        #define PROFILE_INIT
        #define PROFILE_READ   (__get_GTSC(1), __STSC)
#endif

/* Profiling globals */
extern uint64_t    beg_count; /* Begin cycle count for profiling */
extern uint64_t    end_count; /* End cycle count for profiling */
extern uint64_t    overhead;  /* Cycle profiling overhead */
extern uint64_t    cycles[TI_PROFILE_KERNEL_CNT]; /* Cycle counts for C-natural & optimized */

void TI_profile_init(char *kernel_name);
int64_t TI_profile_get_cycles(int kernel);
void TI_profile_add_test(uint32_t testNum, int Ax0, uint64_t arch_cycles, uint64_t est_cycles, int errorFlag, char *desc);
void TI_profile_clear_run_stats ();
void TI_profile_skip_test (char *desc);
void TI_profile_cycle_report (int reportType, char *formulaInfo, char *rangeInfo);
void TI_profile_initStack(uint32_t SP);
void TI_profile_setStackDepth(void);
uint32_t TI_profile_getSP(void);
void TI_profile_stack_memory(void);
void TI_profile_clear_cycle_counts(void);
void TI_profile_clear_stack_sizes(void);
void TI_profile_clear_cycle_count_single(int kernel);

/* Profiling start routine */
static inline void TI_profile_start (int kernel)
{
    /* Set the active kernel for subsequent calls */
    act_kernel = kernel;

    /* Invalidate the cache */
    TI_cache_inval();

    /* Register starting timestamp */
    beg_count = PROFILE_READ;
}

/* Profiling stop (and calculate) routine */
static inline void TI_profile_stop (void)
{
    /* Register ending timestamp */
    end_count = PROFILE_READ;

    /* Calculate and store cycles */
    cycles[act_kernel] += end_count - beg_count - overhead;

}


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* =========================================================================*/
/*  End of file:  TI_profile.h                                              */
/* =========================================================================*/
