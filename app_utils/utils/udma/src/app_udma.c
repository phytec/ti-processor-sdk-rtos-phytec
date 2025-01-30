/*
 *
 * Copyright (c) 2019 Texas Instruments Incorporated
 *
 * All rights reserved not granted herein.
 *
 * Limited License.
 *
 * Texas Instruments Incorporated grants a world-wide, royalty-free, non-exclusive
 * license under copyrights and patents it now or hereafter owns or controls to make,
 * have made, use, import, offer to sell and sell ("Utilize") this software subject to the
 * terms herein.  With respect to the foregoing patent license, such license is granted
 * solely to the extent that any such patent is necessary to Utilize the software alone.
 * The patent license shall not apply to any combinations which include this software,
 * other than combinations with devices manufactured by or for TI ("TI Devices").
 * No hardware patent is licensed hereunder.
 *
 * Redistributions must preserve existing copyright notices and reproduce this license
 * (including the above copyright notice and the disclaimer and (if applicable) source
 * code license limitations below) in the documentation and/or other materials provided
 * with the distribution
 *
 * Redistribution and use in binary form, without modification, are permitted provided
 * that the following conditions are met:
 *
 * *       No reverse engineering, decompilation, or disassembly of this software is
 * permitted with respect to any software provided in binary form.
 *
 * *       any redistribution and use are licensed by TI for use only with TI Devices.
 *
 * *       Nothing shall obligate TI to provide you with source code for the software
 * licensed and provided to you in object code.
 *
 * If software source code is provided to you, modification and redistribution of the
 * source code are permitted provided that the following conditions are met:
 *
 * *       any redistribution and use of the source code, including any resulting derivative
 * works, are licensed by TI for use only with TI Devices.
 *
 * *       any redistribution and use of any object code compiled from the source code
 * and any resulting derivative works, are licensed by TI for use only with TI Devices.
 *
 * Neither the name of Texas Instruments Incorporated nor the names of its suppliers
 *
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * DISCLAIMER.
 *
 * THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdint.h>
#include <string.h>
#include <CacheP.h>
#if defined(SOC_AM62A) || defined(SOC_J722S)
#include <dmautils/udma_standalone/udma.h>
#else
#include <udma.h>
#endif
#include <utils/udma/include/app_udma.h>
#include <utils/mem/include/app_mem.h>
#include <utils/console_io/include/app_log.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/*
 * UDMA driver objects
 */
static struct Udma_DrvObj gAppUdmaDrvObj;

/*
 * UDMA driver objects
 */
#if defined(SOC_J721S2) || defined(SOC_J784S4)

static struct Udma_DrvObj gAppUdmaDrvObjCsirxCsitx;

#endif

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
#if !(defined(SOC_AM62A) || defined(SOC_J722S))
/* appUdmaOsalMutexLock/Unlock are kept empty so that locks are not taken
 * when UDMA driver calls these functions via callback.
 *
 * On C7x for TIDL, we want to avoid interrupts like timer interrupting TIDL process,
 * so we do a Hwi_disable/restore around TIDL process
 *
 * However TIDL calls UDMA open channel API to open DRU channels.
 * And UDMA open channel inturn calls OSAL callback to take locks.
 *
 * By default lock is BIOS semaphore, if BIOS semphores are taken when interrupts
 * are disabled then they get reenabled again.
 *
 * Hence to avoid this reenabling of intetionally disabled interrupts,
 * we keep blank lock/unlock callbacks.
 *
 * Note, this means that Udma driver is now not thread safe.
 * But this is done only in C7x and in C7x we call
 * all UDMA APIs from a single thread so this is safe to do in C7x.
 * */
void appUdmaOsalMutexLock(void *mutexHandle)
{

}

void appUdmaOsalMutexUnlock(void *mutexHandle)
{

}
#endif

uint64_t appUdmaDefaultVirtToPhyAddrConversion(const void *virtAddr,
                                      uint32_t chNum,
                                      void *appData)
{

  return (uint64_t)virtAddr;
}

void appUdmaInitPrmSetDefault(app_udma_init_prms_t *prm)
{
    prm->virtToPhyFxn = NULL;
}

int32_t appUdmaInit(const app_udma_init_prms_t *prms)
{
    int32_t         retVal = 0;
    uint32_t        udmaInstId;
    Udma_InitPrms   udmaInitPrms;

    appLogPrintf("UDMA: Init ... !!!\n");

    udmaInstId = UDMA_INST_ID_MAIN_0;
    UdmaInitPrms_init(udmaInstId, &udmaInitPrms);
    udmaInitPrms.printFxn = (Udma_PrintFxn)appLogPrintf;
    #if defined(SOC_AM62A) || defined(SOC_J722S)
    if (NULL != prms->virtToPhyFxn)
    {
        udmaInitPrms.virtToPhyFxn = prms->virtToPhyFxn;
    }
    else
    {
        udmaInitPrms.virtToPhyFxn = appUdmaDefaultVirtToPhyAddrConversion;
    }
    #else
    udmaInitPrms.skipGlobalEventReg = UFALSE;
    #if defined(C7X_FAMILY)
    udmaInitPrms.osalPrms.lockMutex = appUdmaOsalMutexLock;
    udmaInitPrms.osalPrms.unlockMutex = appUdmaOsalMutexUnlock;
    if (NULL != prms->virtToPhyFxn)
    {
        udmaInitPrms.virtToPhyFxn = prms->virtToPhyFxn;
    }
    else
    {
        udmaInitPrms.virtToPhyFxn = appUdmaDefaultVirtToPhyAddrConversion;
    }
    #endif
    #endif
    retVal = Udma_init(&gAppUdmaDrvObj, &udmaInitPrms);
    if(retVal!=0)
    {
        appLogPrintf("UDMA: ERROR: Udma_init failed !!!\n");
    }

    appLogPrintf("UDMA: Init ... Done !!!\n");

    return (retVal);
}

int32_t appUdmaDeInit(void)
{
    int32_t     retVal = 0;

    retVal = Udma_deinit(&gAppUdmaDrvObj);
    if(retVal != 0)
    {
        appLogPrintf("UDMA: ERROR: Udma_deinit failed !!!\n");
    }

    return (retVal);
}

void *appUdmaGetObj(void)
{
    return (void *)&gAppUdmaDrvObj;
}

#if !(defined(SOC_AM62A) || defined(SOC_J722S))
#if defined(SOC_J721S2) || defined(SOC_J784S4)

int32_t appUdmaCsirxCsitxInit(void)
{
    int32_t         retVal = 0;
    uint32_t        udmaInstId;
    Udma_InitPrms   udmaInitPrms;

    appLogPrintf("UDMA: Init for CSITX/CSIRX ... !!!\n");

    udmaInstId = UDMA_INST_ID_BCDMA_0;
    UdmaInitPrms_init(udmaInstId, &udmaInitPrms);
    udmaInitPrms.printFxn = (Udma_PrintFxn)appLogPrintf;
    retVal = Udma_init(&gAppUdmaDrvObjCsirxCsitx, &udmaInitPrms);
    if(retVal!=0)
    {
        appLogPrintf("UDMA: ERROR: Udma_init for CSITX/CSIRX failed !!!\n");
    }

    appLogPrintf("UDMA: Init for CSITX/CSIRX ... Done !!!\n");

    return (retVal);
}

int32_t appUdmaCsirxCsitxDeInit(void)
{
    int32_t     retVal = 0;

    retVal = Udma_deinit(&gAppUdmaDrvObjCsirxCsitx);
    if(retVal != 0)
    {
        appLogPrintf("UDMA: ERROR: Udma_deinit failed !!!\n");
    }

    return (retVal);
}

#endif

void *appUdmaCsirxCsitxGetObj(void)
{
    #if defined(SOC_J721S2) || defined(SOC_J784S4)
    return (void *)&gAppUdmaDrvObjCsirxCsitx;
    #elif defined(SOC_J721E)
    return (void *)&gAppUdmaDrvObj;
    #elif defined(SOC_J784S4)
    #endif
}
#endif
