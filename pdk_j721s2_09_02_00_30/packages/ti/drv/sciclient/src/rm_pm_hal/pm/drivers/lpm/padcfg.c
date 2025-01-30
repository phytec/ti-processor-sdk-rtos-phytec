/*
 * Device Manager - Manage PADCFG Ctrl MMR during Suspend/Resume
 *
 * Copyright (C) 2021-2023, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 * its contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <lib/ioremap.h>
#include <lib/mmr_lock.h>
#include <types/errno.h>
#include "soc_ctrl_mmr.h"
#include "padcfg.h"

static u32 padcfg_data[PADCFG_SIZE];

s32 lpm_sleep_save_main_padconf(void)
{
	u32 p = PADCFG_CTRL_BASE + PADCFG_OFFSET;
	u32 i;

	for (i = 0; i < PADCFG_SIZE; i++) {
		padcfg_data[i] = readl(p);
		p += 4U;
	}

	return SUCCESS;
}

s32 lpm_resume_restore_main_padconf(void)
{
	u32 p = PADCFG_CTRL_BASE + PADCFG_OFFSET;
	u32 i;

	mmr_unlock(PADCFG_CTRL_BASE, 1);

	for (i = 0; i < PADCFG_SIZE; i++) {
		writel(padcfg_data[i], p);
		p += 4U;
	}

	return SUCCESS;
}
