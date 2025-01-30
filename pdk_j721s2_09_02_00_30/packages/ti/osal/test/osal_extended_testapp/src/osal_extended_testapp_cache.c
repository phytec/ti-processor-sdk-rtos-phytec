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
 *  \file   osal_extended_testapp_cache.c
 *
 *  \brief  OSAL Cache Sub Module testcases file.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "osal_extended_test.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define OSAL_APP_CACHE_TEST_ARR_SIZE (64U)
#define OSAL_APP_CACHE_TEST_ITER     (1024U)

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

uint8_t gOsalAppCacheTestArr[OSAL_APP_CACHE_TEST_ARR_SIZE]; 

/* ========================================================================== */
/*                           Function Declarations                            */
/* ========================================================================== */

/*
 * Description : Test Cache writeback.
 */
int32_t OsalApp_cacheWbTest();

/*
 * Description : Test Cache invalidation.
 */
int32_t OsalApp_cacheInvTest();

/* ========================================================================== */
/*                          Internal Function Definitions                     */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int32_t OsalApp_cacheWbTest() 
{
    uint32_t index = 0U;
    int32_t result = osal_OK;

    for(index = 0U; index < OSAL_APP_CACHE_TEST_ARR_SIZE; index++)
    {
        gOsalAppCacheTestArr[index]='A';
    }

    /*  Write back the cache */
    CacheP_wb(gOsalAppCacheTestArr, OSAL_APP_CACHE_TEST_ARR_SIZE);

    /* Invalidate the cache, so that the next read is force to be from backing store. */
    CacheP_Inv(gOsalAppCacheTestArr, OSAL_APP_CACHE_TEST_ARR_SIZE);

    for(index = 0U; index < OSAL_APP_CACHE_TEST_ARR_SIZE; index++)
    {
        if('A' != gOsalAppCacheTestArr[index])
        {
            result = osal_FAILURE;
            break;
        }
    }
    if(osal_OK == result)
    {
        OSAL_log("\n OsalApp_cacheWbTest passed!!\n");
    }
    else
    {
        OSAL_log("\n OsalApp_cacheWbTest failed!!\n");
    }

    return result;
}

/*
 * Description  : Testing by storing data in array and reading it and then paasing to CacheP_Inv
 */
int32_t OsalApp_cacheInvTest(void)
{
    uint32_t index = 0U;
    int32_t result = osal_OK;

    /*  Make a lot of accesses so that the array is cached in. */
    for(index = 0U; index < OSAL_APP_CACHE_TEST_ITER; index++)
    {
        gOsalAppCacheTestArr[index%4] = 'A' + index%4;
    }
    /* Update the data that is already in the cache due to a lot of access made just now. */
    gOsalAppCacheTestArr[0] = 'P';
    gOsalAppCacheTestArr[1] = 'Q';
    gOsalAppCacheTestArr[2] = 'R';
    gOsalAppCacheTestArr[3] = 'S';

    /* Immediately invalidate, so that the cached data is not written back. */
    CacheP_Inv(gOsalAppCacheTestArr,64U);

    /* As cache was invalidated without writeback, latest update should not be visible. */
    if('P' == gOsalAppCacheTestArr[0])
    {
        OSAL_log("\n OsalApp_cacheInvTest failed!!\n");
        result = osal_FAILURE;
    }
    else
    {
        OSAL_log("\n OsalApp_cacheInvTest passed!!\n");
    }

    return result;    
}

int32_t OsalApp_cacheTests(void)
{
    int32_t result = osal_OK;

    result += OsalApp_cacheInvTest();
    result += OsalApp_cacheWbTest();

    if(osal_OK == result)
    {
        OSAL_log("\n All Cache Tests have passed!!\n");
    }
    else
    {
        OSAL_log("\n All Cache Tests have failed!!\n");
    }

    return result;
}

