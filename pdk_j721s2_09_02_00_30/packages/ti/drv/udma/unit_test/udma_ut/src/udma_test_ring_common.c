/*
 *  Copyright (c) Texas Instruments Incorporated 2023
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

/**
 *  \file udma_test_ring_common.c
 *
 *  \brief UDMA negative test cases for udma_ring_common.c file.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <udma_test.h>

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

/* None */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/*
 * Test Case Description: Verifies the function Udma_ringAttach when
 * 1)ringHandle is NULL and driverHandle is NULL.
 * 2)driverHandle is NULL.
 * 3)ringHandle is NULL.
 * 4)drvInitDone = UDMA_DEINIT_DONE.
 * 5)ringNum >= maxRings
 */
int32_t UdmaTestRingAttach(UdmaTestTaskObj *taskObj)
{
    int32_t             retVal  = UDMA_SOK;
    uint32_t            instID  = 0U;
    uint16_t            ringNum = UDMA_RING_ANY;
    struct Udma_RingObj ringObj;
    Udma_RingHandle     ringHandle;
    Udma_DrvHandle      drvHandle;

    GT_1trace(taskObj->traceMask, GT_INFO1,
               " |TEST INFO|:: Task:%d: Udma ring attach negative Testcase ::\r\n", taskObj->taskId);

    for(instID = UDMA_INST_ID_UDMAP_START; instID <= UDMA_INST_ID_UDMAP_MAX ; instID++)
    {
        /*drvHandle is NULL and ringHandle is NULL*/
        drvHandle  = (Udma_DrvHandle)NULL_PTR;
        ringHandle = (Udma_RingHandle)NULL_PTR;
        retVal = Udma_ringAttach(drvHandle, ringHandle, ringNum);
        if(UDMA_SOK != retVal)
        {
            retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                " |TEST INFO|:: Test::Udma ring ring attach negative Testcase Failed!!\n");
        }

        /*when drvHandle is null*/
        ringHandle = &ringObj;
        drvHandle  = (Udma_DrvHandle)NULL_PTR;
        retVal = Udma_ringAttach(drvHandle, ringHandle, ringNum);
        if(UDMA_SOK != retVal)
        {
            retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                " |TEST INFO|:: Test::Udma ring attach negative Testcase Failed!!\n");
        }

        /* ringHandle is NULL */
        ringHandle = (Udma_RingHandle)NULL_PTR;
        drvHandle  = &taskObj->testObj->drvObj[instID];
        retVal = Udma_ringAttach(drvHandle, ringHandle, ringNum);
        if(UDMA_SOK != retVal)
        {
           retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                " |TEST INFO|:: Test::ring attach negative Testcase Failed!!\n");
        }

        /* drvInitDone = UDMA_DEINIT_DONE */
        ringHandle             = &ringObj;
        drvHandle              = &taskObj->testObj->drvObj[instID];
        drvHandle->drvInitDone = UDMA_DEINIT_DONE;
        retVal = Udma_ringAttach(drvHandle, ringHandle, ringNum);
        if(UDMA_SOK != retVal)
        {
            retVal = UDMA_SOK;
        }
        else
        {
             GT_0trace(taskObj->traceMask, GT_ERR,
                 " |TEST INFO|:: Test::Udma ring attach negative testcase for UDMA_DEINIT_DONE Failed!!\n");
        }

        /* ringNum >= maxRings */
        drvHandle->drvInitDone = UDMA_INIT_DONE;
        retVal = Udma_ringAttach(drvHandle, ringHandle, ringNum);
        if(UDMA_SOK != retVal)
        {
            retVal = UDMA_SOK;
        }
        else
        {
             GT_0trace(taskObj->traceMask, GT_ERR,
                 " |TEST INFO|:: Test::Udma ring attach negative Testcase when ringNum greater than maxring Failed!!\n");
        }
    }

    return retVal;
}

/*
 * Test Case Description: Verifies the function Udma_ringDetach when
 * 1)ringHandle is NULL.
 * 2)ringInitDone = UDMA_DEINIT_DONE.
 * 3)ringHandle->driverHandle is NULL.
 * 4)ringHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE.
 */
int32_t UdmaTestRingDetach(UdmaTestTaskObj *taskObj)
{
    int32_t             retVal = UDMA_SOK;
    uint32_t            instID = 0U;
    struct Udma_RingObj ringObj;
    Udma_RingHandle     ringHandle;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: Udma ring detach negative Testcase ::\r\n", taskObj->taskId);

    /* ringHandle is NULL */
    ringHandle = (Udma_RingHandle)NULL_PTR;
    retVal = Udma_ringDetach(ringHandle);
    if(UDMA_SOK != retVal)
    {
        retVal = UDMA_SOK;
    }
    else
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
            " |TEST INFO|:: Test::Udma ring detach negative Testcase Failed!!\n");
    }

    /*when ringInitDone = UDMA_DEINIT_DONE*/
    ringHandle               = &ringObj;
    ringHandle->ringInitDone = UDMA_DEINIT_DONE;
    retVal = Udma_ringDetach(ringHandle);
    if(UDMA_SOK != retVal)
    {
        retVal = UDMA_SOK;
    }
    else
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
            " |TEST INFO|:: Test::Udma ring detach negative Testcase Failed!!\n");
    }

    for(instID = UDMA_INST_ID_UDMAP_START; instID <= UDMA_INST_ID_UDMAP_MAX ; instID++)
    {
        /* drvHandle is NULL */
        ringHandle                 = &ringObj;
        ringHandle->ringInitDone   = UDMA_INIT_DONE;
        ringHandle->drvHandle      = (Udma_DrvHandle)NULL_PTR;
        retVal = Udma_ringDetach(ringHandle);
        if(UDMA_SOK != retVal)
        {
            retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                " |TEST INFO|:: Test::Udma ring detach negative Testcase Failed!!\n");
        }

        /* ringHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE */
        ringHandle->drvHandle              = &taskObj->testObj->drvObj[instID];
        ringHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE;
        retVal = Udma_ringDetach(ringHandle);
        if(UDMA_SOK != retVal)
        {
    	retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                " |TEST INFO|:: Test::Udma ring detach negative Testcase Failed!!\n");
        }
        ringHandle->drvHandle->drvInitDone = UDMA_INIT_DONE;
    }

    return retVal;
}

/*
 * Test Case Description: Verifies the function Udma_ringGetNum when
 * 1)ringHandle is NULL.
 * 2)ringInitDone = UDMA_DEINIT_DONE.
 */
int32_t UdmaTestRingGetNum(UdmaTestTaskObj *taskObj)
{
    int32_t             retVal = UDMA_SOK;
    struct Udma_RingObj ringObj;
    Udma_RingHandle     ringHandle;
    uint16_t            ringNum = UDMA_RING_INVALID;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: Udma ring get num negative Testcase ::\r\n", taskObj->taskId);

    /* ringHandle is NULL */
    ringHandle = (Udma_RingHandle)NULL_PTR;
    ringNum = Udma_ringGetNum(ringHandle);
    if(UDMA_RING_INVALID == ringNum)
    {
        retVal = UDMA_SOK;
    }
    else
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
            " |TEST INFO|:: Test::Udma ring get num negative Testcase Failed!!\n");
    }

    /*when ringInitDone = UDMA_DEINIT_DONE*/
    ringHandle               = &ringObj;
    ringHandle->ringInitDone = UDMA_DEINIT_DONE;
    ringNum = Udma_ringGetNum(ringHandle);
    if(UDMA_RING_INVALID == ringNum)
    {
        retVal = UDMA_SOK;
    }
    else
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
            " |TEST INFO|:: Test::Udma ring get num negative Testcase Failed!!\n");
    }

    return retVal;
}

/*
 * Test Case Description: Verifies the function Udma_ringDequeueRaw when
 * 1)ringHandle is NULL.
 * 2)ringInitDone = UDMA_DEINIT_DONE.
 * 3)ringNum = UDMA_RING_INVALID.
 * 4)ringHandle->drvHandle is Null.
 * 5)ringHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE.
 */
int32_t UdmaTestRingDequeueRaw(UdmaTestTaskObj *taskObj)
{
    int32_t             retVal     = UDMA_SOK;
    uint64_t            phyDescMem = 0UL;
    struct Udma_RingObj ringObj;
    Udma_RingHandle     ringHandle;
    uint32_t            instID;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: Udma ring dequeue negative Testcase ::\r\n", taskObj->taskId);

    /* ringHandle is NULL */
    ringHandle = (Udma_RingHandle)NULL_PTR;
    retVal = Udma_ringDequeueRaw(ringHandle, &phyDescMem);
    if(UDMA_SOK != retVal)
    {
        retVal = UDMA_SOK;
    }
    else
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
            " |TEST INFO|:: Test::Udma ring dequeue negative Testcase Failed!!\n");
    }

    /*when ringInitDone = UDMA_DEINIT_DONE*/
    ringHandle               = &ringObj;
    ringHandle->ringInitDone = UDMA_DEINIT_DONE;
    ringHandle->ringNum      = UDMA_RING_ANY;
    retVal = Udma_ringDequeueRaw(ringHandle, &phyDescMem);
    if(UDMA_SOK != retVal)
    {
        retVal = UDMA_SOK;
    }
    else
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
            " |TEST INFO|:: Test::Udma ring dequeue negative Testcase Failed!!\n");
    }

    /*when ringNum != UDMA_RING_INVALID*/
    ringHandle               = &ringObj;
    ringHandle->ringInitDone = UDMA_INIT_DONE;
    ringHandle->ringNum      = UDMA_RING_INVALID;
    retVal = Udma_ringDequeueRaw(ringHandle, &phyDescMem);
    if(UDMA_SOK != retVal)
    {
        retVal = UDMA_SOK;
    }
    else
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
            " |TEST INFO|:: Test::Udma ring dequeue negative Testcase Failed!!\n");
    }

    ringHandle->ringNum = UDMA_RING_ANY;
    for(instID = UDMA_INST_ID_UDMAP_START; instID <= UDMA_INST_ID_UDMAP_MAX ; instID++)
    {
        /* ringHandle->drvHandle is Null */
        ringHandle->drvHandle = (Udma_DrvHandle)NULL_PTR;
        retVal = Udma_ringDequeueRaw(ringHandle, &phyDescMem);
        if(UDMA_SOK != retVal)
        {
            retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                " |TEST INFO|:: Test::Udma ring dequeue negative Failed!!\n");
        }

        /* ringHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE */
        ringHandle->drvHandle              = &taskObj->testObj->drvObj[instID];
        ringHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE;
        retVal = Udma_ringDequeueRaw(ringHandle, &phyDescMem);
        if(UDMA_SOK != retVal)
        {
            retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                " |TEST INFO|:: Test::Udma ring dequeue negative  Failed!!\n");
        }
        ringHandle->drvHandle->drvInitDone = UDMA_INIT_DONE;
    }

    return retVal;
}

/*
 * Test Case Description: Verifies the function Udma_ringQueueRaw when
 * 1)ringHandle is NULL.
 * 2)ringInitDone = UDMA_DEINIT_DONE.
 * 3)ringNum = UDMA_RING_INVALID.
 * 4)ringHandle->drvHandle is Null.
 * 5)ringHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE.
 */
int32_t UdmaTestRingQueueRaw(UdmaTestTaskObj *taskObj)
{
    int32_t             retVal     = UDMA_SOK;
    uint64_t            phyDescMem = 0UL;
    uint32_t            instID     = 0U;
    struct Udma_RingObj ringObj;
    Udma_RingHandle     ringHandle;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: Udma ring Queue negative Testcase ::\r\n", taskObj->taskId);

    /* ringHandle is NULL */
    ringHandle =(Udma_RingHandle)NULL_PTR;
    retVal = Udma_ringQueueRaw(ringHandle, phyDescMem);
    if(UDMA_SOK != retVal)
    {
        retVal = UDMA_SOK;
    }
    else
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
            " |TEST INFO|:: Test::Udma ring Queue negative Testcase Failed!!\n");
    }

    /*when ringInitDone = UDMA_DEINIT_DONE*/
    ringHandle               = &ringObj;
    ringHandle->ringInitDone = UDMA_DEINIT_DONE;
    ringHandle->ringNum      = UDMA_RING_ANY;
    retVal = Udma_ringQueueRaw(ringHandle, phyDescMem);
    if(UDMA_SOK != retVal)
    {
        retVal = UDMA_SOK;
    }
    else
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
            " |TEST INFO|:: Test::Udma ring Queue negative Testcase Failed!!\n");
    }

    /*when ringNum = UDMA_RING_INVALID*/
    ringHandle               = &ringObj;
    ringHandle->ringInitDone = UDMA_INIT_DONE;
    ringHandle->ringNum      = UDMA_RING_INVALID;
    retVal = Udma_ringQueueRaw(ringHandle, phyDescMem);
    if(UDMA_SOK != retVal)
    {
        retVal = UDMA_SOK;
    }
    else
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
            " |TEST INFO|:: Test::Udma ring Queue negative Testcase Failed!!\n");
    }

    ringHandle->ringNum = UDMA_RING_ANY;
    for(instID = UDMA_INST_ID_UDMAP_START; instID <= UDMA_INST_ID_UDMAP_MAX ; instID++)
    {
        /* ringHandle->drvHandle is Null */
        ringHandle->drvHandle = (Udma_DrvHandle)NULL_PTR;
        retVal = Udma_ringQueueRaw(ringHandle, phyDescMem);
        if(UDMA_SOK != retVal)
        {
            retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                " |TEST INFO|:: Test::Udma ring Queue negative Testcase Failed!!\n");
        }

        /* ringHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE */
        ringHandle->drvHandle              = &taskObj->testObj->drvObj[instID];
        ringHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE;
        retVal = Udma_ringQueueRaw(ringHandle, phyDescMem);
        if(UDMA_SOK != retVal)
        {
            retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                " |TEST INFO|:: Test::Udma ring Queue negative Testcase Failed!!\n");
        }
        ringHandle->drvHandle->drvInitDone = UDMA_INIT_DONE;
    }

    return retVal;
}
/*
 * Test Case Description: Verifies the function udmaTestRingFree when
 * 1)ringHandle is NULL
 * 2)ringInitDone = UDMA_DEINIT_DONE
 * 3)ringHandle->drvHandle is Null
 * 4)ringHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE
s */
int32_t udmaTestRingFree(UdmaTestTaskObj *taskObj)
{
    int32_t             retVal = UDMA_SOK;
    uint32_t            instID;
    struct Udma_RingObj ringObj;
    Udma_RingHandle     ringHandle;

    GT_1trace(taskObj->traceMask, GT_INFO1,
            " |TEST INFO|:: Task:%d: Udma ring free Testcase ::\r\n", taskObj->taskId);

    /* ringHandle is NULL */
    ringHandle = (Udma_RingHandle)NULL_PTR;
    retVal     = Udma_ringFree(ringHandle);
    if(UDMA_SOK != retVal)
    {
        retVal = UDMA_SOK;
    }
    else
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
               " |TEST INFO|:: Test::Udma ring free Testcase Failed!!\n");
    }

    /* ringInitDone = UDMA_DEINIT_DONE */
    ringHandle                 = &ringObj;
    ringHandle->ringInitDone   = UDMA_DEINIT_DONE;
    retVal = Udma_ringFree(ringHandle);
    if(UDMA_SOK != retVal)
    {
        retVal = UDMA_SOK;
    }
    else
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
               " |TEST INFO|:: Test::Udma ring free Testcase  Failed!!\n");
    }

    ringHandle->ringInitDone = UDMA_INIT_DONE;
    for(instID = UDMA_INST_ID_UDMAP_START; instID <= UDMA_INST_ID_UDMAP_MAX ; instID++)
    {
	/* ringHandle->drvHandle is Null */
	ringHandle->drvHandle = (Udma_DrvHandle)NULL_PTR;
        retVal = Udma_ringFree(ringHandle);
        if(UDMA_SOK != retVal)
        {
            retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                " |TEST INFO|:: Test::Udma ring free Testcase when driver handle is null Failed!!\n");
        }

        /* ringHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE */
        ringHandle->drvHandle              = &taskObj->testObj->drvObj[instID];
        ringHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE;
        retVal = Udma_ringFree(ringHandle);
        if(UDMA_SOK != retVal)
        {
            retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                " |TEST INFO|:: Test::Udma ring free Testcase when driver is UDMA_DEINIT_DONE Failed!!\n");
         }
         ringHandle->drvHandle->drvInitDone = UDMA_INIT_DONE;
    }

    return retVal;
}

/*
 * Test Case Description: Verifies the function udmaTestRingAlloc when
 * 1)ringHandle,driverHandle and ringPrms are NULL
 * 2)ringHandle and driverHandle are NULL
 * 3)ringHandle and ringPrms are NULL
 * 4)ringHandle is NULL
 * 5)ringPrms and driverHandle are NULL
 * 6)ringHandle and ringPrms are NULL
 * 7)drvHandle is NULL
 * 8)ringPrms is NULL
 */
int32_t udmaTestRingAlloc(UdmaTestTaskObj *taskObj)
{
    int32_t             retVal = UDMA_SOK;
    struct Udma_RingObj ringObj;
    Udma_RingHandle     ringHandle;
    Udma_RingPrms       *ringPrms;
    Udma_DrvHandle      drvHandle;
    uint16_t            ringNum = UDMA_RING_ANY;
    uint32_t            instID  = 0U;


    for(instID = UDMA_INST_ID_UDMAP_START; instID <= UDMA_INST_ID_UDMAP_MAX ; instID++)
    {
	/* ringHandle,driverHandle and ringPrms are NULL */
        drvHandle  = (Udma_DrvHandle)NULL_PTR;
        ringHandle = (Udma_RingHandle)NULL_PTR;
        ringPrms   = (Udma_RingPrms*)NULL_PTR;
        retVal = Udma_ringAlloc(drvHandle, ringHandle, ringNum, ringPrms);
        if(UDMA_SOK != retVal)
        {
            retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                   " |TEST INFO|:: Test::Udma ring alloc Testcase when ringhandle,driverhandle and ringprms are null Failed!!\n");
        }

        /* ringHandle and driverHandle are NULL */
        ringHandle = (Udma_RingHandle)NULL_PTR;
        drvHandle  = (Udma_DrvHandle)NULL_PTR;
        retVal = Udma_ringAlloc(drvHandle, ringHandle, ringNum, ringPrms);
        if(UDMA_SOK != retVal)
        {
            retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                   " |TEST INFO|:: Test::Udma ring alloc Testcase when ringhandle and driverhandle are null Failed!!\n");
        }

        /* ringHandle and ringPrms are NULL */
        ringHandle = (Udma_RingHandle)NULL_PTR;
        drvHandle  = &taskObj->testObj->drvObj[instID];
        ringPrms   = (Udma_RingPrms*)NULL_PTR;
        retVal = Udma_ringAlloc(drvHandle, ringHandle, ringNum, ringPrms);
        if(UDMA_SOK != retVal)
        {
            retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                " |TEST INFO|:: Test::Udma ring alloc Testcase when ringhandle and ringPrms are null Failed!!\n");
        }

        /* ringHandle is NULL */
        ringHandle = (Udma_RingHandle)NULL_PTR;;
        drvHandle  = &taskObj->testObj->drvObj[instID];
        retVal = Udma_ringAlloc(drvHandle, ringHandle, ringNum, ringPrms);
        if(UDMA_SOK != retVal)
        {
            retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                " |TEST INFO|:: Test::Udma ring alloc Testcase when ringhandle is null Failed!!\n");
        }

        /* ringPrms and driverHandle are NULL */
        ringHandle = &ringObj;
        drvHandle  = (Udma_DrvHandle)NULL_PTR;;
        ringPrms   = (Udma_RingPrms*)NULL_PTR;
        retVal = Udma_ringAlloc(drvHandle, ringHandle, ringNum, ringPrms);
        if(UDMA_SOK != retVal)
        {
            retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                " |TEST INFO|:: Test::Udma ring alloc Testcase when driverhandle and ringPrms are null Failed!!\n");
        }

        /* drvHandle is NULL */
        ringHandle = &ringObj;
        drvHandle  = (Udma_DrvHandle)NULL_PTR;
        retVal = Udma_ringAlloc(drvHandle, ringHandle, ringNum, ringPrms);
        if(UDMA_SOK != retVal)
        {
            retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                " |TEST INFO|:: Test::Udma ring alloc Testcase when driverhandle is null Failed!!\n");
        }

        /* ringPrms is NULL */
        ringHandle =  &ringObj;
        drvHandle  = &taskObj->testObj->drvObj[instID];
        ringPrms   = (Udma_RingPrms*)NULL_PTR;
        retVal = Udma_ringAlloc(drvHandle, ringHandle, ringNum, ringPrms);
        if(UDMA_SOK != retVal)
        {
            retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                " |TEST INFO|:: Test::Udma ring alloc Testcase when ringprms is null Failed!!\n");
        }
        drvHandle->drvInitDone = UDMA_INIT_DONE;
    }

    return retVal;
}

/*
 * Test Case Description: Verifies the function Udma_ringFlushRaw when
 * 1)ringHandle is NULL.
 * 2)ringInitDone = UDMA_DEINIT_DONE.
 * 3)ringNum = UDMA_RING_INVALID.
 * 4)ringHandle->drvHandle is Null.
 * 5)ringHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE.
 */
int32_t UdmaTestRingFlushRaw(UdmaTestTaskObj *taskObj)
{
    int32_t             retVal     = UDMA_SOK;
    uint64_t            phyDescMem = 0UL;
    struct Udma_RingObj ringObj;
    Udma_RingHandle     ringHandle;
    uint32_t            instID;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: Udma ring flush raw negative Testcase ::\r\n", taskObj->taskId);

    /* ringHandle is NULL */
    ringHandle = (Udma_RingHandle)NULL_PTR;
    retVal = Udma_ringFlushRaw(ringHandle, &phyDescMem);
    if(UDMA_SOK != retVal)
    {
        retVal = UDMA_SOK;
    }
    else
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
            " |TEST INFO|:: Test::Udma ring flush raw negative Testcase Failed!!\n");
    }

    /*when ringInitDone = UDMA_DEINIT_DONE*/
    ringHandle               = &ringObj;
    ringHandle->ringInitDone = UDMA_DEINIT_DONE;
    ringHandle->ringNum      = UDMA_RING_ANY;
    retVal = Udma_ringFlushRaw(ringHandle, &phyDescMem);
    if(UDMA_SOK != retVal)
    {
        retVal = UDMA_SOK;
    }
    else
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
            " |TEST INFO|:: Test::Udma ring flush raw negative Testcase Failed!!\n");
    }

    /*when ringNum != UDMA_RING_INVALID*/
    ringHandle               = &ringObj;
    ringHandle->ringInitDone = UDMA_INIT_DONE;
    ringHandle->ringNum      = UDMA_RING_INVALID;
    retVal = Udma_ringFlushRaw(ringHandle, &phyDescMem);
    if(UDMA_SOK != retVal)
    {
        retVal = UDMA_SOK;
    }
    else
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
            " |TEST INFO|:: Test::Udma ring flush raw negative Testcase Failed!!\n");
    }

    ringHandle->ringNum = UDMA_RING_ANY;
    for(instID = UDMA_INST_ID_UDMAP_START; instID <= UDMA_INST_ID_UDMAP_MAX ; instID++)
    {
        /* ringHandle->drvHandle is Null */
        ringHandle->drvHandle = (Udma_DrvHandle)NULL_PTR;
        retVal = Udma_ringFlushRaw(ringHandle, &phyDescMem);
        if(UDMA_SOK != retVal)
        {
            retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                " |TEST INFO|:: Test::Udma ring flush raw testcase when drvHandle is null Failed!!\n");
        }

        /* ringHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE */
        ringHandle->drvHandle              = &taskObj->testObj->drvObj[instID];
        ringHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE;
        retVal = Udma_ringFlushRaw(ringHandle, &phyDescMem);
        if(UDMA_SOK != retVal)
        {
            retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                " |TEST INFO|:: Test::Udma ring flush raw testcase when drvInitDone = UDMA_DEINIT_DONE Failed!!\n");
        }
        ringHandle->drvHandle->drvInitDone = UDMA_INIT_DONE;
    }

    return retVal;
}

/*
 * Test Case Description: Verifies the function UdmaRingPrms_init when
 * ringPrms is NULL.
 */
int32_t UdmaTest_RingPrms_init(UdmaTestTaskObj *taskObj)
{
    int32_t  retVal = UDMA_SOK;
    GT_1trace(taskObj->traceMask, GT_INFO1,
               " |TEST INFO|:: Task:%d: UDMA ring prms init negative Testcase ::\r\n", taskObj->taskId);

    /* flowPrms is NULL */
    Udma_RingPrms *ringPrms = (Udma_RingPrms*) NULL_PTR;

    retVal = UdmaRingPrms_init(ringPrms);
    if(UDMA_SOK != retVal)
    {
        retVal = UDMA_SOK;
    }
    else
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
               " |TEST INFO|:: Test:: UDMA ring prms init negative Testcase failed\n");
    }

    return retVal;

}

/*
 * Test Case Description: Verifies the function UdmaRingMonPrms_init when
 * monPrms is NULL.
 */
int32_t UdmaTest_RingMonPrms_init(UdmaTestTaskObj *taskObj)
{
    int32_t  retVal = UDMA_SOK;
    GT_1trace(taskObj->traceMask, GT_INFO1,
               " |TEST INFO|:: Task:%d: UDMA ring prms init negative Testcase ::\r\n", taskObj->taskId);

    /* flowPrms is NULL */
    Udma_RingMonPrms *monPrms = (Udma_RingMonPrms*) NULL_PTR;

    retVal = UdmaRingMonPrms_init(monPrms);
    if(UDMA_SOK != retVal)
    {
        retVal = UDMA_SOK;
    }
    else
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
               " |TEST INFO|:: Test:: UDMA ring Mon prms init negative Testcase failed\n");
    }

    return retVal;

}
