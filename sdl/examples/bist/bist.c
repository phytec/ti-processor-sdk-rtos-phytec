/*
*
* Copyright (c) 2022 Texas Instruments Incorporated
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

#include <stdio.h>

#include "lbist_utils.h"
#include "pbist_utils.h"
#include "bist_core_defs.h"

#include <ti/drv/uart/UART.h>
#include <ti/drv/uart/UART_stdio.h>

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


void bist_TaskFxn(void)
{
    int32_t testResult = 0;
    int32_t     i, j;
    int32_t    *pbist_array;
    int32_t    *lbist_array;
    /* Bitmap representing status of each PBIST test in each boot stage */
    int32_t     pbist_stage_status[NUM_BOOT_STAGES];
    /* Bitmap representing status of each negative PBIST test in each boot stage */
    int32_t     pbist_stage_neg_status[NUM_BOOT_STAGES];
        /* Bitmap representing status of each PBIST check of ROM in each boot stage */
    #if defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
    int32_t     pbist_stage_rom_test_status[NUM_BOOT_STAGES];
    #endif
    /* Bitmap representing status of each LBIST test in each boot stage */
    int32_t     lbist_stage_status[NUM_BOOT_STAGES];

    /* Initialize boot stage status bitmaps to "not run/fail" */
    for (i = 0; i < NUM_BOOT_STAGES; i++)
    {
        pbist_stage_status[i] = 0x0;
        pbist_stage_neg_status[i] = 0x0;
        lbist_stage_status[i] = 0x0;
        #if defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
        pbist_stage_rom_test_status[i]=0x0;
        #endif
    }

    testResult = PBIST_commonInit();

    if (testResult != 0)
    {
        UART_printf("PBIST_commonInit ...FAILED \n");
    }
    else
    {
        /* This example presents a scenario in which the BISTs are performed in stages
         * with booting of specific cores performed after certain stages. This example
	 * is adapted from the MCUSW Boot App for J721E. No cores are actually loaded
	 * in this example. Only the flow of performing BIST in stages is shown. */
        /* Run pre-boot-stage PBIST's.  The definitions of the pre-boot-stage PBIST's
         * are found in soc/<SOC Device>/bist_core_defs.c.*/
	for (i = 0; i < num_pbists_pre_boot; i++)
        {
            #if defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
            /* Run test on selected instance */
            testResult = PBIST_runTest(pbist_pre_boot_stage[i], (uint8_t)PBIST_TEST_NEGATIVE);
            /* Convert signed return value (with -1 = failure and 0 = pass) to become
             * 0 = failure and 1 = pass */
            pbist_pre_boot_stage_neg_status[i] = testResult + 1;
            if ( testResult != 0)
            {
                UART_printf("PBIST negative test failed for %d\n",
                                pbist_pre_boot_stage[i]);
                break;
            }
            #else
            /* Run test on selected instance */
            testResult = PBIST_runTest(pbist_pre_boot_stage[i], (uint8_t)1);
            /* Convert signed return value (with -1 = failure and 0 = pass) to become
             * 0 = failure and 1 = pass */
            pbist_pre_boot_stage_neg_status[i] = testResult + 1;
            if ( testResult != 0)
            {
                UART_printf("PBIST negative test failed for %d\n",
                                pbist_pre_boot_stage[i]);
                break;
            }
            #endif

        }

        for (i = 0; i < num_pbists_pre_boot; i++)
        {
            #if defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
            /* Run test on selected instance */
            testResult = PBIST_runTest(pbist_pre_boot_stage[i],(uint8_t)PBIST_TEST_POSITIVE);
            /* Convert signed return value (with -1 = failure and 0 = pass) to become
             * 0 = failure and 1 = pass */
            pbist_pre_boot_stage_status[i] = testResult + 1;
            if ( testResult != 0)
            {
                UART_printf("PBIST functional test failed for %d\n",
                                pbist_pre_boot_stage[i]);
                break;
            }
            #else
            /* Run test on selected instance */
            testResult = PBIST_runTest(pbist_pre_boot_stage[i], (uint8_t)0);
            /* Convert signed return value (with -1 = failure and 0 = pass) to become
             * 0 = failure and 1 = pass */
            pbist_pre_boot_stage_status[i] = testResult + 1;
            if ( testResult != 0)
            {
                UART_printf("PBIST functional test failed for %d\n",
                                pbist_pre_boot_stage[i]);
                break;
            }
            #endif
        }
        #if defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
        for (i = 0; i < num_pbists_pre_boot; i++)
        {
            /* Run test on selected instance */
            testResult = PBIST_runTest(pbist_pre_boot_stage[i], (uint8_t)PBIST_TEST_ROM);
            /* Convert signed return value (with -1 = failure and 0 = pass) to become
             * 0 = failure and 1 = pass */
            pbist_pre_boot_stage_rom_test_status[i] = testResult + 1;
            if ( testResult != 0)
            {
                UART_printf("PBIST ROM test failed for %d\n",
                                pbist_pre_boot_stage[i]);
            }
        }
        #endif

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
                UART_printf("LBIST functional test failed for %d\n",
                                lbist_pre_boot_stage[i]);
                break;
            }
        }

        /* After running PBIST potentially on MSMC, re-initialize the CLEC */
        testResult = PBIST_commonInit();

        if (testResult != 0)
        {
            UART_printf("PBIST_commonInit after pre-boot stage ...FAILED \n");
        }

        /* Run LBIST and PBIST before each boot stage.  The definitions of the LBIST
         * and PBIST sections for each stage are defined in
         * soc/<SOC Device>/bist_core_defs.c. */
        for (j = 0; j < NUM_BOOT_STAGES; j++)
        {
            pbist_array = pbist_array_stage[j];
            lbist_array = lbist_array_stage[j];
            for (i = 0; i < num_pbists_per_boot_stage[j]; i++)
            {
                PBIST_clecConfig(pbist_array[i]);

                #if defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
                /* Run test on selected instance */
                /*MCU instances are not supported for neg and pos test, 
                So skipped according to the pbist_first_boot_stage array sequence */
				#if defined (SOC_J784S4)
				if((i==27)|| (i==28)||(i==29))
				{
					continue;
				}
                #endif		
                /*MCU instances are not supported for neg and pos test, 
                So skipped according to the pbist_first_boot_stage array sequence */				
                #if defined (SOC_J7200)
                if((i== 6U)|| (i==7U)||(i==8U))
                {
                    continue;
                }
                #endif
                /*MCU instances are not supported for neg and pos test, 
                So skipped according to the pbist_first_boot_stage array sequence */					
                #if defined (SOC_J721S2)
                if((i==14)|| (i==15)||(i==16))
                {
                    continue;
                }
                #endif
                testResult = PBIST_runTest(pbist_array[i], (uint8_t)PBIST_TEST_NEGATIVE);

                /* Convert signed return value (with -1 = failure and 0 = pass) to become
                 * a single bit as part of bitfield with 0 = failure and 1 = pass */
                pbist_stage_neg_status[j] |= ((uint32_t)(testResult + 1) << i);
                if ( testResult != 0)
                {
                    UART_printf("PBIST negative test failed for %d\n",
                                    pbist_array[i]);
                    break;
                }
                #else
                /* Run test on selected instance */
                testResult = PBIST_runTest(pbist_array[i], (uint8_t)1);

                /* Convert signed return value (with -1 = failure and 0 = pass) to become
                 * a single bit as part of bitfield with 0 = failure and 1 = pass */
                pbist_stage_neg_status[j] |= ((uint32_t)(testResult + 1) << i);
                if ( testResult != 0)
                {
                    UART_printf("PBIST negative test failed for %d\n",
                                    pbist_array[i]);
                    break;
                }
                #endif
            }

            for (i = 0; i < num_pbists_per_boot_stage[j]; i++)
            {
                #if defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
                /* Run test on selected instance */	
                /*MCU instances are not supported for neg and pos test, 
                So skipped according to the pbist_first_boot_stage array sequence */	
				#if defined (SOC_J784S4)
				if((i==27)|| (i==28)||(i==29))
				{
					continue;
				}
                #endif	
                /*MCU instances are not supported for neg and pos test, 
                So skipped according to the pbist_first_boot_stage array sequence */					
                #if defined (SOC_J7200)
                if((i== 6U)|| (i==7U)||(i==8U))
                {
                    continue;
                }
                #endif	
                /*MCU instances are not supported for neg and pos test, 
                So skipped according to the pbist_first_boot_stage array sequence */					
                #if defined (SOC_J721S2)
                if((i==14)|| (i==15)||(i==16))
                {
                    continue;
                }
                #endif
                testResult = PBIST_runTest(pbist_array[i], (uint8_t)PBIST_TEST_POSITIVE);

                /* Convert signed return value (with -1 = failure and 0 = pass) to become
                 * a single bit as part of bitfield with 0 = failure and 1 = pass */
                pbist_stage_status[j] |= ((uint32_t)(testResult + 1) << i);
                if ( testResult != 0)
                {
                    UART_printf("PBIST functional test failed for %d\n",
                                    pbist_array[i]);
                    break;
                }
                #else
                testResult = PBIST_runTest(pbist_array[i], (uint8_t)0);

                /* Convert signed return value (with -1 = failure and 0 = pass) to become
                 * a single bit as part of bitfield with 0 = failure and 1 = pass */
                pbist_stage_status[j] |= ((uint32_t)(testResult + 1) << i);
                if ( testResult != 0)
                {
                    UART_printf("PBIST functional test failed for %d\n",
                                    pbist_array[i]);
                    break;
                }
                #endif
            }
            #if defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
            for (i = 0; i < num_pbists_per_boot_stage[j]; i++)
            {
                /* Run test on selected instance */
                testResult = PBIST_runTest(pbist_array[i], (uint8_t)PBIST_TEST_ROM);

                /* Convert signed return value (with -1 = failure and 0 = pass) to become
                 * a single bit as part of bitfield with 0 = failure and 1 = pass */
                pbist_stage_rom_test_status[j] |= ((uint32_t)(testResult + 1) << i);
                if ( testResult != 0)
                {
                    UART_printf("PBIST ROM  test failed for %d\n",
                                    pbist_array[i]);
                }
            }
            #endif
#if defined(DEBUG)
            UART_printf( "Ran PBIST for Stage %d\n", j);
#endif
            for (i = 0; i < num_lbists_per_boot_stage[j]; i++)
            {
                testResult = LBIST_runTest(lbist_array[i]);

                /* Convert signed return value (with -1 = failure and 0 = pass) to become
                 * a single bit as part of bitfield with 0 = failure and 1 = pass */
                lbist_stage_status[j] |= ((uint32_t)(testResult + 1) << i);
                if (testResult != 0)
                {
                    UART_printf("LBIST functional test failed for %d\n",
                                    lbist_array[i]);
                    break;
                }
            }
#if defined(DEBUG)
            UART_printf( "Ran LBIST for Stage %d\n", j);
#endif
            /* Signal Boot Task that BIST for the stage is completed */
            UART_printf("\n *** Boot stage %d is complete, cores for this stage may now be loaded ***\n\n", j);
        }
    }

    if (testResult == 0)
    {
        UART_printf("==========================\n");
        UART_printf("BIST: Example App Summary:\n");
        UART_printf("==========================\n");
        for (i = 0; i < num_pbists_pre_boot; i++)
        {
            UART_printf("BIST: Pre-boot Stage - Ran negative PBIST ID - %s, Result = %s\n",
                            pbistName(pbist_pre_boot_stage[i]),
                            testStatusPrint(pbist_pre_boot_stage_neg_status[i]));
        }
        UART_printf("Pre-boot stage - Ran %d negative PBIST total sections\n",
                        num_pbists_pre_boot);

        #if defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
        for (i = 0; i < num_pbists_pre_boot; i++)
        {
            UART_printf("BIST: Pre-boot Stage - Ran ROM Test PBIST ID - %s, Result = %s\n",
                            pbistName(pbist_pre_boot_stage[i]),
                            testStatusPrint(pbist_pre_boot_stage_rom_test_status[i]));
        }
        UART_printf("Pre-boot stage - Ran %d ROM Test of  PBIST total sections\n",
                        num_pbists_pre_boot);
        #endif
        for (i = 0; i < num_pbists_pre_boot; i++)
        {
            UART_printf("BIST: Pre-boot Stage - Ran PBIST ID - %s, Result = %s\n",
                            pbistName(pbist_pre_boot_stage[i]),
                            testStatusPrint(pbist_pre_boot_stage_status[i]));
        }
        UART_printf("Pre-boot stage - Ran %d PBIST total sections\n",
                        num_pbists_pre_boot);
        for (i = 0; i < num_lbists_pre_boot; i++)
        {
            UART_printf("BIST: Pre-boot Stage - Ran LBIST ID - %s, Result = %s\n",
                            lbistName(lbist_pre_boot_stage[i]),
                            LBIST_hwPostStatusPrint(lbist_pre_boot_stage_status[i]));
        }
        UART_printf("Pre-boot stage - Ran %d LBIST total sections\n",
                        num_lbists_pre_boot);

        for (j = 0; j < NUM_BOOT_STAGES; j++)
        {
            pbist_array = pbist_array_stage[j];
            lbist_array = lbist_array_stage[j];
#if defined(GATHER_BIST_STAGE_DETAILS)
            for (i = 0; i < num_pbists_per_boot_stage[j]; i++)
            {
                /*MCU instances are not supported for neg and pos test, 
                So skipped according to the pbist_first_boot_stage array sequence */
				#if defined (SOC_J784S4)
				if((i==27)|| (i==28)||(i==29))
				{
					continue;
				}
                #endif	
                /*MCU instances are not supported for neg and pos test, 
                So skipped according to the pbist_first_boot_stage array sequence */					
                #if defined (SOC_J7200)
                if((i== 6U)|| (i==7U)||(i==8U))
                {
                    continue;
                }
                #endif
                /*MCU instances are not supported for neg and pos test, 
                So skipped according to the pbist_first_boot_stage array sequence */					
                #if defined (SOC_J721S2)
                if((i==14)|| (i==15)||(i==16))
                {
                    continue;
                }
                #endif					
                UART_printf("BIST: Stage %d - Ran negative PBIST ID - %s, Result = %s\n",
                                j, pbistName(pbist_array[i]),
                                testStatusPrint((pbist_stage_neg_status[j] >> i) & 0x1));
            }
            UART_printf("BIST: Stage %d - Ran %d negative PBIST total sections\n",
                            j, (uint32_t)num_pbists_per_boot_stage[j]);

            #if defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
            for (i = 0; i < num_pbists_per_boot_stage[j]; i++)
            {
                UART_printf("BIST: Stage %d - Ran ROM Test PBIST ID - %s, Result = %s\n",
                                j, pbistName(pbist_array[i]),
                                testStatusPrint((pbist_stage_rom_test_status[j] >> i) & 0x1));
            }
            UART_printf("BIST: Stage %d - Ran %d ROM Test PBIST total sections\n",
                            j, (uint32_t)num_pbists_per_boot_stage[j]);

            #endif
            for (i = 0; i < num_pbists_per_boot_stage[j]; i++)
            {
                /*MCU instances are not supported for neg and pos test, 
                So skipped according to the pbist_first_boot_stage array sequence */
				#if defined (SOC_J784S4)
				if((i==27)|| (i==28)||(i==29))
				{
					continue;
				}
                #endif	
                /*MCU instances are not supported for neg and pos test, 
                So skipped according to the pbist_first_boot_stage array sequence */					
                #if defined (SOC_J7200)
                if((i== 6U)|| (i==7U)||(i==8U))
                {
                    continue;
                }
                #endif	
                /*MCU instances are not supported for neg and pos test, 
                So skipped according to the pbist_first_boot_stage array sequence */					
                #if defined (SOC_J721S2)
                if((i==14)|| (i==15)||(i==16))
                {
                    continue;
                }
                #endif				
                UART_printf("BIST: Stage %d - Ran PBIST ID - %s, Result = %s\n",
                                j, pbistName(pbist_array[i]),
                                testStatusPrint((pbist_stage_status[j] >> i) & 0x1));
            }
            UART_printf("BIST: Stage %d - Ran %d PBIST total sections\n",
                            j, (uint32_t)num_pbists_per_boot_stage[j]);

            for (i = 0; i < num_lbists_per_boot_stage[j]; i++)
            {
                UART_printf("BIST: Stage %d - Ran LBIST ID - %s, Result = %s\n",
                                j, lbistName(lbist_array[i]),
                                testStatusPrint((lbist_stage_status[j] >> i) & 0x1));
            }
            UART_printf("BIST: Stage %d - Ran %d LBIST sections\n",
                            j, (uint32_t)num_lbists_per_boot_stage[j]);
#endif
        }
        UART_printStatus("\r\nAll tests have passed. \r\n");
    }
}

