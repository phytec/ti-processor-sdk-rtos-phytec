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
 *  \file     FlsApp.c
 *
 *  \brief    This file contains the Fls Test App
 *
 */
#include "Std_Types.h"
#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
#include <ti/board/board.h>
#include <ti/board/board_cfg.h>
#endif
#include "stdio.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "EcuM_Cbk.h"
#include "app_utils.h"
#include "Fee.h"


#include "string.h"
#include "stdio.h"
#include "stdarg.h"
#include "stdlib.h"
#include "Det.h"
#include "Dem.h"
#include "Os.h"
#include "Fls_Cfg.h"
#include "Fls.h"
#include "Fls_Ospi.h"
#include "FlsApp_Startup.h"
#include "SchM_Fls.h"
#include "EcuM_Cbk.h"
#include "Fls_Cbk.h"

#include <hw_include/arch/lld_arch.h>
#include <hw_include/soc.h>
#include <hw_include/lldr.h>
#include <hw_include/lld_types.h>
#include "app_utils.h"

/* Interrupt Registrations */
#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
#include <ti/osal/osal.h>
#include <ti/drv/udma/udma.h>
#endif

/**********************************************************************
 ************************** Macros ************************************
 **********************************************************************/
#define APP_NAME                        "FLS_APP_INTR"
#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
#define PERF_TEST /* enable flag to log performance data*/
#endif 

/**********************************************************************
 ************************** Global Variables **************************
 **********************************************************************/

volatile uint32 Fls_JobDoneSuccess;
volatile uint32 Fls_JobDoneError;
volatile uint8 testPass = TRUE;
uint8 expectFail = FALSE;

#define DATA_SIZE_TEST 0x100000U
#define DATA_VALUE 0xABU
#define DATA_ERASE 0xFFU
uint32 offset = 0x00U;
uint8 m_flag = 1;

/* Buffer containing the known data that needs to be written to flash */
uint8 txBuf_test[DATA_SIZE_TEST];
/* Buffer containing the received data */
uint8 rxBuf_test[DATA_SIZE_TEST];

#ifdef PERF_TEST
       uint64          startTime; /* start time stamp in usec */
       uint64          elapsedTime; /* elapsed time in usec */
       float            xferRate;
       uint32          xferRateInt;
#endif

/**********************************************************************
 ************************** Function Declarations **************************
 **********************************************************************/

void main_handling(uint8 useIntr)
{
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
            APP_NAME ": Job Processing in Progress.\n");

    while (1U)
    {
        if (useIntr == FALSE)
            Fls_MainFunction();
            
        if ( Fls_JobDoneSuccess == 1U)
        {
            AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                    APP_NAME ": Job Ends: SUCCESS\n");
            if (expectFail == TRUE)
            {
                AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                    APP_NAME ": However job should've failed\n");
                testPass = FALSE;
                expectFail = FALSE;
            }
            break;
        }
        if ( Fls_JobDoneError == 1U)
        {
            AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                    APP_NAME ": Job Ends: ERROR\n");
            if (expectFail == FALSE)
                testPass = FALSE;
            else
                expectFail = FALSE;
            break;
        }
    }

    Fls_JobDoneSuccess = 0U;
    Fls_JobDoneError = 0U;
    return;
}

void main_cancel_handling()
{
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
            APP_NAME ": Cancel job Processing in Progress.\n");
#if ( STD_ON == FLS_CANCEL_API)
    Fls_Cancel();
#endif

    while (1U)
    {
        Fls_MainFunction();

      //Only error Notification for Cancelled Job
        if ( Fls_JobDoneError == 1U)
        {  
            AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                    APP_NAME ": Error is expected as we are cancelling the operation. \n");
            AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                    APP_NAME ": Job Ends: ERROR\n");            
            break;
        }
    }

    Fls_JobDoneError = 0U;
    Fls_JobDoneSuccess = 0U;
    return;
}

void run_erase(uint32 ofs, uint32 size, uint8 yesCancel)
{
    while (1U)
    {
        Std_ReturnType job_accepted = Fls_Erase(ofs, size);
        if (E_OK == job_accepted)
            break;
    }
    if (yesCancel == TRUE)
        main_cancel_handling();
    else
        main_handling(FALSE);
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL," \n");
}


void run_read(uint32 ofs, uint32 size, uint8 *rxBuf, uint8 useIntr, uint8 yesCancel)
{
    while (1U)
    {
        Std_ReturnType job_accepted = Fls_Read(ofs, &rxBuf[0], size);
        if (E_OK == job_accepted)
            break;
    }
#ifdef PERF_TEST
    /* Get start time stamp for the write performance measurement */
    startTime = TimerP_getTimeInUsecs();
#endif
    if (yesCancel == TRUE)
        main_cancel_handling();
    else
        main_handling(useIntr);
#ifdef PERF_TEST
    elapsedTime = TimerP_getTimeInUsecs() - startTime;
    /* calculate the write transfer rate in Kbps */
    xferRate = (float) (((float) (DATA_SIZE_TEST * 8)) / elapsedTime) * 1000U;
    xferRateInt = (uint32)xferRate;
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
            APP_NAME ": \n Read %d bytes at transfer rate %d Kbps \n",  DATA_SIZE_TEST, xferRateInt);
#endif
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL," \n");
}


void run_write(uint32 ofs, uint32 size, uint8 *txBuf, uint8 useIntr, uint8 yesCancel)
{
    while (1U)
    {
        Std_ReturnType job_accepted = Fls_Write(ofs, &txBuf[0], size);
        if (E_OK == job_accepted)
            break;
    }
    #ifdef PERF_TEST
        /* Get start time stamp for the write performance measurement */
        startTime = TimerP_getTimeInUsecs();
    #endif
        if (yesCancel == TRUE)
            main_cancel_handling();
        else
            main_handling(useIntr);
    #ifdef PERF_TEST
        elapsedTime = TimerP_getTimeInUsecs() - startTime;
        /* calculate the write transfer rate in Kbps */
        xferRate = (float) (((float) (DATA_SIZE_TEST * 8)) / elapsedTime) * 1000U;
        xferRateInt = (uint32)xferRate;
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                APP_NAME ": \n Write %d bytes at transfer rate %d Kbps \n",  DATA_SIZE_TEST, xferRateInt);
    #endif
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL," \n");
}

#if ( STD_ON == FLS_COMPARE_API)
void run_compare(uint32 ofs, uint32 size, uint8 *txBuf, uint8 yesCancel)
{
    while (1U)
    {
        Std_ReturnType job_accepted = Fls_Compare(ofs, &txBuf[0], size);
        if (E_OK == job_accepted)
            break;
    }
    if (yesCancel == TRUE)
            main_cancel_handling();
        else
            main_handling(FALSE);
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL," \n");
}
#endif 


#if ( STD_ON == FLS_BLANK_CHECK_API)
void run_blankCheck(uint32 ofs, uint32 size)
{
    while (1U)
    {
        Std_ReturnType job_accepted = Fls_BlankCheck(ofs, size);
        if (E_OK == job_accepted)
            break;
    }
    main_handling(FALSE);
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL," \n");
}
#endif


void run_manualCompare(uint32 ofs, uint32 size, uint8 *rxBuf, uint8 compareData, uint8 useIntr)
{
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
            APP_NAME ": Calling Read to manually compare \n");
    run_read(ofs, size, rxBuf, useIntr, FALSE);

    for(uint32 idx = 0U; idx < DATA_SIZE_TEST; idx++)
    {   
        if (rxBuf[idx] != compareData)
        {
            AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                    APP_NAME ": Manual Comparing is Wrong at index %d\n\n", idx);
            m_flag = 0;
            testPass = FALSE;
            break;
        }
    }
    if (m_flag)
    {
         AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                APP_NAME ": Manual Comparing is Correct\n\n");
    }
    m_flag = 1;
}


void spi_test_init()
{
#if defined (FLS_OSPI) || defined (FLS_XSPI)
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
        APP_NAME ": OSPI0 controller will be configured for OSPI/XSPI flash\n");
#elif defined (FLS_QSPI)
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
        APP_NAME ": OSPI1 controller will be configured for QSPI flash\n");
#endif

    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
       APP_NAME ": FLS spi_test Initiating and Starting.\n");

#if (STD_ON == FLS_USE_INTERRUPTS)
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
            APP_NAME ": Configuring Interrupt.\n");
    FlsApp_InterruptConfig();
#endif

#if (STD_ON == FLS_PRE_COMPILE_VARIANT)
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
            APP_NAME ": Variant - Pre Compile being used !!!\n");
    Fls_Init((const Fls_ConfigType *) NULL_PTR);

    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
            APP_NAME ": Configuring Clocks.\n");
    Int32 Clk_retVal = Fls_OSPI_configClk(FLS_INIT_CONFIG_PC.ospiClkSpeed);
    if (Clk_retVal != LLD_PASS)
    {
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                APP_NAME ": Configuring Clocks : Failed.\n");
        testPass = FALSE;
    }
    else
    {
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                APP_NAME ": Clock Configured at %dHz\n", FLS_INIT_CONFIG_PC.ospiClkSpeed);
    }
#else
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
            APP_NAME ": Variant - Post Build being used !!!\n");
    Fls_Init(&FlsConfigSet);

    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
            APP_NAME ": Configuring Clocks.\n");
    Int32 Clk_retVal = Fls_OSPI_configClk(FlsConfigSet.ospiClkSpeed);
    if (Clk_retVal != LLD_PASS)
    {
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                APP_NAME ": Configuring Clocks : Failed.\n");
        testPass = FALSE;
    }
    else
    {
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                APP_NAME ": Clock Configured at %dHz\n", FlsConfigSet.ospiClkSpeed);
    }
#endif

#if (STD_ON == FLS_VERSION_INFO_API)
    Std_VersionInfoType versioninfo;

    /* Get and print version */
    Fls_GetVersionInfo(&versioninfo);
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL," \n");
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"FLS MCAL Version Info\n");
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"---------------------\n");
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"Vendor ID           : %d\n", versioninfo.vendorID);
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"Module ID           : %d\n", versioninfo.moduleID);
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"SW Major Version    : %d\n", versioninfo.sw_major_version);
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"SW Minor Version    : %d\n", versioninfo.sw_minor_version);
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,"SW Patch Version    : %d\n", versioninfo.sw_patch_version);
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL," \n");
#endif  /* #if (FLS_VERSION_INFO_API == STD_ON) */

/* FLS driver should be free now - check */
#if ( STD_ON == FLS_GET_STATUS_API)
    MemIf_StatusType      status;
    status = Fls_GetStatus();
    if (status != MEMIF_IDLE)
    {
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
        APP_NAME ": FLS driver is not IDLE!!\n");
        testPass = FALSE;
    }
#endif

}


/*
 *  ======== test function ========
 */
void spi_test()
{

    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
            APP_NAME ": DATA SIZE TEST is 0x%X \n", DATA_SIZE_TEST);
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
            APP_NAME ": Offset is 0x%X \n\n", offset);

    volatile uint32 idx;
    volatile uint8 *txPtr_test = txBuf_test;
    volatile uint8 *rxPtr_test = rxBuf_test;


    for(idx = 0U; idx < DATA_SIZE_TEST; idx++)
    {
        *txPtr_test++ = DATA_VALUE;
        *rxPtr_test++ = (uint8)0U;
    }

    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
           APP_NAME ": Erasing \n");
    run_erase(offset, DATA_SIZE_TEST, FALSE);

#if (STD_ON == FLS_USE_INTERRUPTS)
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
            APP_NAME ": Manual comparing erased data\n\n");
    run_manualCompare(offset, DATA_SIZE_TEST, rxBuf_test, DATA_ERASE, TRUE);

#if ( STD_ON == FLS_CANCEL_API)
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
            APP_NAME ": Cancel while Writing \n");
    run_write(offset, DATA_SIZE_TEST, txBuf_test, FALSE, TRUE);

    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
            APP_NAME ": Cancel while Reading \n");
    run_read(offset, DATA_SIZE_TEST, rxBuf_test, FALSE, TRUE);
#endif //Cancel

    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
           APP_NAME ": Erasing \n");
    run_erase(offset, DATA_SIZE_TEST, FALSE);

    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
            APP_NAME ": Manual comparing erased data\n\n");
    run_manualCompare(offset, DATA_SIZE_TEST, rxBuf_test, DATA_ERASE, TRUE);

    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
            APP_NAME ": Writing Again without Cancel\n");
    run_write(offset, DATA_SIZE_TEST, txBuf_test, TRUE, FALSE);

    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
            APP_NAME ": Reading without cancel \n");
    run_read(offset, DATA_SIZE_TEST, rxBuf_test, TRUE, FALSE);

    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
            APP_NAME ": Manual Comparing written data \n\n");
    run_manualCompare(offset, DATA_SIZE_TEST, rxBuf_test, DATA_VALUE, TRUE);

#endif /*STD_ON == FLS_USE_INTERRUPTS*/

    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
        APP_NAME ": \n Changing mode to DAC \n");
    Fls_SwitchMode(TRUE, FALSE);

   /*ALL TESTS DONE*/
   AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
           APP_NAME ": FLS SPI TEST DONE! \n\n");

   return;
}

/*
*  ======== main ========
*/
int main(void)
{
   /* Call board init functions */
#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
    Board_initCfg boardCfg;
    boardCfg = BOARD_INIT_PLL | BOARD_INIT_PINMUX_CONFIG |
    BOARD_INIT_MODULE_CLOCK |
    BOARD_INIT_UART_STDIO;
    Board_init(boardCfg);
#elif defined (SOC_AM62X)  || defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
    System_init();
#endif
#ifdef UART_ENABLED
    AppUtils_Init();
#endif

#if (STD_ON == FLS_USE_INTERRUPTS)
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
           APP_NAME ": \n------------------FLS Sample Application - STARTS !!! ------------------\n");

    Fls_JobDoneSuccess = 0U;
    Fls_JobDoneError = 0U;

    spi_test_init();
    if (testPass)
    {
        spi_test();
    }
    else
    {
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL, "\n");
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL, APP_NAME ": spi_test_init() failed, skip main spi_test\n");
    }

    #if (STD_ON == OSPI_REGISTER_READBACK_API)
    CSL_ospi_flash_cfgRegs RegReadback, *RegRbPtr;
    RegRbPtr = &RegReadback;

    Std_ReturnType retVal = Ospi_RegisterReadback(RegRbPtr);

    if (retVal != E_OK)
    {
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
            APP_NAME ": OSPI configuration Register Readback Failed!!\n");
        testPass = FALSE;
    }
    else
    {
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
            ": Ospi configuration Register Readback values  \n" );
        AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
            ": CONFIG_REG 			              : 0x%x\n", RegRbPtr->CONFIG_REG);
        AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
            ": DEV_INSTR_RD_CONFIG_REG 			: 0x%x\n", RegRbPtr->DEV_INSTR_RD_CONFIG_REG);
        AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
            ": DEV_INSTR_WR_CONFIG_REG 			: 0x%x\n", RegRbPtr->DEV_INSTR_WR_CONFIG_REG);
        AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
            ": DEV_DELAY_REG 			          : 0x%x\n", RegRbPtr->DEV_DELAY_REG);
        AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
            ": DEV_SIZE_CONFIG_REG 			    : 0x%x\n", RegRbPtr->DEV_SIZE_CONFIG_REG);
        AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
            ": SRAM_PARTITION_CFG_REG 			  : 0x%x\n", RegRbPtr->SRAM_PARTITION_CFG_REG);
        AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
            ": DMA_PERIPH_CONFIG_REG 			  : 0x%x\n", RegRbPtr->DMA_PERIPH_CONFIG_REG);
        AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
            ": MODE_BIT_CONFIG_REG 			    : 0x%x\n", RegRbPtr->MODE_BIT_CONFIG_REG);
        AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
            ": TX_THRESH_REG 			          : 0x%x\n", RegRbPtr->TX_THRESH_REG);
        AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
            ": RX_THRESH_REG 			          : 0x%x\n", RegRbPtr->RX_THRESH_REG);
        AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, APP_NAME
            ": PHY_CONFIGURATION_REG 			  : 0x%x\n", RegRbPtr->PHY_CONFIGURATION_REG);

        AppUtils_Printf( APP_UTILS_PRINT_MSG_NORMAL, "\n");
    }
    #endif /* (STD_ON == OSPI_REGISTER_READBACK_API) */
    
    /*ALL TESTS DONE*/
    if (testPass)
    {
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                APP_NAME ": All tests have passed! \n");
    }
    else
    {
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                APP_NAME ": Some tests have failed :( \n");
    }

#else //(STD_OFF == FLS_USE_INTERRUPTS)
    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                APP_NAME ": Interrupt is off. Please turn it on in config to run this app \n");
#endif

    return (0);
}
