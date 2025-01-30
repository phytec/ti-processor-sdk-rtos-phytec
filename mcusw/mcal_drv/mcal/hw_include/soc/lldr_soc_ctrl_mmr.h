/*
 *  Copyright (c) 2024-2020 Texas Instruments Incorporated
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

#ifndef LLDR_SOC_CTRL_MMR_TOP_H
#define LLDR_SOC_CTRL_MMR_TOP_H

#if defined (SOC_J721E)
#include <hw_include/soc/j721e/src/lldr_soc_ctrl_mmr.h>
#endif

#if defined (SOC_J7200)
#include <hw_include/soc/j7200/src/lldr_soc_ctrl_mmr.h>
#endif

#if defined (SOC_J721S2)
#include <hw_include/soc/j721s2/src/lldr_soc_ctrl_mmr.h>
#endif

#if defined (SOC_AM62X)
#include <hw_include/soc/am62x/src/lldr_soc_ctrl_mmr.h>
#endif

#if defined (SOC_AM62AX)
#include <hw_include/soc/am62ax/src/lldr_soc_ctrl_mmr.h>
#endif

#if defined (SOC_AM62PX)
#include <hw_include/soc/am62px/src/lldr_soc_ctrl_mmr.h>
#endif

#endif /* LLDR_SOC_CTRL_MMR_TOP_H */
