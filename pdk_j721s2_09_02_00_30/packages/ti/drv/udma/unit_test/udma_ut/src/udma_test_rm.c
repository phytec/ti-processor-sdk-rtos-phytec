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
 *  \file udma_test_rm.c
 *
 *  \brief File containing test cases for UDMA rm APIs.
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
 * Test Case Description: Verifies the function Udma_rmFreeBlkCopyUhcCh when valid args are passed
 */
int32_t UdmaRmFreeBlkCopyUhcChTest(UdmaTestTaskObj *taskObj)
{
    int32_t            retVal = UDMA_SOK;
    Udma_ChHandle      chHandle;
    struct Udma_ChObj  chObj;
    Udma_ChPrms        chPrms;
    uint32_t           chType;
    Udma_DrvHandle     drvHandle;
    uint32_t           instID;
    
    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA RmFreeBlkCopyUhcCh Testcase ::\r\n"
              , taskObj->taskId);

    chType       = UDMA_CH_TYPE_TR_BLK_COPY_UHC;
    UdmaChPrms_init(&chPrms, chType);
    chHandle     = &chObj;
    instID       = UDMA_INST_ID_MAIN_0;
    drvHandle    = &taskObj->testObj->drvObj[instID];
    retVal       = Udma_chOpen(drvHandle, chHandle, chType, &chPrms);
    if(retVal != UDMA_SOK)
    {  
        GT_0trace(taskObj->traceMask, GT_ERR,
                  " |TEST INFO|:: FAIL:: UDMA:: Udma_rmFreeBlkCopyUhcCh:: Pos:: "
                  " valid args check!!\n");
        retVal = UDMA_EFAIL;
    }
    else
    {
        retVal = Udma_chClose(chHandle);
    }

    return retVal;
}

/*
 * Test Case Description: Verifies the function Udma_rmFreeTxHcCh when valid args are passed
 */
int32_t UdmaRmFreeTxHcChTest(UdmaTestTaskObj *taskObj)
{
    int32_t            retVal = UDMA_SOK;
    Udma_ChHandle      chHandle;
    struct Udma_ChObj  chObj;
    Udma_ChPrms        chPrms;
    uint32_t           chType;
    Udma_DrvHandle     drvHandle;
    uint32_t           instID;
    
    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA RmFreeTxHcCh Testcase ::\r\n"
              , taskObj->taskId);

    chType             = UDMA_CH_TYPE_TX_HC;
    UdmaChPrms_init(&chPrms, chType);
    chHandle         = &chObj;
    instID           = UDMA_INST_ID_MAIN_0;
    drvHandle        = &taskObj->testObj->drvObj[instID];
    chPrms.peerChNum = UDMA_PSIL_CH_MCU_CPSW0_TX;
    retVal           = Udma_chOpen(drvHandle, chHandle, chType, &chPrms);

    if(retVal != UDMA_SOK)
    {  
        GT_0trace(taskObj->traceMask, GT_ERR,
                  " |TEST INFO|:: FAIL:: UDMA:: Udma_rmFreeTxHcCh:: Pos:: "
                  " valid args check!!\n");
        retVal = UDMA_EFAIL;
    }
    else
    {
        retVal = Udma_chClose(chHandle);
    }

    return retVal;
}

/*
 * Test Case Description: Verifies the function Udma_rmFreeTxUhcCh when valid args are passed
 */
int32_t UdmaRmFreeTxUhcChTest(UdmaTestTaskObj *taskObj)
{
    int32_t            retVal = UDMA_SOK;
    Udma_ChHandle      chHandle;
    struct Udma_ChObj  chObj;
    Udma_ChPrms        chPrms;
    uint32_t           chType;
    Udma_DrvHandle     drvHandle;
    uint32_t           instID;
    
    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA RmFreeTxUhcCh Testcase ::\r\n"
              , taskObj->taskId);

    chType           = UDMA_CH_TYPE_TX_UHC;
    UdmaChPrms_init(&chPrms, chType);
    chHandle         = &chObj;
    instID           = UDMA_INST_ID_MAIN_0;
    drvHandle        = &taskObj->testObj->drvObj[instID];
    chPrms.peerChNum = UDMA_PSIL_CH_MCU_CPSW0_TX;
    retVal           = Udma_chOpen(drvHandle, chHandle, chType, &chPrms);
    if(retVal != UDMA_SOK)
    {  
        GT_0trace(taskObj->traceMask, GT_ERR,
                  " |TEST INFO|:: FAIL:: UDMA:: Udma_rmFreeTxUhcCh:: Pos:: "
                  " valid args check!!\n");
        retVal = UDMA_EFAIL;
    }
    else
    {
        retVal = Udma_chClose(chHandle);
    }

    return retVal;
}

/*
 * Test Case Description: Verifies the function Udma_rmFreeRxUhcCh when valid args are passed
 */
int32_t UdmaRmFreeRxUhcChTest(UdmaTestTaskObj *taskObj)
{
    int32_t            retVal = UDMA_SOK;
    Udma_ChHandle      chHandle;
    struct Udma_ChObj  chObj;
    Udma_ChPrms        chPrms;
    uint32_t           chType;
    Udma_DrvHandle     drvHandle;
    uint32_t           instID; 
    
    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA RmFreeRxUhcCh Testcase ::\r\n"
              , taskObj->taskId);

    chType             = UDMA_CH_TYPE_RX_UHC;
    UdmaChPrms_init(&chPrms, chType);
    chHandle         = &chObj;
    instID           = UDMA_INST_ID_MAIN_0;
    drvHandle        = &taskObj->testObj->drvObj[instID];
    chPrms.peerChNum = UDMA_PSIL_CH_MCU_CPSW0_RX;
    retVal           = Udma_chOpen(drvHandle, chHandle, chType, &chPrms);
    if(retVal != UDMA_SOK)
    {  
        GT_0trace(taskObj->traceMask, GT_ERR,
                  " |TEST INFO|:: FAIL:: UDMA:: Udma_rmFreeRxUhcCh:: Pos:: "
                  " valid args check!!\n");
        retVal = UDMA_EFAIL;
    }
    else
    {
        retVal = Udma_chClose(chHandle);
    }

    return retVal;
}

/*
 * Test Case Description: Verifies the function UdmaRmInitPrms_init when
 * Test scenario 1: NULL check for rmInitPrms
 * Test scenario 2: Invalid args check for instId 
 */
int32_t UdmaRmInitPrmsinitTestNeg(UdmaTestTaskObj *taskObj)
{
    int32_t         retVal = UDMA_SOK;
    uint32_t        instId;
    Udma_RmInitPrms rmInitPrms;

    GT_1trace(taskObj->traceMask, GT_INFO1,
              " |TEST INFO|:: Task:%d: UDMA RmInitPrmsinit negative Testcase ::\r\n"
              , taskObj->taskId);

    /*Test scenario 1: NULL check for rmInitPrms*/
    instId = UDMA_TEST_INST_ID_MAIN_0;
    retVal = UdmaRmInitPrms_init(instId, NULL);
    if(retVal == UDMA_SOK)
    {  
        GT_0trace(taskObj->traceMask, GT_ERR,
                  " |TEST INFO|:: FAIL:: UDMA:: RmInitPrms_init:: Neg:: "
                  " valid args check!!\n");
        retVal = UDMA_EFAIL;
    }
    else
    {
        retVal = UDMA_SOK;
    }

    if(retVal == UDMA_SOK)
    {  
        /*Test scenario 2: Invalid args check for instId*/
        instId = UDMA_INST_ID_MAX + 1U;
        retVal = UdmaRmInitPrms_init(instId, &rmInitPrms);
        if(retVal == UDMA_SOK)
        {  
            GT_0trace(taskObj->traceMask, GT_ERR,
                      " |TEST INFO|:: FAIL:: UDMA:: RmInitPrms_init:: Neg:: "
                      " Invalid args check for instId!!\n");
            retVal = UDMA_EFAIL;
        }
        else
        {
            retVal = UDMA_SOK;
        }
    }

    return retVal;
}

