/* Copyright (c) 2021 Texas Instruments Incorporated
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
 *  \file     esm_test.c
 *
 *  \brief    This file contains ESM test code.
 *
 *  \details  ESM tests
 **/

/*===========================================================================*/
/*                         Include files                                     */
/*===========================================================================*/
#include <esm_test_main.h>
#include <osal_interface.h>
#ifdef UNITY_INCLUDE_CONFIG_H
#include <ti/build/unit-test/Unity/src/unity.h>
#include <ti/build/unit-test/config/unity_config.h>
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
static int32_t  sdlApp_initBoard(void);
/* Unity functions */
void test_sdl_esm_baremetal_test_app_runner(void);
void test_sdl_esm_baremetal_test_app (void);
extern int32_t sdl_config_test(void);
extern int32_t sdl_ccm_cb_test(void);
extern int32_t sdl_ecc_cb_test(void);

void sdlApp_print(const char * str)
{
    UART_printf(str);
}

/*===========================================================================*/
/*                         Global Variables                                  */
/*===========================================================================*/
sdlEsmTest_t  sdlEsmTestList[] = {
    {sdl_ecc_cb_test,   "ecc cb test" , SDL_APP_TEST_NOT_RUN},
    {test_sdr_test,   "callback test" , SDL_APP_TEST_NOT_RUN},
    {sdl_ccm_cb_test,   "ccm cb test" , SDL_APP_TEST_NOT_RUN},
    {sdl_config_test,   "sdl config test" , SDL_APP_TEST_NOT_RUN},
    {NULL,           "TERMINATING CONDITION",     SDL_APP_TEST_NOT_RUN }
};

int32_t SDL_ESM_applicationCallbackFunction(SDL_ESM_Inst esmInst,
                                            SDL_ESM_IntType esmIntrType,
                                            uint32_t grpChannel,
                                            uint32_t index,
                                            uint32_t intSrc,
                                            void *arg)
{
    int32_t retVal = 0;
    UART_printf("\n  ESM Call back function called : instType 0x%x, intType 0x%x, " \
                "grpChannel 0x%x, index 0x%x, intSrc 0x%x \n",
                esmInst, esmIntrType, grpChannel, index, intSrc);
    UART_printf("  Take action \n");

    /* Any additional customer specific actions can be added here */
    SDL_ESM_clrNError(esmInst);

    return retVal;
}

int32_t SDL_ESM_ECCapplicationCallbackFunction(SDL_ESM_Inst esmInst,
                                            SDL_ESM_IntType esmIntrType,
                                            uint32_t grpChannel,
                                            uint32_t index,
                                            uint32_t intSrc,
                                            void *arg)
{
    int32_t retVal = 0;
    uint32_t cb_arg = (uint32_t)arg;
    retVal = SDL_ESM_disableGlobalIntr(SDL_MCU_ESM0_CFG_BASE);
    UART_printf("\n  ESM ECC Call back function called : instType 0x%x, intType 0x%x, " \
                "grpChannel 0x%x, index 0x%x, intSrc 0x%x \n",
                esmInst, esmIntrType, grpChannel, index, intSrc);
    if (cb_arg == 0x1)
    {
        UART_printf("  This is application-registered callback\n");
    }
    else if (cb_arg == 0x2)
    {
        UART_printf("  This is ECC-registered callback");
    }

    UART_printf("  Take action \n");

    /* Any additional customer specific actions can be added here */
    SDL_ESM_clrNError(esmInst);
    retVal = SDL_ESM_disableGlobalIntr(SDL_MCU_ESM0_CFG_BASE);
    return retVal;
}

int32_t SDL_ESM_CCMapplicationCallbackFunction(SDL_ESM_Inst esmInst,
                                            SDL_ESM_IntType esmIntrType,
                                            uint32_t grpChannel,
                                            uint32_t index,
                                            uint32_t intSrc,
                                            void *arg)
{
    int32_t retVal = 0;
    uint32_t cb_arg = (uint32_t)arg;
    retVal = SDL_ESM_disableGlobalIntr(SDL_MCU_ESM0_CFG_BASE);
    UART_printf("\n  ESM CCM Call back function called : instType 0x%x, intType 0x%x, " \
                "grpChannel 0x%x, index 0x%x, intSrc 0x%x \n",
                esmInst, esmIntrType, grpChannel, index, intSrc);
    if (cb_arg == 0x1)
    {
        UART_printf("  This is application-registered callback\n");
    }
    else if (cb_arg == 0x2)
    {
        UART_printf("  This is CCM-registered callback");
    }
    UART_printf("  Take action \n");

    /* Any additional customer specific actions can be added here */
    SDL_ESM_clrNError(esmInst);
    retVal = SDL_ESM_disableGlobalIntr(SDL_MCU_ESM0_CFG_BASE);
    return retVal;
}

int32_t SDR_ESM_errorInsert (const SDL_ESM_Inst esmInstType,
                                const SDL_ESM_ErrorConfig_t *esmErrorConfig)
{
    uint32_t   esmInstBaseAddr;
    int32_t result = SDL_EFAIL;

    if (SDL_ESM_getBaseAddr(esmInstType, &esmInstBaseAddr) == ((bool)true)) {
        if (esmErrorConfig != ((void *)0u)) {
            if ((esmErrorConfig->groupNumber < SDL_ESM_MAX_EVENT_MAP_NUM_WORDS)
             && (esmErrorConfig->bitNumber < BITS_PER_WORD)) {
                /* Insert error */
                (void)SDL_ESM_setIntrStatusRAW(esmInstBaseAddr,
                                    (esmErrorConfig->groupNumber*BITS_PER_WORD)
                                    + esmErrorConfig->bitNumber);
                result = SDL_PASS;
            }
        }
    }

    return result;
}


/*===========================================================================*/
/*                   Local Function definitions                              */
/*===========================================================================*/
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



/* initialize the board for the application */
static int32_t  sdlApp_initBoard(void)
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

static int32_t sdlApp_osalInit(void)
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

void test_sdl_esm_baremetal_test_app (void)
{
    /* Declarations of variables */
    int32_t    testResult = SDL_APP_TEST_PASS;
    int32_t    i;
UART_printf("inside test_sdl_esm_baremetal_test_app \n");
    /* Init Board */
    sdlApp_initBoard();

    /* Init Osal */
    sdlApp_osalInit();

    sdlApp_print("\n ESM Test Application\r\n");

    for ( i = 0; sdlEsmTestList[i].testFunction != NULL; i++)
    {
        testResult = sdlEsmTestList[i].testFunction();
        sdlEsmTestList[i].testStatus = testResult;
    }

    testResult = SDL_APP_TEST_PASS;
    for ( i = 0; sdlEsmTestList[i].testFunction != NULL; i++)
    {
        if (sdlEsmTestList[i].testStatus != SDL_APP_TEST_PASS)
        {
            UART_printf("Test Name: %s  FAILED \n", sdlEsmTestList[i].name);
            testResult = SDL_APP_TEST_FAILED;
            break;
        }
        else
        {
            UART_printf("Test Name: %s  PASSED \n", sdlEsmTestList[i].name);
        }
    }

    if (testResult == SDL_APP_TEST_PASS)
    {
        UART_printStatus("\n All tests have passed. \n");
    }
    else
    {
        UART_printStatus("\n Few/all tests Failed \n");
    }
#if defined (UNITY_INCLUDE_CONFIG_H)
    TEST_ASSERT_EQUAL_INT32(SDL_APP_TEST_PASS, testResult);
#endif
}

void test_sdl_esm_baremetal_test_app_runner(void)
{
    /* @description:Test runner for ESM tests

       @requirements: PDK-2429

       @cores: mcu1_0 */
UART_printf("inside test_sdl_esm_baremetal_test_app_runner \n");
#if defined(UNITY_INCLUDE_CONFIG_H)
    UNITY_BEGIN();
    RUN_TEST (test_sdl_esm_baremetal_test_app);
    UNITY_END();
    /* Function to print results defined in our unity_config.h file */
    print_unityOutputBuffer_usingUARTstdio();
#else
    test_sdl_esm_baremetal_test_app();
#endif
    return;
}

int32_t main(void)
{
    test_sdl_esm_baremetal_test_app_runner();
    return 0;
}

/* Nothing past this point */
