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
 *  \file   Fls_OSPI.c
 *
 *  \brief  OSPI  specific driver APIs implementation.
 *
 *   This file contains the driver APIs for OSPI controller.
 */


 /* ========================================================================== */
 /*                             Include Files                                  */
 /* ========================================================================== */
#include "Std_Types.h"
#include "stdint.h"
#include <hw_include/arch/lld_arch.h>
#include "Fls_Ospi.h"
#include "Fls_Brd_Nor.h"
#include "Fls_Cfg.h"
#include <SchM_Fls.h>
#include "Os.h"



/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */


#define OSPI_DAC_WRITE_TIMEOUT      (500000U)
#define OSPI_CHECK_IDLE_DELAY       (10U)
#define OSPI_CALIBRATE_DELAY        (20U)
#define OSPI_XIP_SETUP_DELAY        (250U)

#define OSPI_PHY_DLL_ELEM_DELAY_PERIOD   (80U)  /* worst delay element periord in ps */

#define SPI_OSPI_PER_CNT (1U)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

typedef enum OSPI_intrPollMode_s
{
    SPI_OPER_MODE_POLLING,
    /**< Non interrupt based blocking mode */
    SPI_OPER_MODE_CALLBACK
    /**< Interrupt based call back mode */
} OSPI_intrPollMode;

/* OSPI objects */
OSPI_Object OspiObjects[SPI_OSPI_PER_CNT + 1U];

/* Default SPI parameters structure */
const SPI_Params SPI_defaultParams = {
    SPI_MODE_CALLBACK,  /* transferMode */
    0U,                 /* transferTimeout */
    SPI_MASTER,         /* mode */
    1000000,            /* bitRate */
    8,                  /* dataSize */
    SPI_POL0_PHA0,      /* frameFormat */
    0               /* custom */
};

/* SPI configuration structure */
CSL_PUBLIC_CONST SPI_config_list SPI_config = {
    {
        &OspiObjects[0]
    },
    {
        NULL
    }
};
uint32 devDelays[4]={0,0,0,0x2};
/**< OSPI device delays (CSSOT, CSEOT, CSDADS and CSDA delays) */
boolean tuneDone;
/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

static Std_ReturnType    OSPI_primeTransfer(SPI_Handle handle,
                                        SPI_Transaction *transaction);
static Std_ReturnType    OSPI_waitIdle(SPI_Handle handle, uint32 timeOut);
static Std_ReturnType    OSPI_flashExecCmd(const CSL_ospi_flash_cfgRegs *pRegs);
static Std_ReturnType    OSPI_read(SPI_Handle handle, SPI_Transaction *transaction);
static Std_ReturnType    OSPI_cmdRead(const CSL_ospi_flash_cfgRegs *pRegs,
                               uint32                      cmd,
                               uint8                      *rxBuf,
                               uint32                      rxLen);
static Std_ReturnType    OSPI_cmdExtRead(const CSL_ospi_flash_cfgRegs *pRegs,
                              uint8                      *cmd,
                              uint32                      cmdLen,
                              uint8                      *rxBuf,
                              uint32                      rxLen,
                              uint32                      dummyCycles);
static Std_ReturnType    OSPI_write(SPI_Handle handle, SPI_Transaction *transaction);
static Std_ReturnType    OSPI_cmdWrite(const CSL_ospi_flash_cfgRegs *pRegs,
                                const uint8                *cmdBuf,
                                uint32                      cmdLen,
                                const uint8                *txBuf,
                                uint32                      txLen);
static Std_ReturnType    OSPI_waitReadSramLvl(const CSL_ospi_flash_cfgRegs *pReg,
                                       uint32 *rdLevel);
static Std_ReturnType    OSPI_ind_xfer_mode_read(SPI_Handle handle,
                                             SPI_Transaction *transaction);
static Std_ReturnType    OSPI_waitIndWriteComplete(const CSL_ospi_flash_cfgRegs *pRegs);
static Std_ReturnType    OSPI_waitWriteSramLvl(const CSL_ospi_flash_cfgRegs *pReg,
                                        uint32 *sramLvl);
static Std_ReturnType    OSPI_ind_xfer_mode_write(SPI_Handle handle,
                                              SPI_Transaction *transaction);
static Std_ReturnType    OSPI_dac_xfer_mode_read(SPI_Handle handle,
                                             const SPI_Transaction *transaction);
static Std_ReturnType    OSPI_dac_xfer_mode_write(SPI_Handle handle,
                                              const SPI_Transaction *transaction);
static Std_ReturnType    OSPI_cmd_mode_write(SPI_Handle handle,
                                         const SPI_Transaction *transaction);
static Std_ReturnType    OSPI_cmd_mode_read(SPI_Handle handle,
                                        SPI_Transaction *transaction);
//static void   OSPI_configPhy(SPI_Handle handle, const void *pDelay);
static Std_ReturnType OSPI_configDdr(SPI_Handle handle, uint32 cmd, uint32 addr, uint32 data);
static boolean OSPI_waitDeviceReady(SPI_Handle handle, uint32 timeOut);
static void Fls_delay(uint32 msecs);

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */


#define FLS_START_SEC_CODE
#include "Fls_MemMap.h"

static void OSPI_delay(uint32 delay);
static void OSPI_delay(uint32 delay)
{
    volatile uint32 i = delay;

    while (i > 0U)
    {
        i = i - 1U;
    }
}

/*FLS_CoverageGap_07: Dynamic coverage for the function cannot be achieved as these functions are static.*/
static void Fls_delay(uint32 msecs)
{
    TickType count = 1000000U * msecs;
    TickType start = 0U;
    TickType tempCount = 0U;
    TickType elapsed = 0U;
    StatusType status;

    status = GetCounterValue(FLS_OS_COUNTER_ID, &start);

    while ((E_OK == status) && (count >= elapsed))
    {
        tempCount = start;
        status = GetElapsedValue(FLS_OS_COUNTER_ID, &tempCount, &elapsed);
    }
}
/*
 *  ======== OSPI_hwi ========
 *  Hwi interrupt handler to service the OSPI peripheral
 *
 *  The handler is a generic handler for a OSPI object.
 */
void OSPI_hwi(SPI_Handle handle)
{
    uint32               intrStatus;
    OSPI_Object        *object =(OSPI_Object *)NULL;
    uint32               sramLevel = 0, rdBytes = 0, wrBytes = 0;

    /* Get the pointer to the object and hwAttrs */
    object = (OSPI_Object *)(handle->object);

    /* Read the interrupt status register */
    intrStatus = CSL_ospiIntrStatus((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR));

    if (object->readBufIdx != NULL_PTR)
    {
		/* Indirect read operation */
        if ((intrStatus & CSL_OSPI_INTR_MASK_IND_XFER) != 0U)
        {
            if(object->readCountIdx != 0U)
            {
                while ((boolean)TRUE != 0U)
                {
                    sramLevel = CSL_ospiGetSramLvl((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), 1U);
                    if (sramLevel == 0U)
                    {
                        break;
                    }
                    rdBytes = sramLevel * CSL_OSPI_FIFO_WIDTH;
                    rdBytes = (rdBytes > (uint32)object->readCountIdx) ? (uint32)object->readCountIdx : rdBytes;
                    /* Read data from FIFO */
                    CSL_ospiReadFifoData((Fls_DrvObj.sectorList[0].sectorStartaddress), (uint8 *)object->readBufIdx, rdBytes);

                    object->readBufIdx += rdBytes;
                    object->readCountIdx -= rdBytes;

                }

                if (((uint32)object->readCountIdx > 0U) &&
                    ((uint32)object->readCountIdx < (CSL_OSPI_SRAM_WARERMARK_RD_LVL * CSL_OSPI_FIFO_WIDTH)))
                {

                    while((boolean)TRUE != 0U)
                    {
                        sramLevel = CSL_ospiGetSramLvl((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), 1U);
                        rdBytes = sramLevel * CSL_OSPI_FIFO_WIDTH;
                        if (rdBytes >= object->readCountIdx)
                        {
                            break;
                        }
                    }
                    rdBytes = object->readCountIdx;
                    CSL_ospiReadFifoData((Fls_DrvObj.sectorList[0].sectorStartaddress), object->readBufIdx, rdBytes);
                    object->readBufIdx += rdBytes;
                    object->readCountIdx -= rdBytes;
                }
            }

            if((object->readCountIdx == 0U) || ((intrStatus & CSL_OSPI_INTR_MASK_IND_OP_DONE) != 0U))
            {
                /* Clear indirect read operation complete status */
                CSL_ospiClrIndReadComplete((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR));

                /* disable and clear the interrupts */
                CSL_ospiIntrEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                                    CSL_OSPI_INTR_MASK_ALL,
                                    FALSE);
                CSL_ospiIntrClear((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                                  CSL_OSPI_INTR_MASK_ALL);

                object->transaction = NULL;

                if (Fls_DrvObj.length != 0U)
                {
                    processJobs(Fls_DrvObj.jobType);
                }
            }
            else
            {
                /* Clear interrupt status */
                CSL_ospiIntrClear((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), intrStatus);
            }
        }
    }
    else
    {
		/* Indirect write operation */
        if ((intrStatus & CSL_OSPI_INTR_MASK_IND_XFER) != 0U)
        {
            if (object->writeCountIdx != 0U)
            {
                sramLevel = CSL_OSPI_SRAM_PARTITION_WR - \
                            CSL_ospiGetSramLvl((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), 0U);

                wrBytes = sramLevel * CSL_OSPI_FIFO_WIDTH;
                wrBytes = (wrBytes > object->writeCountIdx) ? object->writeCountIdx : wrBytes;

                /* Write data to FIFO */
                CSL_ospiWriteFifoData((Fls_DrvObj.sectorList[0].sectorStartaddress), object->writeBufIdx, wrBytes);

                object->writeBufIdx += wrBytes;
                object->writeCountIdx -= wrBytes;

                sramLevel = CSL_OSPI_SRAM_PARTITION_WR - \
                            CSL_ospiGetSramLvl((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), 0U);

                if ((object->writeCountIdx > 0U) &&
                    (object->writeCountIdx <= (sramLevel * CSL_OSPI_FIFO_WIDTH)))
                {
                    wrBytes = object->writeCountIdx;
                    CSL_ospiWriteFifoData((Fls_DrvObj.sectorList[0].sectorStartaddress), object->writeBufIdx, wrBytes);
                    object->writeBufIdx += wrBytes;
                    object->writeCountIdx -= wrBytes;
                }
            }

            if ((intrStatus & CSL_OSPI_INTR_MASK_IND_OP_DONE) != 0U)
            {
                /* Clear indirect write operation complete status */
                CSL_ospiClrIndWriteComplete((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR));

                /* disable and clear the interrupts */
                CSL_ospiIntrEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                                    CSL_OSPI_INTR_MASK_ALL,
                                    FALSE);
                CSL_ospiIntrClear((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                                  CSL_OSPI_INTR_MASK_ALL);
                object->transaction = NULL;

                if (Fls_DrvObj.length != 0U)
                {
                    processJobs(Fls_DrvObj.jobType);
                }
            }
            else
            {
                /* Clear interrupt status */
                CSL_ospiIntrClear((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), intrStatus);
            }
        }
    }
}

/*
 *  ======== OSPI_close ========
 */
void Fls_OSPI_close(SPI_Handle handle)
{
    OSPI_Object        *object = (OSPI_Object *)NULL;

    if (handle != NULL)
    {
        /* Get the pointer to the object and hwAttrs */
        object = (OSPI_Object *)handle->object;

        /* disable the interrupts */
        CSL_ospiIntrEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                        CSL_OSPI_INTR_MASK_ALL,
                        FALSE);

        /* Open flag is set FALSE */
        object->isOpen = FALSE;
    }

    return;
}

/*
 *  ======== OSPI_init ========
 */

void Fls_OSPI_init(SPI_Handle handle)
{
    if (handle != NULL)
    {
        /* Mark the object as available */
        OSPI_Object *ospi_Obj;
        ospi_Obj = (OSPI_Object *)(handle->object);
        ospi_Obj->isOpen = FALSE;
    }
}

static Std_ReturnType OSPI_waitIdle(SPI_Handle handle, uint32 timeOut)
{
    uint32               retry = 0U;
    Std_ReturnType     retVal = E_NOT_OK;
    uint32               timeOutVal = timeOut;

    /* Get the pointer to the object and hwAttrs */

    while (timeOutVal != 0U)
    {
        if (CSL_ospiIsIdle((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR)) != 0U)
        {
            retry++;
            if (retry == 3U)
            {
                retVal = E_OK;
                break;
            }
        }
        else
        {
            retry = 0U;
        }
        OSPI_delay(OSPI_CHECK_IDLE_DELAY);
        timeOutVal--;
    }

    return (retVal);
}

/*
 *  ======== OSPI_open ========
 */
SPI_Handle Fls_OSPI_open(SPI_Handle handle, const SPI_Params *params)
{
    SPI_Handle               retHandle = handle;
    OSPI_Object              *object =(OSPI_Object *)NULL;
    sint32                    retFlag = 0;
    uint32                numAddrBytes;
	const CSL_ospi_flash_cfgRegs *pReg = (CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR);

    SchM_Enter_Fls_FLS_EXCLUSIVE_AREA_0();
    if (NULL == handle)
    {
        retHandle = NULL; /*if SPI handle is NULL, return NULL*/
    }
    else /*if handle != NULL*/
    {
		/*Setting default cfg and SDR mode*/
        CSL_ospiSetPreScaler(pReg, CSL_OSPI_BAUD_RATE_DIVISOR_DEFAULT);
        /* Disable dual byte opcode. If OSPI boot mode was used, ROM would have set this. This can cause 1s mode applications to fail */
        CSL_ospiSetDualByteOpcodeMode(pReg, FALSE);
        /* Delay Setup */
        LLD_REG32_WR(&pReg->DEV_DELAY_REG, OSPI_DELAY_ZERO);

        /* sampled on rising edge of the ref_clk */
        LLD_REG32_FINS(&pReg->RD_DATA_CAPTURE_REG,
                        OSPI_FLASH_CFG_RD_DATA_CAPTURE_REG_SAMPLE_EDGE_SEL_FLD,
                        0UL); 
        LLD_REG32_FINS(&pReg->RD_DATA_CAPTURE_REG,
                           OSPI_FLASH_CFG_RD_DATA_CAPTURE_REG_DELAY_FLD,
                           OSPI_RDDATA_CAPDELAY);
        CSL_ospiLoopbackClkEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
        					       FALSE);
        /*Set Transfer OpCodes for SDR mode*/    
        CSL_ospiConfigRead(pReg, NOR_CMD_READ, OSPI_XFER_LINES_SINGLE, NOR_SINGLE_READ_DUMMY_CYCLE);
        CSL_ospiWriteSetup(pReg,NOR_CMD_PAGE_PROG, OSPI_XFER_LINES_SINGLE);
		
        /* Get the pointer to the object and hwAttrs */
        object = (OSPI_Object *)handle->object;
        if (object->isOpen == TRUE)
        {
            retHandle = NULL;
        }
        else
        {
            /* Mark the handle as being used */
            object->isOpen = TRUE;
        }
    }
    SchM_Exit_Fls_FLS_EXCLUSIVE_AREA_0();

    if (retHandle != NULL)
    {
        SPI_Params_init(&(object->ospiParams)); /*Use default SPI params*/

        /* Copy the controller mode from hardware attributes to object */
        object->ospiMode  = (uint32)OSPI_OPER_MODE_CFG;
        object->xferLines = (uint32)Fls_DrvObj.ospixferlines;
        object->hwi       = NULL;

        /* Extract OSPI operating mode based on hwAttrs and input parameters */
#if (STD_ON == FLS_USE_INTERRUPTS)//callback interrupt mode
        object->intrPollMode = (uint32)SPI_OPER_MODE_CALLBACK;
#else
        object->intrPollMode = (uint32)SPI_OPER_MODE_POLLING;
#endif

        object->transaction = NULL;
        /* Disable DAC */
        CSL_ospiDacEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), FALSE);

        /* Disable DTR protocol */
        CSL_ospiDtrEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), FALSE);

        if (Fls_DrvObj.xipEnable == FALSE)
        {
            /* Disable XIP */
            CSL_ospiXipEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), FALSE);
        }

        /* Disable OSPI controller */
        CSL_ospiEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), FALSE);

        /* Wait until Serial Interface and OSPI pipeline is IDLE. */
        if (OSPI_waitIdle(handle, OSPI_DAC_WRITE_TIMEOUT) != E_OK)
        {
            Fls_OSPI_close(handle);
            retHandle = NULL;
            retFlag = 1;
        }

        if(retFlag == 0)
        {
            /* Chip select setup */
            CSL_ospiSetChipSelect((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                                  CSL_OSPI_CS0,
                                  CSL_OSPI_DECODER_SELECT4);

            /* Set clock mode */
            CSL_ospiSetClkMode((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                               CSL_OSPI_CLK_MODE_0);

			/* Set delay in ref_clk cycle for data read capture */
            LLD_REG32_FINS(&pReg->RD_DATA_CAPTURE_REG,
                   			OSPI_FLASH_CFG_RD_DATA_CAPTURE_REG_DELAY_FLD,
                   			1U);

            /* Delay Setup */
            CSL_ospiSetDevDelay((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                                devDelays);

            if (Fls_DrvObj.flsBaudRateDiv != (uint32)0)
            {
                CSL_ospiSetPreScaler((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                                     CSL_OSPI_BAUD_RATE_DIVISOR(Fls_DrvObj.flsBaudRateDiv));
            }
            else
            {
                /* Set default baud rate divider value */
                if (Fls_DrvObj.phyEnable == TRUE)
                {
                    CSL_ospiSetPreScaler((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                                         CSL_OSPI_BAUD_RATE_DIVISOR(2U));
                }
                else
                {
                    /* Disable high speed mode when PHY is disabled */
                    CSL_ospiSetPreScaler((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                                         CSL_OSPI_BAUD_RATE_DIVISOR_DEFAULT);
                }
            }

            if (Fls_DrvObj.phyEnable == TRUE)
            {
                /* Enable PHY mode */
                CSL_ospiPhyEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), TRUE);

            }
            else
            {
                /* Disable PHY mode */
                CSL_ospiPhyEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), FALSE);
            }

            /* Disable PHY pipeline mode */
            CSL_ospiPipelinePhyEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), FALSE);

            if (Fls_DrvObj.dtrEnable == TRUE)
            {
                numAddrBytes = CSL_OSPI_MEM_MAP_NUM_ADDR_BYTES_4;
            }
            else
            {
                numAddrBytes = CSL_OSPI_MEM_MAP_NUM_ADDR_BYTES_3;
            }

            /* Set device size cofigurations */
            CSL_ospiSetDevSize((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                               numAddrBytes,
                               Fls_DrvObj.sectorList[0].sectorPageSize,
                               Fls_DrvObj.blockSize);
							   
			/* Set indirect trigger address register */
            if (Fls_DrvObj.dacEnable == TRUE)
            {
                CSL_ospiSetIndTrigAddr((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                                       0x4000000);
            }
            else
            {
                CSL_ospiSetIndTrigAddr((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                                       0);
            }

            /* Disable write completion auto polling */
            CSL_ospiSetWrCompAutoPolling((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                                         CSL_OSPI_WRITE_COMP_AUTO_POLLING_ENABLE);

            /* Set SRAM partition configuration */
            CSL_ospiSetSramPartition((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                                     CSL_OSPI_SRAM_PARTITION_DEFAULT);

            /* disable and clear the interrupts */
            CSL_ospiIntrEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                                CSL_OSPI_INTR_MASK_ALL,
                                FALSE);
            CSL_ospiIntrClear((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                              CSL_OSPI_INTR_MASK_ALL);

            if (Fls_DrvObj.dacEnable == TRUE)
            {
                CSL_ospiDacEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), TRUE);
            }
            else
            {
                CSL_ospiDacEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), FALSE);
            }

            CSL_ospiSetDataReadCapDelay((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                                        OSPI_RDDATA_CAPDELAY);
            CSL_ospiSetCsSotDelay((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                                  OSPI_CSST_TRASDELAY);
            /* Enable OSPI controller */
            CSL_ospiEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), TRUE);
        }
    }

    return(retHandle);
}

/*
 *  ======== OSPI_primeTransfer =======
 */

/*FLS_CoverageGap_07: Dynamic coverage for the function cannot be achieved as these functions are static.*/
static Std_ReturnType OSPI_primeTransfer(SPI_Handle handle,
                                     SPI_Transaction *transaction)
{
    OSPI_Object        *object =(OSPI_Object *)NULL;
    Std_ReturnType      retVal = E_OK;

    /* Get the pointer to the object and hwAttrs */
    object = (OSPI_Object *)handle->object;

    /* Disable and clear the interrupts */
    CSL_ospiIntrEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                        CSL_OSPI_INTR_MASK_ALL,
                        FALSE);
    CSL_ospiIntrClear((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                      CSL_OSPI_INTR_MASK_ALL);

#if (STD_ON == FLS_USE_INTERRUPTS)
    CSL_ospiIntrEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                   CSL_OSPI_INTR_MASK_ALL,
                   TRUE);

#endif

    /* Identify the direction of transfer (whether read/write) */
    if((uint32)SPI_TRANSACTION_TYPE_READ == object->transactionType)
    {
        retVal = (OSPI_read(handle, transaction));
    }
    else if((uint32)SPI_TRANSACTION_TYPE_WRITE == object->transactionType)
    {
        retVal = (OSPI_write(handle, transaction));
    }
    else
    {
        transaction->status = SPI_TRANSFER_CANCELED;
		retVal = E_NOT_OK;
    }
    return(retVal);
}

/*FLS_CoverageGap_07: Dynamic coverage for the function cannot be achieved as these functions are static.*/
static Std_ReturnType OSPI_waitReadSramLvl(const CSL_ospi_flash_cfgRegs *pReg,
                                    uint32 *rdLevel)
{
    uint32 retry = OSPI_DAC_WRITE_TIMEOUT;
    uint32 sramLevel;
    Std_ReturnType  retVal;
    while(retry != 0U)
    {
        sramLevel = CSL_ospiGetSramLvl(pReg, 1U);
        if (sramLevel != 0U)
        {
            *rdLevel = sramLevel;
            break;
        }
        OSPI_delay(OSPI_CHECK_IDLE_DELAY);
        retry--;
    }

    if (retry != 0U)
    {
        retVal = E_OK;
    }
    else
    {
        retVal = E_NOT_OK;
    }
    return(retVal);
}

/*FLS_CoverageGap_07: Dynamic coverage for the function cannot be achieved as these functions are static.*/
static Std_ReturnType OSPI_waitIndReadComplete(const CSL_ospi_flash_cfgRegs *pRegs)
{
    uint32 retry = OSPI_DAC_WRITE_TIMEOUT;
    Std_ReturnType  retVal;

    /* Check flash indirect read controller status */
    while (retry != 0U)
    {
        if (CSL_ospiIndReadComplete(pRegs) == TRUE)
        {
            break;
        }
        OSPI_delay(OSPI_CHECK_IDLE_DELAY);
        retry--;
    }
    if (retry != 0U)
    {
        /* Clear indirect completion status */
        CSL_ospiClrIndReadComplete(pRegs);
        retVal = E_OK;
    }
    else
    {
        retVal = E_NOT_OK;
    }
    return(retVal);
}

/*FLS_CoverageGap_07: Dynamic coverage for the function cannot be achieved as these functions are static.*/
static Std_ReturnType OSPI_ind_xfer_mode_read(SPI_Handle handle,
                                          SPI_Transaction *transaction)
{
    OSPI_Object        *object;       /* OSPI object */
    uint8               *pDst;         /* Destination address */
    uint32               count;        /* transaction length */
    uint32               offset;       /* OSPI flash offset address */
    uint32               remaining;
    uint32               sramLevel = 0, rdBytes = 0;
    uint32               retFlag = E_OK;
    Std_ReturnType       retVal = E_OK;

    /* Copy flash transaction parameters to local variables */
    offset = (uint32)(transaction->arg); /* NOR Flash offset address to read */
    pDst = (uint8 *)transaction->rxBuf;
    count = (uint32)transaction->count;

    /* Get the pointer to the object and hwAttrs */
    object = (OSPI_Object *)handle->object;

    /* Disable DAC mode */
    CSL_ospiDacEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), FALSE);

    if (Fls_DrvObj.phyEnable == TRUE)
    {
        /* Enable PHY pipeline mode for read */
        CSL_ospiPipelinePhyEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), TRUE);
    }

    /* Set read address in indirect mode */
    CSL_ospiIndSetStartAddr((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                            offset,
                            TRUE);
    CSL_ospiIndReadExecute((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), count);
    if ((uint32)SPI_OPER_MODE_POLLING == object->intrPollMode)
    {
        remaining = count;
        while(remaining > 0U)
        {
            /* Wait indirect read SRAM fifo has data */
            if (OSPI_waitReadSramLvl((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                                     &sramLevel) != E_OK)
            {
                retFlag = 1U;
                retVal = E_NOT_OK;
                transaction->status=SPI_TRANSFER_FAILED;
                break;
            }

            rdBytes = sramLevel * CSL_OSPI_FIFO_WIDTH;
            rdBytes = (rdBytes > remaining) ? remaining : rdBytes;
            /* Read data from FIFO */
            CSL_ospiReadFifoData((Fls_DrvObj.sectorList[0].sectorStartaddress), pDst, rdBytes);

            pDst += rdBytes;
            remaining -= rdBytes;

        }
        if(retFlag == 0U)
        {
            if (OSPI_waitIndReadComplete((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR)) != E_OK)
            {
                retVal = E_NOT_OK;
                transaction->status=SPI_TRANSFER_FAILED;
            }
        }
    }
#if defined (aarch64) || ((__ARM_ARCH == 7) && (__ARM_ARCH_PROFILE == R))
    CSL_archMemoryFence();
#endif

    return (retVal);
}

static uint32 OSPI_getDeviceStatus(SPI_Handle handle);
static uint32 OSPI_getDeviceStatus(SPI_Handle handle)
{
    OSPI_Object        *object;  /* OSPI object */
    uint8                status = 0xff;

    object = (OSPI_Object *)handle->object;

    (void)OSPI_cmdRead((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                       object->rdStatusCmd,
                       &status,
                       1);

    return (status);
}

/*FLS_CoverageGap_07: Dynamic coverage for the function cannot be achieved as these functions are static.*/
static boolean OSPI_waitDeviceReady(SPI_Handle handle, uint32 timeOut)
{
    boolean         retVal = FALSE;
    uint32      status;
    uint32     timeOutVal = timeOut;

    while (timeOutVal != 0U)
    {
        status = OSPI_getDeviceStatus(handle);
        if ((status & 1U) == 0U)
        {
            retVal = TRUE;
            break;
        }
        timeOutVal--;
        OSPI_delay(OSPI_CHECK_IDLE_DELAY);
    }

    return (retVal);
}

/*FLS_CoverageGap_07: Dynamic coverage for the function cannot be achieved as these functions are static.*/
static Std_ReturnType OSPI_dac_xfer_mode_read(SPI_Handle handle,
                                          const SPI_Transaction *transaction)
{
    uint8               *pSrc;         /* Source address */
    uint8               *pDst;         /* Destination address */
    uint32               offset;       /* OSPI flash offset address */
    uint32               i;
    uint32               size;
    uint32               remainSize;

    /* Copy flash transaction parameters to local variables */
    offset = (uint32)(transaction->arg); /* OSPI Flash offset address to read */
    pDst = (uint8 *)transaction->rxBuf;

    /* Get the pointer to the object and hwAttrs */

    /* Disable PHY pipeline mode for read since we are not using DMA*/
    CSL_ospiPipelinePhyEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), FALSE);

    pSrc = (uint8 *)((Fls_DrvObj.sectorList[0].sectorStartaddress) + offset);
    remainSize = (uint32)transaction->count & 3U;
    size = (uint32)transaction->count - remainSize;
    /* Transfer the data in 32-bit size */
    for (i = 0U; i < size; i += 4U)
    {
        LLD_REG32_WR(pDst + i, LLD_REG32_RD(pSrc + i));
#if defined (__aarch64__)
        CSL_archMemoryFence();
#endif
    }
    /* Transfer the remaining data in 8-bit size */
    for (i = 0; i < remainSize; i++)
    {
        LLD_REG8_WR(pDst + size + i, LLD_REG8_RD(pSrc + size + i));
#if defined (__aarch64__)
        CSL_archMemoryFence();
#endif
    }

    return E_OK;
}

/*FLS_CoverageGap_07: Dynamic coverage for the function cannot be achieved as these functions are static.*/
static Std_ReturnType OSPI_flashExecCmd(const CSL_ospi_flash_cfgRegs *pRegs)
{
    uint32 retry = OSPI_DAC_WRITE_TIMEOUT;
    Std_ReturnType  retVal = E_OK;
    
    uint32 startcount= 0U, elapsedcount =0U;
    uint32 tempCount = 0U;

    GetCounterValue(FLS_OS_COUNTER_ID, &startcount);
  	do
  	{
        tempCount = startcount;
  		GetElapsedValue(FLS_OS_COUNTER_ID, &tempCount, & elapsedcount);
        
  		if(elapsedcount >= (uint32)(FLS_TIMEOUT_DURATION*1000000U))
  		{
  			break;
  		}
  	}while (CSL_ospiIsIdle(pRegs) == FALSE);

    /* Start to execute flash read/write command */
    CSL_ospiFlashExecCmd(pRegs);

    while (retry != 0U)
    {
        /* Check the command execution status */
        if (CSL_ospiFlashExecCmdComplete(pRegs) == TRUE)
        {
            break;
        }
        OSPI_delay(OSPI_CHECK_IDLE_DELAY);
        retry--;
    }

    if (retry == 0U)
    {
        retVal = E_NOT_OK;
    }
    
    GetCounterValue(FLS_OS_COUNTER_ID, &startcount);
    elapsedcount = 0U;
    do
	   {
        tempCount = startcount;
  		GetElapsedValue(FLS_OS_COUNTER_ID, &tempCount, & elapsedcount);
  		if(elapsedcount >= (uint32)(FLS_TIMEOUT_DURATION*1000000U))
  		{
  			startcount= 0U;
  			elapsedcount =0U;
  			break;
  		}
	   }while (CSL_ospiIsIdle(pRegs) == FALSE);

    return (retVal);
}

/*FLS_CoverageGap_07: Dynamic coverage for the function cannot be achieved as these functions are static.*/
static Std_ReturnType OSPI_cmdRead(const CSL_ospi_flash_cfgRegs *pRegs,
                            uint32                      cmd,
                            uint8                      *rxBuf,
                            uint32                      rxLen)
{
    uint32 regVal;
    uint32 rdLen;
    Std_ReturnType  retVal;
    uint8 *pBuf = rxBuf;

    (void)CSL_ospiCmdRead(pRegs, cmd, rxLen);
    retVal = OSPI_flashExecCmd(pRegs);
    if (retVal == E_OK)
    {
        regVal = LLD_REG32_RD(&pRegs->FLASH_RD_DATA_LOWER_REG);

        /* Put the read value into rxBuf */
        rdLen = (rxLen > 4U) ? 4U : rxLen;
        (void)memcpy((void *)pBuf, (void *)(&regVal), rdLen);
        pBuf += rdLen;

        if (rxLen > 4U) {
            regVal = LLD_REG32_RD(&pRegs->FLASH_RD_DATA_UPPER_REG);
            rdLen = rxLen - rdLen;
            (void)memcpy((void *)pBuf, (void *)(&regVal), rdLen);
        }
    }
    return (retVal);
}

/*FLS_CoverageGap_07: Dynamic coverage for the function cannot be achieved as these functions are static.*/
static Std_ReturnType OSPI_cmdExtRead(const CSL_ospi_flash_cfgRegs *pRegs,
                              uint8                      *cmd,
                              uint32                      cmdLen,
                              uint8                      *rxBuf,
                              uint32                      rxLen,
                              uint32                      dummyCycles)
{
    uint32 regVal;
    uint32 rdLen;
    Std_ReturnType  retVal = E_NOT_OK;
    uint8 *pBuf = rxBuf;

    (void)CSL_ospiCmdExtRead(pRegs, cmd, cmdLen, rxLen, dummyCycles);
    retVal = OSPI_flashExecCmd(pRegs);
    if (retVal == E_OK)
    {
        regVal = LLD_REG32_RD(&pRegs->FLASH_RD_DATA_LOWER_REG);

        /* Put the read value into rxBuf */
        rdLen = (rxLen > 4U) ? 4U : rxLen;
        (void)memcpy((void *)pBuf, (void *)(&regVal), rdLen);
        pBuf += rdLen;

        if (rxLen > 4U) {
            regVal = LLD_REG32_RD(&pRegs->FLASH_RD_DATA_UPPER_REG);
            rdLen = rxLen - rdLen;
            (void)memcpy((void *)pBuf, (void *)(&regVal), rdLen);
        }
    }
    return (retVal);

}

/*FLS_CoverageGap_07: Dynamic coverage for the function cannot be achieved as these functions are static.*/
static Std_ReturnType OSPI_cmd_mode_read(SPI_Handle handle,
                                     SPI_Transaction *transaction)
{
    uint8               *cmd;
    OSPI_Object *object;
    Std_ReturnType retVal = E_NOT_OK;

    cmd = (uint8 *)transaction->txBuf;
    object = (OSPI_Object *)handle->object;

    if(CSL_ospiGetDualByteOpcodeMode((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR))!= 0U)
    {
        retVal = OSPI_cmdExtRead((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                                 (uint8 *)cmd,
                                 (uint32)CSL_OSPI_CMD_LEN_EXTENDED,
                                 (uint8 *)transaction->rxBuf,
                                 (uint32)transaction->count - CSL_OSPI_CMD_LEN_EXTENDED,
                                 (uint32)object->extRdDummyClks);
    }
    else
    {
        retVal = OSPI_cmdRead((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                              (uint32)cmd[0],
                              (uint8 *)transaction->rxBuf,
                              (uint32)transaction->count - CSL_OSPI_CMD_LEN_DEFAULT);
    }

    return (retVal);
}

/*FLS_CoverageGap_07: Dynamic coverage for the function cannot be achieved as these functions are static.*/
static Std_ReturnType OSPI_read(SPI_Handle handle, SPI_Transaction *transaction)
{
    OSPI_Object *object;
    Std_ReturnType         retVal = E_NOT_OK;

    object = (OSPI_Object *)handle->object;

    if((uint32)OSPI_OPER_MODE_IND_XFER == object->ospiMode)
    {
        retVal = (OSPI_ind_xfer_mode_read(handle, transaction));
    }
    else if((uint32)OSPI_OPER_MODE_DAC_XFER == object->ospiMode)
    {
        retVal = (OSPI_dac_xfer_mode_read(handle, transaction));
    }
    else if((uint32)OSPI_OPER_MODE_CFG == object->ospiMode)
    {
        retVal = (OSPI_cmd_mode_read(handle, transaction));
    }
    else
    {
        transaction->status=SPI_TRANSFER_CANCELED;
    }

	object->transaction = NULL;
    return(retVal);
}

/*FLS_CoverageGap_07: Dynamic coverage for the function cannot be achieved as these functions are static.*/
static Std_ReturnType OSPI_waitWriteSramLvl(const CSL_ospi_flash_cfgRegs *pReg,
                                     uint32 *sramLvl)
{
    uint32 retry = CSL_OSPI_REG_RETRY;
    uint32 sramLevel;
    Std_ReturnType  retVal = E_OK;

    while(retry != 0U)
    {
        sramLevel = CSL_ospiGetSramLvl(pReg, 0);
        if (sramLevel <= CSL_OSPI_SRAM_WATERMARK_WR_LVL)
        {
			*sramLvl = sramLevel;
            break;
        }

        Fls_delay(CSL_OSPI_POLL_IDLE_DELAY);

        retry--;
    }

    if (retry != 0U)
    {
        retVal = E_OK;
    }
    else
    {
        retVal = E_NOT_OK;
    }
    return(retVal);
}

/*FLS_CoverageGap_07: Dynamic coverage for the function cannot be achieved as these functions are static.*/
static Std_ReturnType OSPI_waitIndWriteComplete(const CSL_ospi_flash_cfgRegs *pRegs)
{
    uint32 retry = OSPI_DAC_WRITE_TIMEOUT;
    Std_ReturnType  retVal;

    /* Check flash indirect write controller status */
    while (retry != 0U)
    {
        if (CSL_ospiIsIndWriteComplete(pRegs) == TRUE)
        {
            break;
        }
        OSPI_delay(OSPI_CHECK_IDLE_DELAY);
        retry--;
    }

    if (retry != 0U)
    {
        /* Clear indirect completion status */
        CSL_ospiClrIndWriteComplete(pRegs);
        retVal = E_OK;
    }
    else
    {
        retVal = E_NOT_OK;
    }
    return(retVal);
}

/*FLS_CoverageGap_07: Dynamic coverage for the function cannot be achieved as these functions are static.*/
static Std_ReturnType OSPI_ind_xfer_mode_write(SPI_Handle handle,
                                           SPI_Transaction *transaction)
{
    OSPI_Object        *object;       /* OSPI object */
    uint8               *pSrc;         /* Destination address */
    uint32               count;        /* transaction length */
    uint32               offset;       /* OSPI flash offset address */
    uint32               remaining;
    uint32               sramLevel, wrBytes;
    uint32               retFlag = 0;
    Std_ReturnType       retVal = E_OK;

    /* Copy flash transaction parameters to local variables */
    offset = (uint32)(transaction->arg); /* NOR Flash offset address to write */
    pSrc = (uint8*)transaction->txBuf;
    count = (uint32)transaction->count;


    object = (OSPI_Object *)handle->object;

    /* Disable DAC mode */
    CSL_ospiDacEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), FALSE);

    /* Set write address in indirect mode */
    CSL_ospiIndSetStartAddr((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                            offset,
                            FALSE);
    CSL_ospiIndWriteExecute((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), count);

    if ((uint32)SPI_OPER_MODE_POLLING == object->intrPollMode)
    {
        /* Wait Indirect Write  SRAM fill level below the threshold */
        if (OSPI_waitWriteSramLvl((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                                  &sramLevel) != E_OK)
        {
            retFlag = 1U;
            retVal = E_NOT_OK;
            transaction->status=SPI_TRANSFER_FAILED;
        }
        else
        {
            remaining = count;
            while(remaining > 0U)
            {
                /* Wait indirect write SRAM fifo level below watermark */
                if (OSPI_waitWriteSramLvl((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                                          &sramLevel) != E_OK)
                {
                    retFlag = 1U;
                    retVal = E_NOT_OK;
                    break;
                }

                wrBytes = (CSL_OSPI_SRAM_PARTITION_WR - sramLevel) * CSL_OSPI_FIFO_WIDTH;
                wrBytes = (wrBytes > remaining) ? remaining : wrBytes;

                /* Write data to FIFO */
                CSL_ospiWriteFifoData((Fls_DrvObj.sectorList[0].sectorStartaddress), pSrc, wrBytes);

                pSrc += wrBytes;
                remaining -= wrBytes;
            }

            if(retFlag == 0U)
            {
                if (OSPI_waitIndWriteComplete((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR)) != E_OK)
                {
                    retFlag = 1U;
                    retVal = E_NOT_OK;
                }
            }
        }
    }

    if(retFlag == 1U)
    {
        transaction->status=SPI_TRANSFER_FAILED;
        CSL_ospiIndWriteCancel((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR));
    }
    return (retVal);
}

/*FLS_CoverageGap_07: Dynamic coverage for the function cannot be achieved as these functions are static.*/
static Std_ReturnType OSPI_dac_xfer_mode_write(SPI_Handle handle,
                                           const SPI_Transaction *transaction)
{

    uint8               *pSrc;         /* Source address */
    uint32               offset;       /* OSPI flash offset address */
    uint32               i;
    uint8               *pDst;         /* Destination address */
    uint32               size;
    uint32               remainSize;
    Std_ReturnType       retVal = E_OK;
    uint32               wrWord;
    uint32               wrByte;

    /* Copy flash transaction parameters to local variables */
    offset = (uint32)(transaction->arg); /* OSPI Flash offset address to write */
    pSrc = (uint8 *)transaction->txBuf;



    /* Disable PHY pipeline mode */
    CSL_ospiPipelinePhyEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), FALSE);

    pDst = (uint8 *)((Fls_DrvObj.sectorList[0].sectorStartaddress) + offset);
    remainSize = (uint32)transaction->count & 3U;
    size = (uint32)transaction->count - remainSize;
    /* Transfer the data in 32-bit size */
    for (i = 0U; i < size; i += 4U)
    {
        wrWord = LLD_REG32_RD(pSrc + i);
        LLD_REG32_WR(pDst + i, wrWord);
        if (OSPI_waitDeviceReady(handle, OSPI_DAC_WRITE_TIMEOUT) == TRUE)
        {
            retVal = E_OK;
        }
        else
        {
            retVal = E_NOT_OK;
        }
    }
    if (retVal == E_OK)
    {
        /* Transfer the remaining data in 8-bit size */
        for (i = 0; i < remainSize; i++)
        {
            wrByte = LLD_REG8_RD(pSrc + size + i);
            LLD_REG8_WR(pDst + size + i, wrByte);
            if (OSPI_waitDeviceReady(handle, OSPI_DAC_WRITE_TIMEOUT) == TRUE)
            {
                retVal = E_OK;
            }
            else
            {
                retVal = E_NOT_OK;
            }
        }
    }

    return (retVal);
}

static Std_ReturnType OSPI_cmdWrite(const CSL_ospi_flash_cfgRegs *pRegs,
                             const uint8                *cmdBuf,
                             uint32                      cmdLen,
                             const uint8                *txBuf,
                             uint32                      txLen)
{
    (void)CSL_ospiCmdWrite(pRegs, cmdBuf, cmdLen, txBuf, txLen);
    return (OSPI_flashExecCmd(pRegs));
}

/*FLS_CoverageGap_07: Dynamic coverage for the function cannot be achieved as these functions are static.*/
static Std_ReturnType OSPI_cmd_mode_write(SPI_Handle handle,
                                      const SPI_Transaction *transaction)
{
    uint8               *txBuf;
    uint32               dataLen;
    uint32               cmdLen;
    Std_ReturnType       retVal = E_OK;

    txBuf = (uint8 *)transaction->txBuf;
    dataLen = (uint32)(transaction->arg);
    cmdLen = (uint32)transaction->count - dataLen;


    if (dataLen != 0U)
    {
        retVal = (OSPI_cmdWrite((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                                txBuf,
                                cmdLen,
                                &txBuf[cmdLen],
                                dataLen));
    }
    else
    {
        retVal = (OSPI_cmdWrite((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                                txBuf,
                                cmdLen,
                                NULL,
                                0));
    }
    return (retVal);
}

/*FLS_CoverageGap_07: Dynamic coverage for the function cannot be achieved as these functions are static.*/
static Std_ReturnType OSPI_write(SPI_Handle handle, SPI_Transaction *transaction)
{
    OSPI_Object  *object;
    Std_ReturnType retVal = E_NOT_OK;

    /* Get the pointer to the object and hwAttrs */
    object = (OSPI_Object *)handle->object;

    if((uint32)OSPI_OPER_MODE_IND_XFER == object->ospiMode)
    {
        retVal = (OSPI_ind_xfer_mode_write(handle, transaction));
    }
    else if((uint32)OSPI_OPER_MODE_DAC_XFER == object->ospiMode)
    {
        retVal = (OSPI_dac_xfer_mode_write(handle, transaction));
    }
    else if((uint32)OSPI_OPER_MODE_CFG == object->ospiMode)
    {
        retVal = (OSPI_cmd_mode_write(handle, transaction));
    }
    else
    {
        transaction->status=SPI_TRANSFER_CANCELED;
    }
	
	object->transaction = NULL;
    return (retVal);
}

/*
 *  ======== OSPI_transfer ========
 */

 /* FLS_CoverageGap_09:Dynamic coverage for OSPI_transfer() cannot be acieved,
  * as the variable xferRet expects a value from OSPI_primeTransfer which is a static function.
  */
boolean OSPI_transfer(SPI_Handle handle, SPI_Transaction *transaction_xfer)
{
    boolean                ret = FALSE; /* return value */
    OSPI_Object        *object;      /* OSPI object */
    Std_ReturnType      xferRet;

    /* Check if anything needs to be written or read */
    if ((handle != NULL) && (transaction_xfer != NULL) && (0U != (uint32)transaction_xfer->count))
    {
        /* Get the pointer to the object and hwAttrs */
        object = (OSPI_Object *)handle->object;
        /* Check if a transfer is in progress */
        if (object->transaction != NULL)
        {
            /* Transfer is in progress */
            transaction_xfer->status = SPI_TRANSFER_CANCELED;
        }
        else
        {
            /* Save the pointer to the transaction */
            object->transaction = transaction_xfer;

            /* Book keeping of transmit and receive buffers. */
            object->writeBufIdx = (uint8 *)transaction_xfer->txBuf;
            object->writeCountIdx = (uint32)transaction_xfer->count;
            object->readBufIdx =  (uint8 *)transaction_xfer->rxBuf;
            object->readCountIdx = (uint32)transaction_xfer->count;

            /* FLS_CoverageGap_09:Dynamic coverage for OSPI_transfer() cannot be acieved,
             * as the variable xferRet expects a value from OSPI_primeTransfer which is a static function.
             */
            xferRet = OSPI_primeTransfer(handle, transaction_xfer);
            if (xferRet == E_OK)
            {
                /* Always return TRUE if in Asynchronous mode */
                ret = TRUE;
                transaction_xfer->status = SPI_TRANSFER_COMPLETED;
            }
            else
            {
                transaction_xfer->status=SPI_TRANSFER_FAILED;
            }
        }
    }
    else
    {
        if (transaction_xfer != NULL)
        {
            transaction_xfer->status = SPI_TRANSFER_CANCELED;
        }
    }

    return (ret);
}

/*FLS_CoverageGap_07: Dynamic coverage for the function cannot be achieved as these functions are static.*/
static Std_ReturnType OSPI_configDdr(SPI_Handle handle, uint32 cmd, uint32 addr, uint32 data)
{
    Std_ReturnType      retVal;

    CSL_ospiFlashStig((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), cmd, addr, data);
    retVal = OSPI_flashExecCmd((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR));
    if (retVal == E_OK)
    {
        if (Fls_DrvObj.dacEnable == TRUE)
        {
            CSL_ospiDacEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), TRUE);
        }
        else
        {
            CSL_ospiDacEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), FALSE);
        }

        /* Enable DTR protocol */
        if (Fls_DrvObj.dtrEnable == TRUE)
        {
            CSL_ospiDtrEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), TRUE);
        }
        else
        {
            CSL_ospiDtrEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), FALSE);
        }
    }

    return (retVal);
}

/*FLS_CoverageGap_07: Dynamic coverage for the function cannot be achieved as these functions are static.*/
static Std_ReturnType OSPI_enableXip (SPI_Handle handle, uint32 cmd, uint32 addr, uint32 data)
{

    Std_ReturnType     retVal;


    /* Disable Direct Access Controller */
    CSL_ospiDacEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), FALSE);

    /* Configure Flash Command Control Register to issue VCR write to FLASH memory to enable/disable XIP mode */
    CSL_ospiFlashStig((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), cmd, addr, data);
    retVal = OSPI_flashExecCmd((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR));
    if (retVal == E_OK)
    {
        OSPI_delay(OSPI_XIP_SETUP_DELAY);

        /* Clear the XIP mode bits in the Mode Bit Configuration Register. */
        CSL_ospiSetModeBits((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), 0x00);

        /* Enable the local controllers XIP mode */
        CSL_ospiXipEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), TRUE);
    }

    /* Re-enable the Direct Access Controller */
    CSL_ospiDacEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), TRUE);
    return (retVal);
}

static Std_ReturnType OSPI_configDummyCycle(SPI_Handle handle,
                                                    uint32 cmd, uint32 addr, uint32 data)
{
    Std_ReturnType         retVal;

    CSL_ospiFlashStig((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), cmd, addr, data);
    retVal = OSPI_flashExecCmd((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR));

    return (retVal);
}

/*
 *  ======== OSPI_control ========
 */
Std_ReturnType OSPI_control(SPI_Handle handle, uint32 cmd, const void *arg)
{
    OSPI_Object        *object;  /* OSPI object */
    Std_ReturnType      retVal = E_NOT_OK;
    const uint32        *ctrlData = (const uint32 *)arg;
    uint32               nvcrCmd;
    uint32               addr;
    uint32               data;
    uint32               extOpcodeLo;
    uint32               extOpcodeUp;

    if (handle != NULL)
    {
        /* Get the pointer to the object */
        object = (OSPI_Object *)handle->object;

        switch (cmd)
        {
            case SPI_V0_CMD_XFER_OPCODE:
            {
                object->transferCmd = *ctrlData;
                ctrlData++;
                CSL_ospiConfigRead((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                                   object->transferCmd,
                                   object->xferLines,
                                   object->rdDummyClks);
                object->transferCmd = *ctrlData;
                ctrlData++;
                CSL_ospiWriteSetup((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                                   object->transferCmd,
                                   object->xferLines);
                object->rdStatusCmd = *ctrlData;
                retVal = E_OK;
                break;
            }

            case SPI_V0_CMD_XFER_OPCODE_EXT:
            {
                extOpcodeLo  = *ctrlData; /* EXT_STIG_OPCODE_FLD */
                ctrlData++;
                extOpcodeLo |= (*ctrlData << 8); /* EXT_POLL_OPCODE_FLD */
                ctrlData++;
                extOpcodeLo |= (*ctrlData << 16); /* EXT_WRITE_OPCODE_FLD */
                ctrlData++;
                extOpcodeLo |= (*ctrlData << 24); /* EXT_READ_OPCODE_FLD */
                ctrlData++;
                extOpcodeUp  = (*ctrlData << 16); /* EXT_WEL_OPCODE_FLD */
                ctrlData++;
                extOpcodeUp |= (*ctrlData << 24); /* WEL_OPCODE_FLD */

                CSL_ospiExtOpcodeSet((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                             extOpcodeLo,
                             extOpcodeUp);
                CSL_ospiSetDualByteOpcodeMode((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                                      TRUE);
                retVal = E_OK;
                break;
            }

            case SPI_V0_CMD_XFER_MODE_RW:
            {
                object->transactionType = *ctrlData;
                retVal = E_OK;
                break;
            }

            case SPI_V0_CMD_SET_CFG_MODE:
            {
                object->ospiMode = (uint32)OSPI_OPER_MODE_CFG;
                retVal = E_OK;
                break;
            }

            case SPI_V0_CMD_SET_XFER_MODE:
            {
                if (Fls_DrvObj.dacEnable == TRUE)
                {
                    object->ospiMode = (uint32)OSPI_OPER_MODE_DAC_XFER;
                }
                else
                {
                    object->ospiMode = (uint32)OSPI_OPER_MODE_IND_XFER;
                }
                retVal = E_OK;
                break;
            }

            case SPI_V0_CMD_SET_XFER_LINES:
            {
                uint32 numAddrBytes;
                object->xferLines = *ctrlData;
                numAddrBytes = CSL_OSPI_MEM_MAP_NUM_ADDR_BYTES_3;
                if ((object->xferLines == OSPI_XFER_LINES_OCTAL) && (Fls_DrvObj.dtrEnable != 0U))
                {
                    numAddrBytes = CSL_OSPI_MEM_MAP_NUM_ADDR_BYTES_4;
                }
                if (object->xferLines == OSPI_XFER_LINES_OCTAL)
                {
                    /* 8 dummy cycles required for polling status register in octal mode */
                    CSL_ospiSetPollingDummyCycles((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                                                   8U);
                }
                else
                {
                    /* 0 dummy cycles required for polling status register in single mode */
                    CSL_ospiSetPollingDummyCycles((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                                                   0U);
                }
                /* Set device size cofigurations */
                CSL_ospiSetDevSize((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                                   numAddrBytes,
                                   Fls_DrvObj.sectorList[0].sectorPageSize,
								   Fls_DrvObj.blockSize);
                retVal = E_OK;
                break;
            }

            case SPI_V0_CMD_RD_DUMMY_CLKS:
            {
                object->rdDummyClks = *ctrlData;
                retVal = E_OK;
                break;
            }

            case SPI_V0_CMD_EXT_RD_DUMMY_CLKS:
            {
                object->extRdDummyClks = *ctrlData;
                retVal = E_OK;
                break;
            }

            /* FLS_CoverageGap_08:Dynamic coverage for OSPI_control() for the case SPI_V0_CMD_CFG_PHY is not possible,
             * as it is not used in the driver.
             */
            case SPI_V0_CMD_CFG_PHY:
            {
                uint32 phyEnable = *ctrlData++;
                if (phyEnable == TRUE)
                {
                    if (Fls_DrvObj.phyEnable == FALSE)
                    {
                        CSL_ospiSetPreScaler((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), CSL_OSPI_BAUD_RATE_DIVISOR(2U));
                        /* Enable PHY mode */
                        CSL_ospiPhyEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), TRUE);
                        Fls_DrvObj.phyEnable = TRUE;
                    }
                    uint32 txDelay = *ctrlData++;
                    uint32 rxDelay = *ctrlData;
                    uint32 funcClk = Fls_DrvObj.ospiClkSpeed;
                    CSL_ospiConfigPhyDLL((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),txDelay, rxDelay,CSL_OSPI_CFG_PHY_OP_MODE_MASTER, funcClk);
                }
                else
                {
                    if (Fls_DrvObj.phyEnable == TRUE)
                    {
                        /* Disable high speed mode when PHY is disabled */
                        CSL_ospiSetPreScaler((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), CSL_OSPI_BAUD_RATE_DIVISOR_DEFAULT);
                        CSL_ospiPhyEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), FALSE);
                        Fls_DrvObj.phyEnable = FALSE;
                    }
                }
                retVal = E_OK;
                break;
            }

            case SPI_V0_CMD_ENABLE_DDR:
            case SPI_V0_CMD_ENABLE_SDR:
            {
                nvcrCmd = *ctrlData;
                ctrlData++;
                addr   = *ctrlData;
                ctrlData++;
                data   = *ctrlData;
                retVal = OSPI_configDdr(handle, nvcrCmd, addr, data);
                break;
            }

            /* FLS_CoverageGap_08:Dynamic coverage for OSPI_control() for the case SPI_V0_CMD_CFG_XIP is not possible,
             * as it is not used in the driver.
             */
            case SPI_V0_CMD_CFG_XIP:
            {
                if (Fls_DrvObj.xipEnable == TRUE) {
                    nvcrCmd = *ctrlData;
                    ctrlData++;
                    addr = *ctrlData;
                    ctrlData++;
                    data = *ctrlData;
                    retVal = OSPI_enableXip(handle, nvcrCmd , addr, data);

                }
                /* Enable PHY pipeline mode for read */
                CSL_ospiPipelinePhyEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), TRUE);
                break;
            }

            case SPI_V0_CMD_CFG_RD_DELAY:
            {
                uint32 rdDelay = *ctrlData;
                CSL_ospiSetDataReadCapDelay((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR), rdDelay);
                retVal = E_OK;
                break;
            }

            case SPI_V0_CMD_CFG_DUMMY_CYCLE:
            {
                nvcrCmd = *ctrlData;
                ctrlData++;
                addr   = *ctrlData;
                ctrlData++;
                data   = *ctrlData;
                retVal = OSPI_configDummyCycle(handle, nvcrCmd, addr, data);
                break;
            }

            default:
                retVal = E_STATUS_UNDEFINED;
            break;
        }
    }
    return retVal;
}

/*
 *  ======== SPI_Params_init ========
 */
void SPI_Params_init(SPI_Params *params)
{
    if (params != NULL)
    {
        *params = SPI_defaultParams;
    }
}

/*******************************************************************************
 * Fls_RegisterReadback
 ******************************************************************************/
/*! \brief      This is the function for checking the configuration registers.
 *              This APi can be called by higher layers to check if configuration
 *              is correct.
 *
 *  \param[in] SPI_Handle handle - Handle for the SPI object that will used in OSPI Driver
 *
 *  \context
 ******************************************************************************/
/*
 * Design: MCAL-7300,MCAL-7375,MCAL-7371,MCAL-7423
 */
#if (STD_ON == OSPI_REGISTER_READBACK_API)

Std_ReturnType Ospi_RegisterReadback(
    P2VAR(CSL_ospi_flash_cfgRegs, AUTOMATIC, FLS_APPL_DATA) RegRbPtr)
{
    //uint32 baseAddr;
    Std_ReturnType retVal =  E_OK;
#if (STD_ON == FLS_DEV_ERROR_DETECT)
    if ((Fls_DrvObj.status == MEMIF_UNINIT) || (NULL_PTR == RegRbPtr))
    {
        retVal = E_NOT_OK;
    }
    else
#endif  /* #if (STD_ON == FLS_DEV_ERROR_DETECT) */
    {
		RegRbPtr->CONFIG_REG   =
       LLD_REG32_RD(FLS_OSPI_CTRL_BASE_ADDR + (uint32)CSL_OSPI_FLASH_CFG_CONFIG_REG);
    RegRbPtr->DEV_INSTR_RD_CONFIG_REG  =
			LLD_REG32_RD(FLS_OSPI_CTRL_BASE_ADDR + (uint32)CSL_OSPI_FLASH_CFG_DEV_INSTR_RD_CONFIG_REG);
		RegRbPtr->DEV_INSTR_WR_CONFIG_REG  =
			LLD_REG32_RD(FLS_OSPI_CTRL_BASE_ADDR + (uint32)CSL_OSPI_FLASH_CFG_DEV_INSTR_WR_CONFIG_REG);
		RegRbPtr->DEV_DELAY_REG  =
			LLD_REG32_RD(FLS_OSPI_CTRL_BASE_ADDR + (uint32)CSL_OSPI_FLASH_CFG_DEV_DELAY_REG);
		RegRbPtr->DEV_SIZE_CONFIG_REG  =
			LLD_REG32_RD(FLS_OSPI_CTRL_BASE_ADDR + (uint32)CSL_OSPI_FLASH_CFG_DEV_SIZE_CONFIG_REG);
		RegRbPtr->SRAM_PARTITION_CFG_REG  =
			LLD_REG32_RD(FLS_OSPI_CTRL_BASE_ADDR + (uint32)CSL_OSPI_FLASH_CFG_SRAM_PARTITION_CFG_REG);
		RegRbPtr->DMA_PERIPH_CONFIG_REG   =
			LLD_REG32_RD(FLS_OSPI_CTRL_BASE_ADDR + (uint32)CSL_OSPI_FLASH_CFG_DMA_PERIPH_CONFIG_REG);
		RegRbPtr->MODE_BIT_CONFIG_REG  =
			LLD_REG32_RD(FLS_OSPI_CTRL_BASE_ADDR + (uint32)CSL_OSPI_FLASH_CFG_MODE_BIT_CONFIG_REG);
		RegRbPtr->TX_THRESH_REG  =
			LLD_REG32_RD(FLS_OSPI_CTRL_BASE_ADDR + (uint32)CSL_OSPI_FLASH_CFG_TX_THRESH_REG);
		RegRbPtr->RX_THRESH_REG  =
			LLD_REG32_RD(FLS_OSPI_CTRL_BASE_ADDR + (uint32)CSL_OSPI_FLASH_CFG_RX_THRESH_REG);
		RegRbPtr->PHY_CONFIGURATION_REG  =
			LLD_REG32_RD(FLS_OSPI_CTRL_BASE_ADDR + (uint32)CSL_OSPI_FLASH_CFG_PHY_CONFIGURATION_REG);
    }
    return (retVal);
}
#endif /*#if (STD_ON == OSPI_REGISTER_READBACK_API) */

#if (STD_ON == OSPI_SAFETY_API)
/*
 * Design: MCAL-7401,MCAL-7388
 */
FUNC(Std_ReturnType, FLS_CODE)
    Fls_dataOverflowUnderflowIntrEnable(uint32 intrFlag)
{
    Std_ReturnType retVal = (Std_ReturnType) E_OK;
    if(intrFlag == CSL_OSPI_INTR_MASK_UNDERFLOW_DET)
    {
        CSL_ospiIntrEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                            CSL_OSPI_INTR_MASK_UNDERFLOW_DET, TRUE);
    }
    else if(intrFlag == CSL_OSPI_INTR_MASK_RECV_OVERFLOW_DET)
    {
        CSL_ospiIntrEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                            CSL_OSPI_INTR_MASK_RECV_OVERFLOW_DET, TRUE);
    }

    /* FLS_CoverageGap_10:Dynamic coverage for cannot be acieved as the else part is an additional check . */
    else
    {
        retVal = (Std_ReturnType) E_NOT_OK;
    }

    return (retVal);

}
/*
 * Design: MCAL-7401,MCAL-7388
 */
FUNC(Std_ReturnType, FLS_CODE)
    Fls_dataOverflowUnderflowIntrDisable(uint32 intrFlag)
{
    /* Disable  the interrupts */
    Std_ReturnType retVal = (Std_ReturnType) E_OK;
    if(intrFlag == CSL_OSPI_INTR_MASK_UNDERFLOW_DET)
    {
        CSL_ospiIntrEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                            CSL_OSPI_INTR_MASK_UNDERFLOW_DET, FALSE);
    }
    else if(intrFlag == CSL_OSPI_INTR_MASK_RECV_OVERFLOW_DET)
    {
        CSL_ospiIntrEnable((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                            CSL_OSPI_INTR_MASK_RECV_OVERFLOW_DET, FALSE);
    }

    /* FLS_CoverageGap_10:Dynamic coverage for cannot be acieved as the else part is an additional check . */
    else
    {
        retVal = (Std_ReturnType) E_NOT_OK;
    }

    return (retVal);

}


FUNC(Ospi_IrqStatusType, FLS_CODE)
    Fls_dataOverflowUnderflowIntrGetStatus(uint32 intFlags)
{
    uint32 intrStatus;
    Ospi_IrqStatusType  ospiIrqstatus    =  OSPI_NO_EVENT;
    if ((intFlags == CSL_OSPI_INTR_STATUS_UNDERFLOW_DET) | (intFlags == CSL_OSPI_INTR_STATUS_RECV_OVERFLOW_DET))
    {
        /* Read the interrupt status register */
        intrStatus = CSL_ospiIntrStatus((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR));
        intrStatus  = intrStatus & intFlags;
        /* FLS_CoverageGap_11:Dynamic coverage for the if condition cannot be covered,
         * as it expects a certain status of interrupt from the hardware register.
         */
        if (intrStatus == intFlags)
        {
            ospiIrqstatus = OSPI_EVENT_PENDING;
        }
        /* FLS_CoverageGap_11:Dynamic coverage for the else condition cannot be covered,
         * as it expects a certain status of interrupt from the hardware register.
         */
        else
        {
            ospiIrqstatus = OSPI_NO_EVENT;
        }
    }
    else
    {
        ospiIrqstatus = OSPI_STATUS_READ_FAIL;
    }

    return (ospiIrqstatus);
}

FUNC(Std_ReturnType, FLS_CODE)
    Fls_dataOverflowUnderflowIntrStatusClear(uint32 intrFlag)
{
  Std_ReturnType retVal = (Std_ReturnType) E_OK;
  if(intrFlag == CSL_OSPI_INTR_MASK_UNDERFLOW_DET)
  {
      CSL_ospiIntrClear((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                          CSL_OSPI_INTR_MASK_UNDERFLOW_DET);
  }
  else if(intrFlag == CSL_OSPI_INTR_MASK_RECV_OVERFLOW_DET)
  {
      CSL_ospiIntrClear((CSL_ospi_flash_cfgRegs *)(FLS_OSPI_CTRL_BASE_ADDR),
                          CSL_OSPI_INTR_MASK_RECV_OVERFLOW_DET);
  }

  /* FLS_CoverageGap_10:Dynamic coverage for cannot be acieved as the else part is an additional check . */
  else
  {
      retVal = (Std_ReturnType) E_NOT_OK;
  }

  return (retVal);

}
#endif//#if (STD_ON == OSPI_SAFETY_API)

#define FLS_STOP_SEC_CODE
#include "Fls_MemMap.h"
