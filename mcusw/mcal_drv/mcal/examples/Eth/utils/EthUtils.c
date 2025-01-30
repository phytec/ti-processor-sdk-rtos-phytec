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
 *  \file     EthUtils.c
 *
 *  \brief    This file contains the implementation of miscellaneous Ethernet
 *            test utilities.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "EthUtils_Patterns.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* \brief Octets of payload per console row */
#define OCTETS_PER_ROW                  16

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                  Internal Function Definitions                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int32_t EthUtils_checkPayload(DataFramePayload *payload)
{
    uint16_t len = ntohs(payload->len);
    uint8_t *ref = NULL;
    int32_t status = ETH_TEST_PKT_SOK;

    /* Verify the test packet type id */
    switch (payload->type)
    {
    case ETH_TEST_TYPE_PATTERN_1:
        ref = EthUtils_DataPattern1;
        break;
    case ETH_TEST_TYPE_PATTERN_2:
        ref = EthUtils_DataPattern2;
        break;
    case ETH_TEST_TYPE_PATTERN_3:
        ref = EthUtils_DataPattern3;
        break;
    case ETH_TEST_TYPE_PATTERN_4:
        ref = EthUtils_DataPattern4;
        break;
    default:
        status = ETH_TEST_PKT_ETYPE;
        break;
    }

    /* Check that pattern buffer has enough data to be validated */
    if (ETH_TEST_PKT_SOK == status)
    {
        if (len > (ETH_PAYLOAD_LEN - ETH_TEST_DATA_HDR_LEN))
        {
            status = ETH_TEST_PKT_ESIZE;
        }
    }

    /* Verify packet content */
    if (ETH_TEST_PKT_SOK == status)
    {
        if (0 != memcmp(payload->data, ref, len))
        {
            status = ETH_TEST_PKT_ECONTENT;
        }
    }

    return status;
}

int32_t EthUtils_checkVlanPayload(VlanDataFramePayload *payload)
{
    return EthUtils_checkPayload(&payload->payload);
}

int32_t EthUtils_checkVlanTagAndPayload(VlanDataFramePayload *payload,
                                        uint8_t pcp,
                                        uint16_t vid,
                                        uint16_t etherType)
{
    uint16_t tci = ntohs(payload->tci);
    int32_t status = ETH_TEST_PKT_SOK;

    /* Check packet's VID and EtherType */
    if ((vid != (tci & 0xFFF)) ||
        (etherType != ntohs(payload->etherType)))
    {
        status = ETH_TEST_PKT_ECONTENT;
    }

    if (ETH_TEST_PKT_SOK == status)
    {
        status = EthUtils_checkPayload(&payload->payload);
    }

    return status;
}

int32_t EthUtils_fillPayload(DataFramePayload *payload,
                             uint16_t type,
                             uint16_t len)
{
    uint8_t *ref = NULL;
    int32_t status = ETH_TEST_PKT_SOK;

    /* Check that requested type id */
    switch (type)
    {
    case ETH_TEST_TYPE_PATTERN_1:
        ref = EthUtils_DataPattern1;
        break;
    case ETH_TEST_TYPE_PATTERN_2:
        ref = EthUtils_DataPattern2;
        break;
    case ETH_TEST_TYPE_PATTERN_3:
        ref = EthUtils_DataPattern3;
        break;
    case ETH_TEST_TYPE_PATTERN_4:
        ref = EthUtils_DataPattern4;
        break;
    default:
        status = ETH_TEST_PKT_ETYPE;
        break;
    }

    /* Check that pattern buffer has enough data and copy packet */
    if (ETH_TEST_PKT_SOK == status)
    {
        if ((len >= ETH_TEST_DATA_HDR_LEN) &&
            (len <= ETH_PAYLOAD_LEN))
        {
            len -= ETH_TEST_DATA_HDR_LEN;
            payload->type = type;
            payload->len = htons(len);
            memcpy(payload->data, ref, len);
        }
        else
        {
            status = ETH_TEST_PKT_ESIZE;
        }
    }

    return status;
}

int32_t EthUtils_fillVlanPayload(VlanDataFramePayload *payload,
                                 uint16_t type,
                                 uint16_t len,
                                 uint8_t pcp,
                                 uint16_t vid,
                                 uint16_t etherType)
{
    int32_t status = ETH_TEST_PKT_SOK;

    payload->tci = htons((pcp << 13) | vid);
    payload->etherType = htons(etherType);

    if (len < (ETH_TEST_DATA_HDR_LEN + ETH_VLAN_TAG_LEN))
    {
        status = ETH_TEST_PKT_ESIZE;
    }

    if (ETH_TEST_PKT_SOK == status)
    {
        len -= ETH_VLAN_TAG_LEN;
        status = EthUtils_fillPayload(&payload->payload, type, len);
    }

    return status;
}

int32_t EthUtils_fillVlanHdr(VlanDataFramePayload *payload,
                                 uint16_t type,
                                 uint16_t len,
                                 uint8_t pcp,
                                 uint16_t vid,
                                 uint16_t etherType)
{
    int32_t status = ETH_TEST_PKT_SOK;

    payload->tci = htons((pcp << 13) | vid);
    payload->etherType = htons(etherType);

    len -= ETH_VLAN_TAG_LEN;
    len -= ETH_TEST_DATA_HDR_LEN;
    payload->payload.type = type;
    payload->payload.len = htons(len);

    return status;
}

void EthUtils_printFrame(EthFrame *frame,
                         int len)
{
    uint8_t *payload;
    int i;

    EthUtils_printf("Dst addr : %02x:%02x:%02x:%02x:%02x:%02x\n",
                    frame->hdr.dstMac[0] & 0xFF,
                    frame->hdr.dstMac[1] & 0xFF,
                    frame->hdr.dstMac[2] & 0xFF,
                    frame->hdr.dstMac[3] & 0xFF,
                    frame->hdr.dstMac[4] & 0xFF,
                    frame->hdr.dstMac[5] & 0xFF);

    EthUtils_printf("Src addr : %02x:%02x:%02x:%02x:%02x:%02x\n",
                    frame->hdr.srcMac[0] & 0xFF,
                    frame->hdr.srcMac[1] & 0xFF,
                    frame->hdr.srcMac[2] & 0xFF,
                    frame->hdr.srcMac[3] & 0xFF,
                    frame->hdr.srcMac[4] & 0xFF,
                    frame->hdr.srcMac[5] & 0xFF);

    if (frame->hdr.etherType == htons(ETHERTYPE_VLAN_TAG))
    {
        EthVlanFrame *vlanFrame = (EthVlanFrame *)frame;

        EthUtils_printf("TPID     : 0x%04x\n", ntohs(vlanFrame->hdr.tpid) & 0xFFFFU);
        EthUtils_printf("Priority : %d\n", (ntohs(vlanFrame->hdr.tci) & 0xFFFFU) >> 13);
        EthUtils_printf("VLAN Id  : %d\n", ntohs(vlanFrame->hdr.tci) & 0xFFFU);
        EthUtils_printf("EtherType: 0x%04x\n", ntohs(vlanFrame->hdr.etherType) & 0xFFFFU);
        payload = vlanFrame->payload;
        len -= ETH_VLAN_TAG_LEN;
    }
    else
    {
        EthUtils_printf("EtherType: 0x%04x\n", ntohs(frame->hdr.etherType) & 0xFFFFU);
        payload = frame->payload;
    }

    EthUtils_printf("Payload  : ");
    for (i = 0; i < len; i++)
    {
        EthUtils_printf("0x%02x ", payload[i]);
        if (i && (((i+1) % OCTETS_PER_ROW) == 0))
        {
            EthUtils_printf("\n           ");
        }
    }

    if (len && ((len % OCTETS_PER_ROW) != 0))
    {
        EthUtils_printf("\n");
    }

    EthUtils_printf("\n");
}
