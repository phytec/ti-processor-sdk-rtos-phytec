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
 *  \file     app_utils_eth.c
 *
 *  \brief    Includes sections defined in linker command file for ETH module
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "Std_Types.h"
#include "app_utils.h"
#include "sciclient.h"
#include "Eth_Cfg.h"
#include "Eth_Priv.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#define CSL_PASS                            ( (sint32) (0))
/**
 *  \brief UDMA Sciclient timeout
 *
 */
#define UDMA_SCICLIENT_TIMEOUT              (0xFFFFFFFFU)

/**
*  \brief DMA Ring default values
*  @{
*/
#define UDMA_RING_SIZE_8_BYTES_DEFAULT          (1U)
#define UDMA_RING_MODE_RING_DEFAULT             (0U)
#define UDMA_RING_ASEL_DEFAULT                  (0U)
/* @} */

/**
*  \brief DMA TX default values
*  @{
*/
#define UDMA_TX_CHAN_PAUSE_ON_ERROR_DEFAULT     (0U)
#define UDMA_TX_CHAN_FILTER_EINFO_DEFAULT       (0U)
#define UDMA_TX_CHAN_FILTER_PSWORDS_DEFAULT     (0U)
#define UDMA_TX_CHAN_ATYPE_DEFAULT              (0U)
#define UDMA_TX_CHAN_TYPE_DEFAULT               (2U)
#define UDMA_TX_CHAN_FETCH_SIZE_DEFAULT         (16U)
#define UDMA_TX_CHAN_PRIORITY_DEFAULT           (4U)
#define UDMA_TX_CHAN_QOS_DEFAULT                (4U)
#define UDMA_TX_CHAN_ORDER_ID_DEFAULT           (0U)
#define UDMA_TX_CHAN_SECHED_PRIORITY_DEFAULT    (1U)
#define UDMA_TX_CHAN_BURST_SIZE_DEFAULT         (1U)
#define UDMA_TX_CHAN_CREDIT_COUNT_DEFAULT       (0U)
/* @} */

/**
*  \brief DMA RX default values
*  @{
*/
#define UDMA_RX_CHAN_PAUSE_ON_ERROR_DEFAULT     (0U)
#define UDMA_RX_CHAN_ATYPE_DEFAULT              (0U)
#define UDMA_RX_CHAN_TYPE_DEFAULT               (2U)
#define UDMA_RX_CHAN_FETCH_SIZE_DEFAULT         (16U)
#define UDMA_RX_CHAN_PRIORITY_DEFAULT           (4U)
#define UDMA_RX_CHAN_QOS_DEFAULT                (4U)
#define UDMA_RX_CHAN_ORDER_ID_DEFAULT           (0U)
#define UDMA_RX_CHAN_SECHED_PRIORITY_DEFAULT    (1U)
#define UDMA_RX_CHAN_BURST_SIZE_DEFAULT         (1U)
#define UDMA_RX_CHAN_INGORE_SHORT_DEFAULT       (0U)
#define UDMA_RX_CHAN_INGORE_LONG_DEFAULT        (0U)
/* @} */

/**
*  \brief DMA FLOW default values
*  @{
*/
#define UDMA_FLOW_EINFO_PRESENT_DEFAULT         (1U)
#define UDMA_FLOW_PSWORDS_PRESENT_DEFAULT       (1U)
#define UDMA_FLOW_ERROR_HANDLING                (0U)
#define UDMA_FLOW_RX_DESC_TYPE                  (0U)
#define UDMA_FLOW_RX_PS_LOCALTION               (0U)
#define UDMA_FLOW_SOP_OFFSET_DEFAULT            (0U)
#define UDMA_FLOW_SRC_TAG_HIGH_DEFAULT          (0U)
#define UDMA_FLOW_SRC_TAG_LOW_DEFAULT           (0U)
#define UDMA_FLOW_SRC_TAG_HIGH_SEL_DEFAULT      (2U)
#define UDMA_FLOW_SRC_TAG_LOW_SEL_DEFAULT       (4U)
#define UDMA_FLOW_DST_TAG_HIGH_DEFAULT          (0U)
#define UDMA_FLOW_DST_TAG_LOW_DEFAULT           (0U)
#define UDMA_FLOW_DST_TAG_HIGH_SEL_DEFAULT      (5U)
#define UDMA_FLOW_DST_TAG_LOW_SEL_DEFAULT       (4U)
/* @} */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */
/* None */

/* ========================================================================== */
/*                  Internal Function Declarations                            */
/* ========================================================================== */

/**
 *  \brief Utility SciClient configure tx channel unpair function
 *
 *  Configure tx channel unpair via Sciclient
 *
 *  \param ctrlIdx  Controller Idx
 *  \param chIdx    Channel index
 *
 *  \return \ref Std_ReturnType
 */
static Std_ReturnType AppUtils_EthSciClientTxChannelUnPair(
    uint8 ctrlIdx,
    uint8 chIdx);

/**
 *  \brief Utility SciClient configure rx channel unpair function
 *
 *  Configure rx channel unpair via Sciclient
 *
 *  \param ctrlIdx  Controller Idx
 *  \param chIdx    Channel index
 *
 *  \return \ref Std_ReturnType
 */
static Std_ReturnType AppUtils_EthSciClientRxChannelUnPair(
    uint8 ctrIdx,
    uint8 chIdx);

/**
 *  \brief Utility SciClient configure tx channel function
 *
 *  Configure tx channel
 *
 *  \param ctrlIdx  Controller Idx
 *  \param chIdx    Channel index
 *
 *  \return \ref Std_ReturnType
 */
static Std_ReturnType AppUtils_EthSciClientTxChannelConfig(
    uint8 ctrlIdx,
    uint8 chIdx);

/**
 *  \brief Utility SciClient configure rx channel function
 *
 *  Configure rx channel via Sciclient
 *
 *  \param ctrlIdx  Controller Idx
 *  \param chIdx    Channel index
 *
 *  \return \ref Std_ReturnType
 */
static Std_ReturnType AppUtils_EthSciClientRxChannelConfig(
    uint8 ctrlIdx,
    uint8 chIdx);

/**
 *  \brief Utility SciClient configure flow
 *
 *  Configure UDMA flow via Sciclient
 *
 *  \param ctrlIdx  Controller Idx
 *  \param FlowId     Flow index
 *
 *  \return \ref Std_ReturnType
 */
static Std_ReturnType AppUtils_EthSciClientFlowConfig(
    uint8 ctrlIdx,
    uint8 flowIdx);

/**
 *  \brief Utility SciClient configure master event function
 *
 *  Configure master event via Sciclient
 *
 *  \param ctrlIdx   controller index
 *  \param eventIdx  master event index
 *
 *  \return \ref Std_ReturnType
 */
static Std_ReturnType AppUtils_EthSciClientMasterEventConfig(uint8 ctrlIdx,
    uint8 eventIdx);

/**
 *  \brief Utility SciClient configure sub event function
 *
 *  Configure sub event via Sciclient
 *
 *  \param ctrlIdx   controller index
 *  \param eventIdx  ringEventIdx ring event index
 *  \param masterEventIdx     master event index
 *
 *  \return \ref Std_ReturnType
 */
static Std_ReturnType AppUtils_EthSciClientRingEventConfig(uint8 ctrlIdx,
    uint8 ringEventIdx);

/**
 *  \brief Utility SciClient release master event function
 *
 *  Release master event via Sciclient
 *
 *  \param ctrlIdx   controller index
 *  \param eventIdx  master event index
 *
 *  \return \ref Std_ReturnType
 */
static Std_ReturnType AppUtils_EthSciClientMasterEventRelease(uint8 ctrlIdx,
    uint8 eventIdx);

/**
 *  \brief Utility SciClient release sub event function
 *
 *  Release sub event via Sciclient
 *
 *  \param ctrlIdx   controller index
 *  \param eventIdx  ringEventIdx ring event index
 *  \param masterEventIdx     master event index
 *
 *  \return \ref Std_ReturnType
 */
static Std_ReturnType AppUtils_EthSciClientRingEventRelease(uint8 ctrlIdx,
    uint8 ringEventIdx);

/**
 *  \brief Utility SciClient init proxy
 *
 *  Init proxy via Sciclient
 *
 *  \param ctrlIdx   controller index
 *  \param proxyIdx  proxy index
 *
 *  \return \ref Std_ReturnType
 */
static Std_ReturnType AppUtils_EthSciClientProxyConfig(
    uint8 ctrlIdx,
    uint8 proxyIdx);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
#if STD_ON==ETH_VIRTUALMAC_SUPPORT
#define  ETH_START_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Eth_MemMap.h"
extern Eth_VirtualMacInfo  Eth_VirtMacInfo[ETH_CTRL_ID_MAX];
#define  ETH_STOP_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Eth_MemMap.h"
#endif

/**< Sections defined in linker command file for ETH module */
extern uint32 __linker_eth_text_start, __linker_eth_text_end,
              __linker_eth_const_start, __linker_eth_const_end,
              __linker_eth_init_start, __linker_eth_init_end,
              __linker_eth_no_init_start, __linker_eth_no_init_end,
              __linker_eth_no_init_128_start, __linker_eth_no_init_128_end;
/**< Address list of sections defined in linker command file for ETH module */
static uint32 EthApp_SecHoleAddrList[] =
{
    ((uint32) & __linker_eth_text_start),
    ((uint32) & __linker_eth_text_end) - APP_UTILS_LINKER_FILL_LENGTH,
    ((uint32) & __linker_eth_const_start),
    ((uint32) & __linker_eth_const_end) - APP_UTILS_LINKER_FILL_LENGTH,
    ((uint32) & __linker_eth_init_start),
    ((uint32) & __linker_eth_init_end) - APP_UTILS_LINKER_FILL_LENGTH,
    ((uint32) & __linker_eth_no_init_start),
    ((uint32) & __linker_eth_no_init_end) - APP_UTILS_LINKER_FILL_LENGTH,
    ((uint32) & __linker_eth_no_init_128_end) - APP_UTILS_LINKER_FILL_LENGTH,
    ((uint32) & __linker_eth_no_init_128_start)
};

/* ========================================================================== */
/*                          Function Implementation                           */
/* ========================================================================== */
/** \brief Utility used to initialize memory sections.
 *   This API is used to initialize fixed size(APP_UTILS_LINKER_FILL_LENGTH)
 *   before start of each section and before end of each section. i.e
 *      .=align(4);
 *       __linker_eth_text_start = .;
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      *(ETH_TEXT_SECTION)
 *      .=align(4);
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      __linker_eth_text_end = .;
 *
 *   This API should be called during module start up code.
 */
void AppUtils_EthSectionInit(void)
{
    /* Initialize memory sections  */
	AppUtils_SectionInit(&EthApp_SecHoleAddrList[0U],
        MODULEAPP_NUM_SEC_HOLES(EthApp_SecHoleAddrList));
}

/** \brief Utility used to check any memory corruption occured in
 *   stack and memory sections initialized by AppUtils_EthSectionInit.
 *   In this API, both stack and memory sections initialized by
 *   AppUtils_EthSectionInit are checked.
 *
 *   This API expects AppUtils_EthSectionInit should be called in start up code
 *   before calling this.
 */
uint32 AppUtils_EthCheckStackAndSectionCorruption(void)
{
    uint32 retVal = E_OK;
    if (AppUtils_CheckStackAndSectionCorruption(
            &EthApp_SecHoleAddrList[0U],
                MODULEAPP_NUM_SEC_HOLES(EthApp_SecHoleAddrList)) != E_OK)
    {
        retVal = E_NOT_OK;
    }

    return retVal;
}

Std_ReturnType AppUtils_EthDmaOpen(uint8 ctrlIdx)
{
    Std_ReturnType retVal = E_OK;
    uint8 index;

    for(index = 0u; index < Eth_GetRingTotalNum(ctrlIdx); ++index)
    {
        retVal |= AppUtils_EthSciClientRingCfg(ctrlIdx, index);
    }

    for(index = 0u; index < Eth_GetTxChanTotalNum(ctrlIdx); ++index)
    {
        retVal |= AppUtils_EthSciClientTxChannelConfig(ctrlIdx, index);
    }

#if (ETH_DMA_RX_CH_SPERATE == STD_OFF)
    if(ctrlIdx == 0u)
    {
#endif
        for(index = 0u; index < Eth_GetRxChanTotalNum(ctrlIdx); ++index)
        {
            retVal |= AppUtils_EthSciClientRxChannelConfig(ctrlIdx, index);
        }
#if (ETH_DMA_RX_CH_SPERATE == STD_OFF)
    }
#endif

    for(index = 0u; index < Eth_GetFlowTotalNumber(ctrlIdx); ++index)
    {
        retVal |= AppUtils_EthSciClientFlowConfig(ctrlIdx, index);
    }

#if (ETH_DMA_IR_SUPPORT == STD_ON)
    for(index = 0u; index < Eth_GetEventTotalNum(ctrlIdx); ++index)
    {
        retVal |= AppUtils_EthSciClientMasterEventConfig(ctrlIdx, index);
    }
#endif

    for(index = 0u; index < Eth_GetRingEventTotalNum(ctrlIdx); ++index)
    {
        retVal |= AppUtils_EthSciClientRingEventConfig(ctrlIdx, index);
    }

#if (ETH_DMA_PROXY_SUPPORT == STD_ON)
    for(index = 0u; index < Eth_GetProxyTotalNum(ctrlIdx); ++index)
    {
        retVal |= AppUtils_EthSciClientProxyConfig(ctrlIdx, index);
    }
#endif

    return retVal;
}

Std_ReturnType AppUtils_EthDmaClose(uint8 ctrlIdx)
{
    Std_ReturnType retVal = E_OK;
    uint8 index;

    for(index = 0u; index < Eth_GetRingEventTotalNum(ctrlIdx); ++index)
    {
        retVal |= AppUtils_EthSciClientRingEventRelease(ctrlIdx, index);
    }

    for(index = 0u; index < Eth_GetTxChanTotalNum(ctrlIdx); ++index)
    {
        retVal |= AppUtils_EthSciClientTxChannelUnPair(ctrlIdx, index);
    }

#if (ETH_DMA_RX_CH_SPERATE == STD_OFF)
    if(ctrlIdx == 0u)
    {
#endif
        for(index = 0u; index < Eth_GetRxChanTotalNum(ctrlIdx); ++index)
        {
            retVal |= AppUtils_EthSciClientRxChannelUnPair(ctrlIdx, index);
        }
#if (ETH_DMA_RX_CH_SPERATE == STD_OFF)
    }
#endif
    return retVal;
}

Std_ReturnType AppUtils_EthSciClientRingCfg(
    uint8 ctrlIdx,
    uint8 ringIdx)
{
    Std_ReturnType retVal = E_OK;
    struct tisci_msg_rm_ring_cfg_req rmRingReq;
    struct tisci_msg_rm_ring_cfg_resp rmRingResp;
    uint64 physBase = (uint64)Eth_GetRingMemBaseAddress(ctrlIdx, ringIdx);

    rmRingReq.valid_params  = TISCI_MSG_VALUE_RM_RING_ADDR_LO_VALID |
                              TISCI_MSG_VALUE_RM_RING_ADDR_HI_VALID |
                              TISCI_MSG_VALUE_RM_RING_COUNT_VALID |
                              TISCI_MSG_VALUE_RM_RING_MODE_VALID |
                              TISCI_MSG_VALUE_RM_RING_SIZE_VALID |
                              TISCI_MSG_VALUE_RM_RING_ORDER_ID_VALID |
                              TISCI_MSG_VALUE_RM_RING_ASEL_VALID;
    rmRingReq.nav_id        = UDMA_DEVICE_ID_RING;
    rmRingReq.index         = (uint16)Eth_GetRingHwId(ctrlIdx, ringIdx);
    rmRingReq.count         = Eth_GetRingTotalElemNum(ctrlIdx, ringIdx);
    rmRingReq.order_id      = (uint8)Eth_GetRingPriority(ctrlIdx, ringIdx);
    rmRingReq.addr_lo       = (uint32)physBase;
    rmRingReq.addr_hi       = (uint32)(physBase >> 32UL);
    rmRingReq.mode          = (uint8)Eth_GetRingMode(ctrlIdx, ringIdx);
    rmRingReq.size          = UDMA_RING_SIZE_8_BYTES_DEFAULT;
    rmRingReq.asel          = UDMA_RING_ASEL_DEFAULT;

    if(CSL_PASS != Sciclient_rmRingCfg(&rmRingReq, &rmRingResp, UDMA_SCICLIENT_TIMEOUT))
    {
        retVal = E_NOT_OK;
    }

    return retVal;
}

Std_ReturnType AppUtils_EthRingCfg(
    uint8 ctrlIdx,
    uint8 ringIdx)
{
    uint32 regVal = 0u;

    /* Write to SIZE register to reconfigure RING */
    regVal = CSL_REG32_RD(Eth_GetRingSizeReg(Eth_GetRingHwId(ctrlIdx, ringIdx)));
    CSL_REG32_WR(Eth_GetRingSizeReg(Eth_GetRingHwId(ctrlIdx, ringIdx)), regVal);

    return E_OK;
}

Std_ReturnType AppUtils_EthSciClientFlowResetAll(
    uint8 ctrlIdx)
{
    Std_ReturnType  retVal = E_OK;
    struct tisci_msg_rm_udmap_flow_cfg_req              rmFlowReq;
    struct tisci_msg_rm_udmap_flow_cfg_resp             rmFlowResp;
    sint32 cslRet = CSL_PASS;
    uint8 index;

    for(index = 0u; index < Eth_GetFlowTotalNumber(ctrlIdx); ++index)
    {
        rmFlowReq.valid_params         = TISCI_MSG_VALUE_RM_UDMAP_FLOW_EINFO_PRESENT_VALID |
                                         TISCI_MSG_VALUE_RM_UDMAP_FLOW_PSINFO_PRESENT_VALID |
                                         TISCI_MSG_VALUE_RM_UDMAP_FLOW_ERROR_HANDLING_VALID |
                                         TISCI_MSG_VALUE_RM_UDMAP_FLOW_DESC_TYPE_VALID |
                                         TISCI_MSG_VALUE_RM_UDMAP_FLOW_SOP_OFFSET_VALID |
                                         TISCI_MSG_VALUE_RM_UDMAP_FLOW_DEST_QNUM_VALID |
                                         TISCI_MSG_VALUE_RM_UDMAP_FLOW_SRC_TAG_HI_VALID |
                                         TISCI_MSG_VALUE_RM_UDMAP_FLOW_SRC_TAG_LO_VALID |
                                         TISCI_MSG_VALUE_RM_UDMAP_FLOW_DEST_TAG_HI_VALID |
                                         TISCI_MSG_VALUE_RM_UDMAP_FLOW_DEST_TAG_LO_VALID |
                                         TISCI_MSG_VALUE_RM_UDMAP_FLOW_SRC_TAG_HI_SEL_VALID |
                                         TISCI_MSG_VALUE_RM_UDMAP_FLOW_SRC_TAG_LO_SEL_VALID |
                                         TISCI_MSG_VALUE_RM_UDMAP_FLOW_DEST_TAG_HI_SEL_VALID |
                                         TISCI_MSG_VALUE_RM_UDMAP_FLOW_DEST_TAG_LO_SEL_VALID |
                                         TISCI_MSG_VALUE_RM_UDMAP_FLOW_FDQ0_SZ0_QNUM_VALID |
                                         TISCI_MSG_VALUE_RM_UDMAP_FLOW_FDQ1_QNUM_VALID |
                                         TISCI_MSG_VALUE_RM_UDMAP_FLOW_FDQ2_QNUM_VALID |
                                         TISCI_MSG_VALUE_RM_UDMAP_FLOW_FDQ3_QNUM_VALID |
                                         TISCI_MSG_VALUE_RM_UDMAP_FLOW_PS_LOCATION_VALID;
        rmFlowReq.nav_id               = UDMA_DEVICE_ID_UDMA;
        rmFlowReq.flow_index           = (uint16)Eth_GetFlowId(ctrlIdx, index);
        rmFlowReq.rx_dest_qnum         = (uint16)0xFFFFu;
        rmFlowReq.rx_einfo_present     = UDMA_FLOW_EINFO_PRESENT_DEFAULT;
        rmFlowReq.rx_psinfo_present    = UDMA_FLOW_PSWORDS_PRESENT_DEFAULT;
        rmFlowReq.rx_error_handling    = UDMA_FLOW_ERROR_HANDLING;
        rmFlowReq.rx_desc_type         = UDMA_FLOW_RX_DESC_TYPE;
        rmFlowReq.rx_ps_location       = UDMA_FLOW_RX_PS_LOCALTION;
        rmFlowReq.rx_sop_offset        = UDMA_FLOW_SOP_OFFSET_DEFAULT;
        rmFlowReq.rx_src_tag_hi        = UDMA_FLOW_SRC_TAG_HIGH_DEFAULT;
        rmFlowReq.rx_src_tag_lo        = UDMA_FLOW_SRC_TAG_LOW_DEFAULT;
        rmFlowReq.rx_src_tag_hi_sel    = UDMA_FLOW_SRC_TAG_HIGH_SEL_DEFAULT;
        rmFlowReq.rx_src_tag_lo_sel    = UDMA_FLOW_SRC_TAG_LOW_SEL_DEFAULT;
        rmFlowReq.rx_dest_tag_hi       = UDMA_FLOW_DST_TAG_HIGH_DEFAULT;
        rmFlowReq.rx_dest_tag_lo       = UDMA_FLOW_DST_TAG_LOW_DEFAULT;
        rmFlowReq.rx_dest_tag_hi_sel   = UDMA_FLOW_DST_TAG_HIGH_SEL_DEFAULT;
        rmFlowReq.rx_dest_tag_lo_sel   = UDMA_FLOW_DST_TAG_LOW_SEL_DEFAULT;
        rmFlowReq.rx_fdq0_sz0_qnum     = (uint16)0xFFFFu;
        rmFlowReq.rx_fdq1_qnum         = (uint16)0xFFFFu;
        rmFlowReq.rx_fdq2_qnum         = (uint16)0xFFFFu;
        rmFlowReq.rx_fdq3_qnum         = (uint16)0xFFFFu;

        cslRet = Sciclient_rmUdmapFlowCfg(&rmFlowReq, &rmFlowResp, UDMA_SCICLIENT_TIMEOUT);

        if(CSL_PASS != cslRet)
        {
            retVal |= E_NOT_OK;
        }
    }

    return retVal;
}

Std_ReturnType AppUtils_EthSciClientFlowConfigAll(
    uint8 ctrlIdx)
{
    Std_ReturnType  retVal = E_OK;
    uint8 index;

    for(index = 0u; index < Eth_GetFlowTotalNumber(ctrlIdx); ++index)
    {
        retVal |= AppUtils_EthSciClientFlowConfig(ctrlIdx, index);
    }

    return retVal;
}

Std_ReturnType AppUtils_EthSciClientTxChannelUnPairPSILAll(uint8 ctrlIdx)
{
    uint8 chIdx;
    Std_ReturnType retVal = E_OK;
    struct tisci_msg_rm_psil_unpair_req rmPairReq;

    for(chIdx = 0u; chIdx < Eth_GetTxChanTotalNum(ctrlIdx); ++chIdx)
    {
        rmPairReq.nav_id = UDMA_DEVICE_ID_PSIL;
        rmPairReq.src_thread = (uint32)(Eth_GetTxChanId(ctrlIdx, chIdx) + UDMA_SOURCE_THREAD_OFFSET);
        rmPairReq.dst_thread = (uint32)(Eth_GetTxChannelThreadOffset(ctrlIdx) +
            Eth_GetTxChanId(ctrlIdx, chIdx) - Eth_GetTxChanStartNum(ctrlIdx));

        if(CSL_PASS != Sciclient_rmPsilUnpair(&rmPairReq, UDMA_SCICLIENT_TIMEOUT))
        {
            retVal |= E_NOT_OK;
        }
    }

    return retVal;
}

Std_ReturnType AppUtils_EthSciClientTxChannelPairPSILAll(uint8 ctrlIdx)
{
    uint8 chIdx;
    Std_ReturnType retVal = E_OK;
    struct tisci_msg_rm_psil_pair_req rmPairReq;

    for(chIdx = 0u; chIdx < Eth_GetTxChanTotalNum(ctrlIdx); ++chIdx)
    {
        rmPairReq.nav_id = UDMA_DEVICE_ID_PSIL;
        rmPairReq.src_thread = (uint32)(Eth_GetTxChanId(ctrlIdx, chIdx) + UDMA_SOURCE_THREAD_OFFSET);
        rmPairReq.dst_thread = (uint32)(Eth_GetTxChannelThreadOffset(ctrlIdx) +
            Eth_GetTxChanId(ctrlIdx, chIdx) - Eth_GetTxChanStartNum(ctrlIdx));

        if(CSL_PASS != Sciclient_rmPsilPair(&rmPairReq, UDMA_SCICLIENT_TIMEOUT))
        {
            retVal |= E_NOT_OK;
        }
    }

    return retVal;
}

/* ========================================================================== */
/*                          Internal Function Implementation                  */
/* ========================================================================== */

static Std_ReturnType AppUtils_EthSciClientRxChannelUnPair(
    uint8 ctrlIdx,
    uint8 chIdx)
{
    Std_ReturnType retVal = E_OK;
    struct tisci_msg_rm_psil_unpair_req rmPairReq;

    rmPairReq.nav_id = UDMA_DEVICE_ID_PSIL;
    rmPairReq.src_thread = (uint32)(UDMA_RX_CHANNEL_PEER_OFFSET +
        Eth_GetRxChanId(ctrlIdx, chIdx) - Eth_GetRxChanStartNum(ctrlIdx));
    rmPairReq.dst_thread = (uint32)(Eth_GetRxChanId(ctrlIdx, chIdx) + UDMA_DEST_THREAD_OFFSET);

    if(CSL_PASS != Sciclient_rmPsilUnpair(&rmPairReq, UDMA_SCICLIENT_TIMEOUT))
    {
        retVal = E_NOT_OK;
    }

    return retVal;
}

static Std_ReturnType AppUtils_EthSciClientTxChannelUnPair(
    uint8 ctrlIdx,
    uint8 chIdx)
{
    Std_ReturnType retVal = E_OK;
    struct tisci_msg_rm_psil_unpair_req rmPairReq;

    rmPairReq.nav_id = UDMA_DEVICE_ID_PSIL;
    rmPairReq.src_thread = (uint32)(Eth_GetTxChanId(ctrlIdx, chIdx) + UDMA_SOURCE_THREAD_OFFSET);
    rmPairReq.dst_thread = (uint32)(Eth_GetTxChannelThreadOffset(ctrlIdx) +
        Eth_GetTxChanId(ctrlIdx, chIdx) - Eth_GetTxChanStartNum(ctrlIdx));

    if(CSL_PASS != Sciclient_rmPsilUnpair(&rmPairReq, UDMA_SCICLIENT_TIMEOUT))
    {
        retVal = E_NOT_OK;
    }


    return (retVal);
}


static Std_ReturnType AppUtils_EthSciClientTxChannelConfig(
    uint8 ctrlIdx,
    uint8 chIdx)
{
    Std_ReturnType retVal = E_OK;
    struct tisci_msg_rm_udmap_tx_ch_cfg_req     rmUdmaTxReq;
    struct tisci_msg_rm_udmap_tx_ch_cfg_resp    rmUdmaTxResp;
    struct tisci_msg_rm_psil_pair_req rmPairReq;
    sint32 cslRet = CSL_PASS;


    rmPairReq.nav_id = UDMA_DEVICE_ID_PSIL;
    rmPairReq.src_thread = (uint32)(Eth_GetTxChanId(ctrlIdx, chIdx) + UDMA_SOURCE_THREAD_OFFSET);
    rmPairReq.dst_thread = (uint32)(Eth_GetTxChannelThreadOffset(ctrlIdx) + 
        Eth_GetTxChanId(ctrlIdx, chIdx) - Eth_GetTxChanStartNum(ctrlIdx));

    cslRet = Sciclient_rmPsilPair(&rmPairReq, UDMA_SCICLIENT_TIMEOUT);
    
    if(CSL_PASS == cslRet)
    {
        rmUdmaTxReq.valid_params        = TISCI_MSG_VALUE_RM_UDMAP_CH_PAUSE_ON_ERR_VALID |
                                          TISCI_MSG_VALUE_RM_UDMAP_CH_ATYPE_VALID |
                                          TISCI_MSG_VALUE_RM_UDMAP_CH_CHAN_TYPE_VALID |
                                          TISCI_MSG_VALUE_RM_UDMAP_CH_FETCH_SIZE_VALID |
                                          TISCI_MSG_VALUE_RM_UDMAP_CH_CQ_QNUM_VALID |
                                          TISCI_MSG_VALUE_RM_UDMAP_CH_PRIORITY_VALID |
                                          TISCI_MSG_VALUE_RM_UDMAP_CH_QOS_VALID |
                                          TISCI_MSG_VALUE_RM_UDMAP_CH_ORDER_ID_VALID |
                                          TISCI_MSG_VALUE_RM_UDMAP_CH_SCHED_PRIORITY_VALID |
                                          TISCI_MSG_VALUE_RM_UDMAP_CH_TX_FILT_EINFO_VALID |
                                          TISCI_MSG_VALUE_RM_UDMAP_CH_TX_FILT_PSWORDS_VALID |
                                          TISCI_MSG_VALUE_RM_UDMAP_CH_TX_SUPR_TDPKT_VALID |
                                          TISCI_MSG_VALUE_RM_UDMAP_CH_TX_FDEPTH_VALID |
                                          TISCI_MSG_VALUE_RM_UDMAP_CH_BURST_SIZE_VALID |
                                          TISCI_MSG_VALUE_RM_UDMAP_CH_TX_CREDIT_COUNT_VALID;
        rmUdmaTxReq.nav_id              = UDMA_DEVICE_ID_UDMA;
        rmUdmaTxReq.index               = (uint16)Eth_GetTxChanId(ctrlIdx, chIdx);
        rmUdmaTxReq.fdepth              = Eth_GetTxChanDepth(ctrlIdx, chIdx);
        rmUdmaTxReq.tx_pause_on_err     = UDMA_TX_CHAN_PAUSE_ON_ERROR_DEFAULT;
        rmUdmaTxReq.tx_filt_einfo       = UDMA_TX_CHAN_FILTER_EINFO_DEFAULT;
        rmUdmaTxReq.tx_filt_pswords     = UDMA_TX_CHAN_FILTER_PSWORDS_DEFAULT;
        rmUdmaTxReq.tx_atype            = UDMA_TX_CHAN_ATYPE_DEFAULT;
        rmUdmaTxReq.tx_chan_type        = UDMA_TX_CHAN_TYPE_DEFAULT;
        rmUdmaTxReq.tx_fetch_size       = UDMA_TX_CHAN_FETCH_SIZE_DEFAULT;
        rmUdmaTxReq.tx_priority         = UDMA_TX_CHAN_PRIORITY_DEFAULT;
        rmUdmaTxReq.tx_qos              = UDMA_TX_CHAN_QOS_DEFAULT;
        rmUdmaTxReq.tx_orderid          = UDMA_TX_CHAN_ORDER_ID_DEFAULT;
        rmUdmaTxReq.tx_sched_priority   = UDMA_TX_CHAN_SECHED_PRIORITY_DEFAULT;
        rmUdmaTxReq.tx_burst_size       = UDMA_TX_CHAN_BURST_SIZE_DEFAULT;
        rmUdmaTxReq.tx_credit_count     = UDMA_TX_CHAN_CREDIT_COUNT_DEFAULT;

        if(Eth_GetTxChanTdCqRingIdx(ctrlIdx, chIdx) < Eth_GetRingTotalNum(ctrlIdx))
        {
            rmUdmaTxReq.txcq_qnum       = (uint16)Eth_GetRingHwId(ctrlIdx,
                Eth_GetTxChanTdCqRingIdx(ctrlIdx, chIdx));
            rmUdmaTxReq.tx_supr_tdpkt   = (uint8)FALSE;
        }
        else
        {
            rmUdmaTxReq.tx_supr_tdpkt   = (uint8)TRUE;
        }

        cslRet = Sciclient_rmUdmapTxChCfg(&rmUdmaTxReq, &rmUdmaTxResp, UDMA_SCICLIENT_TIMEOUT);

        if(CSL_PASS != cslRet)
        {
            retVal = E_NOT_OK;
        }
    }
    else
    {
        retVal = E_NOT_OK;
    }

    return retVal;
}

static Std_ReturnType AppUtils_EthSciClientRxChannelConfig(
    uint8 ctrlIdx,
    uint8 chIdx)
{
    Std_ReturnType retVal = E_OK;
    struct tisci_msg_rm_udmap_rx_ch_cfg_req     rmUdmaRxReq;
    struct tisci_msg_rm_udmap_rx_ch_cfg_resp    rmUdmaRxResp;
    struct tisci_msg_rm_psil_pair_req rmPairReq;

    rmPairReq.nav_id = UDMA_DEVICE_ID_PSIL;
    rmPairReq.src_thread = (uint32)(UDMA_RX_CHANNEL_PEER_OFFSET +
            Eth_GetRxChanId(ctrlIdx, chIdx) - Eth_GetRxChanStartNum(ctrlIdx));
    rmPairReq.dst_thread = (uint32)(Eth_GetRxChanId(ctrlIdx, chIdx) +
            UDMA_DEST_THREAD_OFFSET);

    if(CSL_PASS == Sciclient_rmPsilPair(&rmPairReq, UDMA_SCICLIENT_TIMEOUT))
    {
        rmUdmaRxReq.valid_params        = TISCI_MSG_VALUE_RM_UDMAP_CH_PAUSE_ON_ERR_VALID |
                                          TISCI_MSG_VALUE_RM_UDMAP_CH_ATYPE_VALID |
                                          TISCI_MSG_VALUE_RM_UDMAP_CH_CHAN_TYPE_VALID |
                                          TISCI_MSG_VALUE_RM_UDMAP_CH_FETCH_SIZE_VALID |
                                          TISCI_MSG_VALUE_RM_UDMAP_CH_PRIORITY_VALID |
                                          TISCI_MSG_VALUE_RM_UDMAP_CH_QOS_VALID |
                                          TISCI_MSG_VALUE_RM_UDMAP_CH_ORDER_ID_VALID |
                                          TISCI_MSG_VALUE_RM_UDMAP_CH_SCHED_PRIORITY_VALID |
                                          TISCI_MSG_VALUE_RM_UDMAP_CH_RX_FLOWID_START_VALID |
                                          TISCI_MSG_VALUE_RM_UDMAP_CH_RX_FLOWID_CNT_VALID |
                                          TISCI_MSG_VALUE_RM_UDMAP_CH_RX_IGNORE_SHORT_VALID |
                                          TISCI_MSG_VALUE_RM_UDMAP_CH_RX_IGNORE_LONG_VALID |
                                          TISCI_MSG_VALUE_RM_UDMAP_CH_BURST_SIZE_VALID;
        rmUdmaRxReq.nav_id              = UDMA_DEVICE_ID_UDMA;
        rmUdmaRxReq.index               = (uint16)Eth_GetRxChanId(ctrlIdx, chIdx);
        rmUdmaRxReq.flowid_start        = Eth_GetRxChanFlowStartNum(ctrlIdx, chIdx);
        rmUdmaRxReq.flowid_cnt          = Eth_GetRxChanFlowTotalNum(ctrlIdx, chIdx);
        if(Eth_GetRxChanTdCqRingIdx(ctrlIdx, chIdx) < Eth_GetRingTotalNum(ctrlIdx))
        {
            rmUdmaRxReq.valid_params   |= TISCI_MSG_VALUE_RM_UDMAP_CH_CQ_QNUM_VALID;
            rmUdmaRxReq.rxcq_qnum       = (uint16)Eth_GetRingHwId(ctrlIdx, Eth_GetRxChanTdCqRingIdx(ctrlIdx, chIdx));
        }
        rmUdmaRxReq.rx_pause_on_err     = UDMA_RX_CHAN_PAUSE_ON_ERROR_DEFAULT;
        rmUdmaRxReq.rx_atype            = UDMA_RX_CHAN_ATYPE_DEFAULT;
        rmUdmaRxReq.rx_chan_type        = UDMA_RX_CHAN_TYPE_DEFAULT;
        rmUdmaRxReq.rx_fetch_size       = UDMA_RX_CHAN_FETCH_SIZE_DEFAULT;
        rmUdmaRxReq.rx_priority         = UDMA_RX_CHAN_PRIORITY_DEFAULT;
        rmUdmaRxReq.rx_qos              = UDMA_RX_CHAN_QOS_DEFAULT;
        rmUdmaRxReq.rx_orderid          = UDMA_RX_CHAN_ORDER_ID_DEFAULT;
        rmUdmaRxReq.rx_sched_priority   = UDMA_RX_CHAN_SECHED_PRIORITY_DEFAULT;
        rmUdmaRxReq.rx_ignore_short     = UDMA_RX_CHAN_INGORE_SHORT_DEFAULT;
        rmUdmaRxReq.rx_ignore_long      = UDMA_RX_CHAN_INGORE_LONG_DEFAULT;
        rmUdmaRxReq.rx_burst_size       = UDMA_RX_CHAN_BURST_SIZE_DEFAULT;

        if(CSL_PASS != Sciclient_rmUdmapRxChCfg(&rmUdmaRxReq, &rmUdmaRxResp, UDMA_SCICLIENT_TIMEOUT))
        {
            retVal = E_NOT_OK;
        }
    }
    else
    {
        retVal = E_NOT_OK;
    }

    return retVal;
}

static Std_ReturnType AppUtils_EthSciClientFlowConfig(
    uint8 ctrlIdx,
    uint8 flowIdx)
{
    Std_ReturnType  retVal = E_OK;
    struct tisci_msg_rm_udmap_flow_cfg_req              rmFlowReq;
    struct tisci_msg_rm_udmap_flow_cfg_resp             rmFlowResp;
    sint32 cslRet = CSL_PASS;

    rmFlowReq.valid_params         = TISCI_MSG_VALUE_RM_UDMAP_FLOW_EINFO_PRESENT_VALID |
                                     TISCI_MSG_VALUE_RM_UDMAP_FLOW_PSINFO_PRESENT_VALID |
                                     TISCI_MSG_VALUE_RM_UDMAP_FLOW_ERROR_HANDLING_VALID |
                                     TISCI_MSG_VALUE_RM_UDMAP_FLOW_DESC_TYPE_VALID |
                                     TISCI_MSG_VALUE_RM_UDMAP_FLOW_SOP_OFFSET_VALID |
                                     TISCI_MSG_VALUE_RM_UDMAP_FLOW_DEST_QNUM_VALID |
                                     TISCI_MSG_VALUE_RM_UDMAP_FLOW_SRC_TAG_HI_VALID |
                                     TISCI_MSG_VALUE_RM_UDMAP_FLOW_SRC_TAG_LO_VALID |
                                     TISCI_MSG_VALUE_RM_UDMAP_FLOW_DEST_TAG_HI_VALID |
                                     TISCI_MSG_VALUE_RM_UDMAP_FLOW_DEST_TAG_LO_VALID |
                                     TISCI_MSG_VALUE_RM_UDMAP_FLOW_SRC_TAG_HI_SEL_VALID |
                                     TISCI_MSG_VALUE_RM_UDMAP_FLOW_SRC_TAG_LO_SEL_VALID |
                                     TISCI_MSG_VALUE_RM_UDMAP_FLOW_DEST_TAG_HI_SEL_VALID |
                                     TISCI_MSG_VALUE_RM_UDMAP_FLOW_DEST_TAG_LO_SEL_VALID |
                                     TISCI_MSG_VALUE_RM_UDMAP_FLOW_FDQ0_SZ0_QNUM_VALID |
                                     TISCI_MSG_VALUE_RM_UDMAP_FLOW_FDQ1_QNUM_VALID |
                                     TISCI_MSG_VALUE_RM_UDMAP_FLOW_FDQ2_QNUM_VALID |
                                     TISCI_MSG_VALUE_RM_UDMAP_FLOW_FDQ3_QNUM_VALID |
                                     TISCI_MSG_VALUE_RM_UDMAP_FLOW_PS_LOCATION_VALID;
    rmFlowReq.nav_id               = UDMA_DEVICE_ID_UDMA;
    rmFlowReq.flow_index           = (uint16)Eth_GetFlowId(ctrlIdx, flowIdx);
    rmFlowReq.rx_dest_qnum         = (uint16)Eth_GetRingHwId(ctrlIdx, Eth_GetFlowCqRingIdx(ctrlIdx, flowIdx));
    rmFlowReq.rx_einfo_present     = UDMA_FLOW_EINFO_PRESENT_DEFAULT;
    rmFlowReq.rx_psinfo_present    = UDMA_FLOW_PSWORDS_PRESENT_DEFAULT;
    rmFlowReq.rx_error_handling    = UDMA_FLOW_ERROR_HANDLING;
    rmFlowReq.rx_desc_type         = UDMA_FLOW_RX_DESC_TYPE;
    rmFlowReq.rx_ps_location       = UDMA_FLOW_RX_PS_LOCALTION;
    rmFlowReq.rx_sop_offset        = UDMA_FLOW_SOP_OFFSET_DEFAULT;
    rmFlowReq.rx_src_tag_hi        = UDMA_FLOW_SRC_TAG_HIGH_DEFAULT;
    rmFlowReq.rx_src_tag_lo        = UDMA_FLOW_SRC_TAG_LOW_DEFAULT;
    rmFlowReq.rx_src_tag_hi_sel    = UDMA_FLOW_SRC_TAG_HIGH_SEL_DEFAULT;
    rmFlowReq.rx_src_tag_lo_sel    = UDMA_FLOW_SRC_TAG_LOW_SEL_DEFAULT;
    rmFlowReq.rx_dest_tag_hi       = UDMA_FLOW_DST_TAG_HIGH_DEFAULT;
    rmFlowReq.rx_dest_tag_lo       = UDMA_FLOW_DST_TAG_LOW_DEFAULT;
    rmFlowReq.rx_dest_tag_hi_sel   = UDMA_FLOW_DST_TAG_HIGH_SEL_DEFAULT;
    rmFlowReq.rx_dest_tag_lo_sel   = UDMA_FLOW_DST_TAG_LOW_SEL_DEFAULT;
    rmFlowReq.rx_fdq0_sz0_qnum     = (uint16)Eth_GetRingHwId(ctrlIdx, Eth_GetFlowFqRingIdx(ctrlIdx, flowIdx));
    rmFlowReq.rx_fdq1_qnum         = (uint16)Eth_GetRingHwId(ctrlIdx, Eth_GetFlowFqRingIdx(ctrlIdx, flowIdx));
    rmFlowReq.rx_fdq2_qnum         = (uint16)Eth_GetRingHwId(ctrlIdx, Eth_GetFlowFqRingIdx(ctrlIdx, flowIdx));
    rmFlowReq.rx_fdq3_qnum         = (uint16)Eth_GetRingHwId(ctrlIdx, Eth_GetFlowFqRingIdx(ctrlIdx, flowIdx));

    cslRet = Sciclient_rmUdmapFlowCfg(&rmFlowReq, &rmFlowResp, UDMA_SCICLIENT_TIMEOUT);

    if(CSL_PASS != cslRet)
    {
        retVal = E_NOT_OK;
    }

    return retVal;
}

static Std_ReturnType AppUtils_EthSciClientMasterEventConfig(
    uint8 ctrlIdx,
    uint8 eventIdx)
{
    Std_ReturnType retVal = E_OK;
    struct tisci_msg_rm_irq_set_req     rmIrqReq;
    struct tisci_msg_rm_irq_set_resp    rmIrqResp;
    sint32 cslRet = CSL_PASS;

#if (ETH_DMA_IR_SUPPORT == STD_ON)
    uint16 dstId = 0U;
    uint16 irId = 0U;

    cslRet |= Sciclient_rmIrqTranslateIrOutput(
        UDMA_DEVICE_ID_IR,
        (uint16)Eth_GetEventIrIntrNum(ctrlIdx, eventIdx),
        UDMA_DEVICE_ID_CORE,
        &dstId);

    cslRet |= Sciclient_rmIrqTranslateIrqInput(
        UDMA_DEVICE_ID_CORE,
        (uint16)Eth_GetEventCoreIntrNum(ctrlIdx, eventIdx),
        UDMA_DEVICE_ID_IR,
        &irId);

    (void)dstId;
    (void)irId;
#endif
    if(CSL_PASS == cslRet)
    {
        rmIrqReq.valid_params           = 0U;
        rmIrqReq.global_event           = 0U;
        rmIrqReq.src_id                 = 0U;
        rmIrqReq.src_index              = 0U;
        rmIrqReq.dst_id                 = 0U;
        rmIrqReq.dst_host_irq           = 0U;
        rmIrqReq.ia_id                  = 0U;
        rmIrqReq.vint                   = 0U;
        rmIrqReq.vint_status_bit_index  = 0U;
        rmIrqReq.secondary_host         = TISCI_MSG_VALUE_RM_UNUSED_SECONDARY_HOST;

        rmIrqReq.valid_params |= TISCI_MSG_VALUE_RM_DST_ID_VALID;
        rmIrqReq.dst_id        = UDMA_DEVICE_ID_CORE;
        rmIrqReq.valid_params |= TISCI_MSG_VALUE_RM_DST_HOST_IRQ_VALID;
        rmIrqReq.dst_host_irq  = (uint16)Eth_GetEventCoreIntrNum(ctrlIdx, eventIdx);
        rmIrqReq.valid_params |= TISCI_MSG_VALUE_RM_IA_ID_VALID;
        rmIrqReq.ia_id         = UDMA_DEVICE_ID_IA;
        rmIrqReq.valid_params |= TISCI_MSG_VALUE_RM_VINT_VALID;
        rmIrqReq.vint          = (uint16)Eth_GetEventVirtIntrNum(ctrlIdx, eventIdx);

        if(CSL_PASS != Sciclient_rmIrqSet(&rmIrqReq, &rmIrqResp, UDMA_SCICLIENT_TIMEOUT))
        {
            retVal = E_NOT_OK;
        }
    }

    return (retVal);
}

static Std_ReturnType AppUtils_EthSciClientRingEventConfig(
    uint8 ctrlIdx,
    uint8 ringEventIdx)
{
    Std_ReturnType retVal = E_OK;
    struct tisci_msg_rm_irq_set_req     rmIrqReq;
    struct tisci_msg_rm_irq_set_resp    rmIrqResp;

    rmIrqReq.valid_params           = 0U;
    rmIrqReq.global_event           = 0U;
    rmIrqReq.src_id                 = 0U;
    rmIrqReq.src_index              = 0U;
    rmIrqReq.dst_id                 = 0U;
    rmIrqReq.dst_host_irq           = 0U;
    rmIrqReq.ia_id                  = 0U;
    rmIrqReq.vint                   = 0U;
    rmIrqReq.vint_status_bit_index  = 0U;
    rmIrqReq.secondary_host         = TISCI_MSG_VALUE_RM_UNUSED_SECONDARY_HOST;

    rmIrqReq.valid_params |= TISCI_MSG_VALUE_RM_VINT_STATUS_BIT_INDEX_VALID;
    rmIrqReq.valid_params |= TISCI_MSG_VALUE_RM_GLOBAL_EVENT_VALID;
    rmIrqReq.global_event  = (uint16)Eth_GetRingEventGlobalEventNum(ctrlIdx, ringEventIdx);
    rmIrqReq.valid_params |= TISCI_MSG_VALUE_RM_DST_ID_VALID;
    rmIrqReq.valid_params |= TISCI_MSG_VALUE_RM_DST_HOST_IRQ_VALID;
    rmIrqReq.dst_id        = UDMA_DEVICE_ID_CORE;
    rmIrqReq.dst_host_irq  = (uint16)Eth_GetEventCoreIntrNum(ctrlIdx, Eth_GetRingEventEventIdx(ctrlIdx, ringEventIdx));
    rmIrqReq.valid_params |= TISCI_MSG_VALUE_RM_IA_ID_VALID;
    rmIrqReq.valid_params |= TISCI_MSG_VALUE_RM_VINT_VALID;
    rmIrqReq.ia_id         = UDMA_DEVICE_ID_IA;
    rmIrqReq.vint          = (uint16)Eth_GetEventVirtIntrNum(ctrlIdx, Eth_GetRingEventEventIdx(ctrlIdx, ringEventIdx));
#if (ETH_DMA_IR_SUPPORT == STD_ON)
    rmIrqReq.src_id        = UDMA_DEVICE_ID_RING;
#else
    rmIrqReq.src_id        = UDMA_DEVICE_ID_IA;
#endif
    rmIrqReq.src_index     = (uint16)(Eth_GetRingHwId(ctrlIdx, ringEventIdx) +
                                      Eth_GetRingEventSrcOffsetNum(ctrlIdx, ringEventIdx));

    rmIrqReq.valid_params |= TISCI_MSG_VALUE_RM_VINT_STATUS_BIT_INDEX_VALID;
    rmIrqReq.vint_status_bit_index  = (uint8)Eth_GetRingEventVirtBitNum(ctrlIdx, ringEventIdx);

    if(CSL_PASS != Sciclient_rmIrqSet(&rmIrqReq, &rmIrqResp, UDMA_SCICLIENT_TIMEOUT))
    {
        retVal = E_NOT_OK;
    }

    return (retVal);
}

static Std_ReturnType AppUtils_EthSciClientRingEventRelease(
    uint8 ctrlIdx,
    uint8 ringEventIdx)
{
    Std_ReturnType retVal = E_OK;
    struct tisci_msg_rm_irq_release_req     rmIrqReq;

    rmIrqReq.valid_params           = 0U;
    rmIrqReq.global_event           = 0U;
    rmIrqReq.src_id                 = 0U;
    rmIrqReq.src_index              = 0U;
    rmIrqReq.dst_id                 = 0U;
    rmIrqReq.dst_host_irq           = 0U;
    rmIrqReq.ia_id                  = 0U;
    rmIrqReq.vint                   = 0U;
    rmIrqReq.vint_status_bit_index  = 0U;
    rmIrqReq.secondary_host         = TISCI_MSG_VALUE_RM_UNUSED_SECONDARY_HOST;

    rmIrqReq.valid_params |= TISCI_MSG_VALUE_RM_VINT_STATUS_BIT_INDEX_VALID;
    rmIrqReq.valid_params |= TISCI_MSG_VALUE_RM_GLOBAL_EVENT_VALID;
    rmIrqReq.global_event  = (uint16)Eth_GetRingEventGlobalEventNum(ctrlIdx, ringEventIdx);
    rmIrqReq.valid_params |= TISCI_MSG_VALUE_RM_DST_ID_VALID;
    rmIrqReq.valid_params |= TISCI_MSG_VALUE_RM_DST_HOST_IRQ_VALID;
    rmIrqReq.dst_id        = UDMA_DEVICE_ID_CORE;
    rmIrqReq.dst_host_irq  = (uint16)Eth_GetEventCoreIntrNum(ctrlIdx, Eth_GetRingEventEventIdx(ctrlIdx, ringEventIdx));
    rmIrqReq.valid_params |= TISCI_MSG_VALUE_RM_IA_ID_VALID;
    rmIrqReq.valid_params |= TISCI_MSG_VALUE_RM_VINT_VALID;
    rmIrqReq.ia_id         = UDMA_DEVICE_ID_IA;
    rmIrqReq.vint          = (uint16)Eth_GetEventVirtIntrNum(ctrlIdx, Eth_GetRingEventEventIdx(ctrlIdx, ringEventIdx));
#if (ETH_DMA_IR_SUPPORT == STD_ON)
    rmIrqReq.src_id        = UDMA_DEVICE_ID_RING;
#else
    rmIrqReq.src_id        = UDMA_DEVICE_ID_IA;
#endif
    rmIrqReq.src_index     = (uint16)(Eth_GetRingHwId(ctrlIdx, ringEventIdx) +
                                      Eth_GetRingEventSrcOffsetNum(ctrlIdx, ringEventIdx));

    rmIrqReq.valid_params |= TISCI_MSG_VALUE_RM_VINT_STATUS_BIT_INDEX_VALID;
    rmIrqReq.vint_status_bit_index  = (uint8)Eth_GetRingEventVirtBitNum(ctrlIdx, ringEventIdx);

    if(CSL_PASS != Sciclient_rmIrqRelease(&rmIrqReq, UDMA_SCICLIENT_TIMEOUT))
    {
        retVal = E_NOT_OK;
    }

    return (retVal);
}

static Std_ReturnType AppUtils_EthSciClientProxyConfig(
    uint8 ctrlIdx,
    uint8 proxyIdx)
{
    Std_ReturnType retVal = E_NOT_OK;
#if (ETH_DMA_PROXY_SUPPORT == STD_ON)
    struct tisci_msg_rm_proxy_cfg_req   req;

    req.valid_params = 0U;
    req.nav_id       = UDMA_DEVICE_ID_PROXY;
    req.index        = (uint16)Eth_GetProxyThreadNum(ctrlIdx, proxyIdx);

    if(CSL_PASS != Sciclient_rmSetProxyCfg(&req, UDMA_SCICLIENT_TIMEOUT))
    {
        retVal = E_NOT_OK;
    }
    else
    {
        retVal = E_OK;
    }
#endif
    return retVal;
}

