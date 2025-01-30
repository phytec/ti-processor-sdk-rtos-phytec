/**
 * @file  sdl_ip_rat.h
 *
 * @brief
 *  Header file containing various enumerations, structure definitions and function
 *  declarations for the KSBUS RAT Ip.
 *      =
 *  @n   (C) Copyright 2016-2020, Texas Instruments, Inc.
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
#ifndef SDL_IP_RAT_H_
#define SDL_IP_RAT_H_


#include <sdl_types.h>
#include <tistdtypes.h>
#include <stdint.h>
#include <stdbool.h>
#include <src/sdl/hw_types.h>
#include <src/sdl/sdlr.h>


#ifdef __cplusplus
extern "C"
{
#endif

/**
@ingroup SDL_IP_MODULE
@defgroup SDL_RAT_IP_API RAT Low-Level API
*/

/**************************************************************************
* RAT static Registers Structure
**************************************************************************/

typedef struct {
    volatile uint32_t RAT_CTRL_k;                      /* Region Control Register */
    volatile uint32_t RAT_BASE_k;                      /* Region Base Register */
    volatile uint32_t RAT_TRANS_L_k;                   /* Region Translated Lower Address */
    volatile uint32_t RAT_TRANS_U_k;                   /* Region Translated Upper Address */
    volatile uint32_t RAT_EXCEPTION_LOGGING_CONTROL;   /* RAT Error Log control register */
    volatile uint32_t RAT_EXCEPTION_ENABLE_SET;        /* RAT Exception Logging Interrupt Enable Set Register */
} SDL_ratStaticRegs;

/**************************************************************************
* Register Overlay Structure
**************************************************************************/

typedef struct {
    volatile uint32_t CTRL;                      /* Region Control Register */
    volatile uint32_t BASE;                      /* Region Base Register */
    volatile uint32_t TRANS_L;                   /* Region Translated Lower Address */
    volatile uint32_t TRANS_U;                   /* Region Translated Upper Address */
} SDL_ratRegs_region;

/** ---------------------------------------------------------------------------
 * @brief   This structure contains information required to configure an
 *          address translation region.
 *
 * ----------------------------------------------------------------------------
 */
typedef struct
{
    uint32_t    baseAddress;
    /** The translated address returned on a successful match */
    /** The size of the region (in bytes). If this value is not a power-of-2, it is automatically rounded-up to the next power-of-2 size. */
    uint64_t    sizeInBytes;
    /** The 32-bit base address used in the address comparison */
    uint64_t    translatedAddress;
} SDL_ratTranslationCfgInfo;


typedef struct {
    volatile uint32_t PID;                       /* Revision Register */
    volatile uint32_t CONFIG;                    /* Config Register */
    volatile uint8_t  Resv_32[24];
    SDL_ratRegs_region REGION[16];
    volatile uint8_t  Resv_2052[1764];
    volatile uint32_t DESTINATION_ID;            /* Destination ID Register */
    volatile uint8_t  Resv_2080[24];
    volatile uint32_t EXCEPTION_LOGGING_CONTROL;   /* Exception Logging Control Register */
    volatile uint32_t EXCEPTION_LOGGING_HEADER0;   /* Exception Logging Header 0 Register */
    volatile uint32_t EXCEPTION_LOGGING_HEADER1;   /* Exception Logging Header 1 Register */
    volatile uint32_t EXCEPTION_LOGGING_DATA0;   /* Exception Logging Data 0 Register */
    volatile uint32_t EXCEPTION_LOGGING_DATA1;   /* Exception Logging Data 1 Register */
    volatile uint32_t EXCEPTION_LOGGING_DATA2;   /* Exception Logging Data 2 Register */
    volatile uint32_t EXCEPTION_LOGGING_DATA3;   /* Exception Logging Data 3 Register */
    volatile uint8_t  Resv_2112[4];
    volatile uint32_t EXCEPTION_PEND_SET;        /* Exception Logging Interrupt Pending Set Register */
    volatile uint32_t EXCEPTION_PEND_CLEAR;      /* Exception Logging Interrupt Pending Clear Register */
    volatile uint32_t EXCEPTION_ENABLE_SET;      /* Exception Logging Interrupt Enable Set Register */
    volatile uint32_t EXCEPTION_ENABLE_CLEAR;    /* Exception Logging Interrupt Enable Clear Register */
    volatile uint32_t EOI_REG;                   /* EOI Register */
} SDL_ratRegs;


/**************************************************************************
* Register Macros
**************************************************************************/

#define SDL_RAT_PID                                                            (0x00000000U)
#define SDL_RAT_CONFIG                                                         (0x00000004U)
#define SDL_RAT_REGION_CTRL(REGION)                                            (0x00000020U+((REGION)*0x10U))
#define SDL_RAT_REGION_BASE(REGION)                                            (0x00000024U+((REGION)*0x10U))
#define SDL_RAT_REGION_TRANS_L(REGION)                                         (0x00000028U+((REGION)*0x10U))
#define SDL_RAT_REGION_TRANS_U(REGION)                                         (0x0000002CU+((REGION)*0x10U))
#define SDL_RAT_DESTINATION_ID                                                 (0x00000804U)
#define SDL_RAT_EXCEPTION_LOGGING_CONTROL                                      (0x00000820U)
#define SDL_RAT_EXCEPTION_LOGGING_HEADER0                                      (0x00000824U)
#define SDL_RAT_EXCEPTION_LOGGING_HEADER1                                      (0x00000828U)
#define SDL_RAT_EXCEPTION_LOGGING_DATA0                                        (0x0000082CU)
#define SDL_RAT_EXCEPTION_LOGGING_DATA1                                        (0x00000830U)
#define SDL_RAT_EXCEPTION_LOGGING_DATA2                                        (0x00000834U)
#define SDL_RAT_EXCEPTION_LOGGING_DATA3                                        (0x00000838U)
#define SDL_RAT_EXCEPTION_PEND_SET                                             (0x00000840U)
#define SDL_RAT_EXCEPTION_PEND_CLEAR                                           (0x00000844U)
#define SDL_RAT_EXCEPTION_ENABLE_SET                                           (0x00000848U)
#define SDL_RAT_EXCEPTION_ENABLE_CLEAR                                         (0x0000084CU)
#define SDL_RAT_EOI_REG                                                        (0x00000850U)

/**************************************************************************
* Field Definition Macros
**************************************************************************/


/* CTRL */

#define SDL_RAT_REGION_CTRL_EN_MASK                                            (0x80000000U)
#define SDL_RAT_REGION_CTRL_EN_SHIFT                                           (0x0000001FU)
#define SDL_RAT_REGION_CTRL_EN_MAX                                             (0x00000001U)

#define SDL_RAT_REGION_CTRL_SIZE_MASK                                          (0x0000003FU)
#define SDL_RAT_REGION_CTRL_SIZE_SHIFT                                         (0x00000000U)
#define SDL_RAT_REGION_CTRL_SIZE_MAX                                           (0x0000003FU)

/* BASE */

#define SDL_RAT_REGION_BASE_BASE_MASK                                          (0xFFFFFFFFU)
#define SDL_RAT_REGION_BASE_BASE_SHIFT                                         (0x00000000U)
#define SDL_RAT_REGION_BASE_BASE_MAX                                           (0xFFFFFFFFU)

/* TRANS_L */

#define SDL_RAT_REGION_TRANS_L_LOWER_MASK                                      (0xFFFFFFFFU)
#define SDL_RAT_REGION_TRANS_L_LOWER_SHIFT                                     (0x00000000U)
#define SDL_RAT_REGION_TRANS_L_LOWER_MAX                                       (0xFFFFFFFFU)

/* TRANS_U */

#define SDL_RAT_REGION_TRANS_U_UPPER_MASK                                      (0x0000FFFFU)
#define SDL_RAT_REGION_TRANS_U_UPPER_SHIFT                                     (0x00000000U)
#define SDL_RAT_REGION_TRANS_U_UPPER_MAX                                       (0x0000FFFFU)

/* PID */

#define SDL_RAT_PID_MINOR_MASK                                                 (0x0000003FU)
#define SDL_RAT_PID_MINOR_SHIFT                                                (0x00000000U)
#define SDL_RAT_PID_MINOR_MAX                                                  (0x0000003FU)

#define SDL_RAT_PID_CUSTOM_MASK                                                (0x000000C0U)
#define SDL_RAT_PID_CUSTOM_SHIFT                                               (0x00000006U)
#define SDL_RAT_PID_CUSTOM_MAX                                                 (0x00000003U)

#define SDL_RAT_PID_MAJOR_MASK                                                 (0x00000700U)
#define SDL_RAT_PID_MAJOR_SHIFT                                                (0x00000008U)
#define SDL_RAT_PID_MAJOR_MAX                                                  (0x00000007U)

#define SDL_RAT_PID_RTL_MASK                                                   (0x0000F800U)
#define SDL_RAT_PID_RTL_SHIFT                                                  (0x0000000BU)
#define SDL_RAT_PID_RTL_MAX                                                    (0x0000001FU)

#define SDL_RAT_PID_FUNC_MASK                                                  (0x0FFF0000U)
#define SDL_RAT_PID_FUNC_SHIFT                                                 (0x00000010U)
#define SDL_RAT_PID_FUNC_MAX                                                   (0x00000FFFU)

#define SDL_RAT_PID_BU_MASK                                                    (0x30000000U)
#define SDL_RAT_PID_BU_SHIFT                                                   (0x0000001CU)
#define SDL_RAT_PID_BU_MAX                                                     (0x00000003U)

#define SDL_RAT_PID_SCHEME_MASK                                                (0xC0000000U)
#define SDL_RAT_PID_SCHEME_SHIFT                                               (0x0000001EU)
#define SDL_RAT_PID_SCHEME_MAX                                                 (0x00000003U)

/* CONFIG */

#define SDL_RAT_CONFIG_REGIONS_MASK                                            (0x000000FFU)
#define SDL_RAT_CONFIG_REGIONS_SHIFT                                           (0x00000000U)
#define SDL_RAT_CONFIG_REGIONS_MAX                                             (0x000000FFU)

#define SDL_RAT_CONFIG_ADDRS_MASK                                              (0x0000FF00U)
#define SDL_RAT_CONFIG_ADDRS_SHIFT                                             (0x00000008U)
#define SDL_RAT_CONFIG_ADDRS_MAX                                               (0x000000FFU)

#define SDL_RAT_CONFIG_ADDR_WIDTH_MASK                                         (0x00FF0000U)
#define SDL_RAT_CONFIG_ADDR_WIDTH_SHIFT                                        (0x00000010U)
#define SDL_RAT_CONFIG_ADDR_WIDTH_MAX                                          (0x000000FFU)

/* DESTINATION_ID */

#define SDL_RAT_DESTINATION_ID_DEST_ID_MASK                                    (0x000000FFU)
#define SDL_RAT_DESTINATION_ID_DEST_ID_SHIFT                                   (0x00000000U)
#define SDL_RAT_DESTINATION_ID_DEST_ID_MAX                                     (0x000000FFU)

/* EXCEPTION_LOGGING_CONTROL */

#define SDL_RAT_EXCEPTION_LOGGING_CONTROL_DISABLE_F_MASK                       (0x00000001U)
#define SDL_RAT_EXCEPTION_LOGGING_CONTROL_DISABLE_F_SHIFT                      (0x00000000U)
#define SDL_RAT_EXCEPTION_LOGGING_CONTROL_DISABLE_F_MAX                        (0x00000001U)

#define SDL_RAT_EXCEPTION_LOGGING_CONTROL_DISABLE_INTR_MASK                    (0x00000002U)
#define SDL_RAT_EXCEPTION_LOGGING_CONTROL_DISABLE_INTR_SHIFT                   (0x00000001U)
#define SDL_RAT_EXCEPTION_LOGGING_CONTROL_DISABLE_INTR_MAX                     (0x00000001U)

/* EXCEPTION_LOGGING_HEADER0 */

#define SDL_RAT_EXCEPTION_LOGGING_HEADER0_DEST_ID_MASK                         (0x000000FFU)
#define SDL_RAT_EXCEPTION_LOGGING_HEADER0_DEST_ID_SHIFT                        (0x00000000U)
#define SDL_RAT_EXCEPTION_LOGGING_HEADER0_DEST_ID_MAX                          (0x000000FFU)

#define SDL_RAT_EXCEPTION_LOGGING_HEADER0_SRC_ID_MASK                          (0x00FFFF00U)
#define SDL_RAT_EXCEPTION_LOGGING_HEADER0_SRC_ID_SHIFT                         (0x00000008U)
#define SDL_RAT_EXCEPTION_LOGGING_HEADER0_SRC_ID_MAX                           (0x0000FFFFU)

#define SDL_RAT_EXCEPTION_LOGGING_HEADER0_TYPE_F_MASK                          (0xFF000000U)
#define SDL_RAT_EXCEPTION_LOGGING_HEADER0_TYPE_F_SHIFT                         (0x00000018U)
#define SDL_RAT_EXCEPTION_LOGGING_HEADER0_TYPE_F_MAX                           (0x000000FFU)

/* EXCEPTION_LOGGING_HEADER1 */

#define SDL_RAT_EXCEPTION_LOGGING_HEADER1_CODE_MASK                            (0x00FF0000U)
#define SDL_RAT_EXCEPTION_LOGGING_HEADER1_CODE_SHIFT                           (0x00000010U)
#define SDL_RAT_EXCEPTION_LOGGING_HEADER1_CODE_MAX                             (0x000000FFU)

#define SDL_RAT_EXCEPTION_LOGGING_HEADER1_GROUP_MASK                           (0xFF000000U)
#define SDL_RAT_EXCEPTION_LOGGING_HEADER1_GROUP_SHIFT                          (0x00000018U)
#define SDL_RAT_EXCEPTION_LOGGING_HEADER1_GROUP_MAX                            (0x000000FFU)

/* EXCEPTION_LOGGING_DATA0 */

#define SDL_RAT_EXCEPTION_LOGGING_DATA0_ADDR_L_MASK                            (0xFFFFFFFFU)
#define SDL_RAT_EXCEPTION_LOGGING_DATA0_ADDR_L_SHIFT                           (0x00000000U)
#define SDL_RAT_EXCEPTION_LOGGING_DATA0_ADDR_L_MAX                             (0xFFFFFFFFU)

/* EXCEPTION_LOGGING_DATA1 */

#define SDL_RAT_EXCEPTION_LOGGING_DATA1_ADDR_H_MASK                            (0x0000FFFFU)
#define SDL_RAT_EXCEPTION_LOGGING_DATA1_ADDR_H_SHIFT                           (0x00000000U)
#define SDL_RAT_EXCEPTION_LOGGING_DATA1_ADDR_H_MAX                             (0x0000FFFFU)

/* EXCEPTION_LOGGING_DATA2 */

#define SDL_RAT_EXCEPTION_LOGGING_DATA2_PRIV_ID_MASK                           (0x000000FFU)
#define SDL_RAT_EXCEPTION_LOGGING_DATA2_PRIV_ID_SHIFT                          (0x00000000U)
#define SDL_RAT_EXCEPTION_LOGGING_DATA2_PRIV_ID_MAX                            (0x000000FFU)

#define SDL_RAT_EXCEPTION_LOGGING_DATA2_SECURE_MASK                            (0x00000100U)
#define SDL_RAT_EXCEPTION_LOGGING_DATA2_SECURE_SHIFT                           (0x00000008U)
#define SDL_RAT_EXCEPTION_LOGGING_DATA2_SECURE_MAX                             (0x00000001U)

#define SDL_RAT_EXCEPTION_LOGGING_DATA2_PRIV_MASK                              (0x00000200U)
#define SDL_RAT_EXCEPTION_LOGGING_DATA2_PRIV_SHIFT                             (0x00000009U)
#define SDL_RAT_EXCEPTION_LOGGING_DATA2_PRIV_MAX                               (0x00000001U)

#define SDL_RAT_EXCEPTION_LOGGING_DATA2_CACHEABLE_MASK                         (0x00000400U)
#define SDL_RAT_EXCEPTION_LOGGING_DATA2_CACHEABLE_SHIFT                        (0x0000000AU)
#define SDL_RAT_EXCEPTION_LOGGING_DATA2_CACHEABLE_MAX                          (0x00000001U)

#define SDL_RAT_EXCEPTION_LOGGING_DATA2_DEBUG_MASK                             (0x00000800U)
#define SDL_RAT_EXCEPTION_LOGGING_DATA2_DEBUG_SHIFT                            (0x0000000BU)
#define SDL_RAT_EXCEPTION_LOGGING_DATA2_DEBUG_MAX                              (0x00000001U)

#define SDL_RAT_EXCEPTION_LOGGING_DATA2_READ_MASK                              (0x00001000U)
#define SDL_RAT_EXCEPTION_LOGGING_DATA2_READ_SHIFT                             (0x0000000CU)
#define SDL_RAT_EXCEPTION_LOGGING_DATA2_READ_MAX                               (0x00000001U)

#define SDL_RAT_EXCEPTION_LOGGING_DATA2_WRITE_MASK                             (0x00002000U)
#define SDL_RAT_EXCEPTION_LOGGING_DATA2_WRITE_SHIFT                            (0x0000000DU)
#define SDL_RAT_EXCEPTION_LOGGING_DATA2_WRITE_MAX                              (0x00000001U)

#define SDL_RAT_EXCEPTION_LOGGING_DATA2_ROUTEID_MASK                           (0x0FFF0000U)
#define SDL_RAT_EXCEPTION_LOGGING_DATA2_ROUTEID_SHIFT                          (0x00000010U)
#define SDL_RAT_EXCEPTION_LOGGING_DATA2_ROUTEID_MAX                            (0x00000FFFU)

/* EXCEPTION_LOGGING_DATA3 */

#define SDL_RAT_EXCEPTION_LOGGING_DATA3_BYTECNT_MASK                           (0x000003FFU)
#define SDL_RAT_EXCEPTION_LOGGING_DATA3_BYTECNT_SHIFT                          (0x00000000U)
#define SDL_RAT_EXCEPTION_LOGGING_DATA3_BYTECNT_MAX                            (0x000003FFU)

/* EXCEPTION_PEND_SET */

#define SDL_RAT_EXCEPTION_PEND_SET_PEND_SET_MASK                               (0x00000001U)
#define SDL_RAT_EXCEPTION_PEND_SET_PEND_SET_SHIFT                              (0x00000000U)
#define SDL_RAT_EXCEPTION_PEND_SET_PEND_SET_MAX                                (0x00000001U)

/* EXCEPTION_PEND_CLEAR */

#define SDL_RAT_EXCEPTION_PEND_CLEAR_PEND_CLR_MASK                             (0x00000001U)
#define SDL_RAT_EXCEPTION_PEND_CLEAR_PEND_CLR_SHIFT                            (0x00000000U)
#define SDL_RAT_EXCEPTION_PEND_CLEAR_PEND_CLR_MAX                              (0x00000001U)

/* EXCEPTION_ENABLE_SET */

#define SDL_RAT_EXCEPTION_ENABLE_SET_ENABLE_SET_MASK                           (0x00000001U)
#define SDL_RAT_EXCEPTION_ENABLE_SET_ENABLE_SET_SHIFT                          (0x00000000U)
#define SDL_RAT_EXCEPTION_ENABLE_SET_ENABLE_SET_MAX                            (0x00000001U)

/* EXCEPTION_ENABLE_CLEAR */

#define SDL_RAT_EXCEPTION_ENABLE_CLEAR_ENABLE_CLR_MASK                         (0x00000001U)
#define SDL_RAT_EXCEPTION_ENABLE_CLEAR_ENABLE_CLR_SHIFT                        (0x00000000U)
#define SDL_RAT_EXCEPTION_ENABLE_CLEAR_ENABLE_CLR_MAX                          (0x00000001U)

/* EOI_REG */

#define SDL_RAT_EOI_REG_EOI_WR_MASK                                            (0x0000FFFFU)
#define SDL_RAT_EOI_REG_EOI_WR_SHIFT                                           (0x00000000U)
#define SDL_RAT_EOI_REG_EOI_WR_MAX                                             (0x0000FFFFU)

/**
 *
 *
 * @section Introduction
 *
 *  This is the SDL-FL API documentation for the Region-based Address Translation
 *  (RAT) module.
 *
 * @subsection References
 *    - ksbus_rat Module Specification, version 1.0.8
 *
 *
 */
/**
@defgroup SDL_RAT_DATASTRUCT  RAT Data Structures
@ingroup SDL_RAT_IP_API
*/
/**
@defgroup SDL_RAT_FUNCTION  RAT Functions
@ingroup SDL_RAT_IP_API
*/
/**
@defgroup SDL_RAT_ENUM RAT Enumerated Data Types
@ingroup SDL_RAT_IP_API
*/

/**
 *  @addtogroup SDL_RAT_ENUM
    @{
 *
 */

/** ---------------------------------------------------------------------------
 * @brief This enumerator defines the types of possible RAT interrupt sources
 *
 *  \anchor SDL_RAT_intrSrcTypes
 *  \name RAT interrupt source types
 *
 *  @{
 * ----------------------------------------------------------------------------
 */
typedef uint32_t SDL_RAT_intrSrcTypes;
    /** Exception interrupt */
#define SDL_RAT_INTR_SRC_EXCEPTION  ((uint32_t) 0U)
    /** Denotes an invalid error type */
#define SDL_RAT_INTR_SRC_INVALID    ((uint32_t) 1U)
/* @} */

/** @} */

/**
 *  @addtogroup SDL_RAT_DATASTRUCT
    @{
 *
 */

/** ---------------------------------------------------------------------------
 * @brief   This structure contains exception status information returned by the
 *          SDL_RAT_getExceptionInfo function.
 *
 * ----------------------------------------------------------------------------
 */
typedef struct
{
    /** Identifies the exception type (for the RAT, this field should equal 4) */
    uint32_t    type;
    /** Address */
    uint64_t    address;
    /** Group */
    uint32_t    group;
    /** Code */
    uint32_t    code;
    /** Source ID */
    uint32_t    srcId;
    /** Destination ID */
    uint32_t    destId;
    /** Route ID */
    uint32_t    routeId;
    /** Priv ID */
    uint32_t    privId;
    /** Byte Count */
    uint32_t    byteCnt;
    /** Write */
    bool        bWrite;
    /** Read */
    bool        bRead;
    /** Debug */
    bool        bDebug;
    /** Cacheable */
    bool        bCacheable;
    /** Priv */
    bool        bPriv;
    /** Secure */
    bool        bSecure;
} SDL_ratExceptionInfo;

/** @} */

/** @} */

/**************************************************************************
* @brief Below are low level function declarations for rat module
**************************************************************************/

int32_t  SDL_RAT_getRegionTranslationConfig(const SDL_ratRegs *pRatRegs, uint32_t regionIndex,
                                            SDL_ratTranslationCfgInfo *pTranslationCfg);
uint32_t SDL_RAT_lmo(uint64_t v);
int32_t  SDL_RAT_setRegionTranslationEnable(SDL_ratRegs *pRatRegs, uint32_t regionIndex, bool bEnable);
void     SDL_RAT_setIntEnable(SDL_ratRegs *pRatRegs, SDL_RAT_intrSrcTypes intrSrc, bool bEnable);
void     SDL_RAT_setExceptionLoggingEnable(SDL_ratRegs *pRatRegs, bool bEnable);
void     SDL_RAT_setExceptionLoggingIntEnable(SDL_ratRegs *pRatRegs, bool bEnable);
int32_t  SDL_RAT_isPowerOf2(uint64_t v);
int32_t  SDL_RAT_isRegionOverlap(const SDL_ratRegs *pRatRegs, uint32_t thisRegionIndex,
                                 const SDL_ratTranslationCfgInfo *pTranslationCfg);
int32_t  SDL_RAT_validateTranslationConfig(const SDL_ratRegs *pRatRegs, uint32_t regionIndex,
                                           SDL_ratTranslationCfgInfo *pTranslationCfg);
/**
 *  @addtogroup SDL_RAT_FUNCTION
    @{
 *
 */

/**
 *   @n@b SDL_RAT_isRegionTranslationEnabled
 *
 *   @b Description
 *   @n This function returns the enable status of the specified region address
 *      translation
 *
 *   @b Arguments
 *   @n  pRatRegs       - A pointer (of type SDL_ratRegs*) to the base
 *                        address of the RAT module
 *   @n  regionIndex    - The index of the region to configure. This value must
 *                        be in the range 0..(SDL_RAT_getMaxRegions()-1) inclusive.
 *
 *   <b> Return Value </b>
 *   @n  true           - The specified region address translation is enabled
 *   @n  false          - The specified region address translation is not enabled
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Reads
 *   @n SDL_RAT_CTRL
 *
 *   @b Example
 *   @verbatim
       SDL_ratRegs *pRatRegs = (SDL_ratRegs *)RAT_BASE_ADDRESS;
       uint32_t    regionIndex = 0;
       bool        bIsEnabled;

       bIsEnabled = SDL_ratIsRegionTranslationEnabled(pRatRegs,regionIndex);
       printf("Address translation for region %u is %s\n", regionIndex, bIsEnabled ? "enabled" : "not enabled" );
    @endverbatim
 *
 */
int32_t  SDL_RAT_isRegionTranslationEnabled(const SDL_ratRegs *pRatRegs, uint32_t regionIndex);

/**
 *   @n@b SDL_RAT_getMaxRegions
 *
 *   @b Description
 *   @n This function returns the maximum number of regions that can be configured.
 *
 *   @b Arguments
 *   @n  pRatRegs - A pointer (of type SDL_ratRegs*) to the base
 *                  address of the RAT module
 *
 *   <b> Return Value </b>
 *   @n  The the maximum number of regions that can be configured in this RAT
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Reads
 *   @n None
 *
 *   @b Example
 *   @verbatim
       SDL_ratRegs *pRatRegs = (SDL_ratRegs *)RAT_BASE_ADDRESS;
       uint32_t    maxRegions;

       maxRegions = SDL_ratGetMaxRegions(pRatRegs);
    @endverbatim
 *
 */
uint32_t SDL_RAT_getMaxRegions(const SDL_ratRegs *pRatRegs);

/** @} */

#endif
