/*
*
* Copyright (c) 2024 Texas Instruments Incorporated
*
* All rights reserved not granted herein.
*
* Limited License.
*
* Texas Instruments Incorporated grants a world-wide, royalty-free, non-exclusive
* license under copyrights and patents it now or hereafter owns or controls to make,
* have made, use, import, offer to sell and sell ("Utilize") this software subject to the
* terms herein.  With respect to the foregoing patent license, such license is granted
* solely to the extent that any such patent is necessary to Utilize the software alone.
* The patent license shall not apply to any combinations which include this software,
* other than combinations with devices manufactured by or for TI ("TI Devices").
* No hardware patent is licensed hereunder.
*
* Redistributions must preserve existing copyright notices and reproduce this license
* (including the above copyright notice and the disclaimer and (if applicable) source
* code license limitations below) in the documentation and/or other materials provided
* with the distribution
*
* Redistribution and use in binary form, without modification, are permitted provided
* that the following conditions are met:
*
* *       No reverse engineering, decompilation, or disassembly of this software is
* permitted with respect to any software provided in binary form.
*
* *       any redistribution and use are licensed by TI for use only with TI Devices.
*
* *       Nothing shall obligate TI to provide you with source code for the software
* licensed and provided to you in object code.
*
* If software source code is provided to you, modification and redistribution of the
* source code are permitted provided that the following conditions are met:
*
* *       any redistribution and use of the source code, including any resulting derivative
* works, are licensed by TI for use only with TI Devices.
*
* *       any redistribution and use of any object code compiled from the source code
* and any resulting derivative works, are licensed by TI for use only with TI Devices.
*
* Neither the name of Texas Instruments Incorporated nor the names of its suppliers
*
* may be used to endorse or promote products derived from this software without
* specific prior written permission.
*
* DISCLAIMER.
*
* THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
* OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

/**
 *  \file     Eth_Irq.c
 *
 *  \brief    This file contains the ISR implementation of the Ethernet
 *            driver.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "Eth.h"
#include "Eth_Irq.h"
#if (STD_ON == ETH_ENABLE_MII_API)
#include "EthTrcv.h"
#endif
#if (STD_ON == ETH_DEV_ERROR_DETECT)
#include "Det.h"
#endif
#include "Dem.h"
#include "Eth_Priv.h"
#include "cpsw/Cpsw_Dma.h"
#include "Eth_IrqPacing.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

#define  ETH_START_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Eth_MemMap.h"

extern volatile Eth_MiiAccessType Eth_MiiAccess[ETH_CTRL_ID_MAX];

#define  ETH_STOP_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Eth_MemMap.h"

/* ========================================================================== */
/*                  Internal Function Declaration                             */
/* ========================================================================== */

#define ETH_START_SEC_ISR_CODE
#include "Eth_MemMap.h"
static FUNC(void, ETH_CODE_FAST) Eth_RxIrqHdlr(uint8 ctrlIdx);
static FUNC(void, ETH_CODE_FAST) Eth_TxIrqHdlr(uint8 ctrlIdx);
static FUNC(void, ETH_CODE_FAST) Eth_RxIrqPacingHdlr(uint8 ctrlIdx);
static FUNC(void, ETH_CODE_FAST) Eth_TxIrqPacingHdlr(uint8 ctrlIdx);
#define ETH_STOP_SEC_ISR_CODE
#include "Eth_MemMap.h"

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
#define ETH_START_SEC_ISR_CODE
#include "Eth_MemMap.h"

/* Design : ETH_DesignId_017 */
/*
 * Requirements : MCAL-1604, MCAL-1605, MCAL-1606, MCAL-1607,
 *                MCAL-1608
 */
#if (ETH_ISR_TYPE == ETH_ISR_CAT1 || ETH_ISR_TYPE == ETH_ISR_VOID)
FUNC(void, ETH_CODE_FAST) Eth_RxIrqHdlr_0(void)
#else
ISR(Eth_RxIrqHdlr_0)
#endif
{
    Eth_RxIrqHdlr((uint8)ETH_CTRL_ID_0);
}

/* Design : ETH_DesignId_018 */
/*
 * Requirements : MCAL-1609, MCAL-1610, MCAL-1611, MCAL-1612,
 *                MCAL-1613
 */

#if (ETH_ISR_TYPE == ETH_ISR_CAT1 || ETH_ISR_TYPE == ETH_ISR_VOID)
FUNC(void, ETH_CODE_FAST) Eth_TxIrqHdlr_0(void)
#else
ISR(Eth_TxIrqHdlr_0)
#endif
{
    Eth_TxIrqHdlr((uint8)ETH_CTRL_ID_0);
}

#if (ETH_ISR_TYPE == ETH_ISR_CAT1 || ETH_ISR_TYPE == ETH_ISR_VOID)
FUNC(void, ETH_CODE_FAST) Eth_RxIrqPacingHdlr_0(void)
#else
ISR(Eth_RxIrqPacingHdlr_0)
#endif
{
    Eth_RxIrqPacingHdlr((uint8)ETH_CTRL_ID_0);
}

#if (ETH_ISR_TYPE == ETH_ISR_CAT1 || ETH_ISR_TYPE == ETH_ISR_VOID)
FUNC(void, ETH_CODE_FAST) Eth_TxIrqPacingHdlr_0(void)
#else
ISR(Eth_TxIrqPacingHdlr_0)
#endif
{
    Eth_TxIrqPacingHdlr((uint8)ETH_CTRL_ID_0);
}

#if (1U < ETH_CTRL_ID_MAX)
#if (ETH_ISR_TYPE == ETH_ISR_CAT1 || ETH_ISR_TYPE == ETH_ISR_VOID)
FUNC(void, ETH_CODE_FAST) Eth_RxIrqHdlr_1(void)
#else
ISR(Eth_RxIrqHdlr_1)
#endif
{
    Eth_RxIrqHdlr((uint8)ETH_CTRL_ID_1);
}

#if (ETH_ISR_TYPE == ETH_ISR_CAT1 || ETH_ISR_TYPE == ETH_ISR_VOID)
FUNC(void, ETH_CODE_FAST) Eth_TxIrqHdlr_1(void)
#else
ISR(Eth_TxIrqHdlr_1)
#endif
{
    Eth_TxIrqHdlr((uint8)ETH_CTRL_ID_1);
}

#if (ETH_ISR_TYPE == ETH_ISR_CAT1 || ETH_ISR_TYPE == ETH_ISR_VOID)
FUNC(void, ETH_CODE_FAST) Eth_RxIrqPacingHdlr_1(void)
#else
ISR(Eth_RxIrqPacingHdlr_1)
#endif
{
    Eth_RxIrqPacingHdlr((uint8)ETH_CTRL_ID_1);
}

#if (ETH_ISR_TYPE == ETH_ISR_CAT1 || ETH_ISR_TYPE == ETH_ISR_VOID)
FUNC(void, ETH_CODE_FAST) Eth_TxIrqPacingHdlr_1(void)
#else
ISR(Eth_TxIrqPacingHdlr_1)
#endif
{
    Eth_TxIrqPacingHdlr((uint8)ETH_CTRL_ID_1);
}
#endif /* (1U < ETH_CTRL_ID_MAX)  */

#if (STD_ON == ETH_ENABLE_MII_API)
#if (ETH_ISR_TYPE == ETH_ISR_CAT1 || ETH_ISR_TYPE == ETH_ISR_VOID)
FUNC(void, ETH_CODE_FAST) Eth_MdioIrqHdlr(void)
#else
ISR(Eth_MdioIrqHdlr)
#endif
{
    uint16 val;
    Std_ReturnType retVal;
    uint8 ctrlIdx;

    for(ctrlIdx = 0; ctrlIdx < ETH_CTRL_ID_MAX; ++ctrlIdx)
    {
        if(TRUE == Eth_MiiAccess[ctrlIdx].isActive)
        {
            break;
        }
    }

    if (ctrlIdx < ETH_CTRL_ID_MAX)
    {
        if (TRUE == Eth_MiiAccess[ctrlIdx].isRead)
        {
            retVal = Eth_getMiiReadVal(ctrlIdx, &val);
            if (E_OK == retVal)
            {
                EthTrcv_ReadMiiIndication(Eth_MiiAccess[ctrlIdx].ctrlIdx,
                                          Eth_MiiAccess[ctrlIdx].trcvIdx,
                                          Eth_MiiAccess[ctrlIdx].regIdx,
                                          val);
            }
        }
        else
        {
            EthTrcv_WriteMiiIndication(Eth_MiiAccess[ctrlIdx].ctrlIdx,
                                       Eth_MiiAccess[ctrlIdx].trcvIdx,
                                       Eth_MiiAccess[ctrlIdx].regIdx);

        }

        Eth_MiiAccess[ctrlIdx].isActive = FALSE;
    }

    for(ctrlIdx = 0; ctrlIdx < ETH_CTRL_ID_MAX; ++ctrlIdx)
    {
        /* Always clear MDIO Irq */
        Eth_clearMdioInt(ctrlIdx);
    }
}
#endif /* (STD_ON == ETH_ENABLE_MII_API) */

#define ETH_STOP_SEC_ISR_CODE
#include "Eth_MemMap.h"

/* ========================================================================== */
/*                  Internal Function Definitions                             */
/* ========================================================================== */
#define ETH_START_SEC_ISR_CODE
#include "Eth_MemMap.h"

static FUNC(void, ETH_CODE_FAST) Eth_RxIrqHdlr(uint8 ctrlIdx)
{
    sint8 fifoIdx;

    for(fifoIdx = (sint8)(Eth_GetIngressFifoTotalNum(ctrlIdx) - 1u); fifoIdx >= 0; --fifoIdx)
    {
        if(CpswDma_Ring_IrqIsSet(ctrlIdx, Eth_GetIngressFifoCqIdx(ctrlIdx, fifoIdx)) == TRUE)
        {
            (void)Eth_receiveProcess(ctrlIdx, ((uint8)fifoIdx));
        }
    }

    if(TRUE == Eth_GetRxIrqPacingEnable(ctrlIdx))
    {
        for(fifoIdx = (sint8)(Eth_GetIngressFifoTotalNum(ctrlIdx) - 1u); fifoIdx >= 0; --fifoIdx)
        {
            CpswDma_Ring_IrqDisable(ctrlIdx, Eth_GetIngressFifoCqIdx(ctrlIdx, fifoIdx));
        }
        Eth_IrqPacingStartTimer(ctrlIdx, Eth_GetRxHwTimerIdx(ctrlIdx));
    }
}


static FUNC(void, ETH_CODE_FAST) Eth_TxIrqHdlr(uint8 ctrlIdx)
{
    sint8 fifoIdx;

    for(fifoIdx = (sint8)(Eth_GetEgressFifoTotalNum(ctrlIdx) - 1u); fifoIdx >= 0; --fifoIdx)
    {
        if(CpswDma_Ring_IrqIsSet(ctrlIdx, Eth_GetEgressFifoCqIdx(ctrlIdx, fifoIdx)) == TRUE)
        {
            (void)Eth_txProcess(ctrlIdx, ((uint8)fifoIdx));
        }
    }

    if(TRUE == Eth_GetTxIrqPacingEnable(ctrlIdx))
    {
        for(fifoIdx = (sint8)(Eth_GetEgressFifoTotalNum(ctrlIdx) - 1u); fifoIdx >= 0; --fifoIdx)
        {
            CpswDma_Ring_IrqDisable(ctrlIdx, Eth_GetEgressFifoCqIdx(ctrlIdx, fifoIdx));
        }
        Eth_IrqPacingStartTimer(ctrlIdx, Eth_GetTxHwTimerIdx(ctrlIdx));
    }
}

static FUNC(void, ETH_CODE_FAST) Eth_RxIrqPacingHdlr(uint8 ctrlIdx)
{
    boolean isRestart = FALSE;
    sint8 fifoIdx;

    for(fifoIdx = (sint8)(Eth_GetIngressFifoTotalNum(ctrlIdx) - 1u); fifoIdx >= 0; --fifoIdx)
    {
        if(ETH_RECEIVED_MORE_DATA_AVAILABLE == Eth_receiveProcess(ctrlIdx, ((uint8)fifoIdx)))
        {
            isRestart = TRUE;
        }
    }

    if(FALSE == isRestart)
    {
        for(fifoIdx = (sint8)(Eth_GetIngressFifoTotalNum(ctrlIdx) - 1u); fifoIdx >= 0; --fifoIdx)
        {
            CpswDma_Ring_IrqEnable(ctrlIdx, Eth_GetIngressFifoCqIdx(ctrlIdx, fifoIdx));
        }
    }

    Eth_IrqPacingRestartHwTimer(ctrlIdx,
        Eth_GetRxHwTimerIdx(ctrlIdx),
        isRestart);
}

static FUNC(void, ETH_CODE_FAST) Eth_TxIrqPacingHdlr(uint8 ctrlIdx)
{
    boolean isRestart = FALSE;
    sint8 fifoIdx;

    for(fifoIdx = (sint8)(Eth_GetEgressFifoTotalNum(ctrlIdx) - 1u); fifoIdx >= 0; --fifoIdx)
    {
        if(ETH_TRANSMITED_BUFFER_PENDING == Eth_txProcess(ctrlIdx, ((uint8)fifoIdx)))
        {
            isRestart = TRUE;
        }
    }

    if(FALSE == isRestart)
    {
        for(fifoIdx = (sint8)(Eth_GetEgressFifoTotalNum(ctrlIdx) - 1u); fifoIdx >= 0; --fifoIdx)
        {
            CpswDma_Ring_IrqEnable(ctrlIdx, Eth_GetEgressFifoCqIdx(ctrlIdx, fifoIdx));
        }
    }

    Eth_IrqPacingRestartHwTimer(ctrlIdx,
        Eth_GetTxHwTimerIdx(ctrlIdx),
        isRestart);
}

#define ETH_STOP_SEC_ISR_CODE
#include "Eth_MemMap.h"
