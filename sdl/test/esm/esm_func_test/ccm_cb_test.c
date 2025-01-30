#include <src/ip/sdl_esm.h>
#include <ti/drv/uart/UART.h>
#include <ti/drv/uart/UART_stdio.h>
#include <stdint.h>
#include "esm_test_main.h"

#define SDTF_NUM_RUNALL_TEST_COMMANDS 3

SDL_ESM_config esmInitConfig_MCU_appcallback =
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

/* Event BitMap for ECC ESM callback for MAIN */
uint32_t eventBitMapMCU_CB[SDL_ESM_MAX_EVENT_MAP_NUM_WORDS] =
{
     0x00000000u, 0xff0fffffu, 0x7fffffffu, 0x00000007u,
};

extern int32_t SDR_ESM_errorInsert (const SDL_ESM_Inst esmInstType,
                                const SDL_ESM_ErrorConfig_t *esmErrorConfig);

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

extern int32_t SDL_ESM_ECCapplicationCallbackFunction(SDL_ESM_Inst esmInstType,
                                         SDL_ESM_IntType esmIntType,
                                         uint32_t grpChannel,
                                         uint32_t index,
                                         uint32_t intSrc,
                                         void *arg);

extern int32_t SDL_ESM_CCMapplicationCallbackFunction(SDL_ESM_Inst esmInstType,
                                         SDL_ESM_IntType esmIntType,
                                         uint32_t grpChannel,
                                         uint32_t index,
                                         uint32_t intSrc,
                                         void *arg);


/*********************************************************************
 * @fn      SDTF_runESMInject_WKUP
 *
 * @brief   Execute ESM Inject for WKUP
 *
 * @param   None
 *
 * @return  0 : Success; < 0 for failures
 */

int32_t ccm_cb_test(void)
{
    int32_t retVal=0;
    UART_printStatus("\n inside ccm testcase \n");
    SDL_ESM_init(SDL_ESM_INST_MCU_ESM0, &esmInitConfig_MCU_appcallback, SDL_ESM_CCMapplicationCallbackFunction, (void*)0x1);
    SDL_ESM_registerCCMCallback(SDL_ESM_INST_MCU_ESM0,eventBitMapMCU_CB,
                                    SDL_ESM_CCMapplicationCallbackFunction,
                                    (void*)0x2);
    retVal = SDTF_runESMInjectInstance(SDL_ESM_INST_MCU_ESM0, 1, 0);
    SDL_ESM_clrNError(SDL_ESM_INST_MCU_ESM0);
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
SDTF_commandList_t SDTF_commandList_ccm_cb[SDTF_MAX_COMMANDS] =
{
    { "ccm_cb_test",                ccm_cb_test },
};

int32_t sdl_ccm_cb_test(void)
{
    /* Declarations of variables */
    int32_t retVal = 0;
    int32_t i;

    UART_printf("\n Running all sdr test commands supported");
    for(i = 0u; i< SDTF_NUM_RUNALL_TEST_COMMANDS; i++) {
        if (SDTF_commandList_ccm_cb[i].commandFunction!= ((void *)(0u))) {
            retVal = (*SDTF_commandList_ccm_cb[i].commandFunction)();
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
