/*
 * @file  sdl_ip_pok.h
 *
 * @brief
 *  C implementation interface header file for the POK module SDL-FL.
 *
 *  Translates POK ID to POK Address. This is a SOC specific source file.
 *
 *  \par
 *  ============================================================================
 *  @n   (C) Copyright 2021, Texas Instruments, Inc.
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
#ifndef SDL_SOC_POK_IP_H
#define SDL_SOC_POK_IP_H

#ifdef __cplusplus
    extern "C" {
#endif

/** ---------------------------------------------------------------------------
 * @brief This enumerator defines the possible POK module ID values on J7200
 *
 *  \anchor SDL_pok_id
 *  \name POK POK ID values
 *
 *  @{
 * ----------------------------------------------------------------------------
 */
typedef int8_t  SDL_POK_Inst ;

/** Invalid POK/POR ID */
#define SDL_FIRST_POK_ID                              (0)
/* PMIC POK ID */
#define SDL_POK_VDDA_PMIC_IN_ID                       (SDL_FIRST_POK_ID)
/* CORE Over/Under Voltage POK ID */
#define SDL_POK_VDD_CORE_ID                           (1)
/* VDDR CORE Over/Under Voltage POK ID */
#define SDL_POK_VDDR_CORE_ID                          (2)
/* VDD CPU Over/Under Voltage POK ID */
#define SDL_POK_VDD_CPU_ID                            (3)
/* VDDSHV MCU Over/Under Voltage POK ID */
#define SDL_POK_VMON_EXT_ID                           (4)
/* VDDSHV Main 1P8 Over/Under Voltage POK ID */
#define SDL_POK_VMON_EXT_MAIN_1P8_ID                  (5)
/* VDDSHV Main 3P3 Over/Under Voltage POK ID */
#define SDL_POK_VMON_EXT_MAIN_3P3_ID                  (6)
/* VDDR MCU Over/Under Voltage POK ID */
#define SDL_POK_VDDR_MCU_ID                           (7)
/* VDDSHV0 MCU Over/Under Voltage ID */
#define SDL_POK_VDDSHV_WKUP_GEN_ID                    (8)
/* CAP VDDS MCU Over/Under Voltage ID */
#define SDL_POK_CAP_VDDS_MCU_GEN_ID                   (9)
/* VDD MCU Over Voltage ID */
#define SDL_POK_VDD_MCU_OV_ID                         (10)
/* VDD MCU Under Voltage ID */
#define SDL_POR_POKHV_UV_ID                           (11)
/* VDDA MCU Under Voltage ID */
#define SDL_POR_POKLVB_UV_ID                          (12)
/* VDDA MCU Over Voltage ID */
#define SDL_POR_POKLVA_OV_ID                          (13)
/* LAST POK ID */
#define SDL_LAST_POK_ID                               (SDL_POR_POKLVA_OV_ID)

/* @} */

/** ---------------------------------------------------------------------------
 * @brief This enumerator defines the possible PRG module ID values on J7200
 *
 *  \anchor SDL_prg_id
 *  \name POK PRG ID values
 *
 *  @{
 * ----------------------------------------------------------------------------
 */
typedef int8_t  SDL_PRG_Inst ;

/** First PRG ID */
#define SDL_POK_PRG_FIRST_ID                          (0)
/** PRG MAIN */
#define SDL_POK_PRG_MAIN_ID                           (1)
/** PRG MCU */
#define SDL_POK_PRG_MCU_ID                            (2)
/** PRG POR */
#define SDL_POK_PRG_POR_ID                            (3)
/** Last POK ID */
#define SDL_POK_PRG_LAST_ID                           (SDL_POK_PRG_POR_ID)

/* @} */

#ifdef __cplusplus
}
#endif
#endif /* SDL_SOC_POK_IP_H */
/* Nothing past this point */

