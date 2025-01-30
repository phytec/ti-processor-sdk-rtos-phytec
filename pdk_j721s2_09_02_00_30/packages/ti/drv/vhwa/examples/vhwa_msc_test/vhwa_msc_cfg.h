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
*  \file vhwa_msc_tirtos_cfg.h
*
*  \brief Configuration for TI RTOS
*/
#ifndef VHWA_MSC_TIRTOS_CFG_
#define VHWA_MSC_TIRTOS_CFG_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#if defined(SOC_J721E) || defined(SOC_J721S2) || defined (SOC_J784S4)
#define VHWA_MSC_TIRTOS_CFG                                                    \
{                                                                              \
    /* 0 - Thread 0, 1-in 10-out 1920x1080 YUV 12bit Packed input/output */    \
    {                                                                          \
        "TC_MSC_FUNC_TC00",                                                    \
        1u,                     /* NumHandles */                               \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        {&gAppMscTestCfg[0U] },                                                \
        UTRUE,                                                                  \
    },                                                                         \
    /* 1 - Thread 1, 1-in 10-out 1920x1080 YUV 12bit Packed input/output */    \
    {                                                                          \
        "TC_MSC_FUNC_TC01",                                                    \
        1u,                     /* NumHandles */                               \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        {&gAppMscTestCfg[1U] },                                                \
        UTRUE,                                                                  \
    },                                                                         \
    /* 2 - Thread 0, 1-in 10-out 1920x1080 YUV 12bit Unpacked input/output */  \
    {                                                                          \
        "TC_MSC_FUNC_TC02",                                                    \
        1u,                     /* NumHandles */                               \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        {&gAppMscTestCfg[2U] },                                                \
        UTRUE,                                                                  \
    },                                                                         \
    /* 3 - Thread 0, 1-in 10-out 1920x1080 12bit Packed input to 8b */         \
    {                                                                          \
        "TC_MSC_FUNC_TC03",                                                    \
        1u,                     /* NumHandles */                               \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        {&gAppMscTestCfg[3U] },                                                \
        UTRUE,                                                                  \
    },                                                                         \
    /* 4 - Thread 0, 1-in 10-out 1920x1080 8b to 8b */                         \
    {                                                                          \
        "TC_MSC_FUNC_TC04",                                                    \
        1u,                     /* NumHandles */                               \
        100u,                     /* Repeat Cnt */                               \
        UTRUE,                  /* Is Performance Test */                      \
        {&gAppMscTestCfg[4U] },                                                \
        UTRUE,                                                                  \
    },                                                                         \
    /* 5 - Thread 0, 1-in 10-out 1920x1080 8b to 12b packed */                 \
    {                                                                          \
        "TC_MSC_FUNC_TC05",                                                    \
        1u,                     /* NumHandles */                               \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        {&gAppMscTestCfg[5U] },                                                \
        UTRUE,                                                                  \
    },                                                                         \
    /* 6 - Thread 0, 1-in 10-out 1920x1080 ROI 12bP */                         \
    {                                                                          \
        "TC_MSC_FUNC_TC06",                                                    \
        1u,                     /* NumHandles */                               \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        {&gAppMscTestCfg[8U] },                                                \
        UTRUE,                                                                  \
    },                                                                         \
    /* 7 - Thread 0, 1-in 10-out 1920x1080  SP 12b P */                        \
    {                                                                          \
        "TC_MSC_FUNC_TC07",                                                    \
        1u,                     /* NumHandles */                               \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        {&gAppMscTestCfg[13U] },                                               \
        UTRUE,                                                                  \
    },                                                                         \
    /* 8 - Thread 0, 1-in 10-out 1920x1080 64 Phase 12b P */                   \
    {                                                                          \
        "TC_MSC_FUNC_TC08",                                                    \
        1u,                     /* NumHandles */                               \
        1u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        {&gAppMscTestCfg[14U] },                                               \
        UTRUE,                                                                  \
    },                                                                         \
    /* 9 - Thread 0/1, 1-in 5-out 1920x1080 YUV 12bit Packed input */          \
    {                                                                          \
        "TC_MSC_FUNC_TC09",                                                    \
        2u,                     /* NumHandles */                               \
        2u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        {&gAppMscTestCfg[6U],                                                  \
         &gAppMscTestCfg[7U]},                                                 \
        UTRUE,                                                                  \
    },                                                                         \
}
#endif
/* Below config is currently disabled due to limitation at SDK */
#if 0
#if defined(SOC_J721S2) || defined (SOC_J784S4)
#define VHWA_MSC_TIRTOS_CFG                                                    \
{                                                                              \
    /* 0 - Thread 0, 1-in 5-out 1920x1080 YUV 12bit Packed input/output */    \
    {                                                                          \
        "TC_MSC_FUNC_TC00",                                                    \
        1u,                     /* NumHandles */                               \
        2u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        {&gAppMscTestCfg[0U] },                                                \
        UTRUE,                                                                  \
    },                                                                         \
    /* 1 - Thread 1, 1-in 5-out 1920x1080 YUV 12bit Packed input/output */    \
    {                                                                          \
        "TC_MSC_FUNC_TC01",                                                    \
        1u,                     /* NumHandles */                               \
        2u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        {&gAppMscTestCfg[1U] },                                                \
        UTRUE,                                                                  \
    },                                                                         \
    /* 2 - Thread 0, 1-in 5-out 1920x1080 8bit Packed input/output */    \
    {                                                                          \
        "TC_MSC_FUNC_TC02",                                                    \
        1u,                     /* NumHandles */                               \
        2u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        {&gAppMscTestCfg[2U] },                                                \
        UTRUE,                                                                  \
    },                                                                         \
    /* 3 - Thread 0, 1-in 5-out 1920x1080 Y12 UV8 */    \
    {                                                                          \
        "TC_MSC_FUNC_TC03",                                                    \
        1u,                     /* NumHandles */                               \
        2u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        {&gAppMscTestCfg[3U] },                                                \
        UTRUE,                                                                  \
    },                                                                         \
    /* 4 - Thread 0, 1-in 5-out 1920x1080 Y8 UV12 */    \
    {                                                                          \
        "TC_MSC_FUNC_TC04",                                                    \
        1u,                     /* NumHandles */                               \
        2u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        {&gAppMscTestCfg[4U] },                                                \
        UTRUE,                                                                  \
    },                                                                         \
    /* 5 - Thread 0, 1-in 5-out 1920x1080 Y8 Y8 */    \
    {                                                                          \
        "TC_MSC_FUNC_TC05",                                                    \
        1u,                     /* NumHandles */                               \
        2u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        {&gAppMscTestCfg[5U] },                                                \
        UTRUE,                                                                  \
    },                                                                         \
    /* 6 - Thread 0, 1-in 5-out 1920x1080 YUV422SP 12bit Packed input/output */    \
    {                                                                          \
        "TC_MSC_FUNC_TC06",                                                    \
        1u,                     /* NumHandles */                               \
        2u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        {&gAppMscTestCfg[6U] },                                                \
        UTRUE,                                                                  \
    },                                                                         \
    /* 7 - Thread 0, 1-in 4-out 1920x1080 YUV422I 8bit input/output */    \
    {                                                                          \
        "TC_MSC_FUNC_TC07",                                                    \
        1u,                     /* NumHandles */                               \
        2u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        {&gAppMscTestCfg[7U] },                                                \
        UTRUE,                                                                  \
    },                                                                         \
    /* 8 - Thread 0, 11-in 5-out 1920x1080 P1 +P2P3 Interleaved */    \
    {                                                                          \
        "TC_MSC_FUNC_TC08",                                                    \
        1u,                     /* NumHandles */                               \
        2u,                     /* Repeat Cnt */                               \
        UFALSE,                  /* Is Performance Test */                      \
        {&gAppMscTestCfg[8U] },                                                \
        UTRUE,                                                                  \
    },                                                                         \
}
#endif
#endif

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None*/

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */

#endif /*VHWA_MSC_TIRTOS_CFG_*/




;
