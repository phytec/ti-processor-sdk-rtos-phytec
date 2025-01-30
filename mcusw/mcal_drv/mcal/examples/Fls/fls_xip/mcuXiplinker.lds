/*----------------------------------------------------------------------------*/
/* File: mcuXiplinker.cmd                                                           */
/* Description:								      */
/*    Link command file for Maxwell Multicore Testcse			      */
/*                                                                            */
/* (c) Texas Instruments 2019, All rights reserved.                           */
/*----------------------------------------------------------------------------*/

--entry_point=_sblTestResetVectors
--stack_size=0x800

/* Stack Sizes for various modes
__ABORT_STACK_SIZE = 0x1000;*/

MEMORY
{
    OSPI_MCU1_CPU0_XIP_VEC      : origin=0x500e0000 length=0x40			/* 64B */
    OSPI_MCU1_CPU0_XIP	        : origin=0x500e0040 length=0x2000 - 0x40	/* 8KB - 64B*/
    MCU1_0_MSRAM            : origin=0x41C40100 length=0x5C000 - 0x100
    MSMC3_MCU1_CPU0	        : origin=0x70010800 length=0x2000 - 0x800	/* 8KB */
}

SECTIONS
{
    .rstvectors			: {} palign(8) 		> OSPI_MCU1_CPU0_XIP_VEC
    .text	: {} palign(8) 		> OSPI_MCU1_CPU0_XIP, type = NOINIT
    .sbl_mcu_1_0_resetvector	: {} palign(8) 		> OSPI_MCU1_CPU0_XIP, type = NOINIT
    .stack 		: {} align(8) 		> MCU1_0_MSRAM
}
