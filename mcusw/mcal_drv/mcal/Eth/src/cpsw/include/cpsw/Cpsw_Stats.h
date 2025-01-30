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
 *  \file     Cpsw_Stats.h
 *
 *  \brief    This file contains the structure definitions and function
 *            prototypes of the the Statistics module.
 */

/**
 *  \addtogroup CPSW_STATS_API
 *  @{
 */

#ifndef CPSWMCAL_STATS_H_
#define CPSWMCAL_STATS_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "cslr_xge_cpsw.h"
#include "csl_cpsw.h"
#include "Cpsw_Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                                 Macros                                     */
/* ========================================================================== */

/*! \brief Number of elements in a statistics block */
#define STATS_BLOCK_ELEM_NUM                  (128U)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/**
 *  \brief Statistics values
 *
 *  Statistics values reported by the CPSW hardware.
 */
typedef struct
{
    uint32 RxGoodFrames;
    /**< Total number of good frames received */
    uint32 RxBCastFrames;
    /**< Total number of good broadcast frames received */
    uint32 RxMCastFrames;
    /**< Total number of good multicast frames received */
    uint32 RxPauseFrames;
    /**< Total number of pause frames received */
    uint32 RxCRCErrors;
    /**< Total number of CRC errors frames received */
    uint32 RxAlignCodeErrors;
    /**< Total number of alignment/code error frames received */
    uint32 RxOversized;
    /**< Total number of oversized frames received */
    uint32 RxJabber;
    /**< Total number of jabber frames received */
    uint32 RxUndersized;
    /**< Total number of undersized frames received */
    uint32 RxFragments;
    /**< Total number of fragmented frames received */
    uint32 RxAleDrop;
    /**< Total number of frames dropped by the ALE */
    uint32 RxAleOverrunDrop;
    /**< Total number of overrun frames dropped by the ALE */
    uint32 RxOctets;
    /**< Total number of received bytes in good frames */
    uint32 TxGoodFrames;
    /**< Total number of good frames transmitted */
    uint32 TxBCastFrames;
    /**< Total number of good broadcast frames transmitted */
    uint32 TxMCastFrames;
    /**< Total number of good multicast frames transmitted */
    uint32 TxPauseFrames;
    /**< Total number of bytes in all good frames transmitted */
    uint32 TxDeferred;
    /**< Total number of deferred frames transmitted */
    uint32 TxCollision;
    /**< Total number of transmitted frames experiencing a collision */
    uint32 TxSingleColl;
    /**< Total number of transmitted frames experiencing a single
     *   collision */
    uint32 TxMultiColl;
    /**< Total number of transmitted frames experiencing multiple
     *   collisions */
    uint32 TxExcessiveColl;
    /**< Total number of transmitted frames abandoned due to excessive
     *   collisions */
    uint32 TxLateColl;
    /**< Total number of transmitted frames abandoned due to a late
     *   collision */
    uint32 RxIpgError;
    /**< Total number of receive inter-packet gap errors */
    uint32 TxCarrierSLoss;
    /**< Total number of transmitted frames that experienced a
     *   carrier loss */
    uint32 TxOctets;
    /**< Total number of bytes in all good frames transmitted */
    uint32 Frame64;
    /**< Total number of 64-byte frames received and transmitted */
    uint32 Frame65t127;
    /**< Total number of frames of size 65 to 127 bytes received and
     *   transmitted */
    uint32 Frame128t255;
    /**< Total number of frames of size 128 to 255 bytes received and
     *   transmitted */
    uint32 Frame256t511;
    /**< Total number of frames of size 256 to 511 bytes received and
     *   transmitted */
    uint32 Frame512t1023;
    /**< Total number of frames of size 512 to 1023 bytes received and
     *   transmitted */
    uint32 Frame1024tUp;
    /**< Total number of frames of size 1024 to rx_maxlen bytes received
     *   and 1024 bytes or greater transmitted */
    uint32 NetOctets;
    /**< Total number of bytes received and transmitted */
    uint32 RxDropBottom;
    /**< Total number of received frames dropped due to Receive Bottom
     *   of FIFO */
    uint32 PortmaskDrop;
    /**< Total number of received frames dropped due to portmask */
    uint32 RxDropTop;
    /**< Total number of received frames dropped due to Top of FIFO */
    uint32 AleRateLimitDrop;
    /**< Total number of dropped frames due to ALE rate limiting */
    uint32 AleVidDrop;
    /**< Total number of dropped frames due to ALE VID ingress */
    uint32 AleAddrEqDrop;
    /**< Total number of dropped frames due to DA = SA */
    uint32 AleBlockDrop;
    /**< Total number of dropped frames due to ALE Block Mode */
    uint32 AleSecureDrop;
    /**< Total number of dropped frames due to ALE Secure Mode */
    uint32 AleAuthDrop;
    /**< Total number of dropped frames due to ALE Authentication */
    uint32 AleUnKnUni;
    /**< ALE receive unknown unicast */
    uint32 AleUnKnUniBytes;
    /**< ALE receive unknown unicast bytecount */
    uint32 AleUnKnMulti;
    /**< ALE receive unknown multicast */
    uint32 AleUnKnMultiBytes;
    /**< ALE receive unknown multicast bytecount */
    uint32 AleUnKnBCast;
    /**< ALE receive unknown brodcast */
    uint32 AleUnKnBCastBytes;
    /**< ALE receive unknown broadcast bytecount */
    uint32 AlePolMatch;
    /**< ALE Policer matched */
    uint32 AlePolMatchRed;
    /**< ALE Policer Matched and Condition Red */
    uint32 AlePolMatchYellow;
    /**< ALE Policer Matched and Condition Yellow */
    uint32 AleMultSrcAddrDrop;
    /**< ALE Multicast Source Address Drop */
    uint32 AleDualVlanDrop;
    /**< ALE Dual VLAN Drop */
    uint32 AleLenErrDrop;
    /**< ALE Length Error Drop */
    uint32 AleNextHdrDrop;
    /**< ALE IP Next Header Drop */
    uint32 AleIPv4FragDrop;
    /**< ALE IPV4 Frag Drop */
    uint32 Reserve1[24];
    /**< Reserved area (24 words) */
    uint32 IETRxAssemblyErr;
    /**< IET Receive Assembly Error */
    uint32 IETRxAssemblyOK;
    /**< IET Receive Assembly OK */
    uint32 IETRxSmdErr;
    /**< IET Receive Smd Error */
    uint32 IETRxFrag;
    /**< IET Receive Frag */
    uint32 IETTxHold;
    /**< IET Transmit Hold */
    uint32 IETTxFrag;
    /**< IET Transmit Frag */
    uint32 Reserve2[9U];
    /**< Reserved area (9 words) */
    uint32 TxMemProtectErr;
    /**< Transmit Memory Protect CRC Error */
    uint32 TxPriPktCnt[8];
    /**< Tx Priority [0-7] Packet Count */
    uint32 TxPriByteCnt[8];
    /**< Tx Priority [0-7] Packet Byte Count */
    uint32 TxPriDropPktCnt[8];
    /**< Tx Priority [0-7] Packet Drop Count */
    uint32 TxPriDropByteCnt[8];
    /**< Tx Priority [0-7] Packet Drop Byte Count */
} CpswStats_Stats;

/**
 *  \brief Port module
 *
 *  This is an internal/private driver structure and should not be used
 *  or modified by the caller.
 */
typedef struct
{
    CSL_Xge_cpswRegs *regs;
    /**< CPSW register overlay */
    CpswStats_Stats stats[CPSW_PORT_MAX];
    /**< Saved per-port statistics values */
    boolean isInit;
    /**< Module initialization state */
} CpswStats;

/* ========================================================================== */
/*                                 Macros                                     */
/* ========================================================================== */

/** \brief returns the offset of the field member from the start of the structure type. */
#define OFFSETOF(S, E)  ((uint16)(((uint32*)(&(S->E)))-((uint32*)((S)))))

/** \brief CpswStats_Stats reseved member Offsets */
#define CPSW_STATS_RSV1_OFFSET_START(S)     (OFFSETOF((S), Reserve1))
#define CPSW_STATS_RSV1_OFFSET_END(S)       (OFFSETOF((S), IETRxAssemblyErr))
#define CPSW_STATS_RSV2_OFFSET_START(S)     (OFFSETOF((S), Reserve2))
#define CPSW_STATS_RSV2_OFFSET_END(S)       (OFFSETOF((S), TxMemProtectErr))

/* ========================================================================== */
/*                         Global Variables Declarations                      */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief Statistics open function
 *
 *  Opens the Statistics module.
 *
 *  This function must be called before any other CpswStats_*() function.
 *
 *
 *  \return \ref Cpsw_ErrorCodes
 */
void CpswStats_open(void);

/**
 *  \brief Statistics close function
 *
 *  Closes the Statistics module.
 *
 *  No CpswStats_*() functions must be called after this function.
 *
 *  \param stats    CpswStats handle
 */
void CpswStats_close(void);

/**
 *  \brief Get statistics of a port
 *
 *  Gets the statistics values of a given port.
 *
 *  \param num      Port number
 *  \param val      Pointer to the variable where statistics will be copied
 */
void CpswStats_getStats(CpswPort_Num port, CpswStats_Stats *val);
/**
 *  \brief Statistics enable for port function
 *
 *  enable the Statistics for port.
 *
 *  This function must be called before any other CpswStats_*() function.
 *
 *  \param num      Cpsw port number
 *  \param value    enable or disable statistic
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType CpswStats_setPort(CpswPort_Num port, boolean value);

/* ========================================================================== */
/*                        Deprecated Function Declarations                    */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
}
#endif

#endif /* CPSWMCAL_STATS_H_ */

/*! @} */
