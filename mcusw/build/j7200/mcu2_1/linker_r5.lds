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
    /* MCU0_R5F_0 local view */
    MCU0_R5F_TCMA (X)  : origin=0x0        length=0x100

    /* MCU0_R5F_0 SoC view */
    MCU0_R5F0_ATCM (RWIX)   : origin=0x41000000 length=0x8000
    MCU0_R5F0_BTCM (RWIX)   : origin=0x41010000 length=0x8000
    /* MCU0_R5F_1 SoC view */
    MCU0_R5F1_ATCM (RWIX)   : origin=0x41400000 length=0x8000
    MCU0_R5F1_BTCM (RWIX)   : origin=0x41410000 length=0x8000

    MSMC3_DMSC_FW  (RWIX)   : origin=0x700F0000 length=0x10000         /* 64KB */

    /* Used in this file */
    DDR0_MCU_2_1 (RWIX)     : origin=0x9B000000 length=0x4000000         /* 64 MB */
    /* MSMC_SRAM */
    RESET_VECTORS (X)       : origin=0x70000000 length=0x40000    /* 256KB */
    MAIN_MSRAM_0            : origin=0x70040000 length=0xB0000 /* 1MB - 320KB */
}

/* Section Configuration */
SECTIONS
{
    .rstvectors    : {} palign(8)      > MCU0_R5F_TCMA
    .bootCode      : {} palign(8)      > MAIN_MSRAM_0
    .startupCode   : {} palign(8)      > MAIN_MSRAM_0
    .startupData   : {} palign(8)      > MAIN_MSRAM_0, type = NOINIT
    .text          : {} palign(8)      > DDR0_MCU_2_1
    .const         : {} palign(8)      > DDR0_MCU_2_1
    .rodata        : {} palign(8)      > DDR0_MCU_2_1
    .cinit         : {} palign(8)      > DDR0_MCU_2_1
    .pinit         : {} palign(8)      > DDR0_MCU_2_1
    .bss           : {} align(4)       > DDR0_MCU_2_1
    .far           : {} align(4)       > DDR0_MCU_2_1
    .data          : {} palign(128)    > DDR0_MCU_2_1
    .boardcfg_data : {} palign(128)    > MAIN_MSRAM_0
    .sysmem        : {}                > DDR0_MCU_2_1
    .data_buffer   : {} palign(128)    > DDR0_MCU_2_1
    /* Require to host MPU configuration code in MAIN_MSRAM_0 */
    .CDD_IPC_MPU_CFG_OCMRAM : {} palign(8) > MAIN_MSRAM_0

    /* USB or any other LLD buffer for benchmarking */
    .benchmark_buffer (NOLOAD) {} ALIGN (8) > DDR0_MCU_2_1

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
        __linker_spi_text_start = .;
        . += FILL_LENGTH;
        *(SPI_TEXT_SECTION)
        *(SPI_ISR_TEXT_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_spi_text_end = .;

        .=align(4);
        __linker_fls_text_start = .;
        . += FILL_LENGTH;
        *(FLS_TEXT_SECTION)
        *(FLS_ISR_TEXT_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_fls_text_end = .;

        .=align(4);
        __linker_gpt_text_start = .;
        . += FILL_LENGTH;
        *(GPT_TEXT_SECTION)
        *(GPT_ISR_TEXT_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_gpt_text_end = .;

        .=align(4);
        __linker_dio_text_start = .;
        . += FILL_LENGTH;
        *(DIO_TEXT_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_dio_text_end = .;

        .=align(4);
        __linker_eth_text_start = .;
        . += FILL_LENGTH;
        *(ETH_TEXT_SECTION)
        *(ETH_ISR_TEXT_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_eth_text_end = .;

        .=align(4);
        __linker_ethtrcv_text_start = .;
        . += FILL_LENGTH;
        *(ETHTRCV_TEXT_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_ethtrcv_text_end = .;

        .=align(4);
        __linker_can_text_start = .;
        . += FILL_LENGTH;
        *(CAN_TEXT_SECTION)
        *(CAN_ISR_TEXT_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_can_text_end = .;

        .=align(4);
        __linker_wdg_text_start = .;
        . += FILL_LENGTH;
        *(WDG_TEXT_SECTION)
        *(WDG_ISR_TEXT_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_wdg_text_end = .;

        .=align(4);
        __linker_pwm_text_start = .;
        . += FILL_LENGTH;
        *(PWM_TEXT_SECTION)
        *(PWM_ISR_TEXT_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_pwm_text_end = .;

        __linker_adc_text_start = .;
        . += FILL_LENGTH;
        *(ADC_TEXT_SECTION)
        *(ADC_ISR_TEXT_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_adc_text_end = .;

        .=align(4);
        __linker_cdd_ipc_text_start = .;
        . += FILL_LENGTH;
        *(CDD_IPC_TEXT_SECTION)
        *(CDD_IPC_ISR_TEXT_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_cdd_ipc_text_end = .;

        .=align(4);
        __linker_icu_text_start = .;
        . += FILL_LENGTH;
        *(ICU_TEXT_SECTION)
        *(ICU_ISR_TEXT_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_icu_text_end = .;

        .=align(4);
        __linker_mcu_text_start = .;
        . += FILL_LENGTH;
        *(MCU_TEXT_SECTION)
        *(MCU_ISR_TEXT_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_mcu_text_end = .;


    }
    McalConstSection : fill=FILL_PATTERN, align=4, load > DDR0_MCU_2_1
    {
        .=align(4);
        __linker_spi_const_start = .;
        . += FILL_LENGTH;
        *(SPI_CONST_32_SECTION)
        *(SPI_CONFIG_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_spi_const_end = .;

        .=align(4);
        __linker_gpt_const_start = .;
        . += FILL_LENGTH;
        *(GPT_CONST_32_SECTION)
        *(GPT_CONFIG_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_gpt_const_end = .;

        .=align(4);
        __linker_dio_const_start = .;
        . += FILL_LENGTH;
        *(DIO_CONST_32_SECTION)
        *(DIO_CONST_UNSPECIFIED_SECTION)
        *(DIO_CONFIG_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_dio_const_end = .;

        .=align(4);
        __linker_fls_const_start = .;
        . += FILL_LENGTH;
        *(FLS_CONST_32_SECTION)
        *(FLS_CONST_UNSPECIFIED_SECTION)
        *(FLS_CONFIG_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_fls_const_end = .;

        .=align(4);
        __linker_can_const_start = .;
        . += FILL_LENGTH;
        *(CAN_CONST_8_SECTION)
        *(CAN_CONST_32_SECTION)
        *(CAN_CONFIG_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_can_const_end = .;

        .=align(4);
        __linker_eth_const_start = .;
        . += FILL_LENGTH;
        *(ETH_CONST_32_SECTION)
        *(ETH_CONFIG_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_eth_const_end = .;

        .=align(4);
        __linker_ethtrcv_const_start = .;
        . += FILL_LENGTH;
        *(ETHTRCV_CONST_32_SECTION)
        *(ETHTRCV_CONFIG_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_ethtrcv_const_end = .;

        .=align(4);
        __linker_wdg_const_start = .;
        . += FILL_LENGTH;
        *(WDG_CONST_32_SECTION)
        *(WDG_CONFIG_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_wdg_const_end = .;

        .=align(4);
        __linker_pwm_const_start = .;
        . += FILL_LENGTH;
        *(PWM_CONST_32_SECTION)
        *(PWM_CONFIG_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_pwm_const_end = .;

        .=align(4);
        __linker_adc_const_start = .;
        . += FILL_LENGTH;
        *(ADC_CONST_32_SECTION)
        *(ADC_CONFIG_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_adc_const_end = .;

        .=align(4);
        __linker_cdd_ipc_const_start = .;
        . += FILL_LENGTH;
        *(CDD_IPC_CONST_32_SECTION)
        *(CDD_IPC_CONFIG_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_cdd_ipc_const_end = .;

        .=align(4);
        __linker_icu_const_start = .;
        . += FILL_LENGTH;
        *(ICU_CONST_32_SECTION)
        *(ICU_CONFIG_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_icu_const_end = .;

        .=align(4);
        __linker_mcu_const_start = .;
        . += FILL_LENGTH;
        *(MCU_CONST_32_SECTION)
        *(MCU_CONFIG_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_mcu_const_end = .;

    }

    McalInitSection : fill=FILL_PATTERN, align=4, load > DDR0_MCU_2_1
    {
        .=align(4);
        __linker_spi_init_start = .;
        . += FILL_LENGTH;
        *(SPI_DATA_INIT_UNSPECIFIED_SECTION)
        *(SPI_CONST_UNSPECIFIED_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_spi_init_end = .;

        .=align(4);
        __linker_gpt_init_start = .;
        . += FILL_LENGTH;
        *(GPT_DATA_INIT_32_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_gpt_init_end = .;

        .=align(4);
        __linker_pwm_init_start = .;
        . += FILL_LENGTH;
        *(PWM_DATA_INIT_32_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_pwm_init_end = .;

        .=align(4);
        __linker_dio_init_start = .;
        . += FILL_LENGTH;
        *(DIO_DATA_INIT_32_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_dio_init_end = .;

        .=align(4);
        __linker_fls_init_start = .;
        . += FILL_LENGTH;
        *(FLS_DATA_INIT_32_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_fls_init_end = .;

        .=align(4);
        __linker_eth_init_start = .;
        . += FILL_LENGTH;
        *(ETH_DATA_INIT_UNSPECIFIED_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_eth_init_end = .;

        .=align(4);
        __linker_ethtrcv_init_start = .;
        . += FILL_LENGTH;
        *(ETHTRCV_DATA_INIT_UNSPECIFIED_SECTION)
        *(ETHTRCV_DATA_INIT_32_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_ethtrcv_init_end = .;

        .=align(4);
        __linker_can_init_start = .;
        . += FILL_LENGTH;
        *(CAN_DATA_INIT_8_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_can_init_end = .;

        .=align(4);
        __linker_wdg_init_start = .;
        . += FILL_LENGTH;
        *(WDG_DATA_INIT_UNSPECIFIED_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_wdg_init_end = .;

        .=align(4);
        __linker_adc_init_start = .;
        . += FILL_LENGTH;
        *(ADC_DATA_INIT_UNSPECIFIED_SECTION)
        *(ADC_DATA_INIT_32_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_adc_init_end = .;

        .=align(4);
        __linker_cdd_ipc_init_start = .;
        . += FILL_LENGTH;
        *(CDD_IPC_DATA_INIT_UNSPECIFIED_SECTION)
        *(CDD_IPC_DATA_INIT_32_SECTION)
        *(CDD_IPC_DATA_INIT_8_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_cdd_ipc_init_end = .;

        .=align(4);
        __linker_icu_init_start = .;
        . += FILL_LENGTH;
        *(ICU_DATA_INIT_32_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_icu_init_end = .;

        .=align(4);
        __linker_mcu_init_start = .;
        . += FILL_LENGTH;
        *(MCU_DATA_INIT_32_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_mcu_init_end = .;

    }
    McalNoInitSection : fill=FILL_PATTERN, align=4, load > DDR0_MCU_2_1, type = NOINIT
    {
        .=align(4);
        __linker_spi_no_init_start = .;
        . += FILL_LENGTH;
        *(SPI_DATA_NO_INIT_UNSPECIFIED_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_spi_no_init_end = .;

        .=align(4);
        __linker_gpt_no_init_start = .;
        . += FILL_LENGTH;
        *(GPT_DATA_NO_INIT_UNSPECIFIED_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_gpt_no_init_end = .;

        .=align(4);
        __linker_dio_no_init_start = .;
        . += FILL_LENGTH;
        *(DIO_DATA_NO_INIT_UNSPECIFIED_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_dio_no_init_end = .;

        .=align(4);
        __linker_fls_no_init_start = .;
        . += FILL_LENGTH;
        *(FLS_DATA_NO_INIT_UNSPECIFIED_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_fls_no_init_end = .;

        .=align(4);
        __linker_eth_no_init_start = .;
        . += FILL_LENGTH;
        *(ETH_DATA_NO_INIT_UNSPECIFIED_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_eth_no_init_end = .;

        .=align(128);
        __linker_eth_no_init_128_start = .;
        . += FILL_LENGTH;
        *(ETH_DATA_NO_INIT_UNSPECIFIED_SECTION_128)
        .=align(128);
        . += FILL_LENGTH;
        __linker_eth_no_init_128_end = .;


        .=align(4);
        __linker_ethtrcv_no_init_start = .;
        . += FILL_LENGTH;
        *(ETHTRCV_DATA_NO_INIT_UNSPECIFIED_SECTION)
        *(ETHTRCV_DATA_NO_INIT_16_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_ethtrcv_no_init_end = .;

        .=align(4);
        __linker_can_no_init_start = .;
        . += FILL_LENGTH;
        *(CAN_DATA_NO_INIT_UNSPECIFIED_SECTION)
        *(CAN_DATA_NO_INIT_32_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_can_no_init_end = .;

        .=align(4);
        __linker_wdg_no_init_start = .;
        . += FILL_LENGTH;
        *(WDG_DATA_NO_INIT_UNSPECIFIED_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_wdg_no_init_end = .;

        .=align(4);
        __linker_pwm_no_init_start = .;
        . += FILL_LENGTH;
        *(PWM_DATA_NO_INIT_UNSPECIFIED_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_pwm_no_init_end = .;

        __linker_adc_no_init_start = .;
        . += FILL_LENGTH;
        *(ADC_DATA_NO_INIT_UNSPECIFIED_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_adc_no_init_end = .;

        __linker_cdd_ipc_no_init_start = .;
        . += FILL_LENGTH;
        *(CDD_IPC_DATA_NO_INIT_UNSPECIFIED_SECTION)
        *(CDD_IPC_DATA_NO_INIT_8_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_cdd_ipc_no_init_end = .;

        __linker_icu_no_init_start = .;
        . += FILL_LENGTH;
        *(ICU_DATA_NO_INIT_UNSPECIFIED_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_icu_no_init_end = .;

        __linker_mcu_no_init_start = .;
        . += FILL_LENGTH;
        *(MCU_DATA_NO_INIT_UNSPECIFIED_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_mcu_no_init_end = .;


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
    /* This section is used for descs and ring mems. It's best to have
     * it in OCMRAM or MSMC3 */
    McalUdmaSection : fill=FILL_PATTERN, align=128, load > MAIN_MSRAM_0
    {
        .=align(128);
        __linker_eth_udma_desc_start = .;
        . += FILL_LENGTH;
        *(ETH_UDMA_DESC_SECTION)
        .=align(128);
        . += FILL_LENGTH;
        __linker_eth_udma_desc_end = .;

        .=align(128);
        __linker_eth_udma_ring_start = .;
        . += FILL_LENGTH;
        *(ETH_UDMA_RING_SECTION)
        .=align(128);
        . += FILL_LENGTH;
        __linker_eth_udma_ring_end = .;

        .=align(128);
        __linker_spi_udma_ring_start = .;
        . += FILL_LENGTH;
        *(SPI_UDMA_RING_SECTION)
        .=align(128);
        . += FILL_LENGTH;
        __linker_spi_udma_ring_end = .;
    }
    McalTxDataSection : fill=FILL_PATTERN, align=128, load > DDR0_MCU_2_1, type = NOINIT
    {
        .=align(128);
        __linker_eth_tx_data_start = .;
        . += FILL_LENGTH;
        *(ETH_TX_DATA_SECTION)
        .=align(128);
        . += FILL_LENGTH;
        __linker_eth_tx_data_end = .;
    }
    McalRxDataSection : fill=FILL_PATTERN, align=128, load > DDR0_MCU_2_1, type = NOINIT
    {
        .=align(128);
        __linker_eth_rx_data_start = .;
        . += FILL_LENGTH;
        *(ETH_RX_DATA_SECTION)
        .=align(128);
        . += FILL_LENGTH;
        __linker_eth_rx_data_end = .;
    }
}  /* end of SECTIONS */

/*----------------------------------------------------------------------------*/
/* Misc linker settings                                                       */


/*-------------------------------- END ---------------------------------------*/
