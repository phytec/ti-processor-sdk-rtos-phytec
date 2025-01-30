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

#ifndef ETH_RPC_PRIV_H_
#define ETH_RPC_PRIV_H_

#include <stdint.h>

#include "Eth.h"
#include "Eth_Priv.h"
#include "Eth_Cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  \brief Announce Ethernet MCAL driver service is up to ethernet firmware core
 *
 *  On initialization of AUTOSAR MCAL driver in virtual mac mode, the driver
 *  announces autosar ethernet mcal driver RpMsg service to the ethernet
 *  firmware core. The ethernet firmware server service will block  waiting for
 *  the announcment from MCAL driver and then attach to the Ethernet MCAL driver
 *  by sending it a EthFwInfo msg
 *
 *  \param ctrlIdx  Eth Controller Index
 *
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType Eth_RpcAnnounce(uint8 ctrlIdx);


/**
 *  \brief Check and process and msgs received from ethernet firmware
 *
 *  \param ctrlIdx      Eth Controller Index
 *  \param callerApiId   Caller API id is used for DET Error reporting
 *
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType Eth_RpcRecvMsg(uint8 ctrlIdx, uint8 callerApiId);

/**
 *  \brief Send extended attach request to the ethernet firmware
 *
 *  \param ctrlIdx  Eth Controller Index
 *
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType Eth_RpcSendExtendedAttachReq(uint8 ctrlIdx);

/**
 *  \brief Send detach request to the ethernet firmware
 *
 *  \param ctrlIdx  Eth Controller Index
 *
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType Eth_RpcSendDetachReq(uint8 ctrlIdx);

/**
 *  \brief Send default flow registration request to the ethernet firmware
 *
 *  \param ctrlIdx  Eth Controller Index
 *
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType Eth_RpcSendRegisterDefaultReq(uint8 ctrlIdx);

/**
 *  \brief Send default flow unregistration request to the ethernet firmware
 *
 *  \param ctrlIdx  Eth Controller Index
 *
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType Eth_RpcSendUnregisterDefaultReq(uint8 ctrlIdx);

/**
 *  \brief Send destination mac flow registration request to the ethernet firmware
 *
 *  \param ctrlIdx     Eth Controller Index
 *  \param macAddress  Mac Address to be registered to the Eth MCAL driver Rx flow id
 *
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType Eth_RpcSendRegisterMacReq(uint8 ctrlIdx, uint8 *macAddress);

/**
 *  \brief Send destination mac flow unregistration request to the ethernet firmware
 *
 *  \param ctrlIdx     Eth Controller Index
 *  \param macAddress  Mac Address to be unregistered from the Eth MCAL driver Rx flow id
 *
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType Eth_RpcSendUnregisterMacReq(uint8 ctrlIdx, uint8 *macAddress);

/**
 *  \brief Send IPv4 address::mac address registration request to the ethernet firmware
 *
 *  \param ctrlIdx     Eth Controller Index
 *  \param ipv4Address IPv4 address to be added to ARP database
 *  \param macAddress  Mac Address to be associated with  the IPV4 address
 *
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType Eth_RpcSendIPv4RegisterMacReq(uint8 ctrlIdx, uint8 * ipv4Address, uint8 *macAddress);

/**
 *  \brief Send IPv4 address::mac address unregistration request to the ethernet firmware
 *
 *  \param ctrlIdx     Eth Controller Index
 *  \param ipv4Address IPv4 address to be removed from ARP database
 *
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType Eth_RpcSendIPv4UnregisterMacReq(uint8 ctrlIdx, uint8 * ipv4Address);

/**
 *  \brief Send add unicast address request to the ethernet firmware
 *
 *  \param ctrlIdx     Eth Controller Index
 *  \param macAddr     Mac address to be added to the switch address resolution table
 *  \param port        Destination port
 *  \param vlanId      Vlan id associated with the macAddress
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType Eth_RpcSendAddUnicastAddr(uint8 ctrlIdx, uint8 *macAddr, Eth_PortType port, uint32 vlanId);

/**
 *  \brief Send add multicast address to RX filter request to the ethernet firmware
 *
 *  \param ctrlIdx     Eth Controller Index
 *  \param macAddr     Mac address to be added to the switch address resolution table
 *  \param vlanId      Vlan id associated with the macAddress
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType Eth_RpcSendAddMulticastEntry(uint8 ctrlIdx,
                                            const uint8 *macAddr,
                                            uint32 vlanId);

/**
 *  \brief Send delete multicast address to RX filter request to the ethernet firmware
 *
 *  \param ctrlIdx     Eth Controller Index
 *  \param macAddr     Mac address to be deleted from the switch address resolution table
 *  \param vlanId      Vlan id associated with the macAddress
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType Eth_RpcSendDelMulticastEntry(uint8 ctrlIdx,
                                            const uint8 *macAddr,
                                            uint32 vlanId);

/**
 *  \brief Send delete address request to the ethernet firmware
 *
 *  \param ctrlIdx     Eth Controller Index
 *  \param macAddr     Mac address to be deleted from the switch address resolution table
 *  \param vlanId      Vlan id associated with the macAddress
 *  \return         E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType Eth_RpcSendDelAddrEntry(uint8 ctrlIdx, const uint8 *macAddr, uint32 vlanId);

/**
 *  \brief Send add vlan request to the ethernet firmware
 *
 *  \param ctrlIdx     Eth Controller Index
 *  \param vlanId      VLAN Id to be added to switch
 *  \return            E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType Eth_RpcSendAddVlan(uint8 ctrlIdx, uint32 vlanId);

/**
 *  \brief Send delete vlan request to the ethernet firmware
 *
 *  \param ctrlIdx     Eth Controller Index
 *  \param vlanId      VLAN Id to be deleted from switch
 *  \return            E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType Eth_RpcSendDelVlan(uint8 ctrlIdx, uint32 vlanId);

/**
 *  \brief Send reset complete notification to the ethernet firmware
 *
 *  \param ctrlIdx     Eth Controller Index
 */
Std_ReturnType Eth_RpcSendResetComplete(uint8 ctrlIdx);

#ifdef __cplusplus
}
#endif

#endif
