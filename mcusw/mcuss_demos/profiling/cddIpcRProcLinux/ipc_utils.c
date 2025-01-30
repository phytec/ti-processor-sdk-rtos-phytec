/*
 *  Copyright (c) Texas Instruments Incorporated 2020
 *  All rights reserved.
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
 *  \file ipc_utils.c
 *
 *  \brief IPC  utility function used for common setup
 *  
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdio.h>
#include <stdint.h>

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Memory.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/drv/ipc/ipc.h>
#include <ti/osal/osal.h>

#define CACHE_WB_TICK_PERIOD    5


void SetManualBreak()
{
#if  defined (__aarch64__)
    volatile uint32_t x = 0;
    while(x)
        ;
#endif
}

void sysIdleLoop(void)
{
#if defined(BUILD_C66X_1) || defined(BUILD_C66X_2)
    __asm(" IDLE");
#elif defined(BUILD_C7X_1)
    __asm(" IDLE");
#elif defined(BUILD_MCU1_0) || defined(BUILD_MCU1_1) || \
      defined(BUILD_MCU2_0) || defined(BUILD_MCU2_1) || \
      defined(BUILD_MCU3_0) || defined(BUILD_MCU3_1)
   asm(" wfi");
#endif
}

/*
 * ======== traceBuf_cacheWb ========
 *
 * Used for flushing SysMin trace buffer.
 */

static uint8_t *traceBufAddr = 0U;

Void traceBuf_cacheWb()
{
    static uint64_t oldticks;
    uint64_t newticks;
    Types_Timestamp64 bios_timestamp64;

    Timestamp_get64(&bios_timestamp64);
    newticks = ((uint64_t) bios_timestamp64.hi << 32) | bios_timestamp64.lo;
    /* Don't keep flusing cache */
    if ((newticks - oldticks) >= (uint64_t)CACHE_WB_TICK_PERIOD) {
        oldticks = newticks;

        /* Flush the cache of the SysMin buffer only: */
        if (traceBufAddr == NULL) {
            traceBufAddr = Ipc_getResourceTraceBufPtr();
        }
        if (traceBufAddr != NULL) {
            CacheP_wb((const void *)traceBufAddr, 0x80000);
        }
    }
}

Void traceBuf_cacheWb_Exception()
{
    uint8_t *traceBufAddr = 0;

    traceBufAddr = Ipc_getResourceTraceBufPtr();
    CacheP_wb((const void *)traceBufAddr, 0x80000);
}
