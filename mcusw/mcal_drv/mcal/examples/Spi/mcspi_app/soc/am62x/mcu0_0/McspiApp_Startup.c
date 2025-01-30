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
static void SpiAppStartup_BuildIntList(void);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
#if (STD_ON == SPI_DMA_ENABLE)
uint32 SpiAppStartup_IntNumbersTx[SPI_MAX_HW_DMA_UNIT];
SpiApp_IsrType SpiAppStartup_IsrTx[SPI_MAX_HW_DMA_UNIT];
uint32 SpiAppStartup_IntNumbersRx[SPI_MAX_HW_DMA_UNIT];
SpiApp_IsrType SpiAppStartup_IsrRx[SPI_MAX_HW_DMA_UNIT];
#endif
uint32 SpiAppStartup_IntNumbers[APP_SPI_TOTAL_COUNT];
SpiApp_IsrType SpiAppStartup_Isr[APP_SPI_TOTAL_COUNT];

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
/** \brief Start up sequence : Program the interrupt muxes / priorities */
void SpiApp_Startup(void)
{
	System_init();

   
    /* Initialize profiling API's */
    AppUtils_ProfileInit (0U);

    /* Initialize memory sections  */
	AppUtils_SpiSectionInit();

    /* Build Interrupt List */
    SpiAppStartup_BuildIntList();

    /* Interrup Registrations */
    SpiApp_InterruptConfig();

    return;
}

/** \brief Interrupt registrations*/
void SpiApp_InterruptConfig(void)
{

    const Spi_ConfigType *cfgPtr;
    uint32 idx;
	
	int32_t status = SystemP_SUCCESS;
    HwiP_Params    hwiPrms;
    static HwiP_Object SpiHwiObject;

    /* Init */
#if (STD_ON == SPI_PRE_COMPILE_VARIANT)
    cfgPtr = &SPI_INIT_CONFIG_PC;
#else
    cfgPtr = &SpiDriver;
#endif

    Intc_Init();    /* Interrupt handler initialized, here as other functions
                     * can use API's to clear pending interrupts if any
                     */
    for (idx = 0U; idx < cfgPtr->maxHwUnit; idx++)
    {
        if (cfgPtr->hwUnitCfg[idx].enabledmaMode == FALSE)
        {
            HwiP_Params_init(&hwiPrms);
#if (STD_ON == SPI_UNIT_MCU_MCSPI0_ACTIVE) || (STD_ON == SPI_UNIT_MCU_MCSPI1_ACTIVE ) || (STD_ON == SPI_UNIT_MCSPI0_ACTIVE ) || (STD_ON == SPI_UNIT_MCSPI1_ACTIVE )|| (STD_ON == SPI_UNIT_MCSPI2_ACTIVE )
#if (SPI_ISR_TYPE == SPI_ISR_CAT1 || SPI_ISR_TYPE == SPI_ISR_VOID)
           hwiPrms.args = SpiAppStartup_Isr[idx];
#endif
#endif                           
       hwiPrms.priority = 1U;
       hwiPrms.eventId = 0U; /* NOT USED ? */
       hwiPrms.callback = &SpiApp_SpiXIsr;
       hwiPrms.intNum = SpiAppStartup_IntNumbers[idx];
       status = HwiP_construct(&SpiHwiObject, &hwiPrms);
       if(SystemP_SUCCESS != status)
		{
			AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
				"Error Could not register SPI ISR !!!\n");
		}
        }
#if (STD_ON == SPI_DMA_ENABLE)
        else
        {			
		HwiP_Params_init(&hwiPrms);
		hwiPrms.args          = SpiAppStartup_IsrTx[idx];
		hwiPrms.priority     = 1U;
		hwiPrms.eventId = 0U; /* NOT USED ? */
		hwiPrms.callback = &SpiApp_SpiXIsrTx;
		hwiPrms.intNum = SpiAppStartup_IntNumbersTx[idx];;
		status = HwiP_construct(&SpiHwiObject, &hwiPrms);
		if(SystemP_SUCCESS != status)
		{
			AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
				"Error Could not register SPI TX DMA ISR !!!\n");
		}

            /* RX DMA completion interrupt */
        if (SystemP_SUCCESS == status)
        {				
			HwiP_Params_init(&hwiPrms);
			hwiPrms.args          = SpiAppStartup_IsrRx[idx];
			hwiPrms.priority     = 1U;
			hwiPrms.eventId = 0U; /* NOT USED ? */
			hwiPrms.callback = &SpiApp_SpiXIsrTx;
			hwiPrms.intNum = SpiAppStartup_IntNumbersRx[idx];;
			status = HwiP_construct(&SpiHwiObject, &hwiPrms);
	    if(SystemP_SUCCESS != status)
	    {
			AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
				"Error Could not register SPI RX DMA ISR !!!\n");
		}
        }
        }
#endif
    }

    return;
}

/** \brief None, SBL/GEL powers up the mcspi's and clock sources */
void SpiApp_PowerAndClkSrc(void)
{
    /* Mcu module, when included will replace this operation */
    return;
}

/** \brief Determines the used mcspi's and builds a list of valid mcspi's & ISR */
static void SpiAppStartup_BuildIntList(void)
{
    uint32 idx;
    const Spi_ConfigType *cfgPtr;
#if (STD_ON == SPI_DMA_ENABLE)
    uint32 dmaEnabledHwCnt = (uint32)SPI_MAX_HW_DMA_UNIT;
#endif

    /*
     * 1. Determine the number of mcspi's used
     * 2. Build valid mcspi's list
     * 3. Build isr and interrupt number for enabled mcspi's only
     */

     AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL, APP_NAME ": Building Interrupt List !!!\n");
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

    for (idx = 0U; idx < APP_SPI_TOTAL_COUNT; idx++)
    {
        SpiAppStartup_IntNumbers[idx] = 0U;
    }
#if (STD_ON == SPI_DMA_ENABLE)
    for (idx = 0U; idx < dmaEnabledHwCnt; idx++)
    {
        SpiAppStartup_IntNumbersTx[idx] = 0U;
        SpiAppStartup_IntNumbersRx[idx] = 0U;
    }
#endif

    for (idx = 0U; idx < cfgPtr->maxHwUnit; idx++)
    {
        switch(cfgPtr->hwUnitCfg[idx].hwUnitId)
        {
            case SPI_UNIT_MCU_MCSPI0:
                if (cfgPtr->hwUnitCfg[idx].enabledmaMode == FALSE)
                {
                    SpiAppStartup_IntNumbers[idx] = APP_SPI_MCU_0_INT;
                    SpiAppStartup_Isr[idx] = Spi_IrqUnitMcuMcspi0TxRx;
                }
#if (STD_ON == SPI_DMA_ENABLE)
                else
                {
                    SpiAppStartup_IntNumbersTx[idx] = cfgPtr->hwUnitCfg[idx].dmaTxChIntrNum;
                    SpiAppStartup_IntNumbersRx[idx] = cfgPtr->hwUnitCfg[idx].dmaRxChIntrNum;
                    SpiAppStartup_IsrTx[idx] = Spi_IrqUnitMcuMcspi0DmaTx;
                    SpiAppStartup_IsrRx[idx] = Spi_IrqUnitMcuMcspi0DmaRx;
                }
#endif
            break;
            case SPI_UNIT_MCU_MCSPI1:
                if (cfgPtr->hwUnitCfg[idx].enabledmaMode == FALSE)
                {
                     SpiAppStartup_IntNumbers[idx] = APP_SPI_MCU_1_INT;
					SpiAppStartup_Isr[idx] = Spi_IrqUnitMcuMcspi1TxRx;
                }
#if (STD_ON == SPI_DMA_ENABLE)
                else
                {
                    SpiAppStartup_IntNumbersTx[idx] = cfgPtr->hwUnitCfg[idx].dmaTxChIntrNum;
                    SpiAppStartup_IntNumbersRx[idx] = cfgPtr->hwUnitCfg[idx].dmaRxChIntrNum;
                    SpiAppStartup_IsrTx[idx] = Spi_IrqUnitMcuMcspi1DmaTx;
                    SpiAppStartup_IsrRx[idx] = Spi_IrqUnitMcuMcspi1DmaRx;
                }
#endif
            break;
            case SPI_UNIT_MCSPI0:
                if (cfgPtr->hwUnitCfg[idx].enabledmaMode == FALSE)
                {
                    SpiAppStartup_IntNumbers[idx] = APP_SPI_0_INT;
                    SpiAppStartup_Isr[idx] = Spi_IrqUnitMcspi0TxRx;
                }
#if (STD_ON == SPI_DMA_ENABLE)
                else
                {
                    SpiAppStartup_IntNumbersTx[idx] = cfgPtr->hwUnitCfg[idx].dmaTxChIntrNum;
                    SpiAppStartup_IntNumbersRx[idx] = cfgPtr->hwUnitCfg[idx].dmaRxChIntrNum;
                    SpiAppStartup_IsrTx[idx] = Spi_IrqUnitMcuMcspi0DmaTx;
                    SpiAppStartup_IsrRx[idx] = Spi_IrqUnitMcuMcspi0DmaRx;
                }
#endif
            break;
                case SPI_UNIT_MCSPI1:
                if (cfgPtr->hwUnitCfg[idx].enabledmaMode == FALSE)
                {
                    SpiAppStartup_IntNumbers[idx] = APP_SPI_1_INT;
                    SpiAppStartup_Isr[idx] = Spi_IrqUnitMcspi1TxRx;
                }
    #if (STD_ON == SPI_DMA_ENABLE)
                else
                {
                    SpiAppStartup_IntNumbersTx[idx] = cfgPtr->hwUnitCfg[idx].dmaTxChIntrNum;
                    SpiAppStartup_IntNumbersRx[idx] = cfgPtr->hwUnitCfg[idx].dmaRxChIntrNum;
                    SpiAppStartup_IsrTx[idx] = Spi_IrqUnitMcuMcspi0DmaTx;
                    SpiAppStartup_IsrRx[idx] = Spi_IrqUnitMcuMcspi0DmaRx;
                }
    #endif
            break;
                case SPI_UNIT_MCSPI2:
                if (cfgPtr->hwUnitCfg[idx].enabledmaMode == FALSE)
                {
                    SpiAppStartup_IntNumbers[idx] = APP_SPI_2_INT;
                    SpiAppStartup_Isr[idx] = Spi_IrqUnitMcspi2TxRx;
                }
    #if (STD_ON == SPI_DMA_ENABLE)
                else
                {
                    SpiAppStartup_IntNumbersTx[idx] = cfgPtr->hwUnitCfg[idx].dmaTxChIntrNum;
                    SpiAppStartup_IntNumbersRx[idx] = cfgPtr->hwUnitCfg[idx].dmaRxChIntrNum;
                    SpiAppStartup_IsrTx[idx] = Spi_IrqUnitMcuMcspi0DmaTx;
                    SpiAppStartup_IsrRx[idx] = Spi_IrqUnitMcuMcspi0DmaRx;
                }
    #endif
            break;
            default:
                AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL, APP_NAME ": ERROR Interrupt registration is not done for mcspi's ID %d !!!\n", cfgPtr->hwUnitCfg[idx].hwUnitId);
            break;
        }
    }

    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL, APP_NAME ": Interrupt List Completed !!!\n");

    return;
}

#define SPI_START_SEC_ISR_CODE
#include "Spi_MemMap.h"

FUNC(void, SPI_CODE_FAST) SpiApp_SpiXIsr(void * SpiPtr)
{
    SpiApp_IsrType spiChIsr = (SpiApp_IsrType)SpiPtr;

    /* Associated Spi ISR */
    spiChIsr();
}

#if (STD_ON == SPI_DMA_ENABLE)
SPI_ISR_TEXT_SECTION FUNC(void, SPI_CODE_FAST) SpiApp_SpiXIsrTx(void * SpiPtr)
{
    SpiApp_IsrType spiChIsr = (SpiApp_IsrType)SpiPtr;

    /* Associated Spi ISR */
    spiChIsr();
}

SPI_ISR_TEXT_SECTION FUNC(void, SPI_CODE_FAST) SpiApp_SpiXIsrRx(void * SpiPtr)
{
    SpiApp_IsrType spiChIsr = (SpiApp_IsrType)SpiPtr;

    /* Associated Spi ISR */
    spiChIsr();
}
#endif

#define SPI_STOP_SEC_ISR_CODE
#include "Spi_MemMap.h"

