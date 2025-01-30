/*
 * Copyright (C) 2024, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**
 *
 * \file  nor_spi_phy_tune.h
 *
 * \brief This file contains OSPI/xSPI PHY tuning structure, macro definitions
 *
 *****************************************************************************/
#ifndef NOR_SPI_PHY_TUNE_H_
#define NOR_SPI_PHY_TUNE_H_

#include "Std_Types.h"

/*!
 *  @brief  PHY configuration data
 *
 *  PHY setting for Read Delay, txDLL, and rxDLL values to describe a "point" in the PHY shmoo
 */
typedef struct NOR_PhyConfig_s
{
    sint32 txDLL;
    sint32 rxDLL;
    sint32 rdDelay;
} NOR_PhyConfig;

#define NOR_SPI_PHY_TXDLL_LOW_WINDOW_START     (18U)
#if defined (SOC_J784S4) // J784s4 occasionally goes over 24 for TXDLL_LOW_WINDOW
#define NOR_SPI_PHY_TXDLL_LOW_WINDOW_END       (28U)
#else
#define NOR_SPI_PHY_TXDLL_LOW_WINDOW_END       (24U)
#endif

#define NOR_SPI_PHY_TXDLL_HIGH_WINDOW_START    (48U)
#define NOR_SPI_PHY_TXDLL_HIGH_WINDOW_END      (38U)

#define NOR_SPI_PHY_INIT_RD_DELAY   (1U)
#define NOR_SPI_PHY_VTM_TARGET      (700U)

extern Std_ReturnType Nor_spiPhyTune(SPI_Handle handle, uint32 offset);
extern void Nor_spiPhyTuneReset(boolean ddrMode);
#endif /* NOR_SPI_PHY_TUNE_H_ */
