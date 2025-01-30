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

Std_ReturnType Nor_ospiCmdWrite(SPI_Handle handle, uint8 *cmdBuf,
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

Std_ReturnType NOR_ospiCmdRead(SPI_Handle handle, uint8 *cmdBuf,
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

Std_ReturnType Nor_ospiReadId(SPI_Handle handle)
{
    Std_ReturnType       retVal = E_NOT_OK;
    uint8     idCode[NOR_RDID_NUM_BYTES];
    uint8     cmd = NOR_CMD_RDID;
    uint32    manfID, devID;

    retVal = NOR_ospiCmdRead(handle, &cmd, 1, idCode, NOR_RDID_NUM_BYTES);
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


Std_ReturnType Nor_ospiEnableDDR(SPI_Handle handle)
{
    Std_ReturnType       retVal = E_NOT_OK;
    uint8          cmdWren = NOR_CMD_WREN;
    uint32         data[3];

    /* Send Write Enable command */
    retVal = Nor_ospiCmdWrite(handle, &cmdWren, 1U, 0U);

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
        data[1] = 0U;     /* Non-volatile config register address */
        data[2] = 0xE7U; /* set to Octal DDR in Nonvolatile Config Reg 0x0 */
        retVal = OSPI_control(handle, SPI_V0_CMD_ENABLE_DDR, (void *)data);
    }
    return retVal;
}

Std_ReturnType Nor_ospiEnableSDR(SPI_Handle handle)
{
    Std_ReturnType       retVal = E_NOT_OK;
    uint8          cmdWren = NOR_CMD_WREN;
    uint32         data[3];

    /* Send Write Enable command */
    retVal = Nor_ospiCmdWrite(handle, &cmdWren, 1U, 0U);

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
        data[1] = 0;     /* Non-volatile config register address */
        data[2] = 0xFFU; /* set to Extended SPI mode in Nonvolatile Config Reg 0x0 */
        retVal = OSPI_control(handle, SPI_V0_CMD_ENABLE_SDR, (void *)data);
    }
    return retVal;
}

Std_ReturnType Nor_ospiXipEnable(SPI_Handle handle, uint8 modeBit)
{
    Std_ReturnType       retVal = E_NOT_OK;
    uint8  cmdWren = NOR_CMD_WREN;
    uint32 value = 0x0;
    uint8  stigCmd[10];
    uint32 data[3];

    /* Send Write Enable command */
    retVal = Nor_ospiCmdWrite(handle, &cmdWren, 1U, 0U);

    if (retVal == E_OK)
    {
        stigCmd[0] = NOR_CMD_WRITE_VCR; /* opcode */
        stigCmd[1] = 0x0U; /* disable read operation */
        stigCmd[2] = 0x7U; /* read 0x7=8 data bytes (ignored) */
        stigCmd[3] = 0x1U; /* enable cmd address */
        stigCmd[4] = modeBit; /* Set the mode bit, 0:Enter XIP, 1: Terminate XIP */
        stigCmd[5] = 0x3U; /* use 0x3=4 address bytes */
        stigCmd[6] = 0x1U; /* enable write operation */
        stigCmd[7] = 0x0U; /* write 0x0=1 data byte */
        stigCmd[8] = 0x0U; /* 0x7=8 dummy cycles */
        stigCmd[9] = 0x0U; /* disable memory bank */

        value |= ((uint32)stigCmd[0] << 24U);
        value |= ((uint32)stigCmd[1] << 23U);
        value |= ((uint32)stigCmd[2] << 20U);
        value |= ((uint32)stigCmd[3] << 19U);
        value |= ((uint32)stigCmd[4] << 18U);
        value |= ((uint32)stigCmd[5] << 16U);
        value |= ((uint32)stigCmd[6] << 15U);
        value |= ((uint32)stigCmd[7] << 12U);
        value |= ((uint32)stigCmd[8] << 7U);
        value |= ((uint32)stigCmd[9] << 2U);

        data[0] = value; /* NVCR cmd */
        data[1] = 0x06U;  /* addr */
        data[2] = 0xFEU;  /* data */

        retVal = OSPI_control(handle, SPI_V0_CMD_CFG_XIP, (void*)data);
    }

    return retVal;
}

/*
 * Design : MCAL-15384
 * Fls_norAsyncErase Function calls this
 * removed polling in Nor_ospiWaitReady
 */
Fls_InternalStateType Nor_ospiAsyncWaitReady(SPI_Handle handle, uint32 timeOut)
{
    uint8         status;
    uint8         cmd = NOR_CMD_RDSR;
    Std_ReturnType       retRead = E_OK;
    Fls_InternalStateType retValAsync = FLS_INTERNAL_JOB_WAIT;

    retRead = NOR_ospiCmdRead(handle, &cmd, 1U, &status, 1U);
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


Std_ReturnType Nor_ospiWaitReady(SPI_Handle handle, uint32 timeOut)
{
    uint8         status;
    uint8         cmd = NOR_CMD_RDSR;
    Std_ReturnType       retVal = E_OK;
    Std_ReturnType       ready = E_NOT_OK;
    uint32 timeOutVal = timeOut;

    do
    {
        if (E_OK == NOR_ospiCmdRead(handle, &cmd, 1U, &status, 1U))
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

Std_ReturnType Nor_ospiSetOpcode(SPI_Handle handle)
{
    uint32               data[3];
    uint32               dummyCycles;
    uint32               rx_lines;
    Std_ReturnType       retVal = E_OK;
    rx_lines = (uint32)OSPI_XFER_LINES_OCTAL;
    dummyCycles = NOR_OCTAL_READ_DUMMY_CYCLE;

    if (Fls_DrvObj.dacEnable == TRUE)
    {
        dummyCycles = NOR_OCTAL_READ_DUMMY_CYCLE;
    }
    else
    {
        dummyCycles = 16U;
    }

    if (gDtrEnable == TRUE)
    {
        data[0]     = NOR_CMD_OCTAL_DDR_O_FAST_RD;
        data[1]     = NOR_CMD_OCTAL_FAST_PROG;
    }
    else
    {
        data[0]     = NOR_CMD_OCTAL_IO_FAST_RD;
        data[1]     = NOR_CMD_EXT_OCTAL_FAST_PROG;
    }
    data[2]     = NOR_CMD_RDSR;

    /* Update the read opCode, rx lines and read dummy cycles */
    if (OSPI_control(handle, SPI_V0_CMD_RD_DUMMY_CLKS, (void *)&dummyCycles) != E_OK)
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
        /* Set read dummy cycles to the flash device */
        retVal = Nor_ospiSetDummyCycle(handle, dummyCycles);
    }
    return retVal;
}

Std_ReturnType Nor_ospiSetDummyCycle(SPI_Handle handle, uint32 dummyCycle)
{
    Std_ReturnType         retVal;
    uint8                cmdWren = NOR_CMD_WREN;
    uint32               data[3];
    uint32               addrBytes;

    if (Fls_DrvObj.dtrEnable == TRUE)
    {
        addrBytes = 3U;
    }
    else
    {
        addrBytes = 2U;
    }

    /* Send Write Enable command */
    retVal = Nor_ospiCmdWrite(handle, &cmdWren, 1, 0);

    /* Enable single transfer rate mode */
    if (retVal == E_OK)
    {
        /* send write VCR command to reg addr 0x0 to set to SDR mode */
        data[0] = ((uint32)NOR_CMD_WRITE_VCR << 24U)         | /* write volatile config reg cmd */
                  ((uint32)0U << 23U)                        | /* read data disable */
                  ((uint32)7U << 20U)                        | /* read 8 data bytes */
                  ((uint32)1U << 19U)                        | /* enable cmd adddr */
                  ((uint32)addrBytes << 16U)                 | /* address bytes */
                  (1U << 15U);                          /* write data enable */
        data[1] = 1U;                                  /* Dummy cycle config register address */
        data[2] = dummyCycle;                         /* Dummy cycle # */
        retVal = OSPI_control(handle, SPI_V0_CMD_CFG_DUMMY_CYCLE, (void *)data);
    }

    return retVal;
}


#define FLS_STOP_SEC_CODE
#include "Fls_MemMap.h"
