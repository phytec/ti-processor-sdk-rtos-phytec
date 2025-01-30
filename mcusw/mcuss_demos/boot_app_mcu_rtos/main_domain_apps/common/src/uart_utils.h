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
 */

/*
 *  \file uart_utils.h
 *
 *  \brief Define the core names and print delays for UART printing
 *
 */
#ifndef UART_UTILS_H_
#define UART_UTILS_H_

#if defined(SOC_J721E)
#    include <ti/board/src/j721e_evm/include/board_utils.h>
#    include <ti/board/src/j721e_evm/include/board_cfg.h>
#endif
#if defined(SOC_J7200)
#    include <ti/board/src/j7200_evm/include/board_utils.h>
#    include <ti/board/src/j7200_evm/include/board_cfg.h>
#endif

#ifdef BUILD_MCU2_0
#    define CORE_NAME "MCU2_0"
#    define BOOT_PRINT_DELAY (0xA0000)
#endif

#ifdef BUILD_MCU2_1
#    define CORE_NAME "MCU2_1"
#    define BOOT_PRINT_DELAY (0xC0000)
#endif

#ifdef BUILD_MCU3_0
#    define CORE_NAME "MCU3_0"
#    define BOOT_PRINT_DELAY (0xE0000)
#endif

#ifdef BUILD_MCU3_1
#    define CORE_NAME "MCU3_1"
#    define BOOT_PRINT_DELAY (0x100000)
#endif

#ifdef BUILD_C66X_1
#    define CORE_NAME "C66X_0"
//#    define BOOT_PRINT_DELAY (0x180000)
#    define BOOT_PRINT_DELAY (0x280000)
#endif

#ifdef BUILD_C66X_2
#    define CORE_NAME "C66X_1"
//#    define BOOT_PRINT_DELAY (0x200000)
#    define BOOT_PRINT_DELAY (0x300000)
#endif

#ifdef BUILD_C7X_1
#    define CORE_NAME "C7X_0"
#    define BOOT_PRINT_DELAY (0x2800000)
#endif

#ifdef BUILD_MPU1_0
#    define CORE_NAME "MPU1_0"
//#    define BOOT_PRINT_DELAY (0x1)
#    define BOOT_PRINT_DELAY (0x2000000)
#endif

#ifdef BUILD_MPU1_1
#    define CORE_NAME "MPU1_1"
#    define BOOT_PRINT_DELAY (0x20000)
#endif

#endif /* UART_UTILS_H_ */
