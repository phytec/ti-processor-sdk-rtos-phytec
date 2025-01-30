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
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIESFls_Brd_Nor
*
*/
/**
 *  \file     Fls_Brd_Nor_Ospi.c
 *
 *  \brief    This file contains FLS MCAL driver internal functions for Board Nor OSPI
 *
 */


/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "Fls_Brd_Nor.h"
/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

#define FLS_START_SEC_CODE
#include "Fls_MemMap.h"

Std_ReturnType Nor_qspiCmdWrite(SPI_Handle handle, uint8 *cmdBuf,
                                        uint32 cmdLen, uint32 dataLen)
{
    SPI_Transaction  transaction_local;
    uint32         transferType = (uint32) SPI_TRANSACTION_TYPE_WRITE;
    Std_ReturnType  retVal = E_NOT_OK;
    boolean             ret;

    /* Update the mode and transfer type with the required values */
    retVal = OSPI_control(handle, SPI_V0_CMD_SET_CFG_MODE, NULL);
    if (retVal == E_OK)
    {
        retVal = OSPI_control(handle, SPI_V0_CMD_XFER_MODE_RW, (void *)&transferType);
    }

    if (retVal == E_OK)
    {
        transaction_local.txBuf = (void *)cmdBuf;
        /* Buffer includes command and write data */
        transaction_local.count = cmdLen + dataLen;
        transaction_local.rxBuf = NULL;
        transaction_local.arg = (void *)(uint32)dataLen;

        ret = OSPI_transfer(handle, &transaction_local);
        if (ret == TRUE)
        {
            retVal = E_OK;
        }
        else
        {
            retVal = E_NOT_OK;
        }
    }
    return retVal;
}

Std_ReturnType NOR_qspiCmdRead(SPI_Handle handle, uint8 *cmdBuf,
                            uint32 cmdLen, uint8 *rxBuf, uint32 rxLen)
{
    SPI_Transaction  transaction_local;
    uint32         transferType = (uint32) SPI_TRANSACTION_TYPE_READ;
    Std_ReturnType  retVal = E_NOT_OK;
    boolean             ret;

    /* Update the mode and transfer type with the required values */
    retVal = OSPI_control(handle, SPI_V0_CMD_SET_CFG_MODE, NULL);
    if (retVal == E_OK)
    {
        retVal = OSPI_control(handle, SPI_V0_CMD_XFER_MODE_RW, (void *)&transferType);
    }

    if (retVal == E_OK)
    {
        transaction_local.txBuf = (void *)cmdBuf;
        transaction_local.rxBuf = (void *)rxBuf;
        transaction_local.count = cmdLen + rxLen;

        ret = OSPI_transfer(handle, &transaction_local);
        if (ret == TRUE)
        {
            retVal = E_OK;
        }
        else
        {
            retVal = E_NOT_OK;
        }
    }
    return retVal;
}

Std_ReturnType Nor_qspiReadId(SPI_Handle handle)
{
    Std_ReturnType       retVal = E_NOT_OK;
    uint8     idCode[NOR_RDID_NUM_BYTES];
    uint8     cmd = NOR_CMD_RDID;
    uint32    manfID, devID;

    retVal = NOR_qspiCmdRead(handle, &cmd, 1, idCode, NOR_RDID_NUM_BYTES);
    if (retVal == E_OK)
    {
        manfID = (uint32)idCode[0];
        devID = ((uint32)idCode[1] << 8) | ((uint32)idCode[2]);
        if ((manfID != NOR_MANF_ID) || (devID != NOR_DEVICE_ID))
        {
            retVal = E_NOT_OK;
        }
    }
    return (retVal);
}

Std_ReturnType Nor_qspiEnableDDR(SPI_Handle handle)
{
    Std_ReturnType       retVal = E_NOT_OK;
    uint8          cmdWren = NOR_CMD_WREN;
    uint8_t          data[3];
    uint32_t         opCode[3];
    uint32_t         dummyCycles;
    uint32_t         rx_lines;

#if defined (SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J7200) || defined(SOC_J784S4)
    /* Send Write Enable command */
    if (Nor_qspiCmdWrite(handle, &cmdWren, 1, 0))
    {
    	return E_NOT_OK;
    }

    if (Nor_qspiWaitReady(handle, NOR_WRR_WRITE_TIMEOUT))
    {
    	return E_NOT_OK;
    }

    /* Write Enhanced VCR register to enable DDR mode */
    data[0] = NOR_CMD_WRITE_ENVCR;
    data[1] = 0x5F;  /* Enable quad mode and DTR mode */
    retVal = Nor_qspiCmdWrite(handle, data, 1, 1);
    if (retVal == E_OK)
    {
        /* Set opcodes */
        dummyCycles = NOR_QUAD_READ_DUMMY_CYCLE - 2;
        rx_lines    = OSPI_XFER_LINES_QUAD;
        opCode[0]   = NOR_CMD_QUAD_DDR_O_FAST_RD;
        opCode[1]   = NOR_CMD_QUAD_FAST_PROG;
        opCode[2]   = NOR_CMD_RDSR;

        /* Update the read opCode, rx lines and read dummy cycles */
        OSPI_control(handle, SPI_V0_CMD_RD_DUMMY_CLKS, (void *)&dummyCycles);
        OSPI_control(handle, SPI_V0_CMD_SET_XFER_LINES, (void *)&rx_lines);
        OSPI_control(handle, SPI_V0_CMD_XFER_OPCODE, (void *)opCode);
    }
#else
    /* Send Write Enable command */
    if (Nor_qspiCmdWrite(handle, &cmdWren, 1, 0))
    {
        return E_NOT_OK;
    }

    /* Write CR1 register to enable QSPI mode */
    data[0] = NOR_CMD_WRR;
    data[1] = 0x02;
    retVal = Nor_qspiCmdWrite(handle, data, 1, 1);
    if (retVal == E_OK)
    {
        if (Nor_qspiWaitReady(handle, NOR_WRR_WRITE_TIMEOUT))
        {
            return E_NOT_OK;
        }

        /* Set opcodes */
        dummyCycles = NOR_QUAD_READ_DUMMY_CYCLE;
        rx_lines    = OSPI_XFER_LINES_QUAD;
        opCode[0]   = NOR_CMD_QUAD_DDR_IO_READ;
        opCode[1]   = NOR_CMD_QUAD_PAGE_PROG;
        opCode[2]   = NOR_CMD_RDSR;

        /* Update the read opCode, rx lines and read dummy cycles */
        OSPI_control(handle, SPI_V0_CMD_RD_DUMMY_CLKS, (void *)&dummyCycles);
        OSPI_control(handle, SPI_V0_CMD_SET_XFER_LINES, (void *)&rx_lines);
        OSPI_control(handle, SPI_V0_CMD_XFER_OPCODE, (void *)opCode);
    }
#endif
    CSL_ospiDtrEnable((const CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), TRUE);

    return retVal;
}

Std_ReturnType Nor_qspiEnableSDR(SPI_Handle handle)
{
    Std_ReturnType       retVal = E_NOT_OK;
    uint8          cmdWren = NOR_CMD_WREN;
    uint8         data[3];
    CSL_ospi_flash_cfgRegs *regAddr;
    uint32_t         opCode[3];
    uint32_t         dummyCycles;
    uint32_t         rx_lines;
    uint32_t         regVal;

    // #if defined (j721e_evm) || defined (j7200_evm) || defined (j721s2_evm) || defined(j784s4_evm)
    #if defined (SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J7200) || defined(SOC_J784S4)
    /* Send Write Enable command */
    if (Nor_qspiCmdWrite(handle, &cmdWren, 1, 0))
    {
    	return E_NOT_OK;
    }

    /* Write Enhanced VCR register to enable quad mode */
    data[0] = NOR_CMD_WRITE_ENVCR;
    data[1] = 0x7F;  /* Enable quad mode */
    retVal = Nor_qspiCmdWrite(handle, data, 1, 1);
    if (retVal == E_OK)
    {
        dummyCycles = NOR_QUAD_READ_DUMMY_CYCLE;
        rx_lines    = OSPI_XFER_LINES_QUAD;
        opCode[0]     = NOR_CMD_QUAD_IO_FAST_RD;
        opCode[1]     = NOR_CMD_QUAD_FAST_PROG;
        opCode[2]     = NOR_CMD_RDSR;

        OSPI_control(handle, SPI_V0_CMD_RD_DUMMY_CLKS, (void *)&dummyCycles);
        OSPI_control(handle, SPI_V0_CMD_SET_XFER_LINES, (void *)&rx_lines);
        OSPI_control(handle, SPI_V0_CMD_XFER_OPCODE, (void *)opCode);
    }
#else
    /* Send Write Enable command */
    if (Nor_qspiCmdWrite(handle, &cmdWren, 1, 0))
    {
        return E_NOT_OK;
    }

    /* Write CR1 register to enable QSPI mode */
    data[0] = NOR_CMD_WRR;
    data[1] = 0x02;
    retVal = Nor_qspiCmdWrite(handle, data, 1, 1);
    if (retVal == E_OK)
    {
        rx_lines      = OSPI_XFER_LINES_QUAD;
        opCode[0]     = NOR_CMD_QUAD_READ;
        opCode[1]     = NOR_CMD_PAGE_PROG;
        opCode[2]     = NOR_CMD_RDSR;

        OSPI_control(handle, SPI_V0_CMD_RD_DUMMY_CLKS, (void *)&dummyCycles);
        OSPI_control(handle, SPI_V0_CMD_SET_XFER_LINES, (void *)&rx_lines);
        OSPI_control(handle, SPI_V0_CMD_XFER_OPCODE, (void *)opCode);
    }
#endif
    /* Flash device requires 4-bit access for command as well in quad mode */
    regAddr = (CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR);
    regVal = LLD_REG32_RD(&regAddr->DEV_INSTR_RD_CONFIG_REG);
    regVal |= 0x200;
    LLD_REG32_WR(&regAddr->DEV_INSTR_RD_CONFIG_REG, regVal);

    return retVal;
}

Std_ReturnType Nor_qspiXipEnable(SPI_Handle handle, uint8 modeBit)
{
    return E_OK;
}

/*
 * Design : MCAL-15384
 * Fls_norAsyncErase Function calls this
 * removed polling in Nor_qspiWaitReady
 */
Fls_InternalStateType Nor_qspiAsyncWaitReady(SPI_Handle handle, uint32 timeOut)
{
    uint8         status;
    uint8         cmd = NOR_CMD_RDSR;
    Std_ReturnType       retRead = E_OK;
    Fls_InternalStateType retValAsync = FLS_INTERNAL_JOB_WAIT;

    retRead = NOR_qspiCmdRead(handle, &cmd, 1U, &status, 1U);
    if ((retRead == E_OK) && ((status & NOR_SR_WIP) == 0U))
    {
            retValAsync = FLS_INTERNAL_JOB_DONE;
    }
    else
    {
        Fls_DrvObj.timeOutVal--;
        if (Fls_DrvObj.timeOutVal == 0U)
        {
            retValAsync = FLS_INTERNAL_JOB_FAIL;
        }
    }

    return retValAsync;
}

Std_ReturnType Nor_qspiWaitReady(SPI_Handle handle, uint32 timeOut)
{
    uint8         status;
    uint8         cmd = NOR_CMD_RDSR;
    Std_ReturnType       retVal = E_OK;
    Std_ReturnType       ready = E_NOT_OK;
    uint32 timeOutVal = timeOut;

    do
    {
        if (E_OK == NOR_qspiCmdRead(handle, &cmd, 1U, &status, 1U))
        {
            if ((status & NOR_SR_WIP) == 0U)
            {
                ready = E_OK;
            }
            if (E_OK == ready)
            {
                break;
            }
            else
            {
                timeOutVal--;
                if (timeOutVal == 0U)
                {
                    retVal = E_NOT_OK;
                }
            }
        }
        else
        {
            retVal = E_NOT_OK;
        }
    } while (retVal == E_OK);

    /*Timed out*/
    if (ready == E_OK)
    {
        retVal = E_OK;
    }
    return retVal;
}


#define FLS_STOP_SEC_CODE
#include "Fls_MemMap.h"
