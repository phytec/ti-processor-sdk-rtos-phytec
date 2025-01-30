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
 *  \file     app_utils.h
 *
 *  \brief    This file contains common utility functions
 *
 */

#ifndef APP_UTILS_H_
#define APP_UTILS_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "stdio.h"
#include "Std_Types.h"
#include <hw_include/soc.h>

#if defined (SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J784S4) || defined (SOC_AM62X) || defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
#include <hw_include/soc/lldr_soc_ctrl_mmr.h>
#endif

#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
/* SCI Client */
#include <ti/drv/sciclient/sciclient.h>
#endif

#if defined (SOC_AM62X) || defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
/* SCI Client */
#include <drivers/sciclient.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
/** \brief Address of the UART to used to display messages on console */
#define APP_UTILS_UART_CONSOLE_ADDR    (0x0U)

/** \brief Magic String to indicate successful completion */
#define APP_UTILS_STATUS_PASS   (0x50415353U)

/** \brief Magic String to indicate failures */
#define APP_UTILS_STATUS_FAIL   (0x46417376U)

/** \brief Status messages only, used limit to critical messages only */
#define APP_UTILS_PRINT_MSG_STATUS      (0x01U)
/** \brief Status messages only, used limit to critical messages only */
#define APP_UTILS_PRINT_MSG_NORMAL      (0x02U)
/** \brief Determines the log level enabled */
#define APP_UTILS_PRINT_MSG_LEVEL       (0x01U)
/** \brief Maximum size of the string that could be printed */
#define APP_UTILS_PRINT_MAX_STRING_SIZE (2000U)
/** \brief Demo / Example Status */
#define APP_UTILS_TEST_STATUS_INIT      (0U)
/** \brief Demo / Example Status */
#define APP_UTILS_TEST_STATUS_PASS      (2U)
/** \brief Demo / Example Status */
#define APP_UTILS_TEST_STATUS_FAIL      (3U)

#define SCICLIENT_SERVICE_WAIT_FOREVER (0xFFFFFFFFU)

/* These should match what is given in linker command file */
/** \brief Length of the section used to check section corruption */
#define APP_UTILS_LINKER_FILL_LENGTH    (0x100U)
/** \brief Pattern written in each sections */
#define APP_UTILS_LINKER_FILL_PATTERN   (0xFEAA55EFU)
/** \brief Sizeof Utility macro */
#define MODULEAPP_NUM_SEC_HOLES(x)     (sizeof (x) / sizeof (x[0]))
/** \brief SCICLIENT API timeout */
#define APP_SCICLIENT_TIMEOUT          (SCICLIENT_SERVICE_WAIT_FOREVER)

#define TISCI_MSG_VALUE_DEVICE_HW_STATE_OFF         0

#define TISCI_BIT(n)  (1UL << (n))


#define TISCI_MSG_FLAG_AOP    TISCI_BIT(1)


#define TISCI_MSG_FLAG_DEVICE_RESET_ISO       TISCI_BIT(9)

#define TISCI_MSG_FLAG_CLOCK_ALLOW_FREQ_CHANGE        TISCI_BIT(9)
/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */
/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
/** \brief Initializes the core internal timer (PMU in case of R5F)
 *
 *          This should be called before any profiling API's are called. (i.e.
 *          AppUtils_GetTimeStamp (), AppUtils_GetTimeStampFreq(),
 *          AppUtils_GetCycles()
 */
void AppUtils_ProfileInit(uint32 unUsedForNow);

/** \brief Provides the frequency of the clock source used to drive counter
 *
 */
uint32 AppUtils_ProfileGetClkFreq(void);

/** \brief Provides the current tick of the counter
 *
 *      Start the timer, when called for the very first time and returns the
 *      counts. On subsequent calls, returns the counts.
 *
 *      Please note that counter overflows might not track-able, please use
 *      caution while using
 */
uint32 AppUtils_ProfileGetCycles(void);

/** \brief Provides time in milliseconds w.r.t to provided clk ticks
 *
 *      Please on counter overflow, a value of 0xFFFFFFFF would be returned.
 */
uint32 AppUtils_ProfileGetElapsedTimeInMs(uint32 cycleCnt);

/** \brief Stores the provided 32bit value in global variable in segment
        EG_TEST_RESULT_32_SECTION. Example application will use to track the
        status of test/example application result. */
void AppUtils_LogResult(uint32 testResult);
/** \brief Print Init Utility
 *
 *  type        :void
 */
#ifdef UART_ENABLED
void AppUtils_Init(void);
#endif
/** \brief Print utility used to display on CCS console of UART terminal.
 *
 *  type        :Either APP_UTILS_PRINT_MSG_STATUS or APP_UTILS_PRINT_MSG_NORMAL
 *  baseAddr    :Not used for now
 *  pcString    :printf, string
 */
void AppUtils_Printf(uint32 type, const char *pcString, ...);
/** \brief Input utility used to accept a character from terminal
 *
 *  type        :char
 *  baseAddr    :Not used for now
 */
char AppUtils_GetChar(void);
/** \brief Input utility used to accept a number from terminal
 *
 *  type        :sint32
 *  baseAddr    :Not used for now
 */
sint32 AppUtils_GetNum(void);
/** \brief Input utility used to set MCU Core Frequency
 *
 *  type        :void
 */
#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4) || defined (SOC_AM62X) || (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
void AppUtils_SetCpuFrequency(void);
#endif
/** \brief Utility used to get stack usage.
 *   Parameters (in)    : None
 *   Parameters (out)   : Max stack used in bytes.
 *   Prerequisite - Known pattern 0xFEAA55EFU is written to the entire
 *                  stack size in linker command file
 *   In this API, stack size is known from the compiler macro __STACK_SIZE and
 *   stack start pointer from __stack.
 *   Pattern(0xFEAA55EFU) matching is done from the start of the stack and
 *   it is continued until pattern does not matches. The Stack
 *   region not consumed is updated in a variable freeEntry.
 *   Max stack used is calculated by (__STACK_SIZE - freeEntry).
 */
uint32 AppUtils_GetStackUsage(void);
/** \brief Utility used to Adc Voltage.
 *   Parameters (in)    : adcValue
 *                        Digital adc output value
 *   Parameters (out)   : Returns analog adc value.
 */
uint32 AppUtils_GetAdcVoltage(uint32 adcValue);
/** \brief Utility used to initialize memory sections.
 *   Parameters (in)    : pAppUtils_SecHoleAddrList
 *                        Pointer to the list of the memory sections of each
 *                        module defined in linker command.
 *   Parameters (in)    : secCnt
 *                        Number of total memory sections
 *   Parameters (out)   : None
 *
 *   This API is used to initialize fixed size(APP_UTILS_LINKER_FILL_LENGTH)
 *   before start of each section and before end of each section. i.e For eg
 *      .=align(4);
 *       __linker_spi_text_start = .;
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      *(SPI_TEXT_SECTION)
 *      .=align(4);
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      __linker_spi_text_end = .;
 *
 *   This API should be called during module start up code.
 */
void AppUtils_SectionInit(
        uint32 *pAppUtils_SecHoleAddrList, uint32 secCnt);
/** \brief Utility used to check any memory corruption occured in
 *   stack and memory sections initialized by AppUtils_SectionInit.
 *   Parameters (in)    : pAppUtils_SecHoleAddrList
 *                        Pointer to the list of the memory sections of each
 *                        module defined in linker command.
 *   Parameters (in)    : secCnt
 *                        Number of total memory sections
 *   Parameters (out)   : retVal
 *                        E_OK:     No corruption occured.
 *                        E_NOT_OK: Corruption occured.
 *
 *   In this API, both stack and memory sections initialized by
 *   AppUtils_SectionInit are checked.
 *   Stack size is known from the compiler macro __STACK_SIZE and stack size
 *   used is calculated from AppUtils_GetStackUsage. If stack size used is
 *   greater than __STACK_SIZE then stack corruption occured and return E_NOT_OK
 *   Similarly Pattern(0xFEAA55EFU) matching is done from the start of the each
 *   memory section initialized by AppUtils_SectionInit and it is continued
 *   till end of the section. If there is a pattern mismatch, breaks and
 *   returns E_NOT_OK otherwise returns E_OK.
 *
 *   This API expects AppUtils_SectionInit should be called in start up code
 *   before calling this.
 */
uint32 AppUtils_CheckStackAndSectionCorruption(
        uint32 *pAppUtils_SecHoleAddrList, uint32 secCnt);
/** \brief Utility used to initialize Dio memory sections.
 *   Parameters (in)    : None
 *   Parameters (out)   : None
 *
 *   This API is used to initialize fixed size(APP_UTILS_LINKER_FILL_LENGTH)
 *   before start of each section and before end of each section. i.e For eg
 *      .=align(4);
 *       __linker_dio_text_start = .;
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      *(DIO_TEXT_SECTION)
 *      .=align(4);
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      __linker_dio_text_end = .;
 *
 *   This API should be called during module start up code.
 *   This API in turn calls AppUtils_SectionInit to initialize Dio specific
 *   memory sections.
 */
void AppUtils_DioSectionInit(void);
/** \brief Utility used to check any memory corruption occured in
 *   stack and memory sections initialized by AppUtils_DioSectionInit.
 *   Parameters (in)    : None
 *   Parameters (out)   : retVal
 *                        E_OK:     No corruption occured.
 *                        E_NOT_OK: Corruption occured.
 *
 *   In this API, both stack and memory sections initialized by
 *   AppUtils_DioSectionInit are checked.
 *   This API expects AppUtils_DioSectionInit should be called in start up code
 *   before calling this.
 */
uint32 AppUtils_DioCheckStackAndSectionCorruption(void);
/** \brief Utility used to initialize Fls memory sections.
 *   Parameters (in)    : None
 *   Parameters (out)   : None
 *
 *   This API is used to initialize fixed size(APP_UTILS_LINKER_FILL_LENGTH)
 *   before start of each section and before end of each section. i.e For eg
 *      .=align(4);
 *       __linker_fls_text_start = .;
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      *(DIO_TEXT_SECTION)
 *      .=align(4);
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      __linker_fls_text_end = .;
 *
 *   This API should be called during module start up code.
 *   This API in turn calls AppUtils_SectionInit to initialize Dio specific
 *   memory sections.
 */
void AppUtils_FlsSectionInit(void);
/** \brief Utility used to check any memory corruption occured in
 *   stack and memory sections initialized by AppUtils_DioSectionInit.
 *   Parameters (in)    : None
 *   Parameters (out)   : retVal
 *                        E_OK:     No corruption occured.
 *                        E_NOT_OK: Corruption occured.
 *
 *   In this API, both stack and memory sections initialized by
 *   AppUtils_DioSectionInit are checked.
 *   This API expects AppUtils_DioSectionInit should be called in start up code
 *   before calling this.
 */
uint32 AppUtils_FlsCheckStackAndSectionCorruption(void);
/** \brief Utility used to initialize Gpt memory sections.
 *   Parameters (in)    : None
 *   Parameters (out)   : None
 *
 *   This API is used to initialize fixed size(APP_UTILS_LINKER_FILL_LENGTH)
 *   before start of each section and before end of each section. i.e For eg
 *      .=align(4);
 *       __linker_gpt_text_start = .;
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      *(GPT_TEXT_SECTION)
 *      .=align(4);
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      __linker_gpt_text_end = .;
 *
 *   This API should be called during module start up code.
 *   This API in turn calls AppUtils_SectionInit to initialize Gpt specific
 *   memory sections.
 */
void AppUtils_GptSectionInit(void);
/** \brief Utility used to check any memory corruption occured in
 *   stack and memory sections initialized by AppUtils_GptSectionInit.
 *   Parameters (in)    : None
 *   Parameters (out)   : retVal
 *                        E_OK:     No corruption occured.
 *                        E_NOT_OK: Corruption occured.
 *
 *   In this API, both stack and memory sections initialized by
 *   AppUtils_GptSectionInit are checked.
 *   This API expects AppUtils_GptSectionInit should be called in start up code
 *   before calling this.
 */
uint32 AppUtils_GptCheckStackAndSectionCorruption(void);
/** \brief Utility used to initialize Spi memory sections.
 *   Parameters (in)    : None
 *   Parameters (out)   : None
 *
 *   This API is used to initialize fixed size(APP_UTILS_LINKER_FILL_LENGTH)
 *   before start of each section and before end of each section. i.e For eg
 *      .=align(4);
 *       __linker_spi_text_start = .;
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      *(SPI_TEXT_SECTION)
 *      .=align(4);
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      __linker_spi_text_end = .;
 *
 *   This API should be called during module start up code.
 *   This API in turn calls AppUtils_SectionInit to initialize Spi specific
 *   memory sections.
 */
void AppUtils_SpiSectionInit(void);
/** \brief Utility used to check any memory corruption occured in
 *   stack and memory sections initialized by AppUtils_SpiSectionInit.
 *   Parameters (in)    : None
 *   Parameters (out)   : retVal
 *                        E_OK:     No corruption occured.
 *                        E_NOT_OK: Corruption occured.
 *
 *   In this API, both stack and memory sections initialized by
 *   AppUtils_SpiSectionInit are checked.
 *   This API expects AppUtils_SpiSectionInit should be called in start up code
 *   before calling this.
 */
uint32 AppUtils_SpiCheckStackAndSectionCorruption(void);
/** \brief Utility used to initialize Can memory sections.
 *   Parameters (in)    : None
 *   Parameters (out)   : None
 *
 *   This API is used to initialize fixed size(APP_UTILS_LINKER_FILL_LENGTH)
 *   before start of each section and before end of each section. i.e For eg
 *      .=align(4);
 *       __linker_can_text_start = .;
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      *(CAN_TEXT_SECTION)
 *      .=align(4);
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      __linker_can_text_end = .;
 *
 *   This API should be called during module start up code.
 *   This API in turn calls AppUtils_SectionInit to initialize Can specific
 *   memory sections.
 */
void AppUtils_CanSectionInit(void);
/** \brief Utility used to check any memory corruption occured in
 *   stack and memory sections initialized by AppUtils_CanSectionInit.
 *   Parameters (in)    : None
 *   Parameters (out)   : retVal
 *                        E_OK:     No corruption occured.
 *                        E_NOT_OK: Corruption occured.
 *
 *   In this API, both stack and memory sections initialized by
 *   AppUtils_CanSectionInit are checked.
 *   This API expects AppUtils_CanSectionInit should be called in start up code
 *   before calling this.
 */
uint32 AppUtils_CanCheckStackAndSectionCorruption(void);
/** \brief Utility used to initialize Eth memory sections.
 *   Parameters (in)    : None
 *   Parameters (out)   : None
 *
 *   This API is used to initialize fixed size(APP_UTILS_LINKER_FILL_LENGTH)
 *   before start of each section and before end of each section. i.e For eg
 *      .=align(4);
 *       __linker_eth_text_start = .;
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      *(ETH_TEXT_SECTION)
 *      .=align(4);
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      __linker_eth_text_end = .;
 *
 *   This API should be called during module start up code.
 *   This API in turn calls AppUtils_SectionInit to initialize Eth specific
 *   memory sections.
 */
void AppUtils_EthSectionInit(void);
/** \brief Utility used to check any memory corruption occured in
 *   stack and memory sections initialized by AppUtils_EthSectionInit.
 *   Parameters (in)    : None
 *   Parameters (out)   : retVal
 *                        E_OK:     No corruption occured.
 *                        E_NOT_OK: Corruption occured.
 *
 *   In this API, both stack and memory sections initialized by
 *   AppUtils_EthSectionInit are checked.
 *   This API expects AppUtils_EthSectionInit should be called in start up code
 *   before calling this.
 */
uint32 AppUtils_EthCheckStackAndSectionCorruption(void);
/** \brief Utility used to initialize Wdg memory sections.
 *   Parameters (in)    : None
 *   Parameters (out)   : None
 *
 *   This API is used to initialize fixed size(APP_UTILS_LINKER_FILL_LENGTH)
 *   before start of each section and before end of each section. i.e For eg
 *      .=align(4);
 *       __linker_wdg_text_start = .;
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      *(WDG_TEXT_SECTION)
 *      .=align(4);
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      __linker_wdg_text_end = .;
 *
 *   This API should be called during module start up code.
 *   This API in turn calls AppUtils_SectionInit to initialize Wdg specific
 *   memory sections.
 */
void AppUtils_WdgSectionInit(void);
/** \brief Utility used to check any memory corruption occured in
 *   stack and memory sections initialized by AppUtils_WdgSectionInit.
 *   Parameters (in)    : None
 *   Parameters (out)   : retVal
 *                        E_OK:     No corruption occured.
 *                        E_NOT_OK: Corruption occured.
 *
 *   In this API, both stack and memory sections initialized by
 *   AppUtils_WdgSectionInit are checked.
 *   This API expects AppUtils_WdgSectionInit should be called in start up code
 *   before calling this.
 */
uint32 AppUtils_WdgCheckStackAndSectionCorruption(void);
/** \brief Utility used to initialize Pwm memory sections.
 *   Parameters (in)    : None
 *   Parameters (out)   : None
 *
 *   This API is used to initialize fixed size(APP_UTILS_LINKER_FILL_LENGTH)
 *   before start of each section and before end of each section. i.e For eg
 *      .=align(4);
 *       __linker_pwm_text_start = .;
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      *(PWM_TEXT_SECTION)
 *      .=align(4);
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      __linker_pwm_text_end = .;
 *
 *   This API should be called during module start up code.
 *   This API in turn calls AppUtils_SectionInit to initialize Pwm specific
 *   memory sections.
 */
void AppUtils_PwmSectionInit(void);
/** \brief Utility used to check any memory corruption occured in
 *   stack and memory sections initialized by AppUtils_PwmSectionInit.
 *   Parameters (in)    : None
 *   Parameters (out)   : retVal
 *                        E_OK:     No corruption occured.
 *                        E_NOT_OK: Corruption occured.
 *
 *   In this API, both stack and memory sections initialized by
 *   AppUtils_PwmSectionInit are checked.
 *   This API expects AppUtils_PwmSectionInit should be called in start up code
 *   before calling this.
 */


uint32 AppUtils_PwmCheckStackAndSectionCorruption(void);
/** \brief Utility used to initialize Adc memory sections.
 *   Parameters (in)    : None
 *   Parameters (out)   : None
 *
 *   This API is used to initialize fixed size(APP_UTILS_LINKER_FILL_LENGTH)
 *   before start of each section and before end of each section. i.e For eg
 *      .=align(4);
 *       __linker_adc_text_start = .;
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      *(ADC_TEXT_SECTION)
 *      .=align(4);
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      __linker_adc_text_end = .;
 *
 *   This API should be called during module start up code.
 *   This API in turn calls AppUtils_SectionInit to initialize Adc specific
 *   memory sections.
 */
void AppUtils_IcuSectionInit(void);
/** \brief Utility used to check any memory corruption occured in
 *   stack and memory sections initialized by AppUtils_IcuSectionInit.
 *   Parameters (in)    : None
 *   Parameters (out)   : retVal
 *                        E_OK:     No corruption occured.
 *                        E_NOT_OK: Corruption occured.
 *
 *   In this API, both stack and memory sections initialized by
 *   AppUtils_IcuSectionInit are checked.
 *   This API expects AppUtils_IcuSectionInit should be called in start up code
 *   before calling this.
 */


uint32 AppUtils_IcuCheckStackAndSectionCorruption(void);
/** \brief Utility used to initialize Adc memory sections.
 *   Parameters (in)    : None
 *   Parameters (out)   : None
 *
 *   This API is used to initialize fixed size(APP_UTILS_LINKER_FILL_LENGTH)
 *   before start of each section and before end of each section. i.e For eg
 *      .=align(4);
 *       __linker_adc_text_start = .;
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      *(ADC_TEXT_SECTION)
 *      .=align(4);
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      __linker_adc_text_end = .;
 *
 *   This API should be called during module start up code.
 *   This API in turn calls AppUtils_SectionInit to initialize Adc specific
 *   memory sections.
 */
void AppUtils_AdcSectionInit(void);
/** \brief Utility used to check any memory corruption occured in
 *   stack and memory sections initialized by AppUtils_AdcSectionInit.
 *   Parameters (in)    : None
 *   Parameters (out)   : retVal
 *                        E_OK:     No corruption occured.
 *                        E_NOT_OK: Corruption occured.
 *
 *   In this API, both stack and memory sections initialized by
 *   AppUtils_AdcSectionInit are checked.
 *   This API expects AppUtils_AdcSectionInit should be called in start up code
 *   before calling this.
 */
uint32 AppUtils_AdcCheckStackAndSectionCorruption(void);

/** \brief Utility used to initialize memory sections.
 *   This API is used to initialize fixed size(APP_UTILS_LINKER_FILL_LENGTH)
 *   before start of each section and before end of each section. i.e
 *      .=align(4);
 *       __linker_cdd_ipc_text_start = .;
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      *(ADC_TEXT_SECTION)
 *      .=align(4);
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      __linker_cdd_ipc_text_end = .;
 *
 *   This API should be called during module start up code.
 */
void AppUtils_CddIpcSectionInit(void);

/** \brief Utility used to check any memory corruption occurred in
 *   stack and memory sections initialized by AppUtils_AdcSectionInit.
 *   In this API, both stack and memory sections initialized by
 *   AppUtils_AdcSectionInit are checked.
 *
 *   This API expects AppUtils_CddIpcSectionInit should be called in start up
 *   code before calling this.
 */
uint32 AppUtils_CddIpcCheckStackAndSectionCorruption(void);

/** \brief Utility used to enable module clock explicitly. */
sint32 AppUtils_ModuleClockEnable(uint32 moduleId);

/** \brief Utility used to initialize Mcu memory sections.
 *   Parameters (in)    : None
 *   Parameters (out)   : None
 *
 *   This API is used to initialize fixed size(APP_UTILS_LINKER_FILL_LENGTH)
 *   before start of each section and before end of each section. i.e For eg
 *      .=align(4);
 *       __linker_gpt_text_start = .;
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      *(GPT_TEXT_SECTION)
 *      .=align(4);
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      __linker_gpt_text_end = .;
 *
 *   This API should be called during module start up code.
 *   This API in turn calls AppUtils_SectionInit to initialize Mcu specific
 *   memory sections.
 */
void AppUtils_McuSectionInit(void);

/** \brief Utility used to check any memory corruption occured in
 *   stack and memory sections initialized by AppUtils_McuSectionInit.
 *   Parameters (in)    : None
 *   Parameters (out)   : retVal
 *                        E_OK:     No corruption occured.
 *                        E_NOT_OK: Corruption occured.
 *
 *   In this API, both stack and memory sections initialized by
 *   AppUtils_McuSectionInit are checked.
 *   This API expects AppUtils_McuSectionInit should be called in start up code
 *   before calling this.
 */
uint32 AppUtils_McuCheckStackAndSectionCorruption(void);

/** \brief Utility used to call the Internal API to 
 *   translate an IR output to the destination processor
 *   input IRQ value.
 *   Parameters (in)    : coreId, eventId, procIrq
 *   Parameters (out)   : retVal 
 *                        E_OK:     Success.
 *                        E_NOT_OK: Error in translation.
 *
 *   This API is used for Eth Virtmac Application.
 */
int32_t Ipc_sciclientIrqTranslate(uint16 coreId, uint32 eventId,
        uint16 *procIrq);

/** \brief Utility used to get the Interrupt range.
 *   Parameters (in)    : coreIndex, *rangeStartP, *rangeNumP
 *   Parameters (out)   : retVal
 *                        E_OK:  interrupt number range successful.
 *                        E_NOT_OK: Error in getting interrupt number range.
 *
 *   This API is used for Eth Virtmac Application.
 */
int32_t Ipc_getIntNumRange(uint32 coreIndex,
        uint16 *rangeStartP, uint16 *rangeNumP);

/**
 *  \brief Utility SciClient Open DMA function
 *
 *  Open DMA resource for eth controller via Sciclient
 *
 *  \param ctrlIdx  Controller Idx
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType AppUtils_EthDmaOpen(uint8 ctrlIdx);

/**
 *  \brief Utility SciClient Close DMA function
 *
 *  Open DMA resource for eth controller via Sciclient
 *
 *  \param ctrlIdx  Controller Idx
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType AppUtils_EthDmaClose(uint8 ctrlIdx);

/**
 *  \brief Utility SciClient configure ring function
 *
 *  Configure sciclient for ring via Sciclient
 *
 *  \param ctrlIdx  Controller Idx
 *  \param ringIdx  Ring Idx
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType AppUtils_EthSciClientRingCfg(
    uint8 ctrlIdx,
    uint8 ringIdx);

/**
 *  \brief Utility configure ring directly function
 *
 *  Configure sciclient for ring via Sciclient
 *
 *  \param ctrlIdx  Controller Idx
 *  \param ringIdx  Ring Idx
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType AppUtils_EthRingCfg(
    uint8 ctrlIdx,
    uint8 ringIdx);

/**
 *  \brief Utility to reset all flow for a controller
 *
 *
 *  \param ctrlIdx  Controller Idx
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType AppUtils_EthSciClientFlowResetAll(
    uint8 ctrlIdx);

/**
 *  \brief Utility to configure all flow for a controller
 *
 *
 *  \param ctrlIdx  Controller Idx
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType AppUtils_EthSciClientFlowConfigAll(
    uint8 ctrlIdx);

/**
 *  \brief Utility to unpair PSIL for TX channel for a controller
 *
 *
 *  \param ctrlIdx  Controller Idx
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType AppUtils_EthSciClientTxChannelUnPairPSILAll(
    uint8 ctrlIdx);

/**
 *  \brief Utility to pair PSIL for TX channel for a controller
 *
 *
 *  \param ctrlIdx  Controller Idx
 *
 *  \return \ref Std_ReturnType
 */
Std_ReturnType AppUtils_EthSciClientTxChannelPairPSILAll(
    uint8 ctrlIdx);

#ifdef __cplusplus
}
#endif

#endif /* APP_UTILS_H_ */
