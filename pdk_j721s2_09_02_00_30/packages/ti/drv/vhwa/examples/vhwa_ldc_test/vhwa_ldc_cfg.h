/*
*  Copyright (c) Texas Instruments Incorporated 2019
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
*/

/**
*  \file vhwa_ldc_tirtos_cfg.h
*
*  \brief Configuration for TI RTOS
*/
#ifndef VHWA_LDC_TIRTOS_CFG_
#define VHWA_LDC_TIRTOS_CFG_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#if defined (SOC_J721E)
#define VHWA_LDC_TIRTOS_CFG                                                    \
{                                                                              \
    {                                                                          \
        "TC_LDC_FUNC_000",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[0U]                                                \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_001",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[1U]                                                \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_002",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[2U]                                                \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_003",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        100u,                     /* Repeat Cnt */                               \
        UTRUE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[3U]                                                \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_004",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[4U]                                                \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_005",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[5U]                                                \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_006",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[6U]                                                \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_007",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[7U]                                                \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_008",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[8U]                                                \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_009",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[9U]                                                \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_010",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[10U]                                               \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_011",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[11U]                                               \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_012",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[12U]                                               \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_013",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[13U]                                               \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_014",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[14U]                                               \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_015",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[15U]                                               \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_016",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[16U]                                               \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_017",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[17U]                                               \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_018",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[18U]                                               \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_019",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[19U]                                               \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_020",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[20U]                                               \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_021",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[21U]                                               \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_022",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[22U]                                               \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_023",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[23U]                                               \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_024",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[24U]                                               \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_025",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[25U]                                               \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_026",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[26U]                                               \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_027",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[27U]                                               \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_028",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[28U]                                               \
        },                                                                     \
        UFALSE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_029",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[29U]                                               \
        },                                                                     \
        UFALSE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_030",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[30U]                                               \
        },                                                                     \
        UFALSE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_031",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[31U]                                               \
        },                                                                     \
        UFALSE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_033",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[32U]                                               \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_034",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[33U]                                               \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_035",      /* Test Name */                                \
        4u,                     /* Num Handles */                              \
        1000u,                  /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[0U],                                              \
            &gAppLdcTestCfg[14U],                                              \
            &gAppLdcTestCfg[16U],                                              \
            &gAppLdcTestCfg[17U]                                               \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
}
#endif

#if defined (SOC_J721S2) || defined (SOC_J784S4)
#define VHWA_LDC_TIRTOS_CFG                                                    \
{                                                                              \
    {                                                                          \
        "TC_LDC_FUNC_000",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[3U]                                                \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_001",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[35U]                                                \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_002",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[36U]                                                \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_003",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        100u,                     /* Repeat Cnt */                               \
        UTRUE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[37U]                                                \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_004",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[38U]                                                \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_005",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[39U]                                                \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_006",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[40U]                                                \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_007",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[41U]                                                \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_008",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[42U]                                                \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_009",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[43U]                                                \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_010",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[44U]                                               \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_011",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[45U]                                               \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_012",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[46U]                                               \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_013",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[47U]                                               \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_014",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[48U]                                               \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_015",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[49U]                                               \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_016",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[50U]                                               \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_017",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[51U]                                               \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_018",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[52U]                                               \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
    {                                                                          \
        "TC_LDC_FUNC_019",      /* Test Name */                                \
        1u,                     /* Num Handles */                              \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        /* Test Configuration */                                               \
        {                                                                      \
            &gAppLdcTestCfg[53U]                                               \
        },                                                                     \
        UTRUE                                                                   \
    },                                                                         \
}
#endif

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None*/

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */

#endif /*VHWA_LDC_TIRTOS_CFG_*/




;
