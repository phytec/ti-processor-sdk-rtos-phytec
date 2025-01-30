/* Linker Settings */
--retain="*(.bootCode)"
--retain="*(.startupCode)"
--retain="*(.startupData)"
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

--define FILL_PATTERN=0xFEAA55EF
--define FILL_LENGTH=0x100

/*          8 MB Max                                        */
/*          600000  6 Slices                                */
/*                                      Rounding Offset     */
/*  SBL?    Start   70000000    1048576 0                   */
/*          End     70100000                                */
/*  MCU 20  Start   70100100    1048576 100                 */
/*          End     70200100                                */
/*  MCU 21  Start   70200200    1048576 100                 */
/*          End     70300200                                */
/*  MCU 30  Start   70300300    1048576 100                 */
/*          End     70400300                                */
/*  MCU 31  Start   70400400    1048576 100                 */
/*          End     70500400                                */
/*  MPU     Start   70500500    1048576 100                 */
/*          End     70600500                                */

/* Memory Map */
MEMORY
{

   RESET_VECTORS (X)       : origin=0x0 length=0x100
    /* MCU0_R5F_0 local view */
    MCU0_R5F_TCMA (X)       : origin=0x100      length=0x8000 - 0x100

    /* MCU0_R5F_0 SoC view */
    MCU0_R5F0_ATCM (RWIX)   : origin=0x41000000 length=0x8000
    MCU0_R5F0_BTCM (RWIX)   : origin=0x41010000 length=0x8000
    /* MCU0_R5F_1 SoC view */
    MCU0_R5F1_ATCM (RWIX)   : origin=0x41400000 length=0x8000
    MCU0_R5F1_BTCM (RWIX)   : origin=0x41410000 length=0x8000

    MSMC3_DMSC_FW  (RWIX)   : origin=0x700F0000 length=0x10000         /* 64KB */

    /* Used in this file */
    DDR0_MCU_2_1 (RWIX)     : origin=0xA3200000 length=0xE00000       /* 16MB */


    DDR0_MCU_2_1_RSTB (RWIX)     : origin=0xA3100000 length=0x80000       /* 1MB */

    DDR0_MCU_2_1_TRCB (RWIX)     : origin=0xA3180000 length=0x80000       /* 1MB */

 MAIN_MSRAM_0            : origin=0x70040000 length=0xB0000 /* 1MB - 320KB */
}

/* Section Configuration */
SECTIONS
{
    .rstvectors    : {} palign(8)      > RESET_VECTORS
    .bootCode      : {} palign(8)      > MCU0_R5F_TCMA
    .startupCode   : {} palign(8)      > MCU0_R5F_TCMA
    .startupData   : {} palign(8)      > MCU0_R5F_TCMA, type = NOINIT
    .text          : {} palign(8)      > DDR0_MCU_2_1
    .const         : {} palign(8)      > DDR0_MCU_2_1
    .cinit         : {} palign(8)      > DDR0_MCU_2_1
    .pinit         : {} palign(8)      > DDR0_MCU_2_1
    .bss           : {} align(4)       > DDR0_MCU_2_1
    .far           : {} align(4)       > DDR0_MCU_2_1
    .data          : {} palign(128)    > DDR0_MCU_2_1
    .boardcfg_data : {} palign(128)    > DDR0_MCU_2_1
    .sysmem        : {}                > DDR0_MCU_2_1
    .data_buffer   : {} palign(128)    > DDR0_MCU_2_1
    /* Require to host MPU configuration code in MAIN_MSRAM_0 */
    .CDD_IPC_MPU_CFG_OCMRAM : {} palign(8) > DDR0_MCU_2_1

    /* USB or any other LLD buffer for benchmarking */
    .benchmark_buffer (NOLOAD) {} ALIGN (8) > DDR0_MCU_2_1

    .resource_table : {
        __RESOURCE_TABLE = .;
    } > DDR0_MCU_2_1_RSTB

    .tracebuf   : {}	align(1024) > DDR0_MCU_2_1_TRCB

    .stack      : {} align(4)       > DDR0_MCU_2_1  (HIGH)  fill=FILL_PATTERN
    .irqStack   : {. = . + __IRQ_STACK_SIZE;} align(4)      > DDR0_MCU_2_1  (HIGH)
    RUN_START(__IRQ_STACK_START)
    RUN_END(__IRQ_STACK_END)
    .fiqStack   : {. = . + __FIQ_STACK_SIZE;} align(4)      > DDR0_MCU_2_1  (HIGH)
    RUN_START(__FIQ_STACK_START)
    RUN_END(__FIQ_STACK_END)
    .abortStack : {. = . + __ABORT_STACK_SIZE;} align(4)    > DDR0_MCU_2_1  (HIGH)
    RUN_START(__ABORT_STACK_START)
    RUN_END(__ABORT_STACK_END)
    .undStack   : {. = . + __UND_STACK_SIZE;} align(4)      > DDR0_MCU_2_1  (HIGH)
    RUN_START(__UND_STACK_START)
    RUN_END(__UND_STACK_END)
    .svcStack   : {. = . + __SVC_STACK_SIZE;} align(4)      > DDR0_MCU_2_1  (HIGH)
    RUN_START(__SVC_STACK_START)
    RUN_END(__SVC_STACK_END)

    /* Additional sections settings     */
    McalTextSection : fill=FILL_PATTERN, align=4, load > DDR0_MCU_2_1
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
    McalConstSection : fill=FILL_PATTERN, align=4, load > DDR0_MCU_2_1
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

    McalInitSection : fill=FILL_PATTERN, align=4, load > DDR0_MCU_2_1
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
    McalNoInitSection : fill=FILL_PATTERN, align=4, load > DDR0_MCU_2_1, type = NOINIT
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
    VariablesAlignedNoInitSection : align=8, load > DDR0_MCU_2_1, type = NOINIT
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
    UtilityNoInitSection : align=4, load > DDR0_MCU_2_1, type = NOINIT
    {
        .=align(4);
        __linker_utility_no_init_start = .;
        . += FILL_LENGTH;
        *(EG_TEST_RESULT_32_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_utility_no_init_end = .;
    }
    SciClientBoardCfgSection : align=128, load > DDR0_MCU_2_1, type = NOINIT
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
