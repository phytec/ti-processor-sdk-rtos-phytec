/**
 *  \file   monitor.c
 *
 *  \brief  CAN Profiling XIP FOTA application monitor file.
 *
 */
 /*
 *  Copyright (C) 2024 Texas Instruments Incorporated - http://www.ti.com/
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

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "fota_writer.h"

/* UART Header files */
#include <ti/drv/uart/UART.h>
#include <ti/drv/uart/UART_stdio.h>

#include <ti/osal/TaskP.h>
#include <ti/csl/arch/csl_arch.h>

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

int32_t WriterApp_VerifyData(uint8_t *expData, uint8_t *rxData, uint32_t length) __attribute__((section(".fota_writer_app")));
void WriterApp_GeneratePattern(uint8_t *txBuf, uint8_t *rxBuf, uint32_t length) __attribute__((section(".fota_writer_app")));
static void WriterApp_OspiDelay(uint32_t delay) __attribute__((section(".fota_writer_app")));
static int32_t WriterApp_OspiFlashExecCmd(const CSL_ospi_flash_cfgRegs *pRegs) __attribute__((section(".fota_writer_app")));
static int32_t WriterApp_OspiCmdRead(const CSL_ospi_flash_cfgRegs *pRegs, uint32_t cmd, uint8_t *rxBuf, uint32_t rxLen) __attribute__((section(".fota_writer_app")));

static int32_t WriterApp_OspiCmdWrite(const CSL_ospi_flash_cfgRegs *pRegs,
                                      const uint8_t                *cmdBuf,
                                      uint32_t                      cmdLen,
                                      const uint8_t                *txBuf,
                                      uint32_t                      txLen) __attribute__((section(".fota_writer_app")));

static uint8_t WriterApp_OspiGetDeviceStatus() __attribute__((section(".fota_writer_app")));
static int32_t WriterApp_OspiWaitDeviceReady(uint32_t timeOut) __attribute__((section(".fota_writer_app")));
static int32_t WriterApp_waitWriteSramLvl(const CSL_ospi_flash_cfgRegs *pReg, uint32_t *sramLvl) __attribute__((section(".fota_writer_app")));
static int32_t WriterApp_waitIndWriteComplete(const CSL_ospi_flash_cfgRegs *pRegs) __attribute__((section(".fota_writer_app")));
static int32_t WriterApp_waitReadSramLvl(const CSL_ospi_flash_cfgRegs *pReg, uint32_t *rdLvl) __attribute__((section(".fota_writer_app")));
static int32_t WriterApp_waitIndReadComplete(const CSL_ospi_flash_cfgRegs *pRegs) __attribute__((section(".fota_writer_app")));

/**********************************************************************
 ************************** Global Variables **************************
 **********************************************************************/

extern SemaphoreP_Handle WriterApp_WriteCompletionSemaphore, WriterApp_FlashRequestSemaphore, WriterApp_FlashAccessSemaphore, WriterApp_ChunkWriteCompleteSemaphore;
extern TaskP_Handle gMainTask, gCanTask, gMonitorTask, gWriterTask;

extern void sbl_puts(char *str);
extern void sbl_putsNum(char *str, uint32_t num);

extern uint8_t *dataBufAppimage;
extern uint8_t *dataBufAppimageXip;
extern uint8_t *chkBuf;

/** \brief Compare data and return error on mismatch */
int32_t WriterApp_VerifyData(uint8_t *expData, uint8_t *rxData, uint32_t length)
{
    uint32_t idx = 0;
    uint32_t match = 1;
    int32_t  retVal = (int32_t)(-1);

    for(idx = 0; ((idx < length) && (match != 0)); idx++)
    {
        if(*expData != *rxData)
        {
            match = 0;
            UART_printf("Data mismatch at idx %d\n", idx);
        }
        expData++;
        rxData++;
    }

    if(match == 1)
    {
        retVal = 0;
    }

    return retVal;
}

/** \brief Generate a test pattern */
void WriterApp_GeneratePattern(uint8_t *txBuf, uint8_t *rxBuf, uint32_t length)
{
    volatile uint32_t idx;
    volatile uint8_t *txPtr = txBuf;
    volatile uint8_t *rxPtr = rxBuf;

    for(idx = 0; idx < length; idx++)
    {
        if (idx < (length/2))
        {
            *txPtr++ = (uint8_t)idx;
        }
        else if (idx < (length/4*3))
        {
            *txPtr++ = 0xaa;
        }
        else
        {
            *txPtr++ = 0x55;
        }
    	*rxPtr++ = (uint8_t)0U;
    }
}

/** \brief Blocking delay */
static void WriterApp_OspiDelay(uint32_t delay)
{
    volatile uint32_t i = delay;

    while (i > 0U)
    {
        i = i - 1U;
    }
}

/** \brief Execute the command previuosly written */
static int32_t WriterApp_OspiFlashExecCmd(const CSL_ospi_flash_cfgRegs *pRegs)
{
    uint32_t retry = OSPI_DAC_WRITE_TIMEOUT;
    int32_t  retVal = 0;
    uint32_t idleFlag = FALSE;

    while (idleFlag == FALSE)
    {
        idleFlag = CSL_ospiIsIdle(pRegs);
    }

    /* Start to execute flash read/write command */
    CSL_ospiFlashExecCmd(pRegs);

    while (retry != 0U)
    {
        /* Check the command execution status */
        if (CSL_ospiFlashExecCmdComplete(pRegs) == TRUE)
        {
            break;
        }
        WriterApp_OspiDelay(OSPI_CHECK_IDLE_DELAY);
        retry--;
    }

    if (retry == 0U)
    {
        retVal = (int32_t)(-1);
    }

    idleFlag = FALSE;
    while (idleFlag == FALSE)
    {
        idleFlag = CSL_ospiIsIdle(pRegs);
    }

    return (retVal);
}

/** \brief Issue a read command */
static int32_t WriterApp_OspiCmdRead(const CSL_ospi_flash_cfgRegs *pRegs, uint32_t cmd, uint8_t *rxBuf, uint32_t rxLen)
{
    uint32_t regVal;
    uint32_t rdLen;
    int32_t  retVal;
    uint8_t *pBuf = rxBuf;

    (void)CSL_ospiCmdRead(pRegs, cmd, rxLen);
    retVal = WriterApp_OspiFlashExecCmd(pRegs);
    if (retVal == 0)
    {
        regVal = *(&pRegs->FLASH_RD_DATA_LOWER_REG);

        /* Put the read value into rxBuf */
        rdLen = (rxLen > 4U) ? 4U : rxLen;
        uint8_t *d = (uint8_t*)pBuf;
        const uint8_t *s = (uint8_t*)&regVal;
        while (rdLen--)
        {
            *d++ = *s++;
        }
        pBuf += rdLen;

        if (rxLen > 4U) {
            regVal = *(&pRegs->FLASH_RD_DATA_UPPER_REG);
            rdLen = rxLen - rdLen;
            uint8_t *d = (uint8_t*)pBuf;
            const uint8_t *s = (uint8_t*)&regVal;
            while (rdLen--)
            {
                *d++ = *s++;
            }
        }
    }
    return (retVal);
}

/** \brief Issue a write command */
static int32_t WriterApp_OspiCmdWrite(const CSL_ospi_flash_cfgRegs *pRegs,
                                      const uint8_t                *cmdBuf,
                                      uint32_t                      cmdLen,
                                      const uint8_t                *txBuf,
                                      uint32_t                      txLen)
{
    (void)CSL_ospiCmdWrite(pRegs, cmdBuf, cmdLen, txBuf, txLen);
    return (WriterApp_OspiFlashExecCmd(pRegs));
}

/** \brief Gets the status of the flash */
static uint8_t WriterApp_OspiGetDeviceStatus()
{
    uint8_t  status         = 0xff;
    uint32_t rdStatusCmd    = OPSI_FLASH_CMD_RDSR;

    const CSL_ospi_flash_cfgRegs *baseAddr = (const CSL_ospi_flash_cfgRegs *)(OSPI_FLASH_CONFIG_REG_BASE_ADDR);

    (void)WriterApp_OspiCmdRead(baseAddr,
                                rdStatusCmd,
                                &status,
                                1);

    return (status);
}

/** \brief Waits till the flash is idle */
static int32_t WriterApp_OspiWaitDeviceReady(uint32_t timeOut)
{
    int32_t      retVal = (int32_t)(-1);
    uint8_t      status;
    uint32_t     timeOutVal = timeOut;

    while (timeOutVal != 0U)
    {
        status = WriterApp_OspiGetDeviceStatus();
        if ((status & 1U) == 0U)
        {
            retVal = 0;
            break;
        }
        timeOutVal--;
        WriterApp_OspiDelay(OSPI_CHECK_IDLE_DELAY);
    }

    return (retVal);
}

/** \brief Waits till OSPI controller is idle */
int32_t WriterApp_WaitOspiIdle(uint32_t timeOut)
{
    uint32_t               retry = 0U;
    int32_t                retVal = (int32_t)(-1);
    uint32_t               timeOutVal = timeOut;

    const CSL_ospi_flash_cfgRegs *baseAddr = (const CSL_ospi_flash_cfgRegs *)(OSPI_FLASH_CONFIG_REG_BASE_ADDR);

    while (timeOutVal != 0U)
    {
        if (CSL_ospiIsIdle(baseAddr) != 0U)
        {
            retry++;
            if (retry == 3U)
            {
                retVal = 0;
                break;
            }
        }
        else
        {
            retry = 0U;
        }
        WriterApp_OspiDelay(OSPI_CHECK_IDLE_DELAY);
        timeOutVal--;
    }

    return (retVal);
}

int32_t WriterApp_OspiConfigDummyCycle(uint32_t dummyCycle)
{
    int32_t retVal;
    uint8_t cmdWren = OSPI_FLASH_CMD_WREN;
    uint32_t addrBytes = 3U;
    uint32_t cmd;
    uint32_t addr;
    uint32_t data;
    const CSL_ospi_flash_cfgRegs *baseAddr = (const CSL_ospi_flash_cfgRegs *)(OSPI_FLASH_CONFIG_REG_BASE_ADDR);

    /* Send Write Enable command */
    retVal = WriterApp_OspiCmdWrite(baseAddr, &cmdWren, 1, NULL, 0);

    /* Enable single transfer rate mode */
    if (retVal == 0U)
    {
        /* send write VCR command to reg addr 0x0 to set to SDR mode */
        cmd     = (OSPI_FLASH_CMD_WRITE_VCR << 24)         | /* write volatile config reg cmd */
                  (0 << 23)                         | /* read data disable */
                  (7 << 20)                         | /* read 8 data bytes */
                  (1 << 19)                         | /* enable cmd adddr */
                  (addrBytes << 16)                 | /* address bytes */
                  (1 << 15);                          /* write data enable */
        addr    = 1;                                  /* Dummy cycle config register address */
        data    = dummyCycle;                         /* Dummy cycle # */

        CSL_ospiFlashStig(baseAddr, cmd, addr, data);
        retVal = WriterApp_OspiFlashExecCmd(baseAddr);
    }

    return (retVal);
}

/** \brief Waits till sram level is below watermark level */
static int32_t WriterApp_waitWriteSramLvl(const CSL_ospi_flash_cfgRegs *pReg, uint32_t *sramLvl)
{
    uint32_t retry = CSL_OSPI_REG_RETRY;
    uint32_t sramLevel;
    int32_t  retVal = 0;

    while(retry != 0U)
    {
        sramLevel = CSL_ospiGetSramLvl(pReg, 0);
        if (sramLevel <= CSL_OSPI_SRAM_WATERMARK_WR_LVL)
        {
			*sramLvl = sramLevel;
            break;
        }
        WriterApp_OspiDelay(OSPI_CHECK_IDLE_DELAY);
        retry--;
    }

    if (retry != 0U)
    {
        retVal = 0;
    }
    else
    {
        retVal = (int32_t)(-1);
    }
    return(retVal);
}

/** \brief Wait for indirect write completion */
static int32_t WriterApp_waitIndWriteComplete(const CSL_ospi_flash_cfgRegs *pRegs)
{
    uint32_t retry = OSPI_DAC_WRITE_TIMEOUT;
    int32_t  retVal;

    /* Check flash indirect write controller status */
    while (retry != 0U)
    {
        if (CSL_ospiIsIndWriteComplete(pRegs) == TRUE)
        {
            break;
        }
        WriterApp_OspiDelay(OSPI_CHECK_IDLE_DELAY);
        retry--;
    }

    if (retry != 0U)
    {
        /* Clear indirect completion status */
        CSL_ospiClrIndWriteComplete(pRegs);
        retVal = 0;
    }
    else
    {
        retVal = (int32_t)(-1);
    }
    return(retVal);
}

/** \brief Performs indirect write operation */
int32_t WriterApp_IndWrite(uint32_t offset, uint8_t* pSrc, uint32_t count)
{
    uint32_t               remaining;
    uint32_t               sramLevel, wrBytes;
    int32_t                retVal = 0;

    const CSL_ospi_flash_cfgRegs *baseAddr = (const CSL_ospi_flash_cfgRegs *)(OSPI_FLASH_CONFIG_REG_BASE_ADDR);

    /* Set write address in indirect mode */
    CSL_ospiIndSetStartAddr(baseAddr, offset, FALSE);
    CSL_ospiIndWriteExecute(baseAddr, count);

    remaining = count;
    while(remaining > 0U)
    {
        /* Wait indirect write SRAM fifo level below watermark */
        if (WriterApp_waitWriteSramLvl(baseAddr, &sramLevel) != (int32_t)0U)
        {
            retVal = (int32_t)(-1);
            break;
        }

        wrBytes = (CSL_OSPI_SRAM_PARTITION_WR - sramLevel) * CSL_OSPI_FIFO_WIDTH;
        wrBytes = (wrBytes > remaining) ? remaining : wrBytes;

        /* Write data to FIFO */
        CSL_ospiWriteFifoData(OSPI_FLASH_DATA_BASE_ADDR + OSPI_INDAC_TRIG_ADDR, pSrc, wrBytes);

        pSrc += wrBytes;
        remaining -= wrBytes;
    }

    if(retVal == 0)
    {
        if (WriterApp_waitIndWriteComplete(baseAddr) != (int32_t)0U)
        {
            retVal = (int32_t)(-1);
        }
        if (WriterApp_OspiWaitDeviceReady(OSPI_DAC_WRITE_TIMEOUT) != (int32_t)0U)
        {
            retVal = (int32_t)(-1);
        }
    }

    if(retVal == (int32_t)(-1))
    {
        CSL_ospiIndWriteCancel(baseAddr);
    }
    return (retVal);
}

/** \brief Waits till sram level is below watermark level */
static int32_t WriterApp_waitReadSramLvl(const CSL_ospi_flash_cfgRegs *pReg, uint32_t *rdLvl)
{
    uint32_t retry = OSPI_DAC_WRITE_TIMEOUT;
    uint32_t sramLevel;
    int32_t  retVal = 0;

    while(retry != 0U)
    {
        sramLevel = CSL_ospiGetSramLvl(pReg, 1U);
        if (sramLevel != 0U)
        {
			*rdLvl = sramLevel;
            break;
        }
        WriterApp_OspiDelay(OSPI_CHECK_IDLE_DELAY);
        retry--;
    }

    if (retry != 0U)
    {
        retVal = 0;
    }
    else
    {
        retVal = (int32_t)(-1);
    }
    return(retVal);
}

/** \brief Wait for indirect write completion */
static int32_t WriterApp_waitIndReadComplete(const CSL_ospi_flash_cfgRegs *pRegs)
{
    uint32_t retry = OSPI_DAC_WRITE_TIMEOUT;
    int32_t  retVal;

    /* Check flash indirect write controller status */
    while (retry != 0U)
    {
        if (CSL_ospiIndReadComplete(pRegs) == TRUE)
        {
            break;
        }
        WriterApp_OspiDelay(OSPI_CHECK_IDLE_DELAY);
        retry--;
    }

    if (retry != 0U)
    {
        /* Clear indirect completion status */
        CSL_ospiClrIndReadComplete(pRegs);
        retVal = 0;
    }
    else
    {
        retVal = (int32_t)(-1);
    }
    return(retVal);
}

/** \brief Performs indirect read operation */
int32_t WriterApp_IndRead(uint32_t offset, uint8_t* pDst, uint32_t count)
{
    uint32_t               remaining;
    uint32_t               sramLevel, rdBytes;
    int32_t                retVal = 0;

    const CSL_ospi_flash_cfgRegs *baseAddr = (const CSL_ospi_flash_cfgRegs *)(OSPI_FLASH_CONFIG_REG_BASE_ADDR);

    /* Set write address in indirect mode */
    CSL_ospiIndSetStartAddr(baseAddr, offset, FALSE);
    CSL_ospiIndReadExecute(baseAddr, count);

    remaining = count;
    while(remaining > 0U)
    {
        /* Wait indirect read SRAM fifo level below watermark */
        if (WriterApp_waitReadSramLvl(baseAddr, &sramLevel) != (int32_t)0U)
        {
            retVal = (int32_t)(-1);
            break;
        }

        rdBytes = sramLevel * CSL_OSPI_FIFO_WIDTH;
        rdBytes = (rdBytes > remaining) ? remaining : rdBytes;

        /* Read data from FIFO */
        CSL_ospiReadFifoData(OSPI_FLASH_DATA_BASE_ADDR + OSPI_INDAC_TRIG_ADDR, pDst, rdBytes);

        pDst += rdBytes;
        remaining -= rdBytes;
    }

    if(retVal == 0)
    {
        if (WriterApp_waitIndReadComplete(baseAddr) != (int32_t)0U)
        {
            retVal = (int32_t)(-1);
        }
        if (WriterApp_OspiWaitDeviceReady(OSPI_DAC_WRITE_TIMEOUT) != (int32_t)0U)
        {
            retVal = (int32_t)(-1);
        }
    }

    return (retVal);
}

/** \brief Function to switch between DAC/INDAC mode */
/* NOTE: This switch function is not setting up the dummy cycles which are required for reading only */
int32_t WriterApp_OspiSwitchMode(bool dacEnable)
{
    int32_t retVal = 0;

    const CSL_ospi_flash_cfgRegs *baseAddr = (const CSL_ospi_flash_cfgRegs *)(OSPI_FLASH_CONFIG_REG_BASE_ADDR);

    /* Disable OSPI controller */
    CSL_ospiEnable(baseAddr, FALSE);

    if (dacEnable)
    {
        /* Enable PHY mode */
        CSL_ospiPhyEnable(baseAddr, TRUE);
        
        /* Enable PHY pipeline mode */
        CSL_ospiPipelinePhyEnable(baseAddr, TRUE);

        /* Set indirect trigger address register */
        CSL_ospiSetIndTrigAddr(baseAddr, 0x4000000);

        /* Set the divider to 0 */
        CSL_ospiSetPreScaler(baseAddr, CSL_OSPI_BAUD_RATE_DIVISOR(2U));

        /* Disable DAC mode */
        CSL_ospiDacEnable(baseAddr, TRUE);
    }    
    else
    {
        /* Disable PHY mode */
        CSL_ospiPhyEnable(baseAddr, FALSE);
        
        /* Disable PHY pipeline mode */
        CSL_ospiPipelinePhyEnable(baseAddr, FALSE);

        /* Set indirect trigger address register */
        CSL_ospiSetIndTrigAddr(baseAddr, 0x3FE0000);

        /* Set the divider to 32 */
        CSL_ospiSetPreScaler(baseAddr, CSL_OSPI_BAUD_RATE_DIVISOR_DEFAULT);

        /* Disable DAC mode */
        CSL_ospiDacEnable(baseAddr, FALSE);
    }

    /* Enable OSPI controller */
    CSL_ospiEnable(baseAddr, TRUE);

    return (retVal);
}

/** \brief Function to convert offset to block numer in Flash */
int32_t WriterApp_OspiOffsetToBlkPage(uint32_t offset, uint32_t *block, uint32_t *page)
{
    int32_t retVal = 0;
    uint32_t leftover, block_size, block_count, page_size, page_count;

    block_count = OSPI_FLASH_NUM_BLOCKS;
    page_size	= OSPI_FLASH_PAGE_SIZE;
    page_count 	= OSPI_FLASH_NUM_PAGES_PER_BLOCK;

    block_size = (page_count * page_size);

    *block 	  = offset / block_size;
    leftover  = offset % block_size;
    *page 	  = leftover / page_size;

    if (leftover % page_size)
    {
        /* All writes must be page aligned for now */
        retVal = (int32_t)(-1);
    }
    if (*block > block_count)
    {
        retVal = (int32_t)(-1);
    }
    if (*page > page_count)
    {
        retVal = (int32_t)(-1);
    }

    return (retVal);
}

/** \brief Function to erase a block in Flash */
int32_t WriterApp_OspiErase(int32_t erLoc)
{
    int32_t         retVal = 0;
    uint8_t         cmd[5];
    uint32_t        cmdLen;
    uint32_t        address = 0;
    uint8_t         cmdWren  = OSPI_FLASH_CMD_WREN;
    const           CSL_ospi_flash_cfgRegs *baseAddr = (const CSL_ospi_flash_cfgRegs *)(OSPI_FLASH_CONFIG_REG_BASE_ADDR);

    if (erLoc >= OSPI_FLASH_NUM_BLOCKS)
    {
        retVal = (int32_t)(-1);
    }
    address   = erLoc * OSPI_FLASH_BLOCK_SIZE;
    cmd[0] = OSPI_FLASH_CMD_BLOCK_ERASE;

    cmd[1] = (address >> 24) & 0xff; /* 4 address bytes */
    cmd[2] = (address >> 16) & 0xff;
    cmd[3] = (address >>  8) & 0xff;
    cmd[4] = (address >>  0) & 0xff;
    cmdLen = 5;

#if defined(NOR_XSPI)
    if (WriterApp_OspiWaitDeviceReady(OSPI_FLASH_WRR_WRITE_TIMEOUT))
    {
        retVal = (int32_t)(-1);
    }
#endif

    if (WriterApp_OspiCmdWrite(baseAddr, &cmdWren, 1, NULL, 0))
    {
        retVal = (int32_t)(-1);
    }

    if (WriterApp_OspiWaitDeviceReady(OSPI_FLASH_WRR_WRITE_TIMEOUT))
    {
        retVal = (int32_t)(-1);
    }

    if (WriterApp_OspiCmdWrite(baseAddr, cmd, cmdLen, NULL, 0))
    {
        retVal = (int32_t)(-1);
    }

    if (WriterApp_OspiWaitDeviceReady(OSPI_FLASH_BULK_ERASE_TIMEOUT))
    {
        retVal = (int32_t)(-1);
    }

    return (retVal);
}

/** \brief Contains the loop that performs FOTA in chunks */
void WriterApp_Test(uint32_t updateImageOffset)
{
    uintptr_t   hwiKey;
    uint32_t    tskKey;
    uint32_t    testLen     = TEST_BUF_LEN;
    uint32_t    retVal      = 0;
    volatile uint64_t   preTimeStamp, postTimeStamp;
    uint32_t appImageExpected = 1;
    uint32_t offset = APPIMAGE_OFFSET;
    uint32_t artificialOffset = P1_ARTIFICIAL_OFFSET;
    Bootloader_MetaHeaderStart static mHdrStr;
    Bootloader_MetaHeaderCore  static mHdrCore[BOOTLOADER_MAX_INPUT_FILES];
    Bootloader_RprcFileHeader  static rprcHeader;
    Bootloader_RprcSectionHeader static rprcSection;
    uint32_t static sectionCount;
    uint8_t *txBuf = &dataBufAppimage[0];
    uint8_t *xipImagePtr = &dataBufAppimageXip[0];

    memset(&mHdrCore[0], 0xFF, BOOTLOADER_MAX_INPUT_FILES*sizeof(Bootloader_MetaHeaderCore));
    memcpy(&mHdrStr, xipImagePtr, sizeof(Bootloader_MetaHeaderStart));
    xipImagePtr += sizeof(Bootloader_MetaHeaderStart);
    if(mHdrStr.magicStr != BOOTLOADER_META_HDR_MAGIC_STR)
    {
        sbl_puts("\rWriter App: Metaheader magic string mismatch! Exiting!\n");
        return;
    }
    else
    {
        /* Read all the core offset addresses */
        uint32_t i;
        for(i=0U; i<mHdrStr.numFiles; i++)
        {
            memcpy(&mHdrCore[i], xipImagePtr, sizeof(Bootloader_MetaHeaderCore));
            xipImagePtr += sizeof(Bootloader_MetaHeaderCore);
        }
    }
    /* Skip Header End */
    xipImagePtr += sizeof(Bootloader_MetaHeaderCore);
    memcpy(&rprcHeader, xipImagePtr, sizeof(Bootloader_RprcFileHeader));
    xipImagePtr += sizeof(Bootloader_RprcFileHeader);
    if(rprcHeader.magic != BOOTLOADER_RPRC_MAGIC_NUMBER)
    {
        sbl_puts("\rWriter App: RPRC header magic string mismatch! Exiting!\n");
        return;
    }

    while(1)
    {
        /* Request Flash access from the XIP application */
        SemaphoreP_post(WriterApp_FlashRequestSemaphore);

        /* Wait for the XIP application to give flash access */
        SemaphoreP_pend(WriterApp_FlashAccessSemaphore, SemaphoreP_WAIT_FOREVER);

        sbl_putsNum("\rWriter App: Writing a 256B chunk at 0x%X\n", (OSPI_FLASH_DATA_BASE_ADDR + offset));

        preTimeStamp = TimerP_getTimeInUsecs();

        hwiKey = HwiP_disable();
        tskKey = TaskP_disable();

        /* Switch to INDAC mode */
        retVal += WriterApp_OspiSwitchMode(FALSE);

        /* Write buffer to flash */
        retVal += WriterApp_IndWrite(offset, txBuf, testLen);

        /* Wait for the OSPI controller to be idle */
        retVal += WriterApp_WaitOspiIdle(OSPI_CHECK_IDLE_DELAY);

        /* Switch back to DAC mode */
        retVal += WriterApp_OspiSwitchMode(TRUE);

        TaskP_restore(tskKey);
        HwiP_restore(hwiKey);

        postTimeStamp = TimerP_getTimeInUsecs();

        if(retVal != 0)
        {
            sbl_putsNum("\rWriter App: Writing a 256B chunk at 0x%X failed!\n", offset);
        }
        else
        {
            sbl_putsNum("\rWriter App: Writing a 256B chunk took %d us!\n", (postTimeStamp-preTimeStamp));
            offset += 256U;
            txBuf += 256;
        }

        SemaphoreP_post(WriterApp_ChunkWriteCompleteSemaphore);

        if(appImageExpected && (offset >= (APPIMAGE_OFFSET + UPDATE_APPIMAGE_SIZE)) )     /* Appimage write complete */
        {
            sbl_puts("\rWriter App: .appimage write complete! writing .appimage_xip sections now!\n");
            appImageExpected = 0;
            sectionCount = 1;
            memcpy(&rprcSection, xipImagePtr, sizeof(Bootloader_RprcSectionHeader));
            xipImagePtr += sizeof(Bootloader_RprcSectionHeader);

            if(updateImageOffset == P1_REGION_OFFSET)
            {
                artificialOffset = P1_ARTIFICIAL_OFFSET;
            }
            else
            {
                artificialOffset = P2_ARTIFICIAL_OFFSET;
            }

            offset = rprcSection.addr + artificialOffset;
            txBuf = xipImagePtr;
        }
        else if(!appImageExpected && ((offset-artificialOffset) >= (rprcSection.addr+rprcSection.size)))
        {
            if(sectionCount < rprcHeader.sectionCount)
            {
                memcpy(&rprcSection, xipImagePtr, sizeof(Bootloader_RprcSectionHeader));
                xipImagePtr += sizeof(Bootloader_RprcSectionHeader);
                offset = rprcSection.addr + artificialOffset;
                txBuf = xipImagePtr;
                sectionCount++;
            }
            else
            {
                break;
            }
        }
    }

    SemaphoreP_post(WriterApp_WriteCompletionSemaphore);
    return;
}
