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
 *  \file     EthUtils_Target.c
 *
 *  \brief    This file contains Eth ultils target function definitions.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <string.h>

#include "Eth.h"
#include "Eth_Irq.h"
#include "EthIf_Cbk.h"
#include "EthTrcv.h"
#include "EthUtils.h"
#include "EthUtils_Target.h"
#include "board_utils.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Global Variable Definitions                        */
/* ========================================================================== */

extern EthApp_StateSet gEthApp;

#define  ETH_START_SEC_VAR_NO_INIT_UNSPECIFIED_128
#include "Eth_MemMap.h"
VAR(uint8, ETH_VAR_NO_INIT_128) EthApp_ExtTxBuffer[ETH_CTRL_ID_MAX][1536U];
#define  ETH_STOP_SEC_VAR_NO_INIT_UNSPECIFIED_128
#include "Eth_MemMap.h"

/* ========================================================================== */
/*                         Internel Variable Definitions                      */
/* ========================================================================== */

static const char *CtrOffsToName[CPSW_STATS_BLOCK_ELEM_NUM+1] =
{
    "RxGoodFrames",
    /**< Total number of good frames received */
    "RxBCastFrames",
    /**< Total number of good broadcast frames received */
    "RxMCastFrames",
    /**< Total number of good multicast frames received */
    "RxPauseFrames",
    /**< Total number of pause frames received */
    "RxCRCErrors",
    /**< Total number of CRC errors frames received */
    "RxAlignCodeErrors",
    /**< Total number of alignment/code error frames received */
    "RxOversized",
    /**< Total number of oversized frames received */
    "RxJabber",
    /**< Total number of jabber frames received */
    "RxUndersized",
    /**< Total number of undersized frames received */
    "RxFragments",
    /**< Total number of fragmented frames received */
    "RxAleDrop",
    /**< Total number of frames dropped by the ALE */
    "RxAleOverrunDrop",
    /**< Total number of overrun frames dropped by the ALE */
    "RxOctets",
    /**< Total number of received bytes in good frames */
    "TxGoodFrames",
    /**< Total number of good frames transmitted */
    "TxBCastFrames",
    /**< Total number of good broadcast frames transmitted */
    "TxMCastFrames",
    /**< Total number of good multicast frames transmitted */
    "TxPauseFrames",
    /**< Total number of bytes in all good frames transmitted */
    "TxDeferred",
    /**< Total number of deferred frames transmitted */
    "TxCollision",
    /**< Total number of transmitted frames experiencing a collision */
    "TxSingleColl",
    /**< Total number of transmitted frames experiencing a single
     *   collision */
    "TxMultiColl",
    /**< Total number of transmitted frames experiencing multiple
     *   collisions */
    "TxExcessiveColl",
    /**< Total number of transmitted frames abandoned due to excessive
     *   collisions */
    "TxLateColl",
    /**< Total number of transmitted frames abandoned due to a late
     *   collision */
    "RxIpgError",
    /**< Total number of receive inter-packet gap errors */
    "TxCarrierSLoss",
    /**< Total number of transmitted frames that experienced a
     *   carrier loss */
    "TxOctets",
    /**< Total number of bytes in all good frames transmitted */
    "Frame64",
    /**< Total number of 64-byte frames received and transmitted */
    "Frame65t127",
    /**< Total number of frames of size 65 to 127 bytes received and
     *   transmitted */
    "Frame128t255",
    /**< Total number of frames of size 128 to 255 bytes received and
     *   transmitted */
    "Frame256t511",
    /**< Total number of frames of size 256 to 511 bytes received and
     *   transmitted */
    "Frame512t1023",
    /**< Total number of frames of size 512 to 1023 bytes received and
     *   transmitted */
    "Frame1024tUp",
    /**< Total number of frames of size 1024 to rx_maxlen bytes received
     *   and 1024 bytes or greater transmitted */
    "NetOctets",
    /**< Total number of bytes received and transmitted */
    "RxDropBottom",
    /**< Total number of received frames dropped due to Receive Bottom
     *   of FIFO */
    "PortmaskDrop",
    /**< Total number of received frames dropped due to portmask */
    "RxDropTop",
    /**< Total number of received frames dropped due to Top of FIFO */
    "AleRateLimitDrop",
    /**< Total number of dropped frames due to ALE rate limiting */
    "AleVidDrop",
    /**< Total number of dropped frames due to ALE VID ingress */
    "AleAddrEqDrop",
    /**< Total number of dropped frames due to DA = SA */
    "AleBlockDrop",
    /**< Total number of dropped frames due to ALE Block Mode */
    "AleSecureDrop",
    /**< Total number of dropped frames due to ALE Secure Mode */
    "AleAuthDrop",
    /**< Total number of dropped frames due to ALE Authentication */
    "AleUnKnUni",
    /**< ALE receive unknown unicast */
    "AleUnKnUniBytes",
    /**< ALE receive unknown unicast bytecount */
    "AleUnKnMulti",
    /**< ALE receive unknown multicast */
    "AleUnKnMultiBytes",
    /**< ALE receive unknown multicast bytecount */
    "AleUnKnBCast",
    /**< ALE receive unknown brodcast */
    "AleUnKnBCastBytes",
    /**< ALE receive unknown broadcast bytecount */
    "AlePolMatch",
    /**< ALE Policer matched */
    "AlePolMatchRed",
    /**< ALE Policer Matched and Condition Red */
    "AlePolMatchYellow",
    /**< ALE Policer Matched and Condition Yellow */
    "AleMultSrcAddrDrop",
    /**< ALE Multicast Source Address Drop */
    "AleDualVlanDrop",
    /**< ALE Dual VLAN Drop */
    "AleLenErrDrop",
    /**< ALE Length Error Drop */
    "AleNextHdrDrop",
    /**< ALE IP Next Header Drop */
    "AleIPv4FragDrop",
    /**< ALE IPV4 Frag Drop */
    "Reserve1[00]",
    "Reserve1[01]",
    "Reserve1[02]",
    "Reserve1[03]",
    "Reserve1[04]",
    "Reserve1[05]",
    "Reserve1[06]",
    "Reserve1[07]",
    "Reserve1[08]",
    "Reserve1[09]",
    "Reserve1[10]",
    "Reserve1[11]",
    "Reserve1[12]",
    "Reserve1[13]",
    "Reserve1[14]",
    "Reserve1[15]",
    "Reserve1[16]",
    "Reserve1[17]",
    "Reserve1[18]",
    "Reserve1[19]",
    "Reserve1[20]",
    "Reserve1[21]",
    "Reserve1[22]",
    "Reserve1[23]",
    /**< Reserved area (24 words) */
    "IETRxAssemblyErr",
    /**< IET Receive Assembly Error */
    "IETRxAssemblyOK",
    /**< IET Receive Assembly OK */
    "IETRxSmdErr",
    /**< IET Receive Smd Error */
    "IETRxFrag",
    /**< IET Receive Frag */
    "IETTxHold",
    /**< IET Transmit Hold */
    "IETTxFrag",
    /**< IET Transmit Frag */
    "Reserve2[0]",
    "Reserve2[1]",
    "Reserve2[2]",
    "Reserve2[3]",
    "Reserve2[4]",
    "Reserve2[5]",
    "Reserve2[6]",
    "Reserve2[7]",
    "Reserve2[8]",
    /**< Reserved area (9 words) */
    "TxMemProtectErr",
    /**< Transmit Memory Protect CRC Error */
    "TxPriPktCnt[0]",
    "TxPriPktCnt[1]",
    "TxPriPktCnt[2]",
    "TxPriPktCnt[3]",
    "TxPriPktCnt[4]",
    "TxPriPktCnt[5]",
    "TxPriPktCnt[6]",
    "TxPriPktCnt[7]",
    /**< Tx Priority [0-7] Packet Count */
    "TxPriByteCnt[0]",
    "TxPriByteCnt[1]",
    "TxPriByteCnt[2]",
    "TxPriByteCnt[3]",
    "TxPriByteCnt[4]",
    "TxPriByteCnt[5]",
    "TxPriByteCnt[6]",
    "TxPriByteCnt[7]",
    /**< Tx Priority [0-7] Packet Byte Count */
    "TxPriDropPktCnt[0]",
    "TxPriDropPktCnt[1]",
    "TxPriDropPktCnt[2]",
    "TxPriDropPktCnt[3]",
    "TxPriDropPktCnt[4]",
    "TxPriDropPktCnt[5]",
    "TxPriDropPktCnt[6]",
    "TxPriDropPktCnt[7]",
    /**< Tx Priority [0-7] Packet Drop Count */
    "TxPriDropByteCnt[0]",
    "TxPriDropByteCnt[1]",
    "TxPriDropByteCnt[2]",
    "TxPriDropByteCnt[3]",
    "TxPriDropByteCnt[4]",
    "TxPriDropByteCnt[5]",
    "TxPriDropByteCnt[6]",
    "TxPriDropByteCnt[7]",
    /**< Tx Priority [0-7] Packet Drop Byte Count */
    "<END>"
};

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */
static const char *EthApp_ethErrorIdToString(uint8 errorId);
static const char *EthApp_ethTrcvErrorIdToString(uint8 errorId);
static const char *EthApp_errorIdToString(uint8 moduleId, uint8 errorId);
static const char *EthApp_ethApiIdToString(uint8 apiId);
static const char *EthApp_ethTrcvApiIdToString(uint8 apiId);
static const char *EthApp_apiIdToString(uint8 moduleId, uint8 apiId);
static boolean EthApp_verifyTxStatus(uint8 ctrlIdx, uint32 num, boolean txConfirmation);
static void EthApp_setConnectionType(uint32 macPortNum, Eth_MacConnectionType type, boolean delay);
static void EthApp_interruptConfig(void);

#define ETH_START_SEC_ISR_CODE
#include "Eth_MemMap.h"
static void Eth_MdioIrqHdlr_wrapper(void* arg);

static void Eth_TxIrqHdlr_wrapper_0(void* arg);
static void Eth_RxIrqHdlr_wrapper_0(void* arg);
static void Eth_TxIrqPacingHdlr_wrapper_0(void* arg);
static void Eth_RxIrqPacingHdlr_wrapper_0(void* arg);

#if (ETH_CTRL_ID_MAX > 1U)
static void Eth_TxIrqHdlr_wrapper_1(void* arg);
static void Eth_RxIrqHdlr_wrapper_1(void* arg);
static void Eth_TxIrqPacingHdlr_wrapper_1(void* arg);
static void Eth_RxIrqPacingHdlr_wrapper_1(void* arg);
#endif

#define ETH_STOP_SEC_ISR_CODE
#include "Eth_MemMap.h"

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
void EthApp_Startup(void)
{
    uint8 ctrlIdx;

    for(ctrlIdx = 0u; ctrlIdx < ETH_CTRL_ID_MAX; ++ctrlIdx)
    {
        if(FALSE == Eth_IsVirtualMacModeEnable(ctrlIdx))
        {
            EthApp_setConnectionType(Eth_GetMacPortNum(ctrlIdx),
                Eth_GetMiiConnectionType(ctrlIdx),
                ENET_CTRL_RGMII_ID_NODELAY);
        }
    }

    /* Equivalent to EcuM_AL_SetProgrammableInterrupts */
    EthApp_interruptConfig();

    /* Initialize counters, that would be required for timed operations */
    AppUtils_ProfileInit(0);

    /* Initialize memory sections  */
    AppUtils_EthSectionInit();
}

Std_ReturnType EthApp_initDma(void)
{
    Std_ReturnType retVal = E_OK;
    uint8 Idx;

    for(Idx = 0u; Idx < ETH_CTRL_ID_MAX; ++Idx)
    {
        retVal |= AppUtils_EthDmaOpen(Idx);  
    }

    return retVal;
}

Std_ReturnType EthApp_closeDma(void)
{
    Std_ReturnType retVal = E_OK;
    uint8 Idx;

    for(Idx = 0u; Idx < ETH_CTRL_ID_MAX; ++Idx)
    {
        retVal |= AppUtils_EthDmaClose(Idx);   
    }

    return retVal;
}

Std_ReturnType EthApp_initAll(void)
{
    Std_ReturnType retVal = E_OK;
    uint8 Idx;

    for(Idx = 0u; Idx < ETH_CTRL_ID_MAX; ++Idx)
    {
        retVal |= EthApp_init(Idx);   
    }

    return retVal;
}

Std_ReturnType EthApp_deinitAll(void)
{
    Std_ReturnType retVal = E_OK;
    uint8 Idx;

    for(Idx = 0u; Idx < ETH_CTRL_ID_MAX; ++Idx)
    {
        retVal |= EthApp_deinit(Idx);   
    }

    return retVal;
}

Std_ReturnType EthApp_showStatsAll(void)
{
    Std_ReturnType retVal = E_OK;
    uint8 Idx;

    for(Idx = 0u; Idx < ETH_CTRL_ID_MAX; ++Idx)
    {
        retVal |= EthApp_showStats(Idx);   
    }

    return retVal;
}

boolean EthApp_waitForHostAll(void)
{
    boolean retVal = TRUE;
    uint8 Idx;

    for(Idx = 0u; Idx < ETH_CTRL_ID_MAX; ++Idx)
    {
        retVal &= EthApp_waitForHost(Idx);   
    }

    return retVal; 
}

Std_ReturnType EthApp_initDriver(void)
{
    uint8 ci = 0u;
    const Eth_ConfigType *ethCfg = (const Eth_ConfigType *)NULL_PTR;
    const EthTrcv_ConfigType *ethTrcvCfg = (const EthTrcv_ConfigType *)NULL_PTR;
    Std_ReturnType retVal = E_OK;
    boolean needTrcv = FALSE;

    /* Eth driver initialization */
#if (STD_OFF == ETH_PRE_COMPILE_VARIANT)
    ethCfg = (const Eth_ConfigType *)&EthConfigSet_0;
    Eth_CfgPtr = (Eth_ConfigType *)&EthConfigSet_0;
#endif

#if (STD_ON == ETH_WRAPPER_ENABLED)
    Eth_30_Wrapper_Init(ethCfg);
#else
    Eth_Init(ethCfg);
#endif

    for(ci = 0u; ci < ETH_CTRL_ID_MAX; ++ci)
    {
        if(Eth_IsVirtualMacModeEnable(ci) == FALSE)
        {
            needTrcv = TRUE;
            break;
        }
    }

    if(TRUE == needTrcv)
    {
        /* Initialize the transceiver */
        ethTrcvCfg = ETHTRCV_DRV_CONFIG_0;
        retVal = EthApp_trcvInit(ethTrcvCfg);
        if (E_OK != retVal)
        {
            EthUtils_printf("init: failed to initialize transceiver\r\n");
        }
    }

    return retVal;
}

Std_ReturnType EthApp_init(uint8 ctrlIdx)
{
    Std_ReturnType retVal;

    /* Set controller to active mode */
#if (STD_ON == ETH_WRAPPER_ENABLED)
    retVal = Eth_30_Wrapper_SetControllerMode(ctrlIdx, ETH_MODE_ACTIVE);
#else
    retVal = Eth_SetControllerMode(ctrlIdx, ETH_MODE_ACTIVE);
#endif
    if (E_OK != retVal)
    {
        EthUtils_printf("init: failed to set the controller to active mode\r\n");
        return retVal;
    }

    /* Print initial physical address */
    if (E_OK == retVal)
    {
#if (STD_ON == ETH_WRAPPER_ENABLED)
        Eth_30_Wrapper_GetPhysAddr(ctrlIdx, gEthApp.state[ctrlIdx].macAddr);
#else
        Eth_GetPhysAddr(ctrlIdx, gEthApp.state[ctrlIdx].macAddr);
#endif
        EthUtils_printf("MAC Port %d Address: %02x:%02x:%02x:%02x:%02x:%02x\r\n",
                        ctrlIdx + 1,
                        gEthApp.state[ctrlIdx].macAddr[0U], gEthApp.state[ctrlIdx].macAddr[1U], gEthApp.state[ctrlIdx].macAddr[2U],
                        gEthApp.state[ctrlIdx].macAddr[3U], gEthApp.state[ctrlIdx].macAddr[4U], gEthApp.state[ctrlIdx].macAddr[5U]);
    }

    if (Eth_IsVirtualMacModeEnable(ctrlIdx) == FALSE)
    {
        /* Subscribe to spl mcast address used by host app to send mcast frames.
         * HostApp sends mcast frames with non-broadcast address so that
         * broadcast frames in network from Host PC/other machines on network
         * get routed to target test app inadvertently
         */
#if (STD_ON == ETH_UPDATE_PHYS_ADDR_FILTER_API)
        if (E_OK == retVal)
        {
#if (STD_ON == ETH_WRAPPER_ENABLED)
            retVal = Eth_30_Wrapper_UpdatePhysAddrFilter(ctrlIdx, TargetMcastAddr, ETH_ADD_TO_FILTER);
#else
            retVal = Eth_UpdatePhysAddrFilter(ctrlIdx, TargetMcastAddr, ETH_ADD_TO_FILTER);
#endif
        }
#endif
    }

    return retVal;
}

Std_ReturnType EthApp_trcvInit(const EthTrcv_ConfigType *cfg)
{
#if ((STD_ON == ETHTRCV_SETTRANSCEIVERMODE_API) || \
     (STD_ON == ETHTRCV_GETTRANSCEIVERMODE_API) || \
     (STD_ON == ETHTRCV_GETLINKSTATE_API) ||       \
     (STD_ON == ETHTRCV_GETBAUDRATE_API) ||        \
     (STD_ON == ETHTRCV_GETDUPLEXMODE_API))
    uint8 trcvIdx;
#endif
    Std_ReturnType retVal = E_OK;

    /* Initialize the EthTrcv driver */
    EthTrcv_Init(cfg);

    for(trcvIdx = 0u; trcvIdx < ETH_CTRL_ID_MAX; ++trcvIdx)
    {
        /* Set the transceiver mode to ACTIVE */
#if (STD_ON == ETHTRCV_SETTRANSCEIVERMODE_API)
        if (E_OK == retVal)
        {
            retVal = EthTrcv_SetTransceiverMode(trcvIdx, ETHTRCV_MODE_ACTIVE);
            if (E_OK != retVal)
            {
                EthUtils_printf("TrcvIdx %d: Failed to set trcv mode: %d\r\n", trcvIdx, retVal);
            }
        }
#else
        EthUtils_printf("EthTrcv_SetTransceiverMode API is not enabled\r\n");
#endif

        /* Read the mode to make sure the change took effect */
#if (STD_ON == ETHTRCV_GETTRANSCEIVERMODE_API)
        if (E_OK == retVal)
        {
            EthTrcv_ModeType trcvMode;

            retVal = EthTrcv_GetTransceiverMode(trcvIdx, &trcvMode);
            if (E_OK == retVal)
            {
                EthUtils_printf("TrcvIdx %d: EthTrcv mode: %s\r\n", trcvIdx, 
                                (trcvMode == ETHTRCV_MODE_ACTIVE) ? "ACTIVE" : "DOWN");
            }
            else
            {
                EthUtils_printf("TrcvIdx %d: Failed to get trcv mode: %d\r\n", trcvIdx, retVal);
            }
        }
#else
        EthUtils_printf("EthTrcv_GetTransceiverMode API is not enabled\r\n");
#endif

        /* Get the link state */
#if (STD_ON == ETHTRCV_GETLINKSTATE_API)
        if (E_OK == retVal)
        {
            EthTrcv_LinkStateType linkState;

            retVal = EthTrcv_GetLinkState(trcvIdx, &linkState);
            if (E_OK == retVal)
            {
                EthUtils_printf("TrcvIdx %d: EthTrcv link state: %s\r\n", trcvIdx, 
                                (linkState == ETHTRCV_LINK_STATE_ACTIVE) ? "Up" : "Down");
            }
            else
            {
                EthUtils_printf("Failed to get link state: %d\r\n", retVal);
            }
        }
#else
        EthUtils_printf("EthTrcv_GetLinkState API is not enabled\r\n");
#endif

        /* Get the baud rate */
#if (STD_ON == ETHTRCV_GETBAUDRATE_API)
        if (E_OK == retVal)
        {
            EthTrcv_BaudRateType baudRate;

            retVal = EthTrcv_GetBaudRate(trcvIdx, &baudRate);
            if (E_OK == retVal)
            {
                if (ETHTRCV_BAUD_RATE_10MBIT == baudRate)
                {
                    EthUtils_printf("TrcvIdx %d: EthTrcv baud rate: 10Mbps\r\n", trcvIdx);
                }
                else if (ETHTRCV_BAUD_RATE_100MBIT == baudRate)
                {
                    EthUtils_printf("TrcvIdx %d: EthTrcv baud rate: 100Mbps\r\n", trcvIdx);
                }
                else if (ETHTRCV_BAUD_RATE_1000MBIT == baudRate)
                {
                    EthUtils_printf("TrcvIdx %d: EthTrcv baud rate: 1000Mbps\r\n", trcvIdx);
                }
            }
            else
            {
                EthUtils_printf("TrcvIdx %d: Failed to get baud rate: %d\r\n", trcvIdx, retVal);
            }
        }
#else
        EthUtils_printf("EthTrcv_GetBaudRate API is not enabled\r\n");
#endif

        /* Get duplexity */
#if (STD_ON == ETHTRCV_GETDUPLEXMODE_API)
        if (E_OK == retVal)
        {
            EthTrcv_DuplexModeType duplexMode;

            retVal = EthTrcv_GetDuplexMode(trcvIdx, &duplexMode);
            if (E_OK == retVal)
            {
                EthUtils_printf("TrcvIdx %d: EthTrcv duplexity: %s\r\n", trcvIdx,
                                (duplexMode == ETHTRCV_DUPLEX_MODE_FULL) ? "Full" : "Half");
            }
            else
            {
                EthUtils_printf("TrcvIdx %d:Failed to get duplexity: %d\r\n", trcvIdx, retVal);
            }
        }
#else
        EthUtils_printf("EthTrcv_GetDuplexMode API is not enabled\r\n");
#endif
    }

    EthTrcv_MainFunction();

    return retVal;
}

Std_ReturnType EthApp_deinit(uint8 ctrlIdx)
{
    Std_ReturnType retVal;

    /* Set controller to down mode */
#if (STD_ON == ETH_WRAPPER_ENABLED)
    retVal = Eth_30_Wrapper_SetControllerMode(ctrlIdx, ETH_MODE_DOWN);
#else
    retVal = Eth_SetControllerMode(ctrlIdx, ETH_MODE_DOWN);
#endif

    if (E_OK != retVal)
    {
        EthUtils_printf("deinit: failed to set the controller to down mode\r\n");
    }

    return retVal;
}

BufReq_ReturnType EthApp_provideTxBuffer(uint8 ctrlIdx,
                                                uint8 priority,
                                                Eth_BufIdxType* bufIdxPtr,
                                                uint8** bufPtr,
                                                uint16* lenBytePtr)
{
    BufReq_ReturnType bufStatus = BUFREQ_E_NOT_OK;
    boolean retry = FALSE;

    /* Request one buffer, retry if not available */
    do
    {
#if (STD_ON == ETH_ZERO_COPY_API)
        if(gEthApp.state[ctrlIdx].useExtTxBuffer == FALSE)
        {
#if (STD_ON == ETH_WRAPPER_ENABLED)
            bufStatus = Eth_30_Wrapper_ProvideTxBuffer(ctrlIdx,
                                            (uint8*)bufIdxPtr,
                                            (Eth_DataType**)bufPtr,
                                            lenBytePtr);
#else
            bufStatus = Eth_ProvideTxBuffer(ctrlIdx,
                                            priority,
                                            bufIdxPtr,
                                            bufPtr,
                                            lenBytePtr);
#endif
        }
        else
        {
            *lenBytePtr += ETH_HDR_LEN;
            *bufPtr =  EthApp_ExtTxBuffer[ctrlIdx];
#if (STD_ON == ETH_WRAPPER_ENABLED)
            bufStatus = Eth_30_Wrapper_ProvideExtTxBuffer(ctrlIdx,
                                               bufIdxPtr,
                                               (Eth_DataType**)bufPtr,
                                               lenBytePtr);
#else
            bufStatus = Eth_ProvideExtTxBuffer(ctrlIdx,
                                               priority,
                                               bufIdxPtr,
                                               (Eth_DataType**)bufPtr,
                                               lenBytePtr);
#endif
        }
#else
#if (STD_ON == ETH_WRAPPER_ENABLED)
        bufStatus = Eth_30_Wrapper_ProvideTxBuffer(ctrlIdx,
                                            (uint8*)bufIdxPtr,
                                            (Eth_DataType**)bufPtr,
                                            lenBytePtr);
#else
        bufStatus = Eth_ProvideTxBuffer(ctrlIdx,
                                        priority,
                                        bufIdxPtr,
                                        bufPtr,
                                        lenBytePtr);
#endif
#endif /* (STD_ON == ETH_ZERO_COPY_API) */

        if (BUFREQ_E_BUSY == bufStatus)
        {
            /* Temporarily no buffer available. As per spec, it's up to the
             * requester to retry request for a certain time */
            retry = TRUE;

            if(Eth_GetTxEnableInterrupt(ctrlIdx) == FALSE)
            {
                /* Free TX packets are recycled during Eth_TxConfirmation(),
                 * which would normally be called periodically by EthIf, but since
                 * we don't have an actual EthIf implemented in this test app,
                 * we just call it here to let descriptor recycling happen. */
#if (STD_ON == ETH_WRAPPER_ENABLED)
                Eth_30_Wrapper_TxConfirmation(ctrlIdx);
#else
                Eth_TxConfirmation(ctrlIdx);
#endif
            }
        }
        else
        {
            retry = FALSE;
        }
    }
    while (retry == TRUE);

    return bufStatus;
}
boolean EthApp_transmit(uint8 ctrlIdx,
                               uint32 num,
                               uint16 len,
                               boolean txConfirmation,
                               boolean vlan,
                               uint8 priority,
                               uint8 *dstAddr)
{
    Eth_FrameType frameType;
    Eth_BufIdxType bufIdx;
    BufReq_ReturnType bufStatus;
    uint16 grantedLen = len;
    uint8 *bufPtr;
    uint32 retryCnt;
    uint32 i;
    sint32 fillStatus;
    boolean randomBufLen;
    boolean status = PASS;
    Std_ReturnType retVal;

    randomBufLen = (len == 0U) ? TRUE : FALSE;

    /* Reset test counters */
    gEthApp.state[ctrlIdx].stats.txPktCnt     = 0U;
    gEthApp.state[ctrlIdx].stats.txConfPktCnt = 0U;
    gEthApp.state[ctrlIdx].stats.txBufLenErr  = 0U;
    gEthApp.state[ctrlIdx].stats.txBufProvErr = 0U;
    gEthApp.state[ctrlIdx].stats.txPktErr     = 0U;
    gEthApp.state[ctrlIdx].stats.detErr       = 0U;
    gEthApp.state[ctrlIdx].stats.demErr       = 0U;

    /* Transmit the requested number of buffers */
    for (i = 0U; i < num; i++)
    {
        if (randomBufLen)
        {
            grantedLen = EthUtils_rand(ETH_TEST_BUF_LEN_MIN,
                                       ETH_TEST_BUF_LEN_MAX);
        }
        else
        {
            grantedLen = len;
        }

        if (vlan)
        {
            grantedLen += ETH_VLAN_TAG_LEN;
        }

        /* Request TX buffer */
        bufStatus = EthApp_provideTxBuffer(ctrlIdx,
                                           priority,
                                           &bufIdx,
                                           &bufPtr,
                                           &grantedLen);
        if (BUFREQ_E_OVFL == bufStatus)
        {
            EthUtils_printf("transmit: failed to get exact size (req=%d got=%d)\r\n",
                            len, grantedLen);
            gEthApp.state[ctrlIdx].stats.txBufLenErr++;
            status = FAIL;
        }
        else if (BUFREQ_E_NOT_OK == bufStatus)
        {
            EthUtils_printf("transmit: failed to get buffer\r\n");
            gEthApp.state[ctrlIdx].stats.txBufProvErr++;
            status = FAIL;
        }
        else if (bufPtr == NULL_PTR)
        {
            EthUtils_printf("transmit: invalid buffer pointer\r\n");
            gEthApp.state[ctrlIdx].stats.txBufProvErr++;
            status = FAIL;
        }

        /* Exit test if error is too critical */
        if (FAIL == status)
        {
            break;
        }

        /* Fill buffer with test pattern */
        if (vlan)
        {
            frameType = ETHERTYPE_VLAN_TAG;
            fillStatus = EthUtils_fillVlanPayload((VlanDataFramePayload *)bufPtr,
                                                  i % ETH_TEST_NUM_TYPES,
                                                  grantedLen,
                                                  ETH_TEST_VLAN_PCP,
                                                  ETH_TEST_VLAN_VID,
                                                  ETHERTYPE_EXPERIMENTAL1);

            /* We are sending very high prioriy packet (PCP = 5) with max rate,
             * so it sometimes make host(Linux side) not able to process
             * with this rate and drop some frame, we can reduce rate for this test
             * abit to make host happy
             */
            EthApp_delay(1u);
        }
        else
        {
            frameType = ETHERTYPE_EXPERIMENTAL1;
            fillStatus = EthUtils_fillPayload((DataFramePayload *)bufPtr,
                                              i % ETH_TEST_NUM_TYPES,
                                              grantedLen);
        }

        if (ETH_TEST_PKT_SOK != fillStatus)
        {
            EthUtils_printf("transmit: failed to fill test packet\r\n");
            status = FAIL;
            break;
        }

        /* Transmit the packet */
        retryCnt = txConfirmation ? 0U : ETHAPP_TRANSMIT_RETRY_COUNT;
        do
        {
#if (STD_ON == ETH_WRAPPER_ENABLED)
            retVal = Eth_30_Wrapper_Transmit(ctrlIdx,
                                  bufIdx,
                                  frameType,
                                  txConfirmation,
                                  grantedLen,
                                  dstAddr);
#else
            retVal = Eth_Transmit(ctrlIdx,
                                  bufIdx,
                                  frameType,
                                  txConfirmation,
                                  grantedLen,
                                  dstAddr);
#endif
            if (E_OK == retVal)
            {
                break;
            }

            if (retryCnt > 0U)
            {
                EthApp_delay(1U);
                retryCnt--;
            }
            else
            {
                break;
            }
        }
        while (TRUE);

        if (E_OK != retVal)
        {
            EthUtils_printf("transmit: failed to transmit buffer with idx %d\r\n",
                            (sint32)bufIdx);
            gEthApp.state[ctrlIdx].stats.txPktErr++;
        }
        else
        {
            gEthApp.state[ctrlIdx].stats.txPktCnt++;
        }

        if(Eth_GetTxEnableInterrupt(ctrlIdx) == FALSE)
        {
#if (STD_ON == ETH_WRAPPER_ENABLED)
            Eth_30_Wrapper_TxConfirmation(ctrlIdx);
#else
            Eth_TxConfirmation(ctrlIdx);
#endif
        }
    }

    if (TRUE == gEthApp.state[ctrlIdx].sendMultipleStopCmd)
    {
        /* Send STOP cmd 10x to prevent lost,
         * receiver app will handle flushing the extra STOP cmd */
        for (i = 0U; i < ETH_TEST_ITER_S_COUNT; i++)
        {
            EthApp_sendCmd(ctrlIdx, CTRL_FRAME_CMD_STOP);
        }
    }
    else
    {
        /* Send STOP cmd */
        EthApp_sendCmd(ctrlIdx, CTRL_FRAME_CMD_STOP);
    }

    /* If no error on Tx API, check Tx Stats */
    if (PASS == status)
    {
        status = EthApp_verifyTxStatus(ctrlIdx, num, txConfirmation);
    }

    return status;
}

void EthApp_send(uint8 ctrlIdx,
                        EthFrame *frame,
                        uint16 len,
                        uint8 priority)
{
    Eth_FrameType frameType = frame->hdr.etherType;
    Eth_BufIdxType bufIdx;
    BufReq_ReturnType bufStatus;
    boolean txConfirmation = FALSE;
    uint8 *bufPtr;
    Std_ReturnType retVal;

    bufStatus = EthApp_provideTxBuffer(ctrlIdx,
                                       priority,
                                       &bufIdx,
                                       &bufPtr,
                                       &len);

    if (BUFREQ_OK == bufStatus)
    {
        memcpy(bufPtr, frame->payload, len);
#if (STD_ON == ETH_WRAPPER_ENABLED)
        retVal = Eth_30_Wrapper_Transmit(ctrlIdx,
                              bufIdx,
                              frameType,
                              txConfirmation,
                              len,
                              frame->hdr.dstMac);
#else
        retVal = Eth_Transmit(ctrlIdx,
                              bufIdx,
                              frameType,
                              txConfirmation,
                              len,
                              frame->hdr.dstMac);
#endif
        if (E_OK != retVal)
        {
            EthUtils_printf("send: failed to send buffer\r\n");
        }
    }
    else
    {
        EthUtils_printf("send: failed to get TX buffer: %d\r\n", (int)bufStatus);
    }

    if(Eth_GetTxEnableInterrupt(ctrlIdx) == FALSE)
    {
#if (STD_ON == ETH_WRAPPER_ENABLED)
        Eth_30_Wrapper_TxConfirmation(ctrlIdx);
#else
        Eth_TxConfirmation(ctrlIdx);
#endif
    }
}

void EthApp_sendCmd(uint8 ctrlIdx,
                           uint8 cmd)
{
    CtrlFramePayload *control;
    Eth_FrameType frameType = ETHERTYPE_EXP_CONTROL;
    Eth_BufIdxType bufIdx;
    BufReq_ReturnType bufStatus;
    boolean txConfirmation = FALSE;
    uint16 len = sizeof(CtrlFramePayload);
    uint8 *bufPtr;
    Std_ReturnType retVal;

    bufStatus = EthApp_provideTxBuffer(ctrlIdx,
                                       ETH_DEFAULT_TX_PRIORITY,
                                       &bufIdx,
                                       &bufPtr,
                                       &len);
    if (BUFREQ_OK == bufStatus)
    {
        control = (CtrlFramePayload *)bufPtr;
        control->cmd = cmd;
#if (STD_ON == ETH_WRAPPER_ENABLED)
        retVal = Eth_30_Wrapper_Transmit(ctrlIdx,
                              bufIdx,
                              frameType,
                              txConfirmation,
                              len,
                              BcastAddr);
#else
        retVal = Eth_Transmit(ctrlIdx,
                              bufIdx,
                              frameType,
                              txConfirmation,
                              len,
                              BcastAddr);
#endif
        if (E_OK != retVal)
        {
            EthUtils_printf("sendCmd: failed to send STOP cmd\r\n");
        }
    }
    else
    {
        EthUtils_printf("sendCmd: failed to get TX buffer: %d\r\n", (int)bufStatus);
    }

    if(Eth_GetTxEnableInterrupt(ctrlIdx) == FALSE)
    {
#if (STD_ON == ETH_WRAPPER_ENABLED)
        Eth_30_Wrapper_TxConfirmation(ctrlIdx);
#else
        Eth_TxConfirmation(ctrlIdx);
#endif
    }
}


boolean EthApp_receive(uint8 ctrlIdx, uint32 num)
{
    boolean forever;
    boolean status = PASS;

    /* Reset test counters */
    gEthApp.state[ctrlIdx].stats.rxPktCnt       = 0U;
    gEthApp.state[ctrlIdx].stats.rxExp1PktCnt   = 0U;
    gEthApp.state[ctrlIdx].stats.rxExp2PktCnt   = 0U;
    gEthApp.state[ctrlIdx].stats.rxVlanPktCnt   = 0U;
    gEthApp.state[ctrlIdx].stats.rxPktNotRecv   = 0U;
    gEthApp.state[ctrlIdx].stats.rxPktIndCnt    = 0U;
    gEthApp.state[ctrlIdx].stats.rxEtherTypeErr = 0U;
    gEthApp.state[ctrlIdx].stats.rxPayloadErr   = 0U;
    gEthApp.state[ctrlIdx].stats.rxCtrlIdxErr   = 0U;
    gEthApp.state[ctrlIdx].stats.detErr         = 0U;
    gEthApp.state[ctrlIdx].stats.demErr         = 0U;

    forever = (num == 0U) ? TRUE : FALSE;

    gEthApp.state[ctrlIdx].runTest = TRUE;

    /* Wait for Rx frames */
    while (gEthApp.state[ctrlIdx].runTest)
    {
        if(Eth_GetRxEnableInterrupt(ctrlIdx) == FALSE)
        {
            EthApp_receiveAllFifo(ctrlIdx);
        }
        if (!forever && (gEthApp.state[ctrlIdx].stats.rxPktIndCnt >= num))
        {
            break;
        }
    }

    if ((gEthApp.state[ctrlIdx].stats.rxPktIndCnt != gEthApp.state[ctrlIdx].stats.rxPktCnt) ||
        (gEthApp.state[ctrlIdx].stats.rxEtherTypeErr > 0U) ||
        (gEthApp.state[ctrlIdx].stats.rxPayloadErr > 0U) ||
        (gEthApp.state[ctrlIdx].stats.rxCtrlIdxErr > 0U) ||
        (gEthApp.state[ctrlIdx].stats.detErr > 0U) ||
        (gEthApp.state[ctrlIdx].stats.demErr > 0U))
    {
        status = FAIL;
    }

    if ((FAIL == status) || (TRUE == gEthApp.state[ctrlIdx].verbose))
    {
        EthUtils_printf("Receive Test Results:\r\n");
        EthUtils_printf("  Status:                %s\r\n", status?"Pass":"Fail");
        EthUtils_printf("  Requested:             %d\r\n", num);
        EthUtils_printf("  Received:              %d\r\n", gEthApp.state[ctrlIdx].stats.rxPktCnt);
        EthUtils_printf("  Indicated:             %d\r\n", gEthApp.state[ctrlIdx].stats.rxPktIndCnt);
        EthUtils_printf("  Experimental 1:        %d\r\n", gEthApp.state[ctrlIdx].stats.rxExp1PktCnt);
        EthUtils_printf("  Experimental 2:        %d\r\n", gEthApp.state[ctrlIdx].stats.rxExp2PktCnt);
        EthUtils_printf("  VLAN tagged:           %d\r\n", gEthApp.state[ctrlIdx].stats.rxVlanPktCnt);
        EthUtils_printf("  Not received:          %d\r\n", gEthApp.state[ctrlIdx].stats.rxPktNotRecv);
        EthUtils_printf("  EtherType errors:      %d\r\n", gEthApp.state[ctrlIdx].stats.rxEtherTypeErr);
        EthUtils_printf("  Payload errors:        %d\r\n", gEthApp.state[ctrlIdx].stats.rxPayloadErr);
        EthUtils_printf("  Controller idx errors: %d\r\n", gEthApp.state[ctrlIdx].stats.rxCtrlIdxErr);
        EthUtils_printf("  DEM errors:            %d\r\n", gEthApp.state[ctrlIdx].stats.demErr);
        EthUtils_printf("  DET errors:            %d\r\n", gEthApp.state[ctrlIdx].stats.detErr);
        EthUtils_printf("\r\n");
    }

    return status;
}

boolean EthApp_waitForHost(uint8 ctrlIdx)
{
    boolean status = FAIL;
    int retryCount = 100;

    /* Emulate TCP's Initial Handshake to make sure both side's interface is
     * up and running.
     *  [DUT]      [HOST]
     *    |--- READY->|
     *    |<-- SACK --|
     *    |--- ACK -->|
     */
    EthUtils_printf("Waiting for Host..\r\n");
    gEthApp.state[ctrlIdx].hostUp = FALSE;

    do {
        /* Send command indicating that eth interface is up and ready to
         * received the acknowledgment command */
        EthApp_sendCmd(ctrlIdx, CTRL_FRAME_CMD_READY);
        if(Eth_GetRxEnableInterrupt(ctrlIdx) == FALSE)
        {
            EthApp_receiveAllFifo(ctrlIdx);
        }

        if (!(retryCount % 10) && !gEthApp.state[ctrlIdx].hostUp)
        {
            EthUtils_printf("No Response from Host, remaining retry = %u\r\n",
                            retryCount);
        }
        EthApp_delay(1000u);
        retryCount--;
    } while(!gEthApp.state[ctrlIdx].hostUp && retryCount > 0);

    /* Send command to indicating that we received the acknowledgement
     * we're ready to start the test stage */
    if (gEthApp.state[ctrlIdx].hostUp)
    {
        EthUtils_printf("Host App detected! Sending Read ACK CMD...\r\n");
        EthApp_sendCmd(ctrlIdx, CTRL_FRAME_CMD_READY_ACK);
        status = PASS;
    }
    else
    {
        EthUtils_printf("ERROR: NO Host App detected after 100 retries!\r\n");
    }

    return status;
}

void EthApp_receiveAllFifo(uint8 ctrlIdx)
{
    uint8 fifoIdx = 0u;
    Eth_RxStatusType rxStatus;

    for(fifoIdx = 0u; fifoIdx < Eth_GetIngressFifoTotalNum(ctrlIdx); fifoIdx++)
    {
#if (STD_ON == ETH_WRAPPER_ENABLED)
        (void)Eth_30_Wrapper_Receive(ctrlIdx, &rxStatus);
#else
        (void)Eth_Receive(ctrlIdx, fifoIdx, &rxStatus);
#endif
    }
}

void EthApp_transmitFlush(uint8 ctrlIdx)
{
    uint8 fifoIdx=0u;
    uint16 maxPacket=0u;
    uint16 pktIdx=0u;

    for(fifoIdx = 0u; fifoIdx < Eth_GetEgressFifoTotalNum(ctrlIdx); ++fifoIdx)
    {
        maxPacket += Eth_GetEgressFifoPacketNum(ctrlIdx, fifoIdx);
    }

    for(pktIdx = 0u; pktIdx < maxPacket; ++pktIdx)
    {
        EthApp_delay(2u);

        if(Eth_GetTxEnableInterrupt(ctrlIdx) == FALSE)
        {
#if (STD_ON == ETH_WRAPPER_ENABLED)
            Eth_30_Wrapper_TxConfirmation(ctrlIdx);
#else
            Eth_TxConfirmation(ctrlIdx);
#endif
        }
    }
}

void EthApp_flushRecvQueue(uint8 ctrlIdx)
{
    Eth_RxStatusType rxStatus;
    uint32 pktIdx;
    uint8 fifoIdx = 0U;

    for(fifoIdx = 0u; fifoIdx < Eth_GetIngressFifoTotalNum(ctrlIdx); ++fifoIdx)
    {
        for(pktIdx = 0; pktIdx < Eth_GetEgressFifoPacketNum(ctrlIdx, fifoIdx); ++pktIdx)
        {
            EthApp_delay(2);

            if(Eth_GetRxEnableInterrupt(ctrlIdx) == FALSE)
            {
#if (STD_ON == ETH_WRAPPER_ENABLED)
                (void)Eth_30_Wrapper_Receive(ctrlIdx, &rxStatus);
#else
                (void)Eth_Receive(ctrlIdx, fifoIdx, &rxStatus);
#endif
            }
        }
    }
}

void EthApp_notifyCmdRecv(uint8 ctrlIdx)
{
    gEthApp.state[ctrlIdx].ackRecv = FALSE;

    /* Notify Host that we've received the Stop Command */
    EthApp_sendCmd(ctrlIdx, CTRL_FRAME_CMD_RECV_SYN_ACK);

    /* Wait for Host to acknowledge our SYN ACK */
    while (!gEthApp.state[ctrlIdx].ackRecv)
    {
        if(Eth_GetRxEnableInterrupt(ctrlIdx) == FALSE)
        {
            EthApp_receiveAllFifo(ctrlIdx);
        }
    }
}

void EthApp_delay(uint32 msecs)
{
    TickType count = OS_TIME_SCALE_VALUE * msecs;
    TickType start = 0U;
    TickType tempCount = 0U;
    TickType elapsed = 0U;
    StatusType status;

    status = GetCounterValue(ETH_OS_COUNTER_ID, &start);

    while ((E_OK == status) && (count >= elapsed))
    {
        tempCount = start;
        status = GetElapsedValue(ETH_OS_COUNTER_ID, &tempCount, &elapsed);
    }
}

#if (STD_ON == ETHTRCV_GETLINKSTATE_API)
Std_ReturnType EthApp_getTranceiverLinkState(uint8 trcvIdx)
{
    Std_ReturnType retVal = E_OK;

    /* Get the link state */
    if (E_OK == retVal)
    {
        EthTrcv_LinkStateType linkState;

        retVal = EthTrcv_GetLinkState(trcvIdx, &linkState);
        if (E_OK == retVal)
        {
            EthUtils_printf("EthTrcv link state: %s\r\n",
                      (linkState == ETHTRCV_LINK_STATE_ACTIVE) ? "Up" : "Down");
        }
        else
        {
            EthUtils_printf("Failed to get link state: %d\r\n", retVal);
        }
    }

    return retVal;
}
#endif /* (STD_ON == ETHTRCV_GETLINKSTATE_API) */

Std_ReturnType EthApp_showStats(uint8 ctrlIdx)
{
    Std_ReturnType retVal;
#if (STD_ON == ETH_GET_COUNTER_VALUES_API)
    Eth_CounterType ethCounter;
    retVal = Eth_GetCounterValues(ctrlIdx, &ethCounter);
    if (E_OK == retVal)
    {
        EthUtils_printf("----------------------------\r\n");
        EthUtils_printf("Controller %d Counters\r\n", ctrlIdx);
        EthUtils_printf("----------------------------\r\n");
        EthUtils_printf("Buffer overruns   : %d\r\n", ethCounter.DropPktBufOverrun);
        EthUtils_printf("CRC errors        : %d\r\n", ethCounter.DropPktCrc);
        EthUtils_printf("Undersize packets : %d\r\n", ethCounter.UndersizePkt);
        EthUtils_printf("Oversized packets : %d\r\n", ethCounter.OversizePkt);
        EthUtils_printf("Alignment errors  : %d\r\n", ethCounter.AlgnmtErr);
        EthUtils_printf("SQE errors        : %d\r\n", ethCounter.SqeTestErr);
        EthUtils_printf("Discarded inbound : %d\r\n", ethCounter.DiscInbdPkt);
        EthUtils_printf("Erroneous inbound : %d\r\n", ethCounter.ErrInbdPkt);
        EthUtils_printf("Discarded outbound: %d\r\n", ethCounter.DiscOtbdPkt);
        EthUtils_printf("Erroneous outbound: %d\r\n", ethCounter.ErrOtbdPkt);
        EthUtils_printf("Single collision  : %d\r\n", ethCounter.SnglCollPkt);
        EthUtils_printf("Multiple collision: %d\r\n", ethCounter.MultCollPkt);
        EthUtils_printf("Deferred transm   : %d\r\n", ethCounter.DfrdPkt);
        EthUtils_printf("Late collisions   : %d\r\n", ethCounter.LatCollPkt);
        EthUtils_printf("Excessive colls   : %d\r\n", ethCounter.HwDepCtr0);
        EthUtils_printf("Carrier loss      : %d\r\n", ethCounter.HwDepCtr1);
    }
    else
    {
        EthUtils_printf("showCountValues: failed to get counters: %d\r\n", retVal);
        gEthApp.state[ctrlIdx].stats.etherGetCountValuesErr++;
    }
#else
    EthUtils_printf("showCountValues: Eth_GetCounterValues() API not enabled\r\n");
    retVal = E_NOT_OK;
#endif

#if (STD_ON == ETH_GET_RX_STATS_API)
    Eth_RxStatsType rxStats;
#if (STD_ON == ETH_WRAPPER_ENABLED)
    retVal = Eth_30_Wrapper_GetRxStats(ctrlIdx, &rxStats);
#else
    retVal = Eth_GetRxStats(ctrlIdx, &rxStats);
#endif
    if (E_OK == retVal)
    {
        EthUtils_printf("----------------------------\r\n");
        EthUtils_printf("Controller %d Rx Statistics\r\n", ctrlIdx);
        EthUtils_printf("----------------------------\r\n");
        EthUtils_printf("Rx Drop Events       : %d\r\n", rxStats.RxStatsDropEvents);
        EthUtils_printf("Rx Octets            : %d\r\n", rxStats.RxStatsOctets);
        EthUtils_printf("Rx Packets           : %d\r\n", rxStats.RxStatsPkts);
        EthUtils_printf("Rx Bcast Packets     : %d\r\n", rxStats.RxStatsBroadcastPkts);
        EthUtils_printf("Rx Mcast Packets     : %d\r\n", rxStats.RxStatsMulticastPkts);
        EthUtils_printf("Rx CRC/Align Errors  : %d\r\n", rxStats.RxStatsCrcAlignErrors);
        EthUtils_printf("Rx Undersized Packets: %d\r\n", rxStats.RxStatsUndersizePkts);
        EthUtils_printf("Rx Oversized Packets : %d\r\n", rxStats.RxStatsOversizePkts);
        EthUtils_printf("Rx Fragments         : %d\r\n", rxStats.RxStatsFragments);
        EthUtils_printf("Rx Jabbers           : %d\r\n", rxStats.RxStatsJabbers);
        EthUtils_printf("Rx Collisions        : %d\r\n", rxStats.RxStatsCollisions);
        EthUtils_printf("Rx 64 bytes or less Packets   : %d\r\n", rxStats.RxStatsPkts64Octets);
        EthUtils_printf("Rx 65 to 127 bytes Packets    : %d\r\n", rxStats.RxStatsPkts65to127Octets);
        EthUtils_printf("Rx 128 to 255 bytes Packets   : %d\r\n", rxStats.RxStatsPkts128to255Octets);
        EthUtils_printf("Rx 256 to 511 bytes Packets   : %d\r\n", rxStats.RxStatsPkts256to511Octets);
        EthUtils_printf("Rx 512 to 1023 bytes Packets  : %d\r\n", rxStats.RxStatsPkts512to1023Octets);
        EthUtils_printf("Rx 1024 to 1518 bytes Packets : %d\r\n", rxStats.RxStatsPkts1024to1518Octets);
        EthUtils_printf("Rx Unicast Packets            : %d\r\n", rxStats.RxUnicastFrames);
    }
    else
    {
        EthUtils_printf("getRxStats: failed to get counters: %d\r\n", retVal);
        gEthApp.state[ctrlIdx].stats.etherGetRxStasErr++;
    }
#else
    EthUtils_printf("getRxStats: Eth_GetRxStats() API not enabled\r\n");
    retVal = E_NOT_OK;
#endif

#if (STD_ON == ETH_GET_TX_STATS_API)
    Eth_TxStatsType txStats;
#if (STD_ON == ETH_WRAPPER_ENABLED)
    retVal = Eth_30_Wrapper_GetTxStats(ctrlIdx, &txStats);
#else
    retVal = Eth_GetTxStats(ctrlIdx, &txStats);
#endif
    if (E_OK == retVal)
    {
        EthUtils_printf("----------------------------\r\n");
        EthUtils_printf("Controller %d Tx Statistics \r\n", ctrlIdx);
        EthUtils_printf("----------------------------\r\n");
        EthUtils_printf("Tx Num octets          : %d\r\n", txStats.TxNumberOfOctets);
        EthUtils_printf("Tx No Unicast Packets  : %d\r\n", txStats.TxNUcastPkts);
        EthUtils_printf("Tx Unicast Packets     : %d\r\n", txStats.TxUniCastPkts);
    }
    else
    {
        EthUtils_printf("getTxStats: failed to get TX stats: %d\r\n", retVal);
        gEthApp.state[ctrlIdx].stats.etherGetTxStasErr++;
    }
#else
    EthUtils_printf("getTxStats: Eth_GetTxStats() API not enabled\r\n");
    retVal = E_NOT_OK;
#endif

#if (STD_ON == ETH_GET_TX_ERROR_COUNTERSVALUES_API)
    Eth_TxErrorCounterValuesType txErrorCounterValues;
    retVal = Eth_GetTxErrorCounterValues(ctrlIdx, &txErrorCounterValues);
    if (E_OK == retVal)
    {
        EthUtils_printf("--------------------------------\r\n");
        EthUtils_printf("Controller %d Tx Error Counters \r\n", ctrlIdx);
        EthUtils_printf("--------------------------------\r\n");
        EthUtils_printf("Tx Deferred transm      : %d\r\n", txErrorCounterValues.TxDeferredTrans);
        EthUtils_printf("Tx Single collisions    : %d\r\n", txErrorCounterValues.TxSingleCollision);
        EthUtils_printf("Tx Multiple collisions  : %d\r\n", txErrorCounterValues.TxMultipleCollision);
        EthUtils_printf("Tx Late collisions      : %d\r\n", txErrorCounterValues.TxLateCollision);
        EthUtils_printf("Tx Excessive collisions : %d\r\n", txErrorCounterValues.TxExcessiveCollison);
    }
    else
    {
        EthUtils_printf("showTxErrStatsCount: failed to get counters: %d\r\n", retVal);
        gEthApp.state[ctrlIdx].stats.etherGetTxErrStasErr++;
    }
#else
    EthUtils_printf("showTxErrStatsCount: Eth_GetTxErrorCounterValues() API not enabled\r\n");
    retVal = E_NOT_OK;
#endif

#if (STD_ON == ETH_GET_COUNTER_STATE_API)
    for (uint16 i = 0; i < CPSW_STATS_BLOCK_ELEM_NUM; i++)
    {
        uint32 CtrValue = 0U;

#if (STD_ON == ETH_WRAPPER_ENABLED)
        retVal = Eth_30_Wrapper_GetCounterState(ctrlIdx, i, &CtrValue);
#else
        retVal = Eth_GetCounterState(ctrlIdx, i, &CtrValue);
#endif
        if (E_OK == retVal)
        {
            if (i == 0)
            {
                EthUtils_printf("-------------------------------------\r\n");
                EthUtils_printf("Controller %d Counter Register States\r\n", ctrlIdx);
                EthUtils_printf("-------------------------------------\r\n");
            }

            EthUtils_printf("CtrOffs: %3d %20s: %d\r\n", i, CtrOffsToName[i], CtrValue);
        }
        else
        {
            EthUtils_printf("CtrOffs: %3d %20s: <Err>\r\n", i, CtrOffsToName[i]);
        }
        retVal = E_OK;
    }
#else
    (void)CtrOffsToName;
#endif
    return retVal;
}

#if (STD_ON == ETH_GLOBALTIMESUPPORT_API)
void EthApp_GetCurrentTime(uint8 ctrlIdx,
    Eth_TimeStampQualType *timeQualPtr, Eth_TimeStampType *tsPtr)
{
#if (STD_ON == ETH_WRAPPER_ENABLED)
    Eth_30_Wrapper_GetGlobalTime(ctrlIdx, tsPtr, timeQualPtr);
#else
    Eth_GetCurrentTime(ctrlIdx, timeQualPtr, tsPtr);
#endif
}

void EthApp_EnableEgressTimeStamp(uint8 ctrlIdx, Eth_BufIdxType bufIdx)
{
#if (STD_ON == ETH_WRAPPER_ENABLED)
    Eth_EnableEgressTimeStamp(ctrlIdx, bufIdx);
#else
    Eth_EnableEgressTimeStamp(ctrlIdx, bufIdx);
#endif
}
#endif

Std_ReturnType EthApp_SetControllerMode(uint8 ctrlIdx, Eth_ModeType ctrlMode)
{
    Std_ReturnType retVal = E_NOT_OK;

#if (STD_ON == ETH_WRAPPER_ENABLED)
    retVal = Eth_30_Wrapper_SetControllerMode(ctrlIdx, ctrlMode);
#else
    retVal = Eth_SetControllerMode(ctrlIdx, ctrlMode);
#endif
    return retVal;
}

Std_ReturnType EthApp_GetControllerMode(uint8 ctrlIdx, Eth_ModeType *ctrlMode)
{
    Std_ReturnType retVal = E_NOT_OK;

#if (STD_ON == ETH_WRAPPER_ENABLED)
    retVal = Eth_30_Wrapper_GetControllerMode(ctrlIdx, ctrlMode);
#else
    retVal = Eth_GetControllerMode(ctrlIdx, ctrlMode);
#endif
    return retVal;
}

Std_ReturnType EthApp_Transmit(uint8 ctrlIdx,
             Eth_BufIdxType bufIdx,
             Eth_FrameType frameType,
             boolean txConfirmation,
             uint16 lenByte,
             P2CONST(uint8, AUTOMATIC, ETH_APPL_DAT) physAddrPtr)
{
    Std_ReturnType retVal = E_NOT_OK;

#if (STD_ON == ETH_WRAPPER_ENABLED)
    retVal = Eth_30_Wrapper_Transmit(ctrlIdx,
                bufIdx,
                frameType,
                txConfirmation,
                lenByte,
                physAddrPtr);
#else
    retVal = Eth_Transmit(ctrlIdx,
                bufIdx,
                frameType,
                txConfirmation,
                lenByte,
                physAddrPtr);
#endif
    return retVal;
}

void EthApp_TxConfirmationInt(uint8 ctrlIdx)
{
#if (STD_ON == ETH_WRAPPER_ENABLED)
    Eth_30_Wrapper_TxConfirmation(ctrlIdx);
#else
    Eth_TxConfirmation(ctrlIdx);
#endif
}

/* ========================================================================== */
/*                          External User function Definition                 */
/* ========================================================================== */

void EthApp_CtrlModeIndication(uint8 ctrlIdx,
                               Eth_ModeType ControllerMode)
{
    if (ETH_MODE_DOWN == ControllerMode)
    {
        EthUtils_printf("EthApp_CtrlModeIndication: DOWN\r\n");
        gEthApp.state[ctrlIdx].stats.ctrlModeIndDownCnt++;
    }
    else if (ETH_MODE_ACTIVE == ControllerMode)
    {
        EthUtils_printf("EthApp_CtrlModeIndication: ACTIVE\r\n");
        gEthApp.state[ctrlIdx].stats.ctrlModeIndActCnt++;
    }
    else
    {
        EthUtils_printf("EthApp_CtrlModeIndication: invalid mode\r\n");
        gEthApp.state[ctrlIdx].stats.ctrlModeIndErr++;
    }
}

void EthApp_TrcvModeIndication(uint8 ctrlIdx,
                               EthTrcv_ModeType TrcvMode)
{
    EthUtils_printf("EthApp_TrcvModeIndication: %s\r\n",
                    (ETHTRCV_MODE_ACTIVE == TrcvMode) ? "Active" : "Down");
}

void EthApp_RxIndication(uint8 ctrlIdx,
                         Eth_FrameType FrameType,
                         boolean IsBroadcast,
                         uint8 *PhysAddrPtr,
                         Eth_DataType *DataPtr,
                         uint16 lenByte)
{
    uint8 pcp = ETH_TEST_VLAN_PCP;
    uint16 vid = ETH_TEST_VLAN_VID;
    CtrlFramePayload *ctrlFramePtr;
    sint32 ret;
#if (STD_ON == ETH_GLOBALTIMESUPPORT_API)
    Eth_TimeStampQualType timeQual;
    Eth_TimeStampType tsVal;
#endif
#if (STD_ON == ETH_HEADER_ACCESS_API)
    Eth_DataType *ethHdrPtr;
    uint16 ethHeaderLen = 0u;
#endif

    if (TRUE == gEthApp.state[ctrlIdx].veryVerbose)
    {
        EthUtils_printf("EthIf_RxIndication: len: %d, bcast: %s, EtherType: 0x%04x\r\n",
                        lenByte,
                        IsBroadcast ? "yes" : "no",
                        FrameType & 0xFFFFU);
    }

    /* Check that the correct controller index is being reported */
    if (ctrlIdx >= ETH_CTRL_ID_MAX)
    {
        EthUtils_printf("EthIf_RxIndication: incorrect controller index "
                        "(got %d)\r\n",
                        ctrlIdx);
        gEthApp.state[ctrlIdx].stats.rxCtrlIdxErr++;
    }

    /* Validate packet content according to its EtherType */
    switch (FrameType)
    {
    case ETHERTYPE_EXP_CONTROL:
        ctrlFramePtr = (CtrlFramePayload*)DataPtr;
        switch (ctrlFramePtr->cmd)
        {
        case CTRL_FRAME_CMD_START:
            gEthApp.state[ctrlIdx].runTest = TRUE;
            break;
        case CTRL_FRAME_CMD_STOP: 
            if(gEthApp.state[ctrlIdx].runTest == TRUE)
            {
#ifdef freertos            
                xSemaphoreGive((SemaphoreHandle_t)(gEthApp.state[ctrlIdx].testDoneNotifySem));
#endif
                gEthApp.state[ctrlIdx].runTest = FALSE;
            }
            break;
        case CTRL_FRAME_CMD_READY_SYN_ACK:
            gEthApp.state[ctrlIdx].hostUp = TRUE;
            gEthApp.state[ctrlIdx].testIdx = ctrlFramePtr->data.payload[0];
            break;
        case CTRL_FRAME_CMD_RECV_ACK:
            gEthApp.state[ctrlIdx].ackRecv = TRUE;
            break;
        default:
            EthUtils_printf("EthIf_RxIndication: invalid control command: %d\r\n", ctrlFramePtr->cmd);
            break;
        }
        gEthApp.state[ctrlIdx].stats.rxPktCnt++;
        break;

    case ETHERTYPE_VLAN_TAG:
        if(gEthApp.state[ctrlIdx].qosTest)
        {
            VlanDataFramePayload *vlanHdr = (VlanDataFramePayload *)DataPtr;
            uint8 tci = (ntohs(vlanHdr->tci) >> 13) & 0x7;
            uint16 vlanId = ntohs(vlanHdr->tci) & 0xFFF;

            if(vlanId == ETH_TEST_VLAN_VID)
            {
                gEthApp.state[ctrlIdx].stats.qosPacketCnt[tci]++;
                gEthApp.state[ctrlIdx].stats.qosBytesCnt[tci] += lenByte;
            }
            else
            {
                gEthApp.state[ctrlIdx].stats.rxPayloadErr++;
            }
        }
        else
        {
            ret = EthUtils_checkVlanTagAndPayload((VlanDataFramePayload *)DataPtr,
                                                  pcp, vid, ETHERTYPE_EXPERIMENTAL1);
            if (ETH_TEST_PKT_SOK != ret)
            {
                EthUtils_printf("EthIf_RxIndication: VLAN tagged payload error: %d\r\n", ret);
                gEthApp.state[ctrlIdx].stats.rxPayloadErr++;
            }
            else
            {
                gEthApp.state[ctrlIdx].stats.rxPktCnt++;
                gEthApp.state[ctrlIdx].stats.rxVlanPktCnt++;
            }
        }
        break;

    case ETHERTYPE_EXPERIMENTAL1:
        ret = EthUtils_checkPayload((DataFramePayload *)DataPtr);
        if (ETH_TEST_PKT_SOK != ret)
        {
            EthUtils_printf("EthIf_RxIndication: packet validation failed: %d\r\n", ret);
            gEthApp.state[ctrlIdx].stats.rxPayloadErr++;
        }
        else
        {
            gEthApp.state[ctrlIdx].stats.rxPktCnt++;
            gEthApp.state[ctrlIdx].stats.rxExp1PktCnt++;
        }
        break;

    case ETHERTYPE_EXPERIMENTAL2:
        ret = EthUtils_checkPayload((DataFramePayload *)DataPtr);
        if (ETH_TEST_PKT_SOK != ret)
        {
            EthUtils_printf("EthIf_RxIndication: packet validation failed: %d\r\n", ret);
            gEthApp.state[ctrlIdx].stats.rxPayloadErr++;
        }
        else
        {
            gEthApp.state[ctrlIdx].stats.rxPktCnt++;
            gEthApp.state[ctrlIdx].stats.rxExp2PktCnt++;
        }
        break;

    case ETHERTYPE_EXPERIMENTAL3:
        {
            gEthApp.state[ctrlIdx].stats.rxPktCnt++;
            gEthApp.state[ctrlIdx].stats.rxExp3PktCnt++;
            gEthApp.state[ctrlIdx].stats.rxExp3BytesCnt += (lenByte + 14u);
        }
        break;

    default:
        gEthApp.state[ctrlIdx].stats.rxEtherTypeErr++;
        break;
    }

    if (gEthApp.state[ctrlIdx].saveRxFrame)
    {
        if (IsBroadcast)
        {
            memset(gEthApp.state[ctrlIdx].rxFrame.hdr.dstMac, 0xff, ETH_MAC_ADDR_LEN);
        }
        else
        {
            memset(gEthApp.state[ctrlIdx].rxFrame.hdr.dstMac, 0x00U, ETH_MAC_ADDR_LEN);
        }

        memcpy(gEthApp.state[ctrlIdx].rxFrame.hdr.srcMac, PhysAddrPtr, ETH_MAC_ADDR_LEN);
        gEthApp.state[ctrlIdx].rxFrame.hdr.etherType = (uint16)FrameType;
        memcpy(gEthApp.state[ctrlIdx].rxFrame.payload, DataPtr, lenByte);
        gEthApp.state[ctrlIdx].saveRxFrame = FALSE;
    }

#if (STD_ON == ETH_GLOBALTIMESUPPORT_API)
    if(gEthApp.state[ctrlIdx].saveRxTime)
    {
        if(FrameType == ETHERTYPE_EXPERIMENTAL1)
        {
#if (STD_ON == ETH_WRAPPER_ENABLED)
            Eth_30_Wrapper_GetIngressTimestamp(ctrlIdx, DataPtr, &tsVal, &timeQual );
#else
            Eth_GetIngressTimeStamp(ctrlIdx, DataPtr, &timeQual, &tsVal);
#endif
            if(timeQual == ETH_VALID)
            {
                gEthApp.state[ctrlIdx].saveRxTimeStamp[gEthApp.state[ctrlIdx].rxFrameCnt] = tsVal;
                gEthApp.state[ctrlIdx].rxFrameCnt = (gEthApp.state[ctrlIdx].rxFrameCnt + 1) % ETH_TEST_ITER_S_COUNT;
            }
        }
    }
#endif

#if (STD_ON == ETH_HEADER_ACCESS_API)
    ethHdrPtr = DataPtr;
    if(E_NOT_OK == Eth_GetRxHeaderPtr(ctrlIdx, &ethHdrPtr, &ethHeaderLen))
    {
        gEthApp.state[ctrlIdx].stats.rxPayloadErr++;
    }
    else
    {
        if(ethHeaderLen != ETH_HDR_LEN)
        {
            gEthApp.state[ctrlIdx].stats.rxPayloadErr++;
        }
    }
#endif /* (STD_ON == ETH_HEADER_ACCESS_API) */

#if (STD_ON == ETH_ZERO_COPY_API)
    if(E_NOT_OK == Eth_ReleaseRxBuffer(ctrlIdx, DataPtr))
    {
        gEthApp.state[ctrlIdx].stats.rxPayloadErr++;
    }
#endif /* (STD_ON == ETH_ZERO_COPY_API) */

    gEthApp.state[ctrlIdx].stats.rxPktIndCnt++;
}

void EthApp_TxConfirmation(uint8 ctrlIdx,
                          Eth_BufIdxType bufIdx,
                          Std_ReturnType result)
{
#if (STD_ON == ETH_GLOBALTIMESUPPORT_API)
    Eth_TimeStampQualType timeQual;
    Eth_TimeStampType tsVal;
#endif

    if (E_OK == result)
    {
        gEthApp.state[ctrlIdx].stats.txConfPktCnt++;
    }
    else if (E_NOT_OK == result)
    {
        EthUtils_printf("TxConfirmation: transmission of buffer %d failed\r\n", bufIdx);
        gEthApp.state[ctrlIdx].stats.txPktErr++;
    }
    else
    {
        EthUtils_printf("TxConfirmation: invalid confirmation result %d for buffer %d\r\n",
                        result, bufIdx);
    }

#if (STD_ON == ETH_GLOBALTIMESUPPORT_API)
    if(gEthApp.state[ctrlIdx].saveTxTime)
    {
#if (STD_ON == ETH_WRAPPER_ENABLED)
        Eth_30_Wrapper_GetEgressTimestamp(ctrlIdx, bufIdx, &tsVal, &timeQual);
#else
        Eth_GetEgressTimeStamp(ctrlIdx, bufIdx, &timeQual, &tsVal);
#endif
        if(timeQual == ETH_VALID)
        {
            gEthApp.state[ctrlIdx].saveTxTimeStamp[gEthApp.state[ctrlIdx].txFrameCnt] = tsVal;
            gEthApp.state[ctrlIdx].txFrameCnt = (gEthApp.state[ctrlIdx].txFrameCnt + 1) % ETH_TEST_ITER_S_COUNT;
        }
    }
#endif
}

Std_ReturnType Det_ReportError(uint16 moduleId,
                               uint8 instanceId,
                               uint8 apiId,
                               uint8 errorId)
{
    if ((ETH_MODULE_ID == moduleId) ||
        (ETHTRCV_MODULE_ID == moduleId))
    {
        EthUtils_printf("Det_ReportError: %s: %s\r\n",
                    EthApp_apiIdToString(moduleId, apiId),
                    EthApp_errorIdToString(moduleId, errorId));
    }
    else
    {
        EthUtils_printf("Det_ReportError: Error parsing not supported for module\r\n");
    }

    return E_OK;
}

FUNC(Std_ReturnType, DEM_CODE) Dem_SetEventStatus(
                VAR(Dem_EventIdType, AUTOMATIC) EventId,
                VAR(Dem_EventStatusType, AUTOMATIC) EventStatus)
{
    if ((DEM_EVENT_STATUS_FAILED | DEM_EVENT_STATUS_PREFAILED) == EventStatus)
    {
        EthUtils_printf("Dem_SetEventStatus: %d\r\n", (uint16)EventId);
    }
    return (E_OK);
}

/* ========================================================================== */
/*                         Internal Function Definitions                      */
/* ========================================================================== */

static const char *EthApp_ethErrorIdToString(uint8 errorId)
{
    switch (errorId)
    {
    case ETHTRCV_E_INV_TRCV_IDX:
        return "Invalid controller index error";
    case ETHTRCV_E_UNINIT:
        return "Module or controller not initialized";
    case ETHTRCV_E_PARAM_POINTER:
        return "Invalid pointer in parameter list";
    case ETHTRCV_E_INIT_FAILED:
        return "Invalid parameter";
    case ETH_E_INV_MODE:
        return "Invalid mode";
    default:
        return "Unknown error";
   }
}

static const char *EthApp_ethTrcvErrorIdToString(uint8 errorId)
{
    switch (errorId)
    {
    case ETH_E_INV_CTRL_IDX:
        return "Invalid controller index error";
    case ETH_E_UNINIT:
        return "Module or controller not initialized";
    case ETH_E_PARAM_POINTER:
        return "Invalid pointer in parameter list";
    case ETH_E_INV_PARAM:
        return "Invalid parameter";
    case ETH_E_INV_MODE:
        return "Invalid mode";
    default:
        return "Unknown error";
   }
}

static const char *EthApp_errorIdToString(uint8 moduleId, uint8 errorId)
{
    switch(moduleId)
    {
        case ETH_MODULE_ID:
            return EthApp_ethErrorIdToString(errorId);
        case ETHTRCV_MODULE_ID:
            return EthApp_ethTrcvErrorIdToString(errorId);
        default:
            return "Unknown module";
    }
}

static const char *EthApp_ethApiIdToString(uint8 apiId)
{
    switch (apiId)
    {
        case ETH_SID_INIT:
            return "Eth_Init";
        case ETH_SID_SET_CONTROLLER_MODE:
            return "Eth_SetControllerMode";
        case ETH_SID_GET_CONTROLLER_MODE:
            return "Eth_GetControllerMode";
        case ETH_SID_WRITE_MII:
            return "Eth_WriteMii";
        case ETH_SID_READ_MII:
            return "Eth_ReadMii";
        case ETH_SID_GET_PHYS_ADDR:
            return "Eth_GetPhysAddr";
        case ETH_SID_PROVIDE_TX_BUFFER:
            return "Eth_ProvideTxBuffer";
        case ETH_SID_TRANSMIT:
        /* REVISIT: There are two services with the same ID in 4.2.1 spec */
        /* case ETH_SID_MAIN_FUNCTION:  */
            return "Eth_MainFunction or Eth_Transmit";
        case ETH_SID_RECEIVE:
            return "Eth_Receive";
        case ETH_SID_TX_CONFIRMATION:
            return "Eth_TxConfirmation";
        case ETH_SID_GET_VERSION_INFO:
            return "Eth_GetVersionInfo";
        case ETH_SID_RX_IRQ_HDLR:
            return "Eth_RxIrqHdlr_<ctrlIdx>";
        case ETH_SID_TX_IRQ_HDLR:
            return "Eth_TxIrqHdlr_<ctrlIdx>";
        case ETH_SID_UPDATE_PHYS_ADDR_FILTER:
            return "Eth_UpdatePhysAddrFilter";
        case ETH_SID_SET_PHYS_ADDR:
            return "Eth_SetPhysAddr";
        case ETH_SID_GET_COUNTER_VALUES:
            return "Eth_GetCounterValues";
        case ETH_SID_GET_RX_STATS:
            return "Eth_GetRxStats";
        case ETH_SID_GET_TX_STATS:
            return "Eth_GetTxStats";
        case ETH_SID_GET_TXERROR_COUNTERVALUES:
            return "Eth_GetTxErrorCounterValues";
        case ETH_SID_GET_CURRENT_TIME:
            return "Eth_GetCurrentTime";
        case ETH_SID_ENABLE_EGRESS_TIMESTAMP:
            return "Eth_EnableEgressTimeStamp";
        case ETH_SID_GET_EGRESS_TIMESTAMP:
            return "Eth_GetEgressTimeStamp";
        case ETH_SID_GET_INGRESS_TIMESTAMP:
            return "Eth_GetIngressTimeStamp";
        default:
            return "Unknown service";
    }
}

static const char *EthApp_ethTrcvApiIdToString(uint8 apiId)
{
    switch (apiId)
    {
        case ETHTRCV_MAINFCT_WRITE_ID:
            return "EthTrcv_MainFunction";
        case ETHTRCV_VERSION_ID:
            return "EthTrcv_GetVersionInfo";
        case ETHTRCV_WRITEMIIIND_ID:
            return "EthTrcv_WriteMiiIndication";
        case ETHTRCV_READMIIIND_ID:
            return "EthTrcv_ReadMiiIndication";
        case ETHTRCV_GETDPLXMODE_ID:
            return "EthTrcv_GetDuplexMode";
        case ETHTRCV_GETBAUD_ID:
            return "EthTrcv_GetBaudRate";
        case ETHTRCV_GETLNKSTATE_ID:
            return "EthTrcv_GetLinkState or Eth_Transmit";
        case ETHTRCV_SRTAUTONEGOTIA_ID:
            return "EthTrcv_StartAutoNegotiation";
        case ETHTRCV_GETTRCVMODE_ID:
            return "EthTrcv_GetTransceiverMode";
        case ETHTRCV_SETTRCVMODE_ID:
            return "EthTrcv_SetTransceiverMode";
        case ETHTRCV_INIT_ID:
            return "EthTrcv_Init";
        default:
            return "Unknown service";
    }
}

static const char *EthApp_apiIdToString(uint8 moduleId, uint8 apiId)
{
    switch(moduleId)
    {
        case ETH_MODULE_ID:
            return EthApp_ethApiIdToString(apiId);
        case ETHTRCV_MODULE_ID:
            return EthApp_ethTrcvApiIdToString(apiId);
        default:
            return "Unknown API ID";
    }
}

static boolean EthApp_verifyTxStatus(uint8 ctrlIdx, uint32 num, boolean txConfirmation)
{
    boolean status = PASS;
    uint8 retry_cnt = 5U;

    if (TRUE == Eth_GetTxIrqPacingEnable(ctrlIdx))
    {
        /* Incase IRQ pacing enable need more time to wait */
        retry_cnt *= 1000u;
    }

    do{
        /* Flush any pending Tx packet */
        EthApp_transmitFlush(ctrlIdx);

        /* Check error conditions */
        if ((txConfirmation && (gEthApp.state[ctrlIdx].stats.txConfPktCnt != gEthApp.state[ctrlIdx].stats.txPktCnt)) ||
            (gEthApp.state[ctrlIdx].stats.txPktCnt < num) ||
            (gEthApp.state[ctrlIdx].stats.txPktErr > 0U) ||
            (gEthApp.state[ctrlIdx].stats.txBufProvErr > 0U) ||
            (gEthApp.state[ctrlIdx].stats.txBufLenErr > 0U) ||
            (gEthApp.state[ctrlIdx].stats.detErr > 0U) ||
            (gEthApp.state[ctrlIdx].stats.demErr > 0U))
        {
            retry_cnt--;
            if (retry_cnt)
            {
                continue;
            }
            status = FAIL;
        }
        break;
    }while(retry_cnt > 0);

    if ((FAIL == status) || (TRUE == gEthApp.state[ctrlIdx].verbose))
    {
        EthUtils_printf("Transmit Test Results controller %d:\r\n", ctrlIdx);
        EthUtils_printf("  Status:                 %s\r\n", status?"Pass":"Fail");
        EthUtils_printf("  Requested:              %d\r\n", num);
        EthUtils_printf("  Confirmation requested: %s\r\n", txConfirmation?"Yes":"No");
        EthUtils_printf("  Transmitted:            %d\r\n", gEthApp.state[ctrlIdx].stats.txPktCnt);
        EthUtils_printf("  Confirmed:              %d\r\n", gEthApp.state[ctrlIdx].stats.txConfPktCnt);
        EthUtils_printf("  Buffer length errors:   %d\r\n", gEthApp.state[ctrlIdx].stats.txBufLenErr);
        EthUtils_printf("  Buffer request errors:  %d\r\n", gEthApp.state[ctrlIdx].stats.txBufProvErr);
        EthUtils_printf("  Transmission errors:    %d\r\n", gEthApp.state[ctrlIdx].stats.txPktErr);
        EthUtils_printf("  DEM errors:             %d\r\n", gEthApp.state[ctrlIdx].stats.demErr);
        EthUtils_printf("  DET errors:             %d\r\n", gEthApp.state[ctrlIdx].stats.detErr);
        EthUtils_printf("\r\n");
    }

    return status;
}

static void EthApp_setConnectionType(uint32 macPortNum, Eth_MacConnectionType type, boolean delay)
{
    uint32 val = delay << ENET_CTRL_RGMII_ID_SHIFT;

    switch (type)
    {
        case ETH_MAC_CONN_TYPE_RMII_10:
        case ETH_MAC_CONN_TYPE_RMII_100:
            val |= ENET_CTRL_MODE_RMII;
            break;
        case ETH_MAC_CONN_TYPE_RGMII_FORCE_100_HALF:
        case ETH_MAC_CONN_TYPE_RGMII_FORCE_100_FULL:
        case ETH_MAC_CONN_TYPE_RGMII_FORCE_1000_FULL:
        case ETH_MAC_CONN_TYPE_RGMII_DETECT_INBAND:
            val |= ENET_CTRL_MODE_RGMII;
            break;
        default:
            break;
    }
    BoardUtils_setEnetCtrl(macPortNum, val);
}

#define ETH_START_SEC_ISR_CODE
#include "Eth_MemMap.h"

static void Eth_MdioIrqHdlr_wrapper(void* arg)
{
#if (STD_ON == ETH_ENABLE_MII_API)
    Eth_MdioIrqHdlr();
#endif
}

static void Eth_TxIrqHdlr_wrapper_0(void* arg)
{
    Eth_TxIrqHdlr_0();  
}

static void Eth_RxIrqHdlr_wrapper_0(void* arg)
{
    Eth_RxIrqHdlr_0();
}

static void Eth_TxIrqPacingHdlr_wrapper_0(void* arg)
{
    Eth_TxIrqPacingHdlr_0();
}

static void Eth_RxIrqPacingHdlr_wrapper_0(void* arg)
{
    Eth_RxIrqPacingHdlr_0();
}

#if (ETH_CTRL_ID_MAX > 1U)
static void Eth_TxIrqHdlr_wrapper_1(void* arg)
{
    Eth_TxIrqHdlr_1();
}

static void Eth_RxIrqHdlr_wrapper_1(void* arg)
{
    Eth_RxIrqHdlr_1();
}

static void Eth_TxIrqPacingHdlr_wrapper_1(void* arg)
{
    Eth_TxIrqPacingHdlr_1();
}

static void Eth_RxIrqPacingHdlr_wrapper_1(void* arg)
{
    Eth_RxIrqPacingHdlr_1();
}
#endif

#define ETH_STOP_SEC_ISR_CODE
#include "Eth_MemMap.h"

static void EthApp_interruptConfig(void)
{
    Std_ReturnType retVal = E_OK;

#if (STD_OFF == ETH_VIRTUALMAC_SUPPORT)
    /* MDIO Read/Write MII interrupt */
    if (Eth_Cpsw_GetMdioEnableInterrupt() == TRUE &&
        ETH_MDIO_OPMODE_MANUAL != Eth_Cpsw_GetMdioOpMode())
    {
        retVal = BoardUtils_registerInterrupt(
                            BoardUtils_getMdioIntrNum(),
                            &Eth_MdioIrqHdlr_wrapper);
        if (E_OK != retVal)
        {
            EthUtils_printf("%s: failed to register MDIO interrupt\n", __func__);
        }
    }
#endif

    /* TX DMA completion interrupt */
    if(TRUE == Eth_GetTxEnableInterrupt(ETH_CTRL_ID_0) && E_OK == retVal)
    {
        retVal = BoardUtils_registerInterrupt(
                            Eth_GetTxEventCoreIntrNum(ETH_CTRL_ID_0),
                            &Eth_TxIrqHdlr_wrapper_0);
        if (E_OK != retVal)
        {
            EthUtils_printf("%s: failed to register TX DMA interrupt\n", __func__);
        }
    }

    /* RX DMA completion interrupt */
    if(TRUE == Eth_GetRxEnableInterrupt(ETH_CTRL_ID_0) && E_OK == retVal)
    {
        retVal = BoardUtils_registerInterrupt(
                            Eth_GetRxEventCoreIntrNum(ETH_CTRL_ID_0),
                            &Eth_RxIrqHdlr_wrapper_0);
        if (E_OK != retVal)
        {
            EthUtils_printf("%s: failed to register RX DMA interrupt\n", __func__);
        }
    }

    if(TRUE == Eth_GetRxIrqPacingEnable(ETH_CTRL_ID_0) && E_OK == retVal)
    {
#if defined (SOC_J721E)
        {
            struct tisci_msg_rm_irq_set_req     rmIrqReq;
            struct tisci_msg_rm_irq_set_resp    rmIrqResp;
            rmIrqReq.valid_params           = TISCI_MSG_VALUE_RM_DST_ID_VALID;
            rmIrqReq.valid_params          |= TISCI_MSG_VALUE_RM_DST_HOST_IRQ_VALID;
            rmIrqReq.src_id                 = TISCI_DEV_TIMER2;
            rmIrqReq.global_event           = 0U;
            rmIrqReq.src_index              = 0U;
            rmIrqReq.dst_id                 = TISCI_DEV_R5FSS0_CORE1;
            rmIrqReq.dst_host_irq           = ETH_RX_IRQ_PACING_HW_TIMER_IRQ_NUM;
            rmIrqReq.ia_id                  = 0U;
            rmIrqReq.vint                   = 0U;
            rmIrqReq.vint_status_bit_index  = 0U;
            rmIrqReq.secondary_host         = TISCI_MSG_VALUE_RM_UNUSED_SECONDARY_HOST;
            (void)Sciclient_rmIrqSet(
                         &rmIrqReq, &rmIrqResp, APP_SCICLIENT_TIMEOUT);
        }
#endif
        retVal = BoardUtils_registerInterrupt(
                            ETH_RX_IRQ_PACING_HW_TIMER_IRQ_NUM,
                            &Eth_RxIrqPacingHdlr_wrapper_0);
        if (E_OK != retVal)
        {
            EthUtils_printf("%s: failed to register Rx Hw Timer interrupt\n", __func__);
        }
    }

    if(TRUE == Eth_GetTxIrqPacingEnable(ETH_CTRL_ID_0) && E_OK == retVal)
    {
#if defined (SOC_J721E)
        {
            struct tisci_msg_rm_irq_set_req     rmIrqReq;
            struct tisci_msg_rm_irq_set_resp    rmIrqResp;
            rmIrqReq.valid_params           = TISCI_MSG_VALUE_RM_DST_ID_VALID;
            rmIrqReq.valid_params          |= TISCI_MSG_VALUE_RM_DST_HOST_IRQ_VALID;
            rmIrqReq.src_id                 = TISCI_DEV_TIMER3;
            rmIrqReq.global_event           = 0U;
            rmIrqReq.src_index              = 0U;
            rmIrqReq.dst_id                 = TISCI_DEV_R5FSS0_CORE1;
            rmIrqReq.dst_host_irq           = ETH_TX_IRQ_PACING_HW_TIMER_IRQ_NUM;
            rmIrqReq.ia_id                  = 0U;
            rmIrqReq.vint                   = 0U;
            rmIrqReq.vint_status_bit_index  = 0U;
            rmIrqReq.secondary_host         = TISCI_MSG_VALUE_RM_UNUSED_SECONDARY_HOST;
            (void)Sciclient_rmIrqSet(
                         &rmIrqReq, &rmIrqResp, APP_SCICLIENT_TIMEOUT);
        }
#endif
        retVal = BoardUtils_registerInterrupt(
                            ETH_TX_IRQ_PACING_HW_TIMER_IRQ_NUM,
                            &Eth_TxIrqPacingHdlr_wrapper_0);
        if (E_OK != retVal)
        {
            EthUtils_printf("%s: failed to register Tx Hw Timer interrupt\n", __func__);
        }
    }

#if (ETH_CTRL_ID_MAX > 1U)
    /* TX DMA completion interrupt */
    if(TRUE == Eth_GetTxEnableInterrupt(ETH_CTRL_ID_1) && E_OK == retVal)
    {
        retVal = BoardUtils_registerInterrupt(
                            Eth_GetTxEventCoreIntrNum(ETH_CTRL_ID_1),
                            &Eth_TxIrqHdlr_wrapper_1);
        if (E_OK != retVal)
        {
            EthUtils_printf("%s: failed to register TX DMA interrupt\n", __func__);
        }
    }

    /* RX DMA completion interrupt */
    if(TRUE == Eth_GetRxEnableInterrupt(ETH_CTRL_ID_1) && E_OK == retVal)
    {
        retVal = BoardUtils_registerInterrupt(
                            Eth_GetRxEventCoreIntrNum(ETH_CTRL_ID_1),
                            &Eth_RxIrqHdlr_wrapper_1);
        if (E_OK != retVal)
        {
            EthUtils_printf("%s: failed to register RX DMA interrupt\n", __func__);
        }
    }

    if(TRUE == Eth_GetRxIrqPacingEnable(ETH_CTRL_ID_1) && E_OK == retVal)
    {
        retVal = BoardUtils_registerInterrupt(
                            ETH_RX_IRQ_PACING_HW_TIMER_IRQ_NUM,
                            &Eth_RxIrqPacingHdlr_wrapper_1);
        if (E_OK != retVal)
        {
            EthUtils_printf("%s: failed to register Rx Hw Timer interrupt\n", __func__);
        }
    }

    if(TRUE == Eth_GetTxIrqPacingEnable(ETH_CTRL_ID_1) && E_OK == retVal)
    {
        retVal = BoardUtils_registerInterrupt(
                            ETH_TX_IRQ_PACING_HW_TIMER_IRQ_NUM,
                            &Eth_TxIrqPacingHdlr_wrapper_1);
        if (E_OK != retVal)
        {
            EthUtils_printf("%s: failed to register Tx Hw Timer interrupt\n", __func__);
        }
    }
#endif
}