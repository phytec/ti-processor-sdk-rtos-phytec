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
 *  \file     Eth_LL_Types.h
 *
 *  \brief    This file contains the structure definitions and function
 *            prototypes of the Eth Dma lowler type
 */
#ifndef ETH_LL_TYPES_H_
#define ETH_LL_TYPES_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "Std_Types.h"
#include "Eth_GeneralTypes.h"

#define ETH_CPPI_PROTOCOL_SPECIFIC_LENGTH ((uint32)4u)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/**
 *  \brief Cppi receive control type
 *
 *  Type of Cppi receive control
 */
typedef struct sEth_CppiRxControlType
{
    uint32 rsvd1;
    /**< reserved 1 */
    uint32 tsInfo;
    /**< timestamp information */
    uint32 chkSumInfo;
    /**< checksum information */
    uint32 rsvd2;
    /**< reserved 2 */
} Eth_CppiRxControlType;

/**
 *  \brief Cppi5 host packet descriptor type
 *
 *  Type of Cppi5 host packet descriptor
 */
typedef struct sEth_Cppi5HMPDType
{
  uint32      descInfo;
  /**< word 0: Packet information word 0 (not used in Buffer descriptors) */
  uint32      pktInfo1;
  /**< word 1: Packet information word 1 (not used in Buffer descriptors) */
  uint32      pktInfo2;
  /**< word 2: Packet information word 2 | Buffer reclamation information */
  uint32      srcDstTag;
  /**< word 3: Packet information word 3 (not used in Buffer descriptors) */
  uint64      nextDescPtr;
  /**< words 4/5: Linking word */
  uint64      bufPtr;
  /**< words 6/7: Buffer 0 information words 0 and 1 (PD words 6 and 7) */
  uint32      bufInfo1;
  /**< word 8: Buffer 0 information word 2 (PD word 8) */
  uint32      orgBufLen;
  /**< word 9: Original buffer 0 information word 0 (PD word 9) */
  uint64      orgBufPtr;
  /**< words 10/11: Original buffer 0 information words 1 and 2 (PD words 10 and 11) */
} Eth_Cppi5HMPDType;

/**
 *  \brief Cppi5 protocol specific information type
 *
 *  Type of Cppi5 protocol specific information
 */
typedef struct sEth_Cppi5PsInfoType
{
    uint32 tsLow;
    /**< Lower word of 64-bit timestamp */
    uint32 tsHigh;
    /**< Higher word of 64-bit timestamp */
    uint32 chkSumInfo;
    /**< pV4/IpV6 protocol information info status word */
    uint32 rsvd;
    /**< Reserved */
} Eth_Cppi5PsInfoType;

/**
 *  \brief User buffer information
 *
 *  Type of User buffer information
 */
typedef struct sEth_BufferInforType
{
    uint32 Idx;
    uint32 Length;
    uint32 FifoIdx;
    struct sEth_BufferInforType *next;
    Eth_DataType *BufPtr;
} Eth_BufferInforType;

/**
 *  \brief Eth low level descriptor type
 *
 *  Type of Eth low level descriptor
 */
typedef struct sEth_LLDescType
{
    Eth_Cppi5HMPDType   desc;
    /**< host packet descriptor  */
    uint8               extendPktInfo[16U];
    /**< extend packet information  */
    uint32              psInfo[4U];
    /**< protocol specific information  */
    Eth_BufferInforType bufferInfo;
    /**< user specific information  */
    uint8               reserved[28U];
    /**< Reserved  */
} Eth_DescType;

typedef struct sEth_QueueDescType
{
    uint16 count;
    /**< Number of element in queue  */
    Eth_BufferInforType *head;
    /**< Head pointer of queue */
    Eth_BufferInforType *tail;
    /**< Tail pointer of queue */
} Eth_QueueType;

#endif /* ETH_LL_TYPES_H_ */
