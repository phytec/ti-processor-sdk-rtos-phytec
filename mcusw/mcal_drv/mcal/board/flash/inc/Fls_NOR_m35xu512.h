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
 *
 * \file  Fls_NOR_m35xu512.h
 *
 * \brief This file contains M35XU512 NOR device definitions
 *
 *****************************************************************************/
#ifndef FLS_NOR_M35XU512_H_
#define FLS_NOR_M35XU512_H_

/**************************************************************************
                            Macro Definitions
 **************************************************************************/

/** Macro to enable 4 byte addressing */
/* #define EXT_ADDRESS_ENABLE        (0U) */

/** FLASH device specific items (note: sizes are in bytes) */
#define NOR_BLOCK_SIZE               (128U * 1024U)
#define NOR_SECTOR_SIZE              (4U * 1024U)
#define NOR_SIZE                     (64U * 1024U * 1024U)
#define NOR_NUM_BLOCKS               (NOR_SIZE / NOR_BLOCK_SIZE)
#define NOR_NUM_SECTORS              (NOR_SIZE / NOR_SECTOR_SIZE)
#define NOR_PAGE_SIZE                (256U)
#define NOR_NUM_PAGES_PER_SECTOR     (NOR_SECTOR_SIZE / NOR_PAGE_SIZE)
#define NOR_NUM_PAGES_PER_BLOCK      (NOR_BLOCK_SIZE / NOR_PAGE_SIZE)
#define NOR_ERASED_DATA              (0xFF)

/** Flash device commands */
#define NOR_BE_SECTOR_NUM            (-1U)
#define NOR_CMD_BULK_ERASE           (0x60U)
#define NOR_CMD_WRR                  (0x01U)
#define NOR_CMD_WREN                 (0x06U)
#define NOR_CMD_RDSR                 (0x05U)
#define NOR_CMD_RDCR_VOL             (0x85U)
#define NOR_CMD_RDCR_NVOL            (0xB5U)
#define NOR_CMD_RDID                 (0x9FU)
#define NOR_CMD_RSTEN                (0x66U)
#define NOR_CMD_RST_MEM              (0x99U)


/** Different commands for 4 byte addressing and 3 byte addressing */
#ifdef EXT_ADDRESS_ENABLE
#define NOR_CMD_BLOCK_ERASE          (0xDCU)
#define NOR_CMD_SECTOR_ERASE         (0x21U)
#define NOR_CMD_READ                 (0x13U)
#define NOR_CMD_FAST_READ            (0x0CU)
#define NOR_CMD_OCTAL_O_FAST_RD      (0x7CU)
#define NOR_CMD_OCTAL_IO_FAST_RD     (0xCCU)
#define NOR_CMD_OCTAL_READ           (NOR_CMD_OCTAL_O_FAST_RD)
#define NOR_CMD_PAGE_PROG            (0x12U)
#define NOR_CMD_OCTAL_FAST_PROG      (0x84U)
#define NOR_CMD_EXT_OCTAL_FAST_PROG  (0x8EU)
#define NOR_CMD_OCTAL_PROG           (NOR_CMD_OCTAL_FAST_PROG)
#else
#define NOR_CMD_BLOCK_ERASE          (0xD8U)
#define NOR_CMD_SECTOR_ERASE         (0x20U)
#define NOR_CMD_READ                 (0x03U)
#define NOR_CMD_FAST_READ            (0x0BU)
#define NOR_CMD_OCTAL_O_FAST_RD      (0x8BU)
#define NOR_CMD_OCTAL_IO_FAST_RD     (0xCBU)
#define NOR_CMD_OCTAL_DDR_O_FAST_RD  (0x9DU)
#define NOR_CMD_OCTAL_DDR_IO_FAST_RD (0xFDU)
#define NOR_CMD_OCTAL_READ           (NOR_CMD_OCTAL_O_FAST_RD)
#define NOR_CMD_PAGE_PROG            (0x02U)
#define NOR_CMD_OCTAL_FAST_PROG      (0x82U)
#define NOR_CMD_EXT_OCTAL_FAST_PROG  (0xC2U)
#define NOR_CMD_OCTAL_PROG           (NOR_CMD_OCTAL_FAST_PROG)
#define NOR_CMD_WRITE_VCR            (0x81U)
#define NOR_CMD_READ_VCR             (0x85U)
#endif

/* \brief Read ID command definitions */
#define NOR_RDID_NUM_BYTES           (0x3U)
#define NOR_MANF_ID                  (0x2CU)    /* Manufacturer ID */
#define NOR_DEVICE_ID                (0x5B1A)   /* Device ID */

/** Status Register, Write-in-Progress bit */
#define NOR_SR_WIP                   (1U << 0U)

/** Dummy cycles for Read operation */
/** Dummy cycles for Read operation */
#define NOR_SINGLE_READ_DUMMY_CYCLE  (0U)
#define NOR_OCTAL_READ_DUMMY_CYCLE   (30U)


/** In Micro seconds */
#define NOR_PAGE_PROG_TIMEOUT        (400U)
#define NOR_SECTOR_ERASE_TIMEOUT     (600U * 1000U)
#define NOR_WRR_WRITE_TIMEOUT        (600U * 1000U)
#define NOR_BULK_ERASE_TIMEOUT       (110U * 1000U * 1000U)

#endif /* FLS_NOR_M35XU512_H_ */

/* Nothing past this point */
