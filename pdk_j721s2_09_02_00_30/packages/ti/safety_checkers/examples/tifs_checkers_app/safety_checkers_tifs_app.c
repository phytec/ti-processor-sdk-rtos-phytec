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
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <safety_checkers_common.h>
#include <safety_checkers_tifs.h>
#include <tifs_checkers_fwl_config.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

SafetyCheckers_TifsFwlConfig *pFwlConfig = gSafetyCheckers_TifsFwlConfig;
uint32_t gSafetyCheckersTifsCfgSize = TIFS_CHECKER_FWL_MAX_NUM;

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

void SafetyCheckersApp_softwareDelay(void)
{
    volatile uint32_t i = 0U;
    for (i=0; i<0xFFFFF; i++);
}

void SafetyCheckersApp_tifsTest(void *args)
{
    uint32_t  status = SAFETY_CHECKERS_SOK, i=10U;

    status = SafetyCheckers_tifsReqFwlOpen();

    if (status == SAFETY_CHECKERS_SOK)
    {
        SAFETY_CHECKERS_log("\n Firewall open successful \r\n");
    }
    else
    {
        SAFETY_CHECKERS_log("\n Firewall open unsuccessful !!\r\n");
    }

    status = SafetyCheckers_tifsGetFwlCfg(pFwlConfig, gSafetyCheckersTifsCfgSize);
    if (status == SAFETY_CHECKERS_SOK)
    {
        SAFETY_CHECKERS_log("\n Get firewall configuration successful \r\n");
    }
    else
    {
        SAFETY_CHECKERS_log("\n Get firewall configuration unsuccessful !!\r\n");
    }

    /* Place to verify and save firewall configuration as Golden Reference */

    while (i > 0)
    {
        status = SafetyCheckers_tifsVerifyFwlCfg(pFwlConfig, gSafetyCheckersTifsCfgSize);

        if (status == SAFETY_CHECKERS_REG_DATA_MISMATCH)
        {
            SAFETY_CHECKERS_log("\n Firewall register mismatch with Golden Reference !!\r\n");
        }

        SafetyCheckersApp_softwareDelay();
        i--;
    }

    if (status == SAFETY_CHECKERS_SOK)
    {
        SAFETY_CHECKERS_log("\n No firewall register mismatch with Golden Reference \r\n");
    }

    status = SafetyCheckers_tifsReqFwlClose();
    if (status == SAFETY_CHECKERS_SOK)
    {
        SAFETY_CHECKERS_log("\n Firewall close successful \n");
    }
    else
    {
        SAFETY_CHECKERS_log("\n Firewall close unsuccessful !!\n");
    }
}
