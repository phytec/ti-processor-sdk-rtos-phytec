/*
*
* Copyright (c) 2024 Texas Instruments Incorporated
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

/**
 *  \file utils_prf.c
 *
 *  \brief Profiling API utility file.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <string.h> 
#include "tistdtypes.h"
#include "lld_error.h"
#include "lld_types.h"
#include <ti/osal/osal.h>
#include <ti/osal/HwiP.h>
#include <ti/osal/TaskP.h>
#include <ti/osal/CycleprofilerP.h>

#include "utils_prf.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

typedef struct
{
    uint32_t isAlloc;
    /**< Flag to indicate if this instance is allocated or not */
    TaskP_Handle    pTsk;
    /**< Handle to the task */
    char name[32];
    /**< name of the task */
    uint64_t totalTskThreadTime;
    /**< Total time the task has spent */
} Utils_PrfLoadObj;

typedef struct
{
    Utils_PrfTsHndl  tsObj[UTILS_PRF_MAX_HNDL];
    Utils_PrfLoadObj loadObj[UTILS_PRF_MAX_HNDL];
} Utils_PrfObj;

typedef struct
{
    uint64_t totalSwiThreadTime;
    uint64_t totalHwiPThreadTime;
    uint64_t totalTime;
    uint64_t totalIdlTskTime;
} Utils_AccPrfLoadObj;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

static Utils_PrfObj        gUtils_prfObj;
Utils_AccPrfLoadObj gUtils_accPrfLoadObj;
//static uint32_t               gUtils_startLoadCalc;

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int32_t Utils_prfInit(void)
{
    memset(&gUtils_prfObj, 0, sizeof (gUtils_prfObj));
    memset(&gUtils_accPrfLoadObj, 0, sizeof (Utils_AccPrfLoadObj));

    return (0);
}

int32_t Utils_prfDeInit(void)
{
    return (0);
}

Utils_PrfTsHndl *Utils_prfTsCreate(const char *name)
{
    uint32_t        hndlId;
    uint32_t        cookie;
    Utils_PrfTsHndl *pHndl = NULL;

    cookie = HwiP_disable();

    for (hndlId = 0; hndlId < UTILS_PRF_MAX_HNDL; hndlId++)
    {
        pHndl = &gUtils_prfObj.tsObj[hndlId];

        if(FALSE == pHndl->isAlloc)
        {
            /* One less for NULL character */
            strncpy(pHndl->name, name, ((uint32_t) sizeof (pHndl->name) - 1U));
            pHndl->name[sizeof (pHndl->name) - 1U] = (UInt8) '\0';
            pHndl->isAlloc = (uint32_t) TRUE;
            Utils_prfTsReset(pHndl);
            break;
        }
    }

    HwiP_restore(cookie);

    return (pHndl);
}

int32_t Utils_prfTsDelete(Utils_PrfTsHndl *pHndl)
{
    pHndl->isAlloc = (uint32_t) FALSE;
    return (0);
}

uint64_t Utils_prfTsBegin(Utils_PrfTsHndl *pHndl)
{
    pHndl->startTs = Utils_prfTsGet64();

    return (pHndl->startTs);
}

uint64_t Utils_prfTsEnd(Utils_PrfTsHndl *pHndl, uint32_t numFrames)
{
    return (Utils_prfTsDelta(pHndl, pHndl->startTs, numFrames));
}

uint64_t Utils_prfTsDelta(Utils_PrfTsHndl *pHndl,
                           uint64_t              startTime,
                           uint32_t              numFrames)
{
    uint64_t endTs;
    uint32_t cookie;

    endTs = Utils_prfTsGet64();

    cookie = HwiP_disable();

    pHndl->totalTs += (endTs - pHndl->startTs);
    pHndl->count++;
    pHndl->numFrames += numFrames;

    HwiP_restore(cookie);

    return (endTs);
}

int32_t Utils_prfTsReset(Utils_PrfTsHndl *pHndl)
{
    uint32_t cookie;

    cookie = HwiP_disable();

    pHndl->totalTs   = 0;
    pHndl->count     = 0;
    pHndl->numFrames = 0;

    HwiP_restore(cookie);

    return (0);
}

uint64_t Utils_prfTsGet64(void)
{
    uint64_t curTs;
    //Types_CycleprofilerP_TimeStamp ts64;

    //CycleprofilerP_getTimeStamp();

    //curTs = ((uint64_t) ts64.hi << 32) | ts64.lo;
    curTs = CycleprofilerP_getTimeStamp();
    return (curTs);
}

int32_t Utils_prfTsPrint(Utils_PrfTsHndl *pHndl, uint32_t resetAfterPrint, uint32_t trace)
{
#if 0
    uint32_t       cpuKhz;
    uint32_t       timeMs, fps, fpc;
    Types_FreqHz cpuHz;

    TimerP_FreqHz(&cpuHz);

    cpuKhz = cpuHz.lo / (uint32_t) 1000U; /* convert to Khz */

    timeMs = pHndl->totalTs / cpuKhz;

    if(0U == timeMs)
    {
        fps = 0U;
    }
    else
    {
        fps = (pHndl->numFrames * (uint32_t) 1000U) / timeMs;
    }
    if(0U == pHndl->count)
    {
        fpc = 0U;
    }
    else
    {
        fpc = pHndl->numFrames / pHndl->count;
    }

    // GT_7trace(
        // trace, GT_INFO,
        // " %d: PRF : %s : t: %d ms, count: %d, frames: %d, fps: %d, fpc: %d \r\n",
        // AppUtils_getCurTimeInMsec(),
        // pHndl->name,
        // timeMs,       /* in msecs    */
        // pHndl->count,
        // pHndl->numFrames,
        // fps,       /* frames per second */
        // fpc        /* frames per count */
        // );

    if(resetAfterPrint)
    {
        Utils_prfTsReset(pHndl);
    }
#endif

    return (0);
}

int32_t Utils_prfTsPrintAll(uint32_t resetAfterPrint, uint32_t trace)
{
    uint32_t        hndlId;
    Utils_PrfTsHndl *pHndl;

//    GT_0trace(trace, GT_INFO, "\r\n");

    for (hndlId = 0; hndlId < UTILS_PRF_MAX_HNDL; hndlId++)
    {
        pHndl = &gUtils_prfObj.tsObj[hndlId];

        if(TRUE == pHndl->isAlloc)
        {
            Utils_prfTsPrint(pHndl, resetAfterPrint, trace);
        }
    }

//    GT_0trace(trace, GT_INFO, "\r\n");

    return (0);
}

int32_t Utils_prfLoadRegister(TaskP_Handle pTsk, const char *name)
{
    uint32_t            hndlId;
    uint32_t            cookie;
    int32_t             status = LLD_EFAIL;
    Utils_PrfLoadObj   *pHndl;

    cookie = HwiP_disable();

    for (hndlId = 0; hndlId < UTILS_PRF_MAX_HNDL; hndlId++)
    {
        pHndl = &gUtils_prfObj.loadObj[hndlId];

        if(FALSE == pHndl->isAlloc)
        {
            pHndl->isAlloc = (uint32_t) TRUE;
            pHndl->pTsk    = pTsk;
            /* One less for NULL character */
            strncpy(pHndl->name, name, ((uint32_t) sizeof (pHndl->name) - 1U));
            pHndl->name[sizeof (pHndl->name) - 1U] = (UInt8) '\0';
            status = CSL_SOK;
            break;
        }
    }

    HwiP_restore(cookie);

    return (status);
}

int32_t Utils_prfLoadUnRegister(TaskP_Handle pTsk)
{
    uint32_t            hndlId;
    uint32_t            cookie;
    int32_t             status = LLD_EFAIL;
    Utils_PrfLoadObj   *pHndl;

    cookie = HwiP_disable();

    for (hndlId = 0; hndlId < UTILS_PRF_MAX_HNDL; hndlId++)
    {
        pHndl = &gUtils_prfObj.loadObj[hndlId];

        if((TRUE == pHndl->isAlloc) && (pHndl->pTsk == pTsk))
        {
            pHndl->isAlloc = (uint32_t) FALSE;
            status         = CSL_SOK;
            break;
        }
    }

    HwiP_restore(cookie);

    return (status);
}

void Utils_prfLoadGetTaskLoad(TaskP_Handle pTsk, Utils_PrfLoad *load)
{
    float            hwiLoad = 0U, swiLoad = 0U, tskLoad = 0U, cpuLoad = 0U;
    uint32_t            hndlId = 0U;
    Utils_PrfLoadObj   *pHndl;
    uint32_t            cookie;


    cookie = HwiP_disable();

    /* Avoid divide by 0 condition, when CPU is 100% loaded */
    if (0 == gUtils_accPrfLoadObj.totalTime)
    {
        gUtils_accPrfLoadObj.totalTime = 1;
    }

    hwiLoad = (float) ((gUtils_accPrfLoadObj.totalHwiPThreadTime *
                         (double) 100.0) / gUtils_accPrfLoadObj.totalTime);
    swiLoad = (float) ((gUtils_accPrfLoadObj.totalSwiThreadTime *
                         (double) 100.0) / gUtils_accPrfLoadObj.totalTime);
    cpuLoad = (float) 100.0 -
              (float) ((gUtils_accPrfLoadObj.totalIdlTskTime *
                         (double) 100.0) /
                        gUtils_accPrfLoadObj.totalTime);
    for (hndlId = 0; hndlId < UTILS_PRF_MAX_HNDL; hndlId++)
    {
        pHndl = &gUtils_prfObj.loadObj[hndlId];

        if((TRUE == pHndl->isAlloc) && (pHndl->pTsk == pTsk))
        {
            tskLoad = (float) ((pHndl->totalTskThreadTime *
                                     (double) 100.0) /
                                    gUtils_accPrfLoadObj.totalTime);
        }
    }

    HwiP_restore(cookie);

    load->cpuLoad = cpuLoad;
    load->hwiLoad = hwiLoad;
    load->swiLoad = swiLoad;
    load->tskLoad = tskLoad;

    return;
}

uint32_t Utils_prfLoadCalcStart(void)
{
    uint32_t cookie;

    cookie = HwiP_disable();
    //gUtils_startLoadCalc = (uint32_t) TRUE;
    HwiP_restore(cookie);

    return cookie;
}

uint32_t Utils_prfLoadCalcStop(void)
{
    uint32_t cookie;

    cookie = HwiP_disable();
    //gUtils_startLoadCalc = FALSE;
    HwiP_restore(cookie);

    return cookie;
}

uint32_t Utils_prfLoadCalcReset(void)
{
    uint32_t            hndlId;
    Utils_PrfLoadObj   *pHndl;
    uint32_t cookie;

    cookie = HwiP_disable();
    gUtils_accPrfLoadObj.totalHwiPThreadTime = 0;
    gUtils_accPrfLoadObj.totalSwiThreadTime = 0;
    gUtils_accPrfLoadObj.totalTime          = 0;
    gUtils_accPrfLoadObj.totalIdlTskTime    = 0;

    /* Reset the performance loads accumulator */
    for (hndlId = 0; hndlId < UTILS_PRF_MAX_HNDL; hndlId++)
    {
        pHndl = &gUtils_prfObj.loadObj[hndlId];

        if(((uint32_t) TRUE == pHndl->isAlloc) &&
            (pHndl->pTsk != NULL))
        {
            pHndl->totalTskThreadTime = 0;
        }
    }
    HwiP_restore(cookie);

    return cookie;
}

/* Function called by Loadupdate for each update cycle */
/*
void Utils_prfLoadUpdate(void)
{
    uint32_t            hndlId;
    Load_Stat           hwiLoadStat, swiLoadStat, tskLoadStat, idlTskLoadStat;
    TaskP_Handle        idlTskHndl = NULL;
    Utils_PrfLoadObj   *pHndl;

    if(((uint32_t) TRUE) == (uint32_t) gUtils_startLoadCalc)
    {
        idlTskHndl = TaskP_getIdleTask();

        * Get the all loads first *
        Load_getGlobalHwiPLoad(&hwiLoadStat);
        Load_getGlobalSwiLoad(&swiLoadStat);
        Load_getTaskLoad(idlTskHndl, &idlTskLoadStat);

        gUtils_accPrfLoadObj.totalHwiThreadTime += hwiLoadStat.threadTime;
        gUtils_accPrfLoadObj.totalSwiThreadTime += swiLoadStat.threadTime;
        gUtils_accPrfLoadObj.totalTime          += hwiLoadStat.totalTime;
        gUtils_accPrfLoadObj.totalIdlTskTime    += idlTskLoadStat.threadTime;

        * Call the load updated function of each registered task one by one
         * along with the swiLoad, hwiLoad, and Task's own load *
        for (hndlId = 0; hndlId < UTILS_PRF_MAX_HNDL; hndlId++)
        {
            pHndl = &gUtils_prfObj.loadObj[hndlId];

            if(((uint32_t) TRUE == pHndl->isAlloc) &&
                (pHndl->pTsk != NULL))
            {
                Load_getTaskLoad(pHndl->pTsk, &tskLoadStat);
                pHndl->totalTskThreadTime += tskLoadStat.threadTime;
            }
        }
    }

    return;
}
*/
uint64_t Utils_prfTsGetTimeinMs(uint64_t startTime, uint64_t endTime)
{
    uint32_t        cpuKhz;
  //  Types_FreqHz    cpuHz;
    uint64_t        timeInMs;
    uint64_t        lowTs = startTime, highTs = endTime;

    /* Roll over */
    if (endTime < startTime)
    {
        lowTs = 0U;
        highTs = ((~(uint64_t)0) - startTime) + endTime;
    }

    cpuKhz = CycleprofilerP_getTimeStamp();
    //cpuKhz = cpuHz.lo / (uint32_t) 1000U; /* convert to Khz */

    timeInMs = (highTs - lowTs) / cpuKhz;

    return timeInMs;
}
