/*----------------------------------------------------------------------------*/
/* File: k3m4_r5f_linker.cmd                                                  */
/* Description:																  */
/*    Link command file for j721e M4 MCU 0 view							  */
/*	  TI ARM Compiler version 15.12.3 LTS or later							  */
/*                                                                            */
/*    Platform: QT                                                            */
/* (c) Texas Instruments 2017, All rights reserved.                           */
/*----------------------------------------------------------------------------*/
/*  History:															      *'
/*    Aug 26th, 2016 Original version .......................... Loc Truong   */
/*    Aug 01th, 2017 new TCM mem map  .......................... Loc Truong   */
/*    Nov 07th, 2017 Changes for R5F Init Code.................. Vivek Dhande */
/*    Sep 17th, 2018 Added DDR sections for IPC................. J. Bergsagel */
/*    Sep 26th, 2018 Extra mem sections for IPC resource table.. J. Bergsagel */
/*    Nov 06th, 2018 Linker command file for MCU1_1............. J. Bergsagel */
/*    Apr 23th, 2019 Changes for R5F startup Code............... Vivek Dhande */
/*----------------------------------------------------------------------------*/
/* Linker Settings                                                            */
/* Standard linker options													  */
--retain="*(.intvecs)"
--retain="*(.intc_text)"
--retain="*(.rstvectors)"
--retain="*(.utilsCopyVecsToAtcm)"
--fill_value=0
/*----------------------------------------------------------------------------*/
/* Memory Map                                                                 */

--define FILL_PATTERN=0xFEAA55EF
--define FILL_LENGTH=0x100

/*    15 MB 0F00000 Num Slices 6                                */
/*
/*                                            Rounding Offset   */
/*    SBL?    Start   70000000    2621440                       */
/*            End     70280000                                  */
/*    MCU 20  Start   70280100    2621440    100                */
/*            End     70500100                                  */
/*    MCU 21  Start   70500200    2621440    100                */
/*            End     70780200                                  */
/*    MCU 30  Start   70780300    2621440    100                */
/*            End     70A00300                                  */
/*    MCU 31  Start   70A00400    2621440    100                */
/*            End     70C80400                                  */
/*    MPU     Start   70C80500    2621440    100                */
/*            End     70F00500                                  */

MEMORY
{
    /* MCU0_R5F_0 local view 												  */
    MCU0_R5F_TCMA (X)		: origin=0x0			length=0x8000
  /*  MCU0_R5F_TCMB0 (RWIX)	: origin=0x41010000	length=0x8000 */

    /* MCU0_R5F_1 SoC view													  */
    MCU0_R5F1_ATCM (RWIX)  	: origin=0x41400000 length=0x8000
    MCU0_R5F1_BTCM (RWIX) 	: origin=0x41410000 length=0x8000

    /* MCU0 share locations													  */
    OCMRAM 	(RWIX) 			: origin=0x41C00100 length=0x80000 - 0x1100	     /* ~510KB */

    /* j721e MSMC3 Memory													  */
    /* j721e Reserved Memory for ARM Trusted Firmware                        */
    MSMC3_ARM_FW   (RWIX)   : origin=0x70000000 length=0x40000         /* 256KB */
    MSMC3   (RWIX)          : origin=0x70040000 length=0x7B0000        /* 8MB - 320KB */
    /* j721e Reserved Memory for DMSC Firmware                                */
    MSMC3_DMSC_FW  (RWIX)   : origin=0x707F0000 length=0x10000         /* 64KB */

    /* Used in this file */
    DDR0_MCU_2_1 (RWIX)     : origin=0x9B000000 length=0x4000000

    VECTORS (X)             : origin=0x70500200 length=0x1000
    RESET_VECTORS (X)       : origin=0x70501200 length=0x100
    MAIN_MSRAM_0            : origin=0x70501300 length=0x280000 - 0x1100

/* Additional memory settings	*/

}  /* end of MEMORY */

/*----------------------------------------------------------------------------*/
/* Section Configuration                                                      */

SECTIONS
{
/* 'intvecs' and 'intc_text' sections shall be placed within                  */
/* a range of +\- 16 MB                                                       */
    .intvecs 	: {} palign(8) 		> VECTORS
    .intc_text 	: {} palign(8) 		> VECTORS
    .rstvectors 	: {} palign(8) 		> RESET_VECTORS
    .text    	: {} palign(8) 		> MAIN_MSRAM_0
    .const   	: {} palign(8) 		> MAIN_MSRAM_0
    .cinit   	: {} palign(8) 		> MAIN_MSRAM_0
    .pinit   	: {} palign(8) 		> MAIN_MSRAM_0

    /* For NDK packet memory, we need to map this sections before .bss*/
    .bss:NDK_MMBUFFER  (NOLOAD) {} ALIGN (128) > DDR0_MCU_2_1
    .bss:NDK_PACKETMEM (NOLOAD) {} ALIGN (128) > DDR0_MCU_2_1

    .bss     	: {} align(4)  		> MAIN_MSRAM_0
    .far     	: {} align(4)  		> DDR0_MCU_2_1
    .data    	: {} palign(128) 	> MAIN_MSRAM_0
    .data_buffer: {} palign(128) 	> DDR0_MCU_2_1
	.sysmem  	: {} 				> MAIN_MSRAM_0
	.stack  	: {} align(4)		> MAIN_MSRAM_0  (HIGH)  fill=FILL_PATTERN
    .utilsCopyVecsToAtcm : {} palign(8) > MCU0_R5F_TCMB0

    /* USB or any other LLD buffer for benchmarking */
    .benchmark_buffer (NOLOAD) {} ALIGN (8) > DDR0_MCU_2_1

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

        .=align(4);
        __linker_mcu_init_start = .;
        . += FILL_LENGTH;
        *(MCU_DATA_INIT_UNSPECIFIED_SECTION)
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

        __linker_mcu_no_init_start = .;
        . += FILL_LENGTH;
        *(MCU_DATA_NO_INIT_UNSPECIFIED_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_mcu_no_init_end = .;

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
