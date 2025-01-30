/**
 *   Copyright (c) Texas Instruments Incorporated 2018
 *   All rights reserved.
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
 *  \file vhwa_utils.h
 *
 *  \brief File containing utility APIs
 *
 */

#ifndef VHWA_UTILS_H_
#define VHWA_UTILS_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/csl/soc.h>
#include <ti/osal/osal.h>

/* UTILS is currently enabling irq in INTD module,
 * so including VPAC CSLR here.
 * TODO: Move interrupt enable in INTD to common place */
#include <ti/drv/vhwa/soc/vhwa_soc.h>
#include <ti/csl/cslr_dmpac.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
/**< SL2 Instance ID for VPAC */
#define VHWA_SL2_INST_VPAC      (1U)
/**< SL2 Instance ID for DMPAC */
#define VHWA_SL2_INST_DMPAC     (2U)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */


/* ========================================================================== */
/*                           Function Declarations                            */
/* ========================================================================== */

uint32_t Vhwa_calcSl2Pitch(uint32_t imgPitch);

uint32_t Vhwa_ceil(uint32_t num, uint32_t den);

uint64_t Vhwa_allocateSl2(uint32_t allocSize, uint32_t sl2Inst);

void Vhwa_FreeSl2(uint64_t addr, uint32_t sl2Inst);

void Vhwa_enableHtsIntr(CSL_vpac_intd_cfgRegs *intdRegs, uint32_t irqNum,
    uint32_t htsPipelineNum);
void Vhwa_disableHtsIntr(CSL_vpac_intd_cfgRegs *intdRegs, uint32_t irqNum,
    uint32_t htsPipelineNum);
void Vhwa_clearHtsIntr(CSL_vpac_intd_cfgRegs *intdRegs, uint32_t irqNum,
    uint32_t htsPipelineNum);

void Vhwa_enableDmpacHtsIntr(CSL_dmpac_intd_cfgRegs *intdRegs, uint32_t irqNum,
    uint32_t htsPipelineNum);
void Vhwa_disableDmpacHtsIntr(CSL_dmpac_intd_cfgRegs *intdRegs, uint32_t irqNum,
    uint32_t htsPipelineNum);
void Vhwa_clearDmpacHtsIntr(CSL_dmpac_intd_cfgRegs *intdRegs, uint32_t irqNum,
    uint32_t htsPipelineNum);

#ifdef __cplusplus
}
#endif

#endif  /* VHWA_UTILS_H_ */
