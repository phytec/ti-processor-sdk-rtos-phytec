/*
*
* Copyright (C) 2024 Texas Instruments Incorporated
*
* All rights reserved not granted herein.
*
* Limited License.
*
* Texas Instruments Incorporated grants a world-wide, royalty-free, non-exclusive
* license under copyrights and patents it now or hereafter owns or controls to make,
* have made, use, import, offer to sell and sell ("Utilize") this software subject to the
* terms herein.  With respect to the foregoing patent license, such license is granted
* solely to the extent that any such patent is necessary to Utilize the software alone.
* The patent license shall not apply to any combinations which include this software,
* other than combinations with devices manufactured by or for TI ("TI Devices").
* No hardware patent is licensed hereunder.
*
* Redistributions must preserve existing copyright notices and reproduce this license
* (including the above copyright notice and the disclaimer and (if applicable) source
* code license limitations below) in the documentation and/or other materials provided
* with the distribution
*
* Redistribution and use in binary form, without modification, are permitted provided
* that the following conditions are met:
*
* *       No reverse engineering, decompilation, or disassembly of this software is
* permitted with respect to any software provided in binary form.
*
* *       any redistribution and use are licensed by TI for use only with TI Devices.
*
* *       Nothing shall obligate TI to provide you with source code for the software
* licensed and provided to you in object code.
*
* If software source code is provided to you, modification and redistribution of the
* source code are permitted provided that the following conditions are met:
*
* *       any redistribution and use of the source code, including any resulting derivative
* works, are licensed by TI for use only with TI Devices.
*
* *       any redistribution and use of any object code compiled from the source code
* and any resulting derivative works, are licensed by TI for use only with TI Devices.
*
* Neither the name of Texas Instruments Incorporated nor the names of its suppliers
*
* may be used to endorse or promote products derived from this software without
* specific prior written permission.
*
* DISCLAIMER.
*
* THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
* OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/
/**
 *  \file     PwmApp.c
 *
 *  \brief    This file contains the PWM test example
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "PwmApp_Startup.h"
#include "PwmApp.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#define MSG_NORMAL      (APP_UTILS_PRINT_MSG_NORMAL)
/**< Message type */
#define MSG_STATUS      (APP_UTILS_PRINT_MSG_STATUS)
/**< Message type */

#define APP_MAIN_CTRL_MMR_CFG0_EPWM0_CTRL_TB_CLKEN_EN (0x1U)
#define APP_MAIN_CTRL_MMR_CFG0_EPWM1_CTRL_TB_CLKEN_EN (0x1U)
#define MMR_KEY0_UNLOCK     (0x68EF3490U)
#define MMR_KEY1_UNLOCK     (0xD172BC5AU)


/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */
#if (STD_ON == PWM_NOTIFICATION_SUPPORTED)
static void PwmApp_InterruptConfig(void);
#endif

#if (PWM_INDEX == 0U) /*EPWM*/
static void Pwm_ClockEnable_MmrUnlock(void);
static void Pwm_PinMux(uint32 offset, uint32 muxmode);
static void PwmApp_unlockMmrRegisters(uint32 lockKey0, uint32 lockKey1);
#endif /*EPWM*/


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/** \brief Start up sequence : Program the interrupt muxes / priorities */
void PwmApp_Startup(void)
{
#if (STD_ON == PWM_NOTIFICATION_SUPPORTED)
        PwmApp_InterruptConfig();
#endif

#if (PWM_INDEX == 0U) /*EPWM*/
    Pwm_ClockEnable_MmrUnlock();

/*
 * J721S2: EHRPWM0_A signal
 * The ePWM pin J24-pin 42 on GESI board on J721S2 can be probed to check the output EPWM signals.
 *
 * SOM board :
 * SPI0_CS0 -> PM_I2C_SEL(CP:GPIO1_0/EQEP0_I/MLB0_MLBDAT) -> C5 Pin(J20C Connector)
 *
 * CP Board : Pin C5(J20C Connector) -> Pin 7(U137) -> Pin 5(U137) ->  Pin 7(U155) -> Pin 6(U155) -> EQEP0_I -> Pin 12(J51 Connector)
 * In order to select EQEP0_I following pre-requisite needs to be taken care as below mentioned.
 * 	a. MLB_MUX_SEL must be configured to 0 through I2C GPIO EXPANDER2 I2C0 through I2C0 instance.
 * 	b. MCAN_MUX_SEL must be configured to 1 through I2C GPIO EXPANDER2 I2C0 through I2C0 instance.
 * 	c. MUX_MC/BP_SEL must be set to 0: FROM Common Processor Board DIP SW3.8 [MUX_MC/BP_SEL] can be turned OFF.
 *
 * Gessi board :
 * Pin 12(J2 Connector) EQEP0_I --> Pin 4(U44)-> Pin 2(U44) [MC_EQEP0_I] -> Pin 42(J24 Connector) -> EHRPWM0_A signal.
 */

    Pwm_PinMux(CSL_MAIN_CTRL_MMR_CFG0_PADCONFIG51, 0x08050005); /*EHRPWM0_A*/
    Pwm_PinMux(CSL_MAIN_CTRL_MMR_CFG0_PADCONFIG25, 0x08050009); /*EHRPWM0_B*/
    
	/* Initialization of I2C0 instance and setting the I2C expander
	 * Setting MLB_MUX_SEL and MCAN_MUX_SEL through I2C expander
	 */
	TCA6424_Init();

#endif /*EPWM*/

    /* OSAL being generic might not record the correct frequency and hence we
     * are setting it explicitly. Osal_delay function uses PMU counter which
     * runs at CPU speed and uses OSAL_CPU_FREQ_KHZ_DEFAULT to get the CPU
     * frequency. By default OSAL_CPU_FREQ_KHZ_DEFAULT is set to 400MHZ.
	 * In case of J721E this parameter needs to be set explicitly to 1GHZ
     * to get the correct delay functionality.
     */
	AppUtils_SetCpuFrequency();
}

/* ========================================================================== */
/*                        Application Local Functions                         */
/* ========================================================================== */
#if (STD_ON == PWM_NOTIFICATION_SUPPORTED)
static void PwmApp_InterruptConfig(void)
{
    /*interrupt config*/
    OsalRegisterIntrParams_t    intrPrms;
    OsalInterruptRetCode_e      osalRetVal;
    HwiP_Handle hwiHandle;

#if (PWM_INDEX == 0U) /*EPWM*/
    /* We will have to use the routed interrupt number
    subsequently */
    Osal_RegisterInterrupt_initParams(&intrPrms);

    intrPrms.corepacConfig.arg          = (uint32)NULL_PTR;
    intrPrms.corepacConfig.isrRoutine   = (void (*)(uintptr_t))Pwm_Ch1Notify;
    intrPrms.corepacConfig.priority     = 1U;
    intrPrms.corepacConfig.corepacEventNum = 0U; /* NOT USED  */
    intrPrms.corepacConfig.intVecNum = CSLR_R5FSS0_CORE1_INTR_EPWM0_EPWM_ETINT_0;
    osalRetVal = Osal_RegisterInterrupt(&intrPrms, &hwiHandle);
    if(OSAL_INT_SUCCESS != osalRetVal)
    {
        AppUtils_Printf(MSG_NORMAL,APP_NAME
            ": Error in OSAL Interrupt Registration !!!\n");
    }
#endif /*EPWM*/

}
#endif


#if (PWM_INDEX == 0U)/*EPWM*/
static void Pwm_ClockEnable_MmrUnlock(void)
{
    /* write Partition 0 Register */
    PwmApp_unlockMmrRegisters(CSL_MAIN_CTRL_MMR_CFG0_LOCK0_KICK0, CSL_MAIN_CTRL_MMR_CFG0_LOCK0_KICK1);
    /* write Partition 1 Register */
    PwmApp_unlockMmrRegisters(CSL_MAIN_CTRL_MMR_CFG0_LOCK1_KICK0, CSL_MAIN_CTRL_MMR_CFG0_LOCK1_KICK1);
    /* write Partition 2 Register */
    PwmApp_unlockMmrRegisters(CSL_MAIN_CTRL_MMR_CFG0_LOCK2_KICK0, CSL_MAIN_CTRL_MMR_CFG0_LOCK2_KICK1);
    /* write Partition 3 Register */
    PwmApp_unlockMmrRegisters(CSL_MAIN_CTRL_MMR_CFG0_LOCK3_KICK0, CSL_MAIN_CTRL_MMR_CFG0_LOCK3_KICK1);
    /* write Partition 4 Register */
    PwmApp_unlockMmrRegisters(CSL_MAIN_CTRL_MMR_CFG0_LOCK4_KICK0, CSL_MAIN_CTRL_MMR_CFG0_LOCK4_KICK1);
    /* write Partition 6 Register */
//    PwmApp_unlockMmrRegisters(CSL_MAIN_CTRL_MMR_CFG0_LOCK6_KICK0, CSL_MAIN_CTRL_MMR_CFG0_LOCK6_KICK1);
    /* write Partition 7 Register */
    PwmApp_unlockMmrRegisters(CSL_MAIN_CTRL_MMR_CFG0_LOCK7_KICK0, CSL_MAIN_CTRL_MMR_CFG0_LOCK7_KICK1);
    /*write 0x08050006 to LOCK 7*/

    /*enable Clock*/
    HW_WR_FIELD32(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_EPWM0_CTRL,
        CSL_MAIN_CTRL_MMR_CFG0_EPWM0_CTRL_TB_CLKEN,
        APP_MAIN_CTRL_MMR_CFG0_EPWM0_CTRL_TB_CLKEN_EN);

    /*enable eallow*/
    HW_WR_FIELD32(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_EPWM0_CTRL,
        CSL_MAIN_CTRL_MMR_CFG0_EPWM0_CTRL_EALLOW,
        1U);

}

static void PwmApp_unlockMmrRegisters(uint32 lockKey0, uint32 lockKey1)
{
    uint32 regVal = 0U;
    /* write Partition Lock Key 0 Register */
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + lockKey0, MMR_KEY0_UNLOCK);
    /* write Partition Lock Key 1 Register */
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + lockKey1, MMR_KEY1_UNLOCK);
    /* Check for unlock */
    regVal = CSL_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + lockKey0);
    while ((regVal & 0x1) != 0x1U)
    {
        regVal = CSL_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + lockKey0);
    }

    return;
}

static void Pwm_PinMux(uint32 offset, uint32 muxmode)
{
    uint32 regVal = 0U;
    regVal = CSL_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + offset);
    regVal &= 0U;
    regVal |= muxmode;
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + offset, regVal);
    return;
}

/** \brief None, SBL/GEL powers up the timers and clock sources */
void PwmApp_PowerAndClkSrc(void)
{
    sint32 retVal;
    /* Mcu module, when included will replace this operation */
    retVal = AppUtils_ModuleClockEnable(TISCI_DEV_EPWM0);
     if(retVal != 0)
    {
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                          ": Error could not enable module !!!\n");
    }
    return;
}

void TCA6424_Init(void)
{
    uint32 regVal = 0U;

    I2C_Params i2cParams;
    I2C_Handle handle = NULL;
    uint8 dataToSlave[4] = {0};

    /* I2C PinMux */
    /* I2C0_SCL */
    regVal = 0x40000U;
    LLD_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + I2C0_SCL_PAD_OFFSET_ADDR, regVal);
    /* I2C0_SDA */
    regVal = 0x40000U;
    LLD_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + I2C0_SDA_PAD_OFFSET_ADDR, regVal);

    /* I2C initialization */
    I2C_init();
    I2C_Params_init(&i2cParams);
    i2cParams.transferMode = I2C_MODE_BLOCKING;
    i2cParams.bitRate = I2C_400kHz;
    i2cParams.transferCallbackFxn = NULL;

    handle = I2C_open(0U, &i2cParams);
	/* If Auto Increment is disabled, During a read operation, the same register bank is read each time.
	 * During a write operation, data is written to the same register bank each time.
	 * Example demonstrates how individual port registers can be set.
	 * dataToSlave[0] = TCA6424_REG_OUTPUT1;
	 * dataToSlave[1] &= ~(0x80);
	 * dataToSlave[2] = 0x0U;
	 * SetupI2CTransfer(handle, GPIO_EXPANDER2_TCA6424ARGJR_ADDR, &dataToSlave[0], 2, &dataToSlave[2], 1);
	 */

	/* If Auto Increment is enabled, this allows the user to
	 * program and/or read the 3 register banks sequentially.
	 */
	dataToSlave[0] = TCA6424_REG_CONFIG0 | TCA6424_CMD_AUTO_INC;
	dataToSlave[1] = 0x0U;
	dataToSlave[2] = 0x0U;
	dataToSlave[3] = 0x0U;
	SetupI2CTransfer(handle, GPIO_EXPANDER2_TCA6424ARGJR_ADDR, &dataToSlave[0], 4, NULL, 0);

	dataToSlave[0] = TCA6424_REG_INPUT0 | TCA6424_CMD_AUTO_INC;
	dataToSlave[1] = 0x0U;
	dataToSlave[2] = 0x0U;
	dataToSlave[3] = 0x0U;
	SetupI2CTransfer(handle, GPIO_EXPANDER2_TCA6424ARGJR_ADDR, &dataToSlave[0], 1, &dataToSlave[1], 3);

	/*
	 * J24 Pin 42 [MC_EQEP0_I] is output EPWM signal pin to probe on the GESI board.
	 * SPI0_CS0 [AE27][PADCONFIG_51] considered as EHRPWM0_A signal.
	 * Signal Routing SPI0_CS0 <--> PM_I2C_SEL,
	 * PM_I2C_SEL <--> CPB : GPIO1_0/EQEP0_I/MLB0_MLBDAT.
	 * In order to select EQEP0_I , MLB_MUX_SEL must be configured to 0 --> (P1.3)
	 * and MCAN_MUX_SEL must be configured to 1 --> (P1.4) of I2C GPIO EXPANDER2 respectively.
	 * This is controlled through I2C GPIO EXPANDER2 which is configured through I2C0 instance.
	 * MUX_MC/BP_SEL must be set �0�: FROM CPB DIP SW3.8 [MUX_MC/BP_SEL] must be OFF.Which routes EQEP0_I to [MC_EQEP0_I] J24 Pin 42.
	 */
	dataToSlave[0] = TCA6424_REG_OUTPUT0 | TCA6424_CMD_AUTO_INC;
	dataToSlave[1] = 0xFF;
	dataToSlave[2] &= ~(0x08);
	dataToSlave[3] = 0xFF;
	SetupI2CTransfer(handle, GPIO_EXPANDER2_TCA6424ARGJR_ADDR, &dataToSlave[0], 4, &dataToSlave[1], 3);
}

void SetupI2CTransfer(I2C_Handle handle,  uint32 slaveAddr,
                      uint8 *writeData, uint32 numWriteBytes,
                      uint8 *readData,  uint32 numReadBytes)
{
    int16_t status = 0;
    I2C_Transaction i2cTransaction;

    I2C_transactionInit(&i2cTransaction);
    i2cTransaction.slaveAddress = slaveAddr;
    i2cTransaction.writeBuf = (uint8 *)&writeData[0];
    i2cTransaction.writeCount = numWriteBytes;
    i2cTransaction.readBuf = (uint8 *)&readData[0];
    i2cTransaction.readCount = numReadBytes;
    status = I2C_transfer(handle, &i2cTransaction);
    if(status != E_NOT_OK)
    {
        AppUtils_Printf(MSG_NORMAL, "\n Data Transfer failed. \n");
    }
}
#endif /*EPWM*/
/*EoF*/
