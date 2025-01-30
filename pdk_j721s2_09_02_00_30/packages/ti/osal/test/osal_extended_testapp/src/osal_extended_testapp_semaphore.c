/*
 * Copyright (C) 2024 Texas Instruments Incorporated - http://www.ti.com/
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of Texas Instruments Incorporated nor the names of
 * its contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**
 *  \file   osal_extended_testapp_semaphore.c
 *
 *  \brief  OSAL Semaphore Sub Module testcases file.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "osal_extended_test.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#if defined(BARE_METAL)
#define OSAL_APP_MAX_SEMAPHORE    OSAL_NONOS_CONFIGNUM_SEMAPHORE
#elif defined(FREERTOS)
#define OSAL_APP_MAX_SEMAPHORE    OSAL_FREERTOS_CONFIGNUM_SEMAPHORE
#elif defined(SAFERTOS)
#define OSAL_APP_MAX_SEMAPHORE    OSAL_SAFERTOS_CONFIGNUM_SEMAPHORE
#endif

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                           Function Declarations                            */
/* ========================================================================== */

/*
 * Description : Testing Null check on below semaphore APIs
 *                 1. SemaphoreP_Params_init
 *                 2. SemaphoreP_postFromISR
 *                 3. SemaphoreP_getCount
 */
static int32_t OsalApp_semaphoreNullTest(void);

/*
 * Description : Testing below APIs 
 *                 1. SemaphoreP_pend API with timeout
 *                 2. SemaphoreP_compileTime_SizeChk for size check 
 */
static int32_t OsalApp_semaphorePendTest(void);

/*
 * Description : Testing Maximum semaphore creation
 */
static int32_t OsalApp_semaphoreMaxTest(void);

/* ========================================================================== */
/*                    Internal Function Definitions                           */
/* ========================================================================== */

static int32_t OsalApp_semaphoreNullTest(void)
{
    SemaphoreP_Params    semParams;
    SemaphoreP_Handle    nullPtr = NULL_PTR;
    int32_t              result = osal_OK;
    
    SemaphoreP_Params_init(&semParams);
    
    /* SemaphoreP_post will also be covered */
    if(SemaphoreP_OK == SemaphoreP_postFromISR(nullPtr))
    {
        result = osal_FAILURE;
    }
    
    if(0U != SemaphoreP_getCount(nullPtr))
    {
        result = osal_FAILURE;
    }
    
    if(SemaphoreP_OK == SemaphoreP_delete(nullPtr))
    {
        result = osal_FAILURE;
    }
    if(osal_OK != result)
    {
        OSAL_log("\n Semaphore Null test has failed!\n");
    }
    
    return result;
}

static int32_t OsalApp_semaphorePendTest(void)
{
    SemaphoreP_Params    semParams;
    SemaphoreP_Handle    semhandle;
    int32_t              result = osal_OK;
    
    SemaphoreP_Params_init(&semParams);
    
    semParams.mode = SemaphoreP_Mode_COUNTING;
    semhandle = SemaphoreP_create(0, &semParams);
    if(NULL_PTR != semhandle)
    {
        if(SemaphoreP_OK != SemaphoreP_post(semhandle))
        {
            result = osal_FAILURE;
        }
        if(SemaphoreP_OK != SemaphoreP_pend(semhandle, SemaphoreP_WAIT_FOREVER))
        {
            result = osal_FAILURE;
        }
        SemaphoreP_compileTime_SizeChk();
        if(SemaphoreP_OK != SemaphoreP_delete(semhandle))
        {
            result = osal_FAILURE;
        }    
    }
    if(osal_OK != result)
    {
        OSAL_log("\n Semaphore Pend test failed! \n");
    }
    else
    {
        OSAL_log("\n Semaphore Pend test passed! \n");
    }
    
    return result;
}

static int32_t OsalApp_semaphoreMaxTest(void)
{
    SemaphoreP_Params    semParams;
    SemaphoreP_Handle    semhandle[OSAL_APP_MAX_SEMAPHORE];
    int32_t              result = osal_OK;
    uint32_t             index = 0U, maxindex = 0U;

    SemaphoreP_Params_init(&semParams);
    
    semParams.mode = SemaphoreP_Mode_COUNTING;
    for(index = 0U; index < OSAL_APP_MAX_SEMAPHORE; index++)
    {
        semhandle[index] = SemaphoreP_create(0, &semParams);
        if(NULL_PTR == semhandle[index])
        {
            break;
        }
    }
    maxindex = index;
    if(NULL_PTR != SemaphoreP_create(0, &semParams))
    {
        result = osal_FAILURE;
    }
    for(index = 0U ; index < maxindex; index++)
    {
        if(SemaphoreP_OK != SemaphoreP_delete(semhandle[index]))
        {
            result = osal_FAILURE;
        }
    }
    if(osal_OK != result)
    {
        OSAL_log("\n Multiple Semaphore create test failed! \n");
    }
    else
    {
        OSAL_log("\n Multiple Semaphore create test passed! \n");
    }
    return result;
}

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int32_t OsalApp_semaphoreTests(void)
{
    int32_t result = osal_OK;
    
    result += OsalApp_semaphoreNullTest();
    result += OsalApp_semaphorePendTest();
    result += OsalApp_semaphoreMaxTest();
    
    if(osal_OK == result)
    {
        OSAL_log("\n All Semaphore test have passed!\n");
    }
    else
    {
        OSAL_log("\n Some or All Semaphore test have failed!\n");
    }
    
    return result;
}

