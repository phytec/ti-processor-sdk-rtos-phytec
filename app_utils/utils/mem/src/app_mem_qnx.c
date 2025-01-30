/*
 *
 * Copyright (c) 2018 Texas Instruments Incorporated
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
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/mman_peer.h>
#include <sys/neutrino.h>
#include <errno.h>
#include <hw/inout.h>
#include <string.h>
#include <CacheP.h>
#include <process.h>
#include <pthread.h>

#include <app_mem_priv.h>
#include <utils/mem/include/app_mem.h>
#include <SharedMemoryAllocatorUsr.h>

//#define APP_MEM_DEBUG
/**
 * \brief Memory module initialization parameters
 */
typedef struct {

} app_qnx_init_prm_t;

//static app_qnx_init_prm_t g_app_mem_init_prm;
#define MAX_BUFS 10240

typedef struct
{
    pthread_mutex_t mem_mutex;

    uint32_t num_allocs;
    uint32_t num_maps;

}app_mem_obj_t;


typedef struct
{
    /* Shared memory buffer */
    shm_buf buf;
    /* Value of 1 indicates that it is natively allocated */
    uint8_t native_alloc;
    /* Value of 0 indicates that it is not in use, value of 1 indicates that it is in use */
    uint8_t is_buf_used;
}app_mem_bufs_t;

/*GLOBALS*/
app_mem_obj_t g_app_mem_obj;

app_mem_bufs_t g_app_mem_bufs[MAX_BUFS];

int32_t appMemInit(app_mem_init_prm_t *prm)
{
    app_mem_obj_t *obj = &g_app_mem_obj;

    int32_t status = 0;

    /* Initialize the pthread mutex */
    pthread_mutex_init(&obj->mem_mutex, NULL);

    obj->num_allocs = 0;
    obj->num_maps   = 0;

    #ifdef APP_MEM_DEBUG
    printf("MEM: Init ... !!!\n");
    #endif

    /* Get I/O privilege */
    ThreadCtl (_NTO_TCTL_IO, 0);

    memset(&g_app_mem_bufs[0], 0, (sizeof(app_mem_bufs_t) * MAX_BUFS));

    #ifdef APP_MEM_DEBUG
    printf("MEM: Init ... Done !!!\n");
    #endif
    return(status);
}

int32_t appMemDeInit(void)
{
    int32_t status = 0;

    #ifdef APP_MEM_DEBUG
    printf("MEM: Deinit ... empty function !!!\n");
    printf("MEM: Deinit ... Done !!!\n");
    #endif

    return(status);
}


// Don't care for now, will be called for OpenGL
uint64_t appMemGetDmaBufFd(void *virPtr, volatile uint32_t *dmaBufFdOffset)
{
    app_mem_obj_t *obj = &g_app_mem_obj;

    uint64_t physAddr = (uint32_t)-1;
    int i = 0;

    #ifdef APP_MEM_DEBUG
    printf("MEM: appMemGetDmaBufFd ... !!!\n");
    #endif

    /* Enter critical section */
    pthread_mutex_lock(&obj->mem_mutex);

    /* Find buffer */
    for(i=0; i < MAX_BUFS; i++)
    {
        if((void *) g_app_mem_bufs[i].buf.vir_addr == virPtr)
        {
            break;
        }
    }
    if(i == MAX_BUFS)
    {
        #ifdef APP_MEM_DEBUG
        printf("appMemGetDmaBufFd: Unable to locate buffer\n");
        #endif
    }
    else
    {
        physAddr = g_app_mem_bufs[i].buf.phy_addr;
    }
    /* Exit critical section */
    pthread_mutex_unlock(&obj->mem_mutex);

    return(physAddr);
}

int32_t appMemTranslateDmaBufFd(uint64_t dmaBufFd, uint32_t size, uint64_t *virtPtr, uint64_t *phyPtr)
{
    app_mem_obj_t *obj = &g_app_mem_obj;

    int32_t status = 0;
    int i = 0, free_buf_idx = -1;

    #ifdef APP_MEM_DEBUG
    printf("MEM: appMemTranslateDmaBufFd ... !!!\n");
    #endif

    /* Enter critical section */
    pthread_mutex_lock(&obj->mem_mutex);

    /* Find buffer */
    for(i=0; i < MAX_BUFS; i++)
    {
        if( (g_app_mem_bufs[i].buf.phy_addr == dmaBufFd) )
        {
            break;
        }
        else if ( (0 == g_app_mem_bufs[i].is_buf_used) &&
                  (-1 == free_buf_idx) )
        {
            free_buf_idx = i;
        }
    }

    if( i == MAX_BUFS )
    {
        if (-1 == free_buf_idx)
        {
            #ifdef APP_MEM_DEBUG
            printf("appMemTranslateDmaBufFd: Unable to locate buffer and no free buffers available\n");
            #endif
            status = -1;
        }
        else
        {
            i = free_buf_idx;
        }
    }

    if (0==status)
    {
        if (1 == g_app_mem_bufs[i].is_buf_used)
        {
            *virtPtr = g_app_mem_bufs[i].buf.vir_addr;
        }
        else
        {
            /* Since the virtual pointer is not used in this process, map the virtual pointer to this process */
            *virtPtr = (uint64_t)mmap64(NULL, size, PROT_READ | PROT_WRITE, MAP_PHYS | MAP_SHARED, NOFD, dmaBufFd);

            if (virtPtr == MAP_FAILED)
            {
                perror(__FUNCTION__);
                virtPtr = NULL;
                status = -1;
            }
            else
            {
                int pid;

                /* Note: retaining the pid given that the shm_buf structure contains this parameter */
                pid = getpid();

                /* Virtual pointer in the buffer structure set to newly mapped virtual pointer and pid updated */
                g_app_mem_bufs[i].buf.vir_addr = (uint64_t)(*virtPtr);
                g_app_mem_bufs[i].buf.pid = pid;
                g_app_mem_bufs[i].buf.size = size;
                g_app_mem_bufs[i].buf.phy_addr = dmaBufFd;
                g_app_mem_bufs[i].native_alloc = 0;
                g_app_mem_bufs[i].is_buf_used = 1;
                obj->num_maps++;
            }
        }

        *phyPtr  = g_app_mem_bufs[i].buf.phy_addr;
    }

    /* Exit critical section */
    pthread_mutex_unlock(&obj->mem_mutex);

    return(status);
}

/* Maintained for compatibility with Linux */
void appMemCloseDmaBufFd(uint64_t dmaBufFd)
{
    return;
}

uint64_t appMemGetVirt2PhyBufPtr(uint64_t virtPtr, uint32_t heap_id)
{
    off_t   phyPtr = 0;
    size_t  len = 0;
    int32_t ret = 0;
    int64_t offset = 0;

    #ifdef APP_MEM_DEBUG
    printf("MEM: appMemGetVirt2PhyBufPtr ... !!!\n");
    #endif

    ret = mem_offset64((void *) virtPtr, NOFD, 4096, &offset, &len);
    if (ret) {
        printf("MEM: Error from mem_offset\n");
    }
    else
    {
        phyPtr = (uint64_t)offset;
    }

    #ifdef APP_MEM_DEBUG
    printf("MEM: phyPtr 0x%x len/%d for virtPtr/0x%lx\n",(uint32_t) phyPtr, (int) len, virtPtr);
    #endif
    return (uint64_t)(phyPtr);
}

void *appMemAlloc(uint32_t block, uint32_t size, uint32_t align)
{
    app_mem_obj_t *obj = &g_app_mem_obj;

    void *virtPtr = 0;
    int32_t status = 0;
    int i = 0;

    #ifdef APP_MEM_DEBUG
    printf("MEM: appMemAlloc ... !!!\n");
    #endif

    /* Enter critical section */
    pthread_mutex_lock(&obj->mem_mutex);

    /* Find empty buffer */
    for(i=0; i < MAX_BUFS; i++)
    {
        if(g_app_mem_bufs[i].is_buf_used == 0)
        {
            break;
        }
    }
    #ifdef APP_MEM_DEBUG
    printf("MEM: Using buf/%d\n",i);
    #endif
    if(align > 0) {
       status = SHM_alloc_aligned(size, align, &g_app_mem_bufs[i].buf);
       #ifdef APP_MEM_DEBUG
       printf("\nMEM : SHM_alloc_aligned(%d, %d) : %s\n", size, align, status?"FAILED":"PASSED");
       #endif
    }
    else {
       status = SHM_alloc(size, &g_app_mem_bufs[i].buf);
       #ifdef APP_MEM_DEBUG
       printf("\nMEM : SHM_alloc(%d) : %s\n", size, status?"FAILED":"PASSED");
       #endif
    }

    if(0!=status)
    {
       printf("MEM: allocation of size %d failed with status %d\n", size, status);
    }
    else
    {
        virtPtr = (void *) (g_app_mem_bufs[i].buf.vir_addr);
        g_app_mem_bufs[i].native_alloc = 1;
        g_app_mem_bufs[i].is_buf_used  = 1;
        obj->num_allocs++;
    }

    /* Exit critical section */
    pthread_mutex_unlock(&obj->mem_mutex);

    #ifdef APP_MEM_DEBUG
    printf("MEM: Allocated memory - virt base address = %p, phy/0x%08lx size = %d \n", (void *) virtPtr, g_app_mem_bufs[i].buf.phy_addr, size);
    #endif
    return(virtPtr);
}

int32_t appMemFree(uint32_t block, void *virtPtr, uint32_t size )
{
    app_mem_obj_t *obj = &g_app_mem_obj;

    int32_t status = 0;
    int i = 0;

    #ifdef APP_MEM_DEBUG
    printf("MEM: appMemFree ... !!!\n");
    #endif

    /* Enter critical section */
    pthread_mutex_lock(&obj->mem_mutex);

    /* Find buffer */
    for(i=0; i < MAX_BUFS; i++)
    {
        if( ((void *) g_app_mem_bufs[i].buf.vir_addr == virtPtr) )
        {
            break;
        }
    }
    if(i == MAX_BUFS)
    {
        #ifdef APP_MEM_DEBUG
        printf("MEM: Unable to locate buffer to free\n");
        #endif
    }
    else {
        #ifdef APP_MEM_DEBUG
        printf("MEM: Found virtPtr at buf/%d, virt/%0lx08x phy/%0lx08x size/%d\n",i, g_app_mem_bufs[i].buf.vir_addr, g_app_mem_bufs[i].buf.phy_addr, size);
        #endif
        if (1 == g_app_mem_bufs[i].native_alloc)
        {
            status = SHM_release(&g_app_mem_bufs[i].buf);
            if(0!=status)
            {
            #ifdef APP_MEM_DEBUG
                printf("MEM: unable to release shared memory virtual address/%p size %d \n", virtPtr, size);
            #endif
            }
            else
            {
                g_app_mem_bufs[i].buf.vir_addr = 0;
                /* Note: retaining the pid given that the shm_buf structure contains this parameter */
                g_app_mem_bufs[i].buf.pid = 0;
                g_app_mem_bufs[i].buf.size = 0;
                g_app_mem_bufs[i].buf.phy_addr = 0;
                g_app_mem_bufs[i].native_alloc = 0;
                g_app_mem_bufs[i].is_buf_used = 0;
                obj->num_allocs--;
            }
        }
        else
        {
            status = munmap(virtPtr, size);
            if(0!=status)
            {
                #ifdef APP_MEM_DEBUG
                printf("MEM: unable to unmap shared memory virtual address/%p size %d \n", virtPtr, size);
                #endif
            }
            else
            {
                g_app_mem_bufs[i].buf.vir_addr = 0;
                /* Note: retaining the pid given that the shm_buf structure contains this parameter */
                g_app_mem_bufs[i].buf.pid = 0;
                g_app_mem_bufs[i].buf.size = 0;
                g_app_mem_bufs[i].buf.phy_addr = 0;
                g_app_mem_bufs[i].native_alloc = 0;
                g_app_mem_bufs[i].is_buf_used = 0;
                obj->num_maps--;
            }
        }
    }
    virtPtr = NULL;

    /* Exit critical section */
    pthread_mutex_unlock(&obj->mem_mutex);

    return(status);
}

int32_t appMemStats(uint32_t block, app_mem_stats_t *stats)
{
    /* TBD */
    return(0);
}

void  appMemCacheInv(void *ptr, uint32_t size)
{
    /* Use the OSAL Cache APIs */
    CacheP_Inv(ptr, size);
}

void  appMemCacheWbInv(void *ptr, uint32_t size)
{
    /* Use the OSAL Cache APIs */
    CacheP_wbInv(ptr, size);
}

void  appMemCacheWb(void *ptr, uint32_t size)
{
    /* Use the OSAL Cache APIs */
    CacheP_wb(ptr, size);
}

/* Note: needed for linking tivx mem platform layer*/
int32_t appMemResetScratchHeap(uint32_t heap_id)
{
    return(0);
}

void appMemPrintMemAllocInfo()
{
    int i, cur_bufs_used = 0, cur_num_allocs = 0, cur_num_maps = 0;
    app_mem_obj_t *obj = &g_app_mem_obj;

    /* Enter critical section */
    pthread_mutex_lock(&obj->mem_mutex);

    cur_num_allocs = obj->num_allocs;
    cur_num_maps   = obj->num_maps;

    for(i=0; i < MAX_BUFS; i++)
    {
        if(g_app_mem_bufs[i].is_buf_used == 1)
        {
            cur_bufs_used++;
        }
    }

    /* Exit critical section */
    pthread_mutex_unlock(&obj->mem_mutex);

    printf("Total number of buffers allocated = %d\n", cur_num_allocs);
    printf("Total number of buffers mapped = %d\n", cur_num_maps);
    printf("Total number of buf array elements used = %d\n", cur_bufs_used);

    return;
}

uint32_t appMemGetNumAllocs()
{
    app_mem_obj_t *obj = &g_app_mem_obj;
    uint32_t num_allocs;

    /* Enter critical section */
    pthread_mutex_lock(&obj->mem_mutex);

    num_allocs = obj->num_allocs;

    /* Exit critical section */
    pthread_mutex_unlock(&obj->mem_mutex);

    return num_allocs;
}

uint32_t appMemGetNumMaps()
{
    app_mem_obj_t *obj = &g_app_mem_obj;
    uint32_t num_maps;

    /* Enter critical section */
    pthread_mutex_lock(&obj->mem_mutex);

    num_maps = obj->num_maps;

    /* Exit critical section */
    pthread_mutex_unlock(&obj->mem_mutex);

    return num_maps;
}

uint32_t appMemGetNumBufElements()
{
    app_mem_obj_t *obj = &g_app_mem_obj;
    uint32_t cur_bufs_used = 0, i;

    /* Enter critical section */
    pthread_mutex_lock(&obj->mem_mutex);

    for(i=0; i < MAX_BUFS; i++)
    {
        if(g_app_mem_bufs[i].is_buf_used == 1)
        {
            cur_bufs_used++;
        }
    }

    /* Exit critical section */
    pthread_mutex_unlock(&obj->mem_mutex);

    return cur_bufs_used;
}

uint64_t appMemShared2PhysPtr(uint64_t shared_ptr, uint32_t heap_id)
{
    return shared_ptr;
}

uint64_t appMemShared2TargetPtr(uint64_t shared_ptr)
{
    return shared_ptr;
}

void *appMemMap(void *phys_ptr, uint32_t size)
{
    uint32_t  pageSize = getpagesize();
    uintptr_t taddr;
    uint32_t  tsize;
    void     *virt_ptr = NULL;
#ifdef MEM_DEV_OPEN
    int32_t   status = 0;
    static int dev_mem_fd = -1;

    if(dev_mem_fd == -1)
    {
        dev_mem_fd = open("/dev/mem",O_RDWR|O_SYNC);
        if(dev_mem_fd  < 0)
        {
            printf("APP_LOG: ERROR: Unable to open /dev/mem !!!\n");
            status = -1;
        }
    }
    if(status==0 && dev_mem_fd >= 0)
    {
        #ifdef APP_LOG_DEBUG
        printf("APP_LOG: Mapping %p ...\n", phys_ptr);
        #endif
        /* Mapping this physical address to qnx user space */
        taddr = (uintptr_t)phys_ptr;
        tsize = size;

        /* Align the physical address to page boundary */
        tsize = appAlign(tsize + (taddr % pageSize), pageSize);
        taddr = appFloor(taddr, pageSize);

        virt_ptr  = mmap64(0, tsize,
                        (PROT_READ | PROT_WRITE | PROT_NOCACHE),
                        (MAP_SHARED), dev_mem_fd, taddr);
        if(virt_ptr==MAP_FAILED)
        {
            virt_ptr = NULL;
        }
        else
        {
            virt_ptr = (void*)((uintptr_t)virt_ptr + ((uintptr_t)phys_ptr % pageSize));
        }
        #ifdef APP_LOG_DEBUG
        printf("APP_LOG: Mapped %p -> %p of size %d bytes \n", phys_ptr, virt_ptr, size);
        #endif
    }
    if(virt_ptr==NULL)
    {
        printf("APP_LOG: ERROR: Unable to map memory @ %p of size %d bytes !!!\n", phys_ptr, size);
    }
#else
    #ifdef APP_LOG_DEBUG
    printf("APP_LOG: Mapping %p ...\n", phys_ptr);
    #endif

    /* Mapping this physical address to qnx user space */
    taddr = (uintptr_t)phys_ptr;
    tsize = size;

    /* Align the physical address to page boundary */
    tsize = appAlign(tsize + (taddr % pageSize), pageSize);
    taddr = appFloor(taddr, pageSize);


    virt_ptr  = mmap_device_memory(0, tsize, PROT_READ|PROT_WRITE|PROT_NOCACHE, 0, taddr);
    if(virt_ptr==MAP_FAILED)
    {
        virt_ptr = NULL;
    }
    else
    {
        virt_ptr = (void*)((uintptr_t)virt_ptr + ((uintptr_t)phys_ptr % pageSize));
    }
    #ifdef APP_LOG_DEBUG
    printf("APP_LOG: Mapped %p -> %p of size %d bytes \n", phys_ptr, virt_ptr, size);
    #endif
    if(virt_ptr==NULL)
    {
        printf("APP_LOG: ERROR: Unable to map memory @ %p of size %d bytes !!!\n", phys_ptr, size);
    }
#endif
    return virt_ptr;
}

int32_t appMemUnMap(void *virt_ptr, uint32_t size)
{
    int32_t status=0;
    uint32_t pageSize = getpagesize();
    uintptr_t taddr;
    uint32_t  tsize;

    #ifdef APP_LOG_DEBUG
    printf("APP_LOG: UnMapped memory at virtual address @ 0x%p of size %d bytes \n", virt_ptr, size);
    #endif

    taddr = (uint64_t)virt_ptr;
    tsize = size;

    tsize = appAlign(tsize + (taddr % pageSize), pageSize);
    taddr = appFloor(taddr, pageSize);

#ifdef MEM_DEV_OPEN
    munmap((void *)taddr, tsize);
#else
    munmap_device_memory((void *)taddr, tsize);
#endif

    return status;
}
