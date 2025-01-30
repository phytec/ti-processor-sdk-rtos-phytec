#include <src/ip/sdl_esm.h>
#include <ti/drv/uart/UART.h>
#include <ti/drv/uart/UART_stdio.h>
#include <src/ip/sdl_esm.h>
#include "esm_test_main.h"
#define SDTF_NUM_RUNALL_TEST_COMMANDS    10

SDL_ESM_config ESM_esmInitConfig_WKUP =
{
    .esmErrorConfig = {0u, 8u}, /* Self test error config */
    .enableBitmap = {0xffffffffu, 0x00180003u, 0xffffffffu,
                },
     /**< All events enable: except clkstop events for unused clocks */
    .priorityBitmap = {0xfffffbffu, 0x00180003u, 0xffffffffu,
                        },
    /**< All events high priority: except clkstop events for unused clocks */
    .errorpinBitmap = {0xffffffffu, 0x00180003u, 0xffffffffu,
                      },
    /**< All events high priority: except clkstop for unused clocks
     *   and selftest error events */
};

SDL_ESM_config SDTF_esmInitConfig_MCU_appcallback =
{
    .esmErrorConfig = {0u, 3u}, /* Self test error config */
    .enableBitmap = {0x00000000u, 0xff0fffffu, 0x7fffffffu, 0x00000007u,
                },
     /**< All events enable: except timer and self test  events, and Main ESM output */
    /* Temporarily disabling vim compare error as well*/
    .priorityBitmap = {0x00000000u, 0xff0ffffeu, 0x7fffffffu, 0x00000007u,
                        },
    /**< All events high priority: except timer, selftest error events, and Main ESM output */
    .errorpinBitmap = {0x00000000u, 0xff0fffffu, 0x7fffffffu, 0x00000007u,
                      },
    /**< All events high priority: except timer, selftest error events, and Main ESM output */
};

#if defined (SOC_J721E)||(SOC_J7200)
SDL_ESM_config SDTF_esmInitConfig_MAIN_appcallback =
{
    .esmErrorConfig = {1u, 8u}, /* Self test error config */
    .enableBitmap = {0x00000000u, 0xfffffffbu, 0x7fffffffu, 0xffffffffu,
                 0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                 0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                 0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                 0xffffffffu,
                },
     /**< All events enable: except clkstop events for unused clocks
      *   and PCIE events */
    .priorityBitmap = {0x00000000u, 0xfffffffbu, 0x7fffffffu, 0x00000001u,//0x00000007u,//0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                         0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                         0xffffffffu,
                        },
    /**< All events high priority: except clkstop events for unused clocks
     *   and PCIE events */
    .errorpinBitmap = {0x00000000u, 0xfffffffbu, 0x7fffffffu, 0xffffffffu,
                       0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                       0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                       0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                       0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                       0xffffffffu,
                      },
    /**< All events high priority: except clkstop for unused clocks
     *   and PCIE events */
};
#endif

#if defined (SOC_J721S2)
SDL_ESM_config SDTF_esmInitConfig_MAIN_appcallback =
{
    .esmErrorConfig = {1u, 8u}, /* Self test error config */
    .enableBitmap = {0x00000000u, 0xffffffdbu, 0x7fffffffu, 0xffffffffu,
                 0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                 0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                 0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                 0xfffbfeffu,
                },
     /**< All events enable: except clkstop events for unused clocks
      *   and PCIE events */
    .priorityBitmap = {0x00000000u, 0xffffffdbu, 0x7fffffffu, 0x00000001u,//0x00000007u,//0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                         0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                         0xfffbfeffu,
                        },
    /**< All events high priority: except clkstop events for unused clocks
     *   and PCIE events */
    .errorpinBitmap = {0x00000000u, 0xffffffdbu, 0x7fffffffu, 0xffffffffu,
                       0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                       0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                       0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                       0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                       0xfffbfeffu,
                      },
    /**< All events high priority: except clkstop for unused clocks
     *   and PCIE events */
};
#endif

#if defined (SOC_J784S4)
SDL_ESM_config SDTF_esmInitConfig_MAIN_appcallback =
{
    .esmErrorConfig = {1u, 8u}, /* Self test error config */
    .enableBitmap = {0x00000000u, 0xffffffdbu, 0x7fffffffu, 0xffffffffu,
                 0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                 0xffffffffu, 0xfffffffbu, 0xffffffffu, 0xffffffffu,
                 0xffff7fffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                 0xfffbfeffu,
                },
     /**< All events enable: except clkstop events for unused clocks
      *   and PCIE events */
    .priorityBitmap = {0x00000000u, 0xffffffdbu, 0x7fffffffu, 0x00000001u,//0x00000007u,//0xffffffffu,
                         0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                         0xffffffffu, 0xfffffffbu, 0xffffffffu, 0xffffffffu,
                         0xffff7fffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                         0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                         0xfffbfeffu,
                        },
    /**< All events high priority: except clkstop events for unused clocks
     *   and PCIE events */
    .errorpinBitmap = {0x00000000u, 0xffffffdbu, 0x7fffffffu, 0xffffffffu,
                       0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                       0xffffffffu, 0xfffffffbu, 0xffffffffu, 0xffffffffu,
                       0xffff7fffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
                       0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
                       0xfffbfeffu,
                      },
    /**< All events high priority: except clkstop for unused clocks
     *   and PCIE events */
};
#endif


extern int32_t SDR_ESM_errorInsert (const SDL_ESM_Inst esmInstType,
                                const SDL_ESM_ErrorConfig_t *esmErrorConfig);

extern int32_t SDL_ESM_applicationCallbackFunction(SDL_ESM_Inst esmInstType,
                                         SDL_ESM_IntType esmIntType,
                                         uint32_t grpChannel,
                                         uint32_t index,
                                         uint32_t intSrc,
                                         void *arg);

static uint32_t arg;
void  esm_init_appcb(SDL_ESM_Inst esmType)
{
    void *ptr = (void *)&arg;
    SDL_ErrType_t result;
    /* Initialize MAIN ESM module */
    if(esmType == SDL_ESM_INST_WKUP_ESM0)
    result = SDL_ESM_init(esmType, &ESM_esmInitConfig_WKUP,SDL_ESM_applicationCallbackFunction,ptr);
    else if(esmType == SDL_ESM_INST_MCU_ESM0)
    result = SDL_ESM_init(esmType, &SDTF_esmInitConfig_MCU_appcallback,SDL_ESM_applicationCallbackFunction,ptr);
    else
    result = SDL_ESM_init(esmType, &SDTF_esmInitConfig_MAIN_appcallback,SDL_ESM_applicationCallbackFunction,ptr);
        if (result != SDL_PASS) {
            /* print error and quit */
            UART_printf("ESM_ECC_Example_init: Error initializing MAIN ESM: result = %d\n", result);

        } else {
            UART_printf("\nESM_ECC_Example_init: Init MAIN ESM complete \n");
        }
}

/*********************************************************************
 * @fn      SDTF_runESMInject
 *
 * @brief   Execute ESM Inject
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
static int32_t SDTF_runESMInjectInstance(SDL_ESM_Inst esmType,
                                         uint32_t groupNumber,
                                         uint32_t bitNumber)
{
    SDL_ErrType_t result;
    int32_t retVal=0;

    SDL_ESM_ErrorConfig_t esmErrorConfig;

    esmErrorConfig.groupNumber = groupNumber;
    esmErrorConfig.bitNumber = bitNumber;

    UART_printf("\n ESM inject: test starting for Esm instance %d", esmType);

    /* Run esm test 2*/
    result = SDR_ESM_errorInsert(esmType, &esmErrorConfig);

    if (result != SDL_PASS ) {
        UART_printf("\n ESM inject test for Esm instance %d failed", esmType);
        retVal = -1;
    } else {
        UART_printf("\n ESM inject test for Esm instance %d Done", esmType);

    }
    SDL_ESM_clrNError(esmType);
    return retVal;
}

/*********************************************************************
 * @fn      SDTF_runESMInjectHigh_WKUP
 *
 * @brief   Execute ESM Inject for WKUP
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t SDTF_runESMInjectHigh_WKUP(void)
{
    int32_t retVal=0;
    esm_init_appcb(SDL_ESM_INST_WKUP_ESM0);

    retVal = SDTF_runESMInjectInstance(SDL_ESM_INST_WKUP_ESM0, 0, 9);
    return retVal;
}

int32_t SDTF_runESMInjectLow_WKUP(void)
{
    int32_t retVal=0;
    esm_init_appcb(SDL_ESM_INST_WKUP_ESM0);

    retVal = SDTF_runESMInjectInstance(SDL_ESM_INST_WKUP_ESM0, 0, 10);
    return retVal;
}
/*********************************************************************
 * @fn      SDTF_runESMInjectHigh_MCU
 *
 * @brief   Execute ESM Inject for MCU
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */
int32_t SDTF_runESMInjectHigh_MCU(void)
{
    int32_t retVal=0;
    esm_init_appcb(SDL_ESM_INST_MCU_ESM0);
    retVal = SDTF_runESMInjectInstance(SDL_ESM_INST_MCU_ESM0, 1, 4);

    return retVal;
}

int32_t SDTF_runESMInjectLow1_MCU(void)
{
    int32_t retVal=0;
    esm_init_appcb(SDL_ESM_INST_MCU_ESM0);
    retVal = SDTF_runESMInjectInstance(SDL_ESM_INST_MCU_ESM0, 1, 0);

    return retVal;
}

int32_t SDTF_runESMInjectLow2_MCU(void)
{
    int32_t retVal=0;
    esm_init_appcb(SDL_ESM_INST_MCU_ESM0);
    retVal = SDTF_runESMInjectInstance(SDL_ESM_INST_MCU_ESM0, 3, 0); //1, 0);

    return retVal;
}
/*********************************************************************
 * @fn      SDTF_runESMInjectHigh_MAIN
 *
 * @brief   Execute ESM Inject for MAIN
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */

int32_t SDTF_runESMInjectHigh_MAIN(void)
{
    int32_t retVal=0;
    esm_init_appcb(SDL_ESM_INST_MAIN_ESM0);
    retVal = SDTF_runESMInjectInstance(SDL_ESM_INST_MAIN_ESM0, 3, 0);
    SDL_ESM_clrNError(SDL_ESM_INST_MAIN_ESM0);
    return retVal;
}

int32_t SDTF_runESMInjectLow_MAIN(void)
{
    int32_t retVal=0;
    esm_init_appcb(SDL_ESM_INST_MAIN_ESM0);
    retVal = SDTF_runESMInjectInstance(SDL_ESM_INST_MAIN_ESM0, 3, 1);
    SDL_ESM_clrNError(SDL_ESM_INST_MAIN_ESM0);
    return retVal;
}

int32_t SDTF_runESMInjectSelfTest_MAIN(void)
{
    int32_t retVal=0;
    esm_init_appcb(SDL_ESM_INST_MAIN_ESM0);
    retVal = SDTF_runESMInjectInstance(SDL_ESM_INST_MAIN_ESM0, 1, 8);
    SDL_ESM_clrNError(SDL_ESM_INST_MAIN_ESM0);
    return retVal;
}

int32_t ESMIntNumberTest(void)
{
    int32_t retVal=0;
    uint32_t intNum;
    intNum = SDL_ESM_getIntNumber(SDL_ESM_INST_MAIN_ESM0, SDL_ESM_INT_TYPE_MAX);
    if(intNum == 0xffffffffu)
    {
       intNum = SDL_ESM_getIntNumber(SDL_ESM_INST_WKUP_ESM0, SDL_ESM_INT_TYPE_MAX);
    }
    else
    {
        retVal = -1;
    }
    if(intNum == 0xffffffffu)
    {
       intNum = SDL_ESM_getIntNumber(SDL_ESM_INST_MCU_ESM0, SDL_ESM_INT_TYPE_MAX);
    }
    else
    {
        retVal = -1;
    }
    if(intNum == 0xffffffffu)
    {
       intNum = SDL_ESM_getIntNumber(SDL_ESM_INSTANCE_MAX, SDL_ESM_INT_TYPE_MAX);
    }
    else
    {
        retVal = -1;
    }
    return retVal;
}

int32_t Negative_test_priv_file(void){
    int32_t retVal=0;
    uint32_t esmInstBaseAddr;
    static uint32_t arg;
    void *ptr = (void *)&arg;
       SDL_ErrType_t result;
    result = SDL_ESM_init(SDL_ESM_INST_WKUP_ESM0, &ESM_esmInitConfig_WKUP,SDL_ESM_applicationCallbackFunction,ptr);
    if(result != SDL_PASS)
        UART_printf("SDL_ESM_init: failure \n");
    SDL_ESM_getBaseAddr(SDL_ESM_INST_WKUP_ESM0, &esmInstBaseAddr);
    if (SDL_ESM_enableIntr(esmInstBaseAddr, 40U) != SDL_PASS)
    {
        UART_printf("sdlEsm_apiTest: failure on line no. %d \n", __LINE__);
    }
    if (SDL_ESM_setIntrPriorityLvl(esmInstBaseAddr, 40U, ESM_INTR_PRIORITY_LEVEL_LOW) != SDL_PASS)
    {
        UART_printf("sdlEsm_apiTest: failure on line no. %d \n", __LINE__);
    }
    if (SDL_ESM_setIntrStatusRAW(esmInstBaseAddr, 40U) != SDL_PASS)
    {
        UART_printf("sdlEsm_apiTest: failure on line no. %d \n", __LINE__);
    }
    if (SDL_ESM_enableGlobalIntr(esmInstBaseAddr) != SDL_PASS)
    {
        UART_printf("sdlEsm_apiTest: failure on line no. %d \n", __LINE__);
    }
    if (SDL_ESM_setInfluenceOnErrPin(esmInstBaseAddr, 40U, true) != SDL_PASS)
    {
        UART_printf("sdlEsm_apiTest: failure on line no. %d \n", __LINE__);
    }
    if (SDL_ESM_setMode(esmInstBaseAddr, ESM_OPERATION_MODE_NORMAL) != SDL_PASS)
    {
        UART_printf("sdlEsm_apiTest: failure on line no. %d \n", __LINE__);
    }
    retVal =SDTF_runESMInjectInstance(SDL_ESM_INST_WKUP_ESM0, 1, 8);
    return retVal;
}

/* Other commands not covered by run_all */
#define SDTF_NUM_OTHER_TEST_COMMANDS (0u)
#define SDTF_MAX_COMMANDS (SDTF_NUM_RUNALL_TEST_COMMANDS+SDTF_NUM_OTHER_TEST_COMMANDS)

#define SDTF_MAX_COMMAND_LEN (64u)
typedef int32_t (* SDTF_testFunctionPtr_t) (void);

typedef struct SDTF_commandList_s
{
    char commandString[SDTF_MAX_COMMAND_LEN+1];
    SDTF_testFunctionPtr_t commandFunction;
}SDTF_commandList_t;
/* Full list of commands */
SDTF_commandList_t SDTF_commandList[SDTF_MAX_COMMANDS] =
{
    { "esm_injectHigh_WKUP",            SDTF_runESMInjectHigh_WKUP },
    { "esm_injectLow_WKUP",             SDTF_runESMInjectLow_WKUP },
    { "esm_injectHigh_MCU",             SDTF_runESMInjectHigh_MCU },
    { "esm_injectLow1_MCU",              SDTF_runESMInjectLow1_MCU },
    { "esm_injectLow2_MCU",              SDTF_runESMInjectLow2_MCU },
    { "esm_injectHigh_MAIN",            SDTF_runESMInjectHigh_MAIN },
    { "esm_injectLow_MAIN",             SDTF_runESMInjectLow_MAIN },
    { "esm_injectSelfTest_MAIN",        SDTF_runESMInjectSelfTest_MAIN },
    { "Negative_test_priv_file",        Negative_test_priv_file },
    { "ESMIntNumberTest",               ESMIntNumberTest },
};

int32_t test_sdr_test(void)
{
    /* Declarations of variables */
    int32_t retVal = 0;
    int32_t i;

    UART_printf("\n Running all sdr test commands supported");
    for(i = 0u; i< SDTF_NUM_RUNALL_TEST_COMMANDS; i++) {
        if (SDTF_commandList[i].commandFunction!= ((void *)(0u))) {
            retVal = (*SDTF_commandList[i].commandFunction)();
            if ( retVal != 0) {
                break;
            }
        }
    }

    if (retVal == 0)
    {
        UART_printStatus("\n All tests have passed. \n");
    }
    else
    {
        UART_printStatus("\n Few/all tests Failed \n");
    }

return retVal;
}
