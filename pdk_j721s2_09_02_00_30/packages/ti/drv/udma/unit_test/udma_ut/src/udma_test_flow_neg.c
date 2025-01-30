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
 *  \file udma_test_flow_neg.c
 *
 *  \brief UDMA negative test cases for udma_flow.c test case file.
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
 * Test Case Description: Verifies the function Udma_flowGetNum when
 * 1)flowHandle is NULL.
 * 2)flowInitDone = UDMA_DEINIT_DONE.
 */
int32_t UdmaTest_flowGetNum(UdmaTestTaskObj *taskObj)
{
    int32_t     retVal = UDMA_SOK;
    uint32_t    flowNum = UDMA_FLOW_INVALID;

    struct Udma_FlowObj flowObj;
    Udma_FlowHandle flowHandle;
    GT_1trace(taskObj->traceMask, GT_INFO1,
                " |TEST INFO|:: Task:%d: UDMA Flow get Num negative Testcase ::\r\n", taskObj->taskId);

    /* flowHandle is NULL */
    flowHandle = NULL_PTR;
    flowNum = Udma_flowGetNum(flowHandle);
    if(UDMA_FLOW_INVALID == flowNum)
    {
        retVal = UDMA_SOK;
    }
    else
    {
       GT_0trace(taskObj->traceMask, GT_ERR," |TEST INFO|::UDMA Flow get Num negative Testcase!!\n");
    }

     /* flowInitDone = UDMA_DEINIT_DONE */
     flowHandle               = &flowObj;
     flowHandle->flowInitDone = UDMA_DEINIT_DONE;
     flowNum = Udma_flowGetNum(flowHandle);
     if(UDMA_FLOW_INVALID == flowNum)
     {
         retVal = UDMA_SOK;
     }
     else
     {
         GT_0trace(taskObj->traceMask, GT_ERR,
                 " |TEST INFO|:: Test:: UDMA Flow get Num negative Testcase Failed!!\n");
     }
     return retVal;
}

/*
 * Test Case Description: Verifies the function Udma_flowGetCount when
 * 1)flowHandle is NULL.
 * 2)flowInitDone = UDMA_DEINIT_DONE.
 */
int32_t UdmaTest_flowGetCount(UdmaTestTaskObj *taskObj)
{
    int32_t     retVal = UDMA_SOK;
    uint32_t    flowCnt = UDMA_FLOW_INVALID;
    struct Udma_FlowObj flowObj;
    Udma_FlowHandle flowHandle;

    GT_1trace(taskObj->traceMask, GT_INFO1,
            " |TEST INFO|:: Task:%d: UDMA Flow get count negative Testcase ::\r\n", taskObj->taskId);

    /* flowHandle is NULL */
    flowHandle =  NULL_PTR;
    flowCnt = Udma_flowGetCount(flowHandle);
    if(UDMA_FLOW_INVALID == flowCnt)
    {
        retVal = UDMA_SOK;
    }
    else
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
                " |TEST INFO|:: Test:: UDMA Flow get count negative Testcase Failed!!\n");
    }

    /* flowInitDone = UDMA_DEINIT_DONE */
    flowHandle               = &flowObj;
    flowHandle->flowInitDone = UDMA_DEINIT_DONE;
    flowCnt = Udma_flowGetCount(flowHandle);
    if(UDMA_FLOW_INVALID == flowCnt)
    {
        retVal = UDMA_SOK;
    }
    else
    {
        GT_0trace(taskObj->traceMask, GT_ERR,
                " |TEST INFO|:: Test:: UDMA Flow get count negative Testcase Failed!!\n");
    }

    return retVal;
}

/*
  * Test Case Description: Verifies the function Udma_flowFree when
  * flowHandle is NULL.
  */
 int32_t UdmaTest_flowFree(UdmaTestTaskObj *taskObj)
 {
     int32_t   retVal = UDMA_SOK;
     Udma_FlowHandle flowHandle;

     GT_1trace(taskObj->traceMask, GT_INFO1,
               " |TEST INFO|:: Task:%d: UDMA Flow free negative Testcase ::\r\n", taskObj->taskId);

     /* flowHandle is NULL */
     flowHandle = (Udma_FlowHandle)NULL_PTR;
     retVal = Udma_flowFree(flowHandle);
     if(UDMA_SOK != retVal)
     {
         retVal = UDMA_SOK;
     }
     else
     {
         GT_0trace(taskObj->traceMask, GT_ERR,
                " |TEST INFO|:: Test:: UDMA Flow free negative Testcase failed\n");
     }

     return retVal;
 }

 /*
  * Test Case Description: Verifies the function UdmaFlowPrms_init when
  * flowPrms is NULL.
  */
 int32_t UdmaTest_FlowPrms_init(UdmaTestTaskObj *taskObj)
 {
     int32_t  retVal = UDMA_SOK;
     uint32_t chType = UDMA_CH_TYPE_RX;
     GT_1trace(taskObj->traceMask, GT_INFO1,
                " |TEST INFO|:: Task:%d: UDMA Flow prms init negative Testcase ::\r\n", taskObj->taskId);

     /* flowPrms is NULL */
     Udma_FlowPrms *flowPrms = (Udma_FlowPrms*) NULL_PTR;

     retVal = UdmaFlowPrms_init(flowPrms, chType);
     if(UDMA_SOK != retVal)
     {
         retVal = UDMA_SOK;
     }
     else
     {
         GT_0trace(taskObj->traceMask, GT_ERR,
                " |TEST INFO|:: Test:: UDMA Flow free negative Testcase failed\n");
     }

     return retVal;

 }
