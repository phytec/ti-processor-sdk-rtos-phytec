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
 *  \file     Eth_Packet.h
 *
 *  \brief    This file contains the structure definitions and function
 *            prototypes of the Ethernet packet and packet queue abstactions.
 */

#ifndef ETH_PACKET_H_
#define ETH_PACKET_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */


#include "Eth_Types.h"
#include "Eth_LL_Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                                 Macros                                     */
/* ========================================================================== */

/**
 *  \brief Ethernet max payload length
 *
 *  This value is calculated from the maximum Ethernet frame length (1522 bytes) minus Layer-2 header size (14 bytes).
 *  1522 bytes is the maximum value that user can configure for Ethernet frame in configuration tool.
 */
#define ETH_MAX_PAYLOAD_LENGTH  (1508u)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/**
 *  \brief Ethernet frame header
 *
 *  Ethernet frame header without VLAN tag
 */
typedef struct
{
    uint8 dstMac[ETH_MAC_ADDR_LEN];
    /**< Destination MAC address */
    uint8 srcMac[ETH_MAC_ADDR_LEN];
    /**< Source MAC address */
    Eth_FrameType etherType;
    /**< EtherType field */
} Eth_FrameHeader;

/**
 *  \brief Ethernet frame
 *
 *  Ethernet frame without VLAN tag
 */
typedef struct
{
    Eth_FrameHeader hdr;
    /**< Ethernet frame header */
    uint8 payload[ETH_MAX_PAYLOAD_LENGTH];
    /**< Pointer to the frame's payload */
} __attribute__((packed)) Eth_Frame;

/* ========================================================================== */
/*                         Global Variables Declarations                      */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
/**
 *  \brief Init eth queue
 *
 *  \param queuePtr point to a queue
 *
 *  \return none
 */
void Eth_Queue_init(Eth_QueueType *queuePtr);

/**
 *  \brief add node to end of queue
 *
 *  \param queuePtr point to a queue
 *  \param nodePtr point to a node
 *
 *  \return none
 */
void Eth_Queue_add(Eth_QueueType *queuePtr, Eth_BufferInforType *nodePtr);

/**
 *  \brief remove node in front of queue
 *
 *  \param queuePtr point to a queue
 *
 *  \return a pointer to node
 */
Eth_BufferInforType* Eth_Queue_remove(Eth_QueueType *queuePtr);

#if (STD_ON == ETH_ZERO_COPY_API)
/**
 *  \brief remove node in queue by BufPtr
 *
 *  \param queuePtr point to a queue
 *  \param BufPtr point to a buffer address
 *
 *  \return a pointer to node
 */
Eth_BufferInforType *Eth_Queue_removeByBuffer(Eth_QueueType *queuePtr,
    Eth_DataType *BufPtr);

/**
 *  \brief find node in queue by BufPtr
 *
 *  \param queuePtr point to a queue
 *  \param BufPtr point to a buffer address
 *
 *  \return a pointer to node
 */
Eth_BufferInforType *Eth_Queue_findByBuffer(Eth_QueueType *queuePtr,
    Eth_DataType *BufPtr);
#endif /* (STD_ON == ETH_ZERO_COPY_API) */

/* ========================================================================== */
/*                        Deprecated Function Declarations                    */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
}
#endif

#endif /* ETH_PACKET_H_ */
