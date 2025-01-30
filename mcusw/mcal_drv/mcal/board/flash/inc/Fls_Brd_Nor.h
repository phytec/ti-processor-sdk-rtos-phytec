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
 *  \file     Fls_Brd_Nor.h
 *
 *  \brief    This file contains the common declarations and macros used across
 *            all the FLS MCAL driver files.
 *
 */

#ifndef FLS_BRD_NOR_H_
#define FLS_BRD_NOR_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "stdint.h"
#include "string.h"
#include "SchM_Fls.h"
#include "Fls.h"
#include "Fee_Cbk.h"
#include "Fls_Ospi.h"


#include <hw_include/soc.h>
#include <hw_include/hw_types.h>

#if defined (FLS_NOR)

#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
#include "nor_spi_phy_tune.h"
#include "nor_spi_patterns.h"
#endif 

#if defined (m35xu512)
#include "Fls_NOR_m35xu512.h"
#endif
#if defined (s28hs512t)
#include "Fls_NOR_s28hs512t.h"
#endif
#if defined (mt25qu512abb)
#include "Fls_NOR_mt25qu512abb.h"
#endif

#endif

#include "Det.h"

#ifdef __cplusplus
extern "C"
{
#endif


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define BOARD_XSPI_BAUDRATE_DIV    (6U)

#define BOARD_XSPI_BAUDRATE_DIV_133M    (4U)
#define BOARD_XSPI_BAUDRATE_DIV_200M    (6U)

/* The retVal definitions */
#define E_COMPARE_MISMATCH      (2U)
#define E_BLANKCHECK_MISMATCH   (3U)

/* FLS Stage of Erase. */
#define FLS_S_DEFAULT           (0U)
#define FLS_S_DELAY_0_STAGE     (1U)
#define FLS_S_INVALID_ADDRESS   (2U)
#define FLS_S_DELAY_1_STAGE     (3U)
#define FLS_S_CMD_STAGE         (4U)
#define FLS_S_DELAY_2_STAGE     (5U)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/**< ENUM for job type names */
typedef enum {
	FLS_JOB_NONE, FLS_JOB_COMPARE, FLS_JOB_ERASE, FLS_JOB_READ, FLS_JOB_WRITE, FLS_JOB_BLANKCHECK
} Fls_JobType;


/**
 * \breif FLS Driver Object, global information used by module.
 */
typedef struct {
    Fls_JobEndNotifyType            Fls_JobEndNotification;
    /**<    Mapped to the job end notification routine provided by some upper layer
     *      module, typically the Fee module. */
    Fls_JobErrorNotifyType          Fls_JobErrorNotification;
    /**<    Mapped to the job error notification routine provided by some upper layer
     *      module, typically the Fee module. */
    uint32                          maxReadNormalMode;
    /**<    The maximum number of bytes to read or compare in one cycle of the
     *      flash driver's job processing function in normal mode. */
    uint32                          maxWriteNormalMode;
    /**<    The maximum number of bytes to write in one cycle of the
     *      flash driver's job processing function in normal mode. */
	uint32							clkSpeed;
	/**<    OSPI Module Reference Clock Speed */
    Fls_SectorType                  sectorList[1];
    /**<     The maximum number of bytes to write in one cycle of the flash driver's job
     *       processing function in normal mode.
     *       Using Fls Info Structure instead of this for now. */
    boolean                         dacEnable;
    /**<    Enable Direct Access Mode of Transfer in OSPI Flash */
    boolean                         xipEnable;
    /**<    Enable XIP mode in OSPI Flash */
    uint64                          ospiClkSpeed;
    /**<    OSPI Clock Speed */
    boolean                         dtrEnable;
    /**<    Enable Double transfer rate protocol for OSPI. */
	boolean 						phyEnable;
    MemIf_StatusType                status;
    /**< Current Module Status update variable */
    MemIf_JobResultType             jobResultType;
    /**< Current Job Result update variable */
    Fls_JobType                     jobType;
    /**< Current running job type */
    Fls_AddressType                 flashAddr;
    /**< Flash Address for the current job */
    uint8 *                         ramAddr;
    /**< Ram Address pointer for the current job */
    Fls_LengthType                  length;
    /**< Data transfer length for the current job */
    MemIf_ModeType                  mode;
    /**< Module mode setup for current job - SLOW for now */
    uint32                          jobChunkSize;
    /**< Length of Data to be transfer in current call cycle */
    SPI_Handle                      spiHandle;
    /**< Handle for the SPI object that will used in OSPI Driver */
    Fls_LengthType                  transferred;
	/**<    Select the OSPI read/write transaction Lines */
	OSPI_xferLines                  ospixferlines;
	/**<    Select the OSPI read/write transaction Lines */
	uint32                          flsBaudRateDiv;
	/**<    To select the FLS baudrate Divider value */
	uint32                          blockSize;
	/**<    To select OSPI block size based on variants */
    uint32                          timeOutVal;
    /**<    timeout value used in xspi/ospiAsyncWaitReady function*/
    uint32                          prevFlashaddr;
    /**<    Flash Previous Address. For checking blankcheck right after OSPI erase*/

} Fls_DriverObjType;

/**< ENUM for Internal State type names used for Nor_xspiAsyncWaitReady*/
typedef enum {
	FLS_INTERNAL_JOB_WAIT=0, 
	FLS_INTERNAL_JOB_FAIL, 
	FLS_INTERNAL_JOB_DONE
} Fls_InternalStateType;

extern Fls_DriverObjType Fls_DrvObj;


/* ========================================================================== */
/*                           Typedefs                                         */
/* ========================================================================== */


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
extern Std_ReturnType Fls_hwUnitInit(void);
extern Std_ReturnType Fls_norOpen(void);
extern void processJobs(Fls_JobType job);
extern void Fls_resetDrvObj(Fls_DriverObjType *drvObj);
extern void Fls_NorOspiClose(void);
extern void Fls_copyConfig(Fls_DriverObjType *drvObj, const Fls_ConfigType *cfgPtr);

static Std_ReturnType Fls_norErase(uint32 actualChunkSize);
static Std_ReturnType Fls_norRead(uint32 actualChunkSize);
static Std_ReturnType Fls_norWrite(uint32 actualChunkSize);
static Std_ReturnType Fls_norCompare(uint32 actualChunkSize);
static Std_ReturnType Fls_norBlankCheck(uint32 actualChunkSize);

static Std_ReturnType Fls_norAsyncErase(uint32 actualChunkSize);

boolean Fls_VerifyData_priv(const uint8 *expData,
                const uint8 *rxData,
                uint32 length);

#if defined (FLS_OSPI)
/*OSPI functions Declarations*/
Std_ReturnType Nor_ospiCmdWrite(SPI_Handle handle, uint8 *cmdBuf,
                                        uint32 cmdLen, uint32 dataLen);
Std_ReturnType NOR_ospiCmdRead(SPI_Handle handle, uint8 *cmdBuf,
                            uint32 cmdLen, uint8 *rxBuf, uint32 rxLen);
Std_ReturnType Nor_ospiReadId(SPI_Handle handle);
Std_ReturnType Nor_ospiEnableDDR(SPI_Handle handle);
Std_ReturnType Nor_ospiEnableSDR(SPI_Handle handle);
Std_ReturnType Nor_ospiXipEnable(SPI_Handle handle, uint8 modeBit);
Std_ReturnType Nor_ospiWaitReady(SPI_Handle handle, uint32 timeOut);
Std_ReturnType Nor_ospiSetOpcode(SPI_Handle handle);
Std_ReturnType Nor_ospiSetDummyCycle(SPI_Handle handle, uint32 dummyCycle);

Fls_InternalStateType Nor_ospiAsyncWaitReady(SPI_Handle handle, uint32 timeOut);
#endif

#if defined (FLS_XSPI)
/*XSPI functions Declarations*/
Std_ReturnType Nor_xspiCmdWrite(SPI_Handle handle, uint8 *cmdBuf,
                                        uint32 cmdLen, uint32 dataLen);
Std_ReturnType Nor_xspiCmdRead(SPI_Handle handle, uint8 *cmdBuf,
                            uint32 cmdLen, uint8 *rxBuf, uint32 rxLen);

Std_ReturnType Nor_xspiHybridSectCfg(SPI_Handle handle, uint8 enable,
                                        uint32   cfgFlag);
Std_ReturnType Nor_xspiReadId(SPI_Handle handle);
Std_ReturnType Nor_xspiEnableDDR(SPI_Handle handle);
Std_ReturnType Nor_xspiEnableSDR(SPI_Handle handle);
Std_ReturnType Nor_xspiWaitReady(SPI_Handle handle, uint32 timeOut);
Std_ReturnType Nor_xspiSetOpcode(SPI_Handle handle);
Std_ReturnType Nor_xspiSetDummyCycle(SPI_Handle handle, uint32 dummyCycle);

Fls_InternalStateType Nor_xspiAsyncWaitReady(SPI_Handle handle, uint32 timeOut);
#endif

#if defined (FLS_QSPI)
/*OSPI functions Declarations*/
Std_ReturnType Nor_qspiCmdWrite(SPI_Handle handle, uint8 *cmdBuf,
                                        uint32 cmdLen, uint32 dataLen);
Std_ReturnType NOR_qspiCmdRead(SPI_Handle handle, uint8 *cmdBuf,
                            uint32 cmdLen, uint8 *rxBuf, uint32 rxLen);
Std_ReturnType Nor_qspiReadId(SPI_Handle handle);
Std_ReturnType Nor_qspiEnableDDR(SPI_Handle handle);
Std_ReturnType Nor_qspiEnableSDR(SPI_Handle handle);
Std_ReturnType Nor_qspiXipEnable(SPI_Handle handle, uint8 modeBit);
Std_ReturnType Nor_qspiWaitReady(SPI_Handle handle, uint32 timeOut);

Fls_InternalStateType Nor_qspiAsyncWaitReady(SPI_Handle handle, uint32 timeOut);
#endif
/*========================================================================= */
/*                       Static Function Definitions                          */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
boolean gPhyEnable;
boolean gDtrEnable;
/* None */

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef FLS_BRD_NOR_H_ */
