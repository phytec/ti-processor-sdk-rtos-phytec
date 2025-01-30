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
 *  \file    osal_extended_testapp_mailbox.c
 *
 *  \brief  OSAL Mailbox Sub Module testcases file.
 *
 */

/* ================================================================================== */
/*                                      Include Files                                 */
/* ================================================================================== */

#include "osal_extended_test.h"
#if defined (SAFERTOS)
#include "SafeRTOS_API.h"
#endif

/* ================================================================================== */
/*                                    Macros & Typedefs                               */
/* ================================================================================== */

#if defined(FREERTOS)
#define OSAL_APP_MAX_NUM_MAILBOX   (OSAL_FREERTOS_CONFIGNUM_MAILBOX)
#define OSAL_APP_MAILBOX_OVERHEAD  (0U)
#elif defined(SAFERTOS)
#define OSAL_APP_MAX_NUM_MAILBOX   (OSAL_SAFERTOS_CONFIGNUM_MAILBOX-4)
#define OSAL_APP_MAILBOX_OVERHEAD  (safertosapiQUEUE_OVERHEAD_BYTES)
#endif
#define OSAL_APP_MB_TIMEOUT        (1U)
#define OSAL_APP_MB_SEND_STR       ("Texas Instruments\0")
#define OSAL_APP_MAX_MB_BUF_LEN    (256U)
#define OSAL_APP_IRQ_INT_NUM       (32U)

/* ================================================================================== */
/*                                     Global Variables                               */
/* ================================================================================== */

/* Mailbox handle used to mailbox post in isr */
volatile MailboxP_Handle gmailboxpHandle;
uint8_t gOsalAppMbQueueBuf[OSAL_APP_MAX_MB_BUF_LEN] __attribute__((aligned(64)));
uint8_t gOsalAppMbMultiCreateQueueBuf[OSAL_APP_MAX_NUM_MAILBOX][OSAL_APP_MAX_MB_BUF_LEN] __attribute__((aligned(64)));
uint8_t gOsalAppStrToSend[64] = {'T', 'e', 'x', 'a', 's', ' ', 'I', 'n', 's', 't', 'r', 'u','m', 'e', 'n', 't', 's', '\0',};

/* ================================================================================== */
/*                                     Internal Functions                             */
/* ================================================================================== */

void OsalApp_mbISR(void *arg)
{   
    uint32_t iter;
    uint32_t len = strlen((char *)gOsalAppStrToSend);

    for(iter = 0U; iter < len; iter++)
    {
        MailboxP_post(gmailboxpHandle, &gOsalAppStrToSend[iter], OSAL_APP_MB_TIMEOUT);
    }
}

/* ================================================================================== */
/*                                    Function Declarations                           */
/* ================================================================================== */

/* 
 * Description: Test creation of as many mailboxes as supported.
 */
int32_t OsalApp_mailboxCreateMultipleTest(void);

/* 
 * Description: Test the mailbox pend and post functionality by calling below mentioned APIs :
 *      1. MailboxP_Params_init
 *      2. MailboxP_create
 *      3. MailboxP_delete
 *      4. MailboxP_post
 *      5. MailboxP_pend
 */
int32_t OsalApp_mailboxPendPostTest(uint32_t timeout);

/*
 * Description  : Tests MailboxP create when the used parameter of mailbox is 0.
 */
int32_t OsalApp_mailboxUsedTest(void);

/* 
 * Description: Test the mailbox pend and post functionality, while posting from an ISR..
 */
int32_t OsalApp_mailboxISRTest(void);

/* ================================================================================== */
/*                                  Function Definitions                              */
/* ================================================================================== */

int32_t OsalApp_mailboxCreateMultipleTest(void)
{
    MailboxP_Params params;
    MailboxP_Handle handles[OSAL_APP_MAX_NUM_MAILBOX];
    uint32_t mbindex;
    int32_t result = osal_OK;
    
    MailboxP_Params_init(&params);
    params.count = strlen((char *)gOsalAppStrToSend);
    params.size  = sizeof(uint8_t);
    params.bufsize = params.count*params.size + OSAL_APP_MAILBOX_OVERHEAD;

    for(mbindex = 0U; mbindex < (OSAL_APP_MAX_NUM_MAILBOX); mbindex++)
    {
        params.buf = gOsalAppMbMultiCreateQueueBuf[mbindex];
        handles[mbindex] = MailboxP_create(&params);
        if(NULL == handles[mbindex])
        {
            result = osal_FAILURE;
            break;
        }
    }
    if(osal_OK == result)
    {
        if(NULL != MailboxP_create(&params))
        {
            result = osal_FAILURE;
        }
    }

    if(osal_OK == result)
    {
        for(mbindex = 0U; mbindex < (OSAL_APP_MAX_NUM_MAILBOX); mbindex++)
        {
#if defined (SAFERTOS)
            /* MailboxP_delete is a dummy call in SafeRTOS. */
            MailboxP_delete(handles[mbindex]);
#elif defined (FREERTOS)
            if(MailboxP_delete(handles[mbindex]) != MailboxP_OK)
            {
                result = osal_FAILURE;
                break;
            }
#endif
        }
    }
    if(osal_OK == result)
    {
        OSAL_log("\n Multiple mailbox create test has passed!!\n");
    }
    else
    {
        OSAL_log("\n Multiple mailbox create test has failed!!\n");
    }

    return result;
}

int32_t OsalApp_mailboxPendPostTest(uint32_t timeout)
{
    MailboxP_Params params;
    MailboxP_Handle handle;
    MailboxP_Status status = MailboxP_OK;
    uint32_t index;
    int32_t result = osal_OK;
    uint8_t recvBuf[OSAL_APP_MAX_MB_BUF_LEN];

    MailboxP_Params_init(&params);
    params.count = strlen((char *)gOsalAppStrToSend);
    params.size  = sizeof(uint8_t);
    memset(gOsalAppMbQueueBuf, 0U, sizeof(gOsalAppMbQueueBuf)/sizeof(gOsalAppMbQueueBuf[0]));
    params.buf = gOsalAppMbQueueBuf;
    params.bufsize = params.count*params.size + OSAL_APP_MAILBOX_OVERHEAD;

    handle = MailboxP_create(&params);
    if(NULL_PTR == handle)
    {
        result = osal_FAILURE;
    }
    
    if(osal_OK == result)
    {
        for(index = 0U; index < params.count; index++)
        {
            status += MailboxP_post(handle, &gOsalAppStrToSend[index], OSAL_APP_MB_TIMEOUT);
        }
        if(MailboxP_OK != status)
        {
            result = osal_FAILURE;
        }
    }
    
    if(osal_OK == result)
    {
        for(index = 0U; index < params.count; index++)
        {
            status += MailboxP_pend(handle, &recvBuf[index], OSAL_APP_MB_TIMEOUT);
        }
        if(MailboxP_OK != status)
        {
            result = osal_FAILURE;
        }
    }
    
    if((osal_OK != result) ||
       (0U != MailboxP_getNumPendingMsgs(handle)) || 
       (0U != strncmp((char *)gOsalAppStrToSend, (char *)recvBuf, params.count)))
    {
        result = osal_FAILURE;
    }

    if(osal_OK == result)
    {
#if defined (FREERTOS)
        if(MailboxP_OK != MailboxP_delete(handle))
#elif defined (SAFERTOS)
        if(MailboxP_FAILURE != MailboxP_delete(handle))
#endif
        {
            result = osal_FAILURE;
        }
    }
    
    if(osal_OK == result)
    {
        OSAL_log("\n MailboxP test passed!!\n");
    }
    else
    {
        OSAL_log("\n MailboxP test failed!!\n");
    }

    return result;
}

int32_t OsalApp_mailboxUsedTest(void)
{
    MailboxP_Params params;
    MailboxP_Handle handle;
    uint32_t *handleAddr, temp;
    int32_t result = osal_OK;

    MailboxP_Params_init(NULL_PTR);
    MailboxP_Params_init(&params);
    params.count = strlen((char *)gOsalAppStrToSend);
    params.size  = sizeof(uint8_t);
    memset(gOsalAppMbQueueBuf, 0U, sizeof(gOsalAppMbQueueBuf)/sizeof(gOsalAppMbQueueBuf[0]));
    params.buf = gOsalAppMbQueueBuf;
    params.bufsize = params.count*params.size + OSAL_APP_MAILBOX_OVERHEAD;

    handle = MailboxP_create(&params);
    if(NULL == handle)
    {
        result = osal_FAILURE;
    }

    if(osal_OK == result)
    {
        handleAddr = handle;
        temp = (uint32_t)(*handleAddr);
        (*handleAddr) = 0U;
        
        /* To make mailbox->used zero to achieve negative case.
        * This tests a scenario when the handle gets corrupted for some reason.
        */
        if(MailboxP_FAILURE != MailboxP_delete(handle))
        {
            result = osal_FAILURE;
        }

        /* Restore the value of the handle. */
        (*handleAddr) = temp;
#if defined (SAFERTOS)
        /* MainlboxP_delete is a dummy call for SafeRTOS. */
        MailboxP_delete(handle);
#elif defined (FREERTOS)
        if(MailboxP_OK != MailboxP_delete(handle))
        {
            result = osal_FAILURE;
        }
#endif
    }

    if(osal_OK == result)
    {
        OSAL_log("\n MailboxP Used test passed!!\n");
    }
    else
    {
        OSAL_log("\n MailboxP Used test failed!!\n");
    }

    return result;
}

int32_t OsalApp_mailboxISRTest(void)
{
    int32_t result = osal_OK;
    uint8_t recvBuf[OSAL_APP_MAX_MB_BUF_LEN];
    uint32_t index;
    HwiP_Params hwipParams;
    HwiP_Handle hHwi;
    MailboxP_Params params;

    MailboxP_Params_init(&params);
    params.count = strlen((char *)gOsalAppStrToSend);
    params.size  = sizeof(uint8_t);
    memset(gOsalAppMbQueueBuf, 0U, sizeof(gOsalAppMbQueueBuf)/sizeof(gOsalAppMbQueueBuf[0]));
    params.buf = gOsalAppMbQueueBuf;
    params.bufsize = params.count*params.size + OSAL_APP_MAILBOX_OVERHEAD;

    gmailboxpHandle = MailboxP_create(&params);
    if(NULL == gmailboxpHandle)
    {
        result = osal_FAILURE;
    }

    HwiP_Params_init(&hwipParams);

    hHwi = HwiP_create(OSAL_APP_IRQ_INT_NUM, (HwiP_Fxn) OsalApp_mbISR, (void *)&hwipParams);
    if(NULL_PTR == hHwi) 
    {
        result = osal_FAILURE;
    }

    if(osal_OK == result)
    {
        HwiP_enableInterrupt(OSAL_APP_IRQ_INT_NUM);

        HwiP_post(OSAL_APP_IRQ_INT_NUM);

        for(index = 0U; index < params.count; index++)
        {
            MailboxP_pend(gmailboxpHandle, &recvBuf[index], MailboxP_WAIT_FOREVER);
        }
        MailboxP_delete(gmailboxpHandle);
    }

    if(0U != memcmp((char *)recvBuf, (char *)gOsalAppStrToSend, params.count))
    {
        result = osal_FAILURE;
    }

    if(NULL != hHwi)
    {
        HwiP_delete( hHwi );
    }
    
    if(osal_OK == result)
    {
        OSAL_log("\n Mailbox Post from ISR test has passed!!\n");
    }
    else
    {
        OSAL_log("\n Mailbox Post from ISR test has failed!!\n");
    }

    return result;
}

int32_t OsalApp_mailboxTests(void)
{
    int32_t result = osal_OK;

    result += OsalApp_mailboxPendPostTest(OSAL_APP_MB_TIMEOUT);
    result += OsalApp_mailboxPendPostTest(0);
    result += OsalApp_mailboxUsedTest();
    result += OsalApp_mailboxISRTest();
    result += OsalApp_mailboxCreateMultipleTest();

    if(osal_OK == result)
    {
        OSAL_log("\n All Mailbox tests have passed!!\n");
    }
    else
    {
        OSAL_log("\n Some or all Mailbox tests have failed!!\n");
    }

    return result;
}

