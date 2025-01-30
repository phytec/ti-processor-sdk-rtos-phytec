/*
 *  Copyright (C) 2024 Texas Instruments Incorporated - www.ti.com
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/**
 *  \ingroup CSL_IP_MODULE
 *  \defgroup CSL_MCSPI McSPI
 *
 *  @{
 */
/**
 * \file       mcspi.h
 *
 * \brief      This file contains the function prototypes for the device
 *             abstraction layer for MCSPI. It also contains necessary
 *             structure, enum and macro definitions.
 *
 * \details    Programming sequence of MCSPI is as follows:
 *             -# MCSPI can be put into reset by calling the API
 *                McSPIReset.
 *             -# MCSPI CS can be enabled by calling the API McSPICSEnable.
 *             -# Polarity of CS can be configured by calling the API
 *                McSPICSPolarityConfig.
 *             -# Master Mode configurations by calling the API
 *                McSPIMasterModeConfig.
 *             -# MCSPI output clock can be configured by calling the API
 *                McSPIClkConfig.
 *             -# Word length of MCSPI can be configured by calling the API
 *                McSPIWordLengthSet.
 *             -# Transmit Fifo can be enabled/disabled by calling the API
 *                McSPITxFIFOConfig.
 *             -# Receive Fifo can be enabled/disabled by calling the API
 *                McSPIRxFIFOConfig.
 *             -# Assert the CS of MCSPI by calling the API McSPICSAssert.
 *             -# Deassert the CS of MCSPI by calling the API McSPICSDeAssert.
 *             -# MCSPI interrupts can be enabled by calling the API
 *                McSPIIntEnable and disabled by calling the API
 *                McSPIIntDisable.
 *             -# MCSPI channel can be enabled by calling the API
 *                McSPIChannelEnable and disabled by calling the API
 *                McSPIChannelDisable.
 *             -# Status on MCSPI interrupts can be checked by calling the API
 *                McSPIIntStatusGet.
 *             -# MCSPI interrupts status can be cleared by calling the API
 *                McSPIIntStatusClear.
 *             -# Data to be transmitted is to be written to the transmit
 *                register by calling the API McSPITransmitData.
 *             -# Received data into the MCSPI receive register can be read
 *                by calling the API McSPIReceiveData.
 *
 **/

#ifndef MCSPI_H_
#define MCSPI_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <hw_include/lldr_mcspi.h>
#include <hw_include/hw_types.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/*
 * \name Values used to determine the channel number used for McSPI
 * communication.
 * @{
 */
/**
 * \brief McSPI channel 0 is used for data communication
 */
#define MCSPI_CHANNEL_0                (0U)

/**
 * \brief McSPI channel 1 is used for data communication
 */
#define MCSPI_CHANNEL_1                (1U)

/**
 * \brief McSPI channel 2 is used for data communication
 */
#define MCSPI_CHANNEL_2                (2U)

/**
 * \brief McSPI channel 3 is used for data communication
 */
#define MCSPI_CHANNEL_3                (3U)
/* @} */

/*
 * \name Values of polarity and phase of SPICLK that are used to configure the
 * clock value in various modes of operation.
 * @{
 */
/**
 * \brief McSPI clock Mode 0 is selected
 *        SPICLK is active high and sampling occurs on the rising edge
 */
#define MCSPI_CLK_MODE_0            (((uint32) MCSPI_CH0CONF_POL_ACTIVEHIGH \
                                      << MCSPI_CH0CONF_POL_SHIFT) |           \
                                     MCSPI_CH0CONF_PHA_ODD)

/**
 * \brief McSPI clock Mode 1 is selected
 *        SPICLK is active high and sampling occurs on the falling edge
 */
#define MCSPI_CLK_MODE_1            (((uint32) MCSPI_CH0CONF_POL_ACTIVEHIGH \
                                      << MCSPI_CH0CONF_POL_SHIFT) |           \
                                     MCSPI_CH0CONF_PHA_EVEN)

/**
 * \brief McSPI clock Mode 2 is selected
 *        SPICLK is active low and sampling occurs on the falling edge
 */
#define MCSPI_CLK_MODE_2            (((uint32) MCSPI_CH0CONF_POL_ACTIVELOW << \
                                      MCSPI_CH0CONF_POL_SHIFT) |                \
                                     MCSPI_CH0CONF_PHA_ODD)

/**
 * \brief McSPI clock Mode 3 is selected
 *        SPICLK is active low and sampling occurs on the rising edge
 */
#define MCSPI_CLK_MODE_3            (((uint32) MCSPI_CH0CONF_POL_ACTIVELOW << \
                                      MCSPI_CH0CONF_POL_SHIFT) |                \
                                     MCSPI_CH0CONF_PHA_EVEN)
/* @} */

/*
 * \name McSPI Register Offset for MCSPI_CHxCONF, MCSPI_CHxSTAT, MCSPI_CHxCTRL,
 * MCSPI_TXx and MCSPI_RXx register set.
 * @{
 */
/**
 * \brief McSPI Register Offset for MCSPI_CHxCONF, MCSPI_CHxSTAT, MCSPI_CHxCTRL,
 *        MCSPI_TXx and MCSPI_RXx register sets
 */
#define MCSPI_REG_OFFSET                (0x14U)

/**
 * \brief Base address of McSPI Channel configuration : MCSPI_CHCONF(x)
 */
#define MCSPI_CHCONF(x)             ((uint32) MCSPI_CH0CONF + \
                                     (uint32) ((uint32) MCSPI_REG_OFFSET * \
                                     ((uint32) (x))))

/**
 * \brief Base address of McSPI Channel status : McSPI_CHSTAT(x)
 */
#define MCSPI_CHSTAT(x)             ((uint32) MCSPI_CH0STAT + \
                                     (uint32) ((uint32) MCSPI_REG_OFFSET * \
                                     ((uint32) (x))))

/**
 * \brief Base address of McSPI_CHCTRL(x) which is used to enable channel
 */
#define MCSPI_CHCTRL(x)             ((uint32) MCSPI_CH0CTRL + \
                                     (uint32) ((uint32) MCSPI_REG_OFFSET * \
                                     ((uint32) (x))))

/**
 * \brief Base address of McSPI_CHTX(x) which is used to store data to be
 *        transmitted
 */
#define MCSPI_CHTX(x)               ((uint32) MCSPI_TX0 + \
                                     (uint32) ((uint32) MCSPI_REG_OFFSET * \
                                     ((uint32) (x))))

/**
 * \brief Base address of McSPI_CHRX(x) which is used to store data to be
 *        received
 */
#define MCSPI_CHRX(x)               ((uint32) MCSPI_RX0 + \
                                     (uint32) ((uint32) MCSPI_REG_OFFSET * \
                                     ((uint32) (x))))
/* @} */

/*
 * \name Values of minumum & maximum word lengths that is valid for SPI channel
 * config.
 * @{
 */
/**
 * \brief Minumum word lengths that is valid for McSPI channel configuration
 */
#define MCSPI_WORD_LENGTH_MIN            ((uint32) 4U)

/**
 * \brief Maximum word lengths that is valid for McSPI channel configuration
 */
#define MCSPI_WORD_LENGTH_MAX            ((uint32) 32U)
/* @} */

/*
 * \name Values used to set the word length for McSPI communication.
 * 'n' can take values only between 4 <= n <= 32.
 * @{
 */
/**
 * \brief Values used to set the word length for McSPI communication
 *        'n' can take values only between 4 <= n <= 32.
 */
#define MCSPI_WORD_LENGTH(n)        ((((uint32) (n)) - \
                                      (uint32) 1U) << MCSPI_CH0CONF_WL_SHIFT)
/* @} */

/*
 * \name Values used to configure the chip select time control(TCS).
 * @{
 */
/**
 * \brief chip select time control(TCS) configuration : Zero interface clock
 *        cycle between CS toggling and first or last edge of SPI clock
 */
#define MCSPI_CS_TCS_0PNT5_CLK      ((uint32) MCSPI_CH0CONF_TCS0_ZEROCYCLEDLY \
                                     << MCSPI_CH0CONF_TCS0_SHIFT)

/**
 * \brief chip select time control(TCS) configuration : One interface clock
 *        cycle between CS toggling and first or last edge of SPI clock
 */
#define MCSPI_CS_TCS_1PNT5_CLK      ((uint32) MCSPI_CH0CONF_TCS0_ONECYCLEDLY \
                                     << MCSPI_CH0CONF_TCS0_SHIFT)

/**
 * \brief chip select time control(TCS) configuration : Two interface clock
 *        cycles between CS toggling and first or last edge of SPI clock
 */
#define MCSPI_CS_TCS_2PNT5_CLK      ((uint32) MCSPI_CH0CONF_TCS0_TWOCYCLEDLY \
                                     << MCSPI_CH0CONF_TCS0_SHIFT)

/**
 * \brief chip select time control(TCS) configuration : Three interface clock
 *        cycles between CS toggling and first or last edge of SPI clock
 */
#define MCSPI_CS_TCS_3PNT5_CLK      ((uint32)                          \
                                     MCSPI_CH0CONF_TCS0_THREECYCLEDLY << \
                                     MCSPI_CH0CONF_TCS0_SHIFT)
/* @} */

/*
 * \name Value used to set the polarity for start bit for McSPI communication.
 * @{
 */
/**
 * \brief Low polarity is set for start bit for McSPI communication.
 */
#define MCSPI_START_BIT_POL_LOW     (MCSPI_CH0CONF_SBPOL_LOWLEVEL)

/**
 * \brief High polarity is set for start bit for McSPI communication.
 */
#define MCSPI_START_BIT_POL_HIGH    (MCSPI_CH0CONF_SBPOL_HIGHLEVEL)
/* @} */

/*
 * \name Values used to determine transmit/receive modes of McSPI peripheral in
 * master mode.
 * @{
 */
/**
 * \brief Transmit-receive mode of McSPI peripheral in master mode is configured
 */
#define MCSPI_TX_RX_MODE            ((uint32) MCSPI_CH0CONF_TRM_TRANSRECEI << \
                                     MCSPI_CH0CONF_TRM_SHIFT)

/**
 * \brief Only Receive mode of McSPI peripheral in master mode is configured
 */
#define MCSPI_RX_ONLY_MODE          ((uint32) MCSPI_CH0CONF_TRM_RECEIVONLY << \
                                     MCSPI_CH0CONF_TRM_SHIFT)

/**
 * \brief Only Transmit mode of McSPI peripheral in master mode is configured
 */
#define MCSPI_TX_ONLY_MODE          ((uint32) MCSPI_CH0CONF_TRM_TRANSONLY << \
                                     MCSPI_CH0CONF_TRM_SHIFT)
/* @} */

/*
 * \name Values used to configure communication on data line pins.
 * @{
 */
/**
 * \brief Communication on Data line pins is configured as :
 *        Data line 0 (SPIDAT[0]) selected for reception
 *        Data line 1 (SPIDAT[1]) selected for transmission
 *        Data Line 0 (SPIDAT[0]) selected for transmission
 */
#define MCSPI_DATA_LINE_COMM_MODE_0  (((uint32) MCSPI_CH0CONF_IS_LINE0 <<     \
                                       MCSPI_CH0CONF_IS_SHIFT) |                \
                                      ((uint32) MCSPI_CH0CONF_DPE1_ENABLED << \
                                       MCSPI_CH0CONF_DPE1_SHIFT) |              \
                                      ((uint32) MCSPI_CH0CONF_DPE0_ENABLED << \
                                       MCSPI_CH0CONF_DPE0_SHIFT))

/**
 * \brief Communication on Data line pins is configured as :
 *        Data line 0 (SPIDAT[0]) selected for reception
 *        Data line 1 (SPIDAT[1]) selected for transmission
 *        No transmission on Data Line 0 (SPIDAT[0])
 */
#define MCSPI_DATA_LINE_COMM_MODE_1  (((uint32) MCSPI_CH0CONF_IS_LINE0 <<     \
                                       MCSPI_CH0CONF_IS_SHIFT) |                \
                                      ((uint32) MCSPI_CH0CONF_DPE1_ENABLED << \
                                       MCSPI_CH0CONF_DPE1_SHIFT) |              \
                                      ((uint32) MCSPI_CH0CONF_DPE0_DISABLED   \
                                       << MCSPI_CH0CONF_DPE0_SHIFT))

/**
 * \brief Communication on Data line pins is configured as :
 *        Data line 0 (SPIDAT[0]) selected for reception
 *        No transmission on Data Line 1 (SPIDAT[1])
 *        Data line 0 (SPIDAT[0]) selected for transmission
 */
#define MCSPI_DATA_LINE_COMM_MODE_2  (((uint32) MCSPI_CH0CONF_IS_LINE0 <<     \
                                       MCSPI_CH0CONF_IS_SHIFT) |                \
                                      ((uint32) MCSPI_CH0CONF_DPE1_DISABLED   \
                                       << MCSPI_CH0CONF_DPE1_SHIFT) |           \
                                      ((uint32) MCSPI_CH0CONF_DPE0_ENABLED << \
                                       MCSPI_CH0CONF_DPE0_SHIFT))

/**
 * \brief Communication on Data line pins is configured as :
 *        Data line 0 (SPIDAT[0]) selected for reception
 *        No transmission on Data Line 1 (SPIDAT[1])
 *        No transmission on Data Line 0 (SPIDAT[0])
 */
#define MCSPI_DATA_LINE_COMM_MODE_3  (((uint32) MCSPI_CH0CONF_IS_LINE0 <<   \
                                       MCSPI_CH0CONF_IS_SHIFT) |              \
                                      ((uint32) MCSPI_CH0CONF_DPE1_DISABLED \
                                       << MCSPI_CH0CONF_DPE1_SHIFT) |         \
                                      ((uint32) MCSPI_CH0CONF_DPE0_DISABLED \
                                       << MCSPI_CH0CONF_DPE0_SHIFT))

/**
 * \brief Communication on Data line pins is configured as :
 *        Data line 1 (SPIDAT[1]) selected for reception
 *        Data line 1 (SPIDAT[1]) selected for transmission
 *        Data Line 0 (SPIDAT[0]) selected for transmission
 */
#define MCSPI_DATA_LINE_COMM_MODE_4  (((uint32) MCSPI_CH0CONF_IS_LINE1 <<     \
                                       MCSPI_CH0CONF_IS_SHIFT) |                \
                                      ((uint32) MCSPI_CH0CONF_DPE1_ENABLED << \
                                       MCSPI_CH0CONF_DPE1_SHIFT) |              \
                                      ((uint32) MCSPI_CH0CONF_DPE0_ENABLED << \
                                       MCSPI_CH0CONF_DPE0_SHIFT))

/**
 * \brief Communication on Data line pins is configured as :
 *        Data line 1 (SPIDAT[1]) selected for reception
 *        Data line 1 (SPIDAT[1]) selected for transmission
 *        No transmission on Data Line 0 (SPIDAT[0])
 */
#define MCSPI_DATA_LINE_COMM_MODE_5  (((uint32) MCSPI_CH0CONF_IS_LINE1 <<     \
                                       MCSPI_CH0CONF_IS_SHIFT) |                \
                                      ((uint32) MCSPI_CH0CONF_DPE1_ENABLED << \
                                       MCSPI_CH0CONF_DPE1_SHIFT) |              \
                                      ((uint32) MCSPI_CH0CONF_DPE0_DISABLED   \
                                       << MCSPI_CH0CONF_DPE0_SHIFT))

/**
 * \brief Communication on Data line pins is configured as :
 *        Data line 1 (SPIDAT[1]) selected for reception
 *        No transmission on Data Line 1 (SPIDAT[1])
 *        Data line 0 (SPIDAT[0]) selected for transmission
 */
#define MCSPI_DATA_LINE_COMM_MODE_6  (((uint32) MCSPI_CH0CONF_IS_LINE1 <<     \
                                       MCSPI_CH0CONF_IS_SHIFT) |                \
                                      ((uint32) MCSPI_CH0CONF_DPE1_DISABLED   \
                                       << MCSPI_CH0CONF_DPE1_SHIFT) |           \
                                      ((uint32) MCSPI_CH0CONF_DPE0_ENABLED << \
                                       MCSPI_CH0CONF_DPE0_SHIFT))

/**
 * \brief Communication on Data line pins is configured as :
 *        Data line 1 (SPIDAT[1]) selected for reception
 *        No transmission on Data Line 1 (SPIDAT[1])
 *        No transmission on Data Line 0 (SPIDAT[0])
 */
#define MCSPI_DATA_LINE_COMM_MODE_7   (((uint32) MCSPI_CH0CONF_IS_LINE1 <<   \
                                        MCSPI_CH0CONF_IS_SHIFT) |              \
                                       ((uint32) MCSPI_CH0CONF_DPE1_DISABLED \
                                        << MCSPI_CH0CONF_DPE1_SHIFT) |         \
                                       ((uint32) MCSPI_CH0CONF_DPE0_DISABLED \
                                        << MCSPI_CH0CONF_DPE0_SHIFT))
/* @} */

/*
 * \name Values used to enable/disable the Tx/Rx FIFOs of McSPI peripheral.
 * @{
 */
/**
 * \brief McSPI peripheral Rx FIFO is enabled
 */
#define MCSPI_RX_FIFO_ENABLE         ((uint32) MCSPI_CH0CONF_FFER_FFENABLED \
                                      <<                                      \
                                      MCSPI_CH0CONF_FFER_SHIFT)

/**
 * \brief McSPI peripheral Rx FIFO is disabled
 */
#define MCSPI_RX_FIFO_DISABLE        ((uint32) MCSPI_CH0CONF_FFER_FFDISABLED \
                                      << MCSPI_CH0CONF_FFER_SHIFT)

/**
 * \brief McSPI peripheral Tx FIFO is enabled
 */
#define MCSPI_TX_FIFO_ENABLE         ((uint32) MCSPI_CH0CONF_FFEW_FFENABLED \
                                      << MCSPI_CH0CONF_FFEW_SHIFT)

/**
 * \brief McSPI peripheral Tx FIFO is disabled
 */
#define MCSPI_TX_FIFO_DISABLE        ((uint32) MCSPI_CH0CONF_FFEW_FFDISABLED \
                                      << MCSPI_CH0CONF_FFEW_SHIFT)
/* @} */

/*
 * \name Values used to enable/disable the read/write DMA events of McSPI
 * peripheral.
 * @{
 */
/**
 * \brief McSPI peripheral read DMA event is enabled
 */
#define MCSPI_DMA_RX_EVENT           ((uint32) MCSPI_CH0CONF_DMAR_ENABLED << \
                                      MCSPI_CH0CONF_DMAR_SHIFT)

/**
 * \brief McSPI peripheral write DMA event is enabled
 */
#define MCSPI_DMA_TX_EVENT           ((uint32) MCSPI_CH0CONF_DMAW_ENABLED << \
                                      MCSPI_CH0CONF_DMAW_SHIFT)
/* @} */

/*
 * \name Values that can be passed to enable/disable/clear status of  the
 * various interrupts of McSPI peripheral.
 * These macros can also be used to check the status obtained from
 * 'McSPIIntStatusGet' API.
 * @{
 */
/**
 * \brief Mask value of TX Empty interrupt enable of McSPI peripheral for the
 *        corresponding channel
 */
#define MCSPI_INT_TX_EMPTY(chan)     ((uint32)                               \
                                      MCSPI_IRQENABLE_TX0_EMPTY_ENABLE_MASK << \
                                      ((chan) * 4U))

/**
 * \brief Mask value of TX Underflow interrupt enable of McSPI peripheral for
 *        the corresponding channel
 */
#define MCSPI_INT_TX_UNDERFLOW(chan) ((uint32)                                \
                                      MCSPI_IRQENABLE_TX0_UNDERFLOW_ENABLE_MASK \
                                      << ((chan) * 4U))

/**
 * \brief Mask value of RX Full interrupt enable of McSPI peripheral for
 *        the corresponding channel
 */
#define MCSPI_INT_RX_FULL(chan)      ((uint32)                              \
                                      MCSPI_IRQENABLE_RX0_FULL_ENABLE_MASK << \
                                      ((chan) * 4U))

/**
 * \brief Mask value of RX Overflow interrupt status of McSPI peripheral
 */
#define MCSPI_INT_RX0_OVERFLOW       ((uint32) \
                                      MCSPI_IRQSTATUS_RX0_OVERFLOW_MASK)

/**
 * \brief Mask value of End of word count interrupt enable of McSPI peripheral
 */
#define MCSPI_INT_EOWKE              ((uint32)                               \
                                      MCSPI_IRQENABLE_EOW_ENABLE_IRQENABLED << \
                                      MCSPI_IRQENABLE_EOW_ENABLE_SHIFT)
/* @} */

/*
 * \name Values used to enable initial delay for first transfer from McSPI
 * peripheral.
 * @{
 */
/**
 * \brief No delay is configured for first spi transfer from McSPI peripheral.
 */
#define MCSPI_INITDLY_0              ((uint32)                         \
                                      MCSPI_MODULCTRL_INITDLY_NODELAY << \
                                      MCSPI_MODULCTRL_INITDLY_SHIFT)
/**
 * \brief 4 SPI bus clock delays is configured for first spi transfer from McSPI
 *        peripheral.
 */
#define MCSPI_INITDLY_4              ((uint32)                         \
                                      MCSPI_MODULCTRL_INITDLY_4CLKDLY << \
                                      MCSPI_MODULCTRL_INITDLY_SHIFT)

/**
 * \brief 8 SPI bus clock delays is configured for first spi transfer from McSPI
 *        peripheral.
 */
#define MCSPI_INITDLY_8              ((uint32)                         \
                                      MCSPI_MODULCTRL_INITDLY_8CLKDLY << \
                                      MCSPI_MODULCTRL_INITDLY_SHIFT)

/**
 * \brief 16 SPI bus clock delays is configured for first spi transfer from
 *        McSPI peripheral.
 */
#define MCSPI_INITDLY_16             ((uint32)                          \
                                      MCSPI_MODULCTRL_INITDLY_16CLKDLY << \
                                      MCSPI_MODULCTRL_INITDLY_SHIFT)

/**
 * \brief 32 SPI bus clock delays is configured for first spi transfer from
 *        McSPI peripheral.
 */
#define MCSPI_INITDLY_32             ((uint32)                          \
                                      MCSPI_MODULCTRL_INITDLY_32CLKDLY << \
                                      MCSPI_MODULCTRL_INITDLY_SHIFT)
/* @} */

/*
 * \name Values used to check the status of McSPI channel status registers and
 * FIFO.
 * @{
 */
/**
 * \brief McSPI channel status if Rx buffer is full
 */
#define MCSPI_CH_STAT_RXS_FULL       ((uint32) MCSPI_CH0STAT_RXS_FULL << \
                                      MCSPI_CH0STAT_RXS_SHIFT)

/**
 * \brief McSPI channel status if Tx buffer is empty
 */
#define MCSPI_CH_STAT_TXS_EMPTY      ((uint32) MCSPI_CH0STAT_TXS_EMPTY << \
                                      MCSPI_CH0STAT_TXS_SHIFT)

/**
 * \brief McSPI channel status if End of Transfer is completed
 */
#define MCSPI_CH_STAT_EOT            ((uint32) MCSPI_CH0STAT_EOT_COMPLETED << \
                                      MCSPI_CH0STAT_EOT_SHIFT)

/**
 * \brief McSPI channel status if Tx FIFO buffer is empty
 */
#define MCSPI_CH_TXFFE               ((uint32) MCSPI_CH0STAT_TXFFE_EMPTY << \
                                      MCSPI_CH0STAT_TXFFE_SHIFT)

/**
 * \brief McSPI channel status if Tx FIFO buffer is full
 */
#define MCSPI_CH_TXFFF               ((uint32) MCSPI_CH0STAT_TXFFF_FULL << \
                                      MCSPI_CH0STAT_TXFFF_SHIFT)

/**
 * \brief McSPI channel status if Rx FIFO buffer is empty
 */
#define MCSPI_CH_RXFFE               ((uint32) MCSPI_CH0STAT_RXFFE_EMPTY << \
                                      MCSPI_CH0STAT_RXFFE_SHIFT)

/**
 * \brief McSPI channel status if Rx FIFO buffer is full
 */
#define MCSPI_CH_RXFFF               ((uint32) MCSPI_CH0STAT_RXFFF_FULL << \
                                      MCSPI_CH0STAT_RXFFF_SHIFT)
/* @} */

/*
 * \name Value used to enable/disable multiple word ocp access.
 * @{
 */
/**
 * \brief Multiple word ocp access is enabled
 */
#define MCSPI_MOA_ENABLE            ((uint32) MCSPI_MODULCTRL_MOA_MULTIACCES \
                                     << MCSPI_MODULCTRL_MOA_SHIFT)

/**
 * \brief Multiple word ocp access is disabled
 */
#define MCSPI_MOA_DISABLE           ((uint32)                           \
                                     MCSPI_MODULCTRL_MOA_NOMULTIACCESS << \
                                     MCSPI_MODULCTRL_MOA_SHIFT)
/* @} */

/*
 * \name Value used to configure to single/multiple channel.
 * @{
 */
/**
 * \brief Single channel is configured for MCSPI Peripheral
 */
#define MCSPI_SINGLE_CH            ((uint32) MCSPI_MODULCTRL_SINGLE_SINGLE << \
                                    MCSPI_MODULCTRL_SINGLE_SHIFT)

/**
 * \brief Multi channel is configured for MCSPI Peripheral
 */
#define MCSPI_MULTI_CH            ((uint32) MCSPI_MODULCTRL_SINGLE_MULTI << \
                                   MCSPI_MODULCTRL_SINGLE_SHIFT)
/* @} */

/*
 * \name Value used to configure the Chip select Polarity
 * @{
 */
/**
 * \brief Chip select is held high during active state
 */
#define MCSPI_CS_POL_HIGH          ((uint32) MCSPI_CH0CONF_EPOL_ACTIVEHIGH << \
                                    MCSPI_CH0CONF_EPOL_SHIFT)

/**
 * \brief Chip select is held low during active state
 */
#define MCSPI_CS_POL_LOW           ((uint32) MCSPI_CH0CONF_EPOL_ACTIVELOW << \
                                    MCSPI_CH0CONF_EPOL_SHIFT)
/* @} */

/*
 * \name Value used to enable/disable FDAA operation of McSPI peripheral.
 * @{
 */
/**
 * \brief FDAA operation of McSPI peripheral is disabled
 */
#define MCSPI_FDAA_DISABLE           ((uint32)                          \
                                      MCSPI_MODULCTRL_FDAA_SHADOWREGEN << \
                                      MCSPI_MODULCTRL_FDAA_SHIFT)

/**
 * \brief FDAA operation of McSPI peripheral is enabled
 */
#define MCSPI_FDAA_ENABLE            ((uint32)                          \
                                      MCSPI_MODULCTRL_FDAA_NOSHADOWREG << \
                                      MCSPI_MODULCTRL_FDAA_SHIFT)
/* @} */

/*
 * \name Values used to configure the SYSCONFIG
 * @{
 */
/**
 * \brief Both OCP and Functional clock is switched off
 */
#define MCSPI_CLOCKS_OCP_OFF_FUNC_OFF ((uint32)                            \
                                       MCSPI_SYSCONFIG_CLOCKACTIVITY_NONE << \
                                       MCSPI_SYSCONFIG_CLOCKACTIVITY_SHIFT)

/**
 * \brief OCP clock is maintained but Functional clock is switched off
 */
#define MCSPI_CLOCKS_OCP_ON_FUNC_OFF  ((uint32)                           \
                                       MCSPI_SYSCONFIG_CLOCKACTIVITY_OCP << \
                                       MCSPI_SYSCONFIG_CLOCKACTIVITY_SHIFT)

/**
 * \brief Functional clock is maintained but OCP clock is switched off
 */
#define MCSPI_CLOCKS_OCP_OFF_FUNC_ON  ((uint32)                            \
                                       MCSPI_SYSCONFIG_CLOCKACTIVITY_FUNC << \
                                       MCSPI_SYSCONFIG_CLOCKACTIVITY_SHIFT)

/**
 * \brief Both OCP and Functional clock is maintained
 */
#define MCSPI_CLOCKS_OCP_ON_FUNC_ON   ((uint32)                            \
                                       MCSPI_SYSCONFIG_CLOCKACTIVITY_BOTH << \
                                       MCSPI_SYSCONFIG_CLOCKACTIVITY_SHIFT)

/**
 * \brief Force Idle Mode is requested for MCSPI peripheral
 */
#define MCSPI_SIDLEMODE_FORCE         ((uint32)                         \
                                       MCSPI_SYSCONFIG_SIDLEMODE_FORCE << \
                                       MCSPI_SYSCONFIG_SIDLEMODE_SHIFT)

/**
 * \brief No Idle Mode is requested for MCSPI peripheral
 */
#define MCSPI_SIDLEMODE_NO            ((uint32) MCSPI_SYSCONFIG_SIDLEMODE_NO \
                                       << MCSPI_SYSCONFIG_SIDLEMODE_SHIFT)

/**
 * \brief Smart Idle Mode is requested for MCSPI peripheral
 */
#define MCSPI_SIDLEMODE_SMART_IDLE    ((uint32)                         \
                                       MCSPI_SYSCONFIG_SIDLEMODE_SMART << \
                                       MCSPI_SYSCONFIG_SIDLEMODE_SHIFT)

/*TI_INSPECTED 67 X : MISRAC_2012_D.4.5
 * "Reason - LDRA tool issue : Not able to understand unique Identifier with
 *  "-" " */
/**
 * \brief Wake-up feature control is enabled
 */
#define MCSPI_WAKEUP_ENABLE           ((uint32) MCSPI_SYSCONFIG_ENAWAKEUP_ON \
                                       << MCSPI_SYSCONFIG_ENAWAKEUP_SHIFT)

/**
 * \brief Wake-up feature control is disabled
 */
#define MCSPI_WAKEUP_DISABLE          ((uint32)                            \
                                       MCSPI_SYSCONFIG_ENAWAKEUP_NOWAKEUP << \
                                       MCSPI_SYSCONFIG_ENAWAKEUP_SHIFT)

/**
 * \brief Auromatic OCP clock gating is configured for MCSPI peripheral
 */
#define MCSPI_AUTOIDLE_ON             ((uint32) MCSPI_SYSCONFIG_AUTOIDLE_ON \
                                       << MCSPI_SYSCONFIG_AUTOIDLE_SHIFT)

/**
 * \brief OCP clock is configured as free running state for MCSPI peripheral
 */
#define MCSPI_AUTOIDLE_OFF            ((uint32) MCSPI_SYSCONFIG_AUTOIDLE_OFF \
                                       << MCSPI_SYSCONFIG_AUTOIDLE_SHIFT)
/* @} */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

extern void McSPIIntEnable(uint32 baseAddr, uint32 intFlags);

/**
 * \brief  This API will disable the McSPI Interrupts.
 *
 * \param  baseAddr        Memory Address of the McSPI instance used.
 * \param  intFlags        Represents the various interrupts to be disabled.
 *
 *         'intFlags' can take the following values.\n
 *         MCSPI_INT_TX_EMPTY(chan) - Transmitter register empty interrupt.\n
 *         MCSPI_INT_TX_UNDERFLOW(chan) - Transmitter register underflow
 *                                        interrupt.\n
 *         MCSPI_INT_RX_FULL(chan) - Receiver register full interrupt.\n
 *         MCSPI_INT_RX0_OVERFLOW - Receiver register 0 overflow interrupt.\n
 *         MCSPI_INT_EOWKE - End of word count interrupt.\n
 *
 *         'chan' stands for channel number.
 *         Please specify the proper channel number while passing the macros.
 *         0 <= chan <= 3 \n
 *
 *         'chNum' can take the following values.\n
 *         MCSPI_CHANNEL_n - Channel n is used for communication.\n
 *
 *         For chNum n can range from 0-3.\n
 *
 * \return none.
 *
 * \note   Please ensure the proper channel number is passed while using the
 *         macros.
 **/
extern void McSPIIntDisable(uint32 baseAddr, uint32 intFlags);

/**
 * \brief  This API will set initial delay for first transfer from McSPI
 *         peripheral.
 *
 * \param  baseAddr        Memory Address of the McSPI instance used.
 * \param  initDelay       Delay used for first transfer.\n
 *
 *         'initDelay' can take the following values.\n
 *         MCSPI_INITDLY_0  - No delay for first transfer.\n
 *         MCSPI_INITDLY_4  - Delay of 4 SPI Clock.\n
 *         MCSPI_INITDLY_8  - Delay of 8 SPI Clock.\n
 *         MCSPI_INITDLY_16 - Delay of 16 SPI Clock.\n
 *         MCSPI_INITDLY_32 - Delay of 32 SPI Clock.\n
 *
 * \return none.
 *
 * \note:  Please note that this option is available only in single master mode.
 **/
extern uint32 McSPIIntStatusGet(uint32 baseAddr);

/**
 * \brief  This API will clear the status of McSPI Interrupts.
 *
 * \param  baseAddr        Memory Address of the McSPI instance used.
 * \param  intFlags        Represents the various interrupts to be cleared.
 *
 * \return none.
 *
 * \note   Please ensure the proper channel number is used while passing the
 *         macros passed.\n
 **/
extern void McSPIIntStatusClear(uint32 baseAddr, uint32 intFlags);

/**
 * \brief  This API will return the status of the McSPI channel currently in
 *         use.
 *
 * \param  baseAddr        Memory Address of the McSPI instance used.
 * \param  chNum           Channel used for communication.\n
 *
 *         'chNum' can take the following values.\n
 *         MCSPI_CHANNEL_n - Channel n is used for communication.\n
 *
 * \return This API will return the status of the McSPI channel status
 *         register.
 *         User can use the following macros to check the status \n
 *         MCSPI_CH_STAT_RXS_FULL - Receiver register is full \n
 *         MCSPI_CH_STAT_TXS_EMPTY - Transmitter register is full \n
 *         MCSPI_CH_STAT_EOT - End of transfer status \n
 *         MCSPI_CH_TXFFE - FIFO transmit buffer empty status \n
 *         MCSPI_CH_TXFFF - FIFO transmit buffer full status \n
 *         MCSPI_CH_RXFFE - FIFO receive buffer empty status \n
 *         MCSPI_CH_RXFFF - FIFO receive buffer full status \n
 **/

#ifdef __cplusplus
}
#endif
#endif  /* MCSPI_H_ */
 /** @} */
/********************************* End of file ******************************/
