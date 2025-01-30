/*----------------------------------------------------------------------------*/
/* File: linker_r5f_mcu1_1_freertos.lds                                       */
/* Description:			  					      */
/*    Link command file for J7ES MCU1_1 view				      */
/*                                                                            */
/* (c) Texas Instruments 2018-2022, All rights reserved.                      */
/*----------------------------------------------------------------------------*/
/*  History:								      */
/*    Aug 26th, 2016 Original version .......................... Loc Truong   */
/*    Aug 01th, 2017 new TCM mem map  .......................... Loc Truong   */
/*    Nov 07th, 2017 Changes for R5F Init Code.................. Vivek Dhande */
/*    Sep 17th, 2018 Added DDR sections for IPC................. J. Bergsagel */
/*    Sep 26th, 2018 Extra mem sections for IPC resource table.. J. Bergsagel */
/*    Nov 06th, 2018 Linker command file for MCU1_1............. J. Bergsagel */
/*    Apr 23th, 2019 Changes for R5F startup Code............... Vivek Dhande */
/*----------------------------------------------------------------------------*/
/*=========================*/
/*     Linker Settings     */
/*=========================*/

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

-stack  0x4000  /* SOFTWARE STACK SIZE */
-heap   0x8000  /* HEAP AREA SIZE      */

/*-------------------------------------------*/
/*       Stack Sizes for various modes       */
/*-------------------------------------------*/
__IRQ_STACK_SIZE   = 0x1000;
__FIQ_STACK_SIZE   = 0x0100;
__ABORT_STACK_SIZE = 0x0100;
__UND_STACK_SIZE   = 0x0100;
__SVC_STACK_SIZE   = 0x0100;

#define DDR0_ALLOCATED_START 0xA0000000

#define MCU1_0_EXT_DATA_BASE     (DDR0_ALLOCATED_START + 0x00100000)
#define MCU1_0_R5F_MEM_TEXT_BASE (DDR0_ALLOCATED_START + 0x00200000)
#define MCU1_0_R5F_MEM_DATA_BASE (DDR0_ALLOCATED_START + 0x00300000)
#define MCU1_0_DDR_SPACE_BASE    (DDR0_ALLOCATED_START + 0x00400000)

#define MCU1_1_ALLOCATED_START   DDR0_ALLOCATED_START + 0x01000000
#define MCU1_1_EXT_DATA_BASE     (MCU1_1_ALLOCATED_START + 0x00100000)
#define MCU1_1_R5F_MEM_TEXT_BASE (MCU1_1_ALLOCATED_START + 0x00200000)
#define MCU1_1_R5F_MEM_DATA_BASE (MCU1_1_ALLOCATED_START + 0x00300000)
#define MCU1_1_DDR_SPACE_BASE    (MCU1_1_ALLOCATED_START + 0x00400000)

#define MCU2_0_ALLOCATED_START   DDR0_ALLOCATED_START + 0x02000000
#define MCU2_0_EXT_DATA_BASE     (MCU2_0_ALLOCATED_START + 0x00100000)
#define MCU2_0_R5F_MEM_TEXT_BASE (MCU2_0_ALLOCATED_START + 0x00200000)
#define MCU2_0_R5F_MEM_DATA_BASE (MCU2_0_ALLOCATED_START + 0x00300000)
#define MCU2_0_DDR_SPACE_BASE    (MCU2_0_ALLOCATED_START + 0x00400000)

#define MCU2_1_ALLOCATED_START   DDR0_ALLOCATED_START + 0x03000000
#define MCU2_1_EXT_DATA_BASE     (MCU2_1_ALLOCATED_START + 0x00100000)
#define MCU2_1_R5F_MEM_TEXT_BASE (MCU2_1_ALLOCATED_START + 0x00200000)
#define MCU2_1_R5F_MEM_DATA_BASE (MCU2_1_ALLOCATED_START + 0x00300000)
#define MCU2_1_DDR_SPACE_BASE    (MCU2_1_ALLOCATED_START + 0x00400000)

#define ATCM_START 0x00000040
#define OCMRAM_MCU1_0_START 0x41cfe000
#define OCMRAM_MCU1_1_START 0x41cff000


/*----------------------------------------------------------------------------*/
/* Memory Map                                                                 */
MEMORY
{
    VECTORS (X)                : ORIGIN = 0x00000000 LENGTH = 0x00000040
    /* MCU1_R5F_1 local view  */
    MCU_ATCM (RWX)		: origin=ATCM_START	length=0x8000
    MCU1_R5F1_TCMB1 (RWIX)	: origin=0x41010000	length=0x8000

    /* MCU1_R5F_1 SoC view  */
    MCU1_R5F1_ATCM (RWIX)  	: origin=0x41400000 length=0x8000
    MCU1_R5F1_BTCM (RWIX) 	: origin=0x41410000 length=0x8000

    /* MCUSS RAM - Start towards the end to avoid Bootloader usage of the SRAM */
    OCMRAM_MCU1_0 (RWIX)        : origin=OCMRAM_MCU1_0_START length=0x1000
    OCMRAM_MCU1_1 (RWIX)        : origin=OCMRAM_MCU1_1_START length=0x1000

    DDR0_RESERVED    (RWIX)  	: origin=0x80000000 length=0x20000000	  	/* 512MB */
    MCU1_0_IPC_DATA (RWIX)	: origin=DDR0_ALLOCATED_START     length=0x00100000	/*   1MB */
    MCU1_0_EXT_DATA  (RWIX)	: origin=MCU1_0_EXT_DATA_BASE     length=0x00100000	/*   1MB */
    MCU1_0_R5F_MEM_TEXT (RWIX)	: origin=MCU1_0_R5F_MEM_TEXT_BASE length=0x00100000	/*   1MB */
    MCU1_0_R5F_MEM_DATA (RWIX)	: origin=MCU1_0_R5F_MEM_DATA_BASE length=0x00100000	/*   1MB */
    MCU1_0_DDR_SPACE (RWIX)	: origin=MCU1_0_DDR_SPACE_BASE    length=0x00C00000	/*  12MB */
    MCU1_1_IPC_DATA (RWIX)	: origin=MCU1_1_ALLOCATED_START   length=0x00100000	/*   1MB */
    MCU1_1_EXT_DATA  (RWIX)	: origin=MCU1_1_EXT_DATA_BASE     length=0x00100000	/*   1MB */
    MCU1_1_R5F_MEM_TEXT (RWIX)	: origin=MCU1_1_R5F_MEM_TEXT_BASE length=0x00100000	/*   1MB */
    MCU1_1_R5F_MEM_DATA (RWIX)	: origin=MCU1_1_R5F_MEM_DATA_BASE length=0x00100000	/*   1MB */
    MCU1_1_DDR_SPACE (RWIX)	: origin=MCU1_1_DDR_SPACE_BASE    length=0x00C00000	/*  12MB */
    MCU2_0_IPC_DATA (RWIX)	: origin=MCU2_0_ALLOCATED_START   length=0x00100000	/*   1MB */
    MCU2_0_EXT_DATA  (RWIX)	: origin=MCU2_0_EXT_DATA_BASE     length=0x00100000	/*   1MB */
    MCU2_0_R5F_MEM_TEXT (RWIX)	: origin=MCU2_0_R5F_MEM_TEXT_BASE length=0x00100000	/*   1MB */
    MCU2_0_R5F_MEM_DATA (RWIX)	: origin=MCU2_0_R5F_MEM_DATA_BASE length=0x00100000	/*   1MB */
    MCU2_0_DDR_SPACE (RWIX)	: origin=MCU2_0_DDR_SPACE_BASE    length=0x00C00000	/*  12MB */
    MCU2_1_IPC_DATA (RWIX)	: origin=MCU2_1_ALLOCATED_START   length=0x00100000	/*   1MB */
    MCU2_1_EXT_DATA  (RWIX)	: origin=MCU2_1_EXT_DATA_BASE     length=0x00100000	/*   1MB */
    MCU2_1_R5F_MEM_TEXT (RWIX)	: origin=MCU2_1_R5F_MEM_TEXT_BASE length=0x00100000	/*   1MB */
    MCU2_1_R5F_MEM_DATA (RWIX)	: origin=MCU2_1_R5F_MEM_DATA_BASE length=0x00100000	/*   1MB */
    MCU2_1_DDR_SPACE (RWIX)	: origin=MCU2_1_DDR_SPACE_BASE    length=0x00C00000	/*  12MB */

    SHARED_DDR_SPACE (RWIX)	: origin=0xAA000000 length=0x01C00000           /*  28MB */

}  /* end of MEMORY */

/*----------------------------------------------------------------------------*/
/* Section Configuration                                                      */

SECTIONS
{
    .freertosrstvectors      : {} palign(8)      > VECTORS
    .bootCode    	: {} palign(8) 		> MCU_ATCM
    .startupCode 	: {} palign(8) 		> MCU_ATCM
    .startupData 	: {} palign(8) 		> MCU_ATCM, type = NOINIT
    .text	: {} palign(8)		> MCU1_1_DDR_SPACE
     GROUP {
        .text.hwi    : palign(8)
        .text.cache  : palign(8)
        .text.mpu    : palign(8)
        .text.boot   : palign(8)
    }                               > MCU1_1_DDR_SPACE
    .const   	: {} palign(8) 		> MCU1_1_DDR_SPACE
    .rodata   : {} palign(8) 		> MCU1_1_DDR_SPACE
    .cinit   	: {} palign(8) 		> MCU1_1_DDR_SPACE
    .pinit   	: {} palign(8) 		> MCU1_1_DDR_SPACE
    .bss     	: {} align(4)  		> MCU1_1_DDR_SPACE
    .data    	: {} palign(128) 	> MCU1_1_DDR_SPACE
    .data_buffer: {} palign(128) 	> MCU1_1_DDR_SPACE
    .sysmem  	: {} 			> MCU1_1_DDR_SPACE
    .boardcfg_data   : {} align(4)       > MCU1_1_DDR_SPACE
    .stack	: {} align(4)		> MCU1_1_DDR_SPACE (HIGH)
    ipc_data_buffer (NOINIT) : {} palign(128)	> MCU1_1_DDR_SPACE
    .resource_table : {
        __RESOURCE_TABLE = .;
    } > MCU1_1_EXT_DATA_BASE

    .tracebuf   : {}			> MCU1_1_EXT_DATA


/*----------------------------------------------------------------------------*/
/* Misc linker settings                                                       */

    .irqStack   : {. = . + __IRQ_STACK_SIZE;} align(4)      > MCU1_1_DDR_SPACE (HIGH)
    RUN_START(__IRQ_STACK_START)
    RUN_END(__IRQ_STACK_END)

    .fiqStack   : {. = . + __FIQ_STACK_SIZE;} align(4)      > MCU1_1_DDR_SPACE (HIGH)
    RUN_START(__FIQ_STACK_START)
    RUN_END(__FIQ_STACK_END)

    .abortStack : {. = . + __ABORT_STACK_SIZE;} align(4)    > MCU1_1_DDR_SPACE (HIGH)
    RUN_START(__ABORT_STACK_START)
    RUN_END(__ABORT_STACK_END)

    .undStack   : {. = . + __UND_STACK_SIZE;} align(4)      > MCU1_1_DDR_SPACE (HIGH)
    RUN_START(__UND_STACK_START)
    RUN_END(__UND_STACK_END)

    .svcStack   : {. = . + __SVC_STACK_SIZE;} align(4)      > MCU1_1_DDR_SPACE (HIGH)
    RUN_START(__SVC_STACK_START)
    RUN_END(__SVC_STACK_END)

}  /* end of SECTIONS */

/*-------------------------------- END ---------------------------------------*/
