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
 *  \file     Eth_Irq.h
 *
 *  \brief    This file contains ISR function declaration for Ethernet driver.
 */

/**
 *  \ingroup MCAL_ETH_API
 *  \defgroup MCAL_ETH_IRQ Eth Driver Interrupt Handlers
 *  @{
 */

#ifndef ETH_IRQ_H_
#define ETH_IRQ_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "Os.h"

/* None */

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \name Eth Driver ISR category level
 *
 *  Definitions for Eth Driver ISR ISR category level.
 *  @{
 */
/** \brief void ISR type */
#define ETH_ISR_VOID                    (0x00U)
/** \brief Category 1 ISR type */
#define ETH_ISR_CAT1                    (0x01U)
/** \brief Category 2 ISR type */
#define ETH_ISR_CAT2                    (0x02U)
/*! @} */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

#define ETH_START_SEC_ISR_CODE
#include "Eth_MemMap.h"

/**
 *  \brief ISR for frame reception interrupts of the indexed controller 0
 *
 *  \verbatim
 *  Service name      : Eth_RxIrqHdlr_0
 *  Syntax            : void Eth_RxIrqHdlr_0(
 *                          void
 *                      )
 *  Service ID[hex]   : 0x10
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : None
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : None
 *  Description       : Handles frame reception interrupts of the indexed
 *                      controller 0.
 *  \endverbatim
 */
#if (ETH_ISR_TYPE == ETH_ISR_CAT1 || ETH_ISR_TYPE == ETH_ISR_VOID)
FUNC(void, ETH_CODE_FAST) Eth_RxIrqHdlr_0(void);
#elif (ETH_ISR_TYPE == ETH_ISR_CAT2)
ISR(Eth_RxIrqHdlr_0);
#endif

/**
 *  \brief ISR for frame transmission interrupts of the indexed controller 0
 *
 *  \verbatim
 *  Service name      : Eth_TxIrqHdlr_0
 *  Syntax            : void Eth_TxIrqHdlr_0(
 *                          void
 *                      )
 *  Service ID[hex]   : 0x11
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : None
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : None
 *  Description       : Handles frame transmission interrupts of the indexed
 *                      controller 0.
 *  \endverbatim
 */
#if (ETH_ISR_TYPE == ETH_ISR_CAT1 || ETH_ISR_TYPE == ETH_ISR_VOID)
FUNC(void, ETH_CODE_FAST) Eth_TxIrqHdlr_0(void);
#elif (ETH_ISR_TYPE == ETH_ISR_CAT2)
ISR(Eth_TxIrqHdlr_0);
#endif

/**
 *  \brief ISR for RX pacing interrupts of the indexed controller 0
 *
 *  \verbatim
 *  Service name      : Eth_RxIrqPacingHdlr_0
 *  Syntax            : void Eth_RxIrqPacingHdlr_0(
 *                          void
 *                      )
 *  Service ID[hex]   : NA
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : None
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : None
 *  Description       : Handles RX pacing interrupts of the indexed
 *                      controller 0.
 *  \endverbatim
 */
#if (ETH_ISR_TYPE == ETH_ISR_CAT1 || ETH_ISR_TYPE == ETH_ISR_VOID)
FUNC(void, ETH_CODE_FAST) Eth_RxIrqPacingHdlr_0(void);
#else
ISR(Eth_RxIrqPacingHdlr_0);
#endif

/**
 *  \brief ISR for TX pacing interrupts of the indexed controller 0
 *
 *  \verbatim
 *  Service name      : Eth_TxIrqPacingHdlr_0
 *  Syntax            : void Eth_TxIrqPacingHdlr_0(
 *                          void
 *                      )
 *  Service ID[hex]   : NA
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : None
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : None
 *  Description       : Handles TX pacing interrupts of the indexed
 *                      controller 0.
 *  \endverbatim
 */
#if (ETH_ISR_TYPE == ETH_ISR_CAT1 || ETH_ISR_TYPE == ETH_ISR_VOID)
FUNC(void, ETH_CODE_FAST) Eth_TxIrqPacingHdlr_0(void);
#else
ISR(Eth_TxIrqPacingHdlr_0);
#endif

#if (ETH_CTRL_ID_MAX > 0)
/**
 *  \brief ISR for frame reception interrupts of the indexed controller 1
 *
 *  \verbatim
 *  Service name      : Eth_RxIrqHdlr_1
 *  Syntax            : void Eth_RxIrqHdlr_1(
 *                          void
 *                      )
 *  Service ID[hex]   : NA
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : None
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : None
 *  Description       : Handles frame reception interrupts of the indexed
 *                      controller 1.
 *  \endverbatim
 */
#if (ETH_ISR_TYPE == ETH_ISR_CAT1 || ETH_ISR_TYPE == ETH_ISR_VOID)
FUNC(void, ETH_CODE_FAST) Eth_RxIrqHdlr_1(void)
#else
ISR(Eth_RxIrqHdlr_1);
#endif

/**
 *  \brief ISR for frame transmission interrupts of the indexed controller 1
 *
 *  \verbatim
 *  Service name      : Eth_TxIrqHdlr_1
 *  Syntax            : void Eth_TxIrqHdlr_1(
 *                          void
 *                      )
 *  Service ID[hex]   : NA
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : None
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : None
 *  Description       : Handles frame transmission interrupts of the indexed
 *                      controller 1.
 *  \endverbatim
 */

#if (ETH_ISR_TYPE == ETH_ISR_CAT1 || ETH_ISR_TYPE == ETH_ISR_VOID)
FUNC(void, ETH_CODE_FAST) Eth_TxIrqHdlr_1(void);
#else
ISR(Eth_TxIrqHdlr_1);
#endif

/**
 *  \brief ISR for RX pacing interrupts of the indexed controller 1
 *
 *  \verbatim
 *  Service name      : Eth_RxIrqPacingHdlr_1
 *  Syntax            : void Eth_RxIrqPacingHdlr_1(
 *                          void
 *                      )
 *  Service ID[hex]   : NA
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : None
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : None
 *  Description       : Handles RX pacing interrupts of the indexed
 *                      controller 1.
 *  \endverbatim
 */
#if (ETH_ISR_TYPE == ETH_ISR_CAT1 || ETH_ISR_TYPE == ETH_ISR_VOID)
FUNC(void, ETH_CODE_FAST) Eth_RxIrqPacingHdlr_1(void)
#else
ISR(Eth_RxIrqPacingHdlr_1);
#endif

/**
 *  \brief ISR for RX pacing interrupts of the indexed controller 1
 *
 *  \verbatim
 *  Service name      : Eth_RxIrqPacingHdlr_1
 *  Syntax            : void Eth_RxIrqPacingHdlr_1(
 *                          void
 *                      )
 *  Service ID[hex]   : NA
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : None
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : None
 *  Description       : Handles RX pacing interrupts of the indexed
 *                      controller 1.
 *  \endverbatim
 */
#if (ETH_ISR_TYPE == ETH_ISR_CAT1 || ETH_ISR_TYPE == ETH_ISR_VOID)
FUNC(void, ETH_CODE_FAST) Eth_TxIrqPacingHdlr_1(void);
#else
ISR(Eth_TxIrqPacingHdlr_1);
#endif
#endif /* (ETH_CTRL_ID_MAX > 0) */

/**
 *  \brief ISR for MDIO interrupts of all controllers
 *
 *  \verbatim
 *  Service name      : Eth_MdioIrqHdlr
 *  Syntax            : void Eth_MdioIrqHdlr(
 *                          void
 *                      )
 *  Service ID[hex]   : NA
 *  Sync/Async        : Synchronous
 *  Reentrancy        : Non Reentrant
 *  Parameters (in)   : None
 *  Parameters (inout): None
 *  Parameters (out)  : None
 *  Return value      : None
 *  Description       : Handles MDIO interrupts of all controllers
 *  \endverbatim
 */
#if (STD_ON == ETH_ENABLE_MII_API)
#if (ETH_ISR_TYPE == ETH_ISR_CAT1 || ETH_ISR_TYPE == ETH_ISR_VOID)
FUNC(void, ETH_CODE_FAST) Eth_MdioIrqHdlr(void);
#elif (ETH_ISR_TYPE == ETH_ISR_CAT2)
ISR(Eth_MdioIrqHdlr);
#endif
#endif

#define ETH_STOP_SEC_ISR_CODE
#include "Eth_MemMap.h"

#ifdef __cplusplus
}
#endif

#endif  /* ETH_IRQ_H_ */

/*! @} */
