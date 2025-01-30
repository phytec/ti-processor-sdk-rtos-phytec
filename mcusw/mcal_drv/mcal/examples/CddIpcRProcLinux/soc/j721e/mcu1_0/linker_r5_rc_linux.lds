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

--define FILL_PATTERN=0xFEAA55EF
--define FILL_LENGTH=0x100

/*--------------------------------------------------------------------------*/
/*                               Memory Map                                 */
/*--------------------------------------------------------------------------*/
MEMORY
{
    MCU1_R5F0_TCMB_VECS       (X) : ORIGIN = 0x41010000 LENGTH = 0x00000040
    MCU1_R5F0_TCMB            (X) : ORIGIN = 0x41010040 LENGTH = 0x00007FC0

    MCU1_0_DDR_SPACE          (RWIX) : ORIGIN = 0xA0400000 LENGTH = 0x00C00000 
    DDR0_MCU_1_0_RSTB         (RWIX) : ORIGIN = 0xA0100000 LENGTH = 0x00001000
    DDR0_MCU_1_0_TRCBF        (RWIX) : ORIGIN = 0xA0101000 LENGTH = 0x00080000
}

/*--------------------------------------------------------------*/
/*                     Section Configuration                    */
/*--------------------------------------------------------------*/
SECTIONS
{
    .freertosrstvectors : {} palign(8)      > MCU1_R5F0_TCMB_VECS
    
    .bootCode           : {} palign(8)      > MCU1_R5F0_TCMB
    .startupCode        : {} palign(8)      > MCU1_R5F0_TCMB
    .startupData        : {} palign(8)      > MCU1_R5F0_TCMB, type = NOINIT
    GROUP 
    {
        .text.hwi       : palign(8)
        .text.cache     : palign(8)
        .text.mpu       : palign(8)
        .text.boot      : palign(8)
    }                                       > MCU1_R5F0_TCMB

    .text               : {} palign(8)      > MCU1_0_DDR_SPACE
    .const              : {} palign(8)      > MCU1_0_DDR_SPACE
    .rodata             : {} palign(8)      > MCU1_0_DDR_SPACE
    .cinit              : {} palign(8)      > MCU1_0_DDR_SPACE
    .bss                : {} align(4)       > MCU1_0_DDR_SPACE
    .far                : {} align(4)       > MCU1_0_DDR_SPACE
    .data               : {} palign(128)    > MCU1_0_DDR_SPACE
    .sysmem             : {}                > MCU1_0_DDR_SPACE
    .data_buffer        : {} palign(128)    > MCU1_0_DDR_SPACE
    .bss.devgroup       : {*(.bss.devgroup*)} align(4)         > MCU1_0_DDR_SPACE
    .const.devgroup     : {*(.const.devgroup*)} align(4)       > MCU1_0_DDR_SPACE
    .boardcfg_data      : {} align(4)       > MCU1_0_DDR_SPACE

    /* USB or any other LLD buffer for benchmarking */
    .benchmark_buffer (NOLOAD) {} align (8)     > MCU1_0_DDR_SPACE
    ipc_data_buffer (NOINIT) : {} palign(128)	> MCU1_0_DDR_SPACE
    .resource_table          : 
    {
        __RESOURCE_TABLE = .;
    }                                           > DDR0_MCU_1_0_RSTB

    .tracebuf                : {} align(1024)   > DDR0_MCU_1_0_TRCBF

    .stack                   : {} align(4)      > MCU1_0_DDR_SPACE  (HIGH)

    .irqStack   : {. = . + __IRQ_STACK_SIZE;} align(4)      > MCU1_0_DDR_SPACE  (HIGH)
    RUN_START(__IRQ_STACK_START)
    RUN_END(__IRQ_STACK_END)

    .fiqStack   : {. = . + __FIQ_STACK_SIZE;} align(4)      > MCU1_0_DDR_SPACE  (HIGH)
    RUN_START(__FIQ_STACK_START)
    RUN_END(__FIQ_STACK_END)

    .abortStack : {. = . + __ABORT_STACK_SIZE;} align(4)    > MCU1_0_DDR_SPACE  (HIGH)
    RUN_START(__ABORT_STACK_START)
    RUN_END(__ABORT_STACK_END)

    .undStack   : {. = . + __UND_STACK_SIZE;} align(4)      > MCU1_0_DDR_SPACE  (HIGH)
    RUN_START(__UND_STACK_START)
    RUN_END(__UND_STACK_END)

    .svcStack   : {. = . + __SVC_STACK_SIZE;} align(4)      > MCU1_0_DDR_SPACE  (HIGH)
    RUN_START(__SVC_STACK_START)
    RUN_END(__SVC_STACK_END)

    /* Additional sections settings     */
    McalTextSection : fill=FILL_PATTERN, align=4, load > MCU1_0_DDR_SPACE
    {
        .=align(4);
        __linker_cdd_ipc_text_start = .;
        . += FILL_LENGTH;
        *(CDD_IPC_TEXT_SECTION)
        *(CDD_IPC_ISR_TEXT_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_cdd_ipc_text_end = .;

    }
    McalConstSection : fill=FILL_PATTERN, align=4, load > MCU1_0_DDR_SPACE
    {
        .=align(4);
        __linker_cdd_ipc_const_start = .;
        . += FILL_LENGTH;
        *(CDD_IPC_CONST_32_SECTION)
        *(CDD_IPC_CONFIG_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_cdd_ipc_const_end = .;
    }

    McalInitSection : fill=FILL_PATTERN, align=4, load > MCU1_0_DDR_SPACE
    {
        .=align(4);
        __linker_cdd_ipc_init_start = .;
        . += FILL_LENGTH;
        *(CDD_IPC_DATA_INIT_UNSPECIFIED_SECTION)
        *(CDD_IPC_DATA_INIT_32_SECTION)
        *(CDD_IPC_DATA_INIT_8_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_cdd_ipc_init_end = .;
    }
    McalNoInitSection : fill=FILL_PATTERN, align=4, load > MCU1_0_DDR_SPACE, type = NOINIT
    {
        __linker_cdd_ipc_no_init_start = .;
        . += FILL_LENGTH;
        *(CDD_IPC_DATA_NO_INIT_UNSPECIFIED_SECTION)
        *(CDD_IPC_DATA_NO_INIT_8_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_cdd_ipc_no_init_end = .;

    }
    /* Example Utility specifics */
    VariablesAlignedNoInitSection : align=8, load > MCU1_0_DDR_SPACE, type = NOINIT
    {
        .=align(8);
        __linker_cdd_ipc_no_init_align_8b_start = .;
        . += FILL_LENGTH;
        *(CDD_IPC_DATA_NO_INIT_8_ALIGN_8B_SECTION)
        .=align(8);
        . += FILL_LENGTH;
        __linker_cdd_ipc_no_init_align_8b_end = .;
    }
    /* Example Utility specifics */
    UtilityNoInitSection : align=4, load > MCU1_0_DDR_SPACE, type = NOINIT
    {
        .=align(4);
        __linker_utility_no_init_start = .;
        . += FILL_LENGTH;
        *(EG_TEST_RESULT_32_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_utility_no_init_end = .;
    }
    SciClientBoardCfgSection : align=128, load > MCU1_0_DDR_SPACE, type = NOINIT
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
