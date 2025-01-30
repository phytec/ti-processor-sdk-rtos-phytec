/*
 *   Copyright (c) Texas Instruments Incorporated 2022
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
 *  \file     msmc_test_main.h
 *
 *  \brief    This file contains MSMC main test defines.
 *
 *  \details  MSMC unit tests
 **/
#ifndef MSMC_TEST_MAIN_H
#define MSMC_TEST_MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <stdint.h>
#include <string.h>
#include <sdl_types.h>
#include <ti/drv/uart/UART.h>
#include <ti/drv/uart/UART_stdio.h>
#include <ti/board/board.h>
#include <src/sdl/sdl_msmc.h>
#if defined (SOC_J721E)
#include "src/sdl/misc/soc/j721e/sdl_soc_msmc.h"
#endif /* SOC_J721E */

#if defined (SOC_J7200)
#include "src/sdl/misc/soc/j7200/sdl_soc_msmc.h"
#endif /* SOC_J7200 */

#if defined (SOC_J721S2)
#include "src/sdl/misc/soc/j721s2/sdl_soc_msmc.h"
#endif /* SOC_J721S2 */

#if defined (SOC_J784S4)
#include "src/sdl/misc/soc/j784s4/sdl_soc_msmc.h"
#endif /* SOC_J784S4 */
/* ========================================================================== */
/*                                Macros                                      */
/* ========================================================================== */
#define  MSMC_SDL_API_TEST_ID      (0U)
#define  MSMC_SDL_ERROR_TEST_ID    (1U)
#define  MSMC_TOTAL_NUM_TESTS      (2U)

/* ========================================================================== */
/*                 External Function Declarations                             */
/* ========================================================================== */

extern int32_t MSMC_API_test(void);

#ifdef __cplusplus
}
#endif

#endif /* MSMC_TEST_MAIN_H */

/* Nothing past this point */
