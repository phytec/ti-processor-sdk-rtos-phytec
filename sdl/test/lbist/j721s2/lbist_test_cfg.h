/*
 *   Copyright (c) Texas Instruments Incorporated 2020-2021
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
 *  \file     lbist_test_cfg.h
 *
 *  \brief    This file contains LBIST test configuration
 *
 *  \details  LBIST Test configuration
 **/

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "lbist_test_func.h"
#include <src/sdl/sdl_lbist.h>

/* #define DEBUG */

/* ========================================================================== */
/*                                Macros                                      */
/* ========================================================================== */

/* Note this is just local test array index */

#define LBIST_HWPOST_INST_SMS_INDEX     0
#define LBIST_HWPOST_INST_MCU_INDEX     1
#define LBIST_INST_MAINR5F0_INDEX       2
#define LBIST_INST_MAINR5F1_INDEX       3
#define LBIST_INST_C7X0_INDEX           4
#define LBIST_INST_C7X1_INDEX           5
#define LBIST_INST_VPAC0_INDEX          6
#define LBIST_INST_DMPAC_INDEX          7
#define LBIST_INST_A72_0_INDEX          8

#define LBIST_NUM_TEST_CORES          (LBIST_INST_A72_0_INDEX+1)

#define LBIST_MAX_CORE_INDEX          (LBIST_INST_A72_0_INDEX)

#define LBIST_MAX_TIMEOUT_VALUE       (10000000u)

/* HW POST core definitions */
#define LBIST_POST_CORE_SMS           LBIST_HWPOST_INST_SMS_INDEX
#define LBIST_POST_CORE_MCU           LBIST_HWPOST_INST_MCU_INDEX

#define LBIST_POST_CORE_MAX               LBIST_POST_CORE_MCU

/* HW POST run status definitions */
#define LBIST_POST_COMPLETED_SUCCESS      (0u)
#define LBIST_POST_COMPLETED_FAILURE      (1u)
#define LBIST_POST_ATTEMPTED_TIMEOUT      (2u)
#define LBIST_POST_NOT_RUN                (3u)

#define A72_NUM_AUX_DEVICES               3

LBIST_TestHandle_t* LBIST_getTestHandleArray(void);

extern LBIST_TestHandle_t LBIST_TestHandleArray[SDL_LBIST_NUM_INSTANCES];

int32_t LBIST_runPostLbistCheck(uint32_t hwPostCoreNum,
                                SDL_lbistRegs *pLBISTRegs,
                                uint32_t *pLBISTSig);

/* Nothing past this point */
