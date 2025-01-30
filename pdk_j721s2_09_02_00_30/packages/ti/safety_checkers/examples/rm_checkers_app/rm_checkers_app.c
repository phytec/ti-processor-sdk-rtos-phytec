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
 *  \file     rm_checkers_app.c
 *
 *  \brief    This file contains RM safety checkers app code.
 *
 */

/*===========================================================================*/
/*                         Include files                                     */
/*===========================================================================*/

#include <stdint.h>
#include <safety_checkers_common.h>
#include <safety_checkers_soc.h>
#include <safety_checkers_rm.h>

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

void SafetyCheckersApp_rmRun(void *arg0);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                  Internal/Private Function Declarations                    */
/* ========================================================================== */

static int32_t SafetyCheckersApp_rmregVerify();

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

void SafetyCheckersApp_rmRun(void *arg0)
{
	uint32_t    status = SAFETY_CHECKERS_SOK;
   	if(status == SAFETY_CHECKERS_SOK)
    {
        status = SafetyCheckersApp_rmregVerify();
   	}
#if defined LDRA_DYN_COVERAGE_EXIT
    SAFETY_CHECKERS_log("\n LDRA ENTRY... \r\n");
    upload_execution_history();
    SAFETY_CHECKERS_log("\n LDRA EXIT... \r\n");
#endif

	return;
}

/* ========================================================================== */
/*                       Internal Function Definitions                        */
/* ========================================================================== */

static int32_t SafetyCheckersApp_rmregVerify()
{
    int32_t      status = SAFETY_CHECKERS_SOK;
    uintptr_t    rmRegisterData[SAFETY_CHECKERS_RM_REGDUMP_SIZE];
    
    status = SafetyCheckers_rmGetRegCfg(rmRegisterData, SAFETY_CHECKERS_RM_REGDUMP_SIZE);
    if(status == SAFETY_CHECKERS_SOK)
    {
        status = SafetyCheckers_rmVerifyRegCfg(rmRegisterData, SAFETY_CHECKERS_RM_REGDUMP_SIZE);
		if (status == SAFETY_CHECKERS_REG_DATA_MISMATCH)
		{
			SAFETY_CHECKERS_log("\nRM register blob test failed\r\n\n");
			status = SAFETY_CHECKERS_SOK;
		}
		else
		{
			SAFETY_CHECKERS_log("\nRM register blob test match !!!\r\n\n");
			status = SAFETY_CHECKERS_SOK;
		}
    }
    return (status);
}
