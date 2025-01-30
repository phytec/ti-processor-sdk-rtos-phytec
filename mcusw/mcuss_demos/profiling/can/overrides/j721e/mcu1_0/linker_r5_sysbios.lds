/*----------------------------------------------------------------------------*/
/* File: k3m4_r5f_linker.cmd                                                  */
/* Description:																  */
/*    Link command file for j721e M4 MCU 0 view							  */
/*	  TI ARM Compiler version 15.12.3 LTS or later							  */
/*                                                                            */
/*    Platform: QT                                                            */
/* (c) Texas Instruments 2019-2020, All rights reserved.                      */
/*----------------------------------------------------------------------------*/
/*  History:								      */
/*    Aug 26th, 2016 Original version .......................... Loc Truong   */
/*    Aug 01th, 2017 new TCM mem map  .......................... Loc Truong   */
/*    Nov 07th, 2017 Changes for R5F Init Code.................. Vivek Dhande */
/*----------------------------------------------------------------------------*/
/* Linker Settings                                                            */
/* Standard linker options													  */
--retain="*(.intvecs)"
--retain="*(.intc_text)"
--retain="*(.rstvectors)"
--retain="*(.utilsCopyVecsToAtcm)"
--fill_value=0
-e __VECS_ENTRY_POINT
/*----------------------------------------------------------------------------*/
/* Memory Map                                                                 */

--define FILL_PATTERN=0xFEAA55EF
--define FILL_LENGTH=0x100

/* 1 MB of MCU Domain MSRAM is split as shown below */
/* Size used  F0000 Number of slices 4 */
/*                                  Rounding Offset */
/*SBL?      Start   41C00000    245760  0   */
/*          End     41C3C000                */
/*MCU 10    Start   41C3C100    245760  100 */
/*          End     41C78100                */
/*MCU 11    Start   41C78200    245760  100 */
/*          End     41CB4200                */

MEMORY
{
    /* MCU0_R5F_0 local view */
    MCU0_R5F_TCMA_SBL_RSVD (X)  : origin=0x0        length=0x100
    MCU0_R5F_TCMA (X)       : origin=0x100      length=0x8000 - 0x100

    /* MCU0_R5F_0 SoC view */
    MCU0_R5F0_ATCM (RWIX)   : origin=0x41000000 length=0x8000
    MCU0_R5F0_BTCM (RWIX)   : origin=0x41010000 length=0x8000
    /* MCU0_R5F_1 SoC view */
    MCU0_R5F1_ATCM (RWIX)   : origin=0x41400000 length=0x8000
    MCU0_R5F1_BTCM (RWIX)   : origin=0x41410000 length=0x8000

    /* j721e MCMS3 locations */
    /* j721e Reserved Memory for ARM Trusted Firmware */
    MSMC3_ARM_FW   (RWIX)   : origin=0x70000000 length=0x40000         /* 256KB */
    MSMC3   (RWIX)          : origin=0x70040000 length=0x7B0000        /* 8MB - 320KB */
    /* j721e Reserved Memory for DMSC Firmware */
    MSMC3_DMSC_FW  (RWIX)   : origin=0x707F0000 length=0x10000         /* 64KB */

    DDR0    (RWIX)          : origin=0x80000000 length=0x8000000      /* 2GB */

    /* Used in this file */
    DDR0_MCU_1_0 (RWIX)     : origin=0x97000000 length=0x1000000       /* 16MB */


    /* Refer the user guide for details on persistence of these sections */
    OCMC_RAM_BOARD_CFG (RWIX)   : origin=0x41C80000 length=0x2000
    OCMC_RAM_SCISERVER (RWIX)   : origin=0x41C82000 length=0x60000
    RESET_VECTORS (X)           : origin=0x41CE2000 length=0x100
    OCMC_RAM (RWIX)             : origin=0x41CE2100 length=0x1DA00
    OCMC_RAM_X509_HEADER (RWIX) : origin=0x41CFFB00 length=0x500


}  /* end of MEMORY */

/*----------------------------------------------------------------------------*/
/* Section Configuration                                                      */

SECTIONS
{
    .vecs : {
         __VECS_ENTRY_POINT = .;
    } palign(8) > MCU0_R5F0_BTCM
    .intvecs 	: {} palign(8) 		> RESET_VECTORS
    .intc_text 	: {} palign(8) 		> RESET_VECTORS
    .rstvectors : {} palign(8)      > RESET_VECTORS
    .text    	: {} palign(8) 		> OCMC_RAM_SCISERVER
    .const   	: {} palign(8) 		> MSMC3
    .cinit   	: {} palign(8) 		> OCMC_RAM
    .pinit   	: {} palign(8) 		> OCMC_RAM

    /* For NDK packet memory, we need to map this sections before .bss*/
    .bss:NDK_MMBUFFER  (NOLOAD) {} ALIGN (128) > OCMC_RAM_SCISERVER
    .bss:NDK_PACKETMEM (NOLOAD) {} ALIGN (128) > OCMC_RAM_SCISERVER

    .bss     	: {} align(4)  		> MSMC3
    .far     	: {} align(4)  		> MSMC3
    .data    	: {} palign(128) 	> MSMC3
    .data_buffer: {} palign(128) 	> MSMC3
	.sysmem  	: {} 				> MSMC3
	.stack  	: {} align(4)		> MSMC3  (HIGH) fill=FILL_PATTERN
    .utilsCopyVecsToAtcm : {} palign(8) > MCU0_R5F0_BTCM
    .bss.devgroup : {*(.bss.devgroup*)}  align(4)       > MSMC3
    .const.devgroup: {*(.const.devgroup*)} align(4)      > MSMC3
    .data_user      : {} align(4)      > OCMC_RAM_SCISERVER
    .boardcfg_data  : {} align(4)      > OCMC_RAM_SCISERVER

    /* USB or any other LLD buffer for benchmarking */
    .benchmark_buffer (NOLOAD) {} ALIGN (8) > OCMC_RAM_SCISERVER

    /* Additional sections settings     */
    McalTextSection : fill=FILL_PATTERN, align=4, load > OCMC_RAM_SCISERVER
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
    McalConstSection : fill=FILL_PATTERN, align=4, load > OCMC_RAM_SCISERVER
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

    McalInitSection : fill=FILL_PATTERN, align=4, load > OCMC_RAM_SCISERVER
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
    McalNoInitSection : fill=FILL_PATTERN, align=4, load > OCMC_RAM_SCISERVER, type = NOINIT
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
    VariablesAlignedNoInitSection : align=8, load > OCMC_RAM_SCISERVER, type = NOINIT
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
    UtilityNoInitSection : align=4, load > OCMC_RAM, type = NOINIT
    {
        .=align(4);
        __linker_utility_no_init_start = .;
        . += FILL_LENGTH;
        *(EG_TEST_RESULT_32_SECTION)
        .=align(4);
        . += FILL_LENGTH;
        __linker_utility_no_init_end = .;
    }
    SciClientBoardCfgSection : align=128, load > OCMC_RAM_SCISERVER, type = NOINIT
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
     * it in OCMRAM or OCMC_RAM_SCISERVER */
    McalUdmaSection : fill=FILL_PATTERN, align=128, load > OCMC_RAM_SCISERVER
    {
        .=align(128);
        __linker_eth_udma_start = .;
        . += FILL_LENGTH;
        *(ETH_UDMA_SECTION)
        .=align(128);
        . += FILL_LENGTH;
        __linker_eth_udma_end = .;
    }
    McalTxDataSection : fill=FILL_PATTERN, align=128, load > OCMC_RAM_SCISERVER, type = NOINIT
    {
        .=align(128);
        __linker_eth_tx_data_start = .;
        . += FILL_LENGTH;
        *(ETH_TX_DATA_SECTION)
        .=align(128);
        . += FILL_LENGTH;
        __linker_eth_tx_data_end = .;
    }
    McalRxDataSection : fill=FILL_PATTERN, align=128, load > OCMC_RAM_SCISERVER, type = NOINIT
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
