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
 *  \file     Eth_Packet.c
 *
 *  \brief    This file contains Ethernet packet and packet queue abstractions
 *            related functionality.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "Eth_Packet.h"

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

/* None */

/* ========================================================================== */
/*                  Internal Function Definitions                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

void Eth_Queue_init(Eth_QueueType *queuePtr)
{
    queuePtr->head = (Eth_BufferInforType *)NULL_PTR;
    queuePtr->tail = (Eth_BufferInforType *)NULL_PTR;
    queuePtr->count = 0u;
}

void Eth_Queue_add(Eth_QueueType *queuePtr, Eth_BufferInforType *nodePtr)
{
    nodePtr->next = (Eth_BufferInforType *)NULL_PTR;

    if(NULL_PTR == queuePtr->head)
    {
        queuePtr->head = nodePtr;
    }
    else
    {
        queuePtr->tail->next = nodePtr;
    }

    queuePtr->tail = nodePtr;
    queuePtr->count++;
}

Eth_BufferInforType *Eth_Queue_remove(Eth_QueueType *queuePtr)
{
    Eth_BufferInforType *nodePtr  = (Eth_BufferInforType *)NULL_PTR;

    nodePtr = queuePtr->head;

    if(NULL_PTR != nodePtr)
    {
        queuePtr->head = nodePtr->next;
        if(NULL_PTR == queuePtr->head)
        {
            queuePtr->tail = (Eth_BufferInforType *)NULL_PTR;
        }

        queuePtr->count--;
        nodePtr->next = (Eth_BufferInforType *)NULL_PTR;
    }

    return nodePtr;
}

#if (STD_ON == ETH_ZERO_COPY_API)
Eth_BufferInforType *Eth_Queue_removeByBuffer(Eth_QueueType *queuePtr,
    Eth_DataType *BufPtr)
{
    Eth_BufferInforType *queueNode = (Eth_BufferInforType *)NULL_PTR;
    Eth_BufferInforType *prevQueueNode = (Eth_BufferInforType *)NULL_PTR;
    uint16 queueIdx;
    uint16 elemCount = queuePtr->count;

    for(queueIdx = 0u; queueIdx < elemCount; ++queueIdx)
    {
        prevQueueNode = queueNode;

        if(queueNode == (Eth_BufferInforType *)NULL_PTR)
        {
            queueNode = queuePtr->head;
        }
        else
        {
            queueNode = queueNode->next;
        }

        if(queueNode->BufPtr == BufPtr)
        {
            if(prevQueueNode == (Eth_BufferInforType *)NULL_PTR)
            {
                queuePtr->head = queueNode->next;
            }
            else
            {
                prevQueueNode->next = queueNode->next;
            }

            if(queueNode == queuePtr->tail)
            {
                queuePtr->tail = prevQueueNode;
            }
            queuePtr->count--;
            break;
        }
    }

    return queueNode;
}

Eth_BufferInforType *Eth_Queue_findByBuffer(Eth_QueueType *queuePtr,
    Eth_DataType *BufPtr)
{
    Eth_BufferInforType *queueNode = (Eth_BufferInforType *)NULL_PTR;
    uint16 queueIdx;
    uint16 elemCount = queuePtr->count;

    for(queueIdx = 0u; queueIdx < elemCount; ++queueIdx)
    {
        if(queueNode == (Eth_BufferInforType *)NULL_PTR)
        {
            queueNode = queuePtr->head;
        }
        else
        {
            queueNode = queueNode->next;
        }

        if(queueNode->BufPtr == BufPtr)
        {
            break;
        }
    }

    return queueNode;
}
#endif /* (STD_ON == ETH_ZERO_COPY_API) */

