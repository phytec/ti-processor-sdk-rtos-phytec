/* ======================================================================
 *   Copyright (C) 2024 Texas Instruments Incorporated
 *
 *   All rights reserved. Property of Texas Instruments Incorporated.
 *   Restricted rights to use, duplicate or disclose this code are
 *   granted through contract.
 *
 *   The program may not be used without the written permission
 *   of Texas Instruments Incorporated or against the terms and conditions
 *   stipulated in the agreement under which this program has been
 *   supplied.
 * ==================================================================== */
/**
 *  \file     PwmApp_Startup.h
 *
 *  \brief    PWM example header file
 *
 */

#ifndef PWMAPP_STARTUP_H_
#define PWMAPP_STARTUP_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "Pwm.h"
#include "Pwm_Irq.h"

#if (PWM_INDEX == 0U) /*EPWM*/
#include <ti/drv/i2c/I2C.h>
#include <ti/drv/i2c/soc/I2C_soc.h>
#endif /*EPWM*/

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#define PWM_CHANNEL              (PWM_CHANNEL2)

#define I2C0_SCL_PAD_OFFSET_ADDR (0x1C0E0U)
#define I2C0_SDA_PAD_OFFSET_ADDR (0x1C0E4U)

/* GPIO Expander-2 slave address of TCA6424ARGJR */
#define GPIO_EXPANDER2_TCA6424ARGJR_ADDR 0x22
#define TCA6424_CMD_AUTO_INC    ((uint8) 0x80U)
#define TCA6424_REG_INPUT0      ((uint8) 0x00U)
#define TCA6424_REG_INPUT1      ((uint8) 0x01U)
#define TCA6424_REG_INPUT2      ((uint8) 0x02U)
#define TCA6424_REG_OUTPUT0     ((uint8) 0x04U)
#define TCA6424_REG_OUTPUT1     ((uint8) 0x05U)
#define TCA6424_REG_OUTPUT2     ((uint8) 0x06U)
#define TCA6424_REG_CONFIG0     ((uint8) 0x0CU)
#define TCA6424_REG_CONFIG1     ((uint8) 0x0DU)
#define TCA6424_REG_CONFIG2     ((uint8) 0x0EU)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

void PwmApp_PowerAndClkSrc(void);

#if (PWM_INDEX == 0U) /*EPWM*/
void TCA6424_Init(void);
void SetupI2CTransfer(I2C_Handle handle,  uint32 slaveAddr,
                      uint8 *writeData, uint32 numWriteBytes,
                      uint8 *readData,  uint32 numReadBytes);
#endif /*EPWM*/

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef PWMAPP_STARTUP_H_ */
