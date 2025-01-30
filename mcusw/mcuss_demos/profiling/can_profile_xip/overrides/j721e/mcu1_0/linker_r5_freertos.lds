/*----------------------------------------------------------------------------*/
/* File: k3_r5f_linker.cmd                                                    */
/* Description:																  */
/*    Link command file for j721e R5F MCU 0 view							  */
/*	  TI ARM Compiler version 15.12.3 LTS or later							  */
/*                                                                            */
/* (c) Texas Instruments 2020, All rights reserved.                           */
/*----------------------------------------------------------------------------*/
/* Linker Settings                                                            */
/* Standard linker options

--retain="*(.bootCode)"
--retain="*(.startupCode)"
--retain="*(.startupData)"													  */
--retain="*(.intvecs)"
--retain="*(.intc_text)"
--retain="*(.rstvectors)"
--fill_value=0
-e __VECS_ENTRY_POINT
/*----------------------------------------------------------------------------*/
/* Memory Map                                                                 */

--stack_size=0x8000
--heap_size=0x4000
--entry_point=_freertosresetvectors

-stack  0x8000  /* SOFTWARE STACK SIZE */
-heap   0x4000  /* HEAP AREA SIZE      */

/*-------------------------------------------*/
/*       Stack Sizes for various modes       */
/*-------------------------------------------*/
__IRQ_STACK_SIZE   = 0x1000;
__FIQ_STACK_SIZE   = 0x0100;
__ABORT_STACK_SIZE = 0x0100;
__UND_STACK_SIZE   = 0x0100;
__SVC_STACK_SIZE   = 0x2000;

--define FILL_PATTERN=0xFEAA55EF
--define FILL_LENGTH=0x100

/* 1 MB of MCU Domain MSRAM is split as shown below */
/* Size used  F0000 Number of slices 4 */
/*                                  Rounding Offset */
/*SBL?      Start   41C00000    245760  0   */
/*          End     41C3C000                */
/*MCU 10    Start   41C3C100    245760  100 */
/*          End     41C78100                */
/*MCU 11    Start   41C78200    245760  100 */
/*          End     41CB4200                */

--define VECS_LEN=0x00

MEMORY
{
    VECTORS (X)                 : ORIGIN = 0x00000000 LENGTH = 0x00000040
    MCU0_R5F_TCMB0_VECS (RWIX)	: origin=0x41010000	length=0x100
    MCU0_R5F_TCMB0 (RWIX)	: origin=0x41010100	length=0x8000-0x100
    RESERVED (X)            : origin=0x41C3E000 length=0x2000
    /* Refer the user guide for details on persistence of these sections */
    OCMC_RAM_BOARD_CFG (RWIX)   : origin=0x41C80000 length=0x2000
    OCMC_RAM_SCISERVER (RWIX)   : origin=0x41C82000 length=0x80000-0x2000

    XIP_FLASH               : origin=0x501c0000+VECS_LEN length=0x400000 - VECS_LEN
    /* j7200 MCMS3 locations */
    /* j7200 Reserved Memory for ARM Trusted Firmware */
    MSMC3_ARM_FW   (RWIX)   : origin=0x70000000 length=0x40000         /* 256KB */
    MSMC3   (RWIX)          : origin=0x70040000 length=0xB0000        /* 1MB - 320KB */
    /* j7200 Reserved Memory for DMSC Firmware */
    MSMC3_DMSC_FW  (RWIX)   : origin=0x700F0000 length=0x10000         /* 64KB */

}  /* end of MEMORY */

/*----------------------------------------------------------------------------*/
/* Section Configuration                                                      */

SECTIONS
{

    .freertosrstvectors : {} palign(8) > MCU0_R5F_TCMB0_VECS

    .bootCode           : {} palign(8) > MCU0_R5F_TCMB0
    .startupCode        : {} palign(8) > MCU0_R5F_TCMB0
    .startupData        : {} palign(8) > MCU0_R5F_TCMB0, type = NOINIT

    .text_boot {
        r5_mpu_freertos.oer5f (.rodata.gCslR5MpuCfg)
    }                                      > MCU0_R5F_TCMB0

    GROUP {
        .text.hwi       : palign(8)
        .text.cache     : palign(8)
        .text.mpu       : palign(8)
        .text.boot      : palign(8)
        .text.abort     : palign(8)
    }                                      > MCU0_R5F_TCMB0
    .cinit   	        : {} palign(8)     > OCMC_RAM_SCISERVER
    .bss     	        : {} align(4)      > OCMC_RAM_SCISERVER
    .data    	        : {} palign(128)   > OCMC_RAM_SCISERVER
    .sysmem             : {} palign(8)     > OCMC_RAM_SCISERVER
    .bss.devgroup*      : {} align(4)      > OCMC_RAM_SCISERVER
    .data_user          : {} align(4)      > OCMC_RAM_SCISERVER
    .boardcfg_data      : {} align(4)      > OCMC_RAM_SCISERVER
    .const   	        : {} palign(8)     > XIP_FLASH
    .rodata           : {} palign(8)     > XIP_FLASH
    .const.devgroup*    : {} align(4)      > XIP_FLASH
    .text               : {} palign(8)     > XIP_FLASH

	.stack  	: {} align(4)		> OCMC_RAM_SCISERVER (HIGH)
    .irqStack   : {. = . + __IRQ_STACK_SIZE;} align(4)      > OCMC_RAM_SCISERVER  (HIGH)
    RUN_START(__IRQ_STACK_START)
    RUN_END(__IRQ_STACK_END)

    .fiqStack   : {. = . + __FIQ_STACK_SIZE;} align(4)      > OCMC_RAM_SCISERVER  (HIGH)
    RUN_START(__FIQ_STACK_START)
    RUN_END(__FIQ_STACK_END)

    .abortStack : {. = . + __ABORT_STACK_SIZE;} align(4)    > OCMC_RAM_SCISERVER  (HIGH)
    RUN_START(__ABORT_STACK_START)
    RUN_END(__ABORT_STACK_END)

    .undStack   : {. = . + __UND_STACK_SIZE;} align(4)      > OCMC_RAM_SCISERVER  (HIGH)
    RUN_START(__UND_STACK_START)
    RUN_END(__UND_STACK_END)

    .svcStack   : {. = . + __SVC_STACK_SIZE;} align(4)      > OCMC_RAM_SCISERVER  (HIGH)
    RUN_START(__SVC_STACK_START)
    RUN_END(__SVC_STACK_END)

    /* Additional sections settings     */
    McalTextSection : align=4, load > XIP_FLASH
    {
        .=align(4);
        __linker_dio_text_start = .;
        *(DIO_TEXT_SECTION)
        .=align(4);
        __linker_dio_text_end = .;

        .=align(4);
        __linker_can_text_start = .;
        *(CAN_TEXT_SECTION)
        *(CAN_ISR_TEXT_SECTION)
        .=align(4);
        __linker_can_text_end = .;

    }
    McalConstSection : align=4, load > OCMC_RAM_SCISERVER
    {
        .=align(4);
        __linker_dio_const_start = .;
        *(DIO_CONST_32_SECTION)
        *(DIO_CONST_UNSPECIFIED_SECTION)
        *(DIO_CONFIG_SECTION)
        .=align(4);
        __linker_dio_const_end = .;

        .=align(4);
        __linker_can_const_start = .;
        *(CAN_CONST_8_SECTION)
        *(CAN_CONST_32_SECTION)
        *(CAN_CONFIG_SECTION)
        .=align(4);
        __linker_can_const_end = .;
    }

    McalInitSection : align=4, load > OCMC_RAM_SCISERVER
    {
        .=align(4);
        __linker_dio_init_start = .;
        *(DIO_DATA_INIT_32_SECTION)
        .=align(4);
        __linker_dio_init_end = .;

        .=align(4);
        __linker_can_init_start = .;
        *(CAN_DATA_INIT_8_SECTION)
        .=align(4);
        __linker_can_init_end = .;
    }
    McalNoInitSection : align=4, load > OCMC_RAM_SCISERVER, type = NOINIT
    {
        .=align(4);
        __linker_dio_no_init_start = .;
        *(DIO_DATA_NO_INIT_UNSPECIFIED_SECTION)
        .=align(4);
        __linker_dio_no_init_end = .;

        .=align(4);
        __linker_can_no_init_start = .;
        *(CAN_DATA_NO_INIT_UNSPECIFIED_SECTION)
        *(CAN_DATA_NO_INIT_32_SECTION)
        .=align(4);
        __linker_can_no_init_end = .;

    }
    /* Example Utility specifics */
    UtilityNoInitSection : align=4, load > OCMC_RAM_SCISERVER, type = NOINIT
    {
        .=align(4);
        __linker_utility_no_init_start = .;
        *(EG_TEST_RESULT_32_SECTION)
        .=align(4);
        __linker_utility_no_init_end = .;
    }
    SciClientBoardCfgSection : align=128, load > XIP_FLASH, type = NOINIT
    {
        .=align(128);
        __linker_boardcfg_data_start = .;
        . += FILL_LENGTH;
        *(.boardcfg_data)
        .=align(128);
        . += FILL_LENGTH;
        __linker_boardcfg_data_end = .;
    }
}  /* end of SECTIONS */

/*----------------------------------------------------------------------------*/
/* Misc linker settings                                                       */


/*-------------------------------- END ---------------------------------------*/
