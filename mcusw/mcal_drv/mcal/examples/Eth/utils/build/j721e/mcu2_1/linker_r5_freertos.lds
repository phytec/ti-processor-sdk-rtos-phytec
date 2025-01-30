/* linker options */

--retain="*(.bootCode)"
--retain="*(.startupCode)"
--retain="*(.startupData)"
--retain="*(.irqStack)"
--retain="*(.fiqStack)"
--retain="*(.abortStack)"
--retain="*(.undStack)"
--retain="*(.svcStack)"

--fill_value=0
--stack_size=0x8000
--heap_size=0x10000
--entry_point=_freertosresetvectors

-stack  0x8000  /* SOFTWARE STACK SIZE */
-heap   0x10000 /* HEAP AREA SIZE      */

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

SECTIONS
{
    .freertosrstvectors : {} palign(8)      > R5F_TCMA
    
    .bootCode           : {} palign(8)      > R5F_TCMB0
    .startupCode        : {} palign(8)      > R5F_TCMB0
    .startupData        : {} palign(8)      > R5F_TCMB0, type = NOINIT
    GROUP 
    {
        .text.hwi       : palign(8)
        .text.cache     : palign(8)
        .text.mpu       : palign(8)
        .text.boot      : palign(8)
    }                                       > R5F_TCMB0

    .text_fast {
/* TODO: Commenting due to linking error reported for missing sections
         need to be removed when the enet-lld + lwip is integrated

        *(.text:EnetDma_retrieveRxPktQ*)
        *(.text:EnetDma_retrieveTxPktQ*)
        *(.text:EnetUdma_ringDequeue*)
        *(.text:EnetUdma_ringEnqueue*)
        *(.text:EnetUdma_submitPkts*)
        *(.text:EnetDma_submitRxPktQ*)
        *(.text:EnetDma_submitTxPktQ*)
        *(.text:EnetQueue_append*)
        *(.text:EnetQueue_copyQ*)
        *(.text:EnetQueue_deq*)
        *(.text:EnetQueue_enq*)
        *(.text:EnetQueue_enqHead*)
        *(.text:EnetQueue_getQCount*)
        *(.text:EnetUtils_physToVirt*)
        *(.text:EnetUtils_virtToPhys*)
        *(.text:Udma_ringQueueRaw*)
        *(.text:Udma_ringDequeueRaw*)
        *(.text:Udma_virtToPhyFxn*)
*/
     }     > DDR_MCU2_1

    .text_rest{
       _text_rest_begin = .;
       *(.text)
       _text_rest_end = .;
    } palign(32)    >  DDR_MCU2_1

    .const              : {} palign(8)      > DDR_MCU2_1
    .rodata             : {} palign(8)      > DDR_MCU2_1
    .cinit              : {} palign(8)      > DDR_MCU2_1
    .pinit              : {} palign(8)      > R5F_TCMB0
    .bss                : {} align(4)       > DDR_MCU2_1
    .far                : {} align(4)       > DDR_MCU2_1
    .data               : {} palign(128)    > DDR_MCU2_1
    .sysmem             : {}                > DDR_MCU2_1
    .data_buffer        : {} palign(128)    > DDR_MCU2_1
    .bss.devgroup       : {*(.bss.devgroup*)} align(4)      > DDR_MCU2_1
    .const.devgroup     : {*(.const.devgroup*)} align(4)    > DDR_MCU2_1
    .boardcfg_data      : {} align(4)       > DDR_MCU2_1

    ipc_data_buffer (NOINIT) : {} palign(128)   > DDR_MCU2_1
    .resource_table          :
    {
        __RESOURCE_TABLE = .;
    }                                           > DDR_MCU2_1_RESOURCE_TABLE

    intercore_eth_desc_mem (NOLOAD) : {} palign(128) > INTERCORE_ETH_DESC_MEM
    intercore_eth_data_mem (NOLOAD) : {} palign(128) > INTERCORE_ETH_DATA_MEM

    .tracebuf                : {} align(1024)   > DDR_MCU2_1

    .bss:ENET_DMA_DESC_MEMPOOL  (NOLOAD) {} ALIGN (128) > DDR_MCU2_1
    .bss:ENET_DMA_RING_MEMPOOL  (NOLOAD) {} ALIGN (128) > DDR_MCU2_1
    .bss:ENET_DMA_PKT_MEMPOOL   (NOLOAD) {} ALIGN (128) > DDR_MCU2_1
    .bss:ENET_DMA_OBJ_MEM       (NOLOAD) {} ALIGN (128) > DDR_MCU2_1

    .bss:app_log_mem        (NOLOAD) : {} > APP_LOG_MEM
    .bss:ipc_vring_mem      (NOLOAD) : {} > IPC_VRING_MEM
    .bss:ddr_shared_mem     (NOLOAD) : {} > DDR_MCU2_1

    .stack                   : {} align(4)      > DDR_MCU2_1  (HIGH)

    .irqStack   : {. = . + __IRQ_STACK_SIZE;} align(4)      > DDR_MCU2_1  (HIGH)
    RUN_START(__IRQ_STACK_START)
    RUN_END(__IRQ_STACK_END)

    .fiqStack   : {. = . + __FIQ_STACK_SIZE;} align(4)      > DDR_MCU2_1  (HIGH)
    RUN_START(__FIQ_STACK_START)
    RUN_END(__FIQ_STACK_END)

    .abortStack : {. = . + __ABORT_STACK_SIZE;} align(4)    > DDR_MCU2_1  (HIGH)
    RUN_START(__ABORT_STACK_START)
    RUN_END(__ABORT_STACK_END)

    .undStack   : {. = . + __UND_STACK_SIZE;} align(4)      > DDR_MCU2_1  (HIGH)
    RUN_START(__UND_STACK_START)
    RUN_END(__UND_STACK_END)

    .svcStack   : {. = . + __SVC_STACK_SIZE;} align(4)      > DDR_MCU2_1  (HIGH)
    RUN_START(__SVC_STACK_START)
    RUN_END(__SVC_STACK_END)
/* Additional sections settings     */
    McalTextSection : fill=FILL_PATTERN, align=4, load > DDR_MCU2_1
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
    McalConstSection : fill=FILL_PATTERN, align=4, load > DDR_MCU2_1
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

    McalInitSection : fill=FILL_PATTERN, align=4, load > DDR_MCU2_1
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
    McalNoInitSection : fill=FILL_PATTERN, align=4, load > DDR_MCU2_1, type = NOINIT
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

    }
    /* Example Utility specifics */
    VariablesAlignedNoInitSection : align=8, load > DDR_MCU2_1, type = NOINIT
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
    UtilityNoInitSection : align=4, load > DDR_MCU2_1, type = NOINIT
    {
        .=align(4);
        __linker_utility_no_init_start = .;
        . += FILL_LENGTH;
        *(EG_TEST_RESULT_32_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_utility_no_init_end = .;
    }
    SciClientBoardCfgSection : align=128, load > DDR_MCU2_1, type = NOINIT
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
    McalUdmaSection : fill=FILL_PATTERN, align=128, load > DDR_MCU2_1
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
    }
    McalTxDataSection : fill=FILL_PATTERN, align=128, load > DDR_MCU2_1, type = NOINIT
    {
        .=align(128);
        __linker_eth_tx_data_start = .;
        . += FILL_LENGTH;
        *(ETH_TX_DATA_SECTION)
        .=align(128);
        . += FILL_LENGTH;
        __linker_eth_tx_data_end = .;
    }
    McalRxDataSection : fill=FILL_PATTERN, align=128, load > DDR_MCU2_1, type = NOINIT
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


MEMORY
{
    /* R5F_TCMA [ size 32.00 KB ] */
    _VEC                     (    X ) : ORIGIN = 0x00000000 , LENGTH = 0x00000040
    R5F_TCMA                 (    X ) : ORIGIN = 0x00000100 , LENGTH = 0x00007F00
    /* R5F_TCMB0 [ size 32.00 KB ] */
    R5F_TCMB0                ( RWIX ) : ORIGIN = 0x41010000 , LENGTH = 0x00008000
    /* DDR for MCU2_1 for Linux IPC [ size 1024.00 KB ] */
    DDR_MCU2_1_IPC           ( RWIX ) : ORIGIN = 0xA3000000 , LENGTH = 0x00100000
    /* DDR for MCU2_1 for Linux resource table [ size 1024 B ] */
    DDR_MCU2_1_RESOURCE_TABLE ( RWIX ) : ORIGIN = 0xA3100000 , LENGTH = 0x00000400
    /* DDR for MCU2_1 for code/data [ size 63.00 MB ] */
    DDR_MCU2_1               ( RWIX ) : ORIGIN = 0xA3100400 , LENGTH = 0x00EFFC00
    /* Memory for remote core logging [ size 256.00 KB ] */
    APP_LOG_MEM                       : ORIGIN = 0xB8000000 , LENGTH = 0x00040000
    /* Memory for TI OpenVX shared memory. MUST be non-cached or cache-coherent [ size 31.75 MB ] */
    TIOVX_OBJ_DESC_MEM                : ORIGIN = 0xB8040000 , LENGTH = 0x01FC0000
    /* Memory for IPC Vring's. MUST be non-cached or cache-coherent [ size 32.00 MB ] */
    IPC_VRING_MEM                     : ORIGIN = 0xAA000000 , LENGTH = 0x02000000
    /* Memory for shared memory buffers in DDR [ size 560.00 MB ] */
    DDR_SHARED_MEM                    : ORIGIN = 0xBC000000 , LENGTH = 0x23000000
    /* Inter-core ethernet shared desc queues. MUST be non-cached or cache-coherent [ size 2 MB ] */
    INTERCORE_ETH_DESC_MEM            : ORIGIN = 0xAC000000 , LENGTH = 0x00200000
    /* Inter-core ethernet shared data buffers. MUST be non-cached or cache-coherent [ size 30 MB ] */
    INTERCORE_ETH_DATA_MEM            : ORIGIN = 0xAC200000 , LENGTH = 0x01E00000
}
