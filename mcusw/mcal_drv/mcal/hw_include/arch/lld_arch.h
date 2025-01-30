/* =============================================================================
 *   Copyright (c) Texas Instruments Incorporated 2024
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

/** ============================================================================
 *   \file  lld_arch.h
 *
 *   \brief This file contains the ARCH related API's
 *  ============================================================================
 */
#ifndef LLD_ARCH_TOP_H_
#define LLD_ARCH_TOP_H_

#include "Std_Types.h"
#include <hw_include/lld.h>

#if defined(SOC_J721E)

#if defined (_TMS320C6X)
#include <hw_include/arch/c66x/hw_dsp_icfg.h>
#if !defined (SOC_TPR12) && !defined (SOC_AWR294X)
#include <hw_include/arch/c66x/hw_dsp_system.h>
#include <hw_include/arch/c66x/dsp_config.h>
#include <hw_include/arch/c66x/dsp_wugen.h>
#endif
#include <hw_include/arch/c66x/interrupt.h>

#if defined (SOC_J721E)
#include <hw_include/arch/c66x/dsp_ecc.h>
#include <hw_include/arch/c66x/dsp_icfg.h>
#include <hw_include/arch/c66x/dsp_usrSpvSupport.h>
#endif
#endif

#endif

#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J784S4) || defined (SOC_J721S2) || defined(SOC_AM62X)
#if defined (__aarch64__)
#include <hw_include/arch/a53/lld_a53.h>
#include <hw_include/arch/a53/lld_a53v8cache.h>
#include <hw_include/arch/a53/lld_a53v8func.h>
#include <hw_include/arch/a53/lld_a53v8misc.h>
#include <hw_include/arch/a53/interrupt.h>
#endif
//#if defined (__TI_ARM_V7R4__)
#if (__ARM_ARCH == 7) && (__ARM_ARCH_PROFILE == 'R') /* R5F */
#include <hw_include/arch/r5/lld_vim.h>
#include <hw_include/arch/r5/lld_arm_r5.h>
#include <hw_include/arch/r5/lld_arm_r5_mpu.h>
#include <hw_include/arch/r5/lld_arm_r5_pmu.h>
#include <hw_include/arch/r5/lld_arm_r5_hard_err_cache.h>
#include <hw_include/arch/r5/interrupt.h>
#include <hw_include/arch/r5/lld_cache.h>
#include <hw_include/arch/r5/lld_mcu_armss_ccmr5.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

static inline void CSL_archMemoryFence(void);
static inline void CSL_archMemoryFence(void)
{
#if defined (__aarch64__)
    CSL_a53v8DsbSy();
#endif
#if (__ARM_ARCH == 7) && (__ARM_ARCH_PROFILE == 'R') /* R5F */
    asm(" dsb");
#endif
}

#ifdef __cplusplus
}
#endif

#endif

#endif

/**
 *  \defgroup LLD_ARCH CSL ARCH Module
 */
