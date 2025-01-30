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
 * \file  Fls_NOR_s28hs512t.h
 *
 * \brief This file contains S28HS512T NOR device definitions
 *
 *****************************************************************************/
#ifndef FLS_NOR_S28hs512T_H_
#define FLS_NOR_S28hs512T_H_
#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
 **                       Macro Definitions
 **************************************************************************/

/** FLASH device specific items (note: sizes are in bytes) */
#define NOR_BLOCK_SIZE               (256U * 1024U)
/* The Smallest Hybrid sector size is 4-KB */
#define NOR_SECTOR_SIZE              (4U * 1024U)
/* The Hybrid sector size of 64-KB */
#define NOR_HYBRID_SECTOR_SIZE       NOR_BLOCK_SIZE
#define NOR_SIZE                     (64U * 1024U * 1024U)
/** Physical set of thirty two 4-KB sectors at top or bottom of the address space */
#define NOR_NUM_4KSECTORS            (32U)
#define NOR_NUM_SECTORS              NOR_NUM_4KSECTORS
#define NOR_NUM_BLOCKS               (NOR_SIZE / NOR_BLOCK_SIZE)
#define NOR_PAGE_SIZE                (256U)
#define NOR_NUM_PAGES_PER_SECTOR     (NOR_SECTOR_SIZE / NOR_PAGE_SIZE)
#define NOR_NUM_PAGES_PER_BLOCK      (NOR_BLOCK_SIZE / NOR_PAGE_SIZE)
#define NOR_ERASED_DATA              (0xFF)


#define NOR_4K_SECT_BOT_END_OFFSET   (0x1FFFF)

/** Flash device commands */ 
#define NOR_BE_SECTOR_NUM            (-1U)

#define NOR_CMD_BULK_ERASE           (0xC7U)
#define NOR_CMD_WRREG                (0x71U)
#define NOR_CMD_WREN                 (0x06U)
#define NOR_CMD_WRDIS                (0x04U)
#define NOR_CMD_RDSR                 (0x05U)
#define NOR_CMD_RDSR2                (0x07U)
#define NOR_CMD_RDREG                (0x65U)
#define NOR_CMD_RDCR                 (0x35U)
#define NOR_CMD_RDID                 (0x9FU)
#define NOR_CMD_SRSTE                (0x66U)
#define NOR_CMD_SFRST                (0x99U)

#define NOR_CMD_BLOCK_ERASE          (0xDCU)
#define NOR_CMD_SECTOR_ERASE         (0x21U)
#define NOR_CMD_READ                 (0x03U)
#define NOR_CMD_FAST_READ            (0x0BU)
#define NOR_CMD_OCTAL_READ           (0xECU)
#define NOR_CMD_OCTAL_DDR_READ       (0xEEU)
#define NOR_CMD_PAGE_PROG            (0x02U)
#define NOR_CMD_OCTAL_PROG           (0x12U)
#define NOR_CMD_WRITE_VCR            (0x71U)
#define NOR_CMD_READ_VCR             (0x85U)

#define NOR_VREG_OFFSET              (0x80U)
#define NOR_NVREG_OFFSET             (0x0U)

#define NOR_STS1_NVREG_ADDR          (0x0U)
#define NOR_STS2_NVREG_ADDR          (0x1U)
#define NOR_CFG1_NVREG_ADDR          (0x2U)
#define NOR_CFG2_NVREG_ADDR          (0x3U)
#define NOR_CFG3_NVREG_ADDR          (0x4U)
#define NOR_CFG4_NVREG_ADDR          (0x5U)
#define NOR_CFG5_NVREG_ADDR          (0x6U)

#define NOR_STS1_VREG_ADDR           (0x800000U)
#define NOR_STS2_VREG_ADDR           (0x800001U)
#define NOR_CFG1_VREG_ADDR           (0x800002U)
#define NOR_CFG2_VREG_ADDR           (0x800003U)
#define NOR_CFG3_VREG_ADDR           (0x800004U)
#define NOR_CFG4_VREG_ADDR           (0x800005U)
#define NOR_CFG5_VREG_ADDR           (0x800006U)

/** Read ID command definitions */
#define NOR_RDID_NUM_BYTES           (0x3U)
#define NOR_MANF_ID                  (0x34U)   /* Manufacturer ID */
#define NOR_DEVICE_ID                (0x5B1A)  /* Device ID */

/** Status Register, Write-in-Progress bit */
#define NOR_SR_WIP			         (1U << 0U)

/** Status Register, program enabled bit */
#define NOR_SR_WRPGEN			     (1U << 1U)

/** Config Register, TBPARM bit */
#define NOR_CR_TBPARM                (1U << 2U)

/** Dummy cycles for Read operation */
#define NOR_SINGLE_READ_DUMMY_CYCLE           (0U)
#define NOR_SINGLE_CMD_READ_DUMMY_CYCLE       (1U)
#define NOR_OCTAL_SDR_CMD_READ_DUMMY_CYCLE    (3U)
#define NOR_OCTAL_DDR_CMD_READ_DUMMY_CYCLE    (4U)
#define NOR_OCTAL_READ_DUMMY_CYCLE            (24U)
#define NOR_OCTAL_READ_DUMMY_CYCLE_LC         (0xBU)
#define NOR_OCTAL_READ_DUMMY_CYCLE_INDAC      (20U)
#define NOR_OCTAL_READ_DUMMY_CYCLE_LC_INDAC   (0x8U)

#define NOR_RDID_CMD_LENGTH_SINGLE            (1U)
#define NOR_RDID_CMD_LENGTH_OCTAL             (5U)

/** In Micro seconds */
#define NOR_PAGE_PROG_TIMEOUT		(400U)
#define NOR_SECTOR_ERASE_TIMEOUT	(600U * 1000U)
#define NOR_WRR_WRITE_TIMEOUT		(600U * 1000U)
#define NOR_BULK_ERASE_TIMEOUT	    (110U * 1000U * 1000U)

#ifdef __cplusplus
}
#endif

#endif /* S25Fl512S_H_ */

/* Nothing past this point */
