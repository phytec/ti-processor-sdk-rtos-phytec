/* Copyright (C) 2021 Texas Instruments Incorporated.
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
 *   @file  sdl_osal.h
 *
 *   @brief  This file contains the SDL OSAL API's.
 *
 *   @defgroup SDL_OSAL_API Operating System Abstraction Layer (OSAL)
 *
 *   Provides the APIs for OSAL.
 */

#ifndef SDL_OSAL_H_
#define SDL_OSAL_H_

#include <stdint.h>
#include "sdl_types.h"

/**
@defgroup SDL_OSAL_DATASTRUCT  OSAL Data Structures
@ingroup SDL_OSAL_API
*/
/**
@defgroup SDL_OSAL_FUNCTION  OSAL Functions
@ingroup SDL_OSAL_API
*/


/**
 *  @addtogroup SDL_OSAL_DATASTRUCT
    @{
 *
 */

// Data Structures and Types

/** ---------------------------------------------------------------------------
 * @brief   Prototype for the interrupt callback function
 * ----------------------------------------------------------------------------
 */
typedef void (*pSDL_OSAL_interruptCallbackFunction)(uintptr_t arg);

/** ---------------------------------------------------------------------------
 * @brief   This structure contains the parameters for interrupt registration
 *          through the SDL OSAL interface.
 * ----------------------------------------------------------------------------
 */
typedef struct SDL_OSAL_hwipParams_s
{
    int32_t intNum;
    /**< Interrupt Vector Id */
    pSDL_OSAL_interruptCallbackFunction callback;
    /**< Callback function for the interrupt */
    uintptr_t callbackArg;
    /**< Argument passed to the callback function */
} SDL_OSAL_hwipParams;

typedef void* pSDL_OSAL_hwipHandle;

/** ---------------------------------------------------------------------------
 * @brief   Prototype for the interrupt enable/disable functions
 * ----------------------------------------------------------------------------
 */
typedef int32_t (*pSDL_OSAL_interruptFunction)(int32_t intNum);

/** ---------------------------------------------------------------------------
 * @brief   Prototype for the interrupt registration function
 * ----------------------------------------------------------------------------
 */
typedef pSDL_OSAL_hwipHandle (*pSDL_OSAL_registerFunction)(SDL_OSAL_hwipParams *pParams);

/** ---------------------------------------------------------------------------
 * @brief   Prototype for the interrupt de-register function
 * ----------------------------------------------------------------------------
 */
typedef int32_t (*pSDL_OSAL_deregisterFunction)(pSDL_OSAL_hwipHandle handle);

/** ---------------------------------------------------------------------------
 * @brief   Prototype for the interrupt global disable function
 * ----------------------------------------------------------------------------
 */
typedef int32_t (*pSDL_OSAL_globalDisableInterruptsFunction)(uintptr_t *key);

/** ---------------------------------------------------------------------------
 * @brief   Prototype for the interrupt global restore function
 * ----------------------------------------------------------------------------
 */
typedef int32_t (*pSDL_OSAL_globalRestoreInterruptsFunction)(uintptr_t key);

/** ---------------------------------------------------------------------------
 * @brief   Prototype for the print function
 * ----------------------------------------------------------------------------
 */
typedef void (*pSDL_OSAL_printFunction)(const char *traceBuf);

/** ---------------------------------------------------------------------------
 * @brief   Prototype for the delay function
 * ----------------------------------------------------------------------------
 */
typedef int32_t (*pSDL_OSAL_delayFunction)(int32_t ndelay);

/** ---------------------------------------------------------------------------
 * @brief   Prototype for address translation function
 */
typedef void* (*pSDL_OSAL_addrTranslateFunction)(uint64_t addr, uint32_t size);

/** ---------------------------------------------------------------------------
 * @brief   This structure contains the pointers for the OSAL interfaces 
 *          provided by the application to SDL_OSAL_init.
 * ----------------------------------------------------------------------------
 */
typedef struct SDL_OSAL_Interface_s
{
    /**< Pointer to interrupt enable function */
    pSDL_OSAL_interruptFunction enableInterrupt;
    /**< Pointer to interrupt disable function */
    pSDL_OSAL_interruptFunction disableInterrupt;
    /**< Pointer to interrupt registration function */
    pSDL_OSAL_registerFunction registerInterrupt;
    /**< Pointer to inerrupt de-register function */
    pSDL_OSAL_deregisterFunction deregisterInterrupt;
    /**< Pointer to global interrupt disable function */
    pSDL_OSAL_globalDisableInterruptsFunction globalDisableInterrupts;
    /**< Pointer to global interrupt enable function */
    pSDL_OSAL_globalRestoreInterruptsFunction globalRestoreInterrupts;
    /**< Pointer to print function */
    pSDL_OSAL_printFunction printFxn;
    /**< Pointer to delay function */
    pSDL_OSAL_delayFunction delay;
    /**< Pointer to address translation function */
    pSDL_OSAL_addrTranslateFunction addrTranslate;
} SDL_OSAL_Interface;

/** @} */

// Functions

/**
 *  @addtogroup SDL_OSAL_FUNCTION
    @{
 *
 */

/**
 *  \brief OSAL init
 *
 *  This function initializes the OSAL interface structure with the
 *  functions provided by the application. These functions are application
 *  dependent, so it is required to be passed by the user.
 *
 *  \param osalInterface     [IN]  OSAL interface structure.
 *
 *  \return The SDL error code for the API.
 *                                 If osalInterface is NULL: SDL_EBADARGS
 *                                 If other error happened: SDL_EFAIL
 *                                 Success: SDL_PASS
 */
int32_t SDL_OSAL_init(SDL_OSAL_Interface *osalInterface);

/**
 *  \brief OSAL enable interrupt
 *
 *  This function will enable the specific interrupt number passed.
 *
 *  \param intNum           [IN]  Interrupt Number
 *
 *  \return The SDL error code for the API.
 *                                 If function pointer in interface is NULL: SDL_EBADARGS
 *                                 If other error happened: SDL_EFAIL
 *                                 Success: SDL_PASS
 */
int32_t SDL_OSAL_enableInterrupt(int32_t intNum);

/**
 *  \brief OSAL disable interrupt
 *
 *  This function will disable the specific interrupt number passed.
 *
 *  \param intNum           [IN]  Interrupt Number
 *
 *  \return The SDL error code for the API.
 *                                 If function pointer in interface is NULL: SDL_EBADARGS
 *                                 If other error happened: SDL_EFAIL
 *                                 Success: SDL_PASS
 */
int32_t SDL_OSAL_disableInterrupt(int32_t intNum);

/**
 *  \brief OSAL register interrupt
 *
 *  This function will register the specific interrupt number passed.
 *
 *  \param pParams          [IN]  Parameters for interrupt registration
 *  \param handle           [OUT] Handle for this registered interrupt
 *
 *  \return The SDL error code for the API.
 *                                 If function pointer in interface is NULL: SDL_EBADARGS
 *                                 If other error happened: SDL_EFAIL
 *                                 Success: SDL_PASS
 */
int32_t SDL_OSAL_registerInterrupt(SDL_OSAL_hwipParams *pParams, pSDL_OSAL_hwipHandle *handle);

/**
 *  \brief OSAL deregister interrupt
 *
 *  This function will deregister the specific interrupt number passed.
 *
 *  \param handle           [IN] Handle for the registered interrupt
 *
 *  \return The SDL error code for the API.
 *                                 If function pointer in interface is NULL: SDL_EBADARGS
 *                                 If other error happened: SDL_EFAIL
 *                                 Success: SDL_PASS
 */
int32_t SDL_OSAL_deregisterInterrupt(pSDL_OSAL_hwipHandle handle);

/**
 *  \brief OSAL globally disable interrupts
 *
 *  This function will disable interrupts globally. Interrupts can be
 *  enabled with the globalRestoreInterrupts() function. Usually used for
 *  critical sections.
 *
 *  The returned key is used to restore the context once interrupts are
 *  restored.
 *
 *  \param key              [OUT] key to use when restoring interrupts
 *
 *  \return The SDL error code for the API.
 *                                 If function pointer in interface is NULL: SDL_EBADARGS
 *                                 If other error happened: SDL_EFAIL
 *                                 Success: SDL_PASS
 */
int32_t SDL_OSAL_globalDisableInterrupts(uintptr_t *key);

/**
 *  \brief OSAL globally enable interrupts
 *
 *  This function will enable interrupts globally. Usually used for
 *  critical sections.
 *
 *  The key is used to restore the context.
 *
 *  \param key              [IN] key to use when restoring interrupts
 *
 *  \return The SDL error code for the API.
 *                                 If function pointer in interface is NULL: SDL_EBADARGS
 *                                 If other error happened: SDL_EFAIL
 *                                 Success: SDL_PASS
 */
int32_t SDL_OSAL_globalRestoreInterrupts(uintptr_t key);

/**
 *  \brief OSAL print function
 *
 *  This function prints the provided string
 *
 *  \param traceBuf         [IN] String to be printed
 *
 *  \return None
 */
void SDL_OSAL_printf(const char *traceBuf);

/**
 *  \brief OSAL delay
 *
 *  This function is used assign delay in the function
 *
 *  \param ndelay           [IN] delay in microseconds
 *
 *  \return The SDL error code for the API.
 *                                 If function pointer in interface is NULL: SDL_EBADARGS
 *                                 If other error happened: SDL_EFAIL
 *                                 Success: SDL_PASS
 */

int32_t SDL_OSAL_delay(int32_t ndelay);

/**
 *  \brief OSAL Address translation function
 *
 *  This function is used by the SDL to get a translation for a 64-bit address to
 *  local address space. It is expected that the requested adddress will remain available
 *  at the returned address and not be removed.
 *
 *  \param addr             [IN] Memory address to be translated
 *  \param size             [IN] Size of the memory
 *
 *  \return The translated address or (-1) for failure.
 */
void *SDL_OSAL_addrTranslate(uint64_t addr, uint32_t size);

/** @} */

#endif /* SDL_OSAL_H_ */
