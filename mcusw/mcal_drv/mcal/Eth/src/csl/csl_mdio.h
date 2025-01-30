/**
 * @file csl_mdio.h
 *
 * @brief
 *  Header file for functional layer of CSL MDIO Version 5
 *
 *  It contains the various enumerations, structure definitions and function
 *  declarations
 *
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
 * @defgroup CSL_MDIO_API_V5 MDIO - V5
 * @ingroup CSL_MDIO_API
 */
/**
@defgroup CSL_MDIO_SYMBOL  Symbols Defined
@ingroup CSL_MDIO_API_V5
*/
/**
@defgroup CSL_MDIO_DATASTRUCT   Data Structures
@ingroup CSL_MDIO_API_V5
*/
/**
@defgroup CSL_MDIO_FUNCTION  Functions
@ingroup CSL_MDIO_API_V5
*/
/**
@defgroup CSL_MDIO_ENUM Enumerated Data Types
@ingroup CSL_MDIO_API_V5
*/
#ifndef CSL_MDIO_V5_H
#define CSL_MDIO_V5_H

#ifdef __cplusplus
extern "C" {
#endif



/** @addtogroup CSL_MDIO_DATASTRUCT
 @{ */


/**
@}
*/

#include "csl_mdio_def.h"


#define CPSW_MDIO_OFFSET    (0x00000F00U)                   /* MDIO offset memory map */

typedef volatile CSL_MdioRegs             *CSL_mdioHandle;

/** @addtogroup CSL_MDIO_FUNCTION
@{ */

/** ============================================================================
 *   @n@b CSL_MDIO_setClkDivVal
 *
 *   @b Description
 *   @n This function configures the clock divider value (CLKDIV) in the
 *      MDIO control register with the value specified.
 *
 *   @b Arguments
     @verbatim
        clkDivVal           The value to use for clock divider configuration.
                            When this set to 0, the MDIO clock is disabled.
 *     @endverbatim
 *
 *   <b> Return Value </b>
 *     @n     None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  CONTROL_REG configured with the clock divider value. Configures the MDIO
 *       clock frequency.
 *
 *   @b Writes
 *   @n MDIO_CONTROL_REG_CLKDIV
 *
 *   @b Example
 *   @verbatim
 *      uint16      clkDivVal;

        clkDivVal = 165;

        // Setup the MDIO clock frequency
        CSL_MDIO_setClkDivVal (clkDivVal);

     @endverbatim
 * =============================================================================
 */
void CSL_MDIO_setClkDivVal(
        CSL_mdioHandle          hMdioRegs,
        uint16                 clkDivVal
);

/** ============================================================================
 *   @n@b CSL_MDIO_enableStateMachine
 *
 *   @b Description
 *   @n This function enables the MDIO state machine if not already active.
 *
 *   @b Arguments   None
 *
 *   <b> Return Value </b>
 *     @n     None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  Enables MDIO State machine.
 *
 *   @b Writes
 *   @n MDIO_CONTROL_REG_ENABLE
 *
 *   @b Example
 *   @verbatim
 *
        // Enable MDIO state machine
        CSL_MDIO_enableStateMachine ();

     @endverbatim
 * =============================================================================
 */
void CSL_MDIO_enableStateMachine(
        CSL_mdioHandle          hMdioRegs
);

/** ============================================================================
 *   @n@b CSL_MDIO_disableStateMachine
 *
 *   @b Description
 *   @n This function sets the 'ENABLE' bit of the MDIO control register to 0,
 *      triggering the disable of MDIO state machine if active.
 *
 *   @b Arguments   None
 *
 *   <b> Return Value </b>
 *     @n     None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  If the MDIO state machine is active at the time it is disabled, will
 *       complete the current operation before halting it and setting the
 *       idle bit.
 *
 *   @b Writes
 *   @n MDIO_CONTROL_REG_ENABLE=0
 *
 *   @b Example
 *   @verbatim
 *
        // Disable MDIO state machine
        CSL_MDIO_disableStateMachine ();

     @endverbatim
 * =============================================================================
 */
void CSL_MDIO_disableStateMachine(
        CSL_mdioHandle          hMdioRegs
);

/** ============================================================================
 *   @n@b CSL_MDIO_isPhyAlive
 *
 *   @b Description
 *   @n For a given PHY address provided to this function in 'phyAddr',  this function
 *      reads the 'ALIVE' bit corresponding to that PHY; The function returns 1 to
 *      indicate that the most recent access for the address was acknowledged by the
 *      PHY and 0 otherwise.
 *
 *   @b Arguments
 *   @verbatim
 *      phyAddr             The PHY address number (0-31) for which the alive bit
 *                          status must be checked and returned by this function.
 *   @endverbatim
 *
 *   <b> Return Value </b>  uint32
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Reads
 *   @n MDIO_ALIVE_REG
 *
 *   @b Example
 *   @verbatim
 *      uint32      phyAddr = 0;

        // Check if PHY Address 0 is alive
        if (CSL_MDIO_isPhyAlive (phyAddr))
        {
            // MDIO PHY 0 access succeeded
        }
        else
        {
            // MDIO PHY 0 access failed
        }

     @endverbatim
 * =============================================================================
 */
uint32 CSL_MDIO_isPhyAlive(
        CSL_mdioHandle          hMdioRegs,
        uint32                  phyAddr
);

/**
@}
*/
#ifdef __cplusplus
}
#endif

#endif /* CSL_MDIO_V5_H */
