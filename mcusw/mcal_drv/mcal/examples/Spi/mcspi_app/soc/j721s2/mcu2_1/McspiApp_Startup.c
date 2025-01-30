/*
*
* Copyright (c) 2024 Texas Instruments Incorporated
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
 *  \file     McspiApp_Startup.c
 *
 *  \brief    This file contains the pinmux, interrupt and clock configuration.
 */
/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "McspiApp_Startup.h"
#include "McspiApp.h"

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
/** \brief Start up sequence : Program the interrupt muxes / priorities */
void SpiApp_Startup(void)
{
    uint32 regVal;

    /* write Partition Lock Key 0 Register */
    CSL_REG32_WR(CSL_MCU_CTRL_MMR0_CFG0_BASE + CSL_MCU_CTRL_MMR_CFG0_LOCK1_KICK0,
        0x68EF3490U);
    /* write Partition Lock Key 1 Register */
    CSL_REG32_WR(CSL_MCU_CTRL_MMR0_CFG0_BASE + CSL_MCU_CTRL_MMR_CFG0_LOCK1_KICK1,
        0xD172BC5AU);
    /* Check for unlock */
    regVal = CSL_REG32_RD(CSL_MCU_CTRL_MMR0_CFG0_BASE + CSL_MCU_CTRL_MMR_CFG0_LOCK1_KICK0);
    while ((regVal & 0x1U) != 0x1U)
    {
        regVal = CSL_REG32_RD(CSL_MCU_CTRL_MMR0_CFG0_BASE + CSL_MCU_CTRL_MMR_CFG0_LOCK1_KICK0);
    }

    /* Unlock lock key registers for Partition 7: IO PAD
       configuration registers in MAIN_CTRL_MMR */
    /* write Partition 7 Lock Key 0 Register */
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_LOCK7_KICK0,
                0x68EF3490);
    /* write Partition 7 Lock Key 1 Register */
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_LOCK7_KICK1,
                0xD172BC5A);
    /* Check for unlock */
    regVal = CSL_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE +
        CSL_MAIN_CTRL_MMR_CFG0_LOCK7_KICK0);
    while ((regVal & 0x1) != 0x1U)
    {
		regVal = CSL_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE +
            CSL_MAIN_CTRL_MMR_CFG0_LOCK7_KICK0);
    }

    /* Enable MCU_MCSPI1 and MCSPI3 independently pin out */
    CSL_REG32_WR(CSL_MCU_CTRL_MMR0_CFG0_BASE +
        CSL_MCU_CTRL_MMR_CFG0_MCU_SPI1_CTRL,
            CSL_MCU_CTRL_MMR_CFG0_MCU_SPI1_CTRL_SPI1_LINKDIS_MASK);

    /* MCSPI 3 CS0 PAD configuration */
    regVal = CSL_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_PADCONFIG47);
    regVal = 0x40008U;
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_PADCONFIG47, regVal);

    /* MCSPI 3 CS1 PAD configuration */
    regVal = CSL_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_PADCONFIG31);
    regVal = 0x4000CU;
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_PADCONFIG31, regVal);

    /* MCSPI 3 CS2 PAD configuration */
    regVal = CSL_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_PADCONFIG34);
    regVal = 0x40008U;
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_PADCONFIG34, regVal);

    /* MCSPI 3 CS3 PAD configuration */
    regVal = CSL_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_PADCONFIG46);
    regVal = 0x4000CU;
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_PADCONFIG46, regVal);

    /* MCSPI 3 D1 PAD configuration */
    regVal = CSL_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_PADCONFIG20);
    regVal = 0x40008U;
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_PADCONFIG20, regVal);

    /* MCSPI 3 D0 PAD configuration */
    regVal = CSL_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_PADCONFIG19);
    regVal = 0x40008U;
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_PADCONFIG19, regVal);

    /* MCSPI 3 CLK PAD configuration */
    regVal = CSL_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_PADCONFIG48);
    regVal = 0x40008U;
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + CSL_MAIN_CTRL_MMR_CFG0_PADCONFIG48, regVal);

    /* Initialize profiling API's */
    AppUtils_ProfileInit (0U);

    /* Initialize memory sections  */
	AppUtils_SpiSectionInit();

    /* Interrup Registrations */
    SpiApp_InterruptConfig();

    return;
}


/** \brief Interrupt registrations*/
void SpiApp_InterruptConfig(void)
{
    OsalRegisterIntrParams_t    intrPrms;
    OsalInterruptRetCode_e      osalRetVal;
    uint32 idx;
    HwiP_Handle hwiHandle;
    const Spi_ConfigType *cfgPtr;

    /* Init */
#if (STD_ON == SPI_PRE_COMPILE_VARIANT)
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
         ": Variant - Pre Compile being used !!!\n");
	cfgPtr = &SPI_INIT_CONFIG_PC;
    Spi_Init((const Spi_ConfigType *) NULL_PTR);
#else
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
         ": Variant - Post Build being used !!!\n");
    cfgPtr = &SpiDriver;
    Spi_Init(cfgPtr);
#endif

    Intc_Init();    /* Interrupt handler initialized, here as other functions
                     * can use API's to clear pending interrupts if any
                     */

    /* We will have to use the routed interrupt number
        subsequently */
    /* Set the destination interrupt */
    for (idx = 0U; idx < cfgPtr->maxHwUnit; idx++)
    {
        if (cfgPtr->hwUnitCfg[idx].enabledmaMode == FALSE)
        {
            Osal_RegisterInterrupt_initParams(&intrPrms);
        #if (STD_ON == SPI_UNIT_MCSPI3_ACTIVE)
        #if (SPI_ISR_TYPE == SPI_ISR_CAT1 || SPI_ISR_TYPE == SPI_ISR_VOID)
            intrPrms.corepacConfig.arg          = (uintptr_t)Spi_IrqUnitMcspi3TxRx;
        #endif
        #endif
            intrPrms.corepacConfig.isrRoutine   = &SpiApp_SpiXIsr;
            intrPrms.corepacConfig.priority     = 1U;
            intrPrms.corepacConfig.corepacEventNum = 0U; /* NOT USED ? */
            intrPrms.corepacConfig.intVecNum        = APP_SPI_3_INT;

            osalRetVal = Osal_RegisterInterrupt(&intrPrms, &hwiHandle);
            if(OSAL_INT_SUCCESS != osalRetVal)
            {
                AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                                  ": Error %d !!!\n");
            }
        }
#if (STD_ON == SPI_DMA_ENABLE)
        else
        {
            /* DMA Tx Channel Interrupt */
            Osal_RegisterInterrupt_initParams(&intrPrms);
        #if (STD_ON == SPI_UNIT_MCSPI3_ACTIVE)
        #if (SPI_ISR_TYPE == SPI_ISR_CAT1 || SPI_ISR_TYPE == SPI_ISR_VOID)
            intrPrms.corepacConfig.arg          = (uintptr_t)Spi_IrqUnitMcspi3DmaTx;
        #endif
        #endif
            intrPrms.corepacConfig.isrRoutine   = &SpiApp_SpiXIsrTx;
            intrPrms.corepacConfig.priority     = 1U;
            intrPrms.corepacConfig.corepacEventNum = 0U; /* NOT USED ? */
            intrPrms.corepacConfig.intVecNum        = cfgPtr->hwUnitCfg[idx].dmaTxChIntrNum;

            osalRetVal = Osal_RegisterInterrupt(&intrPrms, &hwiHandle);
            if(OSAL_INT_SUCCESS != osalRetVal)
            {
                AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                                  ": Error %d !!!\n");
            }

            /* DMA Rx Channel Interrupt */
            Osal_RegisterInterrupt_initParams(&intrPrms);
        #if (STD_ON == SPI_UNIT_MCSPI3_ACTIVE)
        #if (SPI_ISR_TYPE == SPI_ISR_CAT1 || SPI_ISR_TYPE == SPI_ISR_VOID)
            intrPrms.corepacConfig.arg          = (uintptr_t)Spi_IrqUnitMcspi3DmaRx;
        #endif
        #endif
            intrPrms.corepacConfig.isrRoutine   = &SpiApp_SpiXIsrTx;
            intrPrms.corepacConfig.priority     = 1U;
            intrPrms.corepacConfig.corepacEventNum = 0U; /* NOT USED ? */
            intrPrms.corepacConfig.intVecNum        = cfgPtr->hwUnitCfg[idx].dmaRxChIntrNum;

            osalRetVal = Osal_RegisterInterrupt(&intrPrms, &hwiHandle);
            if(OSAL_INT_SUCCESS != osalRetVal)
            {
                AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                                  ": Error %d !!!\n");
            }


        }
#endif
    }
    return;
}

/** \brief None, SBL/GEL powers up the timers and clock sources */
void SpiApp_PowerAndClkSrc(void)
{
    /* Mcu module, when included will replace this operation */
    return;
}

#define SPI_START_SEC_ISR_CODE
#include "Spi_MemMap.h"

FUNC(void, SPI_CODE_FAST) SpiApp_SpiXIsr(uintptr_t SpiPtr)
{
    SpiApp_IsrType spiChIsr = (SpiApp_IsrType)SpiPtr;

    /* Associated Spi ISR */
    spiChIsr();
}

#if (STD_ON == SPI_DMA_ENABLE)
FUNC(void, SPI_CODE_FAST) SpiApp_SpiXIsrTx(uintptr_t SpiPtr)
{
    SpiApp_IsrType spiChIsr = (SpiApp_IsrType)SpiPtr;

    /* Associated Spi ISR */
    spiChIsr();
}

FUNC(void, SPI_CODE_FAST) SpiApp_SpiXIsrRx(uintptr_t SpiPtr)
{
    SpiApp_IsrType spiChIsr = (SpiApp_IsrType)SpiPtr;

    /* Associated Spi ISR */
    spiChIsr();
}
#endif

#define SPI_STOP_SEC_ISR_CODE
#include "Spi_MemMap.h"

