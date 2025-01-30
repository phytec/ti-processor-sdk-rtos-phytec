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
 *  \file     Udma_Ring_LL.h
 *
 *  \brief    This file contains Udma Sciclient function declarations.
 */

#ifndef UDMA_RING_LL_H
#define UDMA_RING_LL_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "Std_Types.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief UDMA Ring low layer init
 *
 *  Initilize for low layer ring
 *
 *  \param CtrlIdx  Udma control index
 *  \param RingIdx  Ring index
 *
 *  \return None
 */
void Udma_Ring_LL_Init(
    uint8 CtrlIdx,
    uint8 RingIdx);

/**
 *  \brief Queue a DMA descriptor to a ring
 *
 *  Queue a DMA descriptor to a ring
 *
 *  \param CtrlIdx  Udma control index
 *  \param RingIdx  Ring index
 *  \param phyDescMem Udma descriptor address
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType Udma_Ring_LL_Queue(
    uint8 CtrlIdx,
    uint8 RingIdx,
    uint64 phyDescMem);

/**
 *  \brief DeQueue a DMA descriptor from a ring
 *
 *  Dequeue a DMA descriptor from a ring
 *
 *  \param CtrlIdx  Udma control index
 *  \param RingIdx  Ring index
 *  \param phyDescMem Udma descriptor address
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType Udma_Ring_LL_Dequeue(
    uint8 CtrlIdx,
    uint8 RingIdx,
    uint64 *phyDescMem);

/**
 *  \brief Flush a DMA descriptor from a ring
 *
 *  Flush a DMA descriptor from a ring
 *
 *  \param CtrlIdx  Udma control index
 *  \param RingIdx  Ring index
 *  \param phyDescMem Udma descriptor address
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType Udma_Ring_LL_Flush(
    uint8 CtrlIdx,
    uint8 RingIdx,
    uint64 *phyDescMem);

/**
 *  \brief Get number DMA read pending descriptor in a ring
 *
 *  Get number DMA descriptor occupy in a ring
 *
 *  \param CtrlIdx  Udma control index
 *  \param RingIdx  Ring index
 *
 *  \return number of DMA descriptor in ring
 */
uint32 Udma_Ring_LL_ReadOccNum(
    uint8 CtrlIdx,
    uint8 RingIdx);

/**
 *  \brief Get number DMA wire pending descriptor in a ring
 *
 *  Get number DMA descriptor occupy in a ring
 *
 *  \param CtrlIdx  Udma control index
 *  \param RingIdx  Ring index
 *
 *  \return number of DMA descriptor in ring
 */
uint32 Udma_Ring_LL_WriteOccNum(
    uint8 CtrlIdx,
    uint8 RingIdx);

#if (ETH_DMA_PROXY_SUPPORT == STD_ON)
/**
 *  \brief Queue a DMA descriptor to a ring via proxy
 *
 *  Queue a DMA descriptor to a ring
 *
 *  \param CtrlIdx  Udma control index
 *  \param RingIdx  Ring index
 *  \param phyDescMem Udma descriptor address
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType Udma_Ring_LL_ProxyQueue(uint8 CtrlIdx,
    uint8 RingIdx,
    uint64 phyDescMem);

/**
 *  \brief Flush a DMA descriptor from a ring via proxy
 *
 *  Flush a DMA descriptor from a ring
 *
 *  \param CtrlIdx  Udma control index
 *  \param RingIdx  Ring index
 *  \param phyDescMem Udma descriptor address
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType Udma_Ring_LL_ProxyFlush(uint8 CtrlIdx,
    uint8 RingIdx,
    uint64 *phyDescMem);

#endif

#ifdef __cplusplus
}
#endif


#endif /* UDMA_RING_LL_H */
