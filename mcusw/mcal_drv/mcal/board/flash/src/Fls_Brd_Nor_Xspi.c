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
 *  \file     Fls_Brd_Nor_Xspi.c
 *
 *  \brief    This file contains FLS MCAL driver internal functions for Board Nor XSPI
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

Std_ReturnType Nor_xspiCmdWrite(SPI_Handle handle, uint8 *cmdBuf,
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

Std_ReturnType Nor_xspiCmdRead(SPI_Handle handle, uint8 *cmdBuf,
                            uint32 cmdLen, uint8 *rxBuf, uint32 rxLen)
{
    SPI_Transaction  transaction_local;
    uint32         transferType = (uint32) SPI_TRANSACTION_TYPE_READ;
    Std_ReturnType  retVal = E_NOT_OK;
    boolean             ret;

     /* Update the mode and transfer type with the required values  */
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

Std_ReturnType Nor_xspiRegRead(SPI_Handle handle,
                                  uint32   regAddr,
                                  uint8    *data)
{
    Std_ReturnType retVal =  E_OK;
    uint8    cmd[6];

    cmd[0] = NOR_CMD_RDREG;
    cmd[1] = (regAddr >> 24) & 0xFF;
    cmd[2] = (regAddr >> 16) & 0xFF;
    cmd[3] = (regAddr >> 8) & 0xFF;
    cmd[4] = (regAddr) & 0xFF;

    retVal = Nor_xspiCmdRead(handle, cmd, 5, data, 1);
    if(retVal != E_OK)
    {
        return E_NOT_OK;
    }

    return retVal;
}

Std_ReturnType Nor_xspiRegWrite(SPI_Handle handle,
                                   uint32   regAddr,
                                   uint8    data)
{
    Std_ReturnType retVal = E_NOT_OK;
    uint8    cmd[6];

    /* Enable flash write */
    cmd[0] = NOR_CMD_WREN;
    retVal = Nor_xspiCmdWrite(handle, cmd, 1, 0);
    if(retVal == E_OK)
    {
        retVal = Nor_xspiWaitReady(handle, NOR_WRR_WRITE_TIMEOUT);
        if (retVal == E_OK)
        {
            cmd[0] = NOR_CMD_WRREG;
            cmd[1] = (regAddr >> 24) & 0xFF;
            cmd[2] = (regAddr >> 16) & 0xFF;
            cmd[3] = (regAddr >> 8) & 0xFF;
            cmd[4] = (regAddr) & 0xFF;
            cmd[5] = data;

            retVal = Nor_xspiCmdWrite(handle, cmd, 5, 1);
            if (retVal == E_OK)
            {
                retVal = Nor_xspiWaitReady(handle, NOR_WRR_WRITE_TIMEOUT);
            }
        }
    }

    return retVal;
}

Std_ReturnType Nor_xspiHybridSectCfg(SPI_Handle handle,
                                        uint8    enable,
                                        uint32   cfgFlag)
{
    Std_ReturnType  retVal = E_OK;
    uint8     regData;

    /* Read configuration register3 */
    retVal = Nor_xspiRegRead(handle, NOR_CFG3_VREG_ADDR, &regData);
    if(retVal != E_OK)
    {
        return E_NOT_OK;
    }

    if(enable == 1)
    {
        /* Enable hybrid sector configuration */
        regData &= ~0x8;
    }
    else
    {
        /* Disable hybrid sector configuration */
        regData |= 0x8;
    }

    /* Write configuration register3 */
    retVal = Nor_xspiRegWrite(handle, NOR_CFG3_VREG_ADDR, regData);
    if(retVal != E_OK)
    {
        return E_NOT_OK;
    }

    return retVal;
}




Std_ReturnType Nor_xspiReadId(SPI_Handle handle)
{
    Std_ReturnType       retVal = E_NOT_OK;
    uint8     idCode[NOR_RDID_NUM_BYTES];
    uint8     cmd[6];
    uint32    manfID, devID;
    cmd[0] = NOR_CMD_RDID;
    cmd[1] = 0;  /* Address Bytes */
    cmd[2] = 0;
    cmd[3] = 0;
    cmd[4] = 0;

    retVal = Nor_xspiCmdRead(handle, cmd, 5, idCode, NOR_RDID_NUM_BYTES);
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


Std_ReturnType Nor_xspiEnableDDR(SPI_Handle handle)
{
    Std_ReturnType       retVal = E_NOT_OK;
    uint8          cmdWren = NOR_CMD_WREN;
    uint32         data[6];

    /* Send Write Enable command */
    retVal = Nor_xspiCmdWrite(handle, &cmdWren, 1U, 0U);

    /* Enable double transfer rate mode */
    if (retVal == E_OK)
    {
        /* send write VCR command to reg addr 0x0 to set to DDR mode */
        data[0] = ((uint32)NOR_CMD_WRITE_VCR << 24U)         | /* write volatile config reg cmd */
                  ((uint32)0U << 23U)                         | /* read data disable */
                  ((uint32)7U << 20U)                         | /* read 8 data bytes */
                  ((uint32)1U << 19U)                         | /* enable cmd adddr */
                  ((uint32)2U << 16U)                         | /* 3 address bytes */
                  ((uint32)1U << 15U);                          /* write data enable */
        data[1] = 0x00800006;     /* Non-volatile config register address */
        data[2] = 0x43; /* set to Octal DDR in Nonvolatile Config Reg 0x0 */
        retVal = OSPI_control(handle, SPI_V0_CMD_ENABLE_DDR, (void *)data);
    }

    return retVal;
}

Std_ReturnType Nor_xspiEnableSDR(SPI_Handle handle)
{
    Std_ReturnType       retVal = E_NOT_OK;
    uint8          cmdWren = NOR_CMD_WREN;
    uint32         data[6];
    CSL_ospi_flash_cfgRegs *regAddr; 
    uint32               regVal;

    /* Send Write Enable command */
    retVal = Nor_xspiCmdWrite(handle, &cmdWren, 1U, 0U);

    /* Enable single transfer rate mode */
    if (retVal == E_OK)
    {
        /* send write VCR command to reg addr 0x0 to set to DDR mode */
        data[0] = ((uint32)NOR_CMD_WRITE_VCR << 24U)         | /* write volatile config reg cmd */
                  ((uint32)0U << 23U)                         | /* read data disable */
                  ((uint32)7U << 20U)                         | /* read 8 data bytes */
                  ((uint32)1U << 19U)                         | /* enable cmd adddr */
                  ((uint32)2U << 16U)                         | /* 3 address bytes */
                  ((uint32)1U << 15U);                          /* write data enable */
        data[1] = 0x00800006;     /* Non-volatile config register address */
        data[2] = 0x41U; /* set to Extended SPI mode in Nonvolatile Config Reg 0x0 */
        retVal = OSPI_control(handle, SPI_V0_CMD_ENABLE_SDR, (void *)data);

        /* Flash device requires 4-bit access for command as well in quad mode */
        regAddr = (CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR); 
        regVal = LLD_REG32_RD(&regAddr->DEV_INSTR_RD_CONFIG_REG);
        regVal |= 0x300;
        LLD_REG32_WR(&regAddr->DEV_INSTR_RD_CONFIG_REG, regVal);

    }
    return retVal;
}

/*
 * Design : MCAL-15384
 * Fls_norAsyncErase Function calls this
 * removed polling in Nor_xspiWaitReady
 */
Fls_InternalStateType Nor_xspiAsyncWaitReady(SPI_Handle handle, uint32 timeOut)
{
    uint8         status;
    uint8         cmd[6];
    Fls_InternalStateType retValAsync = FLS_INTERNAL_JOB_WAIT;
    Std_ReturnType       retRead = E_OK;

    cmd[0] = NOR_CMD_RDREG;
    cmd[1] = 0;
    cmd[2] = NOR_VREG_OFFSET;
    cmd[3] = 0;
    cmd[4] = 0;


    retRead = Nor_xspiCmdRead(handle, cmd, 5U, &status, 1U);
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

Std_ReturnType Nor_xspiWaitReady(SPI_Handle handle, uint32 timeOut)
{
    uint8         status;
    uint8         cmd[6];
    Std_ReturnType       retVal = E_OK;
    Std_ReturnType       ready = E_NOT_OK;
    uint32 timeOutVal = timeOut;

    cmd[0] = NOR_CMD_RDREG;
    cmd[1] = 0;
    cmd[2] = NOR_VREG_OFFSET;
    cmd[3] = 0;
    cmd[4] = 0;

    do
    {
        if (E_OK == Nor_xspiCmdRead(handle, cmd, 5U, &status, 1U))
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

Std_ReturnType Nor_xspiSetDummyCycle(SPI_Handle handle, uint32 dummyCycle)
{
    Std_ReturnType         retVal;
    uint8                 regData;

    /* Read configuration register2 */
    retVal = Nor_xspiRegRead(handle, NOR_CFG2_VREG_ADDR, &regData);
    if (retVal == E_OK)
    {
        /* Set the dummy cycles */
        regData = (regData & ~0xF) | dummyCycle;

        /* Write configuration register2 */
        retVal = Nor_xspiRegWrite(handle, NOR_CFG2_VREG_ADDR, regData);
    }

    return retVal;
}

Std_ReturnType Nor_xspiSetOpcode(SPI_Handle handle)
{
    uint32               data[6];
    uint32               rdDummyCycles = 0;
    uint32               cmdDummyCycles = 0;
    uint32               readCmd;
    uint32               progCmd;
    uint32               latencyCode = 0;
    uint32               rx_lines;
    Std_ReturnType       retVal = E_OK;
    rx_lines = (uint32)OSPI_XFER_LINES_OCTAL;

    if (Fls_DrvObj.dacEnable == TRUE)
    {
        rdDummyCycles = NOR_OCTAL_READ_DUMMY_CYCLE;
        latencyCode   = NOR_OCTAL_READ_DUMMY_CYCLE_LC;
    }
    else
    {
        rdDummyCycles = NOR_OCTAL_READ_DUMMY_CYCLE_INDAC;
        latencyCode   = NOR_OCTAL_READ_DUMMY_CYCLE_LC_INDAC;
    }

    if (gDtrEnable == TRUE)
    {
        cmdDummyCycles = NOR_OCTAL_DDR_CMD_READ_DUMMY_CYCLE;
        readCmd = NOR_CMD_OCTAL_DDR_READ;
        if (gPhyEnable == FALSE)
        {
            rdDummyCycles += 1;
        }
    }
    else
    {
        cmdDummyCycles = NOR_OCTAL_SDR_CMD_READ_DUMMY_CYCLE;
        readCmd = NOR_CMD_OCTAL_READ;
    }

    progCmd = NOR_CMD_OCTAL_PROG;

    data[0] = readCmd;
    data[1] = progCmd;
    data[2] = NOR_CMD_RDSR;

    /* Update the read opCode, rx lines and read dummy cycles */
    if (OSPI_control(handle, SPI_V0_CMD_RD_DUMMY_CLKS, (void *)&rdDummyCycles) != E_OK)
    {
        retVal = E_NOT_OK;
    }
    if (E_OK == retVal)
    {
        if (OSPI_control(handle, SPI_V0_CMD_SET_XFER_LINES, (void *)&rx_lines) != E_OK)
        {
            retVal = E_NOT_OK;
        }
    }
    if (E_OK == retVal)
    {
        if (OSPI_control(handle, SPI_V0_CMD_XFER_OPCODE, (void *)data) != E_OK)
        {
            retVal = E_NOT_OK;
        }
    }
    if (E_OK == retVal)
    {
        /* Set the opcodes for dual opcode mode */
        data[0] = 0;
        data[1] = 0xFA;
        data[2] = readCmd;
        data[3] = progCmd;
        data[4] = 0xF9;
        data[5] = 0x6;
        if (OSPI_control(handle, SPI_V0_CMD_XFER_OPCODE_EXT, (void *)data) != E_OK)
        {
            retVal = E_NOT_OK;
        }
        if (E_OK == retVal)
        {
            if (OSPI_control(handle, SPI_V0_CMD_EXT_RD_DUMMY_CLKS, (void *)&cmdDummyCycles) != E_OK)
            {
                retVal = E_NOT_OK;
            }
        }
        if (E_OK == retVal)
        {
            /* Set read dummy cycles to the flash device */
            retVal = Nor_xspiSetDummyCycle(handle, latencyCode);
        }
    }

    return retVal;
}


#define FLS_STOP_SEC_CODE
#include "Fls_MemMap.h"
