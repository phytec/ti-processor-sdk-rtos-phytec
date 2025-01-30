/*
*
* Copyright (c) 2020 Texas Instruments Incorporated
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
 *  \file     mcu_timer_freq.h
 *
 *  \brief    This header provides common timer information used by MCUSW app and Main Domain apps.
 */

#ifndef __MCU_TIMER_FREQ__
#define __MCU_TIMER_FREQ__

#include <ti/csl/arch/csl_arch.h>
#include <ti/csl/soc.h>

struct mcu_timer {
	uint32_t tidr;		/* 0x00 r */
	uint8_t  res1[0xc];
	uint32_t tiocp_cfg;		/* 0x10 rw */
	uint8_t  res2[0xc];
	uint32_t teoir;		/* 0x20 rw */
	uint32_t tisr_raw;		/* 0x24 r */
	uint32_t tisr;		/* 0x28 rw */
	uint32_t tier;		/* 0x2c rw */
	uint32_t ticr;		/* 0x30 rw */
	uint32_t twer;		/* 0x34 rw */
	uint32_t tclr;		/* 0x38 rw */
	uint32_t tcrr;		/* 0x3c rw */
	uint32_t tldr;		/* 0x40 rw */
	uint32_t ttgr;		/* 0x44 rw */
	uint32_t twps;		/* 0x48 r */
	uint32_t tmar;		/* 0x4c rw */
	uint32_t tcar1;		/* 0x50 r */
	uint32_t tsicr;		/* 0x54 rw */
	uint32_t tcar2;		/* 0x58 r */
	uint32_t tpir;		/* 0x5c rw */
	uint32_t tnir;		/* 0x60 rw */
	uint32_t tcvr;		/* 0x64 rw */
	uint32_t tocr;		/* 0x68 rw */
	uint32_t towr; 		/* 0x6c rw */
};

#define TCLR_ST			(0x1 << 0)
#define TCLR_AR			(0x1 << 1)
#define TCLR_PRE		(0x1 << 5)

#define TSICR_POST		(0x1 << 2)

#define MCU_TIMER0_BASE CSL_MCU_TIMER0_CFG_BASE
#define MCU_TIMER1_BASE CSL_MCU_TIMER1_CFG_BASE
#define MCU_TIMER2_BASE CSL_MCU_TIMER2_CFG_BASE
#define MCU_TIMER3_BASE CSL_MCU_TIMER3_CFG_BASE

#define MCU_PROFILING_TIMER_BASE MCU_TIMER2_BASE

/* Clocks */
#define CLK_12M_RC_FREQ           (12500000ULL)
#define CLK_200M_RC_DEFAULT_FREQ (200000000ULL)  /* Timer 2 clock frequency set by default to 200MHz */

#define MCU_TIMER_PTV       2
#define MCU_TIMER_CLOCK     (CLK_200M_RC_DEFAULT_FREQ / (2 << MCU_TIMER_PTV))

#endif /* __MCU_TIMER_FREQ__ */
