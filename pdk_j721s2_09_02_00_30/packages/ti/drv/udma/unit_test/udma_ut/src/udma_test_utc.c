/*
 *  Copyright (c) Texas Instruments Incorporated 2024
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
 *  \file udma_test_utc.c
 *
 *  \brief File containing test cases for UDMA UTC related APIs.
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
 * Test Case Description: Verifies the function Udma_chPause
 * Test scenario 1: Check to fail DRU channel pause
 */
int32_t UdmaTestChPauseDruNeg(UdmaTestTaskObj *taskObj)
{
    int32_t            retVal = UDMA_SOK;
    struct Udma_ChObj  chObj;
    Udma_ChHandle      chHandle;
    struct Udma_DrvObj backUpDrvObj;
    Udma_DrvHandle     drvHandle;
    Udma_ChPrms        chPrms;
    uint32_t           chType;
    uint32_t           timeout;
    struct Udma_ChObj  backUpChObj;
  
    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA ChPauseDru negative Testcase ::\r\n",
              taskObj->taskId);

    /* Test scenario 1: Check to fail DRU channel pause */
    timeout          = 0U;
    chHandle         = &chObj;
    chType           = UDMA_CH_TYPE_UTC;
    backUpDrvObj     = taskObj->testObj->drvObj[UDMA_TEST_INST_ID_MAIN_0];           
    UdmaChPrms_init(&chPrms, chType);
    chPrms.utcId     = UDMA_UTC_ID_MSMC_DRU0;
    drvHandle        = &taskObj->testObj->drvObj[UDMA_TEST_INST_ID_MAIN_0];
    retVal           = Udma_chOpen(drvHandle, chHandle, chType, &chPrms);
    Udma_ChUtcPrms utcPrms;
    UdmaChUtcPrms_init(&utcPrms);
    if(retVal == UDMA_SOK)
    {
        retVal = Udma_chConfigUtc(chHandle, &utcPrms);
        if(retVal == UDMA_SOK)
        {
            retVal = Udma_chEnable(chHandle);
            if(retVal == UDMA_SOK)
            {
                backUpChObj                = chObj;
                chHandle->utcPrms.druOwner = CSL_DRU_OWNER_DIRECT_TR;
                chHandle->extChNum         = CSL_DRU_NUM_CH;
                retVal = Udma_chPause(chHandle);
                if(retVal == UDMA_SOK)
                {
                    GT_0trace(taskObj->traceMask, GT_ERR,
                              " |TEST INFO|:: FAIL:: UDMA:: chPause:: Neg:: "
                              " Check to fail DRU channel pause!!\n");
                    retVal = UDMA_EFAIL;
                }
                else
                {
                    retVal = UDMA_SOK;
                    chObj  = backUpChObj;
                    Udma_chDisable(chHandle, timeout);
                    Udma_chClose(chHandle); 
                }
            }
        }
    }
    taskObj->testObj->drvObj[UDMA_TEST_INST_ID_MAIN_0] = backUpDrvObj;

    return retVal;
}

/* 
* Test Case Description: Verifies the function Udma_chDisable 
* Test scenario 1: Check when chType is UDMA_CH_TYPE_UTC and 
                   druOwner is CSL_DRU_OWNER_DIRECT_TR
*/
int32_t UdmaTestChDisableDruNeg(UdmaTestTaskObj *taskObj)
{
    int32_t            retVal = UDMA_SOK;
    struct Udma_ChObj  chObj;
    struct Udma_DrvObj backUpDrvObj; 
    Udma_ChHandle      chHandle;
    Udma_ChPrms        chPrms;
    Udma_DrvHandle     drvHandle;
    uint32_t           timeout;
    uint32_t           instID;
    uint32_t           chType;
    uint32_t           bkupExtCh;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA ChDisableDru negative Testcase ::\r\n",
              taskObj->taskId);

    /* Test scenario 1: Check when chType is UDMA_CH_TYPE_UTC and 
    *                   druOwner is CSL_DRU_OWNER_DIRECT_TR 
    */
    timeout      = 0U;
    chHandle     = &chObj;
    instID       = UDMA_TEST_INST_ID_MAIN_0;
    chType       = UDMA_CH_TYPE_UTC;
    UdmaChPrms_init(&chPrms, chType);
    backUpDrvObj = taskObj->testObj->drvObj[instID];
    chPrms.utcId = UDMA_UTC_ID_MSMC_DRU0;
    drvHandle    = &taskObj->testObj->drvObj[instID];
    retVal       = Udma_chOpen(drvHandle, chHandle, chType, &chPrms);
    Udma_ChUtcPrms utcPrms;
    UdmaChUtcPrms_init(&utcPrms);
    utcPrms.druOwner = CSL_DRU_OWNER_DIRECT_TR;
    if(retVal == UDMA_SOK)
    {
        retVal = Udma_chConfigUtc(chHandle, &utcPrms);
        if(retVal == UDMA_SOK)
        {
            retVal = Udma_chEnable(chHandle);
            if(retVal == UDMA_SOK)
            {
                bkupExtCh          = chHandle->extChNum;
                chHandle->extChNum = CSL_DRU_NUM_CH;
                retVal             = Udma_chDisable(chHandle, timeout);
                if(retVal == UDMA_SOK)
                {
                    GT_0trace(taskObj->traceMask, GT_ERR,
                              " |TEST INFO|:: FAIL:: UDMA:: chDisable:: Pos:: "
                              " Check when chType is UDMA_CH_TYPE_UTC and "
                              " druOwner is CSL_DRU_OWNER_DIRECT_TR*/!!\n");
                    retVal = UDMA_EFAIL;
                }
                else
                {
                    retVal             = UDMA_SOK;
                    chHandle->extChNum = bkupExtCh;
                    Udma_chClose(chHandle);
                }
            }
        }
    }
    taskObj->testObj->drvObj[instID] = backUpDrvObj;

    return retVal;
}

/* 
 * Test Case Description: Verifies the function Udma_chConfigUtc 
 * Test scenario 1: Check when druOwner is CSL_DRU_OWNER_UDMAC_TR
 */ 
int32_t UdmaTestChConfigUtcDruTestNeg(UdmaTestTaskObj *taskObj)
{
    int32_t            retVal = UDMA_SOK;
    struct Udma_ChObj  chObj;
    Udma_ChHandle      chHandle;
    Udma_ChUtcPrms     utcPrms;
    Udma_ChPrms        chPrms;
    Udma_DrvHandle     drvHandle;
    struct Udma_DrvObj backUpDrvObj;
    uint32_t           chType;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA ChConfigUtcDru negative Testcase ::\r\n",
              taskObj->taskId);

    /* Test scenario 1: Check when druOwner is CSL_DRU_OWNER_UDMAC_TR */
    chHandle        = &chObj;
    backUpDrvObj    = taskObj->testObj->drvObj[UDMA_TEST_INST_ID_MAIN_0];
    chType          = UDMA_CH_TYPE_UTC;
    drvHandle       = &taskObj->testObj->drvObj[UDMA_TEST_INST_ID_MAIN_0];
    UdmaChPrms_init(&chPrms, chType);
    chPrms.utcId    = UDMA_UTC_ID_MSMC_DRU0;
    retVal          = Udma_chOpen(drvHandle, chHandle, chType, &chPrms);
    if(retVal == UDMA_SOK)
    {
        utcPrms.druOwner = CSL_DRU_OWNER_UDMAC_TR;
        retVal           = Udma_chConfigUtc(chHandle, &utcPrms);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chConfigUtc:: Neg:: "
                      " Check when druOwner is CSL_DRU_OWNER_UDMAC_TR!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
            Udma_chClose(chHandle);
        }
    }
    taskObj->testObj->drvObj[UDMA_TEST_INST_ID_MAIN_0] = backUpDrvObj;

    return retVal;
}

/* 
 * Test Case Description: Verifies the function Udma_chSetSwTrigger
 * Test scenario 1: check when chType is UDMA_CH_TYPE_UTC and 
 *                  trigger is CSL_UDMAP_TR_FLAGS_TRIGGER_GLOBAL1
 */ 
int32_t UdmaTestChSetSwTriggerDruNeg(UdmaTestTaskObj *taskObj)
{
    int32_t            retVal = UDMA_SOK;
    struct Udma_ChObj  chObj;
    Udma_ChHandle      chHandle;
    Udma_ChPrms        chPrms;
    uint32_t           chType; 
    Udma_DrvHandle     drvHandle;
    struct Udma_DrvObj backUpDrvObj;
    uint32_t           trigger;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA ChSetSwTriggerDru negative Testcase ::\r\n",
              taskObj->taskId);

    /* Test scenario 1: check when chType is UDMA_CH_TYPE_UTC and 
    *                   trigger is CSL_UDMAP_TR_FLAGS_TRIGGER_GLOBAL1 
    */
    chHandle     = &chObj;
    backUpDrvObj = taskObj->testObj->drvObj[UDMA_TEST_INST_ID_MAIN_0];
    chType       = UDMA_CH_TYPE_UTC;
    UdmaChPrms_init(&chPrms, chType);
    chPrms.utcId = UDMA_UTC_ID_MSMC_DRU0;
    drvHandle    = &taskObj->testObj->drvObj[UDMA_TEST_INST_ID_MAIN_0];
    retVal       = Udma_chOpen(drvHandle, chHandle, chType, &chPrms);
    if(retVal == UDMA_SOK)
    {
        trigger = CSL_UDMAP_TR_FLAGS_TRIGGER_GLOBAL1;
        retVal  = Udma_chSetSwTrigger(chHandle, trigger);
        if(retVal == UDMA_EFAIL)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                        " |TEST INFO|:: FAIL:: UDMA:: chSetSwTrigger:: Neg::"
                        " When chType is UDMA_CH_TYPE_UTC and trigger is "
                        " CSL_UDMAP_TR_FLAGS_TRIGGER_GLOBAL1!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
            Udma_chClose(chHandle);
        } 
    }
    taskObj->testObj->drvObj[UDMA_TEST_INST_ID_MAIN_0] = backUpDrvObj;

    return retVal;
}

/* Test Case Description: Verifies the function Udma_druGetNumQueue when
 * 1)Test scenario 1: Check when driver handle is NULL.
 * 2)Test scenario 2: Check when drvInitDone is UDMA_DEINIT_DONE.
 * 3)Test scenario 3: Check to get error print message as Invalid UTC ID.
 */
int32_t UdmaTestDruGetNumQueueNeg(UdmaTestTaskObj *taskObj)
{
    int32_t        retVal = UDMA_SOK;
    uint32_t       utcId  = 0U;
    Udma_DrvHandle drvHandle;
    uint32_t       numQueue;
    uint32_t       BackupdrvInitDone;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA DRU get Num negative Testcase ::\r\n",
              taskObj->taskId);

    /* Test scenario 1: Check when driver handle is NULL */
    drvHandle = (Udma_DrvHandle) NULL_PTR;
    numQueue  = Udma_druGetNumQueue(drvHandle, utcId);
    if(0U == numQueue)
    {
        retVal = UDMA_SOK;
    }
    else
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
                  " |TEST INFO|:: FAIL:: UDMA:: druGetNumQueue :: Neg::"
                  " Check when driver handle is NULL!!\n");
        retVal = UDMA_EFAIL;
    }

    if(retVal == UDMA_SOK)
    {
        /* Test scenario 2: Check when drvInitDone is UDMA_DEINIT_DONE */
        drvHandle              = &taskObj->testObj->drvObj[UDMA_INST_ID_MAIN_0];
        BackupdrvInitDone      = drvHandle->drvInitDone;
        drvHandle->drvInitDone = UDMA_DEINIT_DONE;
        numQueue               = Udma_druGetNumQueue(drvHandle, utcId);
        if(0U == numQueue)
        {
            retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: druGetNumQueue :: Neg::"
                      " Check when drvInitDone is UDMA_DEINIT_DONE\n");
            retVal = UDMA_EFAIL;
        }
        drvHandle->drvInitDone = BackupdrvInitDone;
    }

    if(retVal == UDMA_SOK)
    {
        /* Test scenario 3: Check to get error print message as Invalid UTC ID */
        utcId    = UDMA_NUM_UTC_INSTANCE + 1U;
        numQueue = Udma_druGetNumQueue(drvHandle, utcId);
        if(0U == numQueue)
        {
            retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: druGetNumQueue :: Neg::"
                      " Check to get error print message as Invalid UTC ID!!\n");
            retVal = UDMA_EFAIL;
        }
    }

    return retVal;
}

/*
 * Test Case Description: Verifies the function Udma_druQueueConfig when
 * 1)Test scenario 1: Check when driver handle and queueCfg are NULL.
 * 2)Test scenario 2: Check when driver handle is NULL.
 * 3)Test scenario 3: Check when drvInitDone is UDMA_DEINIT_DONE and
 *                    queueCfg is NULL.
 * 4)Test scenario 4: Check when drvInitDone is UDMA_DEINIT_DONE.
 * 5)Test scenario 5: Check when queueCfg is NULL.
 * 6)Test scenario 6: Check to get error print message as Invalid UTC ID.
 * 7)Test scenario 7: Check to get error print message DRU queue config failed.
 */
int32_t UdmaTestDruQueueConfigNeg(UdmaTestTaskObj *taskObj)
{
    int32_t            retVal   = UDMA_SOK;
    uint32_t           utcId    = 0U;
    uint32_t           queId    = 1U;
    uint32_t           DruUtcId = 0U;
    Udma_DrvHandle     drvHandle;
    uint32_t           BackupdrvInitDone;
    CSL_DruQueueConfig DruqueueCfg;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA DRU Queue Config negative Testcase ::\r\n"
              , taskObj->taskId);

    /* Test scenario 1: Check when driver handle and queueCfg are NULL */
    drvHandle = (Udma_DrvHandle) NULL_PTR;
    retVal    = Udma_druQueueConfig(drvHandle, utcId, queId, NULL);
    if(UDMA_SOK != retVal)
    {
        retVal = UDMA_SOK;
    }
    else
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
                  " |TEST INFO|:: FAIL:: UDMA:: druQueueConfig :: Neg:: "
                  " Check when driver handle and queueCfg are NULL!!\n");
        retVal = UDMA_EFAIL;
    }

    if(retVal == UDMA_SOK)
    {
        /* Test scenario 2: Check when driver handle is NULL */
        drvHandle = (Udma_DrvHandle) NULL_PTR;
        retVal    = Udma_druQueueConfig(drvHandle, utcId, queId, &DruqueueCfg);
        if(UDMA_SOK != retVal)
        {
            retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|::FAIL:: UDMA:: druQueueConfig :: Neg:: "
                      " Check when driver handle is NULL!!\n");
            retVal = UDMA_EFAIL;
        }
    }

    if(retVal == UDMA_SOK)
    {
        /* Test scenario 3: Check when drvInitDone is UDMA_DEINIT_DONE and
         *                  queueCfg is NULL
         */
        drvHandle              = &taskObj->testObj->drvObj[UDMA_INST_ID_MAIN_0];
        BackupdrvInitDone      = drvHandle->drvInitDone;
        drvHandle->drvInitDone = UDMA_DEINIT_DONE;
        retVal                 = Udma_druQueueConfig(drvHandle, utcId, queId, NULL);
        if(UDMA_SOK != retVal)
        {
            retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|::FAIL:: UDMA:: druQueueConfig :: Neg::"
                      " Check for drvInitDone is UDMA_DEINIT_DONE and "
                      " queueCfg is NULL!!\n");
            retVal = UDMA_EFAIL;
        }
        drvHandle->drvInitDone = BackupdrvInitDone;
    }

    if(retVal == UDMA_SOK)
    {
        /* Test scenario 4: Check for drvInitDone is UDMA_DEINIT_DONE */
        drvHandle              = &taskObj->testObj->drvObj[UDMA_INST_ID_MAIN_0];
        BackupdrvInitDone      = drvHandle->drvInitDone;
        drvHandle->drvInitDone = UDMA_DEINIT_DONE;
        retVal                 = Udma_druQueueConfig(drvHandle, utcId, queId, &DruqueueCfg);
        if(UDMA_SOK != retVal)
        {
            retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|::FAIL:: UDMA:: druQueueConfig :: Neg::"
                      " Check for drvInitDone is UDMA_DEINIT_DONE!!\n");
            retVal = UDMA_EFAIL;
        }
        drvHandle->drvInitDone = BackupdrvInitDone;
    }

    if(retVal == UDMA_SOK)
    {
        /* Test scenario 5: Check when queueCfg is NULL */
        retVal = Udma_druQueueConfig(drvHandle, utcId, queId, NULL);
        if(UDMA_SOK != retVal)
        {
            retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|::FAIL:: UDMA:: druQueueConfig :: Neg::"
                      " Check when queueCfg is NULL!!\n");
            retVal = UDMA_EFAIL;
        }
    }

    if(retVal == UDMA_SOK)
    {
        /* Test scenario 6: Check to get error print message as Invalid UTC ID */
        UdmaDruQueueConfig_init(&DruqueueCfg);
        utcId  = UDMA_NUM_UTC_INSTANCE + 1U;
        retVal = Udma_druQueueConfig(drvHandle, utcId, queId, &DruqueueCfg);
        if(UDMA_SOK != retVal)
        {
            retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|::FAIL:: UDMA:: druQueueConfig :: Neg::"
                      " Check to get error print message as Invalid UTC ID !!\n");
            retVal = UDMA_EFAIL;
        }
    }

    if(retVal == UDMA_SOK)
    {
        /* Test scenario 7: Check to get error print message DRU queue config failed */
        queId  = CSL_DRU_NUM_QUEUE + 1U;
        retVal = Udma_druQueueConfig(drvHandle, DruUtcId, queId, &DruqueueCfg);
        if(UDMA_SOK != retVal)
        {
            retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|::FAIL:: UDMA:: druQueueConfig :: Neg::"
                      " Check to get error print message DRU queue config failed!!\n");
            retVal = UDMA_EFAIL;
        }
    }

    return retVal;
}

/*
 * Test Case Description: Verifies the function Udma_druGetTriggerRegAddr when
 * 1)Test scenario 1: Check when Channel handle is NULL.
 * 2)Test scenario 2: Check when chInitDone is not UDMA_DEINIT_DONE
 *                    and chType is not UDMA_CH_FLAG_UTC.
 * 3)Test scenario 3: Check when chInitDone is UDMA_DEINIT_DONE.
 * 4)Test scenario 4: Check when chType is not UDMA_CH_FLAG_UTC.
 * 5)Test scenario 5: Check when driver handle is NULL.
 * 6)Test scenario 6: Check when drvInitDone is UDMA_DEINIT_DONE.
 */
int32_t UdmaTestDruGetTriggerRegAddrNeg(UdmaTestTaskObj *taskObj)
{
    int32_t            retVal     = UDMA_SOK;
    volatile uint64_t *pSwTrigReg = (volatile uint64_t *) NULL_PTR;
    struct Udma_ChObj  chObj;
    Udma_ChHandle      channelHandle;
    uint32_t           BackupdrvInitDone;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA DRU Get Trigger Reg Addr negative Testcase ::\r\n",
              taskObj->taskId);

    /* Test scenario 1: Check when Channel handle is NULL */
    channelHandle = (Udma_ChHandle) NULL_PTR;
    pSwTrigReg    = Udma_druGetTriggerRegAddr(channelHandle);
    if(NULL_PTR == pSwTrigReg)
    {
        retVal = UDMA_SOK;
    }
    else
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
                  " |TEST INFO|::FAIL:: UDMA:: druGetTriggerRegAddr :: Neg:: "
                  " Check when Channel handle is NULL!!\n");
        retVal = UDMA_EFAIL;
    }

    if(retVal == UDMA_SOK)
    {
        /* Test scenario 2: Check when chInitDone is not UDMA_DEINIT_DONE
         *                  and chType is not UDMA_CH_FLAG_UTC
         */
        channelHandle             = &chObj;
        channelHandle->chInitDone = UDMA_DEINIT_DONE;
        channelHandle->chType     = UDMA_CH_FLAG_BLK_COPY;
        pSwTrigReg                = Udma_druGetTriggerRegAddr(channelHandle);
        if(NULL_PTR == pSwTrigReg)
        {
            retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|::FAIL:: UDMA:: druGetTriggerRegAddr :: Neg:: "
                      " Check when chInitDone is not UDMA_DEINIT_DONE and "
                      " chType is not UDMA_CH_FLAG_UTC!!\n");
            retVal = UDMA_EFAIL;
        }
    }

    if(retVal == UDMA_SOK)
    {
        /* Test scenario 3: Check when chInitDone is UDMA_DEINIT_DONE */
        channelHandle->chInitDone = UDMA_DEINIT_DONE;
        channelHandle->chType     = UDMA_CH_FLAG_UTC;
        pSwTrigReg                = Udma_druGetTriggerRegAddr(channelHandle);
        if(NULL_PTR == pSwTrigReg)
        {
            retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|::FAIL:: UDMA:: druGetTriggerRegAddr :: Neg:: "
                      " Check when chInitDone is UDMA_DEINIT_DONE!!\n");
            retVal = UDMA_EFAIL;
        }
    }

    if(retVal == UDMA_SOK)
    {
        /* Test scenario 4: Check when chType is not UDMA_CH_FLAG_UTC */
        channelHandle->chInitDone = UDMA_INIT_DONE;
        channelHandle->chType     = UDMA_CH_FLAG_BLK_COPY;
        pSwTrigReg                = Udma_druGetTriggerRegAddr(channelHandle);
        if(NULL_PTR == pSwTrigReg)
        {
            retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|::FAIL:: UDMA:: druGetTriggerRegAddr :: Neg:: "
                      " Check for chType when it is not equal to UDMA_CH_FLAG_UTC!!\n");
            retVal = UDMA_EFAIL;
        }
    }

    if(retVal == UDMA_SOK)
    {
        /* Test scenario 5: Check when driver handle is NULL */
        channelHandle->chType    = UDMA_CH_TYPE_UTC;
        channelHandle->drvHandle = NULL_PTR;
        pSwTrigReg               = Udma_druGetTriggerRegAddr(channelHandle);
        if(NULL_PTR == pSwTrigReg)
        {
            retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|::FAIL:: UDMA:: druGetTriggerRegAddr :: Neg:: "
                      " Check when driver handle is NULL!!\n");
            retVal = UDMA_EFAIL;
        }
    }

    if(retVal == UDMA_SOK)
    {
        /* Test scenario 6: Check when drvInitDone is UDMA_DEINIT_DONE */
        channelHandle->drvHandle              = &taskObj->testObj->drvObj[UDMA_INST_ID_MAIN_0];
        BackupdrvInitDone                     = channelHandle->drvHandle->drvInitDone;
        channelHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE;
        pSwTrigReg                            = Udma_druGetTriggerRegAddr(channelHandle);
        if(NULL_PTR == pSwTrigReg)
        {
            retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|::FAIL:: UDMA:: druGetTriggerRegAddr :: Neg:: "
                      " Check when drvInitDone is UDMA_DEINIT_DONE!!\n");
            retVal = UDMA_EFAIL;
        }
        channelHandle->drvHandle->drvInitDone = BackupdrvInitDone;
    }

    return retVal;
}

/*
 * Test Case Description: Verifies the function Udma_druGetTriggerRegAddr when
 * 1)Test scenario 1: Check when utcType is UDMA_UTC_TYPE_DRU
 */
int32_t UdmaTestDruGetTriggerRegAddr(UdmaTestTaskObj *taskObj)
{
    int32_t            retVal     = UDMA_SOK;
    volatile uint64_t *pSwTrigReg = (volatile uint64_t *) NULL_PTR;
    struct Udma_ChObj  chObj;
    Udma_ChHandle      channelHandle;
    uint32_t           chType;
    Udma_ChPrms        chPrms;
    Udma_DrvHandle     drvHandle;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA DRU Get Trigger Reg Addr positive Testcase ::\r\n",
              taskObj->taskId);

    /* Test scenario 1: Check when utcType is UDMA_UTC_TYPE_DRU */
    channelHandle = &chObj;
    chType        = UDMA_CH_TYPE_UTC;
    UdmaChPrms_init(&chPrms, chType);
    drvHandle     = &taskObj->testObj->drvObj[UDMA_INST_ID_MAIN_0];
    chPrms.utcId  = UDMA_UTC_ID_MSMC_DRU0;
    retVal        = Udma_chOpen(drvHandle, channelHandle, chType, &chPrms);
    if(UDMA_SOK != retVal)
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
                  " |TEST INFO|::FAIL:: UDMA:: druGetTriggerRegAddr :: "
                  " Channel open failed!!\n");
        retVal = UDMA_EFAIL;
    }

    if(UDMA_SOK == retVal)
    {
        pSwTrigReg = Udma_druGetTriggerRegAddr(channelHandle);
        if(NULL_PTR != pSwTrigReg)
        {
            retVal = UDMA_SOK;
        }
        else
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|::FAIL:: UDMA:: druGetTriggerRegAddr :: "
                      " Check for utcType is UDMA_UTC_TYPE_DRU!!\n");
            retVal = UDMA_EFAIL;
        }
    }
    Udma_chClose(channelHandle);

    return retVal;
}

