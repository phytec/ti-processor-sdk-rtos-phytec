/*
*
* Copyright (C) 2024 Texas Instruments Incorporated
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
 *  \file     Udma.h
 *
 *  \brief    This file contains Udma function definitions.
 */

#ifndef UDMA_H_
#define UDMA_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "Std_Types.h"

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief UDMA open tx channel
 *
 *  Open UDMA Tx channel
 *
 *  \param CtrlIdx  Controller index
 *  \param ChIdx    Channel index
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType Udma_Channel_TxOpen(
    uint8 CtrlIdx,
    uint8 ChIdx);

/**
 *  \brief UDMA enable Tx channel
 *
 *  enable UDMA Tx channel
 *
 *  \param CtrlIdx  Controller index
 *  \param ChIdx    Channel index
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType Udma_Channel_TxEnable(
    uint8 CtrlIdx,
    uint8 ChIdx);

/**
 *  \brief UDMA disable Tx channel
 *
 *  disable UDMA Tx channel
 *
 *  \param CtrlIdx  Controller index
 *  \param ChIdx    Channel index
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType Udma_Channel_TxDisable(
    uint8 CtrlIdx,
    uint8 ChIdx);

/**
 *  \brief UDMA open Rx channel
 *
 *  Open UDMA Rx channel
 *
 *  \param CtrlIdx  Controller index
 *  \param ChIdx    Channel index
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType Udma_Channel_RxOpen(
    uint8 CtrlIdx,
    uint8 ChIdx);

/**
 *  \brief UDMA enable Tx channel
 *
 *  enable UDMA Tx channel
 *
 *  \param CtrlIdx  Controller index
 *  \param ChIdx    Channel index
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType Udma_Channel_RxEnable(
    uint8 CtrlIdx,
    uint8 ChIdx);

/**
 *  \brief UDMA disable Rx channel
 *
 *  disable UDMA Rx channel
 *
 *  \param CtrlIdx  Controller index
 *  \param ChIdx    Channel index
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType Udma_Channel_RxDisable(
    uint8 CtrlIdx,
    uint8 ChIdx);

/**
 *  \brief UDMA close Tx channel
 *
 *  close UDMA Tx channel
 *
 *  \param CtrlIdx  Controller index
 *  \param ChIdx    Channel index
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType Udma_Channel_TxClose(
    uint8 CtrlIdx,
    uint8 ChIdx);

/**
 *  \brief UDMA close Rx channel
 *
 *  close UDMA Rx channel
 *
 *  \param CtrlIdx  Controller index
 *  \param ChIdx    Channel index
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType Udma_Channel_RxClose(
    uint8 CtrlIdx,
    uint8 ChIdx);

/**
 *  \brief Queue a DMA descriptor to a ring
 *
 *  Queue a DMA descriptor to a ring
 *
 *  \param CtrlIdx  Controller index
 *  \param RingIdx  Ring index
 *  \param phyDescMem Udma descriptor address
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType Udma_Ring_Queue(
    uint8 CtrlIdx,
    uint8 RingIdx,
    uint64 phyDescMem);

/**
 *  \brief DeQueue a DMA descriptor from a ring
 *
 *  Dequeue a DMA descriptor from a ring
 *
 *  \param CtrlIdx  Controller index
 *  \param RingIdx  Ring index
 *  \param phyDescMem Udma descriptor address
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType Udma_Ring_DeQueue(
    uint8 CtrlIdx,
    uint8 RingIdx,
    uint64 *phyDescMem);

/**
 *  \brief DeQueue a DMA descriptor from a ring
 *
 *  Dequeue a DMA descriptor from a ring
 *
 *  \param CtrlIdx  Controller index
 *  \param RingIdx  Ring index
 *  \param phyDescMem Udma descriptor address
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType Udma_Ring_Flush(
    uint8 CtrlIdx,
    uint8 RingIdx,
    uint64 *phyDescMem);

/**
 *  \brief Get number DMA descriptor occupy for read operation in a ring
 *
 *  Get number DMA descriptor occupy in a ring
 *
 *  \param CtrlIdx  Controller index
 *  \param RingIdx  Ring index
 *
 *  \return number of DMA descriptor in ring
 */
uint32 Udma_Ring_ReadOccNum(
    uint8 CtrlIdx,
    uint8 RingIdx);

/**
 *  \brief Get number DMA descriptor occupy for write operation in a ring
 *
 *  Get number DMA descriptor occupy in a ring
 *
 *  \param CtrlIdx  Controller index
 *  \param RingIdx  Ring index
 *
 *  \return number of DMA descriptor in ring
 */
uint32 Udma_Ring_WriteOccNum(
    uint8 CtrlIdx,
    uint8 RingIdx);

/**
 *  \brief Udma Ring init
 *  Init Udma ring
 *
 *  \param CtrlIdx  Controller index
 *  \param RingIdx  Ring index
 *
 *  \return None
 */
void Udma_Ring_Init(
    uint8 CtrlIdx,
    uint8 RingIdx);

#if (ETH_DMA_IR_SUPPORT == STD_ON)
/**
 *  \brief Udma master event init
 *  Init Udma master event
 *
 *  \param CtrlIdx  Controller index
 *  \param EventIdx Master event index
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType Udma_MasterEvent_Init(
    uint8 CtrlIdx,
    uint8 EventIdx);
#endif

/**
 *  \brief Udma sub event init
 *  Init Udma sub event
 *
 *  \param CtrlIdx  Controller index
 *  \param EventIdx Ring event index
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType Udma_RingEvent_Init(
    uint8 CtrlIdx,
    uint8 EventIdx);

/**
 *  \brief Udma sub event release
 *  Init Udma sub event
 *
 *  \param CtrlIdx  Controller index
 *  \param EventIdx Ring event index
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType Udma_RingEvent_Release(
    uint8 CtrlIdx,
    uint8 EventIdx);

/**
 *  \brief Disable event(IRQ)
 *
 *  \param CtrlIdx  Controller index
 *  \param EventIdx Event index
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType Udma_MasterEvent_Disable(
    uint8 CtrlIdx,
    uint8 EventIdx);

/**
 *  \brief Enable flow
 *
 *  \param CtrlIdx  Controller index
 *  \param FlowId   Flow index
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType Udma_Flow_Enable(
    uint8 CtrlIdx,
    uint8 FlowId);

#if (ETH_DMA_PROXY_SUPPORT == STD_ON)
/**
 *  \brief Proxy Init
 *
 *  \param CtrlIdx  Controller index
 *  \param proxyIdx Proxy index
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType Udma_Proxy_Init(
    uint8 CtrlIdx,
    uint8 proxyIdx);
#endif

#ifdef __cplusplus
}
#endif

#endif /* UDMA_H_ */
