/*
 *
 * Copyright (c) 2018 Texas Instruments Incorporated
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
-c
-heap 0x1e000000
-stack 0x20000
-e _c_int00_secure

--symbol_map=HOSTopen=fileio_HOSTopen
--symbol_map=HOSTread=fileio_HOSTread
--symbol_map=HOSTclose=fileio_HOSTclose
--symbol_map=HOSTwrite=fileio_HOSTwrite
--symbol_map=HOSTlseek=fileio_HOSTlseek
--symbol_map=setvbuf=fileio_setvbuf


SECTIONS
{
    .vecs          >    DDR_C7x_1_VECS  ALIGN(0x200000)
    .secure_vecs   >    DDR_C7x_1_SECURE_VECS  ALIGN(0x200000)
    .text:_c_int00_secure > DDR_C7x_1_BOOT  ALIGN(0x200000)
    .mmlib_code : { mma*.lib<*.obj>(.text) } > DDR_C7x_1
    .tidl_code : { tidl*.lib<*.obj>(.text) } > DDR_C7x_1
     GROUP (frequenty_accessed_functions)
     {
        .text:tidl_printf
        .text:DmaUtilsAutoInc3d_prepareTr
        .text:DmaUtilsAutoInc3d_configure
        .text:DmaUtilsAutoInc3d_trigger
        .text:DmaUtilsAutoInc3d_wait
        .text:DmaUtilsAutoInc3d_getTrMemReq
	    .text:DmaUtilsAutoInc3d_setupTr
	    .text:DmaUtilsAutoInc3d_setupTr
        .text:DmaUtilsAutoInc3d_getTotalBlockCount
	    .text:Udma_chDruSubmitTr
	    .text:Udma_chEnable
        .mmlib_code 
        .tidl_code
     } align = 0x80 > DDR_C7x_1
    .text          >    DDR_C7x_1

    .bss        >       DDR_C7x_1  /* Zero-initialized data */
    .data       >       DDR_C7x_1  /* Initialized data */

    .cinit      >       DDR_C7x_1  /* could be part of const */
    .init_array >       DDR_C7x_1  /* C++ initializations */
    .stack      >       DDR_C7x_1  ALIGN(0x20000) /* MUST be 128KB aligned to handle nested interrupts */
    .args       >       DDR_C7x_1
    .cio        >       DDR_C7x_1
    .const      >       DDR_C7x_1
    .switch     >       DDR_C7x_1
    .bss:taskStackSection       > DDR_C7x_1

    .sysmem                 (NOLOAD) : {} > DDR_SHARED_MEM /* heap */
    .l4ScratchBuf           (NOLOAD) : {} > DDR_C7X_1_SCRATCH

    .l1ScratchBuf           (NOLOAD)(NOINIT) : {} > L1RAM_C7x_1
    .l2ScratchBuf           (NOLOAD)(NOINIT) : {} > L2RAM_C7x_1
    .l3ScratchBuf           (NOLOAD)(NOINIT) : {} > MSMC_C7x_1

    .resource_table > DDR_C7x_1_RESOURCE_TABLE

    GROUP:              >  DDR_C7x_1
    {
        .data.ti_sysbios_family_c7x_Mmu_tableArray : type=NOINIT
        .data.ti_sysbios_family_c7x_Mmu_tableArraySlot : type=NOINIT
        .data.ti_sysbios_family_c7x_Mmu_level1Table : type=NOINIT

        .data.ti_sysbios_family_c7x_Mmu_tableArray_NS : type=NOINIT
        .data.ti_sysbios_family_c7x_Mmu_tableArraySlot_NS : type=NOINIT
        .data.ti_sysbios_family_c7x_Mmu_level1Table_NS : type=NOINIT

    }

    ipc_data_buffer             > DDR_C7x_1
    .createParams               > DDR_C7x_1
    .gIOParams                  > DDR_C7x_1


}
