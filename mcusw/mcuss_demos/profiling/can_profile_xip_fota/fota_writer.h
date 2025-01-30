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
 *  \file     monitor.h
 *
 *  \brief    Common header file for this application
 *
 */
/**
 * \defgroup MCUSS_APP_CAN_PROFILE Can profiling application
 *
 *  This application perform CAN transmission on all the enabled instances.
 *  By default the CAN peripheral is configured to operate in loop-back mode,
 *  i.e. transmit and receive.
 *
 *  TI RTOS is used and CAN tx/rx is performed in a task, The CAN IF stub is
 *  is used to indicate completion of transmission / reception by posting a
 *  semaphore.
 * @{
 */
#ifndef FOTA_WRITER_H_
#define FOTA_WRITER_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "string.h"
#include "Std_Types.h"
#include <stdint.h>

#include <ti/csl/arch/csl_arch.h>
#include <ti/csl/soc.h>
#include <ti/csl/cslr.h>
#include <ti/csl/src/ip/ospi/V0/csl_ospi.h>

#include <ti/osal/osal.h>
#include <ti/osal/TaskP.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                       Application Configurations                           */
/* ========================================================================== */

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#define TEST_BUF_LEN                    256U
#define P1_REGION_OFFSET                0x01000000
#define P2_REGION_OFFSET                0x02000000
#define P3_REGION_OFFSET_APPIMAGE       0x03000000
#define P3_REGION_OFFSET_APPIMAGE_XIP   0x03800000
#define BK_REGION_OFFSET                0x03F00000
#define APPIMAGE_OFFSET                 0x00100000
#define P1_ARTIFICIAL_OFFSET            0x00000000
#define P2_ARTIFICIAL_OFFSET            0x01000000
#define UPDATE_APPIMAGE_SIZE            0x00010000
#define UPDATE_APPIMAGE_XIP_SIZE        0x00050000

/** Modes for Write and Read operation */
#define OSPI_FLASH_OCTAL_READ            (1U)
#define OSPI_FLASH_OCTAL_PAGE_PROG       (1U)

/* OSPI0 on the Main domain :- baseAddr - flash config register baseAddr */
#define OSPI_FLASH_CONFIG_REG_BASE_ADDR  (0x47040000U)
#define OSPI_FLASH_DATA_BASE_ADDR        (0x50000000U)
#define OSPI_INDAC_TRIG_ADDR             (0x3FE0000)
#define OSPI_DAC_WRITE_TIMEOUT           (500000U)
#define OSPI_CHECK_IDLE_DELAY            (10U)
#define OPSI_FLASH_CMD_RDSR              (0x05U)
#define OSPI_FLASH_WRR_WRITE_TIMEOUT     (600U * 1000U)
#define OSPI_FLASH_BULK_ERASE_TIMEOUT    (110U * 1000U * 1000U)

#if defined(SOC_J7200)
#define NOR_XSPI
#endif

#if defined(NOR_XSPI)
/** FLASH device specific items (note: sizes are in bytes) */
#define OSPI_FLASH_BLOCK_SIZE               (256U * 1024U)
#define OSPI_FLASH_SECTOR_SIZE              (4U * 1024U)
#define OSPI_FLASH_SIZE                     (64U * 1024U * 1024U)
#define OSPI_FLASH_NUM_BLOCKS               (OSPI_FLASH_SIZE / OSPI_FLASH_BLOCK_SIZE)
#define OSPI_FLASH_NUM_SECTORS              (OSPI_FLASH_SIZE / OSPI_FLASH_SECTOR_SIZE)
#define OSPI_FLASH_PAGE_SIZE                (256U)
#define OSPI_FLASH_NUM_PAGES_PER_SECTOR     (OSPI_FLASH_SECTOR_SIZE / OSPI_FLASH_PAGE_SIZE)
#define OSPI_FLASH_NUM_PAGES_PER_BLOCK      (OSPI_FLASH_BLOCK_SIZE / OSPI_FLASH_PAGE_SIZE)
/** Flash device commands */
#define OSPI_FLASH_BE_SECTOR_NUM            (-1U)
#define OSPI_FLASH_CMD_WREN                 (0x06U)
#define OSPI_FLASH_CMD_BULK_ERASE           (0xC7U)
#define OSPI_FLASH_CMD_WRR                  (0x01U)
#define OSPI_FLASH_CMD_BLOCK_ERASE          (0xDCU)
#define OSPI_FLASH_CMD_SECTOR_ERASE         (0x21U)
#define OSPI_FLASH_CMD_WRITE_VCR            (0x71U)
#else
/** FLASH device specific items (note: sizes are in bytes) */
#define OSPI_FLASH_BLOCK_SIZE               (128U * 1024U)
#define OSPI_FLASH_SECTOR_SIZE              (4U * 1024U)
#define OSPI_FLASH_SIZE                     (64U * 1024U * 1024U)
#define OSPI_FLASH_NUM_BLOCKS               (OSPI_FLASH_SIZE / OSPI_FLASH_BLOCK_SIZE)
#define OSPI_FLASH_NUM_SECTORS              (OSPI_FLASH_SIZE / OSPI_FLASH_SECTOR_SIZE)
#define OSPI_FLASH_PAGE_SIZE                (256U)
#define OSPI_FLASH_NUM_PAGES_PER_SECTOR     (OSPI_FLASH_SECTOR_SIZE / OSPI_FLASH_PAGE_SIZE)
#define OSPI_FLASH_NUM_PAGES_PER_BLOCK      (OSPI_FLASH_BLOCK_SIZE / OSPI_FLASH_PAGE_SIZE)
/** Flash device commands */
#define OSPI_FLASH_BE_SECTOR_NUM            (-1U)
#define OSPI_FLASH_CMD_WREN                 (0x06U)
#define OSPI_FLASH_CMD_BULK_ERASE           (0x60U)
#define OSPI_FLASH_CMD_WRR                  (0x01U)
#define OSPI_FLASH_CMD_BLOCK_ERASE          (0xD8U)
#define OSPI_FLASH_CMD_SECTOR_ERASE         (0x20U)
#define OSPI_FLASH_CMD_WRITE_VCR            (0x81U)
#endif

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* Magic number and tokens for RPRC format */
#define BOOTLOADER_RPRC_MAGIC_NUMBER   0x43525052
#define BOOTLOADER_RPRC_RESOURCE       0
#define BOOTLOADER_RPRC_BOOTADDR       5

#define BOOTLOADER_MAX_INPUT_FILES 10
#define BOOTLOADER_META_HDR_MAGIC_STR 0x5254534D /* MSTR in ascii */
#define BOOTLOADER_META_HDR_MAGIC_END 0x444E454D /* MEND in ascii */

#define BOOTLOADER_MAX_ADDR_REGIONS 2

#define BOOTLOADER_DEVICE_ID 55U

typedef struct Bootloader_RprcFileHeader_s
{
    uint32_t magic;
    uint32_t entry;
    uint32_t rsvdAddr;
    uint32_t sectionCount;
    uint32_t version;

} Bootloader_RprcFileHeader;

typedef struct Bootloader_RprcSectionHeader_s
{
    uint32_t addr;
    uint32_t rsvdAddr;
    uint32_t size;
    uint32_t rsvdCrc;
    uint32_t rsvd;

} Bootloader_RprcSectionHeader;

typedef struct Bootloader_MetaHeaderStart_s
{
    uint32_t magicStr;
    uint32_t numFiles;
    uint32_t devId;
    uint32_t rsvd;

} Bootloader_MetaHeaderStart;

typedef struct Bootloader_MetaHeaderCore_s
{
    uint32_t coreId;
    uint32_t imageOffset;

} Bootloader_MetaHeaderCore;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

void WriterApp_Test(uint32_t updateImageOffset) __attribute__((section(".fota_writer_app")));

int32_t WriterApp_OspiErase(int32_t erLoc) __attribute__((section(".fota_writer_app")));

int32_t WriterApp_OspiOffsetToBlkPage(uint32_t offset, uint32_t *block, uint32_t *page) __attribute__((section(".fota_writer_app")));

int32_t WriterApp_IndWrite(uint32_t offset, uint8_t* pSrc, uint32_t count) __attribute__((section(".fota_writer_app")));

int32_t WriterApp_IndRead(uint32_t offset, uint8_t* pDst, uint32_t count) __attribute__((section(".fota_writer_app")));

int32_t WriterApp_OspiSwitchMode(bool dacEnable) __attribute__((section(".fota_writer_app")));

int32_t WriterApp_WaitOspiIdle(uint32_t timeOut) __attribute__((section(".fota_writer_app")));

int32_t WriterApp_OspiConfigDummyCycle(uint32_t dummyCycle) __attribute__((section(".fota_writer_app")));

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef FOTA_WRITER_H_ */
/* @} */
