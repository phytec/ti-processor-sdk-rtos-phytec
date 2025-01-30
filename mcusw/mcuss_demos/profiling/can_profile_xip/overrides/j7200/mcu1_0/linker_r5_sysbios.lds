/*----------------------------------------------------------------------------*/
/* File: k3_r5f_linker.cmd                                                    */
/* Description:																  */
/*    Link command file for j721e R5F MCU 0 view							  */
/*	  TI ARM Compiler version 15.12.3 LTS or later							  */
/*                                                                            */
/* (c) Texas Instruments 2020, All rights reserved.                           */
/*----------------------------------------------------------------------------*/
/* Linker Settings                                                            */
/* Standard linker options													  */
--retain="*(.intvecs)"
--retain="*(.intc_text)"
--retain="*(.rstvectors)"
--fill_value=0
-e __VECS_ENTRY_POINT
/*----------------------------------------------------------------------------*/
/* Memory Map                                                                 */

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

MEMORY
{
    MCU0_R5F_TCMB0 (RWIX)	: origin=0x41010000	length=0x8000
    RESERVED (X)            : origin=0x41C3E000 length=0x2000
    /* Refer the user guide for details on persistence of these sections */
    OCMC_RAM_BOARD_CFG (RWIX)   : origin=0x41C80000 length=0x2000
    OCMC_RAM_SCISERVER (RWIX)   : origin=0x41C82000 length=0x60000
    RESET_VECTORS (X)           : origin=0x41CE2000 length=0x100
    OCMC_RAM (RWIX)             : origin=0x41CE2100 length=0x1DA00
    OCMC_RAM_X509_HEADER (RWIX) : origin=0x41CFFB00 length=0x500

    XIP_FLASH               : origin=0x501C0000 length=0x40000
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
    .vecs : {
         __VECS_ENTRY_POINT = .;
    } palign(8) > RESET_VECTORS
    .text_boot {
        *boot.aer5f*<*boot.o*>(.text)
     }  palign(8)   > MCU0_R5F_TCMB0
    .text:xdc_runtime_Startup_reset__I      : {} palign(8) > MCU0_R5F_TCMB0
    .text:ti_sysbios_family_arm_v7r_Cache*  : {} palign(8) > MCU0_R5F_TCMB0
    .const:ti_sysbios_family_arm_v7r_Cache* : {} palign(8) > MCU0_R5F_TCMB0
    .text:ti_sysbios_family_arm_MPU*        : {} palign(8) > MCU0_R5F_TCMB0
    .const:ti_sysbios_family_arm_MPU*       : {} palign(8) > MCU0_R5F_TCMB0

    .text    	: {} palign(8) 		> XIP_FLASH
    .const   	: {} palign(8) 		> XIP_FLASH
    .cinit   	: {} palign(8) 		> OCMC_RAM_SCISERVER
    .bss     	: {} align(4)  		> OCMC_RAM_SCISERVER
    .data    	: {} palign(128) 	> OCMC_RAM_SCISERVER
	.stack  	: {} align(4)		> OCMC_RAM_SCISERVER
    .bss.devgroup* : {} align(4)       > OCMC_RAM_SCISERVER
    .const.devgroup*: {} align(4)      > XIP_FLASH
    .data_user      : {} align(4)      > OCMC_RAM_SCISERVER
    .boardcfg_data  : {} align(4)      > OCMC_RAM_SCISERVER
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
