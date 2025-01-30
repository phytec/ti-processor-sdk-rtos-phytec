/*
 * SDL ECC
 *
 * Software Diagnostics Library module for ECC
 *
 *  Copyright (c) Texas Instruments Incorporated 2018-2020
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

#include <string.h>
#include <sdl_types.h>
#include <sdl_ecc.h>
#include <src/ip/sdl_ip_ecc.h>

#include <src/ip/r5/sdl_arm_r5_pmu.h>
#include <src/ip/r5/sdl_vim.h>

#include <r5/sdl_r5_utils.h>
#include <ecc/sdl_ecc_utils.h>

#include <ecc/sdl_ecc_priv.h>
#include <ecc/sdl_ecc_core.h>
#include <soc.h>

#if defined (SOC_J721E)
#include <src/sdl/r5/j721e/sdl_soc_r5.h>
#endif

#if defined (SOC_J7200)
#include <src/sdl/r5/j7200/sdl_soc_r5.h>
#endif

#if defined (SOC_J721S2)
#include <src/sdl/r5/j721s2/sdl_soc_r5.h>
#endif

#if defined (SOC_J784S4)
#include <src/sdl/r5/j784s4/sdl_soc_r5.h>
#endif

/* Local defines */
#define SDL_ECC_R5_MONITOR_PMU_NUMBER (1u)
#define SDL_ECC_R5_CFLR_ATCM_DATA_ERROR_MASK (0x1000001u)
#define SDL_ECC_R5_CFLR_BTCM_DATA_ERROR_MASK (0x2000001u)

#define SDL_PMU_CTR_MAX_VALUE (0xffffffffu)

#define SDL_ECC_ATCM_SINGLE_BIT_ERROR_EVENT (0x67u)
#define SDL_ECC_B0TCM_SINGLE_BIT_ERROR_EVENT (0x68u)
#define SDL_ECC_B1TCM_SINGLE_BIT_ERROR_EVENT (0x69u)

/* Local functions */
static inline void SDL_ECC_R5EnablePmuForEccEvent(SDL_ECC_MemType eccMemType,
                              SDL_ECC_MemSubType memSubType,
                              SDL_ECC_InjectErrorType errorType);

static inline void SDL_ECC_R5DisablePmuForEccEvent(void);
/*********************************************************************
 *
 * /brief   Configure ECC for given ram Id for the CPU
 *
 * /param  ramId: Ram Id of memory section
 *
 * /return  SDL_PASS : Success; SDL_EFAIL for failures
 */
int32_t SDL_ECC_configECCRam(SDL_ECC_MemType eccMemType, uint32_t ramId)
{
    int32_t retVal= SDL_PASS;

    if (eccMemType == SDL_ECC_MEMTYPE_MCU_R5F0_CORE)
    {
        switch(ramId) {

            case SDL_ECC_R5F_MEM_SUBTYPE_ATCM0_BANK0_VECTOR_ID:
            case SDL_ECC_R5F_MEM_SUBTYPE_ATCM0_BANK1_VECTOR_ID:
                /* Enable ECC for ATCM */
                SDL_ECC_UTILS_enableECCATCM();

                break;

            case SDL_ECC_R5F_MEM_SUBTYPE_B0TCM0_BANK0_VECTOR_ID:
            case SDL_ECC_R5F_MEM_SUBTYPE_B0TCM0_BANK1_VECTOR_ID:
                /* Enable ECC for B0TCM */
                SDL_ECC_UTILS_enableECCB0TCM();

                break;

            case SDL_ECC_R5F_MEM_SUBTYPE_B1TCM0_BANK0_VECTOR_ID:
            case SDL_ECC_R5F_MEM_SUBTYPE_B1TCM0_BANK1_VECTOR_ID:
                /* Enable ECC for B1TCM */
                SDL_ECC_UTILS_enableECCB1TCM();

                break;

            default:
                if ( ramId > SDL_ECC_R5F_MEM_SUBTYPE_KS_VIM_RAM_VECTOR_ID) {
                    retVal = SDL_EFAIL;
                }
                break;
        }
    }

    return retVal;

}

/*********************************************************************
 *
 * /brief   Poll ecc event
 *
 * \param  eccMemType: Memory type
 * \param  memSubType: Memory subtype
 * \param  errorType:  Error type to poll
 *
 * \return SDL_ECC_EVENT_FOUND or if no events found
 */
uint32_t SDL_ECC_pollErrorEvent(SDL_ECC_MemType eccMemType,
                               SDL_ECC_MemSubType memSubType,
                               SDL_ECC_InjectErrorType errorType)
{
    uint32_t retValue = 0u;
    uint32_t regValue;

    /* Polling only for R5F core self test */
    if ((eccMemType == SDL_ECC_MEMTYPE_MCU_R5F0_CORE)
        || (eccMemType == SDL_ECC_MEMTYPE_MCU_R5F1_CORE)) {
        switch(errorType) {
            case SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE:
                /* Only for single bit error do polling */
                switch (memSubType) {
                    case SDL_ECC_R5F_MEM_SUBTYPE_ATCM0_BANK0_VECTOR_ID:
                    case SDL_ECC_R5F_MEM_SUBTYPE_ATCM0_BANK1_VECTOR_ID:
                        /* Check PMOVSR register */
                        regValue =  SDL_UTILS_getPMOVSR();
						if((regValue & (1u <<SDL_ECC_R5_MONITOR_PMU_NUMBER)) > (uint32_t)0u) {
                            /* Check CFLR register */
                            regValue = SDL_UTILS_getCFLR();
                            if((regValue & SDL_ECC_R5_CFLR_ATCM_DATA_ERROR_MASK)
                               == SDL_ECC_R5_CFLR_ATCM_DATA_ERROR_MASK) {
                                retValue = (uint32_t)(SDL_ECC_EVENT_FOUND);
                            }
                        }
                        break;

                    case SDL_ECC_R5F_MEM_SUBTYPE_B0TCM0_BANK0_VECTOR_ID:
                    case SDL_ECC_R5F_MEM_SUBTYPE_B0TCM0_BANK1_VECTOR_ID:
                    case SDL_ECC_R5F_MEM_SUBTYPE_B1TCM0_BANK0_VECTOR_ID:
                    case SDL_ECC_R5F_MEM_SUBTYPE_B1TCM0_BANK1_VECTOR_ID:
                        /* Check PMOVSR register */
                        regValue =  SDL_UTILS_getPMOVSR();
                        if((regValue & (1u <<SDL_ECC_R5_MONITOR_PMU_NUMBER)) > (uint32_t)0u) {
                            /* Check CFLR register */
                            regValue = SDL_UTILS_getCFLR();
                            if( (regValue & SDL_ECC_R5_CFLR_BTCM_DATA_ERROR_MASK)
                                == SDL_ECC_R5_CFLR_BTCM_DATA_ERROR_MASK) {
                                retValue = (uint32_t)(SDL_ECC_EVENT_FOUND);
                            }
                        }
                        break;

                    default:

                        break;
                }
                break;

            default:
                break;
        }
    }
    return retValue;
}

/*********************************************************************
 *
 * \brief   Configure PMU to monitor specific ECC event
 *
 * \param  eccMemType: Memory type
 * \param  memSubType: Memory subtype
 * \param  errorType:  Error type to poll
 *
 * \return  None
 */
static inline void SDL_ECC_R5EnablePmuForEccEvent(SDL_ECC_MemType eccMemType,
                              SDL_ECC_MemSubType memSubType,
                              SDL_ECC_InjectErrorType errorType)
{
    if ((eccMemType == SDL_ECC_MEMTYPE_MCU_R5F0_CORE)
        || (eccMemType == SDL_ECC_MEMTYPE_MCU_R5F1_CORE)) {
        switch (memSubType) {
            case SDL_ECC_R5F_MEM_SUBTYPE_ATCM0_BANK0_VECTOR_ID:
            case SDL_ECC_R5F_MEM_SUBTYPE_ATCM0_BANK1_VECTOR_ID:
                switch(errorType) {
                    case SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE:
                       /* enable PMU event monitoring of ATCM 1 bit*/
                       SDL_ECC_UTILS_configSecIntr(SDL_PMU_CTR_MAX_VALUE,
                                                 SDL_ECC_ATCM_SINGLE_BIT_ERROR_EVENT,
                                                 1);
                       break;

                    default:
                        break;
                }
                break;

            case SDL_ECC_R5F_MEM_SUBTYPE_B0TCM0_BANK0_VECTOR_ID:
            case SDL_ECC_R5F_MEM_SUBTYPE_B0TCM0_BANK1_VECTOR_ID:
                switch(errorType) {
                    case SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE:
                       /* enable PMU event monitoring of ATCM 1 bit*/
                       SDL_ECC_UTILS_configSecIntr(SDL_PMU_CTR_MAX_VALUE,
                                                 SDL_ECC_B0TCM_SINGLE_BIT_ERROR_EVENT,
                                                 1);
                       break;

                    default:
                        break;
                }
                break;

            case SDL_ECC_R5F_MEM_SUBTYPE_B1TCM0_BANK0_VECTOR_ID:
            case SDL_ECC_R5F_MEM_SUBTYPE_B1TCM0_BANK1_VECTOR_ID:
                switch(errorType) {
                    case SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE:
                       /* enable PMU event monitoring of ATCM 1 bit*/
                       SDL_ECC_UTILS_configSecIntr(SDL_PMU_CTR_MAX_VALUE,
                                                 SDL_ECC_B1TCM_SINGLE_BIT_ERROR_EVENT,
                                                 1);
                       break;

                    default:
                       break;
               }
               break;

            default:
                break;
        }
    }
}

/*********************************************************************
 *
 * \brief   Disable PMU from monitoring specific ECC event
 *
 * \param  eccMemType: Memory type
 * \param  memSubType: Memory subtype
 * \param  errorType:  Error type to monitor
 *
 * \return  None
 */
static inline void SDL_ECC_R5DisablePmuForEccEvent(void)
{
    /* PMU Disable counter */
    (void)SDL_R5PMU_enableCntr(SDL_ECC_R5_MONITOR_PMU_NUMBER, 0u );
    /* Disable counter overflow interrupt */
    (void)SDL_R5PMU_enableCntrOverflowIntr(SDL_ECC_R5_MONITOR_PMU_NUMBER, 0u);

}

/*********************************************************************
 *
 * \brief   Disable monitoring specific ECC event
 *
 * \param  eccMemType: Memory type
 * \param  memSubType: Memory subtype
 * \param  errorType:  Error type to monitor
 *
 * \return  None
 */
void SDL_ECC_enableECCEventCheck(SDL_ECC_MemType eccMemType,
                                SDL_ECC_MemSubType memSubType,
                                SDL_ECC_InjectErrorType errorType)
{
    switch(eccMemType)
    {
        case SDL_ECC_MEMTYPE_MCU_R5F0_CORE:
        case SDL_ECC_MEMTYPE_MCU_R5F1_CORE:

            switch(errorType)
            {
                case SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE:
                case SDL_INJECT_ECC_ERROR_FORCING_1BIT_N_ROW_ONCE:
                case SDL_INJECT_ECC_ERROR_FORCING_1BIT_REPEAT:
                case SDL_INJECT_ECC_ERROR_FORCING_1BIT_N_ROW_REPEAT:
                    SDL_ECC_R5EnablePmuForEccEvent(eccMemType, memSubType, errorType);
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }

    return;
}

/*********************************************************************
 *
 * \brief   Disable monitoring specific ECC event
 *
 * \param  eccMemType: Memory type
 * \param  errorType:  Error type to monitor
 *
 * \return  None
 */
void SDL_ECC_disableECCEventCheck(SDL_ECC_MemType eccMemType,
                                 SDL_ECC_InjectErrorType errorType)
{
    switch(eccMemType)
     {
         case SDL_ECC_MEMTYPE_MCU_R5F0_CORE:
         case SDL_ECC_MEMTYPE_MCU_R5F1_CORE:

             switch(errorType)
             {
                 case SDL_INJECT_ECC_ERROR_FORCING_1BIT_ONCE:
                 case SDL_INJECT_ECC_ERROR_FORCING_1BIT_N_ROW_ONCE:
                 case SDL_INJECT_ECC_ERROR_FORCING_1BIT_REPEAT:
                 case SDL_INJECT_ECC_ERROR_FORCING_1BIT_N_ROW_REPEAT:
                     SDL_ECC_R5DisablePmuForEccEvent();
                     break;

                 default:
                     break;
             }
             break;

         default:
             break;
     }

     return;

}

/** ============================================================================
 *
 * \brief   Register Handler for VIM DED ECC error
 *
 * \param  VIMDEDHandler: This is function to be called when an Double bit ECC
 *                        error happens on the VIM RAM. NOTE: That the regular
 *                        vector load will be replaced with this registered
 *                        function.
 *
 * \return  None
 */
void SDL_ECC_registerVIMDEDHandler(SDL_ECC_VIMDEDVector_t VIMDEDHandler)
{
    SDL_vimRegs     *pVimRegs;

    /* initialize the address */
    pVimRegs        = (SDL_vimRegs *)(SDL_MCU_VIC_CFG_BASE);

    SDL_VIM_setDedVectorAddr(pVimRegs, (uint32_t)VIMDEDHandler);
}

