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
 *  \file     safety_checkers_tifs.h
 *
 *  \brief    This file contains TIFS safety checker library interfaces and related data structures.
 *
 */

/**
 *  \defgroup SAFETY_CHECKERS_TIFS_MODULE  API for TIFS Safety Checkers
 *  \ingroup  SAFETY_CHECKERS_TIFS MODULE
 *  \section  SAFETY_CHECKERS_TIFS Overview
 *            TIFS Safety Checker (TIFS-SC) provides APIs
 *            to read the firewall configuration registers
 *            and validate the firewall configuration against the Golden Reference.
 *
 *  @{
 */
/* @} */

#ifndef SAFETY_CHECKERS_TIFS_H_
#define SAFETY_CHECKERS_TIFS_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup  SAFETY_CHECKERS_TIFS
 *
 * @defgroup SAFETY_CHECKERS_TIFS_MODULE_MACROS SAFETY_CHECKERS_TIFS Macros
 *  @{
 */

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define SAFETY_CHECKERS_TIFS_FWL_OPEN              0x902CU
#define SAFETY_CHECKERS_TIFS_FWL_CLOSE             0x902DU

#define SAFETY_CHECKERS_TIFS_FWL_BASE              0x45000000U
#define SAFETY_CHECKERS_TIFS_CONTROL_REG           0x0U
#define SAFETY_CHECKERS_TIFS_PRIV_ID0              0x4U
#define SAFETY_CHECKERS_TIFS_PRIV_ID1              0x8U
#define SAFETY_CHECKERS_TIFS_PRIV_ID2              0xCU
#define SAFETY_CHECKERS_TIFS_START_ADDRL           0x10U
#define SAFETY_CHECKERS_TIFS_START_ADDRH           0x14U
#define SAFETY_CHECKERS_TIFS_END_ADDRL             0x18U
#define SAFETY_CHECKERS_TIFS_END_ADDRH             0x1CU
#define SAFETY_CHECKERS_TIFS_MAX_REGIONS           36U

/** @} */

/**
 * @ingroup  SAFETY_CHECKERS_TIFS
 *
 * @defgroup SAFETY_CHECKERS_TIFS_MODULE_DATA_STRUCTURE SAFETY_CHECKERS TIFS Structure Definitions
 *  @{
 */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *
 * \brief   Structure for firewall configuration registers for
 *          a given firewall region
 *
 */
typedef struct
{
    uint32_t controlReg; /* Region Control Register */
    uint32_t privId0; /* Region Priv-ID 0 slot permission */
    uint32_t privId1; /* Region Priv-ID 1 slot permission */
    uint32_t privId2; /* Region Priv-ID 2 slot permission */
    uint32_t startAddrLow; /* Region Start address Low */
    uint32_t startAddrHigh; /* Region Start address High */
    uint32_t endAddrLow; /* Region End address Low */
    uint32_t endAddrHigh; /* Region End address High */
} SafetyCheckers_TifsFwlRegList;

/**
 *
 * \brief   Structure for firewall configuration register info for
 *          a given firewall id
 *
 */
typedef struct
{
    uint32_t fwlId; /* Firewall id */
    uint32_t numRegions; /* Number of regions stored in the firewall config for an id */
    uint32_t maxNumRegions; /* Maximum number of regions present in an id */
    SafetyCheckers_TifsFwlRegList fwlCfgPerRegion[SAFETY_CHECKERS_TIFS_MAX_REGIONS]; /* Firewall registers for a given region */
} SafetyCheckers_TifsFwlConfig;

/** @} */

/**
 * @ingroup  SAFETY_CHECKERS_TIFS
 *
 * @defgroup SAFETY_CHECKERS_TIFS_MODULE_API SAFETY_CHECKERS TIFS APIs
 *  @{
 */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 * \brief   API to request TIFS to open firewall.
 *
 * \return  status   SAFETY_CHECKERS_SOK : Success
 *                   SAFETY_CHECKERS_FAIL: Failure
 */
int32_t SafetyCheckers_tifsReqFwlOpen(void);

/**
 * \brief   API uses the pointer to firewall configuration fwlConfig as input and
 *          updates fwlConfig with the register dump of the firewall registers specified.
 *
 * \param   fwlConfig  [IN/OUT]    Pointer to static firewall configuration to
 *                                 be populated with register values
 *
 * \param   size       [IN]        Number of entries in the static firewall configuration
 *
 * \return  status   SAFETY_CHECKERS_SOK : Success
 *                   SAFETY_CHECKERS_FAIL: Failure
 */
int32_t SafetyCheckers_tifsGetFwlCfg(SafetyCheckers_TifsFwlConfig *fwlConfig, uint32_t size);

/**
 * \brief   API compares the fwlConfig (golden reference) with runtime firewall
 *          register values and return success or failure.
 *
 * \param   fwlConfig  [IN/OUT]    Pointer to static firewall configuration / Golden Reference to
 *                                 be verified against
 *
 * \param   size       [IN]        Number of entries in the static firewall configuration
 *
 * \return  status   SAFETY_CHECKERS_SOK : Success
 *                   SAFETY_CHECKERS_FAIL: Failure
 */
int32_t SafetyCheckers_tifsVerifyFwlCfg(const SafetyCheckers_TifsFwlConfig *fwlConfig, uint32_t size);

/**
 * \brief   API to request TIFS to close firewall
 *
 * \return  status   SAFETY_CHECKERS_SOK : Success
 *                   SAFETY_CHECKERS_FAIL: Failure
 *
 */
int32_t SafetyCheckers_tifsReqFwlClose(void);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef SAFETY_CHECKERS_TIFS_H_ */
/** @} */
/** @} */
