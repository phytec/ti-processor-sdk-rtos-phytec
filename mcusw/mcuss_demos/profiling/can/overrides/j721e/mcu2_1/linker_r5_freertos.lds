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
    VECTORS (X)                 : ORIGIN = 0x00000000 LENGTH = 0x00000040

    /*=================== MCU0_R5F_0 Local View ========================*/
    MCU0_R5F_TCMA  (X)          : ORIGIN = 0x00000040 LENGTH = 0x00007FC0
    MCU0_R5F_TCMB0 (RWIX)       : ORIGIN = 0x41010000 LENGTH = 0x00008000

    /*==================== MCU0_R5F_1 SoC View =========================*/
    MCU0_R5F1_ATCM (RWIX)       : ORIGIN = 0x41400000 LENGTH = 0x00008000
    MCU0_R5F1_BTCM (RWIX)       : ORIGIN = 0x41410000 LENGTH = 0x00008000

    /* MCU0 share locations													  */
    OCMC_RAM             (RWIX) : ORIGIN = 0x41CE3100 LENGTH = 0x0001CA00


    /* Used in this file */
    DDR0_MCU_2_1 (RWIX)     :  ORIGIN = 0x80000000 LENGTH = 0x80000000   /* 2GB */

    MAIN_MSRAM_0            : origin=0x70000000 length=0x100000

/* Additional memory settings	*/

}  /* end of MEMORY */

/*----------------------------------------------------------------------------*/
/* Section Configuration                                                      */

SECTIONS
{
    .freertosrstvectors      : {} palign(8)      > VECTORS
    .bootCode        : {} palign(8)      > OCMC_RAM
    .startupCode     : {} palign(8)      > OCMC_RAM
    .startupData     : {} palign(8)      > OCMC_RAM, type = NOINIT
    .text    	: {} palign(8) 		> MAIN_MSRAM_0
    GROUP {
        .text.hwi    : palign(8)
        .text.cache  : palign(8)
        .text.mpu    : palign(8)
        .text.boot   : palign(8)
    }                           > MAIN_MSRAM_0
    .const   	: {} palign(8) 		> MAIN_MSRAM_0
    .rodata   : {} palign(8)    > MAIN_MSRAM_0
    .cinit   	: {} palign(8) 		> MAIN_MSRAM_0
    .pinit   	: {} palign(8) 		> MAIN_MSRAM_0

    /* For NDK packet memory, we need to map this sections before .bss*/
    .bss:NDK_MMBUFFER  (NOLOAD) {} ALIGN (128) > DDR0_MCU_2_1
    .bss:NDK_PACKETMEM (NOLOAD) {} ALIGN (128) > DDR0_MCU_2_1

    .bss     	: {} align(4)  		> MAIN_MSRAM_0
    .far     	: {} align(4)  		> MAIN_MSRAM_0
    .data    	: {} palign(128) 	> MAIN_MSRAM_0
    .data_buffer: {} palign(128) 	> MAIN_MSRAM_0
	.sysmem  	: {} 				> MAIN_MSRAM_0
	.stack  	: {} align(4)		> MAIN_MSRAM_0  (HIGH)  fill=FILL_PATTERN

    /* USB or any other LLD buffer for benchmarking */
    .benchmark_buffer (NOLOAD) {} ALIGN (8) > MAIN_MSRAM_0

    .irqStack   : {. = . + __IRQ_STACK_SIZE;} align(4)      > MAIN_MSRAM_0(HIGH)
   RUN_START(__IRQ_STACK_START)
   RUN_END(__IRQ_STACK_END)

   .fiqStack   : {. = . + __FIQ_STACK_SIZE;} align(4)      > MAIN_MSRAM_0(HIGH)
   RUN_START(__FIQ_STACK_START)
   RUN_END(__FIQ_STACK_END)

   .abortStack : {. = . + __ABORT_STACK_SIZE;} align(4)    > MAIN_MSRAM_0(HIGH)
   RUN_START(__ABORT_STACK_START)
   RUN_END(__ABORT_STACK_END)

   .undStack   : {. = . + __UND_STACK_SIZE;} align(4)      > MAIN_MSRAM_0(HIGH)
   RUN_START(__UND_STACK_START)
   RUN_END(__UND_STACK_END)

   .svcStack   : {. = . + __SVC_STACK_SIZE;} align(4)      > MAIN_MSRAM_0(HIGH)
   RUN_START(__SVC_STACK_START)
   RUN_END(__SVC_STACK_END)

    /* Additional sections settings     */
    McalTextSection : fill=FILL_PATTERN, align=4, load > MAIN_MSRAM_0
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

    }
    McalConstSection : fill=FILL_PATTERN, align=4, load > MAIN_MSRAM_0
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
    }

    McalInitSection : fill=FILL_PATTERN, align=4, load > MAIN_MSRAM_0
    {
        .=align(4);
        __linker_spi_init_start = .;
        . += FILL_LENGTH;
        *(SPI_DATA_INIT_UNSPECIFIED_SECTION)
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
    }
    McalNoInitSection : fill=FILL_PATTERN, align=4, load > MAIN_MSRAM_0, type = NOINIT
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
        __linker_eth_no_init_start = .;
        . += FILL_LENGTH;
        *(ETH_DATA_NO_INIT_UNSPECIFIED_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_eth_no_init_end = .;

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

    }
    /* Example Utility specifics */
    UtilityNoInitSection : align=4, load > MAIN_MSRAM_0, type = NOINIT
    {
        .=align(4);
        __linker_utility_no_init_start = .;
        . += FILL_LENGTH;
        *(EG_TEST_RESULT_32_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_utility_no_init_end = .;
    }
    SciClientBoardCfgSection : align=128, load > MAIN_MSRAM_0, type = NOINIT
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
     * it in OCMRAM or MAIN_MSRAM_0 */
    McalUdmaSection : fill=FILL_PATTERN, align=128, load > MAIN_MSRAM_0
    {
        .=align(128);
        __linker_eth_udma_start = .;
        . += FILL_LENGTH;
        *(ETH_UDMA_SECTION)
        .=align(128);
        . += FILL_LENGTH;
        __linker_eth_udma_end = .;
    }
    McalTxDataSection : fill=FILL_PATTERN, align=128, load > MAIN_MSRAM_0, type = NOINIT
    {
        .=align(128);
        __linker_eth_tx_data_start = .;
        . += FILL_LENGTH;
        *(ETH_TX_DATA_SECTION)
        .=align(128);
        . += FILL_LENGTH;
        __linker_eth_tx_data_end = .;
    }
    McalRxDataSection : fill=FILL_PATTERN, align=128, load > MAIN_MSRAM_0, type = NOINIT
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
