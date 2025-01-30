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
 *  \file     Cpsw_Mdio.h
 *
 *  \brief    This file contains the structure definitions and function
 *            prototypes of the MDIO module.
 */

/**
 *  \addtogroup CPSW_MDIO_API
 *  @{
 */

#ifndef CPSWMCAL_MDIO_H_
#define CPSWMCAL_MDIO_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "csl_cpsw.h"
#include "csl_mdio.h"
#include "Cpsw_Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                                 Macros                                     */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Global Variables Declarations                      */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief MDIO module open function
 *
 *  Opens the MDIO module and performs an initial configuration according to
 *  the config structure being passed.  The MDIO clock divider and the state
 *  machine will be configured when the function returns.
 *
 *  This function must be called before any other CpswMdio_*() function.
 *
 *
 *  \return none
 */
void CpswMdio_open(void);

/**
 *  \brief MDIO module close function
 *
 *  Closes the MDIO module.
 *
 *  No CpswMdio_*() functions must be called after this function.
 *
 *  \param none
 *
 *  \return none
 */
void CpswMdio_close(void);

/**
 *  \brief Check if a PHY is alive
 *
 *  Checks if the PHY with the given address is alive.  The PHY is said to be
 *  alive if it acknowledged the read of the Generic Status Register.
 *
 *  \param ctrlIdx  Eth control idx
 *  \param phyAddr  PHY address
 *
 *  \return         TRUE if the PHY is alive, FALSE otherwise
 */
boolean CpswMdio_isPhyAlive(uint8 ctrlIdx, uint32 phyAddr);

/**
 *  \brief Write a PHY register synchronously
 *
 *  Writes a PHY register in a synchronous manner, which means that this
 *  function returns after the write operation is complete or a timeout has
 *  occurred. (Used only during ETH_MDIO_OPMODE_MANUAL)
 *
 *  \param ctrlIdx  Eth control idx
 *  \param phyAddr  PHY address
 *  \param reg      Register address
 *  \param val      Value to be written
 *
 *  \return \ref Cpsw_ErrorCodes
 */
void CpswMdio_writePhyReg(uint8 ctrlIdx,
                          uint32 phyAddr,
                          uint32 reg,
                          uint16 val);

/**
 *  \brief Read a PHY register synchronously
 *
 *  Reads a PHY register in a synchronous manner, which means that this
 *  function returns after the read operation is complete or a timeout has
 *  occurred. (Used only during ETH_MDIO_OPMODE_MANUAL)
 *
 *  \param ctrlIdx  Eth control idx
 *  \param phyAddr  PHY address
 *  \param reg      Register address
 *  \param val      Pointer to the variable where read value will be placed
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType CpswMdio_readPhyReg(uint8 ctrlIdx,
                                   uint32 phyAddr,
                                   uint32 reg,
                                   uint16 *val);

/**
 *  \brief Write a PHY register asynchronously
 *
 *  Writes a PHY register in an asynchronous manner, which means that this
 *  function trigger the write operation and returns immediately.  The
 *  write operation is completed once the MDIO interrupt occurs.
 *
 *  \param phyAddr  PHY address
 *  \param reg      Register address
 *  \param val      Value to be written
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType CpswMdio_initiatePhyRegWrite(uint32 phyAddr,
                                            uint32 reg,
                                            uint16 val);

/**
 *  \brief Read a PHY register asynchronously
 *
 *  Read a PHY register in an asynchronous manner, which means that this
 *  function trigger the read operation and returns immediately.  The
 *  read operation is completed once the MDIO interrupt occurs.
 *
 *  \param phyAddr  PHY address
 *  \param reg      Register address
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType CpswMdio_initiatePhyRegRead(uint32 phyAddr,
                                           uint32 reg);

/**
 *  \brief Get the value read from a PHY register
 *
 *  This function is expected to be called while servicing the MDIO interrupt.
 *  PHY register is read asynchronously, i.e. read needs to be triggered first
 *  before calling this function.
 *
 *  \param ctrlIdx  Eth control idx
 *  \param val      Pointer to the variable where read value will be placed
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType CpswMdio_getPhyRegVal(uint8 ctrlIdx,
                                     uint16 *val);

/**
 *  \brief Clear the MDIO interrupt
 *
 *  Clears the MDIO interrupt.
 *
 *  \param none
 */
void CpswMdio_clearUserInt(void);

/**
 *  \brief check read or write MII opereration pending or complete
 *
 *  \param none
 *
 *  \return TRUE if operation pending or FALSE is operation completed
 */
boolean CpswMdio_isPhyRegReadWritePending(void);


/* ========================================================================== */
/*                        Deprecated Function Declarations                    */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
}
#endif

#endif /* CPSWMCAL_MDIO_H_ */

/*! @} */
