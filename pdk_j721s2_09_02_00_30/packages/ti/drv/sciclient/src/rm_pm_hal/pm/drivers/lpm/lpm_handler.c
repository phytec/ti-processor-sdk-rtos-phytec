/*
 * Device Manager - LPM interface
 *
 * TISCI_MSG_PREPARE_SLEEP and TISCI_MSG_ENTER_SLEEP handler for
 * Low Power Mode implementation
 *
 * Copyright (C) 2021-2024, Texas Instruments Incorporated
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
#include <types/errno.h>
#include <string.h>
#include <tisci/lpm/tisci_lpm.h>
#include <osal/osal_clock_user.h>
#include <osal_hwi.h>
#include <osal_dm.h>
#include "gtc.h"
#include "lpm_handler.h"
#include "padcfg.h"
#include "device.h"
#include "device_pm.h"
#include "devices.h"
#include "sec_proxy.h"
#include "clk.h"
#include "soc_ctrl_mmr.h"
#include "ctrlmmr_raw.h"
#include "ctrl_mmr.h"
#include "rm_lpm.h"


/* counts of 1us delay for 10ms */
#define TIMEOUT_10MS                    10000U

#define LPM_SUSPEND_POWERMASTER                 BIT(0)
#define LPM_DEVICE_DEINIT                       BIT(1)
#define LPM_DISABLE_LPSC                        BIT(2)
#define LPM_SAVE_MAIN_PADCONFIG                 BIT(3)
#define LPM_SUSPEND_GTC                         BIT(4)
#define LPM_CLOCK_SUSPEND                       BIT(5)
#define LPM_SUSPEND_DM                          BIT(6)
#define LPM_SAVE_MMR_LOCK                       BIT(7)

extern s32 _stub_start(void);
extern u32 lpm_get_wake_up_source(void);
extern void lpm_populate_prepare_sleep_data(struct tisci_msg_prepare_sleep_req *p);
extern void lpm_clear_all_wakeup_interrupt(void);

u32 key;
volatile u32 enter_sleep_status = 0;

static void lpm_hang_abort(void)
{
	volatile u32 a = 0x12341234;

	while (a != 0U) {
	}
}

static s32 lpm_sleep_wait_for_tifs_wfi(void)
{
	u32 reg;
	u32 i = 0;
	s32 ret = -ETIMEDOUT;

	do {
		reg = readl(WKUP_CTRL_BASE + WKUP_CTRL_WFI_STATUS);
		if ((reg & SMS_CPU0_WFI_MASK) == SMS_CPU0_WFI_MASK) {
			ret = SUCCESS;
			break;
		}
		osal_delay(1);
	} while (i++ < TIMEOUT_10MS);

	return ret;
}

static s32 lpm_sleep_disable_sec_lpsc(void)
{
	/* Disable security LPSCs */
	return SUCCESS;
}

static s32 lpm_sleep_disable_misc_lpsc(void)
{
	/* Disable non-crtitical  LPSCs */
	return SUCCESS;
}

static s32 lpm_resume_enable_lpsc(void)
{
	/* enable LPSCs as needed for cores to resume */
	return SUCCESS;
}

static s32 lpm_resume_disable_DM_reset_isolation(void)
{
	/* Clear WKUP_CTRL_DS_DM_RESET.mask to stop
	* isolation of DM from MAIN domain
	*/
	writel(DS_DM_RESET_UNMASK, WKUP_CTRL_BASE + WKUP_CTRL_DS_DM_RESET);
	return SUCCESS;
}

static s32 lpm_resume_restore_RM_context(void)
{
	/* Restore IR configurations */
	s32 ret = -EFAIL;

	ret = lpm_resume_restore_ir_config();

	return ret;
}

static s32 lpm_resume_send_core_resume_message(void)
{
	/* send core resume message */
	s32 ret = 0;

	struct tisci_msg_core_resume_req req = {
		.hdr		= {
			.type	= TISCI_MSG_CORE_RESUME,
			.flags	= TISCI_MSG_FLAG_AOP,
			.host	= HOST_ID_DM2TIFS
		}
	};

	ret = sproxy_send_msg_dm2dmsc_fw(&req, sizeof(req));

	if (ret == 0) {
		struct tisci_msg_core_resume_resp resp;
		memset(&resp, 0, sizeof(resp));

		ret = sproxy_receive_msg_dm2dmsc_fw(&resp, sizeof(resp));
		if (ret == 0) {
			if ((resp.hdr.type != TISCI_MSG_CORE_RESUME) || ((resp.hdr.flags & TISCI_MSG_FLAG_ACK) != TISCI_MSG_FLAG_ACK)) {
				ret = -EINVAL;
			}
		}
	}

	return ret;
}

static s32 lpm_resume_send_enter_sleep_abort_message(void)
{
	/* send abort enter sleep message */
	s32 ret = 0;

	struct tisci_msg_abort_enter_sleep_req req = {
		.hdr		= {
			.type	= TISCI_MSG_ABORT_ENTER_SLEEP,
			.flags	= 0,
			.host	= HOST_ID_DM2TIFS
		}
	};

	ret = sproxy_send_msg_dm2dmsc_fw(&req, sizeof(req));

	return ret;
}

static s32 lpm_suspend_power_master(void)
{
	/* release reset of power master */
	struct device *dev;

	dev = device_lookup(DEV_GTC);
	soc_device_ret_disable(dev);
	soc_device_disable(dev, SFALSE);

	dev = device_lookup(POWER_MASTER);
	soc_device_ret_disable(dev);
	soc_device_disable(dev, SFALSE);

	dev = device_lookup(POWER_MASTER_CLUSTER);
	soc_device_ret_disable(dev);
	soc_device_disable(dev, SFALSE);

	return SUCCESS;
}

static s32 lpm_resume_release_reset_of_power_master(void)
{
	/* release reset of power master */
	struct device *dev;

	dev = device_lookup(POWER_MASTER_CLUSTER);
	soc_device_enable(dev);

	dev = device_lookup(POWER_MASTER);
	soc_device_enable(dev);

	dev = device_lookup(DEV_GTC);
	soc_device_enable(dev);

	return SUCCESS;
}

static s32 lpm_sleep_suspend_dm(void)
{
	/* Suspend DM OS */
	osal_dm_disable_interrupt();    /* Disable sciserver interrupt */
	osal_suspend_dm();              /* Suspend DM task scheduler */
	key = osal_hwip_disable();      /* Disable Global interrupt */
	return SUCCESS;
}

static s32 lpm_resume_dm(void)
{
	/* Resume DM OS */
	osal_dm_enable_interrupt();     /* Enable sciserver interrupts */
	osal_resume_dm();               /* Resume DM task scheduler */
	osal_hwip_restore(key);         /* Enable Global interrupts */
	return SUCCESS;
}

static s32 lpm_sleep_jump_to_dm_Stub(void)
{
	/* Jump to DM stub */
	return _stub_start();
}

static void lpm_enter_partial_io_mode(void)
{
	u32 reg = 0;
	u32 timeout = TIMEOUT_10MS;

	/* unlock wkup_ctrl_mmr region 6 */
	ctrlmmr_unlock(WKUP_CTRL_BASE, 6);

	/* set global wuen for WKUP IOs */
	reg = readl(WKUP_CTRL_BASE + WKUP_CTRL_PMCTRL_IO_0);
	reg = reg & WKUP_CTRL_PMCTRL_IO_0_WRITE_MASK;
	reg = reg | WKUP_CTRL_PMCTRL_IO_0_GLOBAL_WUEN;
	writel(reg, (WKUP_CTRL_BASE + WKUP_CTRL_PMCTRL_IO_0));

	/* set global isoin for wakeup IOs */
	reg = readl(WKUP_CTRL_BASE + WKUP_CTRL_PMCTRL_IO_0);
	reg = reg & WKUP_CTRL_PMCTRL_IO_0_WRITE_MASK;
	reg = reg | WKUP_CTRL_PMCTRL_IO_0_IO_ISO_CTRL;
	writel(reg, (WKUP_CTRL_BASE + WKUP_CTRL_PMCTRL_IO_0));

	/* wait for wu clock state to be 1 */
	while ((timeout > 0U) && (((readl(WKUP_CTRL_BASE + WKUP_CTRL_PMCTRL_IO_0)) & WKUP_CTRL_PMCTRL_IO_0_IO_ISO_STATUS) != WKUP_CTRL_PMCTRL_IO_0_IO_ISO_STATUS)) {
		--timeout;
	}
	if (timeout == 0U) {
		lpm_hang_abort();
	}

	/* Enable wakeup from CAN IO */
	writel(WKUP0_EN_CANUART_IO_DAISY_CHAIN, (WKUP_CTRL_BASE + WKUP0_EN));

	/* Ensure that PMIC EN control from SOC is selected */
	writel((WKUP0_PMCTRL_SYS_LPM_EN_PMIC | WKUP0_LPM_PMIC_OUT_EN), (WKUP_CTRL_BASE + WKUP0_PMCTRL_SYS));

	/* Program magic word */
	reg = WKUP_CANUART_MAGIC_WRD;
	writel(reg, WKUP_CTRL_BASE + WKUP_CANUART_CTRL);

	/* Set enable bit */
	reg |= WKUP_CANUART_MAGIC_WRD_LD_EN;
	writel(reg, WKUP_CTRL_BASE + WKUP_CANUART_CTRL);

	/* Wait for CAN_ONLY_IO signal to be 1 */
	while ((timeout > 0U) && ((readl(WKUP_CTRL_BASE + WKUP_CANUART_WAKE_STAT1)) != WKUP_CANUART_IO_MODE_ACTIVE)) {
		--timeout;
	}
	if (timeout == 0U) {
		lpm_hang_abort();
	}

	/* Enter Partial IO mode */
	writel((WKUP0_PMCTRL_SYS_LPM_EN_PMIC | WKUP0_LPM_PMIC_OUT_DIS), WKUP_CTRL_BASE + WKUP0_PMCTRL_SYS);

	/* If PMIC fails to suspend the system, hang abort */
	lpm_hang_abort();
}

s32 dm_prepare_sleep_handler(u32 *msg_recv)
{
	struct tisci_msg_prepare_sleep_req *req =
		(struct tisci_msg_prepare_sleep_req *) msg_recv;
	s32 ret = SUCCESS;
	u8 mode = req->mode;

	/* Only DEEP_SLEEP mode supported at the moment */
	if ((mode == TISCI_MSG_VALUE_SLEEP_MODE_DEEP_SLEEP) || (mode == TISCI_MSG_VALUE_SLEEP_MODE_MCU_ONLY)) {
		/* Parse and store the mode info and ctx address in the prepare sleep message*/
		lpm_populate_prepare_sleep_data(req);

		/*
		 * clearing all wakeup interrupts from VIM. Even if we are cleaning interrupts
		 * from VIM, if the wakeup interrupt is still active it will be able to wake
		 * the soc from LPM. This will only clear any unwanted pending wakeup interrupts
		 */
		lpm_clear_all_wakeup_interrupt();
	} else if (mode == TISCI_MSG_VALUE_SLEEP_MODE_PARTIAL_IO) {
		/* Suspend DM so that in case of failure, idle hook is not executed */
		ret = lpm_sleep_suspend_dm();

		if (ret == SUCCESS) {
			/*
			* wait for tifs to reach WFI in both the failed and successful case.
			* but update the ret value only if it was SUCCESS previously
			*/
			ret = lpm_sleep_wait_for_tifs_wfi();
		}

		if (ret == SUCCESS) {
			/* Enable CANUART IO daisy chain and enter partial io mode */
			lpm_enter_partial_io_mode();
		} else {
			lpm_hang_abort();
		}
	} else {
		ret = -EINVAL;
	}

	return ret;
}

s32 dm_enter_sleep_handler(u32 *msg_recv)
{
	struct tisci_msg_enter_sleep_req *req =
		(struct tisci_msg_enter_sleep_req *) msg_recv;
	/*
	struct tisci_msg_enter_sleep_resp *resp =
	        (struct tisci_msg_enter_sleep_resp *) msg_recv;
	*/
	s32 ret = SUCCESS;
	u8 mode = req->mode;
	u32 i;
	u32 temp_sleep_status = 0;

	enter_sleep_status = 0;

	/* Only DEEP_SLEEP mode supported at the moment */
	if ((mode != TISCI_MSG_VALUE_SLEEP_MODE_DEEP_SLEEP) && (mode != TISCI_MSG_VALUE_SLEEP_MODE_MCU_ONLY)) {
		ret = EINVAL;
	}

	/*
	 * wait for tifs to reach WFI in both the failed and successful case.
	 * but update the ret value only if it was SUCCESS previously
	 */
	if (ret == SUCCESS) {
		ret = lpm_sleep_wait_for_tifs_wfi();
	} else {
		lpm_sleep_wait_for_tifs_wfi();
	}

	/*
	 * since, once power master reaches WFI power master is only recoverable
	 * by reseting the  power master. Only update the ret value only if it was
	 * SUCCESS previously
	 */
	if (ret == SUCCESS) {
		ret = lpm_suspend_power_master();
		enter_sleep_status |= LPM_SUSPEND_POWERMASTER;
	} else {
		lpm_suspend_power_master();
		enter_sleep_status |= LPM_SUSPEND_POWERMASTER;
	}

	if (ret == SUCCESS) {
		ret = devices_deinit_flags();
		enter_sleep_status |= LPM_DEVICE_DEINIT;
	} else {
		devices_deinit_flags();
		enter_sleep_status |= LPM_DEVICE_DEINIT;
	}

	if (ret == SUCCESS) {
		ret = lpm_sleep_disable_sec_lpsc();
		enter_sleep_status |= LPM_DISABLE_LPSC;
	}

	if (ret == SUCCESS) {
		ret = lpm_sleep_disable_misc_lpsc();
		enter_sleep_status |= LPM_DISABLE_LPSC;
	}

	if (ret == SUCCESS) {
		ret = lpm_sleep_save_main_padconf();
		enter_sleep_status |= LPM_SAVE_MAIN_PADCONFIG;
	}

	if (ret == SUCCESS) {
		ret = lpm_save_mmr_lock();
		enter_sleep_status |= LPM_SAVE_MMR_LOCK;
	}

	if (ret == SUCCESS) {
		ret = lpm_sleep_suspend_gtc();
		enter_sleep_status |= LPM_SUSPEND_GTC;
	}

	if (ret == SUCCESS) {
		ret = clks_suspend();
		enter_sleep_status |= LPM_CLOCK_SUSPEND;
	}

	if (ret == SUCCESS) {
		ret = lpm_sleep_suspend_dm();
		enter_sleep_status |= LPM_SUSPEND_DM;
	}

	if (ret == SUCCESS) {
		ret = lpm_sleep_jump_to_dm_Stub();
	}

	/* if there is any failure inform TIFS using abort message */
	if (ret != SUCCESS) {
		if (lpm_resume_send_enter_sleep_abort_message() != SUCCESS) {
			lpm_hang_abort();
		}
	}

	temp_sleep_status = enter_sleep_status;
	if ((ret == SUCCESS) || ((temp_sleep_status & LPM_DISABLE_LPSC) == LPM_DISABLE_LPSC)) {
		if (lpm_resume_enable_lpsc() != SUCCESS) {
			lpm_hang_abort();
		}
	}

	if (ret == SUCCESS) {
		if (lpm_resume_disable_DM_reset_isolation() != SUCCESS) {
			lpm_hang_abort();
		}
	}

	if (ret == SUCCESS) {
		if (lpm_resume_restore_RM_context() != SUCCESS) {
			lpm_hang_abort();
		}
	}

	if ((ret == SUCCESS) || ((temp_sleep_status & LPM_SUSPEND_GTC) == LPM_SUSPEND_GTC)) {
		if (lpm_resume_gtc() != SUCCESS) {
			lpm_hang_abort();
		}
	}

	if ((ret == SUCCESS) || ((temp_sleep_status & LPM_SAVE_MMR_LOCK) == LPM_SAVE_MMR_LOCK)) {
		if (lpm_restore_mmr_lock() != SUCCESS) {
			lpm_hang_abort();
		}
	}

	if ((ret == SUCCESS) || ((temp_sleep_status & LPM_CLOCK_SUSPEND) == LPM_CLOCK_SUSPEND)) {
		if (clks_resume() != SUCCESS) {
			lpm_hang_abort();
		}
	}

	if ((ret == SUCCESS) || ((temp_sleep_status & LPM_SUSPEND_POWERMASTER) == LPM_SUSPEND_POWERMASTER)) {
		if (lpm_resume_send_core_resume_message() != SUCCESS) {
			lpm_hang_abort();
		}
	}

	if ((ret == SUCCESS) || ((temp_sleep_status & LPM_SUSPEND_DM) == LPM_SUSPEND_DM)) {
		if (lpm_resume_dm() != SUCCESS) {
			lpm_hang_abort();
		}
	}

	if ((ret == SUCCESS) || ((temp_sleep_status & LPM_DEVICE_DEINIT) == LPM_DEVICE_DEINIT)) {
		if (devices_init() != SUCCESS) {
			lpm_hang_abort();
		}
	}

	for (i = 0; i < TIMEOUT_10MS; i++) {
		osal_delay(1);
	}

	if ((ret == SUCCESS) || ((temp_sleep_status & LPM_SUSPEND_POWERMASTER) == LPM_SUSPEND_POWERMASTER)) {
		if (lpm_resume_release_reset_of_power_master() != SUCCESS) {
			lpm_hang_abort();
		}
	}

	if (ret == SUCCESS) {
		if (lpm_resume_restore_main_padconf() != SUCCESS) {
			lpm_hang_abort();
		}
	}

	return ret;
}

s32 dm_lpm_wake_reason_handler(u32 *msg_recv)
{
	struct tisci_msg_lpm_wake_reason_resp *resp =
		(struct tisci_msg_lpm_wake_reason_resp *) msg_recv;
	s32 ret = SUCCESS;

	resp->hdr.flags = 0U;
	resp->wake_source = lpm_get_wake_up_source();
	/* TODO: Add support for time stamp */
	resp->wake_timestamp = 0;

	return ret;
}

s32 dm_set_io_isolation_handler(u32 *msg_recv)
{
	struct tisci_msg_set_io_isolation_req *req =
		(struct tisci_msg_set_io_isolation_req *) msg_recv;
	s32 ret = EFAIL;
	u32 reg;
	u32 i = 0;

	/* unlock partion 6 of wakeup ctrl mmr */
	ctrlmmr_unlock(WKUP_CTRL_BASE, 6);
	if (req->state == TISCI_MSG_VALUE_IO_ENABLE) {
		writel(WKUP_CTRL_DEEPSLEEP_CTRL_ENABLE_IO, (WKUP_CTRL_BASE + WKUP_CTRL_DEEPSLEEP_CTRL));
		writel(WKUP_CTRL_PMCTRL_IO_GLB_ENABLE_IO, (WKUP_CTRL_BASE + WKUP_CTRL_PMCTRL_IO_GLB));

		/* set global wuen */
		reg = readl(WKUP_CTRL_BASE + WKUP_CTRL_PMCTRL_IO_0);
		reg = reg & WKUP_CTRL_PMCTRL_IO_0_WRITE_MASK;
		reg = reg | WKUP_CTRL_PMCTRL_IO_0_GLOBAL_WUEN;
		writel(reg, WKUP_CTRL_BASE + WKUP_CTRL_PMCTRL_IO_0);

		reg = readl(WKUP_CTRL_BASE + WKUP_CTRL_PMCTRL_IO_1);
		reg = reg & WKUP_CTRL_PMCTRL_IO_0_WRITE_MASK;
		reg = reg | WKUP_CTRL_PMCTRL_IO_0_GLOBAL_WUEN;
		writel(reg, WKUP_CTRL_BASE + WKUP_CTRL_PMCTRL_IO_1);

		/* set global isoin */
		reg = readl(WKUP_CTRL_BASE + WKUP_CTRL_PMCTRL_IO_0);
		reg = reg & WKUP_CTRL_PMCTRL_IO_0_WRITE_MASK;
		reg = reg | WKUP_CTRL_PMCTRL_IO_0_IO_ISO_CTRL;
		writel(reg, WKUP_CTRL_BASE + WKUP_CTRL_PMCTRL_IO_0);

		reg = readl(WKUP_CTRL_BASE + WKUP_CTRL_PMCTRL_IO_1);
		reg = reg & WKUP_CTRL_PMCTRL_IO_0_WRITE_MASK;
		reg = reg | WKUP_CTRL_PMCTRL_IO_0_IO_ISO_CTRL;
		writel(reg, WKUP_CTRL_BASE + WKUP_CTRL_PMCTRL_IO_1);

		/* wait for wu clock state to be 1*/
		i = 0;
		do {
			ret = ETIMEDOUT;
			reg = readl(WKUP_CTRL_BASE + WKUP_CTRL_PMCTRL_IO_0);
			if ((reg & WKUP_CTRL_PMCTRL_IO_0_IO_ISO_STATUS) == WKUP_CTRL_PMCTRL_IO_0_IO_ISO_STATUS) {
				ret = SUCCESS;
				break;
			}
			osal_delay(1);
		} while (i++ < TIMEOUT_10MS);
		i = 0;
		do {
			ret = ETIMEDOUT;
			reg = readl(WKUP_CTRL_BASE + WKUP_CTRL_PMCTRL_IO_1);
			if ((reg & WKUP_CTRL_PMCTRL_IO_0_IO_ISO_STATUS) == WKUP_CTRL_PMCTRL_IO_0_IO_ISO_STATUS) {
				ret = SUCCESS;
				break;
			}
			osal_delay(1);
		} while (i++ < TIMEOUT_10MS);
	} else if (req->state == TISCI_MSG_VALUE_IO_DISABLE) {
		/* clear global wuen */
		reg = readl(WKUP_CTRL_BASE + WKUP_CTRL_PMCTRL_IO_0);
		reg = reg & WKUP_CTRL_PMCTRL_IO_0_WRITE_MASK;
		reg = reg & (~WKUP_CTRL_PMCTRL_IO_0_GLOBAL_WUEN);
		writel(reg, WKUP_CTRL_BASE + WKUP_CTRL_PMCTRL_IO_0);

		reg = readl(WKUP_CTRL_BASE + WKUP_CTRL_PMCTRL_IO_1);
		reg = reg & WKUP_CTRL_PMCTRL_IO_0_WRITE_MASK;
		reg = reg & (~WKUP_CTRL_PMCTRL_IO_0_GLOBAL_WUEN);
		writel(reg, WKUP_CTRL_BASE + WKUP_CTRL_PMCTRL_IO_1);

		/* clear global isoin */
		reg = readl(WKUP_CTRL_BASE + WKUP_CTRL_PMCTRL_IO_0);
		reg = reg & WKUP_CTRL_PMCTRL_IO_0_WRITE_MASK;
		reg = reg & (~WKUP_CTRL_PMCTRL_IO_0_IO_ISO_CTRL);
		writel(reg, WKUP_CTRL_BASE + WKUP_CTRL_PMCTRL_IO_0);

		reg = readl(WKUP_CTRL_BASE + WKUP_CTRL_PMCTRL_IO_1);
		reg = reg & WKUP_CTRL_PMCTRL_IO_0_WRITE_MASK;
		reg = reg & (~WKUP_CTRL_PMCTRL_IO_0_IO_ISO_CTRL);
		writel(reg, WKUP_CTRL_BASE + WKUP_CTRL_PMCTRL_IO_1);

		writel(WKUP_CTRL_DEEPSLEEP_CTRL_DISABLE_IO, (WKUP_CTRL_BASE + WKUP_CTRL_DEEPSLEEP_CTRL));
		writel(WKUP_CTRL_PMCTRL_IO_GLB_DISABLE_IO, (WKUP_CTRL_BASE + WKUP_CTRL_PMCTRL_IO_GLB));

		ret = SUCCESS;
	} else {
		/* Do Nothing */
	}
	return ret;
}
