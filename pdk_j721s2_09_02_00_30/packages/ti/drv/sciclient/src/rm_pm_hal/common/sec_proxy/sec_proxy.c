/*
 * System Firmware
 *
 * Secure Proxy driver for Message Transfer
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

#include <lib/io.h>
#include <lib/bitops.h>
#include <types/errno.h>
#include <types/sbool.h>
#include <baseaddress.h>
#include <string.h>
#include "sec_proxy.h"

#define SPROXY_THREAD_OFFSET(tid) (0x1000U * (tid))

#define SPROXY_THREAD_DATA_ADDRESS(_target_base, tid)	\
	(_target_base + SPROXY_THREAD_OFFSET(tid) +	\
	 4U)
#define SPROXY_THREAD_DATA_ADDRESS_END(_target_base, tid) \
	(SPROXY_THREAD_DATA_ADDRESS(_target_base, tid) + (14U * 4U))

#define SPROXY_THREAD_STATUS(_rt_base, tid) \
	(_rt_base + SPROXY_THREAD_OFFSET(tid))

#define SPROXY_STATUS_ERR       BIT(31U)
#define SPROXY_STATUS_CNT_MASK  0xFFU

#define SPROXY_SEND             0
#define SPROXY_GET              1

/* retry for 10ms */
#define RETRY_CNT_10ms          (1000U * 10U)

static void asm_func(void)
{
	asm ("");
}

static void delay(void)
{
	/* This while-loop takes 5 instructions. Assume R5 runs @400MHz */
	/* FIXME will -O2 comptimize out the code? */
	unsigned long x = 400 / 5;

	while (x-- != 0U) {
		asm_func();
	}
}

static s32 trans_message(u32 target_base, u32 rt_base, sbool is_rx, u8 thread_id, void *msg, size_t len, sbool is_secure)
{
	u32 start_addr = SPROXY_THREAD_DATA_ADDRESS(target_base, thread_id);
	u32 end_addr = SPROXY_THREAD_DATA_ADDRESS_END(target_base, thread_id);
	u32 *raw = (u32 *) msg;
	u32 status;
	u32 word;
	u32 mask;
	u16 i;
	s32 ret = SUCCESS;

	if ((is_secure == SFALSE) && ((start_addr + len) > end_addr)) {
		ret = -EINVAL;
	}

	if ((ret == SUCCESS) && (is_secure == STRUE) && ((start_addr + len + 4U) > end_addr)) {
		ret = -EINVAL;
	}

	if (ret == SUCCESS) {
		for (i = 0; i < RETRY_CNT_10ms; i++) {
			status = readl(SPROXY_THREAD_STATUS(rt_base, thread_id));
			if ((status & SPROXY_STATUS_ERR) != 0U) {
				ret = -EFAIL;
			}
			if ((status & SPROXY_STATUS_CNT_MASK) != 0U) {
				break;
			}
			if (i < (RETRY_CNT_10ms - 1U)) {
				delay();
			} else {
				ret = -ETIMEDOUT;
			}
		}
	}

	if (ret == SUCCESS) {
		/*
		* HACK: We will need to deal with sec hdr someday...
		* For now, just skip that portion
		*/
		if (is_secure == STRUE) {
			if (is_rx == SFALSE) {
				writel(0U, start_addr);
			}
			start_addr += 4U;
		}

		for (i = 0; i < (len / 4U); i++) {
			if (is_rx == STRUE) {
				*raw = readl(start_addr);
				raw += 1U;
			} else {
				writel(*raw, start_addr);
				raw += 1U;
			}
			start_addr += 4U;
		}

		if ((len % 4U) != 0U) {
			if (is_rx == SFALSE) {
				mask = ~0UL >> ((4U - (len % 4U)) * 8U);
				word = (*raw) & mask;
				writel(word, start_addr);
			} else {
				word = readl(start_addr);
				/* let memcpy deal with the alignment stuff */
				memcpy(raw, &word, len % 4U);
			}
		}

		/* flush out the transfer */
		if (is_rx == STRUE) {
			(void) readl(end_addr);
		} else {
			writel(0x0, end_addr);
		}
	}
	return ret;
}

s32 sproxy_send_msg_r5_to_tifs_fw(void *msg, size_t len)
{
	return trans_message(TIFS_SEC_PROXY_TARGET_ADDRESS, TIFS_SEC_PROXY_RT_ADDRESS, SPROXY_SEND, R5_TO_TIFS_SEC_PROXY_MSG_TX_TID, msg, len, STRUE);
}

s32 sproxy_receive_msg_r5_to_tifs_fw(void *msg, size_t len)
{
	return trans_message(TIFS_SEC_PROXY_TARGET_ADDRESS, TIFS_SEC_PROXY_RT_ADDRESS, SPROXY_GET, R5_TO_TIFS_SEC_PROXY_MSG_RX_TID, msg, len, STRUE);
}
