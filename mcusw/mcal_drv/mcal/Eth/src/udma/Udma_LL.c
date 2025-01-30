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
 *  \file     Udma_LL.c
 *
 *  \brief    This file contains Udma low level function definitions.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "cslr.h"
#include "Eth_Types.h"
#include "Udma_LL.h"
#include "Udma_Ring_LL.h"

/* ========================================================================== */
/*                             Macro Definition                               */
/* ========================================================================== */
/**
*  \brief CLS DMA channle register position definition
*  @{
*/
#define CSL_UDMAP_TXCRT_CHAN_CTL_EN_MASK                                   (0x80000000U)
#define CSL_UDMAP_TXCRT_CHAN_CTL_EN_SHIFT                                  (0x0000001FU)
#define CSL_UDMAP_TXCRT_CHAN_CTL_EN_MAX                                    (0x00000001U)
#define CSL_UDMAP_TXCRT_CHAN_CTL_TDOWN_MASK                                (0x40000000U)
#define CSL_UDMAP_TXCRT_CHAN_CTL_TDOWN_SHIFT                               (0x0000001EU)
#define CSL_UDMAP_TXCRT_CHAN_CTL_TDOWN_MAX                                 (0x00000001U)
#define CSL_UDMAP_TXCRT_CHAN_CTL_PAUSE_MASK                                (0x20000000U)
#define CSL_UDMAP_TXCRT_CHAN_CTL_PAUSE_SHIFT                               (0x0000001DU)
#define CSL_UDMAP_TXCRT_CHAN_CTL_PAUSE_MAX                                 (0x00000001U)
#define CSL_UDMAP_TXCRT_CHAN_CTL_ERROR_MASK                                (0x00000001U)
#define CSL_UDMAP_TXCRT_CHAN_CTL_ERROR_SHIFT                               (0x00000000U)
#define CSL_UDMAP_TXCRT_CHAN_CTL_ERROR_MAX                                 (0x00000001U)
#define CSL_UDMAP_RXCRT_CHAN_CTL_EN_MASK                                   (0x80000000U)
#define CSL_UDMAP_RXCRT_CHAN_CTL_EN_SHIFT                                  (0x0000001FU)
#define CSL_UDMAP_RXCRT_CHAN_CTL_EN_MAX                                    (0x00000001U)
#define CSL_UDMAP_RXCRT_CHAN_CTL_TDOWN_MASK                                (0x40000000U)
#define CSL_UDMAP_RXCRT_CHAN_CTL_TDOWN_SHIFT                               (0x0000001EU)
#define CSL_UDMAP_RXCRT_CHAN_CTL_TDOWN_MAX                                 (0x00000001U)
#define CSL_UDMAP_RXCRT_CHAN_CTL_PAUSE_MASK                                (0x20000000U)
#define CSL_UDMAP_RXCRT_CHAN_CTL_PAUSE_SHIFT                               (0x0000001DU)
#define CSL_UDMAP_RXCRT_CHAN_CTL_PAUSE_MAX                                 (0x00000001U)
#define CSL_UDMAP_RXCRT_CHAN_CTL_ERROR_MASK                                (0x00000001U)
#define CSL_UDMAP_RXCRT_CHAN_CTL_ERROR_SHIFT                               (0x00000000U)
#define CSL_UDMAP_RXCRT_CHAN_CTL_ERROR_MAX                                 (0x00000001U)
#define CSL_PSILCFG_REG_RT_ENABLE_ENABLE_SHIFT                             (31U)
#define CSL_PSILCFG_REG_RT_ENABLE_ENABLE_MASK                              ((uint32)0x01U << CSL_PSILCFG_REG_RT_ENABLE_ENABLE_SHIFT)
#define CSL_PSILCFG_REG_RT_ENABLE_FLUSH_SHIFT                              (28U)
#define CSL_PSILCFG_REG_RT_ENABLE_FLUSH_MASK                               ((uint32)0x01U << CSL_PSILCFG_REG_RT_ENABLE_FLUSH_SHIFT)
#define CSL_PSILCFG_REG_RT_ENABLE_TDOWN_SHIFT                              (30U)
#define CSL_PSILCFG_REG_RT_ENABLE_TDOWN_MASK                               ((uint32)0x01U << CSL_PSILCFG_REG_RT_ENABLE_TDOWN_SHIFT)
/*! @} */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

Std_ReturnType Udma_LL_Event_Disable(uint32 virtIntr)
{
    Std_ReturnType retVal = E_OK;

    CSL_REG64_WR(ETH_DMA_INTAGGR_INTR_VINT_STATUS_CLEAR(virtIntr), (0xFFFFFFFFFFFFFFFFU));

    return retVal;
}

Std_ReturnType Udma_Channel_LL_TxEnable(
    uint8 ctrlIdx,
    uint8 chIdx)
{
    Std_ReturnType retVal = E_OK;
    uint32 regVal = 0u;

    CSL_FINS(regVal, PSILCFG_REG_RT_ENABLE_ENABLE, (uint32)1U);
    CSL_REG32_WR(Eth_GetTxChannelPeer8RegAddress(Eth_GetTxChanId(ctrlIdx, chIdx)), regVal);

    CSL_REG32_WR(Eth_GetTxChannelCtlRegAddress(Eth_GetTxChanId(ctrlIdx, chIdx)),
        CSL_FMK(UDMAP_TXCRT_CHAN_CTL_EN,    (uint32)1u)        |
        CSL_FMK(UDMAP_TXCRT_CHAN_CTL_TDOWN, (uint32)0u)        |
        CSL_FMK(UDMAP_TXCRT_CHAN_CTL_PAUSE, (uint32)0u));


    return retVal;
}

Std_ReturnType Udma_Channel_LL_TxDisable(
    uint8 ctrlIdx,
    uint8 chIdx)
{
    Std_ReturnType retVal = E_OK;
    uint32 regVal;

#if (ETH_DMA_TEARDOWN_SUPPORT == STD_ON)
    uint32 hwLoopCnt = 0u;

    regVal = CSL_REG32_FEXT(Eth_GetTxChannelCtlRegAddress(Eth_GetTxChanId(ctrlIdx, chIdx)), UDMAP_TXCRT_CHAN_CTL_EN);
    if(1u == regVal)
    {
        regVal = CSL_REG32_RD(Eth_GetTxChannelCtlRegAddress(Eth_GetTxChanId(ctrlIdx, chIdx)));
        CSL_FINS(regVal, UDMAP_TXCRT_CHAN_CTL_TDOWN, (uint32)1U );
        CSL_REG32_WR(Eth_GetTxChannelCtlRegAddress(Eth_GetTxChanId(ctrlIdx, chIdx)), regVal);

        while(CSL_REG32_FEXT(Eth_GetTxChannelCtlRegAddress(Eth_GetTxChanId(ctrlIdx, chIdx)), UDMAP_TXCRT_CHAN_CTL_EN ) == 1u)
        {
            if(hwLoopCnt > UDMA_WAIT_TEARDOWN_COUNTER)
            {
                retVal = E_NOT_OK;
                hwLoopCnt = 0u;
                break;
            }
            hwLoopCnt++;
        }
    }

    if(E_OK != retVal)
    {
        regVal =  CSL_REG32_RD(Eth_GetTxChannelPeer8RegAddress(Eth_GetTxChanId(ctrlIdx, chIdx)));
        CSL_FINS(regVal, PSILCFG_REG_RT_ENABLE_FLUSH, (uint32) 1U);
        CSL_REG32_WR(Eth_GetTxChannelPeer8RegAddress(Eth_GetTxChanId(ctrlIdx, chIdx)), regVal);

        regVal =  CSL_REG32_RD(Eth_GetTxChannelPeer8RegAddress(Eth_GetTxChanId(ctrlIdx, chIdx)));
        hwLoopCnt = 0;
        while((CSL_REG32_FEXT(Eth_GetTxChannelCtlRegAddress(Eth_GetTxChanId(ctrlIdx, chIdx)), UDMAP_TXCRT_CHAN_CTL_EN ) == 1u) ||
              (CSL_FEXT(regVal, PSILCFG_REG_RT_ENABLE_ENABLE) == TRUE))
        {
            if(hwLoopCnt > UDMA_WAIT_TEARDOWN_COUNTER)
            {
                retVal = E_NOT_OK;
                hwLoopCnt = 0u;
                break;
            }
            regVal =  CSL_REG32_RD(Eth_GetTxChannelPeer8RegAddress(Eth_GetTxChanId(ctrlIdx, chIdx)));
            hwLoopCnt++;
        }
    }
#endif

    regVal =  CSL_REG32_RD(Eth_GetTxChannelPeer8RegAddress(Eth_GetTxChanId(ctrlIdx, chIdx)));
    CSL_FINS(regVal, PSILCFG_REG_RT_ENABLE_TDOWN, (uint32) 0U);
    CSL_REG32_WR(Eth_GetTxChannelPeer8RegAddress(Eth_GetTxChanId(ctrlIdx, chIdx)), regVal);
    CSL_REG32_WR(Eth_GetTxChannelCtlRegAddress(Eth_GetTxChanId(ctrlIdx, chIdx)),
        CSL_FMK(UDMAP_TXCRT_CHAN_CTL_EN,    (uint32)0u)        |
        CSL_FMK(UDMAP_TXCRT_CHAN_CTL_TDOWN, (uint32)0u)        |
        CSL_FMK(UDMAP_TXCRT_CHAN_CTL_PAUSE, (uint32)0u));

    return retVal;
}

Std_ReturnType Udma_Channel_LL_RxEnable(
    uint8 ctrlIdx,
    uint8 chIdx)
{
    Std_ReturnType retVal = E_OK;
    uint32 regVal;

    CSL_REG32_WR(Eth_GetRxChannelCtlRegAddress(Eth_GetRxChanId(ctrlIdx, chIdx)),
            CSL_FMK(UDMAP_RXCRT_CHAN_CTL_EN,    (uint32)1u)       |
            CSL_FMK(UDMAP_RXCRT_CHAN_CTL_TDOWN, (uint32)0u)       |
            CSL_FMK(UDMAP_RXCRT_CHAN_CTL_PAUSE, (uint32)0u));

    regVal = CSL_REG32_RD(Eth_GetRxChannelPeer8RegAddress(Eth_GetRxChanId(ctrlIdx, chIdx)));
    CSL_FINS(regVal, PSILCFG_REG_RT_ENABLE_ENABLE, (uint32)1U);
    CSL_REG32_WR(Eth_GetRxChannelPeer8RegAddress(Eth_GetRxChanId(ctrlIdx, chIdx)), regVal);

    return retVal;
}

Std_ReturnType Udma_Channel_LL_RxDisable(
    uint8 ctrlIdx,
    uint8 chIdx)
{
    Std_ReturnType retVal = E_OK;
    uint32 regVal;

#if (ETH_DMA_TEARDOWN_SUPPORT == STD_ON)
    uint32 hwLoopCnt;

    regVal = CSL_REG32_RD(Eth_GetRxChannelPeer8RegAddress(Eth_GetRxChanId(ctrlIdx, chIdx)));
    CSL_FINS(regVal, PSILCFG_REG_RT_ENABLE_TDOWN, (uint32) 1U);
    CSL_REG32_WR(Eth_GetRxChannelPeer8RegAddress(Eth_GetRxChanId(ctrlIdx, chIdx)), regVal);

    hwLoopCnt = 0u;
    while(CSL_REG32_FEXT(Eth_GetRxChannelCtlRegAddress(Eth_GetRxChanId(ctrlIdx, chIdx)), UDMAP_RXCRT_CHAN_CTL_EN ) == 1u)
    {
        if(hwLoopCnt > UDMA_WAIT_TEARDOWN_COUNTER)
        {
            retVal = E_NOT_OK;
            hwLoopCnt = 0u;
            break;
        }
        hwLoopCnt++;
    }

    if(E_OK != retVal)
    {
        regVal = CSL_REG32_RD(Eth_GetRxChannelCtlRegAddress(Eth_GetRxChanId(ctrlIdx, chIdx)));
        CSL_FINS(regVal, UDMAP_RXCRT_CHAN_CTL_TDOWN, (uint32)1U );
        CSL_REG32_WR(Eth_GetRxChannelCtlRegAddress(Eth_GetRxChanId(ctrlIdx, chIdx)), regVal);
        hwLoopCnt = 0u;
        while((CSL_REG32_FEXT(Eth_GetRxChannelCtlRegAddress(Eth_GetRxChanId(ctrlIdx, chIdx)), UDMAP_RXCRT_CHAN_CTL_EN ) == 1u) ||
              (CSL_REG32_FEXT(Eth_GetRxChannelPeer8RegAddress(Eth_GetRxChanId(ctrlIdx, chIdx)), PSILCFG_REG_RT_ENABLE_ENABLE ) == 1u))
        {
            if(hwLoopCnt > UDMA_WAIT_TEARDOWN_COUNTER)
            {
                break;
            }
            hwLoopCnt++;
        }
        if(hwLoopCnt <= UDMA_WAIT_TEARDOWN_COUNTER)
        {
            retVal = E_OK;
        }
    }
#endif

    regVal =  CSL_REG32_RD(Eth_GetRxChannelPeer8RegAddress(Eth_GetRxChanId(ctrlIdx, chIdx)));
    CSL_FINS(regVal, PSILCFG_REG_RT_ENABLE_TDOWN, (uint32) 0U);
    CSL_REG32_WR(Eth_GetRxChannelPeer8RegAddress(Eth_GetRxChanId(ctrlIdx, chIdx)), regVal);
    CSL_REG32_WR(Eth_GetRxChannelCtlRegAddress(Eth_GetRxChanId(ctrlIdx, chIdx)),
        CSL_FMK(UDMAP_RXCRT_CHAN_CTL_EN,    (uint32)0u)        |
        CSL_FMK(UDMAP_RXCRT_CHAN_CTL_TDOWN, (uint32)0u)        |
        CSL_FMK(UDMAP_RXCRT_CHAN_CTL_PAUSE, (uint32)0u));

    return retVal;
}
