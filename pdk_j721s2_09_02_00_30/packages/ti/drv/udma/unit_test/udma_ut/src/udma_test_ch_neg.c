/*
 *  Copyright (c) Texas Instruments Incorporated 2023-24
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
 *  \file udma_test_ch_neg.c
 *
 *  \brief File containing negative test cases for UDMA Channel APIs.
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
 * Test scenario 1: NULL check for chHandle
 * Test scenario 2: Check when chInitDone is not UDMA_INIT_DONE
 * Test scenario 3: NULL check for drvHandle
 * Test scenario 4: Check when drvInitDone is not UDMA_INIT_DONE
 */
int32_t UdmaTestChPauseNeg(UdmaTestTaskObj *taskObj)
{
    int32_t           retVal = UDMA_SOK;
    struct Udma_ChObj chObj;
    Udma_ChHandle     chHandle;
    uint32_t          backUpDrvInitDone;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA ChPause negative Testcase ::\r\n",
              taskObj->taskId);

    /* Test scenario 1: NULL check for chHandle */
    retVal = Udma_chPause(NULL);
    if(retVal == UDMA_SOK)
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
                  " |TEST INFO|:: FAIL:: UDMA:: chPause:: Neg:: "
                  " NULL check for chHandle!!\n");
        retVal = UDMA_EFAIL;
    }
    else
    {
        retVal = UDMA_SOK;
    }

    /* Test scenario 2: Check when chInitDone is not UDMA_INIT_DONE */
    if(retVal == UDMA_SOK)
    {
        chHandle             = &chObj;
        chHandle->chInitDone = UDMA_DEINIT_DONE;
        retVal               = Udma_chPause(chHandle);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chPause:: Neg:: "
                      " Check when chInitDone is not UDMA_INIT_DONE!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }
    
    /* Test scenario 3: NULL check for drvHandle */
    if(retVal == UDMA_SOK)
    {
        chHandle->drvHandle  = NULL;
        chHandle->chInitDone = UDMA_INIT_DONE;
        retVal               = Udma_chPause(chHandle);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chPause:: Neg:: "
                      " NULL check for drvHandle!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 4: check when drvInitDone is not UDMA_INIT_DONE */
    if(retVal == UDMA_SOK)
    {
        chHandle->drvHandle              = &taskObj->testObj->drvObj[UDMA_TEST_INST_ID_MAIN_0];
        backUpDrvInitDone                = chHandle->drvHandle->drvInitDone;
        chHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE;
        retVal                           = Udma_chPause(chHandle);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chPause:: Neg:: "
                      " Check when drvInitDone is not UDMA_INIT_DONE!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
        chHandle->drvHandle->drvInitDone = backUpDrvInitDone;
    }

    return retVal;
}

/* 
 * Test Case Description: Verifies the function Udma_chOpen 
 * Test scenario 1: NULL check for drvHandle
 * Test scenario 2: NULL check for chHandle
 * Test scenario 3: NULL check for chPrms
 * Test scenario 4: Check when drvInitDone is not UDMA_INIT_DONE
 * Test scenario 5: Check when Channel resource allocation is failed
 * Test scenario 6: NULL check for utcInfo
 * Test scenario 7: Check when utcId is Invalid for Udma_chCheckParams
 * Test scenario 8: Check when chType is UDMA_CH_FLAG_PDMA and peerChNum 
 *                  is Invalid for Udma_chCheckParams
 * Test scenario 9: Check when chType is UDMA_CH_FLAG_PDMA and peerChNum 
 *                  is UDMA_DMA_CH_NA for Udma_chCheckParams
 * Test scenario 10: Check when chType is UDMA_CH_FLAG_PSIL and peerChNum 
 *                   is UDMA_DMA_CH_INVALID for Udma_chCheckParams
 * Test scenario 11: Check when chType is UDMA_CH_FLAG_PSIL and peerChNum 
 *                   is UDMA_DMA_CH_NA for Udma_chCheckParams
 * Test scenario 12: Check to fail UDMA channel paring
 */
int32_t UdmaTestChOpenNeg(UdmaTestTaskObj *taskObj)
{
    int32_t           retVal  = UDMA_SOK;
    Udma_DrvHandle    drvHandle;
    Udma_ChHandle     chHandle;
    struct Udma_ChObj chObj;
    uint32_t          chType;
    Udma_ChPrms       chPrms;
    uint32_t          backUpDrvInitDone;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA ChOpen negative Testcase ::\r\n",
              taskObj->taskId);

    /* Test scenario 1: NULL check for drvHandle */
    chHandle = &chObj;
    chType   = UDMA_CH_TYPE_TX;
    retVal   = Udma_chOpen(NULL, chHandle, chType, &chPrms);
    if(retVal == UDMA_SOK)
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
                  " |TEST INFO|:: FAIL:: UDMA:: chOpen:: Neg:: "
                  " NULL check for drvHandle!!\n");
        retVal = UDMA_EFAIL;
    }
    else
    {
        retVal = UDMA_SOK;
    }

    /* Test scenario 2: NULL check for chHandle */
    drvHandle = &taskObj->testObj->drvObj[UDMA_TEST_INST_ID_MAIN_0]; 
    if(retVal == UDMA_SOK)
    {
        retVal = Udma_chOpen(drvHandle, NULL, chType, &chPrms);
        if(retVal == UDMA_SOK)
        {  
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chOpen:: Neg:: "
                      " NULL check for chHandle!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 3: NULL check for chPrms */
    if(retVal == UDMA_SOK)
    {
        retVal = Udma_chOpen(drvHandle, chHandle, chType, NULL);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chOpen:: Neg:: "
                      " NULL check for chPrms!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }  
    }

    /* Test scenario 4: Check when drvInitDone is not UDMA_INIT_DONE */
    backUpDrvInitDone = drvHandle->drvInitDone;
    if(retVal == UDMA_SOK)
    {
        drvHandle->drvInitDone = UDMA_DEINIT_DONE;
        retVal                 = Udma_chOpen(drvHandle, chHandle, chType, &chPrms);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chOpen:: Neg:: "
                      " Check when drvInitDone is not UDMA_INIT_DONE!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }
    drvHandle->drvInitDone = backUpDrvInitDone;

    /* Test scenario 5: Check when Channel resource allocation is failed */
    if(retVal == UDMA_SOK)
    {
        chHandle->txChNum = UDMA_DMA_CH_INVALID;
        retVal            = Udma_chOpen(drvHandle, chHandle, chType, &chPrms);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chOpen:: Neg:: "
                      " Check when Channel resource allocation is failed!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 6: NULL check for utcInfo */
    if(retVal == UDMA_SOK)
    {
        chPrms.utcId = (UDMA_UTC_ID_INVALID - 1U);
        chType       = UDMA_CH_TYPE_UTC;
        retVal       = Udma_chOpen(drvHandle, chHandle, chType, &chPrms);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chOpen:: Neg:: "
                      " NULL check for utcInfo!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 7: Check when utcId is Invalid for Udma_chCheckParams */
    if(retVal == UDMA_SOK)
    {
        chPrms.utcId = UDMA_UTC_ID_INVALID;
        chType       = UDMA_CH_TYPE_UTC;
        retVal       = Udma_chOpen(drvHandle, chHandle, chType, &chPrms);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: Udma_chCheckParams:: Neg:: "
                      " check when utcId is Invalid!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

#if (UDMA_NUM_UTC_INSTANCE > 0)
    /* Test scenario 8: Check when chType is UDMA_CH_FLAG_PDMA and 
    *                   peerChNum is Invalid for Udma_chCheckParams 
    */
    if(retVal == UDMA_SOK)
    {
        chPrms.utcId     = UDMA_UTC_ID_MSMC_DRU0;
        chType           = UDMA_CH_FLAG_PDMA;            
        chPrms.peerChNum = UDMA_DMA_CH_INVALID;  
        retVal           = Udma_chOpen(drvHandle, chHandle, chType, &chPrms);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: Udma_chCheckParams:: Neg:: "
                      " when chType is UDMA_CH_FLAG_PDMA and peerChNum is Invalid!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }
           
    /* Test scenario 9: Check when chType is UDMA_CH_FLAG_PDMA and 
    *                   peerChNum is UDMA_DMA_CH_NA for Udma_chCheckParams 
    */
    if(retVal == UDMA_SOK)
    {
        chPrms.utcId     = UDMA_UTC_ID_MSMC_DRU0;
        chType           = UDMA_CH_FLAG_PDMA;   
        chPrms.peerChNum = UDMA_DMA_CH_NA;
        retVal           = Udma_chOpen(drvHandle, chHandle, chType, &chPrms);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: Udma_chCheckParams:: Neg:: "
                      " when chPrms.peerChNum = UDMA_DMA_CH_NA!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }
        
    /* Test scenario 10: Check when chType is UDMA_CH_FLAG_PSIL and 
    *                    peerChNum is UDMA_DMA_CH_INVALID for Udma_chCheckParams 
    */
    if(retVal == UDMA_SOK)
    {
        chPrms.utcId     = UDMA_UTC_ID_MSMC_DRU0;
        chType           = UDMA_CH_FLAG_PSIL;   
        chPrms.peerChNum = UDMA_DMA_CH_INVALID;
        retVal           = Udma_chOpen(drvHandle, chHandle, chType, &chPrms);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: Udma_chCheckParams:: Neg:: when "
                      "chType = UDMA_CH_FLAG_PSIL, chPrms.peerChNum = UDMA_DMA_CH_INVALID!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }
        
    /* Test scenario 11: Check when chType is UDMA_CH_FLAG_PSIL and 
    *                    peerChNum is UDMA_DMA_CH_NA for Udma_chCheckParams 
    */
    if(retVal == UDMA_SOK)
    {
        chPrms.utcId     = UDMA_UTC_ID_MSMC_DRU0;
        chType           = UDMA_CH_FLAG_PSIL;
        chPrms.peerChNum = UDMA_DMA_CH_NA;
        retVal           = Udma_chOpen(drvHandle, chHandle, chType, &chPrms);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: Udma_chCheckParams:: Neg:: "
                      " when chType = UDMA_CH_FLAG_PSIL, chPrms.peerChNum = UDMA_DMA_CH_NA!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        } 
    }
#endif
    drvHandle->drvInitDone = backUpDrvInitDone;

    /* Test scenario 12: Check to fail UDMA channel paring */
    if(retVal == UDMA_SOK)
    {
        chType           = UDMA_CH_TYPE_TX;
        drvHandle        = &taskObj->testObj->drvObj[UDMA_TEST_INST_ID_MAIN_0];
        UdmaChPrms_init(&chPrms, chType);
        chPrms.peerChNum = UDMA_PSIL_CH_MCU_CPSW0_RX;
        retVal           = Udma_chOpen(drvHandle, chHandle, chType, &chPrms);
        if(UDMA_SOK == retVal)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: Udma_chOpen:: Neg:: "
                      " Check to fail UDMA channel paring!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        } 
    }        

    return (retVal);
}

/*
 * Test Case Description: Verifies the function Udma_chClose 
 * Test scenario 1: NULL check for chHandle
 * Test scenario 2: Invalid args check for chInitDone 
 * Test scenario 3: NULL check for drvHandle
 * Test scenario 4: Check when drvInitDone is not UDMA_INIT_DONE 
 * Test scenario 5: UDMA channel Oes de-allocation fail check 
 */
int32_t UdmaTestChCloseNeg(UdmaTestTaskObj *taskObj)
{
    int32_t           retVal = UDMA_SOK;
    Udma_ChHandle     chHandle;
    struct Udma_ChObj chObj;
    uint32_t          backUpDrvInitDone;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA ChClose negative Testcase ::\r\n",
              taskObj->taskId);

    /* Test scenario 1: NULL check for chHandle */
    retVal = Udma_chClose(NULL);
    if(retVal == UDMA_SOK)
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
                  " |TEST INFO|:: FAIL:: UDMA:: chClose:: Neg:: "
                  " NULL check for chHandle!!\n");
        retVal = UDMA_EFAIL;
    }
    else
    {
        retVal = UDMA_SOK;
    }

    /* Test scenario 2: Invalid args check for chInitDone */
    chHandle = &chObj;
    if(retVal == UDMA_SOK)
    {
        chHandle->chInitDone = UDMA_DEINIT_DONE;
        retVal               = Udma_chClose(chHandle);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chClose:: Neg:: "
                      " Invalid args check for chInitDone!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 3: NULL check for drvHandle */
    if(retVal == UDMA_SOK)
    {
        chHandle->chInitDone = UDMA_INIT_DONE;
        chHandle->drvHandle  = NULL;
        retVal               = Udma_chClose(chHandle);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chClose:: Neg:: "
                      " NULL check for drvHandle!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 4: Check when drvInitDone is not UDMA_INIT_DONE */
    chHandle->drvHandle = &taskObj->testObj->drvObj[UDMA_TEST_INST_ID_MAIN_0];
    backUpDrvInitDone   = chHandle->drvHandle->drvInitDone;
    if(retVal == UDMA_SOK)
    {
        chHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE;
        retVal                           = Udma_chClose(chHandle);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chClose:: Neg:: "
                      " Check when drvInitDone is not UDMA_INIT_DONE!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }
    chHandle->drvHandle->drvInitDone = backUpDrvInitDone;

    /* Test scenario 5: UDMA channel Oes de-allocation fail check */
    if(retVal == UDMA_SOK)
    {
        chHandle->drvHandle->drvInitDone = UDMA_INIT_DONE;
        chHandle->chOesAllocDone         = UTRUE;
        retVal                           = Udma_chClose(chHandle);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chClose:: Neg:: "
                      " UDMA channel Oes de-allocation fail check!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }
    chHandle->drvHandle->drvInitDone = backUpDrvInitDone;

    return (retVal);
}

/* 
 * Test Case Description: Verifies the function Udma_chConfigTx
 * Test scenario 1: NULL check for chHandle
 * Test scenario 2: Invalid args check for chInitDone
 * Test scenario 3: INvalid args check for chType
 * Test scenario 4: NULL check for drvHandle 
 * Test scenario 5: Check when drvInitDone is not UDMA_INIT_DONE
 * Test scenario 6: Check when chType is UDMA_CH_TYPE_TR_BLK_COPY 
 *                  and instType is UDMA_INST_TYPE_LCDMA_BCDMA
 */
int32_t UdmaTestChConfigTxNeg(UdmaTestTaskObj *taskObj)
{
    int32_t           retVal = UDMA_SOK;
    Udma_ChTxPrms     txPrms;
    Udma_ChHandle     chHandle;
    uint32_t          backUpDrvInitDone;
    struct Udma_ChObj chObj;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA ChConfigTx negative Testcase ::\r\n",
              taskObj->taskId);

    /* Test scenario 1: NULL check for chHandle */
    retVal = Udma_chConfigTx(NULL, &txPrms);
    if(retVal == UDMA_SOK)
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
                  " |TEST INFO|:: FAIL:: UDMA:: chConfigTx:: Neg:: "
                  " NULL check for chHandle!!\n");
        retVal = UDMA_EFAIL;
    }
    else
    {
        retVal = UDMA_SOK;
    }

    /* Test scenario 2: Invalid args check for chInitDone */
    chHandle = &chObj;
    if(retVal == UDMA_SOK)
    {
        chHandle->chInitDone = UDMA_DEINIT_DONE;
        retVal               = Udma_chConfigTx(chHandle, &txPrms);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chConfigTx:: Neg:: "
                      " Invalid args check for chInitDone!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 3: Invalid args check for chType */
    if(retVal == UDMA_SOK)
    {
        chHandle->chType = UDMA_DEINIT_DONE;
        retVal           = Udma_chConfigTx(chHandle, &txPrms);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chConfigTx:: Neg:: "
                      " INvalid args check for chType!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 4: NULL check for drvHandle */
    if(retVal == UDMA_SOK)
    {
        chHandle->chInitDone = UDMA_INIT_DONE;
        chHandle->chType     = UDMA_CH_FLAG_TX;
        chHandle->drvHandle  = NULL;
        retVal               = Udma_chConfigTx(chHandle, &txPrms);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chConfigTx:: Neg:: "
                      " NULL check for drvHandle!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 5: Check when drvInitDone is not UDMA_INIT_DONE */
    if(retVal == UDMA_SOK)
    {
        chHandle->drvHandle              = &taskObj->testObj->drvObj[UDMA_TEST_INST_ID_MAIN_0];
        backUpDrvInitDone                = chHandle->drvHandle->drvInitDone;
        chHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE;
        retVal                           = Udma_chConfigTx(chHandle, &txPrms);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chConfigTx:: Neg:: "
                      " Check when drvInitDone is not UDMA_INIT_DONE!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
        chHandle->drvHandle->drvInitDone = backUpDrvInitDone;
    }

    /* Test scenario 6: Check when chType is UDMA_CH_TYPE_TR_BLK_COPY and 
    *                   instType is UDMA_INST_TYPE_LCDMA_BCDMA 
    */
    if(retVal == UDMA_SOK)
    {
        chHandle->drvHandle = &taskObj->testObj->drvObj[UDMA_TEST_INST_ID_BCDMA_0];
        chHandle->chType    = UDMA_CH_TYPE_TR_BLK_COPY;
        chHandle->tdCqRing  = (Udma_RingHandle) NULL_PTR;
        retVal              = Udma_chConfigTx(chHandle, &txPrms);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chConfigTx:: Neg:: "
                      " when chType is UDMA_CH_TYPE_TR_BLK_COPY and instType is UDMA_INST_TYPE_LCDMA_BCDMA!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    return (retVal);
}

/* 
 * Test Case Description: Verifies the function Udma_chGetDefaultFlowHandle
 * Test scenario 1: NULL check for chHandle
 * Test scenario 2: Invalid args check for chInitDone
 * Test scenario 3: NULL check for drvHandle 
 * Test scenario 4: Check when drvInitDone is not UDMA_INIT_DONE
 */
int32_t UdmaTestChGetDefaultFlowHandleNeg(UdmaTestTaskObj *taskObj)
{
    int32_t           retVal  = UDMA_SOK;
    Udma_ChHandle     chHandle;
    struct Udma_ChObj chObj;
    uint32_t          backUpDrvInitDone;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA ChGetDefaultFlowHandle negative Testcase ::\r\n",
              taskObj->taskId);

    /* Test scenario 1: NULL check for chHandle */
    if(Udma_chGetDefaultFlowHandle(NULL) != NULL_PTR)
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
                  " |TEST INFO|:: FAIL:: UDMA:: chGetDefaultFlowHandle:: Neg:: "
                  " NULL check for chHandle!!\n");
        retVal = UDMA_EFAIL;
    }
    else
    {
        retVal = UDMA_SOK;
    }

    /* Test scenario 2: Invalid args check for chInitDone */
    chHandle = &chObj;
    if(retVal == UDMA_SOK)
    {
        chHandle->chInitDone = UDMA_DEINIT_DONE;
        if(Udma_chGetDefaultFlowHandle(chHandle) != NULL_PTR)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chGetDefaultFlowHandle:: Neg:: "
                      " Invalid args check for chInitDone!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 3: NULL check for drvHandle */
    if(retVal == UDMA_SOK)
    {
        chHandle->chInitDone = UDMA_INIT_DONE;
        chHandle->drvHandle  = NULL;
        if(Udma_chGetDefaultFlowHandle(chHandle) != NULL_PTR)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chGetDefaultFlowHandle:: Neg:: "
                      " NULL check for drvHandle!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 4: Check when drvInitDone is not UDMA_INIT_DONE */
    if(retVal == UDMA_SOK)
    {
        chHandle->drvHandle              = &taskObj->testObj->drvObj[UDMA_TEST_INST_ID_MAIN_0];
        backUpDrvInitDone                = chHandle->drvHandle->drvInitDone;
        chHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE;
        if(Udma_chGetDefaultFlowHandle(chHandle) != NULL_PTR)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chGetDefaultFlowHandle:: Neg:: "
                      " Check when drvInitDone is not UDMA_INIT_DONE!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
        chHandle->drvHandle->drvInitDone = backUpDrvInitDone;
    }

    return (retVal);
}

/* 
 * Test Case Description: Verifies the function Udma_chGetTdCqRingHandle 
 * Test scenario 1: NULL check for chHandle
 * Test scenario 2: Invalid args check for chInitDone
 * Test scenario 3: NULL check for drvHandle 
 * Test scenario 4: Check when drvInitDone is not UDMA_INIT_DONE
 */
int32_t UdmaTestChGetTdCqRingHandleNeg(UdmaTestTaskObj *taskObj)
{
    int32_t           retVal = UDMA_SOK;
    Udma_ChHandle     chHandle;
    struct Udma_ChObj chObj;
    uint32_t          backUpDrvInitDone;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA ChGetTdCqRingHandle negative Testcase ::\r\n",
              taskObj->taskId);

    /* Test scenario 1: NULL check for chHandle */
    if(Udma_chGetTdCqRingHandle(NULL) != NULL_PTR)
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
                  " |TEST INFO|:: FAIL:: UDMA:: chGetTdCqRingHandle:: Neg:: "
                  " NULL check for chHandle!!\n");
        retVal = UDMA_EFAIL;
    }
    else
    {
        retVal = UDMA_SOK;
    }

    /* Test scenario 2: Invalid args check for chInitDone */
    chHandle = &chObj;
    if(retVal == UDMA_SOK)
    {
        chHandle->chInitDone = UDMA_DEINIT_DONE;
        if(Udma_chGetTdCqRingHandle(chHandle) != NULL_PTR)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chGetTdCqRingHandle:: Neg:: "
                      " Invalid args check for chInitDone!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 3: NULL check for drvHandle */
    if(retVal == UDMA_SOK)
    {
        chHandle->chInitDone = UDMA_INIT_DONE;
        chHandle->drvHandle  = NULL;
        if(Udma_chGetTdCqRingHandle(chHandle) != NULL_PTR)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chGetTdCqRingHandle:: Neg:: "
                      " NULL check for drvHandle!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 4: Check when drvInitDone is not UDMA_INIT_DONE */
    if(retVal == UDMA_SOK)
    {
        chHandle->drvHandle              = &taskObj->testObj->drvObj[UDMA_TEST_INST_ID_MAIN_0];
        backUpDrvInitDone                = chHandle->drvHandle->drvInitDone;
        chHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE;
        if(Udma_chGetTdCqRingHandle(chHandle) != NULL_PTR)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chGetTdCqRingHandle:: Neg:: "
                      " Check when drvInitDone is not UDMA_INIT_DONE!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
        chHandle->drvHandle->drvInitDone = backUpDrvInitDone;
    }
    
    return (retVal);
}

/* 
 * Test Case Description: Verifies the function Udma_chGetFqRingNum
 * Test scenario 1: NULL check for chInitDone
 * Test scenario 2: NULL check for chHandle
 * Test scenario 3: NULL check for drvHandle
 * Test scenario 4: Check when drvInitDone is not UDMA_INIT_DONE
 */
int32_t UdmaTestChGetFqRingNumNeg(UdmaTestTaskObj *taskObj)
{
    int32_t           retVal = UDMA_SOK;
    Udma_ChHandle     chHandle;
    struct Udma_ChObj chObj;
    uint32_t          backUpDrvInitDone;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA ChGetFqRingNum negative Testcase ::\r\n",
              taskObj->taskId);

    /* Test scenario 1: NULL check for chInitDone */
    chHandle             = &chObj;
    chHandle->chInitDone = UDMA_DEINIT_DONE;
    retVal               = Udma_chGetFqRingNum(chHandle);
    if(retVal != UDMA_RING_INVALID)
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
                  " |TEST INFO|:: FAIL:: UDMA:: chGetFqRingHandle:: Neg:: "
                  " NULL check for chInitDone!!\n");
        retVal = UDMA_EFAIL;
    }
    else
    {
        retVal = UDMA_SOK;
    }

    /* Test scenario 2: NULL check for chHandle */
    if(retVal == UDMA_SOK)
    {
        retVal = Udma_chGetFqRingNum(NULL);
        if(retVal != UDMA_RING_INVALID)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chGetFqRingHandle:: Neg:: "
                      " NULL check for chHandle!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 3: NULL check for drvHandle */
    if(retVal == UDMA_SOK)
    {
        chHandle             = &chObj;
        chHandle->chInitDone = UDMA_INIT_DONE;
        chHandle->drvHandle  = NULL;
        retVal               = Udma_chGetFqRingNum(chHandle);
        if(retVal != UDMA_RING_INVALID)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chGetFqRingHandle:: Neg:: "
                      " NULL check for drvHandle!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 4: Check when drvInitDone is not UDMA_INIT_DONE */
    if(retVal == UDMA_SOK)
    {
        chHandle->drvHandle              = &taskObj->testObj->drvObj[UDMA_TEST_INST_ID_MAIN_0];
        backUpDrvInitDone                = chHandle->drvHandle->drvInitDone;
        chHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE;
        retVal                           = Udma_chGetFqRingNum(chHandle);
        if(retVal != UDMA_RING_INVALID)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chGetFqRingHandle:: Neg:: "
                      " Check when drvInitDone is not UDMA_INIT_DONE!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
        chHandle->drvHandle->drvInitDone = backUpDrvInitDone;
    }
    
    return (retVal);
}

/* 
 * Test Case Description: Verifies the function Udma_chGetCqRingNum
 * Test scenario 1: Invalid args check for chInitDone 
 * Test scenario 2: NULL check for chHandle
 * Test scenario 3: NULL check for drvHandle
 * Test scenario 4: Check when drvInitDone is not UDMA_INIT_DONE
 */
int32_t UdmaTestChGetCqRingNumNeg(UdmaTestTaskObj *taskObj)
{
    int32_t           retVal = UDMA_SOK;
    Udma_ChHandle     chHandle;
    struct Udma_ChObj chObj;
    uint32_t          backUpDrvInitDone;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA ChGetCqRingNum negative Testcase ::\r\n",
              taskObj->taskId);

    /* Test scenario 1: Invalid args check for chInitDone */
    chHandle             = &chObj;
    chHandle->chInitDone = UDMA_DEINIT_DONE;
    retVal               = Udma_chGetCqRingNum(chHandle);
    if(retVal != UDMA_RING_INVALID)
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
                  " |TEST INFO|:: FAIL:: UDMA:: Udma_chGetCqRingHandle:: Neg:: "
                  " Invalid args check for chInitDone !!\n");
        retVal = UDMA_EFAIL;
    }
    else
    {
        retVal = UDMA_SOK;
    }

    /* Test scenario 2: NULL check for chHandle */
    if(retVal == UDMA_SOK)
    {
        retVal = Udma_chGetCqRingNum(NULL);
        if(retVal != UDMA_RING_INVALID)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: Udma_chGetCqRingHandle:: Neg:: "
                      " NULL check for chHandle!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 3: NULL check for drvHandle */
    if(retVal == UDMA_SOK)
    {
        chHandle->chInitDone = UDMA_INIT_DONE;
        chHandle->drvHandle  = NULL;
        retVal               = Udma_chGetCqRingNum(chHandle);
        if(retVal != UDMA_RING_INVALID)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: Udma_chGetCqRingHandle:: Neg:: "
                      " NULL check for drvHandle!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 4: Check when drvInitDone is not UDMA_INIT_DONE */
    if(retVal == UDMA_SOK)
    {
        chHandle->drvHandle              = &taskObj->testObj->drvObj[UDMA_TEST_INST_ID_MAIN_0];
        backUpDrvInitDone                = chHandle->drvHandle->drvInitDone;
        chHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE;
        retVal                           = Udma_chGetCqRingNum(chHandle);
        if(retVal != UDMA_RING_INVALID)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: Udma_chGetCqRingHandle:: Neg:: "
                      " Check when drvInitDone is not UDMA_INIT_DONE!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
        chHandle->drvHandle->drvInitDone = backUpDrvInitDone;
    }

    return (retVal);
}

/* 
 * Test Case Description: Verifies the function Udma_chGetTriggerEvent
 * Test scenario 1: NULL check for chHandle
 * Test scenario 2: Invalid args check when chInitDone is not valid
 * Test scenario 3: NULL check for drvHandle
 * Test scenario 4: Check when drvInitDone is not UDMA_INIT_DONE
 * Test scenario 5: NULL check when chType is UDMA_CH_FLAG_PDMA
 */ 
int32_t UdmaTestChGetTriggerEventNeg(UdmaTestTaskObj *taskObj)
{
    int32_t           retVal = UDMA_SOK;
    struct Udma_ChObj chObj;
    Udma_ChHandle     chHandle;
    uint32_t          backUpDrvInitDone;
    uint32_t          trigger;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA ChGetTriggerEvent negative Testcase ::\r\n",
              taskObj->taskId);

    /* Test scenario 1: NULL check for chHandle */
    trigger = CSL_UDMAP_TR_FLAGS_TRIGGER_GLOBAL1;
    retVal  = Udma_chGetTriggerEvent(NULL, trigger);
    if(retVal != UDMA_EVENT_INVALID)
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
                  " |TEST INFO|:: FAIL:: UDMA:: chGetTriggerEvent:: Neg:: "
                  " NULL check for chHandle!!\n");
        retVal = UDMA_EFAIL;
    }
    else
    {
        retVal = UDMA_SOK;
    }

    /* Test scenario 2: Invalid args check when chInitDone is not valid */
    chHandle = &chObj;
    if(retVal == UDMA_SOK)
    {
        chHandle->chInitDone = UDMA_DEINIT_DONE;
        retVal               = Udma_chGetTriggerEvent(chHandle, trigger);
        if(retVal != UDMA_EVENT_INVALID)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chGetTriggerEvent:: Neg:: "
                      " Invalid args check when chInitDone is not valid!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }
    
    /* Test scenario 3: NULL check for drvHandle */
    if(retVal == UDMA_SOK)
    {
        chHandle->drvHandle  = NULL;
        chHandle->chInitDone = UDMA_INIT_DONE;
        retVal               = Udma_chGetTriggerEvent(chHandle, trigger);
        if(retVal != UDMA_EVENT_INVALID)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chGetTriggerEvent:: Neg:: "
                      " NULL check for drvHandle!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 4: Check when drvInitDone is not UDMA_INIT_DONE */
    chHandle->drvHandle              = &taskObj->testObj->drvObj[UDMA_TEST_INST_ID_MAIN_0];
    backUpDrvInitDone                = chHandle->drvHandle->drvInitDone;
    chHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE;
    if(retVal == UDMA_SOK)
    {
        retVal = Udma_chGetTriggerEvent(chHandle, trigger);
        if(retVal != UDMA_EVENT_INVALID)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chGetTriggerEvent:: Neg:: "
                      " Check when drvInitDone is not UDMA_INIT_DONE!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }
    chHandle->drvHandle->drvInitDone = backUpDrvInitDone;

    /* Test scenario 5: NULL check when chType is UDMA_CH_FLAG_PDMA */
    if(retVal == UDMA_SOK)
    {
        chHandle->drvHandle = &taskObj->testObj->drvObj[UDMA_TEST_INST_ID_MAIN_0];
        chHandle->chType    = UDMA_CH_FLAG_PDMA;
        retVal              = Udma_chGetTriggerEvent(chHandle, trigger);
        if(retVal != UDMA_EVENT_INVALID)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chGetTriggerEvent:: Neg:: "
                      " NULL check when chType is UDMA_CH_FLAG_PDMA!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
        chHandle->drvHandle->drvInitDone = backUpDrvInitDone;
    }

    return retVal;
}

/* 
 * Test Case Description: Verifies the function Udma_chSetSwTrigger
 * Test scenario 1: NULL check for chHandle
 * Test scenario 2: Check when chInitDone is not UDMA_INIT_DONE
 * Test scenario 3: Check when trigger > CSL_UDMAP_TR_FLAGS_TRIGGER_LOCAL_EVENT 
 * Test scenario 4: Check when trigger is CSL_UDMAP_TR_FLAGS_TRIGGER_NONE
 * Test scenario 5: NULL check for drvHandle
 * Test scenario 6: Check when drvInitDone is not UDMA_INIT_DONE
 */ 
int32_t UdmaTestChSetSwTriggerNeg(UdmaTestTaskObj *taskObj)
{
    int32_t           retVal = UDMA_SOK;
    struct Udma_ChObj chObj;
    Udma_ChHandle     chHandle;
    uint32_t          backUpDrvInitDone;
    uint32_t          trigger;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA ChSetSwTrigger negative Testcase ::\r\n",
              taskObj->taskId);

    /* Test scenario 1: NULL check for chHandle */
    trigger = CSL_UDMAP_TR_FLAGS_TRIGGER_GLOBAL1;
    retVal  = Udma_chSetSwTrigger(NULL, trigger);
    if(retVal == UDMA_SOK)
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
                  " |TEST INFO|:: FAIL:: UDMA:: chSetSwTrigger:: Neg:: "
                  " NULL check for chHandle!!\n");
        retVal = UDMA_EFAIL;
    }
    else
    {
        retVal = UDMA_SOK;
    }

    /* Test scenario 2: Check when chInitDone is not UDMA_INIT_DONE*/
    if(retVal == UDMA_SOK)
    {
        chHandle             = &chObj;
        chHandle->chInitDone = UDMA_DEINIT_DONE;
        retVal               = Udma_chSetSwTrigger(chHandle, trigger);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chSetSwTrigger:: Neg:: "
                      " Check when chInitDone is not UDMA_INIT_DONE!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }
    
    /* Test scenario 3: Check when trigger > CSL_UDMAP_TR_FLAGS_TRIGGER_LOCAL_EVENT */
    if(retVal == UDMA_SOK)
    {
        trigger              = (CSL_UDMAP_TR_FLAGS_TRIGGER_LOCAL_EVENT+1U);
        chHandle->chInitDone = UDMA_INIT_DONE;
        retVal               = Udma_chSetSwTrigger(chHandle, trigger);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chSetSwTrigger:: Neg:: "
                      " when trigger > CSL_UDMAP_TR_FLAGS_TRIGGER_LOCAL_EVENT!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 4: Check when trigger is CSL_UDMAP_TR_FLAGS_TRIGGER_NONE */
    if(retVal == UDMA_SOK)
    {
        trigger = CSL_UDMAP_TR_FLAGS_TRIGGER_NONE;
        retVal  = Udma_chSetSwTrigger(chHandle, trigger);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chSetSwTrigger:: Neg:: "
                      " when trigger is CSL_UDMAP_TR_FLAGS_TRIGGER_NONE!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 5: NULL check for drvHandle */
    if(retVal == UDMA_SOK)
    {
        chHandle->drvHandle = NULL;
        trigger             = CSL_UDMAP_TR_FLAGS_TRIGGER_GLOBAL1;
        retVal              = Udma_chSetSwTrigger(chHandle, trigger);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chSetSwTrigger:: Neg:: "
                      " NULL check for drvHandle!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 6: Check when drvInitDone is not UDMA_INIT_DONE */
    if(retVal == UDMA_SOK)
    {
        chHandle->drvHandle              = &taskObj->testObj->drvObj[UDMA_TEST_INST_ID_MAIN_0];
        backUpDrvInitDone                = chHandle->drvHandle->drvInitDone;
        trigger                          = CSL_UDMAP_TR_FLAGS_TRIGGER_GLOBAL1;
        chHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE;
        retVal                           = Udma_chSetSwTrigger(chHandle, trigger);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chSetSwTrigger:: Neg:: "
                      " Check when drvInitDone is not UDMA_INIT_DONE!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
        chHandle->drvHandle->drvInitDone = backUpDrvInitDone;
    }

    return retVal;
}

/* 
 * Test Case Description: Verifies the function Udma_chSetChaining 
 * Test scenario 1: NULL check for triggerChHandle
 * Test scenario 2: Check when chInitDone is not UDMA_INIT_DONE for triggerChHandle
 * Test scenario 3: NULL check for chainedChHandle
 * Test scenario 4: Check when chInitDone is not UDMA_INIT_DONE for chainedChHandle
 * Test scenario 5: NULL check for drvHandle 
 * Test scenario 6: Check when drvInitDone is not UDMA_INIT_DONE for triggerChHandle
 * Test scenario 7: check when chOesAllocDone is UTRUE
 * Test scenario 8: check when drvInitDone is not UDMA_INIT_DONE for chainedChHandle
 * Test scenario 9: check when instType is UDMA_INST_TYPE_NORMAL and 
 *                   chType is UDMA_CH_TYPE_RX
 * Test scenario 10: check when instType is UDMA_INST_TYPE_NORMAL and 
 *                   chType is UDMA_CH_TYPE_TX
 * Test scenario 11: check when instType is UDMA_INST_TYPE_NORMAL and 
 *                   chType is UDMA_CH_TYPE_TR_BLK_COPY
 */ 
int32_t UdmaTestChSetChainingNeg(UdmaTestTaskObj *taskObj)
{
    int32_t            retVal = UDMA_SOK;
    struct Udma_ChObj  chObj;
    Udma_ChHandle      triggerChHandle;
    Udma_ChHandle      chainedChHandle;
    uint32_t           trigger;
    uint32_t           instID;
    struct Udma_DrvObj backUpDrvObj;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA ChSetChaining negative Testcase ::\r\n",
              taskObj->taskId);

    /* Test scenario 1: NULL check for triggerChHandle */
    chainedChHandle  = &chObj;
    trigger          = CSL_UDMAP_TR_FLAGS_TRIGGER_GLOBAL1;
    retVal           = Udma_chSetChaining(NULL, chainedChHandle, trigger);
    if(retVal == UDMA_SOK)
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
                  " |TEST INFO|:: FAIL:: UDMA:: chSetChaining:: Neg:: "
                  " NULL check for triggerChHandle!!\n");
        retVal = UDMA_EFAIL;
    }
    else
    {
        retVal = UDMA_SOK;
    }

    /* Test scenario 2: Check when chInitDone is not UDMA_INIT_DONE for triggerChHandle */
    triggerChHandle = &chObj;
    if(retVal == UDMA_SOK)
    {
        triggerChHandle->chInitDone = UDMA_DEINIT_DONE;
        retVal                      = Udma_chSetChaining(triggerChHandle, chainedChHandle, trigger);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chSetChaining:: Neg:: "
                      " When chInitDone is not UDMA_INIT_DONE for triggerChHandle!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }
    
    /* Test scenario 3: NULL check for chainedChHandle */
    if(retVal == UDMA_SOK)
    {
        triggerChHandle->chInitDone = UDMA_INIT_DONE;
        chainedChHandle             = (Udma_ChHandle) NULL_PTR;
        retVal                      = Udma_chSetChaining(triggerChHandle, chainedChHandle, trigger);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chSetChaining:: Neg:: "
                      " NULL check for chainedChHandle!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 4: Check when chInitDone is not UDMA_INIT_DONE for chainedChHandle */
    if(retVal == UDMA_SOK)
    {
        chainedChHandle             = &chObj;
        chainedChHandle->chInitDone = UDMA_DEINIT_DONE;
        retVal                      = Udma_chSetChaining(triggerChHandle, chainedChHandle, trigger);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chSetChaining:: Neg:: "
                      " When chInitDone is not UDMA_INIT_DONE for chainedChHandle!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 5: NULL check for drvHandle */
    triggerChHandle->drvHandle  = NULL;
    chainedChHandle->chInitDone = UDMA_INIT_DONE;
    if(retVal == UDMA_SOK)
    {
        retVal = Udma_chSetChaining(triggerChHandle, chainedChHandle, trigger);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chSetChaining:: Neg:: "
                      " NULL check for drvHandle!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 6: Check when drvInitDone is not UDMA_INIT_DONE for triggerChHandle */
    instID                                  = UDMA_TEST_INST_ID_MAIN_0;
    backUpDrvObj                            = taskObj->testObj->drvObj[instID];
    triggerChHandle->drvHandle              = &taskObj->testObj->drvObj[instID];
    triggerChHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE;
    if(retVal == UDMA_SOK)
    {
        retVal = Udma_chSetChaining(triggerChHandle, chainedChHandle, trigger);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chSetChaining:: Neg:: "
                      " When drvInitDone is not UDMA_INIT_DONE for triggerChHandle!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }
    taskObj->testObj->drvObj[instID] = backUpDrvObj;

    /* Test scenario 7: check when chOesAllocDone is UTRUE */
    triggerChHandle->drvHandle              = &taskObj->testObj->drvObj[instID];
    triggerChHandle->chOesAllocDone         = UTRUE;
    if(retVal == UDMA_SOK)
    {
        retVal = Udma_chSetChaining(triggerChHandle, chainedChHandle, trigger);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chSetChaining:: Neg:: "
                      " check when chOesAllocDone is TRUE!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 8: Check when drvInitDone is not UDMA_INIT_DONE for chainedChHandle */
    chainedChHandle->drvHandle              = &taskObj->testObj->drvObj[instID];
    chainedChHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE;
    triggerChHandle->chOesAllocDone         = UFALSE;
    if(retVal == UDMA_SOK)
    {
        retVal = Udma_chSetChaining(triggerChHandle, chainedChHandle, trigger);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chSetChaining:: Neg:: "
                      " When drvInitDone is not UDMA_INIT_DONE for chainedChHandle!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }
    taskObj->testObj->drvObj[instID] = backUpDrvObj;

    /* Test scenario 9: check when instType is UDMA_INST_TYPE_NORMAL 
    *                   and chType is UDMA_CH_TYPE_RX 
    */
    triggerChHandle->drvHandle = &taskObj->testObj->drvObj[instID];
    triggerChHandle->chType    = UDMA_CH_TYPE_RX;
    if(retVal == UDMA_SOK)
    {
        retVal = Udma_chSetChaining(triggerChHandle, chainedChHandle, trigger);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chSetChaining:: Neg:: "
                      " When instType is UDMA_INST_TYPE_NORMAL and chType is UDMA_CH_TYPE_RX!!\n");
            retVal = UDMA_EFAIL;
        }
        else 
        {
            retVal = UDMA_SOK;
        }
    }
    taskObj->testObj->drvObj[instID] = backUpDrvObj;

    /* Test scenario 10: check when instType is UDMA_INST_TYPE_NORMAL and 
    *                    chType is UDMA_CH_TYPE_TX 
    */
    triggerChHandle->drvHandle = &taskObj->testObj->drvObj[instID];
    triggerChHandle->chType    = UDMA_CH_TYPE_TX;
    if(retVal == UDMA_SOK)
    {
        retVal = Udma_chSetChaining(triggerChHandle, chainedChHandle, trigger);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chSetChaining:: Neg:: "
                      " check when instType is UDMA_INST_TYPE_NORMAL and chType is UDMA_CH_TYPE_TX!!\n");
            retVal = UDMA_EFAIL;
        }
        else 
        {
            retVal = UDMA_SOK;
        }
    }
    taskObj->testObj->drvObj[instID] = backUpDrvObj;

    /* Test scenario 11: check when instType is UDMA_INST_TYPE_NORMAL and 
    *                    chType is UDMA_CH_TYPE_TR_BLK_COPY 
    */
    triggerChHandle->drvHandle = &taskObj->testObj->drvObj[instID];
    triggerChHandle->chType    = UDMA_CH_TYPE_TR_BLK_COPY;
    if(retVal == UDMA_SOK)
    {
        retVal = Udma_chSetChaining(triggerChHandle, chainedChHandle, trigger);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chSetChaining:: Neg:: "
                      " check when instType is UDMA_INST_TYPE_NORMAL and chType is UDMA_CH_TYPE_TR_BLK_COPY!!\n");
            retVal = UDMA_EFAIL;
        }
        else 
        {
            retVal = UDMA_SOK;
        }
    }
    taskObj->testObj->drvObj[instID] = backUpDrvObj;

    return retVal;
}

/* 
 * Test Case Description: Verifies the function Udma_chBreakChaining
 * Test scenario 1: NULL check for triggerChHandle
 * Test scenario 2: Check when chInitDone is not UDMA_INIT_DONE for triggerChHandle
 * Test scenario 3: NULL check for chainedChHandle
 * Test scenario 4: Check when chInitDone is not UDMA_INIT_DONE for chainedChHandle
 * Test scenario 5: NULL check for drvHandle of triggerChHandle
 * Test scenario 6: Check when drvInitDone is not UDMA_INIT_DONE for triggerChHandle
 * Test scenario 7: Check when chOesAllocDone is UFALSE
 * Test scenario 8: Check when drvInitDone is not UDMA_INIT_DONE for chainedChHandle
 * Test scenario 9: Check when instType is UDMA_INST_TYPE_NORMAL and 
 *                  chType is UDMA_CH_TYPE_RX
 * Test scenario 10: Check when instType is UDMA_INST_TYPE_NORMAL and 
 *                   chType is UDMA_CH_TYPE_TX
 * Test scenario 11: Check when instType is UDMA_INST_TYPE_NORMAL and 
 *                   chType is UDMA_CH_TYPE_TR_BLK_COPY
 */ 
int32_t UdmaTestChBreakChainingNeg(UdmaTestTaskObj *taskObj)
{
    int32_t            retVal = UDMA_SOK;
    struct Udma_ChObj  chObj;
    struct Udma_ChObj  chObj1;
    Udma_ChHandle      triggerChHandle;
    Udma_ChHandle      chainedChHandle;
    uint32_t           instID;
    struct Udma_DrvObj backUpDrvObj;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA ChBreakChaining negative Testcase ::\r\n",
              taskObj->taskId);

    /* Test scenario 1: NULL check for triggerChHandle */
    chainedChHandle = &chObj;
    retVal          = Udma_chBreakChaining(NULL, chainedChHandle);
    if(retVal == UDMA_SOK)
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
                  " |TEST INFO|:: FAIL:: UDMA:: chBreakChaining:: Neg:: "
                  " NULL check for triggerChHandle!!\n");
        retVal = UDMA_EFAIL;
    }
    else
    {
        retVal = UDMA_SOK;
    }

    /* Test scenario 2: Check when chInitDone is not UDMA_INIT_DONE for triggerChHandle */
    triggerChHandle             = &chObj1;
    triggerChHandle->chInitDone = UDMA_DEINIT_DONE;
    if(retVal == UDMA_SOK)
    {
        retVal = Udma_chBreakChaining(triggerChHandle, chainedChHandle);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chBreakChaining:: Neg:: "
                      " When chInitDone is not UDMA_INIT_DONE for triggerChHandle!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }
    
    /* Test scenario 3: NULL check for chainedChHandle */
    triggerChHandle->chInitDone = UDMA_INIT_DONE;
    chainedChHandle             = (Udma_ChHandle) NULL_PTR;
    if(retVal == UDMA_SOK)
    {
        retVal = Udma_chBreakChaining(triggerChHandle, chainedChHandle);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chBreakChaining:: Neg:: "
                      " NULL check for chainedChHandle!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 4: Check when chInitDone is not UDMA_INIT_DONE for chainedChHandle */
    chainedChHandle             = &chObj;
    chainedChHandle->chInitDone = UDMA_DEINIT_DONE;
    if(retVal == UDMA_SOK)
    {
        retVal = Udma_chBreakChaining(triggerChHandle, chainedChHandle);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chBreakChaining:: Neg:: "
                      " When chInitDone is not UDMA_INIT_DONE for chainedChHandle!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 5: NULL check for drvHandle of triggerChHandle */
    triggerChHandle->drvHandle  = NULL;
    chainedChHandle->chInitDone = UDMA_INIT_DONE;
    if(retVal == UDMA_SOK)
    {
        retVal = Udma_chBreakChaining(triggerChHandle, chainedChHandle);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chBreakChaining:: Neg:: "
                      " NULL check for drvHandle of triggerChHandle!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 6: Check when drvInitDone is not UDMA_INIT_DONE for triggerChHandle */
    instID                                  = UDMA_TEST_INST_ID_MAIN_0;
    backUpDrvObj                            = taskObj->testObj->drvObj[instID];
    triggerChHandle->drvHandle              = &taskObj->testObj->drvObj[instID];
    triggerChHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE;
    if(retVal == UDMA_SOK)
    {
        retVal = Udma_chBreakChaining(triggerChHandle, chainedChHandle);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chBreakChaining:: Neg:: "
                      " Check when drvInitDone is not UDMA_INIT_DONE for triggerChHandle!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }
    taskObj->testObj->drvObj[instID] = backUpDrvObj;

    /* Test scenario 7: Check when chOesAllocDone is UFALSE */
    triggerChHandle->drvHandle->drvInitDone = UDMA_INIT_DONE;
    triggerChHandle->chOesAllocDone         = UFALSE;
    if(retVal == UDMA_SOK)
    {
        retVal = Udma_chBreakChaining(triggerChHandle, chainedChHandle);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chBreakChaining:: Neg:: "
                      " Check when chOesAllocDone is FALSE!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 8: NULL Check for chainedChHandle->drvHandle */
    triggerChHandle->chOesAllocDone = UTRUE;
    chainedChHandle->drvHandle      = NULL;
    if(retVal == UDMA_SOK)
    {
        retVal = Udma_chBreakChaining(triggerChHandle, chainedChHandle);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chBreakChaining:: Neg:: "
                      " Check when drvInitDone is Invalid for chainedChHandle!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 9: Check when instType is UDMA_INST_TYPE_NORMAL and 
    *                   chType is UDMA_CH_TYPE_RX 
    */
    chainedChHandle->drvHandle = &taskObj->testObj->drvObj[instID];
    triggerChHandle->drvHandle = &taskObj->testObj->drvObj[instID];
    triggerChHandle->trigger   = CSL_UDMAP_TR_FLAGS_TRIGGER_GLOBAL1;
    triggerChHandle->chType    = UDMA_CH_TYPE_RX;
    if(retVal == UDMA_SOK)
    {
        retVal = Udma_chBreakChaining(triggerChHandle, chainedChHandle);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chBreakChaining:: Neg:: "
                      " Check when instType is UDMA_INST_TYPE_NORMAL and "
                      " chType is UDMA_CH_TYPE_RX!!\n");
            retVal = UDMA_EFAIL;
        }
        else 
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 10: Check when instType is UDMA_INST_TYPE_NORMAL and 
    *                    chType is UDMA_CH_TYPE_TX 
    */
    triggerChHandle->drvHandle = &taskObj->testObj->drvObj[instID];
    triggerChHandle->chType    = UDMA_CH_TYPE_TX;
    if(retVal == UDMA_SOK)
    {
        retVal = Udma_chBreakChaining(triggerChHandle, chainedChHandle);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chBreakChaining:: Neg:: "
                      " Check when instType is UDMA_INST_TYPE_NORMAL and "
                      " chType is UDMA_CH_TYPE_TX!!\n");
            retVal = UDMA_EFAIL;
        }
        else 
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 11: Check when instType is UDMA_INST_TYPE_NORMAL and 
    *                    chType is UDMA_CH_TYPE_TR_BLK_COPY 
    */
    triggerChHandle->drvHandle = &taskObj->testObj->drvObj[instID];
    triggerChHandle->chType    = UDMA_CH_TYPE_TR_BLK_COPY;
    if(retVal == UDMA_SOK)
    {
        retVal = Udma_chBreakChaining(triggerChHandle, chainedChHandle);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chBreakChaining:: Neg:: "
                      " Check when instType is UDMA_INST_TYPE_NORMAL and "
                      " chType is UDMA_CH_TYPE_TR_BLK_COPY!!\n");
            retVal = UDMA_EFAIL;
        }
        else 
        {
            retVal = UDMA_SOK;
        }
    }

    return retVal;
}

/* 
 * Test Case Description: Verifies the function Udma_chGetNum
 * Test scenario 1: NULL check for chHandle
 * Test scenario 2: Invalid args check when chInitDone is Invalid
 * Test scenario 3: NULL check for drvHandle
 * Test scenario 4: Check when drvInitDone is not UDMA_INIT_DONE
 */ 
int32_t UdmaTestChGetNumNeg(UdmaTestTaskObj *taskObj)
{
    int32_t           retVal = UDMA_SOK;
    struct Udma_ChObj chObj;
    Udma_ChHandle     chHandle;
    uint32_t          backUpDrvInitDone;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA ChGetNum negative Testcase ::\r\n",
              taskObj->taskId);

    /* Test scenario 1: NULL check for chHandle */
    retVal = Udma_chGetNum(NULL);
    if(retVal != UDMA_DMA_CH_INVALID)
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
                  " |TEST INFO|:: FAIL:: UDMA:: chGetNum:: Neg:: "
                  " NULL check for chHandle!!\n");
        retVal = UDMA_EFAIL;
    }
    else
    {
        retVal = UDMA_SOK;
    }

    /* Test scenario 2: Invalid args check when chInitDone is Invalid */
    chHandle             = &chObj;
    chHandle->chInitDone = UDMA_DEINIT_DONE;
    if(retVal == UDMA_SOK)
    {
        retVal = Udma_chGetNum(chHandle);
        if(retVal != UDMA_DMA_CH_INVALID)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chGetNum:: Neg:: "
                      " Invalid args check when chInitDone is Invalid!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }
    
    /* Test scenario 3: NULL check for drvHandle */
    chHandle->drvHandle  = NULL;
    chHandle->chInitDone = UDMA_INIT_DONE;
    if(retVal == UDMA_SOK)
    {
        retVal = Udma_chGetNum(chHandle);
        if(retVal != UDMA_DMA_CH_INVALID)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chGetNum:: Neg:: "
                      " NULL check for drvHandle!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 4: Check when drvInitDone is not UDMA_INIT_DONE */
    chHandle->drvHandle              = &taskObj->testObj->drvObj[UDMA_TEST_INST_ID_MAIN_0];
    backUpDrvInitDone                = chHandle->drvHandle->drvInitDone;
    chHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE;
    if(retVal == UDMA_SOK)
    {
        retVal = Udma_chGetNum(chHandle);
        if(retVal != UDMA_DMA_CH_INVALID)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chGetNum:: Neg:: "
                      " Check when drvInitDone is not UDMA_INIT_DONE!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }
    chHandle->drvHandle->drvInitDone = backUpDrvInitDone;

    return retVal;
}

/* 
 * Test Case Description: Verifies the function Udma_chConfigUtc 
 * Test scenario 1: NULL check for chHandle
 * Test scenario 2: Invalid args check for chInitDone 
 * Test scenario 3: Invalid args check for chType 
 * Test scenario 4: NULL check for drvHandle
 * Test scenario 5: Check when drvInitDone is not UDMA_INIT_DONE
 */ 
int32_t UdmaTestChConfigUtcTestNeg(UdmaTestTaskObj *taskObj)
{
    int32_t           retVal = UDMA_SOK;
    struct Udma_ChObj chObj;
    Udma_ChHandle     chHandle;
    Udma_ChUtcPrms    utcPrms;
    uint32_t          backUpDrvInitDone;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA ChConfigUtcTest negative Testcase ::\r\n",
              taskObj->taskId);

    /* Test scenario 1: NULL check for chHandle */
    retVal = Udma_chConfigUtc(NULL, &utcPrms);
    if(retVal == UDMA_SOK)
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
                  " |TEST INFO|:: FAIL:: UDMA:: chConfigUtc:: Neg:: "
                  " NULL check for chHandle!!\n");
        retVal = UDMA_EFAIL;
    }
    else
    {
        retVal = UDMA_SOK;
    }

    /* Test scenario 2: Invalid args check for chInitDone */
    chHandle             = &chObj;
    chHandle->chInitDone = UDMA_DEINIT_DONE;
    if(retVal == UDMA_SOK)
    {
        retVal = Udma_chConfigUtc(chHandle, &utcPrms);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chConfigUtc:: Neg:: "
                      " Invalid args check for chInitDone!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 3: Invalid args check for chType */
    chHandle             = &chObj;
    chHandle->chInitDone = UDMA_INIT_DONE;
    chHandle->chType     = UDMA_CH_TYPE_TX;
    if(retVal == UDMA_SOK)
    {
        retVal = Udma_chConfigUtc(chHandle, &utcPrms);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chConfigUtc:: Neg:: "
                      " Invalid args check for chType!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 4: NULL check for drvHandle */
    chHandle->drvHandle = NULL;
    chHandle->chType    = UDMA_CH_TYPE_UTC;
    if(retVal == UDMA_SOK)
    {
        retVal = Udma_chConfigUtc(chHandle, &utcPrms);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chConfigUtc:: Neg:: "
                      " NULL check for drvHandle!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 5: Check when drvInitDone is not UDMA_INIT_DONE */
    chHandle->drvHandle              = &taskObj->testObj->drvObj[UDMA_TEST_INST_ID_MAIN_0];
    backUpDrvInitDone                = chHandle->drvHandle->drvInitDone;
    chHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE;
    if(retVal == UDMA_SOK)
    {
        retVal = Udma_chConfigUtc(chHandle, &utcPrms);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chConfigUtc:: Neg:: "
                      " Check when drvInitDone is not UDMA_INIT_DONE!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }
    chHandle->drvHandle->drvInitDone = backUpDrvInitDone;

    return retVal;
}

/* 
* Test Case Description: Verifies the function Udma_chSetSwTrigger 
* Test scenario 1: Check when chType is UDMA_CH_TYPE_RX
*/
int32_t UdmaTestChSetSwTriggerChTypeNeg(UdmaTestTaskObj *taskObj)
{
    int32_t            retVal = UDMA_SOK;
    struct Udma_DrvObj backUpDrvObj;
    struct Udma_ChObj  chObj;
    Udma_ChHandle      chHandle;
    Udma_ChPrms        chPrms;
    Udma_DrvHandle     drvHandle;
    uint32_t           trigger;
    uint32_t           instID;
    uint32_t           chType;
 
    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA ChSetSwTriggerChType negative Testcase ::\r\n",
              taskObj->taskId);

    /* Test scenario 1: Check when chType is UDMA_CH_TYPE_RX */
    chHandle         = &chObj;
    trigger          = CSL_UDMAP_TR_FLAGS_TRIGGER_GLOBAL1;
    instID           = UDMA_INST_ID_MCU_0;
    chType           = UDMA_CH_TYPE_RX;
    UdmaChPrms_init(&chPrms, chType);
    backUpDrvObj     = taskObj->testObj->drvObj[instID];
    drvHandle        = &taskObj->testObj->drvObj[instID];
    chPrms.peerChNum = UDMA_PSIL_CH_MCU_CPSW0_RX;
    retVal           = Udma_chOpen(drvHandle, chHandle, chType, &chPrms);
    if(retVal == UDMA_SOK)
    {
        retVal = Udma_chSetSwTrigger(chHandle, trigger);
        if(retVal == UDMA_SOK)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chSetSwTrigger:: Neg:: "
                      " Check when chType is UDMA_CH_TYPE_RX!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
            Udma_chClose(chHandle);
        }
    }
    taskObj->testObj->drvObj[instID] = backUpDrvObj;

    return retVal;
}

/* 
 * Test Case Description: Verifies the function Udma_chGetStats when
 * Test scenario 1: Null check for chHandle
 * Test scenario 2: Check when chInitDone is not UDMA_INIT_DONE
 * Test scenario 3: Null check for chStats
 * Test scenario 4: Null check for drvHandle
 * Test scenario 5: Check when drvInitDone is not UDMA_INIT_DONE
 */
int32_t UdmaChGetStatsTestNeg(UdmaTestTaskObj *taskObj)
{
    int32_t            retVal = UDMA_SOK;
    struct Udma_ChObj  chObj;
    Udma_ChHandle      chHandle;
    Udma_ChStats       chStats;
    uint32_t           instID;
    uint32_t           backUpDrvInitDone;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA ChGetStats Negative Testcase ::\r\n",
              taskObj->taskId);
              
    /* Test scenario 1: Null check for chHandle */
    retVal = Udma_chGetStats(NULL, &chStats);
    if(UDMA_SOK == retVal)
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
                  " |TEST INFO|:: FAIL:: UDMA:: chGetStats:: Neg:: "
                  " NULL check for chHandle!!\n");
        retVal = UDMA_EFAIL;
    }
    else
    {
        retVal = UDMA_SOK;
    }

    /* Test scenario 2: Check when chInitDone is not UDMA_INIT_DONE */
    chHandle             = &chObj;
    chHandle->chInitDone = UDMA_DEINIT_DONE;
    if(UDMA_SOK == retVal)
    {
        retVal = Udma_chGetStats(chHandle, &chStats);
        if(UDMA_SOK == retVal)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chGetStats:: Neg:: "
                      " Check when chInitDone is not UDMA_INIT_DONE!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 3: Null check for chStats */
    chHandle->chInitDone = UDMA_INIT_DONE;
    if(UDMA_SOK == retVal)
    {
        retVal = Udma_chGetStats(chHandle, NULL);
        if(UDMA_SOK == retVal)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chGetStats:: Neg:: "
                      " Null check for chStats!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 4: Null check for drvHandle */
    chHandle->drvHandle  = NULL_PTR;
    if(UDMA_SOK == retVal)
    {
        retVal = Udma_chGetStats(chHandle, &chStats);
        if(UDMA_SOK == retVal)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chGetStats:: Neg:: "
                      " Null check for drvHandle!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 5: Check when drvInitDone is not UDMA_INIT_DONE */
    instID = UDMA_TEST_INST_ID_MAIN_0;
    if(UDMA_SOK == retVal)
    {
        chHandle->drvHandle              = &taskObj->testObj->drvObj[instID];
        backUpDrvInitDone                = chHandle->drvHandle->drvInitDone;
        chHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE;
        retVal = Udma_chGetStats(chHandle, &chStats);
        if(UDMA_SOK == retVal)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chGetStats:: Neg:: "
                      " Check when drvInitDone is not UDMA_INIT_DONE!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
        chHandle->drvHandle->drvInitDone = backUpDrvInitDone;
    }

    return (retVal);
}

/* 
 * Test Case Description: Verifies the function Udma_chDisable when
 * Test scenario 1: Null check for chHandle
 * Test scenario 2: Check when chInitDone is not UDMA_INIT_DONE
 * Test scenario 3: Null check for drvHandle
 * Test scenario 4: Check when drvInitDone is not UDMA_INIT_DONE
 */ 
int32_t UdmaChDisableTestNeg(UdmaTestTaskObj *taskObj)
{
    int32_t            retVal = UDMA_SOK;
    struct Udma_ChObj  chObj;
    Udma_ChHandle      chHandle;
    uint32_t           timeout = 0U;
    uint32_t           instID;
    uint32_t           backUpDrvInitDone;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA ChDisable Negative Testcase ::\r\n",
              taskObj->taskId);

    /* Test scenario 1: Null check for chHandle */
    retVal = Udma_chDisable(NULL, timeout);
    if(UDMA_SOK == retVal)
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
                  " |TEST INFO|:: FAIL:: UDMA:: chDisable:: Neg:: "
                  " Null check for chHandle!!\n");
        retVal = UDMA_EFAIL;
    }
    else
    {
        retVal = UDMA_SOK;
    }

    /* Test scenario 2: Check when chInitDone is not UDMA_INIT_DONE */
    chHandle             = &chObj;
    chHandle->chInitDone = UDMA_DEINIT_DONE;
    if(UDMA_SOK == retVal)
    {
        retVal = Udma_chDisable(chHandle, timeout);
        if(UDMA_SOK == retVal)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chDisable:: Neg:: "
                      " Check when chInitDone is not UDMA_INIT_DONE!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 3: Null check for drvHandle */
    chHandle->drvHandle  = NULL_PTR;
    chHandle->chInitDone = UDMA_INIT_DONE;
    if(UDMA_SOK == retVal)
    {
        retVal = Udma_chDisable(chHandle, timeout);
        if(UDMA_SOK == retVal)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chDisable:: Neg:: "
                      " Null check for drvHandle!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 4: Check when drvInitDone is not UDMA_INIT_DONE */
    instID = UDMA_TEST_INST_ID_MAIN_0;
    if(UDMA_SOK == retVal)
    {
        chHandle->drvHandle              = &taskObj->testObj->drvObj[instID];
        backUpDrvInitDone                = chHandle->drvHandle->drvInitDone;
        chHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE;
        retVal = Udma_chDisable(chHandle, timeout);
        if(UDMA_SOK == retVal)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chDisable:: Neg:: "
                      " Check when drvInitDone is not UDMA_INIT_DONE!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
        chHandle->drvHandle->drvInitDone = backUpDrvInitDone;
    }
   
    return retVal;
}

/* 
 * Test Case Description: Verifies the function Udma_chResume when
 * Test scenario 1: Null check for chHandle
 * Test scenario 2: Check when chInitDone is UDMA_INIT_DONE
 * Test scenario 3: NUll check for drvHandle 
 * Test scenario 4: Check when drvInitDone is not UDMA_INIT_DONE
 */ 
int32_t UdmaChResumeTestNeg(UdmaTestTaskObj *taskObj)
{
    int32_t           retVal = UDMA_SOK;
    struct Udma_ChObj chObj;
    Udma_ChHandle     chHandle;
    uint32_t          instID;
    uint32_t          backUpDrvInitDone;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA ChResume Negative Testcase ::\r\n",
              taskObj->taskId);

    /* Test scenario 1: Null check for chHandle */
    retVal = Udma_chResume(NULL);
    if(UDMA_SOK == retVal)
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
                  " |TEST INFO|:: FAIL:: UDMA:: chDisable:: Neg:: "
                  " Null check for chHandle!!\n");
        retVal = UDMA_EFAIL;
    }
    else
    {
        retVal = UDMA_SOK;
    }

    /* Test scenario 2: Check when chInitDone is UDMA_INIT_DONE */
    chHandle             = &chObj;
    chHandle->chInitDone = UDMA_DEINIT_DONE;
    if(UDMA_SOK == retVal)
    {
        retVal = Udma_chResume(chHandle);
        if(UDMA_SOK == retVal)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chDisable:: Neg::"
                      " Check when chInitDone is UDMA_INIT_DONE!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }
    
    /* Test scenario 3: NUll check for drvHandle */
    chHandle->drvHandle  = NULL_PTR;
    chHandle->chInitDone = UDMA_INIT_DONE;
    if(UDMA_SOK == retVal)
    {
        retVal = Udma_chResume(chHandle);
        if(UDMA_SOK == retVal)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chDisable:: Neg::"
                      " NUll check for drvHandle!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 4: Check when drvInitDone is not UDMA_INIT_DONE */
    instID = UDMA_TEST_INST_ID_MAIN_0;
    if(UDMA_SOK == retVal)
    {
        chHandle->drvHandle              = &taskObj->testObj->drvObj[instID];
        backUpDrvInitDone                = chHandle->drvHandle->drvInitDone;
        chHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE;
        retVal = Udma_chResume(chHandle);
        if(UDMA_SOK == retVal)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: chDisable:: Neg::"
                      " Check when drvInitDone is not UDMA_INIT_DONE!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
        chHandle->drvHandle->drvInitDone = backUpDrvInitDone;
    }

    return retVal;
}

/* 
 * Test Case Description: Verifies the function Udma_chConfigPdma when
 * Test scenario 1: Null check for chHandle
 * Test scenario 2: Null check for pdmaPrms
 * Test scenario 3: Check when chInitDone is not UDMA_INIT_DONE
 * Test scenario 4: Check when chType is UDMA_CH_TYPE_TX
 * Test scenario 5: Null check for drvHandle 
 * Test scenario 6: Check when drvInitDone is not UDMA_INIT_DONE
 */ 
int32_t UdmaChConfigPdmaTestNeg(UdmaTestTaskObj *taskObj)
{
    int32_t           retVal = UDMA_SOK;
    struct Udma_ChObj chObj;
    Udma_ChHandle     chHandle;
    Udma_ChPdmaPrms   pdmaPrms;
    uint32_t          instID;
    uint32_t          backUpDrvInitDone;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA ChConfigPdma Negative Testcase ::\r\n",
              taskObj->taskId);

    /* Test scenario 1: Null check for chHandle */ 
    chHandle = (Udma_ChHandle) NULL_PTR;
    retVal = Udma_chConfigPdma(chHandle, &pdmaPrms);
    if(UDMA_SOK == retVal)
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
                  " |TEST INFO|:: FAIL:: UDMA:: ChConfigPdma:: Neg:: "
                  " Null check for chHandle!!\n");
        retVal = UDMA_EFAIL;
    }
    else
    {
        retVal = UDMA_SOK;
    }

    /* Test scenario 2: Null check for pdmaPrms */
    chHandle = &chObj;
    if(UDMA_SOK == retVal)
    {
        retVal = Udma_chConfigPdma(chHandle, NULL_PTR);
        if(UDMA_SOK == retVal)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: ChConfigPdma:: Neg:: "
                      " Null check for pdmaPrms!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 3: Check when chInitDone is not UDMA_INIT_DONE */
    chHandle->chInitDone = UDMA_DEINIT_DONE;
    if(UDMA_SOK == retVal)
    {
        retVal = Udma_chConfigPdma(chHandle, &pdmaPrms);
        if(UDMA_SOK == retVal)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: ChConfigPdma:: Neg:: "
                      " Check when chInitDone is not UDMA_INIT_DONE!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 4: Check when chType is UDMA_CH_TYPE_TX */
    chHandle->chInitDone = UDMA_INIT_DONE;
    chHandle->chType     = UDMA_CH_FLAG_TX;
    if(UDMA_SOK == retVal)
    {
        retVal = Udma_chConfigPdma(chHandle, &pdmaPrms);
        if(UDMA_SOK == retVal)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: ChConfigPdma:: Neg:: "
                      " Check when chInitDone is not UDMA_INIT_DONE!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 5: Null check for drvHandle */
    chHandle->chType     = UDMA_CH_FLAG_PDMA;
    chHandle->drvHandle  = NULL_PTR;
    if(UDMA_SOK == retVal)
    {
        retVal = Udma_chConfigPdma(chHandle, &pdmaPrms);
        if(UDMA_SOK == retVal)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: ChConfigPdma:: Neg:: "
                      " Null check for drvHandle!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 6: Check when drvInitDone is not UDMA_INIT_DONE */
    instID = UDMA_TEST_INST_ID_MAIN_0;
    if(UDMA_SOK == retVal)
    {
        chHandle->drvHandle              = &taskObj->testObj->drvObj[instID];
        backUpDrvInitDone                = chHandle->drvHandle->drvInitDone;
        chHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE;
        retVal                           = Udma_chConfigPdma(chHandle, &pdmaPrms);
        if(UDMA_SOK == retVal)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: ChConfigPdma:: Neg:: "
                      " Null check for drvHandle!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
        chHandle->drvHandle->drvInitDone = backUpDrvInitDone;
    }

    return retVal;
}

/* 
 * Test Case Description: Verifies the function Udma_chConfigRx when
 * Test scenario 1: Null check for chHandle 
 * Test scenario 2: Check when chInitDone is not UDMA_INIT_DONE
 * Test scenario 3: Check when chType is UDMA_CH_TYPE_TX
 * Test scenario 4: Check when drvHandle is Null
 * Test scenario 5: Check when drvInitDone is not UDMA_INIT_DONE
 * Test scenario 6: Check when instType is UDMA_INST_TYPE_NORMAL, chType is UDMA_CH_TYPE_PDMA_RX 
 *                  fqRing and cqRing are Null.
 */ 
int32_t UdmaChConfigRxTestNeg(UdmaTestTaskObj *taskObj)
{
    int32_t             retVal = UDMA_SOK;
    struct Udma_ChObj   chObj;
    Udma_ChHandle       chHandle;
    Udma_ChRxPrms       rxPrms;
    uint32_t            instID;
    uint32_t            backUpDrvInitDone;
    struct Udma_RingObj tdCqRingObj;
    struct Udma_FlowObj defaultFlowObj;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA ChConfigRx Negative Testcase ::\r\n",
              taskObj->taskId);

    /* Test scenario 1: Null check for chHandle */
    retVal = Udma_chConfigRx(NULL, &rxPrms);
    if(UDMA_SOK == retVal)
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
                  " |TEST INFO|:: FAIL:: UDMA:: ChConfigRx:: Neg:: "
                  " Null check for chHandle!!\n");
        retVal = UDMA_EFAIL;
    }
    else
    {
        retVal = UDMA_SOK;
    }

    /* Test scenario 2: Check when chInitDone is not UDMA_INIT_DONE */
    chHandle = &chObj;
    if(UDMA_SOK == retVal)
    {
        chHandle->chInitDone = UDMA_DEINIT_DONE;
        retVal               = Udma_chConfigRx(chHandle, &rxPrms);
        if(UDMA_SOK == retVal)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: ChConfigRx:: Neg:: "
                      " Check when chInitDone is not UDMA_INIT_DONE!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 3: Check when chType is UDMA_CH_TYPE_TX */
    if(UDMA_SOK == retVal)
    {
        chHandle->chInitDone = UDMA_INIT_DONE;
        chHandle->chType     = UDMA_CH_TYPE_TX;
        retVal               = Udma_chConfigRx(chHandle, &rxPrms);
        if(UDMA_SOK == retVal)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: ChConfigRx:: Neg:: "
                      " Check when chInitDone is not UDMA_INIT_DONE!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }
    
    /* Test scenario 4: Check when drvHandle is Null */
    if(UDMA_SOK == retVal)
    {
        chHandle->chType    = UDMA_CH_TYPE_RX;
        chHandle->drvHandle = NULL_PTR;
        retVal = Udma_chConfigRx(chHandle, &rxPrms);
        if(UDMA_SOK == retVal)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: ChConfigRx:: Neg:: "
                      " Check when drvHandle is Null!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 5: Check when drvInitDone is not UDMA_INIT_DONE */
    instID = UDMA_TEST_INST_ID_MAIN_0;
    if(UDMA_SOK == retVal)
    {
        chHandle->drvHandle              = &taskObj->testObj->drvObj[instID];
        backUpDrvInitDone                = chHandle->drvHandle->drvInitDone;
        chHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE;
        retVal                           = Udma_chConfigRx(chHandle, &rxPrms);
        if(UDMA_SOK == retVal)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: ChConfigRx:: Neg:: "
                      " Check when drvInitDone is not UDMA_INIT_DONE!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
        chHandle->drvHandle->drvInitDone = backUpDrvInitDone;
    }

    /* Test scenario 6: Check when instType is UDMA_INST_TYPE_NORMAL, chType is UDMA_CH_TYPE_PDMA_RX 
    *                   fqRing and cqRing are Null.
    */
    if(UDMA_SOK == retVal)
    {
        rxPrms.configDefaultFlow = TRUE;
        chHandle->chType         = UDMA_CH_TYPE_PDMA_RX;
        chHandle->fqRing         = (Udma_RingHandle) NULL_PTR;
        chHandle->cqRing         = (Udma_RingHandle) NULL_PTR;
        chHandle->tdCqRing       = &tdCqRingObj;
        chHandle->defaultFlow    = &defaultFlowObj;
        retVal                   = Udma_chConfigRx(chHandle, &rxPrms);
        if(UDMA_SOK == retVal)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: ChConfigRx:: Neg:: "
                      " when instType is UDMA_INST_TYPE_NORMAL, chType is UDMA_CH_TYPE_PDMA_RX,"
                      " fqRing and cqRing are Null!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    return retVal;
}


/* 
 * Test Case Description: Verifies the function Udma_chEnable when
 * Test scenario 1: Null check for chHandle
 * Test scenario 2: Check when chInitDone is not UDMA_INIT_DONE
 * Test scenario 3: NUll check for drvHandle
 * Test scenario 4: Check when drvInitDone is not UDMA_INIT_DONE
 */ 
int32_t UdmaChEnableTestNeg(UdmaTestTaskObj *taskObj)
{
    int32_t           retVal = UDMA_SOK;
    struct Udma_ChObj chObj;
    Udma_ChHandle     chHandle;
    uint32_t          instID;
    uint32_t          backUpDrvInitDone;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA ChEnable Negative Testcase ::\r\n",
              taskObj->taskId);

    /* Test scenario 1: Null check for chHandle */
    chHandle = (Udma_ChHandle)NULL_PTR;
    retVal = Udma_chEnable(chHandle);
    if(UDMA_SOK == retVal)
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
                  " |TEST INFO|:: FAIL:: UDMA:: ChEnable:: Neg::"
                  " Null check for chHandle!!\n");
        retVal = UDMA_EFAIL;
    }
    else
    {
        retVal = UDMA_SOK;
    }

    /* Test scenario 2: Check when chInitDone is not UDMA_INIT_DONE */
    chHandle             = &chObj;
    chHandle->chInitDone = UDMA_DEINIT_DONE;
    if(UDMA_SOK == retVal)
    {
        retVal = Udma_chEnable(chHandle);
        if(UDMA_SOK == retVal)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: ChEnable:: Neg::"
                      " Check when chInitDone is not UDMA_INIT_DONE!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }
    
    /* Test scenario 3: NUll check for drvHandle */
    chHandle->drvHandle  = NULL_PTR;
    chHandle->chInitDone = UDMA_INIT_DONE;
    if(UDMA_SOK == retVal)
    {
        retVal = Udma_chEnable(chHandle);
        if(UDMA_SOK == retVal)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: ChEnable:: Neg::"
                      " NUll check for drvHandle!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    /* Test scenario 4: Check when drvInitDone is not UDMA_INIT_DONE */
    instID = UDMA_TEST_INST_ID_MAIN_0;
    if(UDMA_SOK == retVal)
    {
        chHandle->drvHandle              = &taskObj->testObj->drvObj[instID];
        backUpDrvInitDone                = chHandle->drvHandle->drvInitDone;
        chHandle->drvHandle->drvInitDone = UDMA_DEINIT_DONE;
        retVal                           = Udma_chEnable(chHandle);
        if(UDMA_SOK == retVal)
        {
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: ChEnable:: Neg::"
                      " Check when drvInitDone is not UDMA_INIT_DONE!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
        chHandle->drvHandle->drvInitDone = backUpDrvInitDone;
    }

    return retVal;
}

