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

/**
 *  \file     FlsApp_Startup.c
 *
 *  \brief    This file contains interrupt and clock configuration.
 */
/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "FlsApp_Startup.h"

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

Int32 Fls_OSPI_configClk(uint64 clk_input)
{
    Int32 retVal = LLD_PASS;
    uint64 ospi_rclk_freq;
    uint32 parClk;
    uint32 ospi_clk_id = TISCI_DEV_MCU_FSS0_OSPI_0_OSPI_RCLK_CLK;
    uint32 dev_id = TISCI_DEV_MCU_FSS0_OSPI_0;

    /* Get the default SPI init configurations */
    if (LLD_PASS ==
        Sciclient_pmModuleClkRequest(dev_id,ospi_clk_id,
                                    TISCI_MSG_VALUE_CLOCK_SW_STATE_REQ,
                                    TISCI_MSG_FLAG_AOP,
                                    SCICLIENT_SERVICE_WAIT_FOREVER))
    {
        /* Max clocks */
        if(clk_input == 166666666U)
        {
            parClk = TISCI_DEV_MCU_FSS0_OSPI_0_OSPI_RCLK_CLK_PARENT_HSDIV4_16FFT_MCU_2_HSDIVOUT4_CLK;
        }
        else
        {
            parClk = TISCI_DEV_MCU_FSS0_OSPI_0_OSPI_RCLK_CLK_PARENT_HSDIV4_16FFT_MCU_1_HSDIVOUT4_CLK;
        }

        if (LLD_PASS ==
            Sciclient_pmSetModuleClkParent(dev_id, ospi_clk_id,
                                           parClk,
                                           SCICLIENT_SERVICE_WAIT_FOREVER))
        {
            ospi_rclk_freq = clk_input;
            if (LLD_PASS ==
                Sciclient_pmSetModuleClkFreq(dev_id, ospi_clk_id,
                                             ospi_rclk_freq,
                                             TISCI_MSG_FLAG_AOP,
                                             SCICLIENT_SERVICE_WAIT_FOREVER))
            {
                ospi_rclk_freq=0;
                retVal=Sciclient_pmGetModuleClkFreq(dev_id, ospi_clk_id,
                                                    &ospi_rclk_freq,
                                                    SCICLIENT_SERVICE_WAIT_FOREVER);
            }
            else
            {
                retVal = LLD_EFAIL;
            }
        }
        else
        {
            retVal = LLD_EFAIL;
        }
    }
    else
    {
        retVal = LLD_EFAIL;
    }
    return retVal;
}
