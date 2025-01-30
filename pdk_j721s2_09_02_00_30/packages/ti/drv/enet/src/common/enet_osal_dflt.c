/*
 *  Copyright (c) Texas Instruments Incorporated 2020
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

/*!
 * \file  enet_osal_dflt.c
 *
 * \brief This file contains a default OSAL implementation.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* EnetTrace id for this module, must be unique within Enet LLD */
#define ENETTRACE_MOD_ID 0x007

#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <ti/osal/osal.h>
#include <ti/drv/enet/include/common/enet_osal_dflt.h>
#include <ti/osal/CycleprofilerP.h>
#include <ti/osal/src/nonos/Nonos_config.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define __NOP  asm (" NOP ")

#define NOP5   do { __NOP; __NOP; __NOP; __NOP; __NOP; } while (0)

#define NOP10  NOP5; \
               NOP5

#define NOP50  NOP10; \
               NOP10; \
               NOP10; \
               NOP10; \
               NOP10

/* Mutex pool size */
#define ENET_OSAL_DFLT_MUTEX_POOL_SIZE            (20U)

/* Second to nanosecs conversion factor */
#define ENET_OSAL_DFLT_SEC2NANOSEC                (1000000000ULL)

/* Max value reported by CycleprofilerP_getTimeStamp() */
#define ENET_OSAL_DFLT_TIMESTAMP_MAX              (0xFFFFFFFFULL)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

typedef struct EnetOsalDflt_Mutex_s
{
    bool used;
    MutexP_Object mutexObj;
    MutexP_Handle handle;
} EnetOsalDflt_Mutex;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static uintptr_t EnetOsalDflt_disableAllIntr(void);

static void EnetOsalDflt_restoreAllIntr(uintptr_t cookie);

static void *EnetOsalDflt_registerIntr(EnetOsal_Isr isrFxn,
                                       uint32_t coreIntrNum,
                                       uint32_t intrPriority,
                                       uint32_t intrTrigType,
                                       void *arg);

static void EnetOsalDflt_unregisterIntr(void *hHwi);

static void EnetOsalDflt_enableIntr(uint32_t coreIntrNum);

static void EnetOsalDflt_disableIntr(uint32_t coreIntrNum);

static void *EnetOsalDflt_createMutex(void);

static void EnetOsalDflt_deleteMutex(void *hMutex);

static void EnetOsalDflt_lockMutex(void *hMutex);

static void EnetOsalDflt_unlockMutex(void *hMutex);

static bool EnetOsalDflt_isCacheCoherent(void);

static void EnetOsalDflt_cacheInv(const void *addr,
                                  int32_t size);

static void EnetOsalDflt_cacheWb(const void *addr,
                                 int32_t size);

static void EnetOsalDflt_cacheWbInv(const void *addr,
                                    int32_t size);

static uint32_t EnetOsalDflt_timerRead();

static uint32_t EnetOsalDflt_convertNanosec2Ticks(uint32_t delayInNsecs);

void EnetOsalDflt_delay(uint64_t delayInNsecs);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* Mutex pool */
static EnetOsalDflt_Mutex gEnetOsalDflt_mutexPool[ENET_OSAL_DFLT_MUTEX_POOL_SIZE];

/* CPU frequency*/
static uint32_t gEnetOsalDflt_selfFreqHz;

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

void EnetOsalDflt_initCfg(EnetOsal_Cfg *cfg)
{
    memset(&gEnetOsalDflt_mutexPool[0U], 0, sizeof(gEnetOsalDflt_mutexPool));

    gEnetOsalDflt_selfFreqHz = osalArch_TimeStampGetFreqKHz()*1000;

    cfg->disableAllIntr  = &EnetOsalDflt_disableAllIntr;;
    cfg->restoreAllIntr  = &EnetOsalDflt_restoreAllIntr;
    cfg->disableIntr     = &EnetOsalDflt_disableIntr;
    cfg->restoreIntr     = &EnetOsalDflt_enableIntr;
    cfg->registerIntr    = &EnetOsalDflt_registerIntr;
    cfg->unregisterIntr  = &EnetOsalDflt_unregisterIntr;
    cfg->createMutex     = &EnetOsalDflt_createMutex;
    cfg->deleteMutex     = &EnetOsalDflt_deleteMutex;
    cfg->lockMutex       = &EnetOsalDflt_lockMutex;
    cfg->unlockMutex     = &EnetOsalDflt_unlockMutex;
    cfg->isCacheCoherent = &EnetOsalDflt_isCacheCoherent;
    cfg->cacheInv        = &EnetOsalDflt_cacheInv;
    cfg->cacheWb         = &EnetOsalDflt_cacheWb;
    cfg->cacheWbInv      = &EnetOsalDflt_cacheWbInv;
    cfg->timerRead       = &EnetOsalDflt_timerRead;
    cfg->delay           = &EnetOsalDflt_delay;
}

static uintptr_t EnetOsalDflt_disableAllIntr(void)
{
    return HwiP_disable();
}

static void EnetOsalDflt_restoreAllIntr(uintptr_t cookie)
{
    HwiP_restore(cookie);
}

static void *EnetOsalDflt_registerIntr(EnetOsal_Isr isrFxn,
                                       uint32_t coreIntrNum,
                                       uint32_t intrPriority,
                                       uint32_t intrTrigType,
                                       void *arg)
{
    OsalRegisterIntrParams_t intrPrms;
    OsalInterruptRetCode_e status;
    HwiP_Handle hHwi = NULL;

    Osal_RegisterInterrupt_initParams(&intrPrms);

    /* Populate the interrupt parameters */
    intrPrms.corepacConfig.arg             = (uintptr_t)arg;
    intrPrms.corepacConfig.isrRoutine      = isrFxn;
    intrPrms.corepacConfig.priority        = intrPriority;
    intrPrms.corepacConfig.triggerSensitivity = intrTrigType;
    intrPrms.corepacConfig.corepacEventNum = 0U;
    intrPrms.corepacConfig.intVecNum       = coreIntrNum;

    /* Register interrupts */
    status = Osal_RegisterInterrupt(&intrPrms, &hHwi);
    if (status != OSAL_INT_SUCCESS)
    {
        hHwi = NULL;
    }

    return hHwi;
}

static void EnetOsalDflt_unregisterIntr(void *hHwi)
{
    uint32_t corepacEventNum = 0U;

    /* Delete interrupts */
    Osal_DeleteInterrupt((HwiP_Handle)hHwi, corepacEventNum);
}

static void EnetOsalDflt_enableIntr(uint32_t coreIntrNum)
{
    uint32_t corepacEventNum = 0U;

    Osal_EnableInterrupt(corepacEventNum, coreIntrNum);
}

static void EnetOsalDflt_disableIntr(uint32_t coreIntrNum)
{
    uint32_t corepacEventNum = 0U;

    Osal_DisableInterrupt(corepacEventNum, coreIntrNum);
}

static void *EnetOsalDflt_createMutex(void)
{
    EnetOsalDflt_Mutex *mutex = NULL;
    uintptr_t key;
    uint32_t i;

    key = HwiP_disable();

    for (i = 0U; i < ENET_OSAL_DFLT_MUTEX_POOL_SIZE; i++)
    {
        if (!gEnetOsalDflt_mutexPool[i].used)
        {
            gEnetOsalDflt_mutexPool[i].used = BTRUE;
            break;
        }
    }

    HwiP_restore(key);

    if (i < ENET_OSAL_DFLT_MUTEX_POOL_SIZE)
    {
        mutex = &gEnetOsalDflt_mutexPool[i];

        mutex->handle = MutexP_create(&mutex->mutexObj);
        if (mutex->handle == NULL)
        {
            key = HwiP_disable();
            mutex->used = BFALSE;
            HwiP_restore(key);
            mutex = NULL;
        }
    }

    return (void *)mutex;
}

static void EnetOsalDflt_deleteMutex(void *hMutex)
{
    EnetOsalDflt_Mutex *mutex = (EnetOsalDflt_Mutex *)hMutex;
    uintptr_t key;

    DebugP_assert(mutex != NULL);

    if (mutex->used)
    {
        MutexP_delete(mutex->handle);

        key = HwiP_disable();
        mutex->used = BFALSE;
        HwiP_restore(key);
    }
}

static void EnetOsalDflt_lockMutex(void *hMutex)
{
    EnetOsalDflt_Mutex *mutex = (EnetOsalDflt_Mutex *)hMutex;

    DebugP_assert(mutex != NULL);

    MutexP_lock(mutex->handle, MutexP_WAIT_FOREVER);
}

static void EnetOsalDflt_unlockMutex(void *hMutex)
{
    EnetOsalDflt_Mutex *mutex = (EnetOsalDflt_Mutex *)hMutex;

    DebugP_assert(mutex != NULL);

    MutexP_unlock(mutex->handle);
}

static bool EnetOsalDflt_isCacheCoherent(void)
{
    bool isCoherent = BFALSE;

#if defined(SOC_J721E) || defined(SOC_J7200) || defined(SOC_J721S2) || defined(SOC_J784S4)
#if defined(BUILD_MPU1_0)
    isCoherent = BTRUE;
#endif
#endif

    return isCoherent;
}

static void EnetOsalDflt_cacheInv(const void *addr,
                                  int32_t size)
{
    CacheP_Inv(addr, size);
}

static void EnetOsalDflt_cacheWb(const void *addr,
                                 int32_t size)
{
    CacheP_wb(addr, size);
}

static void EnetOsalDflt_cacheWbInv(const void *addr,
                                    int32_t size)
{
    CacheP_wbInv(addr, size);
}

static uint32_t EnetOsalDflt_timerRead()
{
    return 0U;
}

static uint32_t EnetOsalDflt_convertNanosec2Ticks(uint32_t delayInNsecs)
{
    return (((uint64_t)delayInNsecs*gEnetOsalDflt_selfFreqHz) / ENET_OSAL_DFLT_SEC2NANOSEC);
}

void EnetOsalDflt_delay(uint64_t delayInNsecs)
{
   uint64_t delayTicks = EnetOsalDflt_convertNanosec2Ticks(delayInNsecs);
   uint64_t currentTick = (uint64_t)CycleprofilerP_getTimeStamp();
   const uint64_t endTick = currentTick + (uint64_t)delayTicks;

   while (currentTick < endTick)
   {
       NOP50;
       currentTick = (uint64_t)CycleprofilerP_getTimeStamp();
       if ((currentTick < endTick) &&
           ((endTick - currentTick) > delayTicks))
       {
           currentTick += ENET_OSAL_DFLT_TIMESTAMP_MAX + 1ULL;
       }
   }
}
