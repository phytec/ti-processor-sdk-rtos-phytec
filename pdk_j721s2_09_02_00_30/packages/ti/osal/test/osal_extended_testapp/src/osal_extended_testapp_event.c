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
 *  \file   osal_extended_testapp_event.c
 *
 *  \brief  OSAL Event Sub Module testcases file.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "osal_extended_test.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define OSAL_APP_INT_NUM_IRQ  (29U)
#if defined (FREERTOS)
#define OSAL_APP_MAX_EVENT    (OSAL_FREERTOS_CONFIGNUM_EVENT)
#elif defined (SAFERTOS)
#define OSAL_APP_MAX_EVENT    (OSAL_SAFERTOS_CONFIGNUM_EVENT)
#endif

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

volatile EventP_Handle gOsalAppISRCtxEventHandle;
volatile uint32_t gOsalAppEventMaskA = EventP_ID_01, gOsalAppEventMaskB = EventP_ID_02, gOsalAppISRisExecuted = 0U;

/* ========================================================================== */
/*                           Function Declarations                            */
/* ========================================================================== */

/*
 * Description  : Event Callback Function
 */
 static void OsalApp_eventIRQ(void *arg);

/*
 * Description  : Test EventP_post and EventP_getPostedEvents APIs from ISR context
 */
static int32_t OsalApp_isInISRCtxEventTest(void);

/*
 * Description: Testing Negative condition for OSAL EvenP APIs
 */
static int32_t OsalApp_eventNegTest(void);

/*
 * Description: Tests creation of maximum supported events.
 */
static int32_t OsalApp_maxEventTest(void);

/* ========================================================================== */
/*                          Internal Function Definitions                     */
/* ========================================================================== */

static void OsalApp_eventIRQ(void *arg)
{
    gOsalAppISRisExecuted = 1U;
    EventP_post(gOsalAppISRCtxEventHandle, gOsalAppEventMaskA);

    if((gOsalAppEventMaskB & EventP_getPostedEvents(gOsalAppISRCtxEventHandle)) != gOsalAppEventMaskB)
    {
        gOsalAppISRisExecuted = 0U;
    }
}

static int32_t OsalApp_isInISRCtxEventTest(void)
{
    uint32_t      interruptNum = OSAL_APP_INT_NUM_IRQ, timeout = 0x10000U;
    uint32_t      unionEvent = gOsalAppEventMaskA | gOsalAppEventMaskB;
    int32_t       result = osal_OK;
    HwiP_Params   hwipParams;
    HwiP_Handle   handle;
    EventP_Params eventParams;

    EventP_Params_init(&eventParams);
    HwiP_Params_init(&hwipParams);
    handle = HwiP_create(interruptNum, (HwiP_Fxn)OsalApp_eventIRQ, &hwipParams);
    gOsalAppISRCtxEventHandle = EventP_create(&eventParams);

    if((NULL_PTR == handle) || (NULL_PTR == gOsalAppISRCtxEventHandle))
    {
        result = osal_FAILURE;
    }

    if(osal_OK == result)
    {
        HwiP_enableInterrupt(interruptNum);

        EventP_post(gOsalAppISRCtxEventHandle, gOsalAppEventMaskB);
        if(HwiP_OK != HwiP_post(interruptNum))
        {
            result = osal_FAILURE;
        }
    }

    if(osal_OK == result)
    {
        /* Wait for software timeout, ISR should hit
        * otherwise return the test as failed */
        while(timeout--)
        {
            if(1U == gOsalAppISRisExecuted)
            {
                break;
            }
        }
        /* Wait is over - did not get any interrupts posted/received
        * declare the test as fail
        */
        if((0U == timeout) || ((unionEvent & EventP_getPostedEvents(gOsalAppISRCtxEventHandle)) != unionEvent))
        {
            result = osal_FAILURE;
        }
    }

    if(NULL_PTR != handle)
    {
        if(HwiP_OK != HwiP_delete(handle))
        {
            result = osal_FAILURE;
        }
    }

    if(NULL_PTR != gOsalAppISRCtxEventHandle)
    {
        if(EventP_OK != EventP_delete((EventP_Handle)&gOsalAppISRCtxEventHandle))
        {
            result = osal_FAILURE;
        }
    }

    if(result != osal_OK)
    {
        OSAL_log("\t ISR context test for Event has failed!! \n");
    }
    
    return result;
}

static int32_t OsalApp_eventNegTest(void)
{
    EventP_Params   params;
    EventP_Handle   eventHandle;
    uint32_t        temp, *handleAddr, eventMask = EventP_ID_01;
    int32_t         result = osal_OK;

    EventP_Params_init(&params);

    eventHandle = EventP_create(&params);
    if(NULL_PTR == eventHandle)
    {
          result = osal_FAILURE;
    }

    /* Corrupt the handle, and set used as 0U, also keep a backup of the handle */
    handleAddr = (uint32_t *)eventHandle;
    temp = (*handleAddr);
    (*handleAddr) = 0U;

    if((EventP_OK == EventP_post(handleAddr, EventP_ID_01)) || (EventP_OK == EventP_post(NULL_PTR, EventP_ID_01)))
    {
        result = osal_FAILURE;
    }
    
    if(0U != EventP_wait(eventHandle, eventMask, EventP_WaitMode_ALL, EventP_WAIT_FOREVER))
    {
        result = osal_FAILURE;
    }
    
    /* Testing Null handle for EventP_wait API */
    if(0U != EventP_wait(NULL_PTR, eventMask, EventP_WaitMode_ALL, EventP_WAIT_FOREVER))
    {
        result = osal_FAILURE;
    }

    /* Pass negative parameters to EventP_getPostedEvents */
    if((0U != EventP_getPostedEvents(eventHandle)) || (0U != EventP_getPostedEvents(NULL_PTR)))
    {
        result = osal_FAILURE;
    }

    if((EventP_FAILURE != EventP_delete(&eventHandle)) || (EventP_FAILURE != EventP_delete(NULL_PTR)))
    {
        result = osal_FAILURE;
    }

    /* Restoring handle */
    (*handleAddr) = temp;
    if(EventP_OK != EventP_delete(&eventHandle))
    {
        result = osal_FAILURE;
    }
#if defined (FREERTOS)
    /* Testing the Allocation Count to be zero
    by making the isused parameter as 1U after deleting the event */
    (*handleAddr) = 1U;
    if(EventP_OK != EventP_delete(&eventHandle))
    {
        result = osal_FAILURE;
    }
#endif

    if(osal_OK != result)
    {
        OSAL_log("\t Negative test for Event has failed!! \n");
    }

    return result;
}

static int32_t OsalApp_maxEventTest(void)
{
    EventP_Params   params;
    EventP_Handle   handles[OSAL_APP_MAX_EVENT];
    uint32_t        eventCnt;
    int32_t         result = osal_OK;


    EventP_Params_init(&params);
    for(eventCnt = 0U; eventCnt < OSAL_APP_MAX_EVENT; eventCnt++)
    {
        handles[eventCnt] = EventP_create(&params);
        if(NULL_PTR == handles[eventCnt])
        {
            result = osal_FAILURE;
            OSAL_log("\t Creation failed at: %d!! \n", eventCnt);
            break;
        }
    }

    if(NULL_PTR != EventP_create(&params))
    {
        result = osal_FAILURE;
    }

    if(osal_OK == result)
    {
        for(eventCnt = 0U; eventCnt < OSAL_APP_MAX_EVENT; eventCnt++)
        {
            if(EventP_OK != EventP_delete(&handles[eventCnt]))
            {
                result = osal_FAILURE;
                OSAL_log("\t Deletion failed at: %d!! \n", eventCnt);
                break;
            }
        }
    }

    if(osal_OK != result)
    {
        OSAL_log("\t Maximum event creation test has failed!! \n");
    }

    return result;
}

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int32_t OsalApp_eventTests(void)
{
    int32_t result = osal_OK;
    
    result += OsalApp_isInISRCtxEventTest();
    result += OsalApp_eventNegTest();
    result += OsalApp_maxEventTest();
    
    if(osal_OK == result)
    {
        OSAL_log("\n All Event tests have passed! \n");
    }
    else
    {
        OSAL_log("\n Some or All Event tests have failed! \n");
    }

    return result;
}

