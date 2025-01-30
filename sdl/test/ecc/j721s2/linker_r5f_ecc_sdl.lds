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
/* File: linker_r5.lds                                                  */
/* Description:                                                               */
/*    Link command file for AM65XX M4 MCU 0 view                              */
/*----------------------------------------------------------------------------*/
/* Linker Settings                                                            */
/* Standard linker options                                                    */
--retain="*(.intvecs)"
--retain="*(.intc_text)"
--retain="*(.rstvectors)"
--retain="*(.irqStack)"
--retain="*(.fiqStack)"
--retain="*(.abortStack)"
--retain="*(.undStack)"
--retain="*(.svcStack)"
--fill_value=0
--stack_size=0x2000
--heap_size=0x1000
--entry_point=_resetvectors     /* Default C RTS boot.asm   */

-stack  0x2000                              /* SOFTWARE STACK SIZE           */
-heap   0x2000                              /* HEAP AREA SIZE                */

/* Stack Sizes for various modes */
__IRQ_STACK_SIZE = 0x1000;
__FIQ_STACK_SIZE = 0x1000;
__ABORT_STACK_SIZE = 0x1000;
__UND_STACK_SIZE = 0x1000;
__SVC_STACK_SIZE = 0x1000;

/*----------------------------------------------------------------------------*/
/* Memory Map                                                                 */
MEMORY
{
    /* MCU0_R5F_0 local view                                                  */
    MCU0_R5F_TCMA (X)       : origin=0x0            length=0x7c00
    MCU0_R5F_TCMA_TEST (X)      : origin=0x7c00         length=0x400
    MCU0_R5F_TCMB0 (RWIX)   : origin=0x41010000 length=0x8000

    /* MCU0_R5F_1 SoC view                                                    */
    MCU0_R5F1_ATCM (RWIX)   : origin=0x41400000 length=0x8000
    MCU0_R5F1_BTCM (RWIX)   : origin=0x41410000 length=0x8000

    /* Refer the user guide for details on persistence of these sections */
    OCMC_RAM_BOARD_CFG (RWIX)   : origin=0x41C80000 length=0x2000
    OCMC_RAM_SCISERVER (RWIX)   : origin=0x41C82000 length=0x60000
    VECTORS (X)                 : origin=0x41CE2000 length=0x1000
    RESET_VECTORS (X)           : origin=0x41CE3000 length=0x100
    OCMC_RAM (RWIX)             : origin=0x41CE3100 length=0x1AA00
    OCMC_RAM_X509_HEADER (RWIX) : origin=0x41CFDB00 length=0x500

    /* MCU0 Location RESERVED for SDL RAT Self Test */
    SDL_OCMC_RAT_SELFTEST (R) : origin=0x41C7EFC0 length=32

    /* MCU0 Location RESERVED for SDL MPU Self Test */
    SDL_OCMC_MPU_SELFTEST (R) : origin=0x41C7EFE0 length=32

    ECC_CFG_SCRATCH  (RWIX)  : origin=0x60000000 length=0x400            /* 4 K */
    ECC_CFG_SCRATCH1 (RWIX)  : origin=0x60000400 length=0x400            /* 4 K */
    ECC_CFG_SCRATCH2 (RWIX)  : origin=0x60001000 length=0x1000            /* 4 K */
    ECC_CFG_SCRATCH3 (RWIX)  : origin=0x60002000 length=0x1000           /* 4 K */
    ECC_CFG_SCRATCH4 (RWIX)  : origin=0x60003000 length=0x1000           /* 4 K */
    ECC_CFG_SCRATCH5 (RWIX)  : origin=0x60004000 length=0x1000           /* 4 K */
    ECC_CFG_SCRATCH6 (RWIX)  : origin=0x60005000 length=0x400            /* 4 K */
    ECC_CFG_SCRATCH7 (RWIX)  : origin=0x60005400 length=0x400            /* 4 K */
    ECC_CFG_SCRATCH8 (RWIX)  : origin=0x60005800 length=0x400            /* 4 K */
    ECC_CFG_SCRATCH9 (RWIX)  : origin=0x60005c00 length=0x400            /* 4 K */

    /* j721s2 MCMS3 locations */
    /* j721s2 Reserved Memory for ARM Trusted Firmware */
    MSMC3_ARM_FW   (RWIX)   : origin=0x70000000 length=0x20000         /* 128KB */
    MSMC3   (RWIX)          : origin=0x70020000 length=0x3D0000        /* 4MB - 192KB */
    /* j721s2 Reserved Memory for DMSC Firmware */
    MSMC3_DMSC_FW  (RWIX)   : origin=0x703F0000 length=0x10000         /* 64KB */

    DDR0    (RWIX)          : origin=0x80000000 length=0x40000000      /* 2GB */

/* Additional memory settings   */

}  /* end of MEMORY */

/*----------------------------------------------------------------------------*/
/* Section Configuration                                                      */

SECTIONS
{
/* 'intvecs' and 'intc_text' sections shall be placed within                  */
/* a range of +\- 16 MB                                                       */
    .intvecs    : {} palign(8)      > VECTORS
    .intc_text  : {} palign(8)      > VECTORS
    .rstvectors     : {} palign(8)      > RESET_VECTORS

    .bootCode   : {} palign(8)      > MSMC3
    .startupCode: {} palign(8)      > MSMC3
    .startupData: {} palign(8)      > MSMC3, type = NOINIT
    .text       : {} palign(8)      > MSMC3
    .const      : {} palign(8)      > MSMC3
    .rodata     : {} palign(8)      > MSMC3
    .cinit      : {} palign(8)      > MSMC3
    .pinit      : {} palign(8)      > MSMC3
    .bss        : {} align(4)       > MSMC3
    .data       : {} palign(128)    > MSMC3
    .bss.devgroup : {*(.bss.devgroup*)} align(4)      > MSMC3
    .const.devgroup : {*(.const.devgroup*)} align(4)    > MSMC3
    .my_aggr_reg           : {*(.my_aggr_reg)}         > ECC_CFG_SCRATCH, type = NOINIT
    .my_aggr_reg1          : {*(.my_aggr_reg1)}        > ECC_CFG_SCRATCH1, type = NOINIT
    .my_aggr_reg2          : {*(.my_aggr_reg2)}        > ECC_CFG_SCRATCH2, type = NOINIT
    .my_aggr_reg3          : {*(.my_aggr_reg3)}        > ECC_CFG_SCRATCH3, type = NOINIT
    .my_aggr_reg4          : {*(.my_aggr_reg4)}        > ECC_CFG_SCRATCH4, type = NOINIT
    .my_aggr_reg5          : {*(.my_aggr_reg5)}        > ECC_CFG_SCRATCH5, type = NOINIT
    .my_aggr_reg6          : {*(.my_aggr_reg6)}        > ECC_CFG_SCRATCH6, type = NOINIT
    .my_aggr_reg7          : {*(.my_aggr_reg7)}        > ECC_CFG_SCRATCH7, type = NOINIT
    .my_aggr_reg8          : {*(.my_aggr_reg8)}        > ECC_CFG_SCRATCH8, type = NOINIT
    .my_aggr_reg9          : {*(.my_aggr_reg9)}        > ECC_CFG_SCRATCH9, type = NOINIT

    .boardcfg_data        : {} palign(128)           > MSMC3
    .sysmem     : {}                > MSMC3
    .bss:extMemCache:ramdisk : {} align (32)     > DDR0
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

/* Additional sections settings     */
    .ecc_test_rat_testsection : {} palign(32)   > SDL_OCMC_RAT_SELFTEST

    .ecc_test_ecc_testcodesection : {} palign(32)   > MCU0_R5F_TCMA_TEST

}  /* end of SECTIONS */

/*----------------------------------------------------------------------------*/
/* Misc linker settings                                                       */


/*-------------------------------- END ---------------------------------------*/
