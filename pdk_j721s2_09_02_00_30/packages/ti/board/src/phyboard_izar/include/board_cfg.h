/******************************************************************************
 * Copyright (c) 2021-2022 Texas Instruments Incorporated - http://www.ti.com
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
 *****************************************************************************/

/**
 * \brief  Board library configurations
 *
 * This file configures the instance numbers, address and gpio reset
 * details of different interfaces of the board.
 *
 */
#ifndef BOARD_CFG_H_
#define BOARD_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <ti/board/board.h>

/* Board ID information */
#define BOARD_INFO_CPU_NAME     "j721s2"
#define BOARD_INFO_BOARD_NAME   "phyboard_izar"

/* Memory sections */
#define BOARD_DDR_START_ADDR                            (0x80000000U)
#define BOARD_DDR_SIZE                                  (2048 * 1024 * 1024UL)
#define BOARD_DDR_END_ADDR                              (0xFFFFFFFFU)
/* Note with ECC enabled, all memory is not usable: 1/8 memory used for inline ECC */
#define BOARD_DDR_ECC_END_ADDR                          (0xF1C71C00U)
/* Enable/Disable DDR Memory Prime for ECC. Define the following macro to enable. */
#undef BOARD_DDR_ENABLE_DDR_MEM_PRIME

/* UART LLD instance number for primary UART port */
#define BOARD_UART_INSTANCE                             (8U)
/* UART LLD instance number for MAIN UART1 port */
#define BOARD_UART1_INSTANCE                            (1U)
/* UART LLD instance number for MAIN UART2 port */
#define BOARD_UART2_INSTANCE                            (2U)
/* UART LLD instance number for MAIN UART4 port */
#define BOARD_UART4_INSTANCE                            (4U)
/* UART LLD instance number for MAIN UART5 port */
#define BOARD_UART5_INSTANCE                            (5U)
/* MCU UART LLD instance number */
#define BOARD_MCU_UART_INSTANCE                         (0U)
/* WKUP UART LLD instance number */
#define BOARD_WKUP_UART_INSTANCE                        (0U)
/* ICSSG UART instance number */
#define BOARD_ICSSG_UART_INSTANCE                       (4U)

/* QSPI instance number */
#define BOARD_SPI_NOR_INSTANCE                          (1U)

/* I2C Instance connected to clock generator */
#define BOARD_CLOCK_GENERATOR_INSTANCE                  (0U)
/* I2C slave address of clock generator */
#define BOARD_I2C_CLOCK_GENERATOR1                      (0)
#define BOARD_I2C_CLOCK_GENERATOR2_ADDR1                (0x76)
#define BOARD_I2C_CLOCK_GENERATOR2_ADDR2                (0x77)
#define BOARD_I2C_QSGMII_CLOCK_GENERATOR_ADDR1          (0x76)
#define BOARD_I2C_QSGMII_CLOCK_GENERATOR_ADDR2          (0x77)
#define BOARD_I2C_PERI_CLOCK_GENERATOR                  (0x6D)

/* OSPI instance connected to OSPI NOR flash */
#define BOARD_OSPI_NOR_INSTANCE                         (0U)

/* OSPI instance connected to OSPI NOR flash */
#define BOARD_OSPI_NAND_INSTANCE                        (0U)

/* HyperFlash instance number */
#define BOARD_HPF_INSTANCE                              (0)

/* I2C instance connected to PMIC - WKUP I2C0 */
#define BOARD_I2C_PMIC_INSTANCE                         (0U)

/* I2C slave address of Leo PMICs */
#define BOARD_I2C_LEO_PMIC_A_ADDR                       (0x48U)
#define BOARD_I2C_LEO_PMIC_B_ADDR                       (0x4CU)
#define BOARD_I2C_LEO_PMIC_A_WDT_ADDR                   (0x12U)
#define BOARD_I2C_LEO_PMIC_B_WDT_ADDR                   (0x13U)

/* OSPI instance number */
#define BOARD_OSPI_INSTANCE                             (0)

/* I2C instance for External RTC */
#define BOARD_I2C_EXT_RTC_INSTANCE                      (0U)

/* I2C address for External RTC */
#define BOARD_I2C_EXT_RTC_ADDR                          (0x6FU)

/* User LED Pin Details */
#define BOARD_USER_LED1                                 (6U)
#define BOARD_USER_LED2                                 (7U)

#define BOARD_ICSS_EMAC_PORT_MAX                        (1U)
#define BOARD_CPSW9G_EMAC_PORT_MAX                      (0U)
#define BOARD_CPSW9G_PORT_MAX                           (0U)


/* ICSS0 EMAC PHY register address */
#define BOARD_ICSS0_EMAC_PHY0_ADDR                      (0x0)
#define BOARD_ICSS0_EMAC_PHY1_ADDR                      (0x3u)

/* ICSS1 EMAC PHY register address */
#define BOARD_ICSS1_EMAC_PHY0_ADDR                      (0xCu)
#define BOARD_ICSS1_EMAC_PHY1_ADDR                      (0xFu)

/* PRG0_RGMII_INTn */
#define BOARD_GPIO_ICSS0_EMAC_INT_PORT_NUM              (1U) /* GPIO1_23 */
#define BOARD_GPIO_ICSS0_EMAC_INT_PIN_NUM               (0x17U)

/* PRG1_RGMII_INTn */
#define BOARD_GPIO_ICSS1_EMAC_INT_PORT_NUM              (1U) /* GPIO1_24 */
#define BOARD_GPIO_ICSS1_EMAC_INT_PIN_NUM               (0x18U)

/* MCU EMAC PHY MDIO address */
#define BOARD_MCU_EMAC_PHY_ADDR                         (0U)

/* MAIN EMAC PHY MDIO address */
#define BOARD_MAIN_EMAC_PHY_ADDR                        (0U)

/* MCU EMAC MAX REG DUMP */
#define BOARD_MCU_EMAC_REG_DUMP_MAX                     ((16U))

/* MCU EMAC PHY register address definitions for reading strap values */
#define BOARD_MCU_EMAC_STRAP_STS1_ADDR                  (0x6EU)
#define BOARD_MCU_EMAC_STRAP_STS2_ADDR                  (0x6FU)

/* ICSS EMAC PHY register address definitions for reading strap values */
#define BOARD_ICSS_EMAC_STRAP_STS1_ADDR                (0x6EU)
#define BOARD_ICSS_EMAC_STRAP_STS2_ADDR                (0x6FU)

/* MCU_ETH1_INTn */
#define BOARD_GPIO_MCU_EMAC_INT_PORT_NUM                (0U) /* WKUP_GPIO0_55 */
#define BOARD_GPIO_MCU_EMAC_INT_PIN_NUM                 (0X37U)

/* AUTOMATION HEADER */
#define BOARD_TEST_HEADER_I2C_INSTANCE                  (1U)
#define BOARD_I2C_BOOT_MODE_SW_ADDR                     (0x22U)

/* Temperature sensor i2c instance */
#define BOARD_TEMP_SENSOR_I2C_INSTANCE                  (0U)

/* Temperature sensor slave device addresses */
#define BOARD_TEMP_SENSOR_I2C_SLAVE_DEVICE1_ADDR        (0x48U)
#define BOARD_TEMP_SENSOR_I2C_SLAVE_DEVICE2_ADDR        (0x49U)

#define BOARD_I2C_CURRENT_MONITOR_INSTANCE              (1U)

/* Instance for interfaces connected to MMCSD */
#define BOARD_MMCSD_SD_INSTANCE                         (1U)
#define BOARD_MMCSD_EMMC_INSTANCE                       (0U)

/* OSPI Domain number */
#define BOARD_OSPI_DOMAIN                               (0U)

/* Enable NOR flash driver */
#define BOARD_NOR_FLASH_IN

/* Enable NAND flash driver */
#define BOARD_NAND_FLASH_IN

/* PinMux data to be programmed to configure a pin to be a GPIO */
#define PINMUX_GPIO_CFG                                 (0x00050007U)

/* Maximum number of SoC domains */
#define BOARD_SOC_DOMAIN_MAX     (3U)
/* Value for indicating SoC main domain */
#define BOARD_SOC_DOMAIN_MAIN    (0)
/* Value for indicating SoC wake-up domain */
#define BOARD_SOC_DOMAIN_WKUP    (1U)
/* Value for indicating SoC MCU domain */
#define BOARD_SOC_DOMAIN_MCU     (2U)
/* Maximum I2C instance number common across the domain */
#define BOARD_I2C_DOMAIN_INSTANCE_MAX     (2U)

/* SoC domain used by UART module */
#if (__ARM_ARCH == 7) && (__ARM_ARCH_PROFILE == 'R') && defined(__ARM_FEATURE_IDIV)
#define BOARD_UART_SOC_DOMAIN    (BOARD_SOC_DOMAIN_MCU)
#else
#define BOARD_UART_SOC_DOMAIN    (BOARD_SOC_DOMAIN_MAIN)
#endif

/* Clock frequency for UART module */
#define BOARD_UART_CLK_MAIN      (48000000U)
#define BOARD_UART_CLK_WKUP      (96000000U)

#define BOARD_MMC_VOLTAGESWITCH_FN      Board_mmc_voltageSwitchFxn

/* Below macros are retained for compatibility with other K3 platforms.
   Need to review and remove them */
/* PRG0_ETH1_LED_LINK */
#define BOARD_GPIO_ICSS0_EMAC_PHY0_LED_LINK_PORT_NUM    (0)
#define BOARD_GPIO_ICSS0_EMAC_PHY0_LED_LINK_PIN_NUM     (0)

/* PRG0_ETH2_LED_LINK */
#define BOARD_GPIO_ICSS0_EMAC_PHY1_LED_LINK_PORT_NUM    (0)
#define BOARD_GPIO_ICSS0_EMAC_PHY1_LED_LINK_PIN_NUM     (0)

/* GPIO to drive PRG0 LED0 */
#define BOARD_GPIO_ICSS0_EMAC_LED0_PORT_NUM             (0)
#define BOARD_GPIO_ICSS0_EMAC_LED0_PIN_NUM              (0)

/* GPIO to drive PRG0 LED1 */
#define BOARD_GPIO_ICSS0_EMAC_LED1_PORT_NUM             (0)
#define BOARD_GPIO_ICSS0_EMAC_LED1_PIN_NUM              (0)

/* GPIO to drive PRG0 LED2 */
#define BOARD_GPIO_ICSS0_EMAC_LED2_PORT_NUM             (0)
#define BOARD_GPIO_ICSS0_EMAC_LED2_PIN_NUM              (0)

/* GPIO to drive PRG0 LED3 */
#define BOARD_GPIO_ICSS0_EMAC_LED3_PORT_NUM             (0)
#define BOARD_GPIO_ICSS0_EMAC_LED3_PIN_NUM              (0)

/* PRG1_ETH1_LED_LINK */
#define BOARD_GPIO_ICSS1_EMAC_PHY0_LED_LINK_PORT_NUM    (0)
#define BOARD_GPIO_ICSS1_EMAC_PHY0_LED_LINK_PIN_NUM     (0)

/* PRG1_ETH2_LED_LINK */
#define BOARD_GPIO_ICSS1_EMAC_PHY1_LED_LINK_PORT_NUM    (0)
#define BOARD_GPIO_ICSS1_EMAC_PHY1_LED_LINK_PIN_NUM     (0)

/* GPIO to drive PRG1 LED0 */
#define BOARD_GPIO_ICSS1_EMAC_LED0_PORT_NUM             (0)
#define BOARD_GPIO_ICSS1_EMAC_LED0_PIN_NUM              (0)

/* GPIO to drive PRG1 LED1 */
#define BOARD_GPIO_ICSS1_EMAC_LED1_PORT_NUM             (0)
#define BOARD_GPIO_ICSS1_EMAC_LED1_PIN_NUM              (0)

/* GPIO to drive PRG1 LED2 */
#define BOARD_GPIO_ICSS1_EMAC_LED2_PORT_NUM             (0)
#define BOARD_GPIO_ICSS1_EMAC_LED2_PIN_NUM              (0)

/* GPIO to drive PRG1 LED3 */
#define BOARD_GPIO_ICSS1_EMAC_LED3_PORT_NUM             (0)
#define BOARD_GPIO_ICSS1_EMAC_LED3_PIN_NUM              (0)

/* GPIO port & pin numbers for  MMC reset */
#define GPIO_MMC_SDCD_PORT_NUM                          (0)
#define GPIO_MMC_SDCD_PIN_NUM                           (0)

#define BOARD_I2C_TOUCH_INSTANCE                        (0)
#define BOARD_I2C_TOUCH_SLAVE_ADDR                      (0)

#define BOARD_I2C_FPD_UB926_INSTANCE                    (3U)
#define BOARD_I2C_FPD_UB926_ADDR                        (0x2CU)

/* I2C instance Board Presence Circuit */
#define BOARD_PRES_WKUP_I2C_INSTANCE                    (0U)
/* I2C address Board Presence Circuit */
#define BOARD_PRES_DETECT_SLAVE_ADDR                    (0)

/* McSPI instance for master and slave test */
#define BOARD_MCSPI_MASTER_INSTANCE                     (1)
#define BOARD_MCSPI_SLAVE_INSTANCE                      (1)

/* LIN uart instance */
#define BOARD_LIN1_UART_INSTANCE                         (6U)
#define BOARD_LIN1_UART_BASE                             (CSL_UART6_BASE)

#define BOARD_LIN2_UART_INSTANCE                         (3U)
#define BOARD_LIN2_UART_BASE                             (CSL_UART3_BASE)

#define BOARD_LIN3_UART_INSTANCE                         (9U)
#define BOARD_LIN3_UART_BASE                             (CSL_UART9_BASE)

/* Default pinmux configuration of UART Tx pin used by ROM/SBL */
#define BOARD_MCU_UART_TX_PINMUX_VAL                    (PIN_MODE(0U) | ((PIN_PULL_DISABLE) & \
                                                         (~PIN_PULL_DIRECTION & ~PIN_INPUT_ENABLE)))
#define BOARD_MCU_UART_TX_PINMUX_ADDR                   (BOARD_WKUP_PMUX_CTRL_ADDR + PIN_WKUP_GPIO0_12)
#define BOARD_MCU_UART_TX_LOCK_KICK_ADDR                (CSL_WKUP_CTRL_MMR0_CFG0_BASE + \
                                                         CSL_WKUP_CTRL_MMR_CFG0_LOCK7_KICK0)


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif   /* BOARD_CFG_H_ */

