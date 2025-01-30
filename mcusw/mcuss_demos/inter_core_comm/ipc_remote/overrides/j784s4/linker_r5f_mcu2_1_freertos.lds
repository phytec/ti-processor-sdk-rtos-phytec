/*==========================*/
/*     Linker Settings      */
/*==========================*/

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

--stack_size=0x2000
--heap_size=0x1000

__IRQ_STACK_SIZE   = 0x1000;
__FIQ_STACK_SIZE   = 0x0100;
__ABORT_STACK_SIZE = 0x0100;
__UND_STACK_SIZE   = 0x0100;
__SVC_STACK_SIZE   = 0x0100;

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

#define ATCM_START 0x00000040
#define BTCM_START 0x41010000
#define OCMRAM_MCU1_0_START 0x41cfe000
#define OCMRAM_MCU1_1_START 0x41cff000


/*----------------------------------------------------------------------------*/
/* Memory Map                                                                 */
MEMORY
{
    VECTORS (X)                 : ORIGIN = 0x00000000 LENGTH = 0x00000040

    /* MCU1_R5F_1 local view  */
    MCU_ATCM (RWX)          : origin=ATCM_START	length=0x8000
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

    MCU2_1_EXT_DATA     (RWIX)  : origin=MCU2_1_EXT_DATA_BASE       length=0x01000000
    MCU2_1_R5F_MEM_TEXT (RWIX)  : origin=MCU2_1_R5F_MEM_TEXT_BASE   length=0x01000000
    MCU2_1_R5F_MEM_DATA (RWIX)  : origin=MCU2_1_R5F_MEM_DATA_BASE   length=0x01000000
    MCU2_1_DDR_SPACE    (RWIX)  : origin=MCU2_1_DDR_SPACE_BASE      length=0x01000000

}  /* end of MEMORY */

/*----------------------------------------------------------------------------*/
/* Section Configuration                                                      */

SECTIONS
{
    .freertosrstvectors      : {} palign(8)      > VECTORS
    .bootCode        : {} palign(8)      > MCU_BTCM
    .startupCode     : {} palign(8)      > MCU_BTCM
    .startupData     : {} palign(8)      > MCU_BTCM, type = NOINIT
    .init_text  : {}  > MCU_BTCM
    .bootCode    	: {} palign(8) 		> MCU_BTCM
    .startupCode 	: {} palign(8) 		> MCU_BTCM
    .startupData 	: {} palign(8) 		> MCU_BTCM, type = NOINIT
    .text	: {} palign(8)		> MCU2_1_DDR_SPACE
    GROUP {
        .text.hwi    : palign(8)
        .text.cache  : palign(8)
        .text.mpu    : palign(8)
        .text.boot   : palign(8)
    }                           > MCU2_1_DDR_SPACE
    .const   	: {} palign(8) 		> MCU2_1_DDR_SPACE
    .cinit   	: {} palign(8) 		> MCU2_1_DDR_SPACE
    .pinit   	: {} palign(8) 		> MCU2_1_DDR_SPACE
    .bss     	: {} align(4)  		> MCU2_1_DDR_SPACE
    .data    	: {} palign(128) 	> MCU2_1_DDR_SPACE
    .data_buffer: {} palign(128) 	> MCU2_1_DDR_SPACE
    .sysmem  	: {} 			> MCU2_1_DDR_SPACE
    .stack	: {} align(4)		> MCU2_1_DDR_SPACE
    ipc_data_buffer (NOINIT) : {} palign(128)	> MCU2_1_DDR_SPACE
    .resource_table : {
        __RESOURCE_TABLE = .;
    } > MCU2_0_EXT_DATA

    .irqStack   : {. = . + __IRQ_STACK_SIZE;} align(4)      > MCU2_1_DDR_SPACE(HIGH)
    RUN_START(__IRQ_STACK_START)
    RUN_END(__IRQ_STACK_END)

    .fiqStack   : {. = . + __FIQ_STACK_SIZE;} align(4)      > MCU2_1_DDR_SPACE(HIGH)
    RUN_START(__FIQ_STACK_START)
    RUN_END(__FIQ_STACK_END)

    .abortStack : {. = . + __ABORT_STACK_SIZE;} align(4)    > MCU2_1_DDR_SPACE(HIGH)
    RUN_START(__ABORT_STACK_START)
    RUN_END(__ABORT_STACK_END)

    .undStack   : {. = . + __UND_STACK_SIZE;} align(4)      > MCU2_1_DDR_SPACE(HIGH)
    RUN_START(__UND_STACK_START)
    RUN_END(__UND_STACK_END)

    .svcStack   : {. = . + __SVC_STACK_SIZE;} align(4)      > MCU2_1_DDR_SPACE(HIGH)
    RUN_START(__SVC_STACK_START)
    RUN_END(__SVC_STACK_END)

    .tracebuf   : {}			> MCU2_0_EXT_DATA

}  /* end of SECTIONS */

/*----------------------------------------------------------------------------*/
/* Misc linker settings                                                       */


/*-------------------------------- END ---------------------------------------*/
