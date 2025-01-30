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
 *  \file     mcal.h
 *
 *  \brief    This file contains cover page for generating doxygen
 *            API documentation.
 *
 */

#ifndef MCAL_H_
#define MCAL_H_

#ifdef __cplusplus
extern "C"
{
#endif

/**
 *
 *  \mainpage  AUTOSAR MCAL Drivers for TI TDA4 SoCs
 *  The Microcontroller Abstraction Layer (MCAL) is the lowest software layer of the Basic Software.
 *  It contains internal drivers, which are software modules with direct access to the uC and internal peripherals
 *  - MCAL contains internal drivers, which are software modules with direct access
 *    to the uC and internal peripherals
 *  - MCAL makes higher software layers independent of uC
 *  - MCAL presents standardized and uC independent APIs to upper layers
 *
 * <b>
 * Also refer to MCAL driver user guide for detailed features,
 * limitations and usage description.
 * </b>
 *
 * The following MCAL drivers are implemented
 *
 *  - <b> CAN Driver </b> (See \ref MCAL_CAN_API) <br>
 *       The Can drivers provides services to control the CAN controller in a hardware
 *       independent API. The Can module provides services for initiating transmissions
 *       and calls the callback functions of the CanIf module for notifying events,
 *       independently from the hardware.Furthermore, it provides services to control
 *       the behavior and state of the CAN controllers that are belonging to the same CAN Hardware Unit
 *
 *  - <b> DIO Driver </b> (See \ref MCAL_DIO_API) <br>
 *       The DIO Driver provides services for reading and writing to/from
 *       -  DIO Channels (General Purpose IO Pins)
 *       -  DIO Ports    (General Purpose IO Registers)
 *       -  DIO Channel Groups (Portion of General Purpose IO Registers)
 *
 *  - <b> ETH Driver </b> (See \ref MCAL_ETH_API) <br>
 *        The main function of Ethernet Driver is to provide to the upper layer (Ethernet Interface)
 *        a hardware independent interface comprising multiple equal controllers.
 *        This interface is uniform for all controllers.
 *        Thus, the upper layer (Ethernet Interface) may access the underlying bus system in a uniform manner.
 *        The interface provides functionality for initialization, configuration and data transmission.
 *        The configuration of the Ethernet Driver however is bus specific, since it takes into account the
 *        specific features of the communication controller
 *
 *  - <b> ETH Transceiver Driver </b> (See \ref MCAL_ETHTRCV_API) <br>
 *        This provide the interface for the transceiver driver.
 *
 *  - <b> GPT Driver </b> (See \ref MCAL_GPT_API) <br>
 *        This provide the interface for the general purpose timer driver.
 *
 */

/**
 *  \defgroup MCAL_CAN_API CAN Driver API
 */

/**
 *  \defgroup MCAL_DIO_API DIO Driver API
 */

/**
 *  \defgroup MCAL_ETH_API Ethernet Driver API
 */

/**
 *  \defgroup MCAL_ETHTRCV_API Ethernet Transceiver Driver API
 */

/**
 *  \defgroup MCAL_GPT_API GPT Driver API
 */

/**
 *  \defgroup MCAL_SPIHANDLER_API SPI Handler and Driver API
 */

#ifdef __cplusplus
}
#endif

#endif /* MCAL_H_ */
