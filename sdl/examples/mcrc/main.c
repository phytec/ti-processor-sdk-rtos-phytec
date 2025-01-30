/* Copyright (c) 2022 Texas Instruments Incorporated
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
 *  \file     main.c
 *
 *  \brief    This file contains mcrc example code.
 *
 *  \details  MCRC app
 **/

/*===========================================================================*/
/*                         Include files                                     */
/*===========================================================================*/
#include "main.h"
#include <osal_interface.h>

#ifdef UNITY_INCLUDE_CONFIG_H
#include <ti/build/unit-test/Unity/src/unity.h>
#include <ti/build/unit-test/config/unity_config.h>
#include <ti/osal/CacheP.h>
#if defined (SOC_J721E)
#include <ti/drv/udma/soc/j721e/udma_soc.h>
#elif defined (SOC_J7200)
#include <ti/drv/udma/soc/j7200/udma_soc.h>
#elif defined (SOC_J721S2)
#include <ti/drv/udma/soc/j721s2/udma_soc.h>
#elif defined (SOC_J784S4)
#include <ti/drv/udma/soc/j784s4/udma_soc.h>
#endif
#endif

/*===========================================================================*/
/*                         Declarations                                      */
/*===========================================================================*/
/* None */

/*===========================================================================*/
/*                         Macros                                            */
/*===========================================================================*/
/* None */

/*===========================================================================*/
/*                         Internal function declarations                    */
/*===========================================================================*/

int32_t  sdlApp_initBoard(void);
/* Unity functions */
void test_sdl_mcrc_baremetal_test_app_runner(void);
void test_sdl_mcrc_baremetal_test_app (void);

/*===========================================================================*/
/*                         Global Variables                                  */
/*===========================================================================*/

sdlMCRCApp_t  sdlmcrcAppList[] = {
    {sdl_mcrcFullCPU_main,      "MCRC_fullCPU_mode",     SDL_APP_NOT_RUN },
    {sdl_mcrcSemiCPU_main,      "MCRC_semiCPU_mode" ,    SDL_APP_NOT_RUN },
    {NULL,                      "TERMINATING CONDITION",     SDL_APP_NOT_RUN }
};

/*===========================================================================*/
/*                   Local Function definitions                              */
/*===========================================================================*/

uint64_t Udma_appVirtToPhyFxn(const void *virtAddr, uint32_t chNum, void *appData)
{
    uint64_t    phyAddr;
    uint64_t    atcmSizeLocal = 0U;
    uint64_t    atcmBaseGlobal = 0U;

    phyAddr = (uint64_t) virtAddr;

#if defined (SOC_J721E)  || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
#if defined (BUILD_MCU1_0)
#if defined (SOC_J721E)
    atcmSizeLocal = CSL_MCU_ARMSS_ATCM_SIZE;
#else
    atcmSizeLocal = CSL_MCU_R5FSS0_ATCM_SIZE;
#endif
    atcmBaseGlobal = CSL_MCU_R5FSS0_CORE0_ATCM_BASE;
#endif
#if defined (BUILD_MCU1_1)
#if defined (SOC_J721E)
    atcmSizeLocal = CSL_MCU_ARMSS_ATCM_SIZE;
#else
    atcmSizeLocal = CSL_MCU_R5FSS0_ATCM_SIZE;
#endif
    atcmBaseGlobal = CSL_MCU_R5FSS0_CORE1_ATCM_BASE;
#endif
#if defined (BUILD_MCU2_0)
#if defined (SOC_J721E)
    atcmSizeLocal = CSL_ARMSS_ATCM_BASE;
#else
    atcmSizeLocal = CSL_R5FSS0_ATCM_SIZE;
#endif
    atcmBaseGlobal = CSL_R5FSS0_CORE0_ATCM_BASE;
#endif
#if defined (BUILD_MCU2_1)
#if defined (SOC_J721E)
    atcmSizeLocal = CSL_ARMSS_ATCM_BASE;
#else
    atcmSizeLocal = CSL_R5FSS0_ATCM_SIZE;
#endif
    atcmBaseGlobal = CSL_R5FSS0_CORE1_ATCM_BASE;
#endif
#if defined (BUILD_MCU3_0)
#if defined (SOC_J721E)
    atcmSizeLocal = CSL_ARMSS_ATCM_BASE;
#else
    atcmSizeLocal = CSL_R5FSS0_ATCM_SIZE;
#endif
    atcmBaseGlobal = CSL_R5FSS1_CORE0_ATCM_BASE;
#endif
#if defined (BUILD_MCU3_1)
#if defined (SOC_J721E)
    atcmSizeLocal = CSL_ARMSS_ATCM_BASE;
#else
    atcmSizeLocal = CSL_R5FSS0_ATCM_SIZE;
#endif
    atcmBaseGlobal = CSL_R5FSS1_CORE1_ATCM_BASE;
#endif
#endif

     /* check for start address avoided since atcmBase is 0U */
    if(phyAddr < atcmSizeLocal)
    {
        phyAddr += atcmBaseGlobal;
    }

    return (phyAddr);
}

void *Udma_appPhyToVirtFxn(uint64_t phyAddr, uint32_t chNum, void *appData)
{
    void       *virtAddr;

#if defined (__aarch64__) || defined (__C7100__)
    virtAddr = (void *) phyAddr;
#else
    uint32_t temp;
    uint64_t    atcmBaseGlobal = 0U;
    uint64_t    atcmSizeGlobal = 0U;

#if defined (SOC_J721E)
#if defined (BUILD_MCU1_0)
    atcmSizeGlobal = CSL_MCU_R5FSS0_CORE0_ATCM_SIZE;
    atcmBaseGlobal = CSL_MCU_R5FSS0_CORE0_ATCM_BASE;
#endif
#if defined (BUILD_MCU1_1)
    atcmSizeGlobal = CSL_MCU_R5FSS0_CORE1_ATCM_SIZE;
    atcmBaseGlobal = CSL_MCU_R5FSS0_CORE1_ATCM_BASE;
#endif
#if defined (BUILD_MCU2_0)
    atcmSizeGlobal = CSL_R5FSS0_CORE0_ATCM_SIZE;
    atcmBaseGlobal = CSL_R5FSS0_CORE0_ATCM_BASE;
#endif
#if defined (BUILD_MCU2_1)
    atcmSizeGlobal = CSL_R5FSS0_CORE1_ATCM_SIZE;
    atcmBaseGlobal = CSL_R5FSS0_CORE1_ATCM_BASE;
#endif
#if defined (BUILD_MCU3_0)
    atcmSizeGlobal = CSL_R5FSS1_CORE0_ATCM_SIZE;
    atcmBaseGlobal = CSL_R5FSS1_CORE0_ATCM_BASE;
#endif
#if defined (BUILD_MCU3_1)
    atcmSizeGlobal = CSL_R5FSS1_CORE1_ATCM_SIZE;
    atcmBaseGlobal = CSL_R5FSS1_CORE1_ATCM_BASE;
#endif
#endif

    if((phyAddr >= atcmBaseGlobal) &&
       (phyAddr < (atcmBaseGlobal + atcmSizeGlobal)))
    {
        phyAddr -= atcmBaseGlobal;
         /*start address not added since atcmBase is 0U */
    }

    /* R5 is 32-bit; need to truncate to avoid void * typecast error */
    temp = (uint32_t) phyAddr;
    virtAddr = (void *) temp;
#endif

    return (virtAddr);
}

void Udma_appUtilsCacheWb(const void *addr, int32_t size)
{
    uint32_t    isCacheCoherent = Udma_isCacheCoherent();

    if(isCacheCoherent != TRUE)
    {
        CacheP_wb(addr, size);
    }

    return;
}

void Udma_appUtilsCacheInv(const void * addr, int32_t size)
{
    uint32_t    isCacheCoherent = Udma_isCacheCoherent();

    if(isCacheCoherent != TRUE)
    {
        CacheP_Inv(addr, size);
    }

    return;
}

#ifdef UNITY_INCLUDE_CONFIG_H
/*
 *  ======== Unity set up and tear down ========
 */
void setUp(void)
{
    /* Do nothing */
}

void tearDown(void)
{
    /* Do nothing */
}
#endif

void sdlApp_print(const char * str)
{
    UART_printf(str);
}

void sdlApp_printArg(uint32_t value)
{
   UART_printf("0x%x \n", value);
}

/* initialize the board for the application */
int32_t  sdlApp_initBoard(void)
{
    Board_initCfg boardCfg;
    Board_STATUS  boardStatus;

    boardCfg = BOARD_INIT_PINMUX_CONFIG |
               BOARD_INIT_UART_STDIO;
    boardStatus = Board_init(boardCfg);
    if (boardStatus != BOARD_SOK)
    {
        sdlApp_print("[Error] Board init failed!!\n");
    }
    return (boardStatus);
}

int32_t sdlApp_osalInit(void)
{
    SDL_ErrType_t ret = SDL_PASS;

    ret = SDL_TEST_osalInit();
    if (ret != SDL_PASS)
    {
        UART_printf("Error: Init Failed\n");
    }

    return ret;
}

/*===========================================================================*/
/*                         Function definitions                              */
/*===========================================================================*/

void test_sdl_mcrc_baremetal_test_app (void)
{
    /* Declarations of variables */
    int32_t    result = SDL_APP_PASS;
    int32_t    i;

    /* Init Board */
    sdlApp_initBoard();

    /* Init Osal */
    sdlApp_osalInit();

    sdlApp_print("\n MCRC Application\r\n");

    for ( i = 0; sdlmcrcAppList[i].application != NULL; i++)
    {
        result = sdlmcrcAppList[i].application();
        sdlmcrcAppList[i].status = result;
    }

    result = SDL_APP_PASS;
    for ( i = 0; sdlmcrcAppList[i].application != NULL; i++)
    {
        if (sdlmcrcAppList[i].status != SDL_APP_PASS)
        {
            UART_printf("Applications Name: %s  FAILED \n", sdlmcrcAppList[i].name);
            result = SDL_APP_FAILED;
            break;
        }
        else
        {
            UART_printf("Applications Name: %s  PASSED \n", sdlmcrcAppList[i].name);
        }
    }

    if (result == SDL_APP_PASS)
    {
        UART_printStatus("\n All tests have passed. \n");
    }
    else
    {
        UART_printStatus("\n Few/all applications Failed \n");
    }

#if defined (UNITY_INCLUDE_CONFIG_H)
    TEST_ASSERT_EQUAL_INT32(SDL_APP_PASS, result);
#endif
}

void test_sdl_mcrc_baremetal_test_app_runner(void)
{
    /* @description:Test runner for MCRC applications */

#if defined(UNITY_INCLUDE_CONFIG_H)
    UNITY_BEGIN();
    RUN_TEST (test_sdl_mcrc_baremetal_test_app);
    UNITY_END();
    /* Function to print results defined in our unity_config.h file */
    print_unityOutputBuffer_usingUARTstdio();
#else
    test_sdl_mcrc_baremetal_test_app();
#endif
    return;
}

int32_t main(void)
{
    test_sdl_mcrc_baremetal_test_app_runner();

    /* Stop the application and wait here */
    while (1);
}
/* Nothing past this point */
