/*
*
* Copyright (C) 2024 Texas Instruments Incorporated
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
 *  \file     bist.c
 *
 *  \brief    This file implements BIST task function
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "Std_Types.h"
#include <stdio.h>

#include "lbist_utils.h"
#include "pbist_utils.h"
#include "bist_core_defs.h"

#include "app_utils.h"
#include "mcu_timer_multicore.h"

#include <ti/osal/osal.h>
#include <ti/osal/TaskP.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* This flag adds more verbose prints */
//#define DEBUG

/* This flags enables gathering timing information for BIST stages */
#define GATHER_BIST_STAGE_DETAILS

/* For PBIST and LBIST stage tests, status is saved in bitfields.
 * For these bitfields, status is represented by below defines */
#define TEST_PASS_BIT         1
#define TEST_FAIL_NOT_RUN_BIT 0

/* ========================================================================== */
/*              Internal Function Declarations                                */
/* ========================================================================== */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* ========================================================================== */
/*                            External Variables                              */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

static const char *testStatusPrint(int32_t status)
{
    char *name;

    switch(status)
    {
        case (TEST_PASS_BIT):
            name="PASS";
            break;
        case (TEST_FAIL_NOT_RUN_BIT):
            name="FAIL or NOT RUN";
            break;
        default:
            name="INVALID STATUS";
            break;
    }

    return name;
}


void bist_TaskFxn(SemaphoreP_Handle bistSemHandle)
{
    int32_t testResult = 0;
    int     i, j;
    int    *pbist_array;
    int    *lbist_array;
    /* Bitmap representing status of each PBIST test in each boot stage */
    int     pbist_stage_status[NUM_BOOT_STAGES];
    /* Bitmap representing status of each PBIST test in each boot stage */
    int     pbist_stage_neg_status[NUM_BOOT_STAGES];
    /* Bitmap representing status of each LBIST test in each boot stage */
    int     lbist_stage_status[NUM_BOOT_STAGES];
    uint64  time_pre_boot_stage_start;
    uint64  time_stage_finish[NUM_BOOT_STAGES];
#if defined(GATHER_BIST_STAGE_DETAILS)
    uint64  time_lbist_start[NUM_BOOT_STAGES];
    uint64  time_pbist_start[NUM_BOOT_STAGES];
#endif

    /* Initialize boot stage status bitmaps to "not run/fail" */
    for (i = 0; i < NUM_BOOT_STAGES; i++)
    {
        pbist_stage_status[i] = 0x0;
        pbist_stage_neg_status[i] = 0x0;
        lbist_stage_status[i] = 0x0;
    }

    testResult = PBIST_commonInit();

    if (testResult != 0)
    {
        AppUtils_Printf(MSG_NORMAL,
                        "PBIST_commonInit ...FAILED \n");
    }
    else
    {
        time_pre_boot_stage_start = get_usec_timestamp();

        /* Run pre-boot-stage PBIST's.  The definitions of the pre-boot-stage PBIST's
         * are found in soc/<SOC Device>/bist_core_defs.c.*/
        for (i = 0; i < num_pbists_pre_boot; i++)
        {
        if(pbist_pre_boot_stage[i]==PBIST_INSTANCE_MAIN_INFRA){
        continue;}
            /* Run test on selected instance */
            testResult = PBIST_runTest(pbist_pre_boot_stage[i], true);
            /* Convert signed return value (with -1 = failure and 0 = pass) to become
             * 0 = failure and 1 = pass */
            pbist_pre_boot_stage_neg_status[i] = testResult + 1;
            if ( testResult != 0)
            {
                AppUtils_Printf(MSG_NORMAL,"PBIST negative test failed for %d\n",
                                pbist_pre_boot_stage[i]);
                break;
            }
        }

	for (i = 0; i < num_pbists_pre_boot; i++)
        {
            /* Run test on selected instance */
            if(pbist_pre_boot_stage[i]==PBIST_INSTANCE_MAIN_INFRA){
        continue;}
            testResult = PBIST_runTest(pbist_pre_boot_stage[i], false);
            /* Convert signed return value (with -1 = failure and 0 = pass) to become
             * 0 = failure and 1 = pass */
            pbist_pre_boot_stage_status[i] = testResult + 1;
            if ( testResult != 0)
            {
                AppUtils_Printf(MSG_NORMAL,
                                "PBIST functional test failed for %d\n",
                                pbist_pre_boot_stage[i]);
                break;
            }
        }

        /* Run pre-boot-stage LBIST's. The definitions of the pre-boot-stage LBIST's
         * are found in soc/<SOC Device>/bist_core_defs.c. */
        for (i = 0; i < num_lbists_pre_boot; i++)
        {
            /* Run test on selected instance */
            testResult = LBIST_runTest(lbist_pre_boot_stage[i]);
            lbist_pre_boot_stage_status[i] = testResult;
            if ((testResult == -1) ||
                (testResult == LBIST_POST_COMPLETED_FAILURE) ||
                (testResult == LBIST_POST_ATTEMPTED_TIMEOUT))
            {
                AppUtils_Printf(MSG_NORMAL,
                                "LBIST functional test failed for %d\n",
                                lbist_pre_boot_stage[i]);
                break;
            }
        }

        /* After running PBIST potentially on MSMC, re-initialize the CLEC */
        testResult = PBIST_commonInit();

        if (testResult != 0)
        {
            AppUtils_Printf(MSG_NORMAL,
                            "PBIST_commonInit after pre-boot stage ...FAILED \n");
        }

        /* Run LBIST and PBIST before each boot stage.  The definitions of the LBIST
         * and PBIST sections for each stage are defined in
         * soc/<SOC Device>/bist_core_defs.c. */
        for (j = 0; j < NUM_BOOT_STAGES; j++)
        {
            pbist_array = pbist_array_stage[j];
            lbist_array = lbist_array_stage[j];
#if defined(GATHER_BIST_STAGE_DETAILS)
            time_pbist_start[j] = get_usec_timestamp();
#endif
            for (i = 0; i < num_pbists_per_boot_stage[j]; i++)
            {
                PBIST_clecConfig(pbist_array[i]);
                /* Run test on selected instance */
                testResult = PBIST_runTest(pbist_array[i], true);

                /* Convert signed return value (with -1 = failure and 0 = pass) to become
                 * a single bit as part of bitfield with 0 = failure and 1 = pass */
                pbist_stage_neg_status[j] |= ((uint32_t)(testResult + 1) << i);
                if ( testResult != 0)
                {
                    AppUtils_Printf(MSG_NORMAL,"PBIST negative test failed for %d\n",
                                    pbist_array[i]);
                    break;
                }
            }
            for (i = 0; i < num_pbists_per_boot_stage[j]; i++)
            {
                testResult = PBIST_runTest(pbist_array[i], false);
                pbist_stage_status[j] |= ((uint32_t)(testResult + 1) << i);
                if ( testResult != 0)
                {
                    AppUtils_Printf(MSG_NORMAL,
                                    "PBIST functional test failed for %d\n",
                                    pbist_array[i]);
                    break;
                }
            }
#if defined(DEBUG)
            AppUtils_Printf(MSG_NORMAL, "Ran PBIST for Stage %d\n", j);
#endif
#if defined(GATHER_BIST_STAGE_DETAILS)
            time_lbist_start[j] = get_usec_timestamp();
#endif
            for (i = 0; i < num_lbists_per_boot_stage[j]; i++)
            {
                testResult = LBIST_runTest(lbist_array[i]);

                /* Convert signed return value (with -1 = failure and 0 = pass) to become
                 * a single bit as part of bitfield with 0 = failure and 1 = pass */
                lbist_stage_status[j] |= ((uint32_t)(testResult + 1) << i);
                if (testResult != 0)
                {
                    AppUtils_Printf(MSG_NORMAL,
                                    "LBIST functional test failed for %d\n",
                                    lbist_array[i]);
                    break;
                }
            }
#if defined(DEBUG)
            AppUtils_Printf(MSG_NORMAL, "Ran LBIST for Stage %d\n", j);
#endif
            /* Signal Boot Task that BIST for the stage is completed */
            SemaphoreP_post(bistSemHandle);

            time_stage_finish[j] = get_usec_timestamp();
        }
    }

    if (testResult == 0)
    {
        /* Delay print out of boot log to avoid prints by other tasks */
        TaskP_sleep(2000);

        AppUtils_Printf(MSG_NORMAL,
                        "BIST: Pre-boot stage - Started at %d usec\n",
                        time_pre_boot_stage_start);
        for (i = 0; i < num_pbists_pre_boot; i++)
        {
            AppUtils_Printf(MSG_NORMAL,
                            "BIST: Pre-boot Stage - Ran negative PBIST ID - %s, Result = %s\n",
                            pbistName(pbist_pre_boot_stage[i]),
                            testStatusPrint(pbist_pre_boot_stage_neg_status[i]));
        }
        AppUtils_Printf(MSG_NORMAL,
                        "Pre-boot stage - Ran %d negative PBIST total sections\n",
                        num_pbists_pre_boot);
        for (i = 0; i < num_pbists_pre_boot; i++)
        {
            AppUtils_Printf(MSG_NORMAL,
                            "BIST: Pre-boot Stage - Ran PBIST ID - %s, Result = %s\n",
                            pbistName(pbist_pre_boot_stage[i]),
                            testStatusPrint(pbist_pre_boot_stage_status[i]));
        }
        AppUtils_Printf(MSG_NORMAL,
                        "Pre-boot stage - Ran %d PBIST total sections\n",
                        num_pbists_pre_boot);
        for (i = 0; i < num_lbists_pre_boot; i++)
        {
            AppUtils_Printf(MSG_NORMAL,
                            "BIST: Pre-boot Stage - Ran LBIST ID - %s, Result = %s\n",
                            lbistName(lbist_pre_boot_stage[i]),
                            LBIST_hwPostStatusPrint(lbist_pre_boot_stage_status[i]));
        }
        AppUtils_Printf(MSG_NORMAL,
                        "Pre-boot stage - Ran %d LBIST total sections\n",
                        num_lbists_pre_boot);

        for (j = 0; j < NUM_BOOT_STAGES; j++)
        {
            pbist_array = pbist_array_stage[j];
            lbist_array = lbist_array_stage[j];
#if defined(GATHER_BIST_STAGE_DETAILS)
            AppUtils_Printf(MSG_NORMAL,
                            "BIST: Stage %d - Started running PBIST sections at %d usec\n",
                            j, (uint32)time_pbist_start[j]);
            for (i = 0; i < num_pbists_per_boot_stage[j]; i++)
            {
                AppUtils_Printf(MSG_NORMAL,
                                "BIST: Stage %d - Ran negative PBIST ID - %s, Result = %s\n",
                                j, pbistName(pbist_array[i]),
                                testStatusPrint((pbist_stage_neg_status[j] >> i) & 0x1));
            }
            AppUtils_Printf(MSG_NORMAL,
                           "BIST: Stage %d - Ran %d negative PBIST total sections\n",
                            j, (uint32)num_pbists_per_boot_stage[j]);
            for (i = 0; i < num_pbists_per_boot_stage[j]; i++)
            {
                AppUtils_Printf(MSG_NORMAL,
                                "BIST: Stage %d - Ran PBIST ID - %s, Result = %s\n",
                                j, pbistName(pbist_array[i]),
                                testStatusPrint((pbist_stage_status[j] >> i) & 0x1));
            }
            AppUtils_Printf(MSG_NORMAL,
                            "BIST: Stage %d - Ran %d PBIST total sections\n",
                            j, (uint32)num_pbists_per_boot_stage[j]);

            AppUtils_Printf(MSG_NORMAL,
                            "BIST: Stage %d - Started running LBIST sections at %d usec\n",
                            j, (uint32)time_lbist_start[j]);
            for (i = 0; i < num_lbists_per_boot_stage[j]; i++)
            {
                AppUtils_Printf(MSG_NORMAL,
                                "BIST: Stage %d - Ran LBIST ID - %s, Result = %s\n",
                                j, lbistName(lbist_array[i]),
                                testStatusPrint((lbist_stage_status[j] >> i) & 0x1));
            }
            AppUtils_Printf(MSG_NORMAL,
                            "BIST: Stage %d - Ran %d LBIST sections\n",
                            j, (uint32)num_lbists_per_boot_stage[j]);
#endif
            AppUtils_Printf(MSG_NORMAL,
                            "BIST: Stage %d - Completed stage at %d usec\n",
                            j, (uint32)time_stage_finish[j]);
        }
    }
}

