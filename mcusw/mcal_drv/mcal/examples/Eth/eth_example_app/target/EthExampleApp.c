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
 *  \file     EthApp.c
 *
 *  \brief    This file contains the target-side Eth test application code.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <string.h>

/* AutoSAR specific Header files */
#include "Eth.h"
#include "Eth_Irq.h"
#include "EthTrcv.h"

#include "Det.h"
#include "Dem.h"
#include "Os.h"

#include "EthUtils.h"
#include "EthUtils_Target.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define ETHETH_EXAMPLE_APP_TEST_PACKET_CNT (100u)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* ========================================================================== */
/*                          External Functions                                */
/* ========================================================================== */
#ifdef __cplusplus
extern "C"
{
#endif

extern int EthApp_Main(void);

#ifdef __cplusplus
}
#endif

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */
static boolean EthApp_testMain(void);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* Eth application state */
EthApp_StateSet gEthApp;
uint8 gCtrlIdx = ETH_CTRL_ID_0;

/* ========================================================================== */
/*                           External Variables                               */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
int EthApp_Main(void)
{
    uint32 ci;
    boolean status;
    boolean globalStatus = TRUE;
    Std_ReturnType retVal;
    uint32 numTestPacket = 0u;
#if (STD_OFF == ETH_PRE_COMPILE_VARIANT)
    Eth_CfgPtr = (const Eth_ConfigType *)&EthConfigSet_0;
#endif

    /* Initialize the global app structure */
    memset(&gEthApp, 0x0u, sizeof(gEthApp));

    for(ci = 0u; ci < ETH_CTRL_ID_MAX; ++ci)
    {
        gEthApp.state[ci].ctrlIdx = ci;
    }

    /* Print date and time */
    EthUtils_printf("---\r\nEth Application build date/time: %s, %s\r\n", __DATE__, __TIME__);

    EthUtils_printf("Eth Application Starts\r\n");

    /* System initialization */
    EthApp_Startup();

    retVal = EthApp_initDma();
    if (E_OK != retVal)
    {
        EthUtils_printf("Failed to initialize DMA: %d\r\n", retVal);
        return retVal;
    }

#if (STD_OFF == ETH_PRE_COMPILE_VARIANT)
    Eth_CfgPtr = (Eth_ConfigType *)NULL_PTR;
#endif

    /* Initialize the Eth driver and controller */
    retVal = EthApp_initDriver();
    if (E_OK != retVal)
    {
        EthUtils_printf("Failed to initialize: %d\r\n", retVal);
        return retVal;
    }

    /* Setup the Eth driver and controller */
    retVal = EthApp_initAll();
    if (E_OK != retVal)
    {
        EthUtils_printf("Failed to setup driver: %d\r\n", retVal);
        return retVal;
    }

    while (TRUE)
    {
        status = EthApp_testMain();
        if(PASS == status)
        {
            numTestPacket++;
            if(ETHETH_EXAMPLE_APP_TEST_PACKET_CNT == numTestPacket)
            {
                break;
            }
        }
        EthApp_delay(1000u);
    }

    /* Show statistics */
    if (E_OK == retVal)
    {
        retVal = EthApp_showStatsAll();
        if (E_OK != retVal)
        {
            EthUtils_printf("main: failed to show statistics\r\n");
        }
    }

    /* Deinitialize the Ethernet controller */
    if (E_OK == retVal)
    {
        retVal = EthApp_deinitAll();
        if (E_OK != retVal)
        {
            EthUtils_printf("Failed to deinitialize: %d\r\n", retVal);
        }
    }


    retVal = EthApp_closeDma();
    if (E_OK != retVal)
    {
        EthUtils_printf("Failed to deinitialize DMA: %d\r\n", retVal);
    }


    EthUtils_printf("ETH Stack Usage: %d bytes\r\n", AppUtils_GetStackUsage());
    if (AppUtils_EthCheckStackAndSectionCorruption() != E_OK)
    {
        retVal = E_NOT_OK;
        EthUtils_printf("ETH Stack/section corruption!!!\r\n");
    }

    EthUtils_printf("Final Tests Results: %s\r\n",
                    (globalStatus == TRUE) ? "PASS" : "FAIL");

    if (E_OK == retVal)
    {
        EthUtils_printf("Eth Application Completed\r\n");
    }
    else
    {
        EthUtils_printf("Eth Application Failed\r\n");
    }

    return retVal;
}

static boolean EthApp_testMain(void)
{
    boolean status = FAIL;
    uint16 len = 100u;
    EthFrame frame;

    gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt = 0u;
    gEthApp.state[gCtrlIdx].stats.rxPayloadErr = 0u;
    gEthApp.state[gCtrlIdx].stats.rxPayloadErr = 0u;

    memcpy(frame.hdr.dstMac, BcastAddr, ETH_MAC_ADDR_LEN);
    memcpy(frame.hdr.srcMac, gEthApp.state[gCtrlIdx].macAddr, ETH_MAC_ADDR_LEN);
    frame.hdr.etherType = ETHERTYPE_EXPERIMENTAL1;

    (void)EthUtils_fillPayload((DataFramePayload *)frame.payload,
                                          0u,
                                          len);

    /* Send the frame */
    EthApp_send(gEthApp.state[gCtrlIdx].ctrlIdx, &frame, len, ETH_DEFAULT_TX_PRIORITY);

    /* Wait echo frame */
    while(gEthApp.state[gCtrlIdx].stats.rxExp1PktCnt == 0u)
    {
        if(Eth_GetRxEnableInterrupt(gCtrlIdx) == FALSE)
        {
            EthApp_receiveAllFifo(gEthApp.state[gCtrlIdx].ctrlIdx);
        }
    }

    if(0u != gEthApp.state[gCtrlIdx].stats.rxPayloadErr)
    {
        EthUtils_printf("Received echo frame NOK\r\n");
    }
    else
    {
        status = PASS;
        EthUtils_printf("Received echo frame OK\r\n");
    }

    return status;
}

#if (STD_ON == ETH_VIRTUALMAC_SUPPORT)
void EthVirtmacApp_rpcCmdComplete (uint8 controllerIdx,
                                   uint8 sid,
                                   sint32 status)
{
    /* nothing */
}

void EthVirtmacApp_rpcFwRegistered (uint8 controllerIdx)
{
    /* nothing */
}
#endif