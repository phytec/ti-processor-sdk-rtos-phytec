/*
 *
 * Copyright (c) 2017 Texas Instruments Incorporated
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

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <fcntl.h>
#if _POSIX_C_SOURCE >= 199309L
#include <time.h>   /* for nanosleep */
int nanosleep(const struct timespec *req, struct timespec *rem);
#else
#include <unistd.h> /* for usleep */
extern int usleep (__useconds_t __useconds);
#endif
#include "app_timer_priv.h"
#include <inttypes.h>


#undef APP_TIMER_DEBUG

static void *GTC_BASE_ADDR = NULL;

#define GET_GTC_VALUE64   (*(volatile uint64_t*)(GTC_BASE_ADDR + 8))
#define GET_GTC_FREQUENCY (*(volatile uint64_t*)(GTC_BASE_ADDR + 0x20))

#define PRI_MAX  sched_get_priority_max(SCHED_FIFO)
#define PRI_MIN  sched_get_priority_min(SCHED_FIFO)

int32_t appLogGlobalTimeInit()
{
    int32_t status = 0;

    GTC_BASE_ADDR = appMemMap((void*)GTC_TIMER_MAPPED_BASE, GTC_TIMER_MAPPED_SIZE);
    if(GTC_BASE_ADDR==NULL)
    {
        (void)printf("APP_TIMER: ERROR: Unable to mmap gtc (%p of %d bytes) !!!\n", (void*)GTC_TIMER_MAPPED_BASE, GTC_TIMER_MAPPED_SIZE );
        status = -1;
        return status;
    }

    return status;
}

int32_t appLogGlobalTimeDeInit()
{
    int32_t status = 0;

    if (NULL != GTC_BASE_ADDR)
    {
        status = appMemUnMap((void*)GTC_BASE_ADDR, GTC_TIMER_MAPPED_SIZE);
    }

    return status;
}

void appLogPrintGtcFreq()
{
    uint64_t mhzFreq = 0u;

    if (NULL != GTC_BASE_ADDR)
    {
        mhzFreq = GET_GTC_FREQUENCY / APP_TIMER_HZ_TO_MHZ;

        appLogPrintf("GTC Frequency = %" PRIu64 " MHz\n", mhzFreq);
    }
}

uint64_t appLogGetGlobalTimeInUsec()
{
    uint64_t cur_ts = 0u, mhzFreq = 0u; /* Returning ts in usecs */

    if (NULL != GTC_BASE_ADDR)
    {
        mhzFreq = GET_GTC_FREQUENCY / APP_TIMER_HZ_TO_MHZ;

        if (0u != mhzFreq)
        {
            cur_ts = GET_GTC_VALUE64 / mhzFreq;
        }
    }

    return cur_ts;
}

uint64_t appLogGetLocalTimeInUsec()
{
    uint64_t timeInUsecs = 0;
    struct timeval tv;
    static uint64_t g_start_time = 0;

    if (gettimeofday(&tv, NULL) < 0)
    {
        timeInUsecs = 0;
    }
    else
    {
        timeInUsecs = tv.tv_sec * 1000000ull + tv.tv_usec;
    }
    if(g_start_time==0)
    {
        g_start_time = timeInUsecs;
    }

    return (timeInUsecs-g_start_time);
}

uint64_t appLogGetTimeInUsec()
{
    #ifdef APP_TIMER_USE_GLOBAL_TIME
    return appLogGetGlobalTimeInUsec();
    #else
    return appLogGetLocalTimeInUsec();
    #endif
}

void appLogWaitMsecs(uint32_t time_in_msecs)
{
#if _POSIX_C_SOURCE >= 199309L
    struct timespec delay_time = {0}, remain_time = {0};
    int ret;

    delay_time.tv_sec  = time_in_msecs/1000;
    delay_time.tv_nsec = (time_in_msecs%1000)*1000000;

    do
    {
        ret = nanosleep(&delay_time, &remain_time);
        if(ret < 0 && remain_time.tv_sec > 0 && remain_time.tv_nsec > 0)
        {
            /* restart for remaining time */
            delay_time = remain_time;
        }
        else
        {
            break;
        }
    } while(1);
#else
    usleep(msec * 1000);
#endif
}
