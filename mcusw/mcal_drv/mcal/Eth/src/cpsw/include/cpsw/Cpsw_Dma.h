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
 *  \file     Cpsw_Dma.h
 *
 *  \brief    This file contains the structure definitions and function
 *            prototypes of the DMA module.
 */

/**
 *  \addtogroup CPSW_DMA_API
 *  @{
 */

#ifndef CPSWMCAL_DMA_H_
#define CPSWMCAL_DMA_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "cslr.h"
#include "Cpsw_Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                             Inline Functun Declarations                    */
/* ========================================================================== */

/**
 *  \brief Check IRQ is set in ring event index
 *
 *
 *  \param ctrlIdx      Eth Control index
 *  \param ringEvtIdx   Ring event index
 *
 *  \return TRUE if IRQ is set else FALSE
 */
static inline boolean CpswDma_Ring_IrqIsSet(
    uint8 ctrlIdx,
    uint8 ringEvtIdx);

/**
 *  \brief Clear IRQ in ring event index
 *
 *
 *  \param ctrlIdx      Eth Control index
 *  \param ringEvtIdx   Ring event index
 *
 *  \return none
 */
static inline void CpswDma_Ring_IrqClear(
    uint8 ctrlIdx,
    uint8 ringEvtIdx);

/* ========================================================================== */
/*                        Global Function Declarations                        */
/* ========================================================================== */

/**
 *  \brief Perform init function for all DMA ring(Init only for ring software counter)
 *
 *  Reset all DMA ring for a given eth control index
 *
 *  \param ctrlIdx      Eth Control index
 *
 *  \return None
 */
void CpswDma_initRings(uint8 ctrlIdx);

/**
 *  \brief Perform reset function for all DMA ring
 *
 *  Reset all DMA ring for a given eth control index
 *
 *  \param ctrlIdx      Eth Control index
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType CpswDma_resetRings(uint8 ctrlIdx);

/**
 *  \brief DMA enable function
 *
 *  Enable the DMA module for a given eth control index
 *
 *  \param ctrlIdx      Eth Control index
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType CpswDma_enable(uint8 ctrlIdx);

/**
 *  \brief DMA disable function
 *
 *  Disable the DMA module for a given eth control index
 *
 *  \param ctrlIdx      Eth Control index
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType CpswDma_disable(uint8 ctrlIdx);

/**
 *  \brief Submit a desciptor to DMA
 *
 *  Submit a host packet desciptor to DMA
 *
 *  \param ctrlIdx      Eth Control index
 *  \param RingIdx      Dma ring index
 *  \param DescAddr     Desciptor address
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType CpswDma_submitDesc(
    uint8 ctrlIdx,
    uint8 RingIdx,
    uint64 DescAddr);

/**
 *  \brief Retrive a desciptor from DMA
 *
 *  Retrive a host packet desciptor from DMA
 *
 *  \param ctrlIdx      Eth Control index
 *  \param RingIdx      Dma ring index
 *  \param DescAddrPtr  Desciptor address
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType CpswDma_retrieveDesc(
    uint8 ctrlIdx,
    uint8 RingIdx,
    uint64 *DescAddrPtr);

/**
 *  \brief Flush a desciptor from DMA
 *
 *  Retrive a host packet desciptor from DMA
 *
 *  \param ctrlIdx      Eth Control index
 *  \param RingIdx      Dma ring index
 *  \param DescAddrPtr  Desciptor address
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType CpswDma_flushDesc(
    uint8 ctrlIdx,
    uint8 RingIdx,
    uint64 *DescAddrPtr);

/**
 *  \brief Retrive all read pending desciptor from DMA
 *
 *  Retrive a host packet desciptor from DMA
 *
 *  \param ctrlIdx      Eth Control index
 *  \param RingIdx      Dma ring index
 *
 *  \return \ref Std_ReturnType
 */
uint32 CpswDma_retrieveReadPendingDescNumber(
    uint8 ctrlIdx,
    uint8 RingIdx);

/**
 *  \brief Retrive all write pending desciptor from DMA
 *
 *  Retrive a host packet desciptor from DMA
 *
 *  \param ctrlIdx      Eth Control index
 *  \param RingIdx      Dma ring index
 *
 *  \return \ref Std_ReturnType
 */
uint32 CpswDma_retrieveWritePendingDescNumber(
    uint8 ctrlIdx,
    uint8 RingIdx);

/* ========================================================================== */
/*                             Inline Functun Definintion                     */
/* ========================================================================== */

static inline boolean CpswDma_Ring_IrqIsSet(
    uint8 ctrlIdx,
    uint8 ringEvtIdx)
{
    boolean retVal = FALSE;
    uint32 vIntrNum = Eth_GetEventVirtIntrNum(ctrlIdx,
                      Eth_GetRingEventEventIdx(ctrlIdx, ringEvtIdx));

    if((CSL_REG64_RD(ETH_DMA_INTAGGR_INTR_VINT_STATUSM(vIntrNum)) &
        (uint64)(1LLU << Eth_GetRingEventVirtBitNum(ctrlIdx, ringEvtIdx))) != 0U)
    {
        retVal = TRUE;
    }

    return retVal;
}

static inline void CpswDma_Ring_IrqClear(
    uint8 ctrlIdx,
    uint8 ringEvtIdx)
{
    uint32 vIntrNum = Eth_GetEventVirtIntrNum(ctrlIdx,
                      Eth_GetRingEventEventIdx(ctrlIdx, ringEvtIdx));

    CSL_REG64_WR(ETH_DMA_INTAGGR_INTR_VINT_STATUS_CLEAR(vIntrNum),
        (uint64)(1LLU << Eth_GetRingEventVirtBitNum(ctrlIdx, ringEvtIdx)));
}

static inline void CpswDma_Ring_IrqDisable(
    uint8 ctrlIdx,
    uint8 ringEvtIdx)
{
    uint32 vIntrNum =  Eth_GetEventVirtIntrNum(ctrlIdx,
                       Eth_GetRingEventEventIdx(ctrlIdx, ringEvtIdx));

    CSL_REG64_WR(ETH_DMA_INTAGGR_INTR_VINT_ENABLE_CLEAR(vIntrNum),
        (uint64)(1LLU << Eth_GetRingEventVirtBitNum(ctrlIdx, ringEvtIdx)));
}

static inline void CpswDma_Ring_IrqEnable(
    uint8 ctrlIdx,
    uint8 ringEvtIdx)
{
    uint32 vIntrNum =  Eth_GetEventVirtIntrNum(ctrlIdx,
                       Eth_GetRingEventEventIdx(ctrlIdx, ringEvtIdx));

    CSL_REG64_WR(ETH_DMA_INTAGGR_INTR_VINT_ENABLE_SET(vIntrNum),
        (uint64)(1LLU << Eth_GetRingEventVirtBitNum(ctrlIdx, ringEvtIdx)));

}

#ifdef __cplusplus
}
#endif

#endif /* CPSWMCAL_DMA_H_ */

/*! @} */
