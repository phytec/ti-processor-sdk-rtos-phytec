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
 *  \file     Fls_Ospi.h
 *
 *  \brief    This file contains interface header for FLS MCAL driver
 *
 */

#ifndef FLS_OSPI_H
#define FLS_OSPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "Fls_Cfg.h"
#include <Fls/fls_hw/ospi/V0/lldr_ospi.h>
#include <Fls/fls_hw/ospi/V0/lld_ospi.h>

#define SPI_CMD_RESERVED    (32U)
#define E_STATUS_UNDEFINED 	(0x02U)
#define SPI_WAIT_FOREVER    (~(0U))
/**
 *  \brief  Set SPI controller into CONFIG Mode
 *
 *  Command code used with SPI_control()
 */
#define SPI_V0_CMD_SET_CFG_MODE        (SPI_CMD_RESERVED + 0U)


/**
 *  \brief  Set SPI controller into Transfer Mode
 *
 *  Command code used with SPI_control()
 */
#define SPI_V0_CMD_SET_XFER_MODE       (SPI_CMD_RESERVED + 1U)

/**
 *  \brief  Set direction of data flow when in memory mapped mode.
 *
 *  Conventional SPI_transfer() calls no not require a read or write direction
 *  as SPI operates a in full duplex mode. This function is only used in
 *  combination with memory mapped mode where a read or write direction is
 *  required with SPI flash memory.
 */
#define SPI_V0_CMD_XFER_MODE_RW        (SPI_CMD_RESERVED + 2U)

/**
 *  \brief  Set flash specific command when in indirect transfer mode.
 *
 */
#define SPI_V0_CMD_XFER_OPCODE         (SPI_CMD_RESERVED + 3U)

/**
 *  \brief  Set number of lines configured for RX/TX operation.
 *
 */
#define SPI_V0_CMD_SET_XFER_LINES      (SPI_CMD_RESERVED + 4U)

/**
 *  \brief  Set number of dummy clock cycles for read operation.
 *
 */
#define SPI_V0_CMD_RD_DUMMY_CLKS       (SPI_CMD_RESERVED + 5U)

/**
 *  \brief  Set command to configure phy.
 *
 */
#define SPI_V0_CMD_CFG_PHY             (SPI_CMD_RESERVED + 6U)

/**
 *  \brief  Set command to enable DDR mode
 *
 */
#define SPI_V0_CMD_ENABLE_DDR          (SPI_CMD_RESERVED + 7U)

/**
 *  \brief  Set command to enable XIP mode
 *
 */
#define SPI_V0_CMD_CFG_XIP          (SPI_CMD_RESERVED + 8U)

/**
 *  \brief  Set command to disable DDR mode
 *
 */
#define SPI_V0_CMD_ENABLE_SDR          (SPI_CMD_RESERVED + 9U)

/*
 *
 *  \brief  Set extended opcodes when in indirect transfer mode.
 */
#define SPI_V0_CMD_XFER_OPCODE_EXT     (SPI_CMD_RESERVED + 10U)

/*
 *
 *  \brief  Set extended opcode command read dummy cycles.
 */
#define SPI_V0_CMD_EXT_RD_DUMMY_CLKS   (SPI_CMD_RESERVED + 11U)

/*
 *
 *  \brief  Set read capture delay.
 */
#define SPI_V0_CMD_CFG_RD_DELAY        (SPI_CMD_RESERVED + 12U)

/*
 *
 *  \brief  Set dummy cycles in flash device.
 */
#define SPI_V0_CMD_CFG_DUMMY_CYCLE     (SPI_CMD_RESERVED + 13U)

#define OSPI_DEV_DELAY_CSDA_2  (0x2U)  /* Chip Select De-Assert device delay in # of ref_clk */
#define OSPI_DEV_DELAY_CSDA_3  (0x3U)
#define OSPI_DEV_DELAY_CSDA    OSPI_DEV_DELAY_CSDA_2
#define OSPI_RDDATA_CAPDELAY  0U
#define OSPI_CSST_TRASDELAY   10U
#define OSPI_DELAY_ZERO		0U

#define SPI_MAX_CONFIG_CNT (2U)

/**
 *  @brief      A handle that is returned from a SPI_open() call.
 */
typedef struct SPI_Config_s      *SPI_Handle;

/**
 *  @brief      Status codes that are set by the SPI driver.
 */
typedef enum SPI_Status_s {
    SPI_TRANSFER_COMPLETED = 0,
    SPI_TRANSFER_STARTED,
    SPI_TRANSFER_CANCELED,
    SPI_TRANSFER_FAILED,
    SPI_TRANSFER_CSN_DEASSERT,
    SPI_TRANSFER_TIMEOUT
} SPI_Status;

/**
 *  @brief
 *  A ::SPI_Transaction data structure is used with SPI_transfer(). It indicates
 *  how many ::SPI_FrameFormat frames are sent and received from the buffers
 *  pointed to txBuf and rxBuf.
 *  The arg variable is an user-definable argument which gets passed to the
 *  ::SPI_CallbackFxn when the SPI driver is in ::SPI_MODE_CALLBACK.
 */
typedef struct SPI_Transaction_s {
    /* User input (write-only) fields */
    uint32     count;
    /**< Number of frames for this transaction */
    void      *txBuf;
    /**< void * to a buffer with data to be transmitted */
    void      *rxBuf;
    /**< void * to a buffer to receive data */
    void      *arg;
    /**< Argument to be passed to the callback function */

    /* User output (read-only) fields */
    SPI_Status status;
    /**< Status code set by SPI_transfer */
} SPI_Transaction;

/**
 *  @brief
 *  Definitions for various SPI modes of operation.
 */
typedef enum SPI_Mode_s {
    SPI_MASTER      = 0,
    /**< SPI in master mode */
    SPI_SLAVE       = 1
    /**< SPI in slave mode */
} SPI_Mode;

/**
 *  @brief
 *  Definitions for various SPI data frame formats.
 */
typedef enum SPI_FrameFormat_s {
    SPI_POL0_PHA0   = 0,
    /**< SPI mode Polarity 0 Phase 0 */
    SPI_POL0_PHA1   = 1,
    /**< SPI mode Polarity 0 Phase 1 */
    SPI_POL1_PHA0   = 2,
    /**< SPI mode Polarity 1 Phase 0 */
    SPI_POL1_PHA1   = 3,
    /**< SPI mode Polarity 1 Phase 1 */
    SPI_TI          = 4,
    /**< TI mode */
    SPI_MW          = 5
    /**< Micro-wire mode */
} SPI_FrameFormat;

/**
 *  \brief      Type of the SPI transaction.
 */
typedef enum SPI_TransactionType_e {
    SPI_TRANSACTION_TYPE_READ = 0,
    /**< Read transaction */
    SPI_TRANSACTION_TYPE_WRITE
    /**< Write transaction */
} SPI_TransactionType;

/**
 *  \brief  OSPI controller modes.
 */
typedef enum OSPI_OperMode_e {
    OSPI_OPER_MODE_CFG = 0U,
    /**< OSPI controller is in config mode */
    OSPI_OPER_MODE_IND_XFER,
    /**< OSPI controller is in indirect transfer mode */
    OSPI_OPER_MODE_DAC_XFER,
    /**< OSPI controller is in direct transfer mode */
} OSPI_OperMode;
/**
 * \brief Irq status and std return type
 */
typedef enum
{
    OSPI_NO_EVENT = 0U,
    /**< No event ocuurs in Overflow/Under flow */
    OSPI_EVENT_PENDING = 1U,
    /**< Event occurs in Overflow/Under flow */
    OSPI_STATUS_READ_FAIL = 2U,
    /**< The status reading fails */
} Ospi_IrqStatusType;


 /*  @brief
 *
 *  SPI transfer mode determines the whether the SPI controller operates
 *  synchronously or asynchronously. In ::SPI_MODE_BLOCKING mode SPI_transfer()
 *  blocks code execution until the SPI transaction has completed. In
 *  ::SPI_MODE_CALLBACK SPI_transfer() does not block code execution and instead
 *  calls a ::SPI_CallbackFxn callback function when the transaction has
 *  completed.
 */
typedef enum SPI_TransferMode_s {
    /**
     * SPI_transfer() blocks execution. This mode can only be used when called
     * within a Task context
     */
    SPI_MODE_BLOCKING,
    /**
     * SPI_transfer() does not block code execution and will call a
     * ::SPI_CallbackFxn. This mode can be used in a Task, Swi, or Hwi context.
     */
    SPI_MODE_CALLBACK
} SPI_TransferMode;


typedef struct SPI_Params_s {
    SPI_TransferMode    transferMode;
    /**< Blocking or Callback mode */
    uint32            transferTimeout;
    /**< Transfer timeout in system ticks (Not supported with all implementations */
    SPI_Mode            mode;
    /**< Master or Slave mode */
    uint32            bitRate;
    /**< SPI bit rate in Hz */
    uint32            dataSize;
    /**< SPI data frame size in bits */
    SPI_FrameFormat     frameFormat;
    /**< SPI frame format */
    void               *custom;
    /**< Custom argument used by driver implementation */
} SPI_Params;


/**
 *  @brief SPI Global configuration
 *
 *  The SPI_Config structure contains a set of pointers used to characterize
 *  the SPI driver implementation.
 *
 *  This structure needs to be defined before calling SPI_init() and it must
 *  not be changed thereafter.
 *
 *  @sa     SPI_init()
 */
typedef struct SPI_Config_s {
    void               *object;
    /**< Pointer to a driver specific data object */
} SPI_Config;


typedef SPI_Config SPI_config_list[SPI_MAX_CONFIG_CNT];

/**
 *  \brief  OSPI IP V0 driver object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct OSPI_Object_s {
    void             *transferComplete;
    /**< Transfer complete lock */
    void             *hwi;
    /**< Hardware interrupt object */
    uint32          waitTimeout;
    /**< Wait time out */
    SPI_Params        ospiParams;
    /**< input parameters */
    uint32          transactionType;
    /**< Read or Write Transaction */
    uint32          transferCmd;
    /**< Transfer command */
    uint32          ospiMode;
    /**< Config or Memory map mode */
    uint32          xferLines;
    /**< Number of transfer Lines */
    uint32          rdDummyClks;
    /**< Number of dummy clock cycles for read op */
    uint32          extRdDummyClks;
    /* Number of dummy clock cycles for extended opcode command read op */
    uint32          intrPollMode;
    /**< Interrupt or polling mode */
    SPI_TransferMode  transferMode;
    /**< Transfer mode */
    SPI_Transaction  *transaction;
    /**< Transaction structure */
    uint8          *writeBufIdx;
    /**< Internal inc. writeBuf index */
    uint32          writeCountIdx;
    /**< Internal dec. writeCounter */
    uint8          *readBufIdx;
    /**< Internal inc. readBuf index */
    uint32          readCountIdx;
    /**< Internal dec. readCounter */
    boolean         isOpen;
    /**< flag to indicate module is open */
    uint32          rdStatusCmd;
    /**< flash command to read status register */

} OSPI_Object;

extern uint32 devDelays[4];
    /**< OSPI device delays (CSSOT, CSEOT, CSDADS and CSDA delays) */

    /**< Master mode baud rate divisor value */
extern boolean tuneDone;

extern const SPI_config_list SPI_config;

extern void       Fls_OSPI_init(SPI_Handle handle);
extern void       Fls_OSPI_close(SPI_Handle handle);

extern Std_ReturnType    OSPI_control(SPI_Handle handle, uint32 cmd, const void *arg);
extern boolean       OSPI_transfer(SPI_Handle handle,
                                   SPI_Transaction *transaction_xfer);
extern void       OSPI_transferCancel(SPI_Handle handle);

extern SPI_Handle Fls_OSPI_open(SPI_Handle handle, const SPI_Params *params);

extern void       OSPI_hwi(SPI_Handle handle);
extern void       OSPI_hwi_2(SPI_Handle handle);
extern void SPI_Params_init(SPI_Params *params);

#if (STD_ON == OSPI_REGISTER_READBACK_API)
/**
 *  \brief This function reads the important registers of the hardware unit
 *  and returns the value in the structure.
 *
 *  This API should be called after Ospi_Init is called. Otherwise this API will
 *  return E_NOT_OK.
 *
 *  This API could be used to readback the register contents after Ospi_Init
 *  and then the readback value could be compared during SPI execution to
 *  check the correctness of the HW unit. Since this API is used for this
 *  purpose, the register returned are the ones which doesn't change after init
 *  based on job or channel config.
 *
 *  \verbatim
 *  Service name        : Ospi_RegisterReadback
 *  Mode                : Supervisor Mode (Privileged Mode)
 *  Sync/Async          : Synchronous
 *  Reentrancy          : Reentrant
 *  Parameters (inout)  : RegRbPtr - Pointer to where to store the readback
 *                        values. If this pointer is NULL_PTR, then the API
 *                        will return E_NOT_OK.
 *  Return value        : Std_ReturnType
 *                        E_OK: Register read back has been done
 *                        E_NOT_OK: Register read back failed
 *  \endverbatim
 */
FUNC(Std_ReturnType, SPI_CODE) Ospi_RegisterReadback(
    P2VAR(CSL_ospi_flash_cfgRegs, AUTOMATIC, SPI_APPL_DATA) RegRbPtr);
#endif  /* #if (STD_ON == SPI_REGISTER_READBACK_API) */
#if (STD_ON == OSPI_SAFETY_API)
FUNC(Std_ReturnType, FLS_CODE)
    Fls_dataOverflowUnderflowIntrEnable(uint32 intrFlag);
FUNC(Std_ReturnType, FLS_CODE)
    Fls_dataOverflowUnderflowIntrDisable(uint32 intrFlag);
FUNC(Ospi_IrqStatusType, FLS_CODE)
    Fls_dataOverflowUnderflowIntrGetStatus(uint32 intFlags);
FUNC(Std_ReturnType, FLS_CODE)
    Fls_dataOverflowUnderflowIntrStatusClear(uint32 intrFlag);
#endif /*#if (STD_ON == OSPI_SAFETY_API) */


#ifdef __cplusplus
}
#endif

#endif /* FLS_OSPI_H */
