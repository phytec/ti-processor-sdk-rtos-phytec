/*
 * Copyright (C) 2023 Texas Instruments Incorporated - http://www.ti.com/
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of Texas Instruments Incorporated nor the names of
 * its contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**
 *  \file   board_power.c
 *
 *  \brief  Implements functions to control the power on the board.
 *
 */

#include <ti/csl/cslr_device.h>

#include <ti/drv/i2c/I2C.h>
#include <ti/drv/i2c/soc/I2C_soc.h>
#include <ti/osal/osal.h>
#include <ti/board/board.h>
#include <ti/board/src/devices/common/common.h>
#include "board_internal.h"
#include "board_power.h"
#include "board_utils.h"

/**
 *  \brief    Function to power off the PMIC.
 *
 *  \param slaveAddr [IN] PMIC slave address
 *
 *  \return   BOARD_SOK in case of success or appropriate error code
 *
 */
Board_STATUS Board_pmPowerOff(uint32_t slaveAddr)
{
    I2C_Handle handle = NULL;
    Board_STATUS retVal = -1;
    uint8_t regData;

    handle = Board_getI2CHandle(BOARD_SOC_DOMAIN_WKUP,
                                BOARD_I2C_PMIC_INSTANCE);
    if(handle == NULL)
    {
        return (retVal);
    }

    /* Write voltage */
    regData = (TPS6594X_FSM_NSLEEP_NSLEEP1B | TPS6594X_FSM_NSLEEP_NSLEEP2B);
    retVal = Board_i2c8BitRegWr(handle,
                                slaveAddr,
                                TPS6594X_FSM_NSLEEP_TRIGGERS,
                                &regData,
                                1U,
                                I2C_WAIT_FOREVER);
    if(retVal != 0)
    {
        return (retVal);
    }

    BOARD_delay(10000);
    
    regData = (TPS6594X_INT_STARTUP_NPWRON_START_INT |
               TPS6594X_INT_STARTUP_ENABLE_INT       |
               TPS6594X_INT_STARTUP_RTC_INT          |
               TPS6594X_INT_STARTUP_SOFT_REBOOT_INT);
    retVal = Board_i2c8BitRegWr(handle,
                                slaveAddr,
                                TPS6594X_INT_STARTUP,
                                &regData,
                                1U,
                                I2C_WAIT_FOREVER);
    if(retVal != 0)
    {
        return (retVal);
    }

    BOARD_delay(10000);

    regData = (TPS6594X_INT_MISC_BIST_PASS_INT |
               TPS6594X_INT_MISC_EXT_CLK_INT   |
               TPS6594X_INT_MISC_TWARN_INT);
    retVal = Board_i2c8BitRegWr(handle,
                                slaveAddr,
                                TPS6594X_INT_MISC,
                                &regData,
                                1U,
                                I2C_WAIT_FOREVER);
    if(retVal != 0)
    {
        return (retVal);
    }

    BOARD_delay(10000);

    regData = (TPS6594X_CONFIG_NSLEEP1_MASK | TPS6594X_CONFIG_NSLEEP2_MASK);
    retVal = Board_i2c8BitRegWr(handle,
                                slaveAddr,
                                TPS6594X_CONFIG_1,
                                &regData,
                                1U,
                                I2C_WAIT_FOREVER);
    if(retVal != 0)
    {
        return (retVal);
    }

    BOARD_delay(10000);

    regData = (TPS6594X_FSM_I2C_TRIGGERS_I2C0);
    retVal = Board_i2c8BitRegWr(handle,
                                slaveAddr,
                                TPS6594X_FSM_I2C_TRIGGERS,
                                &regData,
                                1U,
                                I2C_WAIT_FOREVER);
    if(retVal != 0)
    {
        return (retVal);
    }

    return retVal;
}
