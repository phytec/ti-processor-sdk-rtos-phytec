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
 *  \file     sdl_pok_example.c
 *
 *  \brief    This file contains POK example code.
 *
 *  \details  POK example
 **/

/*===========================================================================*/
/*                         Include files                                     */
/*===========================================================================*/
#include "main.h"

/*===========================================================================*/
/*                         Macros                                            */
/*===========================================================================*/
/* None */

/* Global variables */


/*===========================================================================*/
/*                         Internal function declarations                    */
/*===========================================================================*/
int32_t        SDL_POK_setConfig(SDL_POK_Inst instance, SDL_POK_config *pPokCfg);
int32_t        sdlPOKInPor_func(void);
int32_t        sdlPOK_func(void);
int32_t        SDL_ESM_applicationCallbackFunction(SDL_ESM_Inst esmInstType,
                                                   SDL_ESM_IntType esmIntType,
                                                   uint32_t grpChannel,
                                                   uint32_t index,
                                                   uint32_t intSrc,
                                                   void *arg);
volatile bool ESM_Error = false;
uint32_t deactivate_trigger(uint32_t *esm_err_sig );
static void sdlGetInstance(SDL_POK_Inst *instance, uint32_t *esm_err_sig);

/*===========================================================================*/
/*                         Function definitions                              */
/*===========================================================================*/
int32_t SDL_ESM_applicationCallbackFunction(SDL_ESM_Inst esmInst,
                                            SDL_ESM_IntType esmIntrType,
                                            uint32_t grpChannel,
                                            uint32_t index,
                                            uint32_t intSrc,
                                            void *arg)
{
    int32_t retVal = SDL_PASS;
    UART_printf("\n  ESM Call back function called : instType 0x%x, intType 0x%x, " \
                "grpChannel 0x%x, index 0x%x, intSrc 0x%x \n",
                esmInst, esmIntrType, grpChannel, index, intSrc);
    UART_printf("  Take action \n");
    /* Disable the ESM Interrupt */
    deactivate_trigger(&intSrc);
    SDL_ESM_clrNError(esmInst);
    ESM_Error = true;
    /* Any additional customer specific actions can be added here */

    return retVal;
}

uint32_t deactivate_trigger(uint32_t *esm_err_sig )
{
    SDL_POK_Inst               instance;
    SDL_POK_config             pPokCfg;
    SDL_pokVal_t               pPokVal;
    SDL_wkupCtrlRegsBase_t    *pBaseAddr = (SDL_wkupCtrlRegsBase_t *) SDL_POK_MMR_BASE;
    int32_t                    sdlRet = SDL_EFAIL;

    sdlGetInstance(&instance, esm_err_sig);

    pPokCfg.hystCtrl = SDL_PWRSS_HYSTERESIS_NO_ACTION;
    pPokCfg.voltDetMode = SDL_PWRSS_GET_VOLTAGE_DET_MODE;
    pPokCfg.trim = SDL_PWRSS_TRIM_NO_ACTION;
    pPokCfg.detectionCtrl = SDL_POK_DETECTION_NO_ACTION;
    pPokCfg.pokEnSelSrcCtrl = SDL_POK_ENSEL_NO_ACTION;
#if defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
    pPokCfg.hystCtrlOV = SDL_PWRSS_HYSTERESIS_NO_ACTION;
    pPokCfg.trimOV = SDL_PWRSS_TRIM_NO_ACTION;
    pPokCfg.deglitch = SDL_PWRSS_DEGLITCH_NO_ACTION;
#endif

    SDL_pokGetControl (pBaseAddr,&pPokCfg,&pPokVal,instance);
    /* Re-configure to "good" setting */
    if (pPokVal.voltDetMode == SDL_PWRSS_SET_UNDER_VOLTAGE_DET_ENABLE)
    {
        pPokCfg.trim = 0;
#if defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
        pPokCfg.trimOV = SDL_PWRSS_TRIM_NO_ACTION;
#endif
    }
    else
    {
#if defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
        pPokCfg.trimOV = 45;
        pPokCfg.trim = SDL_PWRSS_TRIM_NO_ACTION;
#else
        pPokCfg.trim = 45;
#endif
    }

    pPokCfg.hystCtrl = SDL_PWRSS_HYSTERESIS_NO_ACTION;
    pPokCfg.voltDetMode = pPokVal.voltDetMode;
    pPokCfg.detectionCtrl = SDL_POK_DETECTION_NO_ACTION;
    pPokCfg.pokEnSelSrcCtrl = SDL_POK_ENSEL_NO_ACTION;
#if defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
    pPokCfg.hystCtrlOV = SDL_PWRSS_HYSTERESIS_NO_ACTION;
    pPokCfg.deglitch = SDL_PWRSS_DEGLITCH_NO_ACTION;
#endif
    sdlRet = SDL_POK_init(instance,&pPokCfg);

    return sdlRet;
}

int32_t SDL_POK_setConfig(SDL_POK_Inst instance, SDL_POK_config *pPokCfg)
{
    int32_t sdlRet = SDL_EFAIL;
    sdlRet = SDL_POK_init(instance, pPokCfg);
    if (sdlRet != SDL_PASS)
    {
        UART_printf("SDL_POK_init failed! \n");
    }
    else
    {
        volatile int32_t i = 0;
        UART_printf("Waiting for ESM to report the error \n");
        /* Wait for the ESM interrupt to report the error */
        do {
            i++;
            if (i > 0x0FFFFFFF)
            {
                /* Timeout for the wait */
                break;
            }
        } while (ESM_Error == false);

        if (ESM_Error == true)
        {
            UART_printf(" Got the ESM Error Interrupt \n");
            UART_printf("Action taken \n");
            ESM_Error = false;
            if (sdlRet != SDL_PASS)
            {
                UART_printf("SDL_POK_init failed! \n");
            }
        }
        else
        {
            sdlRet = SDL_EFAIL;
        }
    }
    return(sdlRet);
}


int32_t sdlPOKInPor_func(void)
{
    int32_t                      testStatus, sdlRet = SDL_PASS, overallStatus = SDL_APP_TEST_PASS;
    SDL_POK_config               pPokCfg;
    SDL_POK_Inst                 instance;

#if defined (SOC_J721E)
    UART_printf(" \n\n Below are the POK In POR ID values for the example\n");
    UART_printf("  SDL_POR_POKHV_UV_ID is:           14 \n");
    UART_printf("  SDL_POR_POKLV_UV_ID is:           15 \n");
    UART_printf("  SDL_POR_POKHV_OV_ID is:           16 \n");

    instance = SDL_POR_POKHV_OV_ID;
#else
    instance = SDL_POR_POKLVA_OV_ID;
#endif

    UART_printf ("\n\n POK ID = %d , monitoring set to OV \n", instance);
    pPokCfg.voltDetMode = SDL_PWRSS_SET_OVER_VOLTAGE_DET_ENABLE;
    pPokCfg.trim = 0;
    pPokCfg.hystCtrl = SDL_PWRSS_HYSTERESIS_NO_ACTION;
    pPokCfg.detectionCtrl = SDL_POK_DETECTION_NO_ACTION;
    pPokCfg.pokEnSelSrcCtrl = SDL_POK_ENSEL_NO_ACTION;
#if defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
    pPokCfg.hystCtrlOV = SDL_PWRSS_HYSTERESIS_NO_ACTION;
    pPokCfg.trimOV = 0;
    pPokCfg.deglitch = SDL_PWRSS_DEGLITCH_NO_ACTION;
#endif

    sdlRet = SDL_POK_setConfig(instance, &pPokCfg);
    if (sdlRet == SDL_PASS)
    {
        testStatus = SDL_APP_TEST_PASS;
    }
    else
    {
        testStatus = SDL_APP_TEST_FAILED;
        overallStatus = SDL_APP_TEST_FAILED;
    }
    UART_printf("Safety software Example UC-2 pok for instance  %d %s\n",
                instance, (testStatus == SDL_APP_TEST_PASS) ? "PASSED" : "FAILED");

    return (overallStatus);
}

int32_t sdlPOK_func(void)
{
    int32_t                      testStatus, sdlRet = SDL_PASS, overallStatus = SDL_APP_TEST_PASS;
    SDL_POK_config               pPokCfg;
    SDL_POK_Inst                 instance;

#if defined (SOC_J721E)
    UART_printf(" \n \n Below are the POK ID values \n");
    UART_printf("  SDL_POK_VDDA_PMIC_IN_ID is:       0 \n");
    UART_printf("  SDL_POK_VDD_CORE_UV_ID is:        1 \n");
    UART_printf("  SDL_POK_VDDSHV_WKUP_GEN_UV_ID is: 2 \n");
    UART_printf("  SDL_POK_VDD_CPU_UV_ID is:         3 \n");
    UART_printf("  SDL_POK_VDDR_MCU_UV_ID is:        4 \n");
    UART_printf("  SDL_POK_VMON_EXT_UV_ID is:        5 \n");
    UART_printf("  SDL_POK_VDD_MCU_OV_ID is:         6 \n");
    UART_printf("  SDL_POK_VDDR_CORE_UV_ID is:       7 \n");
    UART_printf("  SDL_POK_VDDSHV_WKUP_GEN_OV_ID is: 8 \n");
    UART_printf("  SDL_POK_VDD_CORE_OV_ID is:        9 \n");
    UART_printf("  SDL_POK_VDDR_MCU_OV_ID is:        10 \n");
    UART_printf("  SDL_POK_VDD_CPU_OV_ID is:         11 \n");
    UART_printf("  SDL_POK_VDDR_CORE_OV_ID is:       12 \n");
    UART_printf("  SDL_POK_VMON_EXT_OV_ID is:        13 \n\n");

    instance = SDL_POK_VDD_CORE_UV_ID;
#else
    instance = SDL_POK_VDD_CORE_ID;
#endif

    UART_printf ("\n\n POK ID = %d , monitoring set to UV \n", instance);
    pPokCfg.voltDetMode = SDL_PWRSS_SET_UNDER_VOLTAGE_DET_ENABLE;
    pPokCfg.trim = 127;
    pPokCfg.hystCtrl = SDL_PWRSS_HYSTERESIS_NO_ACTION;
    pPokCfg.detectionCtrl = SDL_POK_DETECTION_NO_ACTION;
    pPokCfg.pokEnSelSrcCtrl = SDL_POK_ENSEL_NO_ACTION;
#if defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
    pPokCfg.hystCtrlOV = SDL_PWRSS_HYSTERESIS_NO_ACTION;
    pPokCfg.trimOV = SDL_PWRSS_TRIM_NO_ACTION;
    pPokCfg.deglitch = SDL_PWRSS_DEGLITCH_NO_ACTION;
#endif

    sdlRet = SDL_POK_setConfig(instance, &pPokCfg);
    if (sdlRet == SDL_PASS)
    {
        testStatus = SDL_APP_TEST_PASS;
    }
    else
    {
        testStatus = SDL_APP_TEST_FAILED;
        overallStatus = SDL_APP_TEST_FAILED;
    }
    UART_printf("Safety software Example UC-1 pok for instance %d %s\n",
                instance, (testStatus == SDL_APP_TEST_PASS) ? "PASSED" : "FAILED");

    return (overallStatus);
}

#if defined (SOC_J721E)
static void sdlGetInstance(SDL_POK_Inst *instance, uint32_t *esm_err_sig)
{
    switch (*esm_err_sig)
    {
        case ESM_ERR_SIG_POKHV_UV:
            *instance    = SDL_POR_POKHV_UV_ID;
            break;
        case ESM_ERR_SIG_POKHV_OV:
            *instance    = SDL_POR_POKHV_OV_ID;
            break;
        case ESM_ERR_SIG_POKLV_UV:
            *instance    = SDL_POR_POKLV_UV_ID;
            break;
        case ESM_ERR_SIG_VDDA_IN:
            *instance    = SDL_POK_VDDA_PMIC_IN_ID;
            break;
        case ESM_ERR_SIG_VDD_CORE_UV:
            *instance    = SDL_POK_VDD_CORE_UV_ID;
            break;
        case ESM_ERR_SIG_VDDSHV_WKUP_GEN_UV:
            *instance = SDL_POK_VDDSHV_WKUP_GEN_UV_ID;
            break;
        case ESM_ERR_SIG_VDD_CPU_UV:
            *instance = SDL_POK_VDD_CPU_UV_ID;
            break;
        case ESM_ERR_SIG_VDDR_MCU_UV:
            *instance = SDL_POK_VDDR_MCU_UV_ID;
            break;
        case ESM_ERR_SIG_VMON_EXT_UV:
            *instance = SDL_POK_VMON_EXT_UV_ID;
            break;
        case ESM_ERR_SIG_VDD_MCU_OV:
            *instance = SDL_POK_VDD_MCU_OV_ID;
            break;
        case ESM_ERR_SIG_VDDR_CORE_UV:
            *instance = SDL_POK_VDDR_CORE_UV_ID;
            break;
        case ESM_ERR_SIG_VDDSHV_WKUP_GEN_OV:
            *instance = SDL_POK_VDDSHV_WKUP_GEN_OV_ID;
            break;
        case ESM_ERR_SIG_VDD_CORE_OV:
            *instance = SDL_POK_VDD_CORE_OV_ID;
            break;
        case ESM_ERR_SIG_VDDR_MCU_OV:
            *instance = SDL_POK_VDDR_MCU_OV_ID;
            break;
        case ESM_ERR_SIG_VDD_CPU_OV:
            *instance = SDL_POK_VDD_CPU_OV_ID;
            break;
        case ESM_ERR_SIG_VDDR_CORE_OV:
            *instance = SDL_POK_VDDR_CORE_OV_ID;
            break;
        case ESM_ERR_SIG_VMON_EXT_OV:
        default:
            *instance = SDL_POK_VMON_EXT_OV_ID;
            break;
    }
    return;
}
#else
static void sdlGetInstance(SDL_POK_Inst *instance, uint32_t *esm_err_sig)
{
    switch (*esm_err_sig)
    {
        case WKUP_ESM_ERR_SIG_VDD_CORE_UV:
        case WKUP_ESM_ERR_SIG_VDD_CORE_OV:
            *instance    = SDL_POK_VDD_CORE_ID;
            break;
        case WKUP_ESM_ERR_SIG_VDDR_CORE_UV:
        case WKUP_ESM_ERR_SIG_VDDR_CORE_OV:
            *instance    = SDL_POK_VDDR_CORE_ID;
            break;
        case WKUP_ESM_ERR_SIG_VDD_CPU_UV:
        case WKUP_ESM_ERR_SIG_VDD_CPU_OV:
            *instance    = SDL_POK_VDD_CPU_ID;
            break;
        case WKUP_ESM_ERR_SIG_VMON_EXT_UV:
        case WKUP_ESM_ERR_SIG_VMON_EXT_OV:
            *instance    = SDL_POK_VMON_EXT_ID;
            break;
        case WKUP_ESM_ERR_SIG_VMON_EXT_MAIN_1P8_OV:
        case WKUP_ESM_ERR_SIG_VMON_EXT_MAIN_1P8_UV:
            *instance    = SDL_POK_VMON_EXT_MAIN_1P8_ID;
            break;
        case WKUP_ESM_ERR_SIG_VMON_EXT_MAIN_3P3_OV:
        case WKUP_ESM_ERR_SIG_VMON_EXT_MAIN_3P3_UV:
            *instance = SDL_POK_VMON_EXT_MAIN_3P3_ID;
            break;
        case WKUP_ESM_ERR_SIG_VDD_MCU_OV:
            *instance = SDL_POK_VDD_MCU_OV_ID;
            break;
        case WKUP_ESM_ERR_SIG_VDD_MCU_UV:
            *instance = SDL_POR_POKLVB_UV_ID;
            break;
        case WKUP_ESM_ERR_SIG_VDDR_MCU_UV:
        case WKUP_ESM_ERR_SIG_VDDR_MCU_OV:
            *instance = SDL_POK_VDDR_MCU_ID;
            break;
        case WKUP_ESM_ERR_SIG_VDDSHV_WKUP_GEN_UV:
        case WKUP_ESM_ERR_SIG_VDDSHV_WKUP_GEN_OV:
            *instance = SDL_POK_VDDSHV_WKUP_GEN_ID;
            break;
        case WKUP_ESM_ERR_SIG_CAP_VDDS_MCU_GEN_UV:
        case WKUP_ESM_ERR_SIG_CAP_VDDS_MCU_GEN_OV:
            *instance = SDL_POK_CAP_VDDS_MCU_GEN_ID;
            break;
        case WKUP_ESM_ERR_SIG_VDDA_PMIC_IN_UV:
            *instance = SDL_POK_VDDA_PMIC_IN_ID;
            break;
        case WKUP_ESM_ERR_SIG_VDDA_MCU_UV:
            *instance = SDL_POR_POKHV_UV_ID;
            break;
        case WKUP_ESM_ERR_SIG_VDDA_MCU_OV:
            *instance = SDL_POR_POKLVA_OV_ID;
            break;
        default:
            *instance = (SDL_POK_Inst)(-1);
            break;
    }
    return;
}

#endif
/* Nothing past this point */
