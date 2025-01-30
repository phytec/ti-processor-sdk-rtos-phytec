/*----------------------------------------------------------------------------*/
/* File: linker_r5f_mcu1_1_sysbios.lds                                        */
/* Description:			  					      */
/*    Link command file for J7ES MCU1_1 view				      */
/*	  TI ARM Compiler version 15.12.3 LTS or later			      */
/*                                                                            */
/* (c) Texas Instruments 2018, All rights reserved.                           */
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
/* Linker Settings                                                            */
/* Standard linker options						      */
--retain="*(.bootCode)"
--retain="*(.startupCode)"
--retain="*(.startupData)"
--fill_value=0
--stack_size=0x2000
--heap_size=0x1000
-e __VECS_ENTRY_POINT

-stack  0x2000                              /* SOFTWARE STACK SIZE           */
-heap   0x2000                              /* HEAP AREA SIZE                */

#define DDR0_ALLOCATED_START        (0x80000000)

/* Reserved for other cores */
#define DDR0_RESERVED_LENGTH        (0x17000000) /* 368 MB */
#define DDR0_RESERVED               (DDR0_ALLOCATED_START + DDR0_RESERVED_LENGTH)

#define MCU1_0_ALLOCATED_START      (DDR0_RESERVED)
#define MCU1_0_ALLOCATED_LENGTH     (0x01000000) /* 16 MB */
#define MCU1_0_EXT_DATA_BASE        (MCU1_0_ALLOCATED_START     + 0x00000000)
#define MCU1_0_R5F_MEM_TEXT_BASE    (MCU1_0_EXT_DATA_BASE       + 0x00400000)
#define MCU1_0_R5F_MEM_DATA_BASE    (MCU1_0_R5F_MEM_TEXT_BASE   + 0x00400000)
#define MCU1_0_DDR_SPACE_BASE       (MCU1_0_R5F_MEM_DATA_BASE   + 0x00400000)

#define MCU1_1_ALLOCATED_START      (MCU1_0_ALLOCATED_START + MCU1_0_ALLOCATED_LENGTH)
#define MCU1_1_ALLOCATED_LENGTH     (0x01000000) /* 16 MB */
#define MCU1_1_EXT_DATA_BASE        (MCU1_1_ALLOCATED_START     + 0x00000000)
#define MCU1_1_R5F_MEM_TEXT_BASE    (MCU1_1_EXT_DATA_BASE       + 0x00400000)
#define MCU1_1_R5F_MEM_DATA_BASE    (MCU1_1_R5F_MEM_TEXT_BASE   + 0x00400000)
#define MCU1_1_DDR_SPACE_BASE       (MCU1_1_R5F_MEM_DATA_BASE   + 0x00400000)

#define MCU2_0_ALLOCATED_START      (MCU1_1_ALLOCATED_START + MCU1_1_ALLOCATED_LENGTH)
#define MCU2_0_ALLOCATED_LENGTH     (0x02000000) /* 32 MB */
#define MCU2_0_EXT_DATA_BASE        (MCU2_0_ALLOCATED_START     + 0x00000000)
#define MCU2_0_R5F_MEM_TEXT_BASE    (MCU2_0_EXT_DATA_BASE       + 0x00800000)
#define MCU2_0_R5F_MEM_DATA_BASE    (MCU2_0_R5F_MEM_TEXT_BASE   + 0x00800000)
#define MCU2_0_DDR_SPACE_BASE       (MCU2_0_R5F_MEM_DATA_BASE   + 0x00800000)

#define MCU2_1_ALLOCATED_START      (MCU2_0_ALLOCATED_START + MCU2_0_ALLOCATED_LENGTH)
#define MCU2_1_ALLOCATED_LENGTH     (0x04000000) /* 64 MB */
#define MCU2_1_EXT_DATA_BASE        (MCU2_1_ALLOCATED_START     + 0x00000000)
#define MCU2_1_R5F_MEM_TEXT_BASE    (MCU2_1_EXT_DATA_BASE       + 0x01000000)
#define MCU2_1_R5F_MEM_DATA_BASE    (MCU2_1_R5F_MEM_TEXT_BASE   + 0x01000000)
#define MCU2_1_DDR_SPACE_BASE       (MCU2_1_R5F_MEM_DATA_BASE   + 0x01000000)

#define ATCM_START 0x00000000
#define BTCM_START 0x41010000
#define OCMRAM_MCU1_0_START 0x41cfe000
#define OCMRAM_MCU1_1_START 0x41cff000

/*----------------------------------------------------------------------------*/
/* Memory Map                                                                 */
MEMORY
{
    /* MCU1_R5F_1 local view  */
    MCU_ATCM (RWX)          : origin=ATCM_START length=0x8000
    MCU_BTCM (RWX)          : origin=BTCM_START length=0x8000

    /* MCU1_R5F_1 SoC view  */
    MCU1_R5F1_ATCM (RWIX)   : origin=0x41400000 length=0x8000
    MCU1_R5F1_BTCM (RWIX)   : origin=0x41410000 length=0x8000

    /* MCUSS RAM - Start towards the end to avoid Bootloader usage of the SRAM */
    OCMRAM_MCU1_0 (RWIX)        : origin=OCMRAM_MCU1_0_START length=0x1000
    OCMRAM_MCU1_1 (RWIX)        : origin=OCMRAM_MCU1_1_START length=0x1000

    DDR0_RESERVED_NOT_USED (RWIX)   : origin=DDR0_ALLOCATED_START length=DDR0_RESERVED_LENGTH

    MCU2_0_EXT_DATA     (RWIX)  : origin=MCU2_0_EXT_DATA_BASE       length=0x00800000
    MCU2_0_R5F_MEM_TEXT (RWIX)  : origin=MCU2_0_R5F_MEM_TEXT_BASE   length=0x00800000
    MCU2_0_R5F_MEM_DATA (RWIX)  : origin=MCU2_0_R5F_MEM_DATA_BASE   length=0x00800000
    MCU2_0_DDR_SPACE    (RWIX)  : origin=MCU2_0_DDR_SPACE_BASE      length=0x00800000



}  /* end of MEMORY */

/*----------------------------------------------------------------------------*/
/* Section Configuration                                                      */

SECTIONS
{
    .vecs       : {
        __VECS_ENTRY_POINT = .;
    } palign(8) > MCU_BTCM

    .init_text  : {
                     boot.*(.text)
                     *(.text:ti_sysbios_family_arm_MPU_*)
                     *(.text:ti_sysbios_family_arm_v7r_Cache_*)
                  }  > MCU_BTCM
    .text:xdc_runtime_Startup_reset__I : {} palign(8) > MCU_BTCM
    .bootCode    	: {} palign(8) 		> MCU_BTCM
    .startupCode 	: {} palign(8) 		> MCU_BTCM
    .startupData 	: {} palign(8) 		> MCU_BTCM, type = NOINIT
    .text	: {} palign(8)		> MCU2_0_DDR_SPACE
    .const   	: {} palign(8) 		> MCU2_0_DDR_SPACE
    .rodata   : {} palign(8) 		> MCU2_0_DDR_SPACE
    .cinit   	: {} palign(8) 		> MCU2_0_DDR_SPACE
    .pinit   	: {} palign(8) 		> MCU2_0_DDR_SPACE
    .bss     	: {} align(8)  		> MCU2_0_DDR_SPACE
    .data    	: {} palign(128) 	> MCU2_0_DDR_SPACE
    .data_buffer: {} palign(128) 	> MCU2_0_DDR_SPACE
    .sysmem  	: {} 			> MCU2_0_DDR_SPACE
    .stack	: {} align(8)		> MCU2_0_DDR_SPACE
    ipc_data_buffer (NOINIT) : {} palign(128)	> MCU2_0_DDR_SPACE
    .resource_table : {
        __RESOURCE_TABLE = .;
    } > MCU2_0_EXT_DATA

    .tracebuf   : {}			> MCU2_0_EXT_DATA

}  /* end of SECTIONS */

/*----------------------------------------------------------------------------*/
/* Misc linker settings                                                       */


/*-------------------------------- END ---------------------------------------*/
