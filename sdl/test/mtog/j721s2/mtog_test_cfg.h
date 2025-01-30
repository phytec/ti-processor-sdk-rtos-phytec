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
 *  \file     mtog_test_cfg.h
 *
 *  \brief    This file contains Master TOG test configuration
 *
 *  \details  MTOG Test configuration
 **/

#ifndef MTOG_TEST_CFG_H
#define MTOG_TEST_CFG_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "mtog_test_func.h"

/* #define DEBUG */

/* ========================================================================== */
/*                                Macros                                      */
/* ========================================================================== */
#define MTOG_MAX_MAIN_INSTANCE                   15U
#define MTOG_MAX_MCU_INSTANCE                    9U
/*Instance 0-23 */
#define MTOG_MAX_INSTANCE                        24U
#define MTOG_MAX_TEST_TIMEOUT_VALUE             (1000000000u)
#define MCU_ESM_BASE                            (SDL_MCU_ESM0_CFG_BASE)
#define MAIN_ESM_BASE                           (SDL_ESM0_CFG_BASE)

extern MTOG_TestHandle_t MTOG_TestHandleArray[MTOG_MAX_INSTANCE];

#endif /* MTOG_TEST_CFG_H */

/* Nothing past this point */
