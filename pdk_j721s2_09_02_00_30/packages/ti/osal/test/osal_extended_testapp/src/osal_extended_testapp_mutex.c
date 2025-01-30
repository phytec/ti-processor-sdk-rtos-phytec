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
 *  \file   osal_extended_testapp_mutex.c
 *
 *  \brief  OSAL Mutex Sub Module testcases file.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "osal_extended_test.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#if defined (FREERTOS)
#define OSAL_APP_MAX_MUTEX         (OSAL_FREERTOS_CONFIGNUM_MUTEX)
#elif defined (SAFERTOS)
#define OSAL_APP_MAX_MUTEX         (OSAL_SAFERTOS_CONFIGNUM_MUTEX)
#endif

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                           Function Declarations                            */
/* ========================================================================== */

/*
 * Description: Test Mutex nonos APIs by calling in this function
 */
static int32_t OsalApp_mutexGeneralTest(void);

/*
 * Description: Negative Test for Mutex API
 */
static int32_t OsalApp_mutexNegativeCheckTest(void);

/*
 * Description: Multiple Mutex creation Test
 */
static int32_t OsalApp_mutexMaxCreateTest(void);

/* ========================================================================== */
/*                      Internal Function Definitions                         */
/* ========================================================================== */

static int32_t OsalApp_mutexNegativeCheckTest(void)
{
    MutexP_Object         mutexObj;
    MutexP_Handle         mutexHandle;
    int32_t               result = osal_OK;

    mutexHandle = MutexP_create(NULL_PTR);
    if(NULL_PTR != mutexHandle)
    {
        result = osal_FAILURE;
    }

    if(osal_OK == result)
    {
        mutexHandle = MutexP_create(&mutexObj);
        if(NULL_PTR == mutexHandle)
        {
            result = osal_FAILURE;
        }
    }

    if(osal_OK == result)
    {
        if(MutexP_OK != MutexP_lock(mutexHandle, MutexP_NO_WAIT))
        {
            result = osal_FAILURE;
        }
        else
        {
            if(MutexP_OK != MutexP_unlock(mutexHandle))
            {
                result = osal_FAILURE;
            }
        }
    }

    if(osal_OK == result)
    {
        if(MutexP_OK != MutexP_delete(mutexHandle))
        {
            result = osal_FAILURE;
        }
    }
    if(osal_OK != result)
    {
        OSAL_log("\n Mutex general test failed! \n");
    }

    return result;
}

static int32_t OsalApp_mutexGeneralTest(void)
{
    MutexP_Object         *mutexObj = (MutexP_Object *)NULL;
    MutexP_Handle         mutexHandle;
    MutexP_Status         status;
    int32_t               result = osal_OK;

    mutexHandle = MutexP_create(mutexObj);
    if(NULL != mutexHandle)
    {
       result = osal_FAILURE;
    }
    if(osal_OK == result)
    {
        status = MutexP_lock(mutexHandle, MutexP_WAIT_FOREVER);
        if(MutexP_OK == status)
        {
            result = osal_FAILURE;
        }
    }

    if(osal_OK == result)
    {
        status = MutexP_unlock(mutexHandle);
        if(MutexP_OK == status)
        {
            result = osal_FAILURE;
        }
    }

    if(osal_OK == result)
    {
        status = MutexP_delete(mutexHandle);
        if(MutexP_OK == status)   
        {
            result = osal_FAILURE;
        }
    }
    
    if(osal_OK != result)
    {
        OSAL_log("\n Mutex negative test failed! \n");
    }

    return result;
}

#if !defined (BARE_METAL)
static int32_t OsalApp_mutexMaxCreateTest(void)
{
    MutexP_Object         mutexObj[OSAL_APP_MAX_MUTEX];
    MutexP_Handle         mutexHandles[OSAL_APP_MAX_MUTEX];
    int32_t               result = osal_OK;
    uint32_t              index = 0U;

    for(index = 0U; index < OSAL_APP_MAX_MUTEX; index++)
    {
        mutexHandles[index] = MutexP_create(&mutexObj[index]);
        if(NULL_PTR == mutexHandles[index])
        {
            result = osal_FAILURE;
        }
    }
    if((osal_OK != result) || (NULL_PTR != MutexP_create(&mutexObj[index])))
    {
        result = osal_FAILURE;
    }
    if(osal_OK == result)
    {
        for(index = 0U; index < OSAL_APP_MAX_MUTEX; index++)
        {
            if(MutexP_OK != MutexP_delete(mutexHandles[index]))
            {
                result = osal_FAILURE;
            }
        }
    }

    if(osal_OK != result)
    {
        OSAL_log("\n Multiple Mutex create test failed! \n");
    }
    
    return result;
}
#endif

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int32_t OsalApp_mutexTests(void)
{
    int32_t result = osal_OK;

    result += OsalApp_mutexGeneralTest();
/*    
 * TODO : The Negative check for Mutex APIs
 */
#if !defined(BARE_METAL)
    result += OsalApp_mutexMaxCreateTest();
#endif

    result += OsalApp_mutexNegativeCheckTest();

    if(osal_OK != result)
    {
        OSAL_log("\n Some or all MutexP tests have failed! \n");
    }
    else
    {
        OSAL_log("\n All MutexP tests have passed! \n");
    }

    return result;
}

