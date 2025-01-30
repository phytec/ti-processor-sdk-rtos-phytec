/*
 * SDL TEST
 *
 * Software Diagnostics Library Test
 *
 *  Copyright (c) Texas Instruments Incorporated 2018-2020
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

/*----------------------------------------------------------------------------*/
/* File: linker_r5_ecc_freertos.lds                                           */
/* Description:                                                               */
/*    Link command file for AM65XX M4 MCU 0 view                              */
/*----------------------------------------------------------------------------*/
/* Linker Settings                                                            */
/* Standard linker options                                                    */
--retain="*(.bootCode)"
--retain="*(.startupCode)"
--retain="*(.startupData)"
--retain="*(.irqStack)"
--retain="*(.fiqStack)"
--retain="*(.abortStack)"
--retain="*(.undStack)"
--retain="*(.svcStack)"
--fill_value=0
--stack_size=0x4000
--heap_size=0x8000
--entry_point=_freertosresetvectors

-stack  0x4000                              /* SOFTWARE STACK SIZE           */
-heap   0x8000                              /* HEAP AREA SIZE                */

/* Stack Sizes for various modes */
__IRQ_STACK_SIZE = 0x1000;
__FIQ_STACK_SIZE = 0x0100;
__ABORT_STACK_SIZE = 0x0100;
__UND_STACK_SIZE = 0x0100;
__SVC_STACK_SIZE = 0x0100;

/*----------------------------------------------------------------------------*/
/* Memory Map                                                                 */
MEMORY
{
    VECTORS (X)                 : ORIGIN = 0x00000000 LENGTH = 0x00000040

    /* MCU0_R5F_0 local view                                                  */
    MCU0_R5F_TCMA (X)       : origin=0x40       length=0x7fc0
    MCU0_R5F_TCMB0 (RWIX)   : origin=0x41010000 length=0x8000

    /* MCU0_R5F_1 SoC view                                                    */
    MCU0_R5F1_ATCM (RWIX)   : origin=0x41400000 length=0x8000
    MCU0_R5F1_BTCM (RWIX)   : origin=0x41410000 length=0x8000

    /* Refer the user guide for details on persistence of these sections */
    OCMC_RAM_BOARD_CFG (RWIX)   : origin=0x41C80000 length=0x2000
    OCMC_RAM_SCISERVER (RWIX)   : origin=0x41C82000 length=0x60000
    OCMC_RAM (RWIX)             : origin=0x41CE3100 length=0x1CA00
    OCMC_RAM_X509_HEADER (RWIX) : origin=0x41CFFB00 length=0x500

    ECC_CFG_SCRATCH  (RWIX)  : origin=0x60000000 length=0x400            /* 4 K */
    
    MSMC3_ARM_FW (RWIX)     : origin=0x70000000 length=0x40000           /* 256 K */
    MSMC3   (RWIX)          : origin=0x70040000 length=0x7B0000          /* 8MB - 320KB */
    /* Reserved for DMSC */
    MSMC3_DMSC (RWIX)       : origin=0x707F0000 length=0x10000          /* 64KB */
    DDR0    (RWIX)          : origin=0x80000000 length=0x80000000       /* 2GB */

/* Additional memory settings   */

}  /* end of MEMORY */

/*----------------------------------------------------------------------------*/
/* Section Configuration                                                      */

SECTIONS
{
    .freertosrstvectors      : {} palign(8)      > VECTORS
    .bootCode   : {} palign(8)      > MSMC3
    .startupCode: {} palign(8)      > MSMC3
    .startupData: {} palign(8)      > MSMC3, type = NOINIT
    .text       : {} palign(8)      > MSMC3
    GROUP {
        .text.hwi    : palign(8)
        .text.cache  : palign(8)
        .text.mpu    : palign(8)
        .text.boot   : palign(8)
    }                               > MSMC3  
    .const      : {} palign(8)      > MSMC3
    .rodata     : {} palign(8)      > MSMC3
    .cinit      : {} palign(8)      > MSMC3
    .bss        : {} align(4)       > MSMC3
    .far        : {} align(4)       > MSMC3
    .data       : {} palign(128)    > MSMC3
    .sysmem     : {}                > MSMC3
    .data_buffer : {} palign(128)   > MSMC3
    .bss.devgroup : {*(.bss.devgroup*)} align(4)       > MSMC3
    .const.devgroup : {*(.const.devgroup*)} align(4)      > MSMC3
    .my_aggr_reg          : {*(.my_aggr_reg)}        > ECC_CFG_SCRATCH, type = NOINIT
    .boardcfg_data        : {} palign(128)           > MSMC3
    .stack      : {} align(4)       > MSMC3  (HIGH)
    .irqStack   : {. = . + __IRQ_STACK_SIZE;} align(4)      > MSMC3  (HIGH)
    RUN_START(__IRQ_STACK_START)
    RUN_END(__IRQ_STACK_END)
    .fiqStack   : {. = . + __FIQ_STACK_SIZE;} align(4)      > MSMC3  (HIGH)
    RUN_START(__FIQ_STACK_START)
    RUN_END(__FIQ_STACK_END)
    .abortStack     : {. = . + __ABORT_STACK_SIZE;} align(4)        > MSMC3  (HIGH)
    RUN_START(__ABORT_STACK_START)
    RUN_END(__ABORT_STACK_END)
    .undStack   : {. = . + __UND_STACK_SIZE;} align(4)      > MSMC3  (HIGH)
    RUN_START(__UND_STACK_START)
    RUN_END(__UND_STACK_END)
    .svcStack   : {. = . + __SVC_STACK_SIZE;} align(4)      > MSMC3  (HIGH)
    RUN_START(__SVC_STACK_START)
    RUN_END(__SVC_STACK_END)

}  /* end of SECTIONS */

/*----------------------------------------------------------------------------*/
/* Misc linker settings                                                       */


/*-------------------------------- END ---------------------------------------*/
