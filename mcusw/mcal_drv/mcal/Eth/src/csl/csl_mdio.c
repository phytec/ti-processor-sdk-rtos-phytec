/**
 * @file  csl_mdio.c
 *
 * @brief
 *  Functional layer implementation of CSL MDIO IP version 5
 *
 *  Contains the different control command and status query functions definitions
 *
 *  \par
 *  ============================================================================
 *  @n   (C) Copyright 2024, Texas Instruments, Inc.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

/**
 * \brief This is to disable HW_SYNC_BARRIER for J7 due to performance
 *        requirement
 */
#if defined (SOC_J721E) || defined (SOC_J7200)
#define MEM_BARRIER_DISABLE
#endif

#include "cslr.h"
#include "csl_cpsw.h"
#include "csl_mdio.h"

void CSL_MDIO_setClkDivVal(
    CSL_mdioHandle          hMdioRegs,
    uint16                 clkDivVal
)
{
    CSL_FINS (hMdioRegs->CONTROL_REG, MDIO_CONTROL_REG_CLKDIV, (uint32)clkDivVal);

    return;
}

void CSL_MDIO_enableStateMachine(
    CSL_mdioHandle          hMdioRegs
)
{
    CSL_FINS (hMdioRegs->CONTROL_REG, MDIO_CONTROL_REG_ENABLE, ((uint32)1U));

    return;
}

void CSL_MDIO_disableStateMachine(
    CSL_mdioHandle          hMdioRegs
)
{
    CSL_FINS (hMdioRegs->CONTROL_REG, MDIO_CONTROL_REG_ENABLE, ((uint32)0U));

    return;
}

uint32 CSL_MDIO_isPhyAlive(
    CSL_mdioHandle          hMdioRegs,
    uint32                  phyAddr
)
{
    return CSL_FEXTR (hMdioRegs->ALIVE_REG, phyAddr, phyAddr);
}

