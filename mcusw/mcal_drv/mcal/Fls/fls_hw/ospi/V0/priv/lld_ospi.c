/**
 *  \file   csl_ospi.c
 *
 *  \brief  OSPI IP V0 controller hardware abstraction APIs.
 *
 *   This file contains the hardware abstraction layer APIs for OSPI.
 */

/*
 * Copyright (C) 2024 Texas Instruments Incorporated - http://www.ti.com/
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of Texas Instruments Incorporated nor the names of
 * its contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdint.h>
#include <hw_include/lld_types.h>
#include <Fls/fls_hw/ospi/V0/lld_ospi.h>

/*******************************************************************************
*                        API FUNCTION DEFINITIONS
*******************************************************************************/

void CSL_ospiEnable(const CSL_ospi_flash_cfgRegs *pRegs,
                    uint32                      enable)
{
    LLD_REG32_FINS(&pRegs->CONFIG_REG,
                   OSPI_FLASH_CFG_CONFIG_REG_ENB_SPI_FLD,
                   enable);
}

void CSL_ospiDacEnable(const CSL_ospi_flash_cfgRegs *pRegs,
                       uint32                      enable)
{
    LLD_REG32_FINS(&pRegs->CONFIG_REG,
                   OSPI_FLASH_CFG_CONFIG_REG_ENB_DIR_ACC_CTLR_FLD,
                   enable);
}

void CSL_ospiXipEnable(const CSL_ospi_flash_cfgRegs *pRegs,
                       uint32                      enable)
{
    LLD_REG32_FINS(&pRegs->CONFIG_REG,
                   OSPI_FLASH_CFG_CONFIG_REG_ENTER_XIP_MODE_FLD,
                   enable);
}

void CSL_ospiSetModeBits(const CSL_ospi_flash_cfgRegs *pRegs,
                          uint32                      flashModeVal)
{
    LLD_REG32_FINS(&pRegs->MODE_BIT_CONFIG_REG,
                   OSPI_FLASH_CFG_MODE_BIT_CONFIG_REG_MODE_FLD,
                   flashModeVal);

}

void CSL_ospiPhyEnable(const CSL_ospi_flash_cfgRegs *pRegs,
                       uint32                      enable)
{
    LLD_REG32_FINS(&pRegs->CONFIG_REG,
                   OSPI_FLASH_CFG_CONFIG_REG_PHY_MODE_ENABLE_FLD,
                   enable);
}

void CSL_ospiPipelinePhyEnable(const CSL_ospi_flash_cfgRegs *pRegs, uint32 enable)
{
    LLD_REG32_FINS(&pRegs->CONFIG_REG,
                   OSPI_FLASH_CFG_CONFIG_REG_PIPELINE_PHY_FLD,
                   enable);
}

void CSL_ospiDtrEnable(const CSL_ospi_flash_cfgRegs *pRegs, uint32 enable)
{
    LLD_REG32_FINS(&pRegs->CONFIG_REG,
                   OSPI_FLASH_CFG_CONFIG_REG_ENABLE_DTR_PROTOCOL_FLD,
                   enable);
    LLD_REG32_FINS(&pRegs->DEV_INSTR_RD_CONFIG_REG,
                   OSPI_FLASH_CFG_DEV_INSTR_RD_CONFIG_REG_DDR_EN_FLD,
                   enable);
}

void CSL_ospiSetPreScaler(const CSL_ospi_flash_cfgRegs *pRegs,
                          uint32                      clkDividerVal)
{
    LLD_REG32_FINS(&pRegs->CONFIG_REG,
                   OSPI_FLASH_CFG_CONFIG_REG_MSTR_BAUD_DIV_FLD,
                   clkDividerVal);

}

void CSL_ospiSetClkMode(const CSL_ospi_flash_cfgRegs *pRegs,
                        uint32                      clkMode)
{
    uint32 regVal = LLD_REG32_RD(&pRegs->CONFIG_REG);

    regVal &= ~(CSL_OSPI_FLASH_CFG_CONFIG_REG_SEL_CLK_PHASE_FLD_MASK | \
                CSL_OSPI_FLASH_CFG_CONFIG_REG_SEL_CLK_POL_FLD_MASK);
    regVal |= clkMode;
    LLD_REG32_WR(&pRegs->CONFIG_REG, regVal);
}

void CSL_ospiSetChipSelect(const CSL_ospi_flash_cfgRegs *pRegs,
                           uint32                      chipSelect,
                           uint32                      decSelect)
{
    uint32 regVal = LLD_REG32_RD(&pRegs->CONFIG_REG);
	uint32 cs = CSL_OSPI_CHIP_SELECT(chipSelect);

    regVal &= ~(CSL_OSPI_FLASH_CFG_CONFIG_REG_PERIPH_SEL_DEC_FLD_MASK | \
                CSL_OSPI_FLASH_CFG_CONFIG_REG_PERIPH_CS_LINES_FLD_MASK);
    regVal |= (decSelect << CSL_OSPI_FLASH_CFG_CONFIG_REG_PERIPH_SEL_DEC_FLD_SHIFT) | \
              (cs << CSL_OSPI_FLASH_CFG_CONFIG_REG_PERIPH_CS_LINES_FLD_SHIFT);
    LLD_REG32_WR(&pRegs->CONFIG_REG, regVal);
}

void CSL_ospiSetDevDelay(const CSL_ospi_flash_cfgRegs *pRegs,
                         const uint32               *delays)
{
    uint32 delay = ((delays[0] << CSL_OSPI_FLASH_CFG_DEV_DELAY_REG_D_INIT_FLD_SHIFT)  | \
	                  (delays[1] << CSL_OSPI_FLASH_CFG_DEV_DELAY_REG_D_AFTER_FLD_SHIFT) | \
	                  (delays[2] << CSL_OSPI_FLASH_CFG_DEV_DELAY_REG_D_BTWN_FLD_SHIFT)  | \
	                  (delays[3] << CSL_OSPI_FLASH_CFG_DEV_DELAY_REG_D_NSS_FLD_SHIFT));
    LLD_REG32_WR(&pRegs->DEV_DELAY_REG, delay);
}

void CSL_ospiSetDevSize(const CSL_ospi_flash_cfgRegs *pRegs,
                        uint32                      numAddrBytes,
                        uint32                      pageSize,
                        uint32                      blkSize)
{
    uint32 regVal = LLD_REG32_RD(&pRegs->DEV_SIZE_CONFIG_REG);

    regVal &= ~(CSL_OSPI_FLASH_CFG_DEV_SIZE_CONFIG_REG_NUM_ADDR_BYTES_FLD_MASK        | \
                CSL_OSPI_FLASH_CFG_DEV_SIZE_CONFIG_REG_BYTES_PER_DEVICE_PAGE_FLD_MASK | \
                CSL_OSPI_FLASH_CFG_DEV_SIZE_CONFIG_REG_BYTES_PER_SUBSECTOR_FLD_MASK);

    regVal |= (numAddrBytes << CSL_OSPI_FLASH_CFG_DEV_SIZE_CONFIG_REG_NUM_ADDR_BYTES_FLD_SHIFT)    | \
              (pageSize << CSL_OSPI_FLASH_CFG_DEV_SIZE_CONFIG_REG_BYTES_PER_DEVICE_PAGE_FLD_SHIFT) | \
              (blkSize << CSL_OSPI_FLASH_CFG_DEV_SIZE_CONFIG_REG_BYTES_PER_SUBSECTOR_FLD_SHIFT);

    LLD_REG32_WR(&pRegs->DEV_SIZE_CONFIG_REG, regVal);
}

void CSL_ospiSetIndTrigAddr(const CSL_ospi_flash_cfgRegs *pRegs,
                            uint32                      indTrigAddr)
{
    LLD_REG32_WR(&pRegs->IND_AHB_ADDR_TRIGGER_REG, indTrigAddr);
}

void CSL_ospiSetWrCompAutoPolling(const CSL_ospi_flash_cfgRegs *pRegs,
                                  uint32                      pollingState)
{
    if (pollingState)
    {
        LLD_REG32_FINS(&pRegs->WRITE_COMPLETION_CTRL_REG,
                       OSPI_FLASH_CFG_WRITE_COMPLETION_CTRL_REG_DISABLE_POLLING_FLD,
                       0U);
    }
    else
    {
        LLD_REG32_FINS(&pRegs->WRITE_COMPLETION_CTRL_REG,
                       OSPI_FLASH_CFG_WRITE_COMPLETION_CTRL_REG_DISABLE_POLLING_FLD,
                       1);
        LLD_REG32_FINS(&pRegs->WRITE_COMPLETION_CTRL_REG,
                       OSPI_FLASH_CFG_WRITE_COMPLETION_CTRL_REG_POLL_COUNT_FLD,
                       1U);
    }
}


void CSL_ospiSetPollingDummyCycles(const CSL_ospi_flash_cfgRegs *pRegs,
                                   uint32                      dummyCycles)
{
    LLD_REG32_FINS(&pRegs->POLLING_FLASH_STATUS_REG,
                    OSPI_FLASH_CFG_POLLING_FLASH_STATUS_REG_DEVICE_STATUS_NB_DUMMY,
                    dummyCycles);
}


void CSL_ospiSetSramPartition(const CSL_ospi_flash_cfgRegs *pRegs,
                              uint32                      partition)
{
    LLD_REG32_WR(&pRegs->SRAM_PARTITION_CFG_REG, partition);
}

void CSL_ospiIntrEnable(const CSL_ospi_flash_cfgRegs *pRegs,
                        uint32                      intrFlag,
                        uint32                      enable)
{
    uint32 regVal = LLD_REG32_RD(&pRegs->IRQ_MASK_REG);

    if (enable == TRUE)
    {
        regVal |= intrFlag;
    }
    else
    {
        regVal &= ~(intrFlag);
    }
    LLD_REG32_WR(&pRegs->IRQ_MASK_REG, regVal);
}

uint32 CSL_ospiIntrStatus(const CSL_ospi_flash_cfgRegs *pRegs)
{
    uint32 regVal = LLD_REG32_RD(&pRegs->IRQ_STATUS_REG);
    return (regVal);
}

void CSL_ospiIntrClear(const CSL_ospi_flash_cfgRegs *pRegs,
                       uint32                      intrFlag)
{
    LLD_REG32_WR(&pRegs->IRQ_STATUS_REG, intrFlag);
}

static uint32 CSL_ospiCmd2Addr(const uint8 *addrBuf, uint32 addrWidth); /* for misra warnings */
static uint32 CSL_ospiCmd2Addr(const uint8 *addrBuf, uint32 addrWidth)
{
    uint32 addr = 0U;

    if (addrWidth == 1U) {
        addr = ((uint32)addrBuf[0] << 0U);
    }
    else if (addrWidth == 2U) {
        addr = ((uint32)addrBuf[0] << 8U) | ((uint32)addrBuf[1] << 0U);
    }
    else if (addrWidth == 3U) {
        addr = ((uint32)addrBuf[0] << 16U) | ((uint32)addrBuf[1] << 8U) |
            ((uint32)addrBuf[2] << 0U);
    }
    else {
        addr = ((uint32)addrBuf[0] << 24U) | ((uint32)addrBuf[1] << 16U) |
            ((uint32)addrBuf[2] << 8U) | ((uint32)addrBuf[3] << 0U);
    }

    return addr;
}

void CSL_ospiFlashExecCmd(const CSL_ospi_flash_cfgRegs *pRegs)
{
    /* Start to execute */
    LLD_REG32_FINS(&pRegs->FLASH_CMD_CTRL_REG,
                   OSPI_FLASH_CFG_FLASH_CMD_CTRL_REG_CMD_EXEC_FLD,
                   1);
}

uint32 CSL_ospiFlashExecCmdComplete(const CSL_ospi_flash_cfgRegs *pRegs)
{
    uint32 status;
    uint32 retVal;

    /* Check the command execution in progess */
    status = LLD_REG32_FEXT(&pRegs->FLASH_CMD_CTRL_REG,
                            OSPI_FLASH_CFG_FLASH_CMD_CTRL_REG_CMD_EXEC_STATUS_FLD);
    if (status == 0U)
    {
        retVal = TRUE;
    }
    else
    {
        retVal = FALSE;
    }

    return (retVal);
}

sint32 CSL_ospiCmdRead(const CSL_ospi_flash_cfgRegs *pRegs,
                        uint32                      cmd,
                        uint32                      rxLen)
{
    uint32 regVal;
    uint32 val;
    uint32 dummyCycles;

    /* Check if DTR is enabled */
    val = LLD_REG32_FEXT(&pRegs->CONFIG_REG,
                         OSPI_FLASH_CFG_CONFIG_REG_ENABLE_DTR_PROTOCOL_FLD);
    if (val == 1U)
    {
        dummyCycles = CSL_OSPI_CMD_READ_OCTAL_DUMMY_CLOCKS;
    }
    else
    {
        dummyCycles = CSL_OSPI_CMD_READ_SINGLE_DUMMY_CLOCKS;
    }

    regVal =
        (cmd << CSL_OSPI_FLASH_CFG_FLASH_CMD_CTRL_REG_CMD_OPCODE_FLD_SHIFT) | \
        (CSL_OSPI_FLASH_CFG_FLASH_CMD_CTRL_REG_ENB_READ_DATA_FLD_MASK)      | \
        (rxLen << CSL_OSPI_FLASH_CFG_FLASH_CMD_CTRL_REG_NUM_RD_DATA_BYTES_FLD_SHIFT) | \
        (2U << CSL_OSPI_FLASH_CFG_FLASH_CMD_CTRL_REG_NUM_ADDR_BYTES_FLD_SHIFT) | \
        (dummyCycles << CSL_OSPI_FLASH_CFG_FLASH_CMD_CTRL_REG_NUM_DUMMY_CYCLES_FLD_SHIFT);
    LLD_REG32_WR(&pRegs->FLASH_CMD_CTRL_REG, regVal);

    return (LLD_PASS);
}

sint32 CSL_ospiCmdExtRead(const CSL_ospi_flash_cfgRegs *pRegs,
                           const uint8                *cmdBuf,
                           uint32                      cmdLen,
                           uint32                      rxLen,
                           uint32                      dummyCycles)
{
    uint32 regVal = 0;
    uint32 opcodeLo;
    uint32 opcodeUp;
    uint32 cmdAddr;

    if (cmdLen > 1U)
    {
        /* Enable Command address in command control register */
        regVal |= CSL_OSPI_FLASH_CFG_FLASH_CMD_CTRL_REG_ENB_COMD_ADDR_FLD_MASK | \
            ((cmdLen - 2U) << CSL_OSPI_FLASH_CFG_FLASH_CMD_CTRL_REG_NUM_ADDR_BYTES_FLD_SHIFT);

        /* Set command address register */
        cmdAddr = CSL_ospiCmd2Addr(&cmdBuf[1], (cmdLen - 1U));
        LLD_REG32_WR(&pRegs->FLASH_CMD_ADDR_REG, cmdAddr);
    }

    if(CSL_ospiGetDualByteOpcodeMode(pRegs) != 0U)
    {
        CSL_ospiExtOpcodeGet(pRegs, &opcodeLo, &opcodeUp);
        opcodeLo = (opcodeLo & ~(uint32)(0xFF)) | cmdBuf[0];
        CSL_ospiExtOpcodeSet(pRegs, opcodeLo, opcodeUp);
    }

    regVal |=
        (cmdBuf[0] << CSL_OSPI_FLASH_CFG_FLASH_CMD_CTRL_REG_CMD_OPCODE_FLD_SHIFT) | \
        (CSL_OSPI_FLASH_CFG_FLASH_CMD_CTRL_REG_ENB_READ_DATA_FLD_MASK)      | \
        (rxLen << CSL_OSPI_FLASH_CFG_FLASH_CMD_CTRL_REG_NUM_RD_DATA_BYTES_FLD_SHIFT) | \
        (dummyCycles << CSL_OSPI_FLASH_CFG_FLASH_CMD_CTRL_REG_NUM_DUMMY_CYCLES_FLD_SHIFT);
    LLD_REG32_WR(&pRegs->FLASH_CMD_CTRL_REG, regVal);

    return (LLD_PASS);
}

sint32 CSL_ospiCmdWrite(const CSL_ospi_flash_cfgRegs *pRegs,
                         const uint8                *cmdBuf,
                         uint32                      cmdLen,
                         const uint8                *txBuf,
                         uint32                      txLen)
{
    uint32 regVal, cmdAddr, wrData, wrLen;
    uint32 opcodeLo;
    uint32 opcodeUp;

    regVal = (uint32)cmdBuf[0] << \
        CSL_OSPI_FLASH_CFG_FLASH_CMD_CTRL_REG_CMD_OPCODE_FLD_SHIFT;
    if (cmdLen > 1U)
    {
        /* Enable Command address in command control register */
        regVal |= CSL_OSPI_FLASH_CFG_FLASH_CMD_CTRL_REG_ENB_COMD_ADDR_FLD_MASK | \
            ((cmdLen - 2U) << CSL_OSPI_FLASH_CFG_FLASH_CMD_CTRL_REG_NUM_ADDR_BYTES_FLD_SHIFT);

        /* Set command address register */
        cmdAddr = CSL_ospiCmd2Addr(&cmdBuf[1], (cmdLen - 1U));
        LLD_REG32_WR(&pRegs->FLASH_CMD_ADDR_REG, cmdAddr);
    }

    if(CSL_ospiGetDualByteOpcodeMode(pRegs) != 0U)
    {
        CSL_ospiExtOpcodeGet(pRegs, &opcodeLo, &opcodeUp);
        opcodeLo = (opcodeLo & ~(uint32)(0xFF)) | cmdBuf[0];
        CSL_ospiExtOpcodeSet(pRegs, opcodeLo, opcodeUp);
    }

    if (txLen != 0U)
    {
        /* Enable write data in command control register */
        regVal |= CSL_OSPI_FLASH_CFG_FLASH_CMD_CTRL_REG_ENB_WRITE_DATA_FLD_MASK | \
            ((txLen - 1U) << CSL_OSPI_FLASH_CFG_FLASH_CMD_CTRL_REG_NUM_WR_DATA_BYTES_FLD_SHIFT);

        wrLen = (txLen > 4U) ? 4U : txLen;
        memcpy((void *)&wrData,(void *)txBuf,wrLen);

        LLD_REG32_WR(&pRegs->FLASH_WR_DATA_LOWER_REG, wrData);


        if (txLen > 4U)
        {
            txBuf += wrLen;
            wrLen = txLen - wrLen;
            memcpy((void *)&wrData,(void *)txBuf,wrLen);

            LLD_REG32_WR(&pRegs->FLASH_WR_DATA_UPPER_REG, wrData);
        }
    }
    LLD_REG32_WR(&pRegs->FLASH_CMD_CTRL_REG, regVal);
    return (LLD_PASS);
}

void CSL_ospiConfigRead(const CSL_ospi_flash_cfgRegs *pRegs,
                        uint32                      cmd,
                        uint32                      mode,
                        uint32                      dummyClk)
{
    uint32 regVal = LLD_REG32_RD(&pRegs->DEV_INSTR_RD_CONFIG_REG);
    uint32 dtr;
    uint32 instType;

    /* Set instruction type to standard SPI mode for SDR mode */
    dtr = LLD_REG32_FEXT(&pRegs->CONFIG_REG,
                         OSPI_FLASH_CFG_CONFIG_REG_ENABLE_DTR_PROTOCOL_FLD);
    if (dtr == 1U)
    {
        instType = mode;
    }
    else
    {
        instType = 0U;
    }

    /* Configure the Device Write Instruction Configuration Register */
    regVal &= ~(CSL_OSPI_FLASH_CFG_DEV_INSTR_RD_CONFIG_REG_INSTR_TYPE_FLD_MASK              | \
                CSL_OSPI_FLASH_CFG_DEV_INSTR_RD_CONFIG_REG_RD_OPCODE_NON_XIP_FLD_MASK       | \
                CSL_OSPI_FLASH_CFG_DEV_INSTR_RD_CONFIG_REG_ADDR_XFER_TYPE_STD_MODE_FLD_MASK | \
                CSL_OSPI_FLASH_CFG_DEV_INSTR_RD_CONFIG_REG_DATA_XFER_TYPE_EXT_MODE_FLD_MASK | \
                CSL_OSPI_FLASH_CFG_DEV_INSTR_RD_CONFIG_REG_DDR_EN_FLD_MASK                  | \
                CSL_OSPI_FLASH_CFG_DEV_INSTR_RD_CONFIG_REG_DUMMY_RD_CLK_CYCLES_FLD_MASK);
    regVal |= (cmd << CSL_OSPI_FLASH_CFG_DEV_INSTR_RD_CONFIG_REG_RD_OPCODE_NON_XIP_FLD_SHIFT)        | \
              (mode << CSL_OSPI_FLASH_CFG_DEV_INSTR_RD_CONFIG_REG_DATA_XFER_TYPE_EXT_MODE_FLD_SHIFT) | \
              (mode << CSL_OSPI_FLASH_CFG_DEV_INSTR_RD_CONFIG_REG_ADDR_XFER_TYPE_STD_MODE_FLD_SHIFT) | \
              (instType << CSL_OSPI_FLASH_CFG_DEV_INSTR_RD_CONFIG_REG_INSTR_TYPE_FLD_SHIFT)          | \
              (dummyClk << CSL_OSPI_FLASH_CFG_DEV_INSTR_RD_CONFIG_REG_DUMMY_RD_CLK_CYCLES_FLD_SHIFT);
    LLD_REG32_WR(&pRegs->DEV_INSTR_RD_CONFIG_REG, regVal);
}

void CSL_ospiReadFifoData(uintptr_t indAddr, uint8 *dest, uint32 rdLen)
{
    uint32 temp;
    uint32 remaining = rdLen;
    uint32 *destPtr = (uint32 *)dest;

    while(remaining >  0U)
    {
        if (remaining >= CSL_OSPI_FIFO_WIDTH)
        {
            *destPtr = LLD_REG32_RD(indAddr);
            remaining -= CSL_OSPI_FIFO_WIDTH;
        }
        else
        {
            temp = LLD_REG32_RD(indAddr);
            memcpy(destPtr, &temp, remaining);
            break;
        }
        destPtr++;
    }
}

uint32 CSL_ospiGetSramLvl(const CSL_ospi_flash_cfgRegs *pRegs,
                            uint32                      read)
{
    uint32 sramLevel;

    if (read == TRUE)
    {
        sramLevel = LLD_REG32_RD(&pRegs->SRAM_FILL_REG) &
            CSL_OSPI_FLASH_CFG_SRAM_FILL_REG_SRAM_FILL_INDAC_READ_FLD_MASK;
    }
    else
    {
        sramLevel = LLD_REG32_RD(&pRegs->SRAM_FILL_REG) >> \
            CSL_OSPI_FLASH_CFG_SRAM_FILL_REG_SRAM_FILL_INDAC_WRITE_FLD_SHIFT;
    }

    return (sramLevel);
}

void CSL_ospiClrIndReadComplete(const CSL_ospi_flash_cfgRegs *pRegs)
{
    LLD_REG32_FINS(&pRegs->INDIRECT_READ_XFER_CTRL_REG,
                   OSPI_FLASH_CFG_INDIRECT_READ_XFER_CTRL_REG_IND_OPS_DONE_STATUS_FLD,
                   1);
}

/* FLS_CoverageGap_04:Dynamic coverage for the function cannot be covered,
 * as it expects a certain value from hardware register.
 */
uint32 CSL_ospiIndReadComplete(const CSL_ospi_flash_cfgRegs *pRegs)
{
    uint32 val;
    uint32 retVal;

    /* Check flash indirect read controller status */
    val = LLD_REG32_FEXT(&pRegs->INDIRECT_READ_XFER_CTRL_REG,
                         OSPI_FLASH_CFG_INDIRECT_READ_XFER_CTRL_REG_IND_OPS_DONE_STATUS_FLD);
    if (val == 1U)
    {
        retVal = TRUE;
    }
    else
    {
        retVal = FALSE;
    }

    return(retVal);
}

void CSL_ospiIndReadExecute(const CSL_ospi_flash_cfgRegs *pRegs,
                            uint32                      rxLen)
{
    /* Set the Indirect Write Transfer Start Address Register */
    LLD_REG32_WR(&pRegs->INDIRECT_READ_XFER_NUM_BYTES_REG, rxLen);

    /* Set the Indirect Write Transfer Watermark Register */
    LLD_REG32_WR(&pRegs->INDIRECT_READ_XFER_WATERMARK_REG,
                 CSL_OSPI_SRAM_WARERMARK_RD_LVL);

    /* Start the indirect read transfer */
    LLD_REG32_FINS(&pRegs->INDIRECT_READ_XFER_CTRL_REG,
                   OSPI_FLASH_CFG_INDIRECT_READ_XFER_CTRL_REG_START_FLD,
                   1);
}

void CSL_ospiWriteSetup(const CSL_ospi_flash_cfgRegs *pRegs,
                        uint32                      cmd,
                        uint32                      mode)
{
    uint32 regVal = LLD_REG32_RD(&pRegs->DEV_INSTR_WR_CONFIG_REG);

    /* Configure the Device Write Instruction Configuration Register */
    regVal &= ~(CSL_OSPI_FLASH_CFG_DEV_INSTR_WR_CONFIG_REG_WR_OPCODE_FLD_MASK               | \
                CSL_OSPI_FLASH_CFG_DEV_INSTR_WR_CONFIG_REG_DATA_XFER_TYPE_EXT_MODE_FLD_MASK | \
                CSL_OSPI_FLASH_CFG_DEV_INSTR_WR_CONFIG_REG_ADDR_XFER_TYPE_STD_MODE_FLD_MASK | \
                CSL_OSPI_FLASH_CFG_DEV_INSTR_WR_CONFIG_REG_DUMMY_WR_CLK_CYCLES_FLD_MASK);
    regVal |= (cmd << CSL_OSPI_FLASH_CFG_DEV_INSTR_WR_CONFIG_REG_WR_OPCODE_FLD_SHIFT) | \
              (mode << CSL_OSPI_FLASH_CFG_DEV_INSTR_WR_CONFIG_REG_ADDR_XFER_TYPE_STD_MODE_FLD_SHIFT) | \
              (mode << CSL_OSPI_FLASH_CFG_DEV_INSTR_WR_CONFIG_REG_DATA_XFER_TYPE_EXT_MODE_FLD_SHIFT);
    LLD_REG32_WR(&pRegs->DEV_INSTR_WR_CONFIG_REG, regVal);
}

void CSL_ospiWriteFifoData(uintptr_t indAddr, const uint8 *src, uint32 wrLen)
{
    uint32 temp;
    uint32 remaining = wrLen;
    uint32 *srcPtr = (uint32 *)src;

    while (remaining > 0U)
    {
        if (remaining >= CSL_OSPI_FIFO_WIDTH)
        {
            LLD_REG32_WR(indAddr, *srcPtr);
            remaining -= CSL_OSPI_FIFO_WIDTH;
        }
        else
        {
            /* dangling bytes */
            memcpy(&temp, srcPtr, remaining);
            LLD_REG32_WR(indAddr, temp);
            break;
        }
        srcPtr++;
    }
}

void CSL_ospiClrIndWriteComplete(const CSL_ospi_flash_cfgRegs *pRegs)
{
    LLD_REG32_FINS(&pRegs->INDIRECT_WRITE_XFER_CTRL_REG,
                   OSPI_FLASH_CFG_INDIRECT_WRITE_XFER_CTRL_REG_IND_OPS_DONE_STATUS_FLD,
                   1);
}

/* FLS_CoverageGap_04:Dynamic coverage for the function cannot be covered,
 * as it expects a certain value from hardware register.
 */
uint32 CSL_ospiIsIndWriteComplete(const CSL_ospi_flash_cfgRegs *pRegs)
{
    uint32 val;
    uint32 retVal;

    /* Check flash indirect read controller status */
    val = LLD_REG32_FEXT(&pRegs->INDIRECT_WRITE_XFER_CTRL_REG,
                         OSPI_FLASH_CFG_INDIRECT_WRITE_XFER_CTRL_REG_IND_OPS_DONE_STATUS_FLD);
    if (val == 1U)
    {
        retVal = TRUE;
    }
    else
    {
        retVal = FALSE;
    }

    return(retVal);
}


void CSL_ospiIndWriteExecute(const CSL_ospi_flash_cfgRegs *pRegs,
                             uint32                      txLen)
{
    /* Set the Indirect Write Transfer Start Address Register */
    LLD_REG32_WR(&pRegs->INDIRECT_WRITE_XFER_NUM_BYTES_REG, txLen);

    /* Reset watermark register */
    LLD_REG32_WR(&pRegs->INDIRECT_WRITE_XFER_WATERMARK_REG, 0);

    /* Set the Indirect Write Transfer Watermark Register */
    LLD_REG32_WR(&pRegs->INDIRECT_WRITE_XFER_WATERMARK_REG,
                 CSL_OSPI_SRAM_WATERMARK_WR_LVL);

    /* Start the indirect write transfer */
    LLD_REG32_FINS(&pRegs->INDIRECT_WRITE_XFER_CTRL_REG,
                   OSPI_FLASH_CFG_INDIRECT_WRITE_XFER_CTRL_REG_START_FLD,
                   1);
}

void CSL_ospiIndWriteCancel(const CSL_ospi_flash_cfgRegs *pRegs)
{
    /* Cancel the indirect write */
    LLD_REG32_FINS(&pRegs->INDIRECT_WRITE_XFER_CTRL_REG,
                   OSPI_FLASH_CFG_INDIRECT_WRITE_XFER_CTRL_REG_CANCEL_FLD,
                   1);
}

void CSL_ospiLoopbackClkEnable(const CSL_ospi_flash_cfgRegs *pRegs,
                               uint32                      enable)
{
     uint32 enVal = 0U;
     if(enable != 0U)
     {
       enVal=0U;
     }
     else
     {
       enVal=1U;
     }

    LLD_REG32_FINS(&pRegs->RD_DATA_CAPTURE_REG,
                   OSPI_FLASH_CFG_RD_DATA_CAPTURE_REG_BYPASS_FLD,
                   enVal);
}

uint32 CSL_ospiIsIdle(const CSL_ospi_flash_cfgRegs *pRegs)
{
    uint32 status;

    /* Read STATUS read-only bit */
    status = LLD_REG32_FEXT(&pRegs->CONFIG_REG,
                            OSPI_FLASH_CFG_CONFIG_REG_IDLE_FLD);
    return (status);
}
/* FLS_CoverageGap_05:Dynamic coverage for CSL_ospiPhyResyncDll() function cannot be covered,
 * as it is a static function.
 */
static void CSL_ospiPhyResyncDll(const CSL_ospi_flash_cfgRegs *pRegs)
{
    /* Wait for Idle */
    while(CSL_ospiIsIdle(pRegs) == 0U)
    {

    }


    /* Disable the controller */
    CSL_ospiEnable(pRegs, FALSE);

    /* Clear the delay line resync bit */
    LLD_REG32_FINS(&pRegs->PHY_CONFIGURATION_REG,
                   OSPI_FLASH_CFG_PHY_CONFIGURATION_REG_PHY_CONFIG_RESYNC_FLD,
                   0U);

    /* Reset DLL in master mode */
    LLD_REG32_FINS(&pRegs->PHY_CONFIGURATION_REG,
                   OSPI_FLASH_CFG_PHY_CONFIGURATION_REG_PHY_CONFIG_RESET_FLD,
                   0U);

    /* Set Initial delay for the master DLL */
    LLD_REG32_FINS(&pRegs->PHY_MASTER_CONTROL_REG,
                   OSPI_FLASH_CFG_PHY_MASTER_CONTROL_REG_PHY_MASTER_INITIAL_DELAY_FLD,
                   0x10U);

    /* DLL out of reset */
    LLD_REG32_FINS(&pRegs->PHY_CONFIGURATION_REG,
                   OSPI_FLASH_CFG_PHY_CONFIGURATION_REG_PHY_CONFIG_RESET_FLD,
                   1U);

    /* Wait DLL lock done */
    while (LLD_REG32_FEXT(&pRegs->DLL_OBSERVABLE_LOWER_REG,
           OSPI_FLASH_CFG_DLL_OBSERVABLE_LOWER_REG_DLL_OBSERVABLE_LOWER_DLL_LOCK_FLD) == 0U)
           {

           }

    /* Wait DLL loopback lock done */
    while (LLD_REG32_FEXT(&pRegs->DLL_OBSERVABLE_LOWER_REG,
           OSPI_FLASH_CFG_DLL_OBSERVABLE_LOWER_REG_DLL_OBSERVABLE_LOWER_LOOPBACK_LOCK_FLD) == 0U)
           {

           }

    /* Resync the Slave DLLs */
    LLD_REG32_FINS(&pRegs->PHY_CONFIGURATION_REG,
                   OSPI_FLASH_CFG_PHY_CONFIGURATION_REG_PHY_CONFIG_RESYNC_FLD,
                   1U);

    /* Enable the controller */
    CSL_ospiEnable(pRegs, TRUE);
}

/* FLS_CoverageGap_06:Dynamic coverage for this function cannot be covered,
 * as it is never used in the driver code.
 */
void CSL_ospiConfigPhy(const CSL_ospi_flash_cfgRegs *pRegs,
                       uint32                      masterDelay,
                       const uint32               *pSlaveDelay)
{
    uint32                  twentyFivePercent;
    uint32                 rxDelay;
    uint32                 dtr;
    uint32                 dqsEnable;

    if (pSlaveDelay != NULL)
    {
        /* Set the initial DLL delay */

        /* Enable PHY */
        CSL_ospiPhyEnable(pRegs, TRUE);

        /* Reset DLL */
        LLD_REG32_FINS(&pRegs->PHY_CONFIGURATION_REG,
                       OSPI_FLASH_CFG_PHY_CONFIGURATION_REG_PHY_CONFIG_RESET_FLD,
                       0U);

        /* Configure sample edge */
        dtr = LLD_REG32_FEXT(&pRegs->CONFIG_REG,
                             OSPI_FLASH_CFG_CONFIG_REG_ENABLE_DTR_PROTOCOL_FLD);
        if (dtr == 1U)
        {
            dqsEnable  = 1U; /* DQS enabled */
        }
        else
        {
            dqsEnable  = 0U; /* DQS disabled */
        }
        /* sampled on rising edge of the ref_clk */
        LLD_REG32_FINS(&pRegs->RD_DATA_CAPTURE_REG,
                       OSPI_FLASH_CFG_RD_DATA_CAPTURE_REG_SAMPLE_EDGE_SEL_FLD,
                       CSL_OSPI_FLASH_CFG_RD_DATA_CAPTURE_REG_SAMPLE_EDGE_SEL_FLD_MAX);

        LLD_REG32_FINS(&pRegs->RD_DATA_CAPTURE_REG,
                       OSPI_FLASH_CFG_RD_DATA_CAPTURE_REG_DQS_ENABLE_FLD,
                       dqsEnable);

        twentyFivePercent = masterDelay / 4U; /* TX DLL delay, 25% of masterDelay */

        /* Set Initial delay for the master DLL */
        LLD_REG32_FINS(&pRegs->PHY_MASTER_CONTROL_REG,
                       OSPI_FLASH_CFG_PHY_MASTER_CONTROL_REG_PHY_MASTER_INITIAL_DELAY_FLD,
                       4U);

        /* Set TX DLL delay */
        LLD_REG32_FINS(&pRegs->PHY_CONFIGURATION_REG,
                       OSPI_FLASH_CFG_PHY_CONFIGURATION_REG_PHY_CONFIG_TX_DLL_DELAY_FLD,
                       twentyFivePercent);

        /* Re-sync DLL */
        CSL_ospiPhyResyncDll(pRegs);

        /* DLL out of reset */
        LLD_REG32_FINS(&pRegs->PHY_CONFIGURATION_REG,
                       OSPI_FLASH_CFG_PHY_CONFIGURATION_REG_PHY_CONFIG_RESET_FLD,
                       1U);

        /* Wait DLL lock done */
        while (LLD_REG32_FEXT(&pRegs->DLL_OBSERVABLE_LOWER_REG,
               OSPI_FLASH_CFG_DLL_OBSERVABLE_LOWER_REG_DLL_OBSERVABLE_LOWER_LOOPBACK_LOCK_FLD) == 0U)
               {

               }

        /* Re-sync slave DLLs */
        twentyFivePercent = ((127U/4U) + LLD_REG32_RD(pSlaveDelay));

        /* Set TX DLL delay */
        LLD_REG32_FINS(&pRegs->PHY_CONFIGURATION_REG,
                       OSPI_FLASH_CFG_PHY_CONFIGURATION_REG_PHY_CONFIG_TX_DLL_DELAY_FLD,
                       twentyFivePercent);
        /* Set RX DLL delay */
        LLD_REG32_FINS(&pRegs->PHY_CONFIGURATION_REG,
                       OSPI_FLASH_CFG_PHY_CONFIGURATION_REG_PHY_CONFIG_RX_DLL_DELAY_FLD,
                       twentyFivePercent);
        /* Re-sync DLL */
        CSL_ospiPhyResyncDll(pRegs);
    }
    else
    {
        /* Calibrate PHY */
        rxDelay = LLD_REG32_FEXT(&pRegs->PHY_CONFIGURATION_REG,
                                 OSPI_FLASH_CFG_PHY_CONFIGURATION_REG_PHY_CONFIG_RX_DLL_DELAY_FLD);
        if (rxDelay == 127U)
        {
            rxDelay = 0U;
        }
        else
        {
            rxDelay++;
        }

        /* Set TX DLL delay */
        LLD_REG32_FINS(&pRegs->PHY_CONFIGURATION_REG,
                       OSPI_FLASH_CFG_PHY_CONFIGURATION_REG_PHY_CONFIG_TX_DLL_DELAY_FLD,
                       rxDelay);
        /* Set RX DLL delay */
        LLD_REG32_FINS(&pRegs->PHY_CONFIGURATION_REG,
                       OSPI_FLASH_CFG_PHY_CONFIGURATION_REG_PHY_CONFIG_RX_DLL_DELAY_FLD,
                       rxDelay);
        /* Re-sync DLL */
        CSL_ospiPhyResyncDll(pRegs);
    }
}

void CSL_ospiFlashStig(const CSL_ospi_flash_cfgRegs *pRegs,
                       uint32                      cmd,
                       uint32                      addr,
                       uint32                      data)
{
    LLD_REG32_WR(&pRegs->FLASH_CMD_ADDR_REG, addr);
    LLD_REG32_WR(&pRegs->FLASH_WR_DATA_LOWER_REG, data);
    LLD_REG32_WR(&pRegs->FLASH_CMD_CTRL_REG, cmd);
}

void CSL_ospiIndSetStartAddr(const CSL_ospi_flash_cfgRegs *pRegs,
                             uint32                      startAddr,
                             uint32                      read)
{
    if (read == TRUE)
    {
        /* Config the Indirect Read Transfer Start Address Register */
        LLD_REG32_WR(&pRegs->INDIRECT_READ_XFER_START_REG, startAddr);
    }
    else
    {
        /* Configure the Indirect Write Transfer Start Address Register */
        LLD_REG32_WR(&pRegs->INDIRECT_WRITE_XFER_START_REG, startAddr);
    }
}

void CSL_ospiSetDataReadCapDelay(const CSL_ospi_flash_cfgRegs *pRegs,
                                 uint32                      delay)
{
    /* Set delay in ref_clk cycle for data read capture */
    LLD_REG32_FINS(&pRegs->RD_DATA_CAPTURE_REG,
                   OSPI_FLASH_CFG_RD_DATA_CAPTURE_REG_DELAY_FLD,
                   delay);
}

void CSL_ospiSetCsSotDelay(const CSL_ospi_flash_cfgRegs *pRegs,
                           uint32                      delay)
{
    /* Set Chip Select Start of Transfer delay in ref_clk cycle */
    LLD_REG32_FINS(&pRegs->DEV_DELAY_REG,
                   OSPI_FLASH_CFG_DEV_DELAY_REG_D_INIT_FLD,
                   delay);
}

void CSL_ospiSetDualByteOpcodeMode(const CSL_ospi_flash_cfgRegs *pRegs,
                                   uint32                      opcodeMode)
{
    LLD_REG32_FINS(&pRegs->CONFIG_REG,
                   OSPI_FLASH_CFG_CONFIG_REG_DUAL_BYTE_OPCODE_EN_FLD,
                   opcodeMode);
}

uint32 CSL_ospiGetDualByteOpcodeMode(const CSL_ospi_flash_cfgRegs *pRegs)
{
    uint32    opcodeMode;

    opcodeMode = LLD_REG32_FEXT(&pRegs->CONFIG_REG,
                                 OSPI_FLASH_CFG_CONFIG_REG_DUAL_BYTE_OPCODE_EN_FLD);
    return opcodeMode;
}

void CSL_ospiExtOpcodeSet(const CSL_ospi_flash_cfgRegs *pRegs,
                          uint32                      opcodeLo,
                          uint32                      opcodeUp)
{
    LLD_REG32_WR(&pRegs->OPCODE_EXT_LOWER_REG, opcodeLo);
    LLD_REG32_WR(&pRegs->OPCODE_EXT_UPPER_REG, opcodeUp);
}

void CSL_ospiExtOpcodeGet(const CSL_ospi_flash_cfgRegs *pRegs,
                          uint32                     *opcodeLo,
                          uint32                     *opcodeUp)
{
    *opcodeLo = LLD_REG32_RD(&pRegs->OPCODE_EXT_LOWER_REG);
    *opcodeUp = LLD_REG32_RD(&pRegs->OPCODE_EXT_UPPER_REG);
}

/* FLS_CoverageGap_06:Dynamic coverage for this function cannot be covered,
 * as it is never used in the driver code.
 */
void CSL_ospiConfigPhyDLL(const CSL_ospi_flash_cfgRegs *pRegs,
                          uint32                      txDelay,
                          uint32                      rxDelay,
                          uint32                      opMode,
                          uint32                      funcClk)
{
    uint32                 dtr;
    uint32                 dqsEnable;

    /* Configure sample edge and dqs */
    dtr = LLD_REG32_FEXT(&pRegs->CONFIG_REG,
                         OSPI_FLASH_CFG_CONFIG_REG_ENABLE_DTR_PROTOCOL_FLD);
    if (dtr == 1U)
    {
        dqsEnable  = 1U; /* DQS enabled */
    }
    else
    {
        dqsEnable  = 0U; /* DQS disabled */
    }
    /* sampled on rising edge of the ref_clk */
    LLD_REG32_FINS(&pRegs->RD_DATA_CAPTURE_REG,
                   OSPI_FLASH_CFG_RD_DATA_CAPTURE_REG_SAMPLE_EDGE_SEL_FLD,
                   CSL_OSPI_FLASH_CFG_RD_DATA_CAPTURE_REG_SAMPLE_EDGE_SEL_FLD_MAX);

    LLD_REG32_FINS(&pRegs->RD_DATA_CAPTURE_REG,
                   OSPI_FLASH_CFG_RD_DATA_CAPTURE_REG_DQS_ENABLE_FLD,
                   dqsEnable);
    /* PHY DLL master operational mode */
    if(opMode == CSL_OSPI_CFG_PHY_OP_MODE_MASTER)
    {
        /* Configure PHY in Master operational mode */
        LLD_REG32_FINS(&pRegs->PHY_MASTER_CONTROL_REG,
                    OSPI_FLASH_CFG_PHY_MASTER_CONTROL_REG_PHY_MASTER_BYPASS_MODE_FLD,
                    CSL_OSPI_FLASH_CFG_PHY_MASTER_CONTROL_REG_PHY_MASTER_MODE);
    }
    /* PHY DLL master operational mode */
    else if(opMode == CSL_OSPI_CFG_PHY_OP_MODE_BYPASS)
    {
        /* Configure PHY in Bypass operational mode */
        LLD_REG32_FINS(&pRegs->PHY_MASTER_CONTROL_REG,
                    OSPI_FLASH_CFG_PHY_MASTER_CONTROL_REG_PHY_MASTER_BYPASS_MODE_FLD,
                    CSL_OSPI_FLASH_CFG_PHY_MASTER_CONTROL_REG_PHY_BYPASS_MODE);
    }
    else /* default mode config */
    {
        if(funcClk == 166666666U)
        {
            /* Master operational mode for OSPI clock frequency of 166MHz */
            LLD_REG32_FINS(&pRegs->PHY_MASTER_CONTROL_REG,
                        OSPI_FLASH_CFG_PHY_MASTER_CONTROL_REG_PHY_MASTER_BYPASS_MODE_FLD,
                        0);
        }
        else
        {
            /* Bypass mode for OSPI clock frequencies less than 166MHz */
            LLD_REG32_FINS(&pRegs->PHY_MASTER_CONTROL_REG,
                        OSPI_FLASH_CFG_PHY_MASTER_CONTROL_REG_PHY_MASTER_BYPASS_MODE_FLD,
                        1);
        }
    }
    
    /* Set TX DLL delay */
    LLD_REG32_FINS(&pRegs->PHY_CONFIGURATION_REG,
                   OSPI_FLASH_CFG_PHY_CONFIGURATION_REG_PHY_CONFIG_TX_DLL_DELAY_FLD,
                   txDelay);
    /* Set RX DLL delay */
    LLD_REG32_FINS(&pRegs->PHY_CONFIGURATION_REG,
                   OSPI_FLASH_CFG_PHY_CONFIGURATION_REG_PHY_CONFIG_RX_DLL_DELAY_FLD,
                   rxDelay);
    /* Re-sync DLL */
    CSL_ospiPhyResyncDll(pRegs);
}
