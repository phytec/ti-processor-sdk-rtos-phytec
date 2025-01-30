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
 *  \file     Eth.c
 *
 *  \brief    This file contains the implementation of the main APIs
 *            of the Ethernet driver.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "Eth.h"
#include "SchM_Eth.h"
#if (STD_ON == ETH_DEV_ERROR_DETECT)
#include "Det.h"
#endif
#include "Dem.h"
#include "Eth_Priv.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Consistency check of the Ethernet Driver release version with header file */
/*
 * Design:       ETH_DesignId_001
 * Requirements: MCAL-1523
 */
#if ((ETH_AR_RELEASE_MAJOR_VERSION != (4U)) || \
     (ETH_AR_RELEASE_MINOR_VERSION != (3U)) || \
     (ETH_AR_RELEASE_REVISION_VERSION != (1U)))
#error "Eth: AUTOSAR Version Numbers of Eth are different"
#endif

/* Consistency check of the Ethernet Driver software version with header file */
/*
 * Design:       ETH_DesignId_001
 * Requirements: MCAL-1523
 */
#if ((ETH_SW_MAJOR_VERSION != (9U)) || \
     (ETH_SW_MINOR_VERSION != (2U)) || \
     (ETH_SW_PATCH_VERSION != (0U)))
#error "Eth: Software Version Numbers are inconsistent"
#endif

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

static FUNC(Std_ReturnType, ETH_CODE)
Eth_ControllerInit(uint8 ctrlIdx);

static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkSetControllerModeErrors(uint8 ctrlIdx,
                                 Eth_ModeType ctrlMode);

static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkGetControllerModeErrors(uint8 ctrlIdx,
                                 Eth_ModeType *ctrlModePtr);

#if (STD_ON == ETH_GET_COUNTER_VALUES_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkGetCounterValuesErrors(uint8 ctrlIdx,
                                Eth_CounterType* CounterPtr);
#endif /* (STD_ON == ETH_GET_COUNTER_VALUES_API) */

#if (STD_ON == ETH_GET_COUNTER_STATE_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkGetCounterStateErrors(uint8 ctrlIdx,
                               uint32* CtrValPtr);
#endif /* (STD_ON == ETH_GET_COUNTER_STATE_API) */

#if (STD_ON == ETH_GET_RX_STATS_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkGetRxStatsErrors(uint8 ctrlIdx,
                          Eth_RxStatsType *RxStats);
#endif /* (STD_ON == ETH_GET_RX_STATS_API) */

#if (STD_ON == ETH_GET_TX_STATS_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkGetTxStatsErrors(uint8 ctrlIdx,
                          Eth_TxStatsType *TxStats);
#endif /* (STD_ON == ETH_GET_TX_STATS_API) */

static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkTransmitErrors(uint8 ctrlIdx,
                        Eth_BufIdxType bufIdx,
                        uint16 LenByte,
                        const uint8 *physAddrPtr);

#if (STD_ON == ETH_GET_TX_ERROR_COUNTERSVALUES_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkGetTxErrorCounterValueErrors(uint8 ctrlIdx,
                         Eth_TxErrorCounterValuesType *TxErrorCounterValues);
#endif /* (STD_ON == ETH_GET_TX_ERROR_COUNTERSVALUES_API) */

#if (STD_ON == ETH_DEV_ERROR_DETECT)

LOCAL_INLINE void Eth_reportDetError(uint8 apiId, uint8 errorId);

static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkGetPhysAddrErrors(uint8 ctrlIdx,
                           uint8 *physAddrPtr);

static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkSetPhysAddrErrors(uint8 ctrlIdx,
                           const uint8 *physAddrPtr);

#if (STD_ON == ETH_UPDATE_PHYS_ADDR_FILTER_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkUpdatePhysAddrFilterErrors(uint8 ctrlIdx,
                                    const uint8 *physAddrPtr);
#endif /* (STD_ON == ETH_UPDATE_PHYS_ADDR_FILTER_API) */

#if (STD_ON == ETH_ENABLE_MII_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkWriteMiiErrors(uint8 ctrlIdx,
                        uint8 trcvIdx,
                        uint8 regIdx,
                        uint16 regVal);
#endif /* (STD_ON == ETH_ENABLE_MII_API) */

#if (STD_ON == ETH_ENABLE_MII_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkReadMiiErrors(uint8 ctrlIdx,
                       uint8 trcvIdx,
                       uint8 regIdx,
                       uint16 *regValPtr);
#endif /* (STD_ON == ETH_ENABLE_MII_API) */


static FUNC(BufReq_ReturnType, ETH_CODE)
Eth_checkProvideTxBufferErrors(uint8 ctrlIdx,
                               uint8 priority,
                               Eth_BufIdxType *bufIdxPtr,
                               uint8 **bufPtr,
                               uint16 *lenBytePtr);

static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkReceiveErrors(uint8 ctrlIdx,
                       uint8 FifoIdx,
                       Eth_RxStatusType *rxStatusPtr);

static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkTxConfirmationErrors(uint8 ctrlIdx);

#if (STD_ON == ETH_GLOBALTIMESUPPORT_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkGetCurrentTimeErrors(uint8 ctrlIdx,
                              Eth_TimeStampQualType *timeQualPtr,
                              Eth_TimeStampType *timeStampPtr);
#endif /* (STD_ON == ETH_GLOBALTIMESUPPORT_API) */

#if (STD_ON == ETH_GLOBALTIMESUPPORT_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkEnableEgressTimeStampErrors(uint8 CtrlIdx,
                                     Eth_BufIdxType BufIdx);
#endif /* (STD_ON == ETH_GLOBALTIMESUPPORT_API) */

#if (STD_ON == ETH_GLOBALTIMESUPPORT_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkGetEgressTimeStampErrors(uint8 ctrlIdx,
                                  Eth_TimeStampQualType *timeQualPtr,
                                  Eth_TimeStampType *timeStampPtr);
#endif /* (STD_ON == ETH_GLOBALTIMESUPPORT_API) */

#if (STD_ON == ETH_GLOBALTIMESUPPORT_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkGetIngressTimeStampErrors(uint8 ctrlIdx,
                                   Eth_DataType *dataPtr,
                                   Eth_TimeStampQualType *timeQualPtr,
                                   Eth_TimeStampType *timeStampPtr);
#endif /* (STD_ON == ETH_GLOBALTIMESUPPORT_API) */

#if (STD_ON == ETH_TRAFFIC_SHAPING_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkSetBandwidthLimitErrors(uint8 ctrlIdx,
                                 uint8 queuePrio,
                                 uint32 bandwidthLimit);
#endif /* (STD_ON == ETH_TRAFFIC_SHAPING_API) */

#if (STD_ON == ETH_TRAFFIC_SHAPING_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkGetBandwidthLimitErrors(uint8 ctrlIdx,
                                 uint8 queuePrio,
                                 uint32* BandwidthLimitPtr);
#endif /* (STD_ON == ETH_TRAFFIC_SHAPING_API) */

#if (STD_ON == ETH_ZERO_COPY_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkReleaseRxBufferErrors(uint8 CtrlIdx,
                               Eth_DataType *BufPtr);
#endif /* (STD_ON == ETH_ZERO_COPY_API) */

#if (STD_ON == ETH_ZERO_COPY_API)
static FUNC(BufReq_ReturnType, ETH_CODE)
Eth_checkProvideExtTxBufferErrors(uint8 ctrlIdx,
                               uint8 fifoIdx,
                               Eth_BufIdxType *bufIdxPtr,
                               Eth_DataType **bufPtr,
                               uint16 *lenBytePtr);
#endif /* (STD_ON == ETH_ZERO_COPY_API) */

#if (STD_ON == ETH_HEADER_ACCESS_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkGetTxHeaderPtrErrors(uint8 CtrlIdx,
                              uint8 BufIdx,
                              Eth_DataType **BufPtr,
                              uint16 *LenBytePtr);
#endif /* (STD_ON == ETH_HEADER_ACCESS_API) */

#if (STD_ON == ETH_HEADER_ACCESS_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkGetRxHeaderPtrErrors(uint8 CtrlIdx,
                              Eth_DataType **BufPtr,
                              uint16 *LenBytePtr);
#endif /* (STD_ON == ETH_HEADER_ACCESS_API) */

#if (STD_ON == ETH_REGISTER_READBACK_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkRegisterReadbackErrors(uint8 ctrlIdx,
                                Eth_RegisterReadbackType *regRbPtr);
#endif /* (STD_ON == ETH_REGISTER_READBACK_API) */

#endif /* (STD_ON == ETH_DEV_ERROR_DETECT) */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

#define ETH_START_SEC_VAR_INIT_8
#include "Eth_MemMap.h"

/*
 * Design:       ETH_DesignId_033
 * Requirements: MCAL-1644
 */
volatile VAR(Eth_StateType, ETH_VAR_CLEARED) Eth_DrvStatus;

VAR(uint8, ETH_VAR_CLEARED)   Eth_CtrlMacAddress[ETH_CTRL_ID_MAX][ETH_MAC_ADDR_LEN];
VAR(boolean, ETH_VAR_CLEARED) Eth_CtrlPromiciousMode[ETH_CTRL_ID_MAX];
VAR(boolean, ETH_VAR_CLEARED) Eth_CtrlModeChanged[ETH_CTRL_ID_MAX];


#define ETH_STOP_SEC_VAR_INIT_8
#include "Eth_MemMap.h"

#define ETH_START_SEC_VAR_INIT_UNSPECIFIED
#include "Eth_MemMap.h"

VAR(Eth_ModeType, ETH_VAR_CLEARED)   Eth_CtrlMode[ETH_CTRL_ID_MAX];

#define ETH_STOP_SEC_VAR_INIT_UNSPECIFIED
#include "Eth_MemMap.h"

#define  ETH_START_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Eth_MemMap.h"

extern Eth_VirtualMacInfo  Eth_VirtMacInfo[ETH_CTRL_ID_MAX];

#define  ETH_STOP_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Eth_MemMap.h"

/* ========================================================================== */
/*                  Internal Function Definitions                             */
/* ========================================================================== */

#define ETH_START_SEC_CODE
#include "Eth_MemMap.h"

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
/*
 * Design:       ETH_DesignId_005
 * Requirements: MCAL-1529, MCAL-1532, MCAL-1533, MCAL-1534,
 *               MCAL-1535, MCAL-1536, MCAL-1537, MCAL-1538,
 *               MCAL-1539, MCAL-1540, MCAL-1541, MCAL-1542
 */
FUNC(void, ETH_CODE)
Eth_Init(P2CONST(Eth_ConfigType, AUTOMATIC, ETH_PBCFG) CfgPtr)
{
    Std_ReturnType retVal = E_OK;
    uint8 ctrlIdx = 0u;

#if (STD_ON == ETH_PRE_COMPILE_VARIANT)
    if (NULL_PTR != CfgPtr)
    {
    #if (STD_ON == ETH_DEV_ERROR_DETECT)
        Eth_reportDetError(ETH_SID_INIT, ETH_E_INV_PARAM);
    #endif
        retVal = E_NOT_OK;
    }
#else
    if (NULL_PTR == CfgPtr)
    {
    #if (STD_ON == ETH_DEV_ERROR_DETECT)
        Eth_reportDetError(ETH_SID_INIT, ETH_E_INV_PARAM);
    #endif
        retVal = E_NOT_OK;
    }
    else
    {
        Eth_CfgPtr = (const Eth_ConfigType *)CfgPtr;
    }
#endif

    if (E_OK == retVal)
    {
        Eth_openCpswCommon();

        for (ctrlIdx = 0u; ctrlIdx < ETH_CTRL_ID_MAX; ++ctrlIdx)
        {
            retVal |= Eth_ControllerInit(ctrlIdx);
        }

        if (E_OK == retVal)
        {
             /* Requirements: MCAL-1534 */
            Eth_DrvStatus = ETH_STATE_INIT;
        }
    }
}

/*
 * Design:       ETH_DesignId_007
 * Requirements: MCAL-1544, MCAL-1545, MCAL-1546, MCAL-1547,
 *               MCAL-1548, MCAL-1625, MCAL-1626, MCAL-1653
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_SetControllerMode(uint8 CtrlIdx, Eth_ModeType CtrlMode)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    /* Requirements - SWS_Eth_00043 - return E_NOT_OK if DET is disabled */
    retVal = Eth_checkSetControllerModeErrors(CtrlIdx, CtrlMode);
#endif

    if (E_OK == retVal)
    {
        /* Requirements: MCAL-1653 */
        retVal = Eth_checkControllerAccess(CtrlIdx);
    }

#if (STD_ON == ETH_VIRTUALMAC_SUPPORT)
    if (E_OK == retVal)
    {
        if (ETH_RECOVERY_STATE_NONE != Eth_VirtMacInfo[CtrlIdx].recoverState)
        {
            /* Driver in busy state, not allow to change mode */
            retVal = E_NOT_OK;
        }
    }
#endif

    if (E_OK == retVal)
    {
        if (CtrlMode != Eth_CtrlMode[CtrlIdx])
        {
            if (ETH_MODE_ACTIVE == CtrlMode)
            {
                retVal |= Eth_enableCpsw(CtrlIdx);
            }
            else
            {
                retVal |= Eth_disableCpsw(CtrlIdx);
            }

            if (E_OK == retVal)
            {
                Eth_CtrlModeChanged[CtrlIdx] = TRUE;
                Eth_CtrlMode[CtrlIdx] = CtrlMode;
            }
        }
    }

    return retVal;
}

#if (STD_ON == ETH_VIRTUALMAC_SUPPORT)
FUNC(Std_ReturnType, ETH_CODE)
Eth_SetControllerModeByVirtMac(uint8 CtrlIdx, Eth_ModeType CtrlMode)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    /* Requirements - SWS_Eth_00043 - return E_NOT_OK if DET is disabled */
    retVal = Eth_checkSetControllerModeErrors(CtrlIdx, CtrlMode);
#endif

    if (E_OK == retVal)
    {
        /* Requirements: MCAL-1653 */
        retVal = Eth_checkControllerAccess(CtrlIdx);
    }

    if (E_OK == retVal)
    {
        if (CtrlMode != Eth_CtrlMode[CtrlIdx])
        {
            if (ETH_MODE_ACTIVE == CtrlMode)
            {
                retVal |= Eth_enableCpsw(CtrlIdx);
            }
            else
            {
                retVal |= Eth_disableCpsw(CtrlIdx);
            }

            if (E_OK == retVal)
            {
                Eth_CtrlModeChanged[CtrlIdx] = TRUE;
                Eth_CtrlMode[CtrlIdx] = CtrlMode;
            }
        }
    }

    return retVal;
}
#endif /* STD_ON == ETH_VIRTUALMAC_SUPPORT */

/*
 * Design:       ETH_DesignId_008
 * Requirements: MCAL-1549, MCAL-1550, MCAL-1551, MCAL-1552,
 *               MCAL-1553, MCAL-1554
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_GetControllerMode(uint8 CtrlIdx,
                      P2VAR(Eth_ModeType, AUTOMATIC, ETH_APPL_DATA) CtrlModePtr)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkGetControllerModeErrors(CtrlIdx, CtrlModePtr);
#endif

    if (E_OK == retVal)
    {
        *CtrlModePtr = Eth_CtrlMode[CtrlIdx];
    }

    return retVal;
}

/*
 * Design:       ETH_DesignId_009
 * Requirements: MCAL-1555, MCAL-1556, MCAL-1557, MCAL-1558,
 *               MCAL-1559, MCAL-1560
 */
FUNC(void, ETH_CODE)
Eth_GetPhysAddr(uint8 CtrlIdx,
                P2VAR(uint8, AUTOMATIC, ETH_APPL_DATA) PhysAddrPtr)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkGetPhysAddrErrors(CtrlIdx, PhysAddrPtr);
#endif

    /* Get physical address currently set to the MAC port */
    if (E_OK == retVal)
    {
        PhysAddrPtr[0u] = Eth_CtrlMacAddress[CtrlIdx][0u];
        PhysAddrPtr[1u] = Eth_CtrlMacAddress[CtrlIdx][1u];
        PhysAddrPtr[2u] = Eth_CtrlMacAddress[CtrlIdx][2u];
        PhysAddrPtr[3u] = Eth_CtrlMacAddress[CtrlIdx][3u];
        PhysAddrPtr[4u] = Eth_CtrlMacAddress[CtrlIdx][4u];
        PhysAddrPtr[5u] = Eth_CtrlMacAddress[CtrlIdx][5u];
    }
}

/*
 * Design:       ETH_DesignId_019
 * Requirements: MCAL-1627, MCAL-1628, MCAL-1629, MCAL-1630,
 *               MCAL-1631, MCAL-1638
 */
FUNC(void, ETH_CODE)
Eth_SetPhysAddr(uint8 CtrlIdx,
                P2CONST(uint8, AUTOMATIC, ETH_APPL_DATA) PhysAddrPtr)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkSetPhysAddrErrors(CtrlIdx, PhysAddrPtr);
#endif

    /* Update ALE entries only if in non-promiscuous mode. Otherwise, just
     * save the physical address because ALE is in bypass and no rules are
     * programmed */
    if (E_OK == retVal)
    {
        if (FALSE == Eth_CtrlPromiciousMode[CtrlIdx])
        {
            retVal = Eth_replaceUcastAddr(CtrlIdx, Eth_CtrlMacAddress[CtrlIdx], PhysAddrPtr);
        }
    }

    /* Set new MAC address */
    if (E_OK == retVal)
    {
        retVal = Eth_setMacAddr(CtrlIdx, PhysAddrPtr);
    }

    /* Save the MAC address, it'll be used for any buffer transmission
     * from now on */
    if (E_OK == retVal)
    {
        Eth_CtrlMacAddress[CtrlIdx][0u] = PhysAddrPtr[0u];
        Eth_CtrlMacAddress[CtrlIdx][1u] = PhysAddrPtr[1u];
        Eth_CtrlMacAddress[CtrlIdx][2u] = PhysAddrPtr[2u];
        Eth_CtrlMacAddress[CtrlIdx][3u] = PhysAddrPtr[3u];
        Eth_CtrlMacAddress[CtrlIdx][4u] = PhysAddrPtr[4u];
        Eth_CtrlMacAddress[CtrlIdx][5u] = PhysAddrPtr[5u];
    }
}

#if (STD_ON == ETH_UPDATE_PHYS_ADDR_FILTER_API)
/*
 * Design:       ETH_DesignId_020
 * Requirements: MCAL-1632, MCAL-1633, MCAL-1634, MCAL-1637,
 *               MCAL-1639, MCAL-1649, MCAL-1650, MCAL-1651,
 *               MCAL-1652, MCAL-4827, MCAL-4828
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_UpdatePhysAddrFilter(uint8 CtrlIdx,
                         P2CONST(uint8, AUTOMATIC, ETH_APPL_DATA) PhysAddrPtr,
                         Eth_FilterActionType Action)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkUpdatePhysAddrFilterErrors(CtrlIdx, PhysAddrPtr);
#endif

    if (E_OK == retVal)
    {
        if (TRUE == Eth_isBcastMacAddr(PhysAddrPtr))
        {
            /* Requirements: MCAL-1632 */
            retVal = Eth_setPromiscuousMode(CtrlIdx);
        }
        else if (TRUE == Eth_isNullMacAddr(PhysAddrPtr))
        {
            /* Requirements: MCAL-1634 */
            retVal = Eth_setNonPromiscuousMode(CtrlIdx, Eth_CtrlMacAddress[CtrlIdx]);
        }
        else
        {
            retVal = Eth_doFilterAddr(CtrlIdx, PhysAddrPtr, Action);
        }
    }

    return retVal;
}
#endif /* (STD_ON == ETH_UPDATE_PHYS_ADDR_FILTER_API) */

#if (STD_ON == ETH_ENABLE_MII_API)
/*
 * Design:       ETH_DesignId_010
 * Requirements: MCAL-1561, MCAL-1562, MCAL-1563, MCAL-1564,
 *               MCAL-1565, MCAL-1566, MCAL-1726
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_WriteMii(uint8 CtrlIdx,
             uint8 TrcvIdx,
             uint8 RegIdx,
             uint16 RegVal)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkWriteMiiErrors(CtrlIdx, TrcvIdx, RegIdx, RegVal);
#endif

    if (E_OK == retVal)
    {
        if (Eth_Cpsw_GetMdioEnableInterrupt() == FALSE)
        {
            retVal = Eth_triggerMiiWriteSync(CtrlIdx, TrcvIdx, RegIdx, RegVal);
        }
        else
        {
            retVal = Eth_triggerMiiWriteAsync(CtrlIdx, TrcvIdx, RegIdx, RegVal);
        }
    }

    return retVal;
}
#endif /* (STD_ON == ETH_ENABLE_MII_API) */

#if (STD_ON == ETH_ENABLE_MII_API)
/*
 * Design:       ETH_DesignId_011
 * Requirements: MCAL-1567, MCAL-1568, MCAL-1569, MCAL-1570,
 *               MCAL-1571, MCAL-1572, MCAL-1573, MCAL-1727
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_ReadMii(uint8 CtrlIdx,
            uint8 TrcvIdx,
            uint8 RegIdx,
            P2VAR(uint16, AUTOMATIC, ETH_APPL_DATA) RegValPtr)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkReadMiiErrors(CtrlIdx, TrcvIdx, RegIdx, RegValPtr);
#endif

    if (E_OK == retVal)
    {
        if (Eth_Cpsw_GetMdioEnableInterrupt() == FALSE)
        {
            retVal = Eth_triggerMiiReadSync(CtrlIdx, TrcvIdx, RegIdx, RegValPtr);
        }
        else
        {
            retVal = Eth_triggerMiiReadAsync(CtrlIdx, TrcvIdx, RegIdx, RegValPtr);
        }
    }

    return retVal;
}
#endif /* (STD_ON == ETH_ENABLE_MII_API) */

#if (STD_ON == ETH_GET_COUNTER_VALUES_API)
/*
 * Design:       ETH_DesignId_022
 * Requirements: MCAL-1711, MCAL-1712, MCAL-1713, MCAL-1714,
 *               MCAL-1715, MCAL-1716, MCAL-1717
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_GetCounterValues(uint8 CtrlIdx,
                     Eth_CounterType* CounterPtr)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkGetCounterValuesErrors(CtrlIdx, CounterPtr);
#endif

    if (E_OK == retVal)
    {
        Eth_getCounterValues(CtrlIdx, CounterPtr);
    }

    return retVal;
}
#endif /* #if (STD_ON == ETH_GET_COUNTER_VALUES_API) */

#if (STD_ON == ETH_GET_COUNTER_STATE_API)
/*
 * Design: TBD
 * Requirements: TBD
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_GetCounterState(uint8 CtrlIdx,
                    uint16 CtrOffs,
                    uint32* CtrValPtr)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkGetCounterStateErrors(CtrlIdx, CtrValPtr);
#endif

    if (E_OK == retVal)
    {
        retVal = Eth_getCounterState(CtrlIdx, CtrOffs, CtrValPtr);
    }

    return retVal;
}
#endif /* #if (STD_ON == ETH_GET_COUNTER_STATE_API) */

#if (STD_ON == ETH_GET_RX_STATS_API)
/*
 * Design:       ETH_DesignId_023
 * Requirements: MCAL-1718, MCAL-1719, MCAL-1720, MCAL-1721,
 *               MCAL-1722, MCAL-1723, MCAL-1724
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_GetRxStats(uint8 CtrlIdx,
               P2VAR(Eth_RxStatsType, AUTOMATIC, ETH_APPL_DATA) RxStats)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkGetRxStatsErrors(CtrlIdx, RxStats);
#endif

    if (E_OK == retVal)
    {
        /* Requirements: MCAL-1718 */
        Eth_getRxStatsRFC2819(CtrlIdx, RxStats);
    }

    return retVal;
}
#endif /* (STD_ON == ETH_GET_RX_STATS_API) */

#if (STD_ON == ETH_GET_TX_STATS_API)
/*
 * Design:       ETH_DesignId_040
 * Requirements: MCAL-4844, MCAL-4830, MCAL-4831, MCAL-4832,
 *               MCAL-4833
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_GetTxStats(uint8 CtrlIdx,
               P2VAR(Eth_TxStatsType, AUTOMATIC, ETH_APPL_DATA) TxStats)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkGetTxStatsErrors(CtrlIdx, TxStats);
#endif

    if (E_OK == retVal)
    {
        /* Requirements: MCAL-4844 */
        Eth_getTxStatsRFC1213(CtrlIdx, TxStats);
    }

    return retVal;
}
#endif /* (STD_ON == ETH_GET_TX_STATS_API) */

#if (STD_ON == ETH_GET_TX_ERROR_COUNTERSVALUES_API)
/*
 * Design:       ETH_DesignId_041
 * Requirements: MCAL-4845, MCAL-4834, MCAL-4835, MCAL-4836,
 *               MCAL-4837
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_GetTxErrorCounterValues(uint8 CtrlIdx,
               P2VAR(Eth_TxErrorCounterValuesType, AUTOMATIC, ETH_APPL_DATA) TxErrorCounterValues)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkGetTxErrorCounterValueErrors(CtrlIdx, TxErrorCounterValues);
#endif

    if (E_OK == retVal)
    {
       /* Requirements: MCAL-4845 */
        Eth_getTxErrorCounterValues(CtrlIdx, TxErrorCounterValues);
    }

    return retVal;
}
#endif

/*
 * Design:       ETH_DesignId_012
 * Requirements: MCAL-1574, MCAL-1575, MCAL-1576, MCAL-1577,
 *               MCAL-1578, MCAL-1579, MCAL-1580, MCAL-1581,
 *               MCAL-1582, MCAL-1583, MCAL-1625
 */
FUNC(BufReq_ReturnType, ETH_CODE)
Eth_ProvideTxBuffer(uint8 CtrlIdx,
                    uint8 Priority,
                    P2VAR(Eth_BufIdxType, AUTOMATIC, ETH_APPL_DAT) BufIdxPtr,
                    P2VAR(uint8, AUTOMATIC, ETH_APPL_DAT) *BufPtr,
                    P2VAR(uint16, AUTOMATIC, ETH_APPL_DAT) LenBytePtr)
{
    BufReq_ReturnType retVal = BUFREQ_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkProvideTxBufferErrors(CtrlIdx,
                                            Priority,
                                            BufIdxPtr,
                                            BufPtr,
                                            LenBytePtr);
#endif

    if (BUFREQ_OK == retVal)
    {
        retVal = Eth_provideTxBuffer(CtrlIdx, Priority, BufIdxPtr, BufPtr, LenBytePtr);
    }

    return retVal;
}

/*
 * Design:       ETH_DesignId_013
 * Requirements: MCAL-1584, MCAL-1585, MCAL-1586, MCAL-1587,
 *               MCAL-1588, MCAL-1589, MCAL-1590, MCAL-1591
 *               MCAL-1621, MCAL-1626
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_Transmit(uint8 CtrlIdx,
             Eth_BufIdxType BufIdx,
             Eth_FrameType FrameType,
             boolean TxConfirmation,
             uint16 LenByte,
             P2CONST(uint8, AUTOMATIC, ETH_APPL_DAT) PhysAddrPtr)
{
    Std_ReturnType retVal = E_OK;
    Eth_TxParamsGroupType txParamsGroup;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkTransmitErrors(CtrlIdx, BufIdx, LenByte, PhysAddrPtr);
#endif

    if(E_OK == retVal)
    {
        txParamsGroup.ctrlIdx = CtrlIdx;
        txParamsGroup.bufIdx = BufIdx;
        txParamsGroup.frameType = FrameType;
        txParamsGroup.txConfirmation = TxConfirmation;
        txParamsGroup.lenByte = LenByte;
        txParamsGroup.physAddrPtr = PhysAddrPtr;

        retVal = Eth_transmit(&txParamsGroup);
    }

    return retVal;
}

/*
 * Design:       ETH_DesignId_014
 * Requirements: MCAL-1592, MCAL-1593, MCAL-1594, MCAL-1595,
 *               MCAL-1596, MCAL-1622, MCAL-1640
 */
FUNC(void, ETH_CODE)
Eth_Receive(uint8 CtrlIdx,
            uint8 FifoIdx,
            P2VAR(Eth_RxStatusType, AUTOMATIC, ETH_APPL_DAT) RxStatusPtr)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkReceiveErrors(CtrlIdx, FifoIdx, RxStatusPtr);
#endif

    if (E_OK == retVal)
    {
        *RxStatusPtr = Eth_receiveProcess(CtrlIdx, FifoIdx);
    }
}

/*
 * Design:       ETH_DesignId_015
 * Requirements: MCAL-1597, MCAL-1598, MCAL-1599, MCAL-1600,
 *               MCAL-1601, MCAL-1602, MCAL-1623
 */
FUNC(void, ETH_CODE)
Eth_TxConfirmation(uint8 CtrlIdx)
{
    Std_ReturnType retVal = E_OK;
    uint8 FifoIdx;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkTxConfirmationErrors(CtrlIdx);
#endif

    if (E_OK == retVal)
    {
        for (FifoIdx = 0u; FifoIdx < Eth_GetEgressFifoTotalNum(CtrlIdx); ++FifoIdx)
        {
            (void)Eth_txProcess(CtrlIdx, FifoIdx);
        }
    }
}

/*
 * Design:       ETH_DesignId_021
 * Requirements: MCAL-1654, MCAL-1655, MCAL-1656, MCAL-1657,
 *               MCAL-1725
 */
FUNC(void, ETH_CODE)
Eth_MainFunction(void)
{
    Std_ReturnType retVal = E_OK;
    uint8 ctrlIdx = 0u;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    if (ETH_STATE_INIT != Eth_DrvStatus)
    {
        /* Not mandatory as per 4.3.1 specification */
        Eth_reportDetError(ETH_SID_MAIN_FUNCTION, ETH_E_UNINIT);
        retVal = E_NOT_OK;
    }
#endif

    if (E_OK == retVal)
    {
        for(ctrlIdx = 0u; ctrlIdx < ETH_CTRL_ID_MAX; ctrlIdx++)
        {
            if (FALSE == Eth_IsVirtualMacModeEnable(ctrlIdx))
            {
                /* Requirements: MCAL-1657 */
                Eth_checkControllerErrors(ctrlIdx);
            }

            /* Requirements: MCAL-1725 */
            if (TRUE == Eth_CtrlModeChanged[ctrlIdx])
            {
                EthIf_CtrlModeIndication(ctrlIdx, Eth_CtrlMode[ctrlIdx]);

                Eth_CtrlModeChanged[ctrlIdx] = FALSE;
            }
        }
    }
}

#if (STD_ON == ETH_VERSION_INFO_API)
/*
 * Design:        ETH_DesignId_016
 * Requirements:  MCAL-1603, MCAL-1624
 */
FUNC(void, ETH_CODE)
Eth_GetVersionInfo(P2VAR(Std_VersionInfoType, AUTOMATIC, ETH_APPL_DATA) VersionInfo)
{
#if (STD_ON == ETH_DEV_ERROR_DETECT)
    if (NULL_PTR == VersionInfo)
    {
        /* Requirements: MCAL-1624 */
        Eth_reportDetError(ETH_SID_GET_VERSION_INFO, ETH_E_PARAM_POINTER);
    }
    else
#endif
    {
        VersionInfo->vendorID         = (uint16) ETH_VENDOR_ID;
        VersionInfo->moduleID         = (uint16) ETH_MODULE_ID;
        VersionInfo->sw_major_version = (uint8) ETH_SW_MAJOR_VERSION;
        VersionInfo->sw_minor_version = (uint8) ETH_SW_MINOR_VERSION;
        VersionInfo->sw_patch_version = (uint8) ETH_SW_PATCH_VERSION;
    }
}
#endif /* (STD_ON == ETH_VERSION_INFO_API) */

#if (STD_ON == ETH_GLOBALTIMESUPPORT_API)
/*
 * Design:       ETH_DesignId_026
 * Requirements: MCAL-1666, MCAL-1667, MCAL-1668, MCAL-1669,
 *               MCAL-1670, MCAL-1695
 */
FUNC(Std_ReturnType, ETH_CODE)
Eth_GetCurrentTime(uint8 CtrlIdx,
                   P2VAR(Eth_TimeStampQualType, AUTOMATIC, ETH_APPL_DATA) timeQualPtr,
                   P2VAR(Eth_TimeStampType, AUTOMATIC, ETH_APPL_DATA) timeStampPtr)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkGetCurrentTimeErrors(CtrlIdx, timeQualPtr, timeStampPtr);
#endif

    if(E_OK == retVal)
    {
        retVal = Eth_getCurrentTime(CtrlIdx, timeQualPtr, timeStampPtr);
    }

    return retVal;
}
#endif /* (STD_ON == ETH_GLOBALTIMESUPPORT_API) */

#if (STD_ON == ETH_GLOBALTIMESUPPORT_API)
/*
 * Design:       ETH_DesignId_027
 * Requirements: MCAL-1671, MCAL-1672, MCAL-1673, MCAL-1674,
 *               MCAL-1696
 */
FUNC(void, ETH_CODE)
Eth_EnableEgressTimeStamp(uint8 CtrlIdx, Eth_BufIdxType BufIdx)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkEnableEgressTimeStampErrors(CtrlIdx, BufIdx);
#endif

    if(E_OK == retVal)
    {
        Eth_enableEgressTimeStamp(CtrlIdx, BufIdx);
    }
}
#endif /* (STD_ON == ETH_GLOBALTIMESUPPORT_API) */

#if (STD_ON == ETH_GLOBALTIMESUPPORT_API)
/*
 * Design:       ETH_DesignId_028
 * Requirements: MCAL-1675, MCAL-1676, MCAL-1677, MCAL-1678,
 *               MCAL-1679, MCAL-1697
 */
FUNC(void, ETH_CODE)
Eth_GetEgressTimeStamp(uint8 CtrlIdx,
                       Eth_BufIdxType BufIdx,
                       P2VAR(Eth_TimeStampQualType, AUTOMATIC, ETH_APPL_DATA) timeQualPtr,
                       P2VAR(Eth_TimeStampType, AUTOMATIC, ETH_APPL_DATA) timeStampPtr)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkGetEgressTimeStampErrors(CtrlIdx, timeQualPtr, timeStampPtr);
#endif

    if(E_OK == retVal)
    {
        Eth_getEgressTimeStamp(CtrlIdx, BufIdx, timeQualPtr, timeStampPtr);
    }
}
#endif /* (STD_ON == ETH_GLOBALTIMESUPPORT_API) */

#if (STD_ON == ETH_GLOBALTIMESUPPORT_API)
/*
 * Design:        ETH_DesignId_029
 * Requirements:  MCAL-1680, MCAL-1681, MCAL-1682, MCAL-1683,
 *                MCAL-1684, MCAL-1698
 */
FUNC(void, ETH_CODE)
Eth_GetIngressTimeStamp(uint8 CtrlIdx,
                        P2VAR(Eth_DataType, AUTOMATIC, ETH_APPL_DATA) DataPtr,
                        P2VAR(Eth_TimeStampQualType, AUTOMATIC, ETH_APPL_DATA) timeQualPtr,
                        P2VAR(Eth_TimeStampType, AUTOMATIC, ETH_APPL_DATA) timeStampPtr)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkGetIngressTimeStampErrors(CtrlIdx, DataPtr, timeQualPtr, timeStampPtr);
#endif

    if(E_OK == retVal)
    {
        Eth_getIngressTimeStamp(CtrlIdx, DataPtr, timeQualPtr, timeStampPtr);
    }
}
#endif /* (STD_ON == ETH_GLOBALTIMESUPPORT_API) */

#if (STD_ON == ETH_TRAFFIC_SHAPING_API)
FUNC(Std_ReturnType, ETH_CODE) Eth_SetBandwidthLimit(
    uint8   CtrlIdx,
    uint8   QueuePrio,
    uint32  BandwidthLimit)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkSetBandwidthLimitErrors(CtrlIdx, QueuePrio, BandwidthLimit);
#endif

    if (E_OK == retVal)
    {
        retVal = Eth_setBandwidthLimit(CtrlIdx, QueuePrio, BandwidthLimit);
    }

    return retVal;
}
#endif /* (STD_ON == ETH_TRAFFIC_SHAPING_API) */

#if (STD_ON == ETH_TRAFFIC_SHAPING_API)
FUNC(Std_ReturnType, ETH_CODE) Eth_GetBandwidthLimit(
    uint8              CtrlIdx,
    uint8              QueuePrio,
    P2VAR(uint32, AUTOMATIC, ETH_APPL_DATA)  BandwidthLimitPtr)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkGetBandwidthLimitErrors(CtrlIdx, QueuePrio, BandwidthLimitPtr);
#endif

    if (E_OK == retVal)
    {
        retVal = Eth_getBandwidthLimit(CtrlIdx, QueuePrio, BandwidthLimitPtr);
    }

    return retVal;
}
#endif /* (STD_ON == ETH_TRAFFIC_SHAPING_API) */

#if (STD_ON == ETH_ZERO_COPY_API)
FUNC(Std_ReturnType, ETH_CODE) Eth_ReleaseRxBuffer(uint8 CtrlIdx,
    P2VAR(Eth_DataType, AUTOMATIC, ETH_APPL_DATA) BufPtr)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkReleaseRxBufferErrors(CtrlIdx, BufPtr);
#endif

    if (E_OK == retVal)
    {
        retVal = Eth_releaseRxBuffer(CtrlIdx, BufPtr);
    }

    return retVal;
}
#endif /* (STD_ON == ETH_ZERO_COPY_API) */

#if (STD_ON == ETH_ZERO_COPY_API)
FUNC(BufReq_ReturnType, ETH_CODE) Eth_ProvideExtTxBuffer(uint8 CtrlIdx,
    uint8 Priority,
    P2VAR(Eth_BufIdxType, AUTOMATIC, ETH_APPL_DATA) BufIdxPtr,
    P2VAR(Eth_DataType, AUTOMATIC, ETH_APPL_DATA) *BufPtr,
    P2VAR(uint16, AUTOMATIC, ETH_APPL_DATA) LenBytePtr)
{
    BufReq_ReturnType retVal = BUFREQ_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkProvideExtTxBufferErrors(CtrlIdx,
                                            Priority,
                                            BufIdxPtr,
                                            BufPtr,
                                            LenBytePtr);
#endif

    if (BUFREQ_OK == retVal)
    {
        retVal = Eth_provideExtTxBuffer(CtrlIdx,
                                        Priority,
                                        BufIdxPtr,
                                        BufPtr,
                                        LenBytePtr);
    }

    return retVal;
}
#endif /* (STD_ON == ETH_ZERO_COPY_API) */

#if (STD_ON == ETH_HEADER_ACCESS_API)
FUNC(Std_ReturnType, ETH_CODE) Eth_GetTxHeaderPtr(uint8 CtrlIdx,
    uint8 BufIdx,
    P2VAR(Eth_DataType, AUTOMATIC, ETH_APPL_DATA) *BufPtr,
    P2VAR(uint16, AUTOMATIC, ETH_APPL_DATA) LenBytePtr)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkGetTxHeaderPtrErrors(CtrlIdx, BufIdx,  BufPtr, LenBytePtr);
#endif

    if (E_OK == retVal)
    {
        retVal = Eth_getTxHeaderPtr(CtrlIdx, BufIdx,  BufPtr, LenBytePtr);
    }

    return retVal;
}
#endif /* (STD_ON == ETH_HEADER_ACCESS_API) */

#if (STD_ON == ETH_HEADER_ACCESS_API)
FUNC(Std_ReturnType, ETH_CODE) Eth_GetRxHeaderPtr(uint8 CtrlIdx,
    P2VAR(Eth_DataType, AUTOMATIC, ETH_APPL_DATA) *BufPtr,
    P2VAR(uint16, AUTOMATIC, ETH_APPL_DATA) LenBytePtr)
{
    Std_ReturnType retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkGetRxHeaderPtrErrors(CtrlIdx,  BufPtr, LenBytePtr);
#endif

    if (E_OK == retVal)
    {
        retVal = Eth_getRxHeaderPtr(CtrlIdx,  BufPtr, LenBytePtr);
    }

    return retVal;
}
#endif /* (STD_ON == ETH_HEADER_ACCESS_API) */

#if (STD_ON == ETH_REGISTER_READBACK_API)
Std_ReturnType Eth_RegisterReadback(
    VAR(uint8, AUTOMATIC)CtrlIdx,
    P2VAR(Eth_RegisterReadbackType, AUTOMATIC, ETH_APPL_DATA) RegRbPtr)
{
    VAR(Std_ReturnType, AUTOMATIC) retVal = E_OK;

#if (STD_ON == ETH_DEV_ERROR_DETECT)
    retVal = Eth_checkRegisterReadbackErrors(CtrlIdx, RegRbPtr);
#endif

    if (E_OK == retVal)
    {
        Eth_registerReadback(CtrlIdx, RegRbPtr);
    }

    return retVal;
}
#endif /* (STD_ON == ETH_REGISTER_READBACK_API) */

/* ========================================================================== */
/*                           static functions                                 */
/* ========================================================================== */

static void Eth_GetMacAddress(uint8 ctrlIdx, uint8 *macAddr)
{
    if (Eth_UseDefaultMacAddress(ctrlIdx) == TRUE)
    {
        Eth_readMacAddr(ctrlIdx, macAddr);
    }
    else
    {
        macAddr[0u] = (uint8)((Eth_GetMacAddressHigh(ctrlIdx) >> 24u) & 0xFFu);
        macAddr[1u] = (uint8)((Eth_GetMacAddressHigh(ctrlIdx) >> 16u) & 0xFFu);
        macAddr[2u] = (uint8)((Eth_GetMacAddressHigh(ctrlIdx) >> 8u)  & 0xFFu);
        macAddr[3u] = (uint8)((Eth_GetMacAddressHigh(ctrlIdx))        & 0xFFu);
        macAddr[4u] = (uint8)((Eth_GetMacAddressLow(ctrlIdx)  >> 8u)  & 0xFFu);
        macAddr[5u] = (uint8)((Eth_GetMacAddressLow(ctrlIdx))         & 0xFFu);
    }

    Eth_CtrlMacAddress[ctrlIdx][0u] = macAddr[0u];
    Eth_CtrlMacAddress[ctrlIdx][1u] = macAddr[1u];
    Eth_CtrlMacAddress[ctrlIdx][2u] = macAddr[2u];
    Eth_CtrlMacAddress[ctrlIdx][3u] = macAddr[3u];
    Eth_CtrlMacAddress[ctrlIdx][4u] = macAddr[4u];
    Eth_CtrlMacAddress[ctrlIdx][5u] = macAddr[5u];
}

static FUNC(Std_ReturnType, ETH_CODE)
Eth_ControllerInit(uint8 ctrlIdx)
{
    Std_ReturnType retVal = E_OK;
    uint8 addr[ETH_MAC_ADDR_LEN];

    /* Requirements: MCAL-1542 */
    retVal = Eth_checkControllerAccess(ctrlIdx);

    /* Read transceiver MAC address and update configuration params */
    if (E_OK == retVal)
    {
        Eth_GetMacAddress(ctrlIdx, addr);

        /* Initialize CPSW and the TX/RX packet queues */
        retVal |= Eth_openCpsw(ctrlIdx);

        if ((E_OK == retVal) && (Eth_IsVirtualMacModeEnable(ctrlIdx) == FALSE))
        {
            Eth_CtrlPromiciousMode[ctrlIdx] = FALSE;

            retVal |= Eth_setNonPromiscuousMode(ctrlIdx, addr);

            if (E_OK == retVal)
            {
                retVal |= Eth_setMacAddr(ctrlIdx, addr);
            }
            else
            {
                retVal |= Eth_closeCpsw(ctrlIdx);
            }
        }
    }

    return retVal;
}

static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkSetControllerModeErrors(uint8 ctrlIdx,
                                 Eth_ModeType ctrlMode)
{
    Std_ReturnType retVal = E_OK;

    if (ETH_STATE_INIT != Eth_DrvStatus)
    {
        /* Requirements: MCAL-1546 */
#if (STD_ON == ETH_DEV_ERROR_DETECT)
        Eth_reportDetError(ETH_SID_SET_CONTROLLER_MODE, ETH_E_UNINIT);
#endif
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (ctrlIdx >= ETH_CTRL_ID_MAX))
    {
        /* Requirements: MCAL-1547 */
#if (STD_ON == ETH_DEV_ERROR_DETECT)
        Eth_reportDetError(ETH_SID_SET_CONTROLLER_MODE, ETH_E_INV_CTRL_IDX);
#endif
        retVal = E_NOT_OK;
    }

    return retVal;
}

static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkGetControllerModeErrors(uint8 ctrlIdx,
                                 Eth_ModeType *ctrlModePtr)
{
    Std_ReturnType retVal = E_OK;

    if (ETH_STATE_INIT != Eth_DrvStatus)
    {
        /* Requirements: MCAL-1551 */
#if (STD_ON == ETH_DEV_ERROR_DETECT)
        Eth_reportDetError(ETH_SID_GET_CONTROLLER_MODE, ETH_E_UNINIT);
#endif
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (ctrlIdx >= ETH_CTRL_ID_MAX))
    {
        /* Requirements: MCAL-1552 */
#if (STD_ON == ETH_DEV_ERROR_DETECT)
        Eth_reportDetError(ETH_SID_GET_CONTROLLER_MODE, ETH_E_INV_CTRL_IDX);
#endif
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (NULL_PTR == ctrlModePtr))
    {
        /* Requirements: MCAL-1553 */
#if (STD_ON == ETH_DEV_ERROR_DETECT)
        Eth_reportDetError(ETH_SID_GET_CONTROLLER_MODE, ETH_E_PARAM_POINTER);
#endif
        retVal = E_NOT_OK;
    }

    return retVal;
}

#if (STD_ON == ETH_GET_COUNTER_VALUES_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkGetCounterValuesErrors(uint8 ctrlIdx,
                                Eth_CounterType* CounterPtr)
{
    Std_ReturnType retVal = E_OK;

    if (ETH_STATE_INIT != Eth_DrvStatus)
    {
        /* Requirements: MCAL-1713 */
#if (STD_ON == ETH_DEV_ERROR_DETECT)
        Eth_reportDetError(ETH_SID_GET_COUNTER_VALUES, ETH_E_UNINIT);
#endif
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (ctrlIdx >= ETH_CTRL_ID_MAX))
    {
        /* Requirements: MCAL-1714 */
#if (STD_ON == ETH_DEV_ERROR_DETECT)
        Eth_reportDetError(ETH_SID_GET_COUNTER_VALUES, ETH_E_INV_CTRL_IDX);
#endif
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (NULL_PTR == CounterPtr))
    {
        /* Requirements: MCAL-1715 */
#if (STD_ON == ETH_DEV_ERROR_DETECT)
        Eth_reportDetError(ETH_SID_GET_COUNTER_VALUES, ETH_E_PARAM_POINTER);
#endif
        retVal = E_NOT_OK;
    }

    return retVal;
}
#endif /* (STD_ON == ETH_GET_COUNTER_VALUES_API) */

#if (STD_ON == ETH_GET_COUNTER_STATE_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkGetCounterStateErrors(uint8 ctrlIdx,
                               uint32* CtrValPtr)
{
    Std_ReturnType retVal = E_OK;

    if (ETH_STATE_INIT != Eth_DrvStatus)
    {
        /* Requirements: MCAL-1713 */
#if (STD_ON == ETH_DEV_ERROR_DETECT)
        Eth_reportDetError(ETH_SID_GET_COUNTER_STATE, ETH_E_UNINIT);
#endif
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (ctrlIdx >= ETH_CTRL_ID_MAX))
    {
        /* Requirements: MCAL-1714 */
#if (STD_ON == ETH_DEV_ERROR_DETECT)
        Eth_reportDetError(ETH_SID_GET_COUNTER_STATE, ETH_E_INV_CTRL_IDX);
#endif
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (NULL_PTR == CtrValPtr))
    {
        /* Requirements: MCAL-1715 */
#if (STD_ON == ETH_DEV_ERROR_DETECT)
        Eth_reportDetError(ETH_SID_GET_COUNTER_STATE, ETH_E_PARAM_POINTER);
#endif
        retVal = E_NOT_OK;
    }

    return retVal;
}
#endif /* (STD_ON == ETH_GET_COUNTER_STATE_API) */

#if (STD_ON == ETH_GET_RX_STATS_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkGetRxStatsErrors(uint8 ctrlIdx,
                          Eth_RxStatsType *RxStats)
{
    Std_ReturnType retVal = E_OK;

    if (ETH_STATE_INIT != Eth_DrvStatus)
    {
        /* Requirements: MCAL-1720 */
#if (STD_ON == ETH_DEV_ERROR_DETECT)
        Eth_reportDetError(ETH_SID_GET_RX_STATS, ETH_E_UNINIT);
#endif
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (ctrlIdx >= ETH_CTRL_ID_MAX))
    {
        /* Requirements: MCAL-1721 */
#if (STD_ON == ETH_DEV_ERROR_DETECT)
        Eth_reportDetError(ETH_SID_GET_RX_STATS, ETH_E_INV_CTRL_IDX);
#endif
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (NULL_PTR == RxStats))
    {
        /* Requirements: MCAL-1722 */
#if (STD_ON == ETH_DEV_ERROR_DETECT)
        Eth_reportDetError(ETH_SID_GET_RX_STATS, ETH_E_PARAM_POINTER);
#endif
        retVal = E_NOT_OK;
    }

    return retVal;
}
#endif /* (STD_ON == ETH_GET_RX_STATS_API) */

#if (STD_ON == ETH_GET_TX_STATS_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkGetTxStatsErrors(uint8 ctrlIdx,
                          Eth_TxStatsType *TxStats)
{
    Std_ReturnType retVal = E_OK;

    if (ETH_STATE_INIT != Eth_DrvStatus)
    {
        /* Requirements: MCAL-4830 */
#if (STD_ON == ETH_DEV_ERROR_DETECT)
        Eth_reportDetError(ETH_SID_GET_TX_STATS, ETH_E_UNINIT);
#endif
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (ctrlIdx >= ETH_CTRL_ID_MAX))
    {
        /* Requirements: MCAL-4831 */
#if (STD_ON == ETH_DEV_ERROR_DETECT)
        Eth_reportDetError(ETH_SID_GET_TX_STATS, ETH_E_INV_CTRL_IDX);
#endif
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (NULL_PTR == TxStats))
    {
        /* Requirements: MCAL-4832 */
#if (STD_ON == ETH_DEV_ERROR_DETECT)
        Eth_reportDetError(ETH_SID_GET_TX_STATS, ETH_E_PARAM_POINTER);
#endif
        retVal = E_NOT_OK;
    }

    return retVal;
}
#endif /* (STD_ON == ETH_GET_TX_STATS_API) */

static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkTransmitErrors(uint8 ctrlIdx,
                        Eth_BufIdxType bufIdx,
                        uint16 LenByte,
                        const uint8 *physAddrPtr)
{
    Std_ReturnType retVal = E_OK;
    uint8 fifoIdx;
    uint16 fifoBuffIdx;

    Eth_getFifoFromBufferIdx(bufIdx, &fifoIdx, &fifoBuffIdx);

    if (ETH_STATE_INIT != Eth_DrvStatus)
    {
        /* Requirements: MCAL-1587 */
#if (STD_ON == ETH_DEV_ERROR_DETECT)
        Eth_reportDetError(ETH_SID_TRANSMIT, ETH_E_UNINIT);
#endif
        retVal = E_NOT_OK;
    }

    if ((retVal == E_OK) && (ctrlIdx >= ETH_CTRL_ID_MAX))
    {
        /* Requirements: MCAL-1588 */
#if (STD_ON == ETH_DEV_ERROR_DETECT)
        Eth_reportDetError(ETH_SID_TRANSMIT, ETH_E_INV_CTRL_IDX);
#endif
        retVal = E_NOT_OK;
    }

    if ((retVal == E_OK) && (NULL_PTR == physAddrPtr))
    {
        /* Requirements: MCAL-1590 */
#if (STD_ON == ETH_DEV_ERROR_DETECT)
        Eth_reportDetError(ETH_SID_TRANSMIT, ETH_E_PARAM_POINTER);
#endif
        retVal = E_NOT_OK;
    }

    if ((retVal == E_OK) && (ETH_MODE_ACTIVE != Eth_CtrlMode[ctrlIdx]))
    {
        /* Requirements: MCAL-1621 */
#if (STD_ON == ETH_DEV_ERROR_DETECT)
        Eth_reportDetError(ETH_SID_TRANSMIT, ETH_E_INV_MODE);
#endif
        retVal = E_NOT_OK;
    }

    if((retVal == E_OK) && ((fifoIdx >= Eth_GetEgressFifoTotalNum(ctrlIdx)) ||
        (fifoBuffIdx >= Eth_GetEgressFifoPacketNum(ctrlIdx, fifoIdx))))
    {
#if (STD_ON == ETH_DEV_ERROR_DETECT)
        Eth_reportDetError(ETH_SID_TRANSMIT, ETH_E_INV_PARAM);
#endif
        retVal = E_NOT_OK;
    }

    if((retVal == E_OK) && (LenByte > Eth_GetEgressFifoPacketSize(ctrlIdx, fifoIdx)))
    {
#if (STD_ON == ETH_DEV_ERROR_DETECT)
        Eth_reportDetError(ETH_SID_TRANSMIT, ETH_E_INV_PARAM);
#endif
        retVal = E_NOT_OK;
    }

    return retVal;
}

#if (STD_ON == ETH_GET_TX_ERROR_COUNTERSVALUES_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkGetTxErrorCounterValueErrors(uint8 ctrlIdx,
                          Eth_TxErrorCounterValuesType *TxErrorCounterValues)
{
    Std_ReturnType retVal = E_OK;

    if (ETH_STATE_INIT != Eth_DrvStatus)
    {
        /* Requirements: MCAL-4834 */
#if (STD_ON == ETH_DEV_ERROR_DETECT)
        Eth_reportDetError(ETH_SID_GET_TXERROR_COUNTERVALUES, ETH_E_UNINIT);
#endif
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (ctrlIdx >= ETH_CTRL_ID_MAX))
    {
        /* Requirements: MCAL-4835 */
#if (STD_ON == ETH_DEV_ERROR_DETECT)
        Eth_reportDetError(ETH_SID_GET_TXERROR_COUNTERVALUES, ETH_E_INV_CTRL_IDX);
#endif
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (NULL_PTR == TxErrorCounterValues))
    {
        /* Requirements: MCAL-4836 */
#if (STD_ON == ETH_DEV_ERROR_DETECT)
        Eth_reportDetError(ETH_SID_GET_TXERROR_COUNTERVALUES, ETH_E_PARAM_POINTER);
#endif
        retVal = E_NOT_OK;
    }

    return retVal;
}
#endif

#if (STD_ON == ETH_DEV_ERROR_DETECT)

static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkGetPhysAddrErrors(uint8 ctrlIdx,
                           uint8 *physAddrPtr)
{
    Std_ReturnType retVal = E_OK;

    if (ETH_STATE_INIT != Eth_DrvStatus)
    {
        /* Requirements: MCAL-1557 */
        Eth_reportDetError(ETH_SID_GET_PHYS_ADDR, ETH_E_UNINIT);
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (ctrlIdx >= ETH_CTRL_ID_MAX))
    {
        /* Requirements: MCAL-1558 */
        Eth_reportDetError(ETH_SID_GET_PHYS_ADDR, ETH_E_INV_CTRL_IDX);
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (NULL_PTR == physAddrPtr))
    {
        /* Requirements: MCAL-1559 */
        Eth_reportDetError(ETH_SID_GET_PHYS_ADDR, ETH_E_PARAM_POINTER);
        retVal = E_NOT_OK;
    }

    return retVal;
}

static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkSetPhysAddrErrors(uint8 ctrlIdx,
                           const uint8 *physAddrPtr)
{
    Std_ReturnType retVal = E_OK;

    if (ETH_STATE_INIT != Eth_DrvStatus)
    {
        /* Requirements: MCAL-1628 */
        Eth_reportDetError(ETH_SID_SET_PHYS_ADDR, ETH_E_UNINIT);
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (ctrlIdx >= ETH_CTRL_ID_MAX))
    {
        /* Requirements: MCAL-1629 */
        Eth_reportDetError(ETH_SID_SET_PHYS_ADDR, ETH_E_INV_CTRL_IDX);
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (NULL_PTR == physAddrPtr))
    {
        /* Requirements: MCAL-1630 */
        Eth_reportDetError(ETH_SID_SET_PHYS_ADDR, ETH_E_PARAM_POINTER);
        retVal = E_NOT_OK;
    }
    return retVal;
}

#if (STD_ON == ETH_UPDATE_PHYS_ADDR_FILTER_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkUpdatePhysAddrFilterErrors(uint8 ctrlIdx,
                                    const uint8 *physAddrPtr)
{
    Std_ReturnType retVal = E_OK;

    if (ETH_STATE_INIT != Eth_DrvStatus)
    {
        /* Requirements: MCAL-1649*/
        Eth_reportDetError(ETH_SID_UPDATE_PHYS_ADDR_FILTER, ETH_E_UNINIT);
        retVal = E_NOT_OK;
    }

    if ((retVal == E_OK) && (ctrlIdx >= ETH_CTRL_ID_MAX))
    {
        /* Requirements: MCAL-1650 */
        Eth_reportDetError(ETH_SID_UPDATE_PHYS_ADDR_FILTER, ETH_E_INV_CTRL_IDX);
        retVal = E_NOT_OK;
    }

    if ((retVal == E_OK) && (NULL_PTR == physAddrPtr))
    {
        /* Requirements: MCAL-1651 */
        Eth_reportDetError(ETH_SID_UPDATE_PHYS_ADDR_FILTER, ETH_E_PARAM_POINTER);
        retVal = E_NOT_OK;
    }

    return retVal;
}
#endif /* (STD_ON == ETH_UPDATE_PHYS_ADDR_FILTER_API) */

#if (STD_ON == ETH_ENABLE_MII_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkWriteMiiErrors(uint8 ctrlIdx,
                        uint8 trcvIdx,
                        uint8 regIdx,
                        uint16 regVal)
{
    Std_ReturnType retVal = E_OK;

    if (ETH_STATE_INIT != Eth_DrvStatus)
    {
        /* Requirements: MCAL-1563 */
        Eth_reportDetError(ETH_SID_WRITE_MII, ETH_E_UNINIT);
        retVal = E_NOT_OK;
    }

    if ((retVal == E_OK) && (ctrlIdx >= ETH_CTRL_ID_MAX))
    {
        /* Requirements: MCAL-1564 */
        Eth_reportDetError(ETH_SID_WRITE_MII, ETH_E_INV_CTRL_IDX);
        retVal = E_NOT_OK;
    }

    return retVal;
}

static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkReadMiiErrors(uint8 ctrlIdx,
                       uint8 trcvIdx,
                       uint8 regIdx,
                       uint16 *regValPtr)
{
    Std_ReturnType retVal = E_OK;

    if (ETH_STATE_INIT != Eth_DrvStatus)
    {
        /* Requirements: MCAL-1569 */
        Eth_reportDetError(ETH_SID_READ_MII, ETH_E_UNINIT);
        retVal = E_NOT_OK;
    }

    if ((retVal == E_OK) && (ctrlIdx >= ETH_CTRL_ID_MAX))
    {
        /* Requirements: MCAL-1570 */
        Eth_reportDetError(ETH_SID_READ_MII, ETH_E_INV_CTRL_IDX);
        retVal = E_NOT_OK;
    }

    if ((retVal == E_OK) && (NULL_PTR == regValPtr))
    {
        /* Requirements: MCAL-1571 */
        Eth_reportDetError(ETH_SID_READ_MII, ETH_E_PARAM_POINTER);
        retVal = E_NOT_OK;
    }


    return retVal;
}
#endif /* (STD_ON == ETH_ENABLE_MII_API) */

static FUNC(BufReq_ReturnType, ETH_CODE)
Eth_checkProvideTxBufferErrors(uint8 ctrlIdx,
                               uint8 priority,
                               Eth_BufIdxType *bufIdxPtr,
                               uint8 **bufPtr,
                               uint16 *lenBytePtr)
{
    BufReq_ReturnType retVal = BUFREQ_OK;

    if (ETH_STATE_INIT != Eth_DrvStatus)
    {
        /* Requirements: MCAL-1578 */
        Eth_reportDetError(ETH_SID_PROVIDE_TX_BUFFER, ETH_E_UNINIT);
        retVal = BUFREQ_E_NOT_OK;
    }

    if ((retVal == BUFREQ_OK) && (ctrlIdx >= ETH_CTRL_ID_MAX))
    {
        /* Requirements: MCAL-1579 */
        Eth_reportDetError(ETH_SID_PROVIDE_TX_BUFFER, ETH_E_INV_CTRL_IDX);
        retVal = BUFREQ_E_NOT_OK;
    }

    if ((retVal == BUFREQ_OK) && (NULL_PTR == bufIdxPtr))
    {
        /* Requirements: MCAL-1580 */
        Eth_reportDetError(ETH_SID_PROVIDE_TX_BUFFER, ETH_E_PARAM_POINTER);
        retVal = BUFREQ_E_NOT_OK;
    }

    if ((retVal == BUFREQ_OK) && (NULL_PTR == bufPtr))
    {
        /* Requirements: MCAL-1581 */
        Eth_reportDetError(ETH_SID_PROVIDE_TX_BUFFER, ETH_E_PARAM_POINTER);
        retVal = BUFREQ_E_NOT_OK;
    }

    if ((retVal == BUFREQ_OK) && (NULL_PTR == lenBytePtr))
    {
        /* Requirements: MCAL-1582 */
        Eth_reportDetError(ETH_SID_PROVIDE_TX_BUFFER, ETH_E_PARAM_POINTER);
        retVal = BUFREQ_E_NOT_OK;
    }

    if((retVal == BUFREQ_OK) && (priority >= ETH_PRIORITY_QUEUE_NUM))
    {
        Eth_reportDetError(ETH_SID_PROVIDE_TX_BUFFER, ETH_E_INV_PARAM);
        retVal = BUFREQ_E_NOT_OK;
    }

    return retVal;
}

static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkReceiveErrors(uint8 ctrlIdx,
                       uint8 fifoIdx,
                       Eth_RxStatusType *rxStatusPtr)
{
    Std_ReturnType retVal = E_OK;

    if (ETH_STATE_INIT != Eth_DrvStatus)
    {
        /* Requirements: MCAL-1594 */
        Eth_reportDetError(ETH_SID_RECEIVE, ETH_E_UNINIT);
        retVal = E_NOT_OK;
    }

    if ((retVal == E_OK) && (ctrlIdx >= ETH_CTRL_ID_MAX))
    {
        /* Requirements: MCAL-1595 */
        Eth_reportDetError(ETH_SID_RECEIVE, ETH_E_INV_CTRL_IDX);
        retVal = E_NOT_OK;
    }

    if ((retVal == E_OK) && (ETH_MODE_ACTIVE != Eth_CtrlMode[ctrlIdx]))
    {
        /* Requirements: MCAL-1622 */
        Eth_reportDetError(ETH_SID_RECEIVE, ETH_E_INV_MODE);
        retVal = E_NOT_OK;
    }

    if ((retVal == E_OK) && (NULL_PTR == rxStatusPtr))
    {
        Eth_reportDetError(ETH_SID_RECEIVE, ETH_E_PARAM_POINTER);
        retVal = E_NOT_OK;
    }

    if ((retVal == E_OK) && (fifoIdx >= Eth_GetIngressFifoTotalNum(ctrlIdx)))
    {
        Eth_reportDetError(ETH_SID_RECEIVE, ETH_E_INV_PARAM);
        retVal = E_NOT_OK;
    }

    return retVal;
}

static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkTxConfirmationErrors(uint8 ctrlIdx)
{
    Std_ReturnType retVal = E_OK;

    if (ETH_STATE_INIT != Eth_DrvStatus)
    {
        /* Requirements: MCAL-1600 */
        Eth_reportDetError(ETH_SID_TX_CONFIRMATION, ETH_E_UNINIT);
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (ctrlIdx >= ETH_CTRL_ID_MAX))
    {
        /* Requirements: MCAL-1601 */
        Eth_reportDetError(ETH_SID_TX_CONFIRMATION, ETH_E_INV_CTRL_IDX);
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (ETH_MODE_ACTIVE != Eth_CtrlMode[ctrlIdx]))
    {
        /* Requirements: MCAL-1623 */
        Eth_reportDetError(ETH_SID_TX_CONFIRMATION, ETH_E_INV_MODE);
        retVal = E_NOT_OK;
    }

    return retVal;
}

#if (STD_ON == ETH_GLOBALTIMESUPPORT_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkGetCurrentTimeErrors(uint8 ctrlIdx,
                              Eth_TimeStampQualType *timeQualPtr,
                              Eth_TimeStampType *timeStampPtr)
{
    Std_ReturnType retVal = E_OK;

    if (ETH_STATE_INIT != Eth_DrvStatus)
    {
        /* Requirements: MCAL-1667 */
        Eth_reportDetError(ETH_SID_GET_CURRENT_TIME, ETH_E_UNINIT);
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (ctrlIdx >= ETH_CTRL_ID_MAX))
    {
        /* Requirements: MCAL-1668 */
        Eth_reportDetError(ETH_SID_GET_CURRENT_TIME, ETH_E_INV_CTRL_IDX);
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && ((NULL_PTR == timeQualPtr) || (NULL_PTR == timeStampPtr)))
    {
        /* Requirements: MCAL-1669 */
        Eth_reportDetError(ETH_SID_GET_CURRENT_TIME, ETH_E_PARAM_POINTER);
        retVal = E_NOT_OK;
    }

    return retVal;
}
#endif /* (STD_ON == ETH_GLOBALTIMESUPPORT_API) */

#if (STD_ON == ETH_GLOBALTIMESUPPORT_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkEnableEgressTimeStampErrors(uint8 ctrlIdx,
                                     Eth_BufIdxType bufIdx)
{
    Std_ReturnType retVal = E_OK;

    if (ETH_STATE_INIT != Eth_DrvStatus)
    {
        /* Requirements: MCAL-1672 */
        Eth_reportDetError(ETH_SID_ENABLE_EGRESS_TIMESTAMP, ETH_E_UNINIT);
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (ctrlIdx >= ETH_CTRL_ID_MAX))
    {
        /* Requirements: MCAL-1673 */
        Eth_reportDetError(ETH_SID_ENABLE_EGRESS_TIMESTAMP, ETH_E_INV_CTRL_IDX);
        retVal = E_NOT_OK;
    }

    return retVal;
}
#endif /* (STD_ON == ETH_GLOBALTIMESUPPORT_API) */

#if (STD_ON == ETH_GLOBALTIMESUPPORT_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkGetEgressTimeStampErrors(uint8 ctrlIdx,
                                  Eth_TimeStampQualType *timeQualPtr,
                                  Eth_TimeStampType *timeStampPtr)
{
    Std_ReturnType retVal = E_OK;

    if (ETH_STATE_INIT != Eth_DrvStatus)
    {
        /* Requirements: MCAL-1676 */
        Eth_reportDetError(ETH_SID_GET_EGRESS_TIMESTAMP, ETH_E_UNINIT);
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (ctrlIdx >= ETH_CTRL_ID_MAX))
    {
        /* Requirements: MCAL-1677 */
        Eth_reportDetError(ETH_SID_GET_EGRESS_TIMESTAMP, ETH_E_INV_CTRL_IDX);
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && ((NULL_PTR == timeQualPtr) || (NULL_PTR == timeStampPtr)))
    {
        /* Requirements: MCAL-1678 */
        Eth_reportDetError(ETH_SID_GET_EGRESS_TIMESTAMP, ETH_E_PARAM_POINTER);
        retVal = E_NOT_OK;
    }

    return retVal;
}
#endif /* (STD_ON == ETH_GLOBALTIMESUPPORT_API) */

#if (STD_ON == ETH_GLOBALTIMESUPPORT_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkGetIngressTimeStampErrors(uint8 ctrlIdx,
                                   Eth_DataType *dataPtr,
                                   Eth_TimeStampQualType *timeQualPtr,
                                   Eth_TimeStampType *timeStampPtr)
{
    Std_ReturnType retVal = E_OK;

    if (ETH_STATE_INIT != Eth_DrvStatus)
    {
        /* Requirements: MCAL-1681 */
        Eth_reportDetError(ETH_SID_GET_INGRESS_TIMESTAMP, ETH_E_UNINIT);
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (ctrlIdx >= ETH_CTRL_ID_MAX))
    {
        /* Requirements: MCAL-1682 */
        Eth_reportDetError(ETH_SID_GET_INGRESS_TIMESTAMP, ETH_E_INV_CTRL_IDX);
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && ((NULL_PTR == dataPtr) ||
             (NULL_PTR == timeQualPtr) ||
             (NULL_PTR == timeStampPtr)))
    {
        /* Requirements: MCAL-1683 */
        Eth_reportDetError(ETH_SID_GET_INGRESS_TIMESTAMP, ETH_E_PARAM_POINTER);
        retVal = E_NOT_OK;
    }

    return retVal;
}
#endif /* (STD_ON == ETH_GLOBALTIMESUPPORT_API) */

#if (STD_ON == ETH_TRAFFIC_SHAPING_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkSetBandwidthLimitErrors(uint8 ctrlIdx,
                                 uint8 queuePrio,
                                 uint32 bandwidthLimit)
{
    Std_ReturnType retVal = E_OK;

    if (ETH_STATE_INIT != Eth_DrvStatus)
    {
        Eth_reportDetError(ETH_SID_SET_BANDWIDTH_LIMIT, ETH_E_UNINIT);
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (ctrlIdx >= ETH_CTRL_ID_MAX))
    {
        Eth_reportDetError(ETH_SID_SET_BANDWIDTH_LIMIT, ETH_E_INV_CTRL_IDX);
        retVal = E_NOT_OK;
    }
    if((retVal == E_OK) && (queuePrio >= ETH_PRIORITY_QUEUE_NUM))
    {
        Eth_reportDetError(ETH_SID_SET_BANDWIDTH_LIMIT, ETH_E_INV_PARAM);
        retVal = E_NOT_OK;
    }

    (void)bandwidthLimit;

    return retVal;
}
#endif /* (STD_ON == ETH_TRAFFIC_SHAPING_API) */

#if (STD_ON == ETH_TRAFFIC_SHAPING_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkGetBandwidthLimitErrors(uint8 ctrlIdx,
                                 uint8 queuePrio,
                                 uint32* BandwidthLimitPtr)
{
    Std_ReturnType retVal = E_OK;

    if (ETH_STATE_INIT != Eth_DrvStatus)
    {
        Eth_reportDetError(ETH_SID_GET_BANDWIDTH_LIMIT, ETH_E_UNINIT);
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (ctrlIdx >= ETH_CTRL_ID_MAX))
    {
        Eth_reportDetError(ETH_SID_GET_BANDWIDTH_LIMIT, ETH_E_INV_CTRL_IDX);
        retVal = E_NOT_OK;
    }
    if((retVal == E_OK) && (queuePrio >= ETH_PRIORITY_QUEUE_NUM))
    {
        Eth_reportDetError(ETH_SID_GET_BANDWIDTH_LIMIT, ETH_E_INV_PARAM);
        retVal = E_NOT_OK;
    }
    if((retVal == E_OK) && (NULL_PTR == BandwidthLimitPtr))
    {
        Eth_reportDetError(ETH_SID_GET_BANDWIDTH_LIMIT, ETH_E_PARAM_POINTER);
        retVal = E_NOT_OK;
    }

    return retVal;
}
#endif /* (STD_ON == ETH_TRAFFIC_SHAPING_API) */

#if (STD_ON == ETH_ZERO_COPY_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkReleaseRxBufferErrors(uint8 ctrlIdx,
                               Eth_DataType *bufPtr)
{
    Std_ReturnType retVal = E_OK;

    if (ETH_STATE_INIT != Eth_DrvStatus)
    {
        Eth_reportDetError(ETH_SID_RELEASE_RX_BUFFER, ETH_E_UNINIT);
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (ctrlIdx >= ETH_CTRL_ID_MAX))
    {
        Eth_reportDetError(ETH_SID_RELEASE_RX_BUFFER, ETH_E_INV_CTRL_IDX);
        retVal = E_NOT_OK;
    }
    if((retVal == E_OK) && (NULL_PTR == bufPtr))
    {
        Eth_reportDetError(ETH_SID_RELEASE_RX_BUFFER, ETH_E_PARAM_POINTER);
        retVal = E_NOT_OK;
    }

    return retVal;
}
#endif /* (STD_ON == ETH_ZERO_COPY_API) */

#if (STD_ON == ETH_ZERO_COPY_API)
static FUNC(BufReq_ReturnType, ETH_CODE)
Eth_checkProvideExtTxBufferErrors(uint8 ctrlIdx,
                               uint8 priority,
                               Eth_BufIdxType *bufIdxPtr,
                               Eth_DataType **bufPtr,
                               uint16 *lenBytePtr)
{
    BufReq_ReturnType retVal = BUFREQ_OK;

    if (ETH_STATE_INIT != Eth_DrvStatus)
    {
        Eth_reportDetError(ETH_SID_PROVIDE_EXT_TX_BUFFER, ETH_E_UNINIT);
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (ctrlIdx >= ETH_CTRL_ID_MAX))
    {
        Eth_reportDetError(ETH_SID_PROVIDE_EXT_TX_BUFFER, ETH_E_INV_CTRL_IDX);
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (NULL_PTR == bufIdxPtr))
    {
        Eth_reportDetError(ETH_SID_PROVIDE_EXT_TX_BUFFER, ETH_E_PARAM_POINTER);
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (NULL_PTR == bufPtr))
    {
        Eth_reportDetError(ETH_SID_PROVIDE_EXT_TX_BUFFER, ETH_E_PARAM_POINTER);
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (NULL_PTR == *bufPtr))
    {
        Eth_reportDetError(ETH_SID_PROVIDE_EXT_TX_BUFFER, ETH_E_PARAM_POINTER);
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (NULL_PTR == lenBytePtr))
    {
        Eth_reportDetError(ETH_SID_PROVIDE_EXT_TX_BUFFER, ETH_E_PARAM_POINTER);
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (ETH_HEADER_LEN >= *lenBytePtr))
    {
        Eth_reportDetError(ETH_SID_PROVIDE_EXT_TX_BUFFER, ETH_E_PARAM_POINTER);
        retVal = E_NOT_OK;
    }
    if((retVal == E_OK) && (priority >= ETH_PRIORITY_QUEUE_NUM))
    {
        Eth_reportDetError(ETH_SID_PROVIDE_EXT_TX_BUFFER, ETH_E_INV_PARAM);
        retVal = E_NOT_OK;
    }

    return retVal;
}
#endif /* (STD_ON == ETH_ZERO_COPY_API) */

#if (STD_ON == ETH_HEADER_ACCESS_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkGetTxHeaderPtrErrors(uint8 ctrlIdx,
                               uint8 bufIdx,
                               Eth_DataType **bufPtr,
                               uint16 *lenBytePtr)
{
    Std_ReturnType retVal = E_OK;

    if (ETH_STATE_INIT != Eth_DrvStatus)
    {
        Eth_reportDetError(ETH_SID_GET_TX_HEADER_PTR, ETH_E_UNINIT);
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (ctrlIdx >= ETH_CTRL_ID_MAX))
    {
        Eth_reportDetError(ETH_SID_GET_TX_HEADER_PTR, ETH_E_INV_CTRL_IDX);
        retVal = E_NOT_OK;
    }
    if((retVal == E_OK) && (NULL_PTR == bufPtr))
    {
        Eth_reportDetError(ETH_SID_GET_TX_HEADER_PTR, ETH_E_PARAM_POINTER);
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (NULL_PTR == *bufPtr))
    {
        Eth_reportDetError(ETH_SID_PROVIDE_EXT_TX_BUFFER, ETH_E_PARAM_POINTER);
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (NULL_PTR == lenBytePtr))
    {
        Eth_reportDetError(ETH_SID_PROVIDE_EXT_TX_BUFFER, ETH_E_PARAM_POINTER);
        retVal = E_NOT_OK;
    }

    return retVal;
}
#endif /* (STD_ON == ETH_HEADER_ACCESS_API) */

#if (STD_ON == ETH_HEADER_ACCESS_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkGetRxHeaderPtrErrors(uint8 ctrlIdx,
    Eth_DataType **bufPtr,
    uint16 *lenBytePtr)
{
    Std_ReturnType retVal = E_OK;

    if (ETH_STATE_INIT != Eth_DrvStatus)
    {
        Eth_reportDetError(ETH_SID_GET_RX_HEADER_PTR, ETH_E_UNINIT);
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (ctrlIdx >= ETH_CTRL_ID_MAX))
    {
        Eth_reportDetError(ETH_SID_GET_RX_HEADER_PTR, ETH_E_INV_CTRL_IDX);
        retVal = E_NOT_OK;
    }
    if((retVal == E_OK) && (NULL_PTR == bufPtr))
    {
        Eth_reportDetError(ETH_SID_GET_RX_HEADER_PTR, ETH_E_PARAM_POINTER);
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (NULL_PTR == *bufPtr))
    {
        Eth_reportDetError(ETH_SID_GET_RX_HEADER_PTR, ETH_E_PARAM_POINTER);
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (NULL_PTR == lenBytePtr))
    {
        Eth_reportDetError(ETH_SID_GET_RX_HEADER_PTR, ETH_E_PARAM_POINTER);
        retVal = E_NOT_OK;
    }

    return retVal;
}
#endif /* (STD_ON == ETH_HEADER_ACCESS_API) */

#if (STD_ON == ETH_REGISTER_READBACK_API)
static FUNC(Std_ReturnType, ETH_CODE)
Eth_checkRegisterReadbackErrors(uint8 ctrlIdx,
                                Eth_RegisterReadbackType *regRbPtr)
{
    Std_ReturnType retVal = E_OK;

    if (ETH_STATE_INIT != Eth_DrvStatus)
    {
        Eth_reportDetError(ETH_SID_REGISTER_READBACK, ETH_E_UNINIT);
        retVal = E_NOT_OK;
    }
    if ((retVal == E_OK) && (ctrlIdx >= ETH_CTRL_ID_MAX))
    {
        Eth_reportDetError(ETH_SID_REGISTER_READBACK, ETH_E_INV_CTRL_IDX);
        retVal = E_NOT_OK;
    }
    if((retVal == E_OK) && (NULL_PTR == regRbPtr))
    {
        Eth_reportDetError(ETH_SID_REGISTER_READBACK, ETH_E_PARAM_POINTER);
        retVal = E_NOT_OK;
    }

    return retVal;
}
#endif /* (STD_ON == ETH_REGISTER_READBACK_API) */

#endif /* (STD_ON == ETH_DEV_ERROR_DETECT) */

#define ETH_STOP_SEC_CODE
#include "Eth_MemMap.h"
