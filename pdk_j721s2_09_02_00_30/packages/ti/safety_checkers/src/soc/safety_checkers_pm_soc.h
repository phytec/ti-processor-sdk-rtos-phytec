/*
 *  Copyright (C) 2024 Texas Instruments Incorporated
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

 /**
 *  \defgroup SAFETY_CHECKERS_PM PM Safety Checkers Library
 *  \ingroup  PM SAFETY CHECKERS
 */

/**
 *  \file     safety_checkers_pm_soc.h
 *
 *  \brief    This file contains data structures for PM safety checker module
 *
 */

#ifndef SAFETY_CHECKERS_PM_SOC_H_
#define SAFETY_CHECKERS_PM_SOC_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "cslr_soc.h"
#include "safety_checkers_pm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup SAFETY_CHECKERS_PM
 *
 * @defgroup SAFETY_CHECKERS_PM_MACROS  SAFETY_CHECKERS_PM PM safety checkers macro definition
 *  @{
 */

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief WKUP PSC base address */
#define SAFETY_CHECKERS_PM_WKUP_PSC_BASE_ADDRESS 		         (CSL_WKUP_PSC0_BASE)

/** \brief PSC Power Domain(PD) STAT register offset */
#define SAFETY_CHECKERS_PM_PSC_PD_STAT_OFFSET                    (0x200U)
/** \brief PSC Module Domain(MD) STAT register offset */
#define SAFETY_CHECKERS_PM_PSC_MD_STAT_OFFSET                    (0x800U)

/** \brief Each PLL base addresses */
#define SAFETY_CHECKERS_PM_PLL_BASE_ADDRESS(i)                   (SAFETY_CHECKERS_PM_PLL_CFG_BASE_ADDRESS + (0x1000U * (uint32_t)i))
#define SAFETY_CHECKERS_PM_MCU_PLL_BASE_ADDRESS(i)               (SAFETY_CHECKERS_PM_MCU_PLL_CFG_BASE_ADDRESS + (0x1000U * (uint32_t)i))

/** \brief KICK lock values */
#define SAFETY_CHECKERS_PM_KICK_LOCK                             (0x00000000U)
#define SAFETY_CHECKERS_PM_LOCK_KEY0_OFFSET                      (0x10U)
#define SAFETY_CHECKERS_PM_LOCK_KEY1_OFFSET                      (0x14U)

/** @} */

/**
 * @ingroup SAFETY_CHECKERS_PM
 *
 * @defgroup SAFETY_CHECKERS_PM_GLOBAL_VAR  SAFETY_CHECKERS_PM PM safety checkers global variables declarations
 *  @{
 */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/**
 *
 * \brief     This defines the array holding register offset values for the each PLL's
 *
 */
static uint32_t gSafetyCheckers_PmPllRegOffset0 [] =
{0x00U, 0x08U, 0x20U, 0x24U, 0x30U, 0x34U, 0x38U,
 0x40U, 0x44U, 0x80U, 0x84U, 0x88U, 0x8cU, 0x90U,
 0x94U, 0x98U, 0x9cU, 0xA0U, 0xA4U};

static uint32_t gSafetyCheckers_PmPllRegOffset1 [] =
{0x00U, 0x08U, 0x20U, 0x24U, 0x30U, 0x34U, 0x38U,
 0x40U, 0x44U, 0x60U, 0x80U, 0x84U};

static uint32_t gSafetyCheckers_PmPllRegOffset2 [] =
{0x00U, 0x08U, 0x20U, 0x24U, 0x38U, 0x50U, 0x60U,
 0x64U, 0x80U, 0x84U};

static uint32_t gSafetyCheckers_PmPllRegOffset3 [] =
{0x00U, 0x08U, 0x20U, 0x24U, 0x30U, 0x34U, 0x38U,
 0x40U, 0x44U, 0x60U, 0x64U, 0x80U, 0x84U, 0x88U,
 0x8CU, 0x90U, 0x94U, 0x98U, 0x9CU, 0xA0U, 0xA4U,
 0xA8U};

static uint32_t gSafetyCheckers_PmPllRegOffset4 [] =
{0x00U, 0x08U, 0x20U, 0x24U, 0x30U, 0x34U, 0x38U,
 0x40U, 0x44U, 0x80U, 0x84U, 0x88U, 0x8cU, 0x90U,
 0x94U, 0x9cU, 0xA0U};

static uint32_t gSafetyCheckers_PmPllRegOffset5 [] =
{0x00U, 0x08U, 0x20U, 0x24U, 0x30U, 0x34U, 0x38U,
 0x40U, 0x44U, 0x80U, 0x84U, 0x88U, 0x8cU, 0x90U,
 0x98U, 0x9cU};

/** @} */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                       Static Function Definitions                          */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef SAFETY_CHECKERS_PM_SOC_H_ */
