/*
 *
 * Copyright (c) 2018-2024 Texas Instruments Incorporated
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

#include <app.h>
#include <utils/console_io/include/app_log.h>
#include <utils/timer/include/app_timer.h>
#include <utils/misc/include/app_misc.h>
#include <utils/rtos/include/app_rtos.h>
#include <stdio.h>
#include <string.h>
#include <HwiP.h>
#include <CacheP.h>
#include <app_mem_map.h>
#include <app_ipc_rsctable.h>

#if !defined(MCU_PLUS_SDK)
#include <osal.h>
#include <ti/csl/soc.h>
#include <ti/csl/csl_clec.h>
#else
#include <DebugP.h>
#include <kernel/dpl/ClockP.h>
#include <hw_include/cslr_soc.h>
#include <hw_include/csl_clec.h>
#include <hw_include/am62ax/cslr_soc_baseaddress.h>
#endif

#if (defined (FREERTOS))
#if !defined(MCU_PLUS_SDK)
#include <ti/kernel/freertos/portable/TI_CGT/c7x/Cache.h>
#include <ti/kernel/freertos/portable/TI_CGT/c7x/Hwi.h>
#include <ti/kernel/freertos/portable/TI_CGT/c7x/Mmu.h>
#else
#include <kernel/nortos/dpl/c75/CacheP_c75.h>
#include <kernel/nortos/dpl/c75/HwiP_c75.h>
#include <kernel/nortos/dpl/c75/MmuP_c75.h>
#include <ipc_notify.h>
#include <ipc_notify/v0/ipc_notify_v0.h>
#include <soc.h>
#include <ClockP.h>
#include <SystemP.h>
#endif
#else
#include <ti/sysbios/family/c7x/Cache.h>
#include <ti/sysbios/family/c7x/Hwi.h>
#include <ti/sysbios/family/c7x/Mmu.h>
#endif

#define ENABLE_C7X_CACHE_WRITE_THROUGH

#if (defined (MCU_PLUS_SDK))

extern void vTaskStartScheduler( void );

#define TIMER2_CLOCK_SRC_MUX_ADDR (0x1081B8u)
#define TIMER2_CLOCK_SRC_MCU_HFOSC0 (0x0u)
#define TIMER2_BASE_ADDR     (0x2420000u)

void IpcNotify_getConfig(IpcNotify_InterruptConfig **interruptConfig, uint32_t *interruptConfigNum)
{
    /* extern globals that are specific to this core */
    extern IpcNotify_InterruptConfig gIpcNotifyInterruptConfig_c75ss0_0[];
    extern uint32_t gIpcNotifyInterruptConfigNum_c75ss0_0;

    *interruptConfig = &gIpcNotifyInterruptConfig_c75ss0_0[0];
    *interruptConfigNum = gIpcNotifyInterruptConfigNum_c75ss0_0;
}
ClockP_Config gClockConfig = {
    .timerBaseAddr = TIMER2_BASE_ADDR,
    .timerHwiIntNum = 10,
    .eventId = 378,
    .timerInputClkHz = 25000000,
    .timerInputPreScaler = 1,
    .usecPerTick = 1000,
};

/* ----------- DebugP ----------- */
void putchar_(char character)
{
    /* Output to CCS console */
    putchar(character);    
}
#endif 

#define C7x_EL2_SNOOP_CFG_REG (0x7C00000Cu)

#define DISABLE_C7X_SNOOP_FILTER    (0) /*On reset value is 0*/
#define ENABLE_C7X_MMU_TO_DMC_SNOOP (1) /*On reset value is 1*/
#define ENABLE_C7X_PMC_TO_DMC_SNOOP (0) /*On reset value is 1*/
#define ENABLE_C7X_SE_TO_DMC_SNOOP  (1) /*On reset value is 1*/
#define ENABLE_C7X_DRU_TO_DMC_SNOOP (1) /*On reset value is 1*/
#define ENABLE_C7X_SOC_TO_DMC_SNOOP (1) /*On reset value is 1*/

static void setC7xSnoopCfgReg()
{
    volatile uint32_t *pReg = (uint32_t *)C7x_EL2_SNOOP_CFG_REG;

    /* This operation overrides the existing value of snoop config!*/
    *pReg = (uint32_t)(0u);
}


static void appMain(void* arg0, void* arg1)
{
    appUtilsTaskInit();
    appInit();
    appRun();
    #if 1
    while(1)
    {
        appLogWaitMsecs(100u);
    }
    #else
    appDeInit();
    #endif
}

void StartupEmulatorWaitFxn (void)
{
    volatile uint32_t enableDebug = 0;
    do
    {
    }while (enableDebug);
}

/* IMPORTANT NOTE: For C7x,
 * - stack size and stack ptr MUST be 8KB aligned
 * - AND min stack size MUST be 16KB
 * - AND stack assigned for task context is "size - 8KB"
 *       - 8KB chunk for the stack area is used for interrupt handling in this task context
 */
static uint8_t gTskStackMain[64*1024]
__attribute__ ((section(".bss:taskStackSection")))
__attribute__ ((aligned(8192)))
    ;

#if defined(ENABLE_C7X_CACHE_WRITE_THROUGH)

#ifdef __cplusplus
extern "C" {
#endif

void temp_CSL_c7xSetL1DCFG(uint64_t param);

#ifdef __cplusplus
}
#endif

__asm__ __volatile__("temp_CSL_c7xSetL1DCFG: \n"
" MVC .S1 A4, ECR256 ; \n"
" RET .B1\n"
);

static void configureC7xL1DCacheAsWriteThrough()
{
    volatile uint64_t l1dcfg = 0x1U;
#if !defined(MCU_PLUS_SDK)
    Cache_wbInvL1dAll();
#else
    CacheP_wbInvAll(CacheP_TYPE_L1D);
#endif
    temp_CSL_c7xSetL1DCFG(l1dcfg);
}
#endif

static void appC75ClecInitDru(void)
{
    CSL_ClecEventConfig   cfgClec;
    CSL_CLEC_EVTRegs   *clecBaseAddr = (CSL_CLEC_EVTRegs*) CSL_C7X256V0_CLEC_BASE;

    uint32_t i;
    uint32_t dru_input_num   = 16;
    
    /* DRU Local event start ref: AM62A clec spec*/
    uint32_t dru_input_start = 128;

    /* program CLEC events from DRU used for polling by TIDL
     * to map to required events in C7x
     */
    for(i = dru_input_start; i < (dru_input_start + dru_input_num); i++)
    {
        /* Configure CLEC */
        cfgClec.secureClaimEnable = FALSE;
        cfgClec.evtSendEnable     = TRUE;
        cfgClec.rtMap             = CSL_CLEC_RTMAP_CPU_ALL;
        cfgClec.extEvtNum         = 0;
        cfgClec.c7xEvtNum         = (i - dru_input_start) + 32;
        CSL_clecConfigEvent(clecBaseAddr, i, &cfgClec);
    }
}

int main(void)
{
    app_rtos_task_params_t tskParams;
    app_rtos_task_handle_t task;

    StartupEmulatorWaitFxn();

#if !defined(MCU_PLUS_SDK)
    OS_init();
#else
    /* set timer clock source */
    SOC_controlModuleUnlockMMR(SOC_DOMAIN_ID_MAIN, 2);
    *(volatile uint32_t*)(TIMER2_CLOCK_SRC_MUX_ADDR) = TIMER2_CLOCK_SRC_MCU_HFOSC0;
    SOC_controlModuleLockMMR(SOC_DOMAIN_ID_MAIN, 2);
    /* initialize Clock */
    ClockP_init();
#endif

    appC75ClecInitDru();

    appRtosTaskParamsInit(&tskParams);
    tskParams.priority = 8u;
    tskParams.stack = gTskStackMain;
    tskParams.stacksize = sizeof (gTskStackMain);
    tskParams.taskfxn = &appMain;
    task = appRtosTaskCreate(&tskParams);

#if !defined(MCU_PLUS_SDK)
    if(NULL == task)
    {
        OS_stop();
    }
    OS_start();
#else
    DebugP_assert(task != NULL);
    vTaskStartScheduler();
    /* The following line should never be reached because vTaskStartScheduler()
    will only return if there was not enough FreeRTOS heap memory available to
    create the Idle and (if configured) Timer tasks.  Heap management, and
    techniques for trapping heap exhaustion, are described in the book text. */
    DebugP_assertNoLog(0);
#endif

    return 0;
}

uint32_t g_app_rtos_c7x_mmu_map_error = 0;

void appMmuMap(Bool is_secure)
{

#if !defined(MCU_PLUS_SDK)
    Bool            retVal;
    Mmu_MapAttrs    attrs;

    uint32_t ns = 1;

    if(is_secure)
        ns = 0;
    else
        ns = 1;

    Mmu_initMapAttrs(&attrs);

    attrs.attrIndx = Mmu_AttrIndx_MAIR0;
    attrs.ns = ns;

    retVal = Mmu_map(0x00000000, 0x00000000, 0x20000000, &attrs, is_secure);
    if(retVal==UFALSE)
    {
        goto mmu_exit;
    }

    retVal = Mmu_map(0x20000000, 0x20000000, 0x20000000, &attrs, is_secure);
    if(retVal==UFALSE)
    {
        goto mmu_exit;
    }

    retVal = Mmu_map(0x40000000, 0x40000000, 0x20000000, &attrs, is_secure);
    if(retVal==UFALSE)
    {
        goto mmu_exit;
    }

    retVal = Mmu_map(0x60000000, 0x60000000, 0x10000000, &attrs, is_secure);
    if(retVal==UFALSE)
    {
        goto mmu_exit;
    }

    retVal = Mmu_map(0x70000000, 0x70000000, 0x10000000, &attrs, is_secure);
    if(retVal==UFALSE)
    {
        goto mmu_exit;
    }

    retVal = Mmu_map(0x7C200000U, 0x7C200000U, 0x00100000U, &attrs, is_secure); /* CLEC */
    if(retVal==UFALSE)
    {
        goto mmu_exit;
    }

    retVal = Mmu_map(0x7C400000U, 0x7C400000U, 0x00100000U, &attrs, is_secure); /* DRU */
    if(retVal==UFALSE)
    {
        goto mmu_exit;
    }

    Mmu_initMapAttrs(&attrs);

    attrs.attrIndx = Mmu_AttrIndx_MAIR4;
    attrs.ns = ns;

    retVal = Mmu_map(APP_LOG_MEM_ADDR, APP_LOG_MEM_ADDR, APP_LOG_MEM_SIZE, &attrs, is_secure);
    if(retVal == UFALSE)
    {
        goto mmu_exit;
    }

    retVal = Mmu_map(APP_FILEIO_MEM_ADDR, APP_FILEIO_MEM_ADDR, APP_FILEIO_MEM_SIZE, &attrs, is_secure);
    if(retVal == UFALSE)
    {
        goto mmu_exit;
    }

    retVal = Mmu_map(TIOVX_OBJ_DESC_MEM_ADDR, TIOVX_OBJ_DESC_MEM_ADDR, TIOVX_OBJ_DESC_MEM_SIZE, &attrs, is_secure);
    if(retVal == UFALSE)
    {
        goto mmu_exit;
    }

    retVal = Mmu_map(IPC_VRING_MEM_ADDR, IPC_VRING_MEM_ADDR, IPC_VRING_MEM_SIZE, &attrs, is_secure);
    if(retVal == UFALSE)
    {
        goto mmu_exit;
    }

	retVal = Mmu_map(DDR_C7x_1_IPC_ADDR, DDR_C7x_1_IPC_ADDR, 2*DDR_C7x_1_IPC_SIZE, &attrs, is_secure);
    if(retVal == UFALSE)
    {
        goto mmu_exit;
    }

    retVal = Mmu_map(TIOVX_LOG_RT_MEM_ADDR, TIOVX_LOG_RT_MEM_ADDR, TIOVX_LOG_RT_MEM_SIZE, &attrs, is_secure);
    if(retVal == UFALSE)
    {
        goto mmu_exit;
    }

    retVal = Mmu_map(DDR_C7X_1_LOCAL_HEAP_NON_CACHEABLE_ADDR, DDR_C7X_1_LOCAL_HEAP_NON_CACHEABLE_ADDR, DDR_C7X_1_LOCAL_HEAP_NON_CACHEABLE_SIZE, &attrs, is_secure);
    if(retVal == UFALSE)
    {
        goto mmu_exit;
    }

    retVal = Mmu_map(DDR_C7X_1_SCRATCH_NON_CACHEABLE_ADDR, DDR_C7X_1_SCRATCH_NON_CACHEABLE_ADDR, DDR_C7X_1_SCRATCH_NON_CACHEABLE_SIZE, &attrs, is_secure);
    if(retVal == UFALSE)
    {
        goto mmu_exit;
    }

    Mmu_initMapAttrs(&attrs);
    attrs.attrIndx = Mmu_AttrIndx_MAIR7;
    attrs.ns = ns;

    retVal = Mmu_map(L2RAM_C7x_1_MAIN_ADDR, L2RAM_C7x_1_MAIN_ADDR, 0x01000000, &attrs, is_secure);
    if(retVal == UFALSE)
    {
        goto mmu_exit;
    }

    retVal = Mmu_map(L2RAM_C7x_1_AUX_ADDR, L2RAM_C7x_1_AUX_ADDR, 0x01000000, &attrs, is_secure);
    if(retVal == UFALSE)
    {
        goto mmu_exit;
    }

    retVal = Mmu_map(DDR_C7x_1_DTS_ADDR, DDR_C7x_1_DTS_ADDR, DDR_C7x_1_DTS_SIZE, &attrs, is_secure);
    if(retVal == UFALSE)
    {
        goto mmu_exit;
    }

    retVal = Mmu_map(DDR_SHARED_MEM_ADDR, DDR_SHARED_MEM_ADDR, DDR_SHARED_MEM_SIZE, &attrs, is_secure);
    if(retVal == UFALSE)
    {
        goto mmu_exit;
    }

    retVal = Mmu_map(DDR_C7X_1_LOCAL_HEAP_ADDR, DDR_C7X_1_LOCAL_HEAP_ADDR, DDR_C7X_1_LOCAL_HEAP_SIZE, &attrs, is_secure);
    if(retVal == UFALSE)
    {
        goto mmu_exit;
    }

    retVal = Mmu_map(DDR_C7X_1_SCRATCH_ADDR, DDR_C7X_1_SCRATCH_ADDR, DDR_C7X_1_SCRATCH_SIZE, &attrs, is_secure);
    if(retVal == UFALSE)
    {
        goto mmu_exit;
    }

mmu_exit:
    if(retVal == UFALSE)
    {
        g_app_rtos_c7x_mmu_map_error++;
    }
#else

    int32_t retVal;
    MmuP_MapAttrs    attrs;

    MmuP_MapAttrs_init(&attrs);

    attrs.attrIndx = MMUP_ATTRINDX_MAIR0;

    retVal = MmuP_map(0x00000000, 0x00000000, 0x20000000, &attrs);
    if(retVal==SystemP_FAILURE)
    {
        goto mmu_exit;
    }

    retVal = MmuP_map(0x20000000, 0x20000000, 0x20000000, &attrs);
    if(retVal==SystemP_FAILURE)
    {
        goto mmu_exit;
    }

    retVal = MmuP_map(0x40000000, 0x40000000, 0x20000000, &attrs);
    if(retVal==SystemP_FAILURE)
    {
        goto mmu_exit;
    }

    retVal = MmuP_map(0x60000000, 0x60000000, 0x10000000, &attrs);
    if(retVal==SystemP_FAILURE)
    {
        goto mmu_exit;
    }

    retVal = MmuP_map(0x70000000, 0x70000000, 0x10000000, &attrs);
    if(retVal==SystemP_FAILURE)
    {
        goto mmu_exit;
    }

    retVal = MmuP_map(0x7C200000U, 0x7C200000U, 0x00100000U, &attrs); /* CLEC */
    if(retVal==SystemP_FAILURE)
    {
        goto mmu_exit;
    }

    retVal = MmuP_map(0x7C400000U, 0x7C400000U, 0x00100000U, &attrs); /* DRU */
    if(retVal==SystemP_FAILURE)
    {
        goto mmu_exit;
    }

    MmuP_MapAttrs_init(&attrs);
    attrs.attrIndx = MMUP_ATTRINDX_MAIR0;

    retVal = MmuP_map(L2RAM_C7x_1_MAIN_ADDR, L2RAM_C7x_1_MAIN_ADDR, 0x01000000, &attrs);
    if(retVal == SystemP_FAILURE)
    {
        goto mmu_exit;
    }
    retVal = MmuP_map(L2RAM_C7x_1_AUX_ADDR, L2RAM_C7x_1_AUX_ADDR, 0x01000000, &attrs);
    if(retVal == SystemP_FAILURE)
    {
        goto mmu_exit;
    }
    attrs.attrIndx = MMUP_ATTRINDX_MAIR7;


    retVal = MmuP_map(DDR_C7x_1_DTS_ADDR, DDR_C7x_1_DTS_ADDR, DDR_C7x_1_DTS_SIZE, &attrs);
    if(retVal == SystemP_FAILURE)
    {
        goto mmu_exit;
    }

    retVal = MmuP_map(DDR_SHARED_MEM_ADDR, DDR_SHARED_MEM_ADDR, DDR_SHARED_MEM_SIZE, &attrs);
    if(retVal == SystemP_FAILURE)
    {
        goto mmu_exit;
    }

    retVal = MmuP_map(DDR_C7X_1_LOCAL_HEAP_ADDR, DDR_C7X_1_LOCAL_HEAP_ADDR, DDR_C7X_1_LOCAL_HEAP_SIZE, &attrs);
    if(retVal == SystemP_FAILURE)
    {
        goto mmu_exit;
    }

    retVal = MmuP_map(DDR_C7X_1_SCRATCH_ADDR, DDR_C7X_1_SCRATCH_ADDR, DDR_C7X_1_SCRATCH_SIZE, &attrs);
    if(retVal == SystemP_FAILURE)
    {
        goto mmu_exit;
    }

    MmuP_MapAttrs_init(&attrs);

    attrs.attrIndx = MMUP_ATTRINDX_MAIR4;

    retVal = MmuP_map(APP_LOG_MEM_ADDR, APP_LOG_MEM_ADDR, APP_LOG_MEM_SIZE, &attrs);
    if(retVal == SystemP_FAILURE)
    {
        goto mmu_exit;
    }

    retVal = MmuP_map(APP_FILEIO_MEM_ADDR, APP_FILEIO_MEM_ADDR, APP_FILEIO_MEM_SIZE, &attrs);
    if(retVal == SystemP_FAILURE)
    {
        goto mmu_exit;
    }

    retVal = MmuP_map(TIOVX_OBJ_DESC_MEM_ADDR, TIOVX_OBJ_DESC_MEM_ADDR, TIOVX_OBJ_DESC_MEM_SIZE, &attrs);
    if(retVal == SystemP_FAILURE)
    {
        goto mmu_exit;
    }

    retVal = MmuP_map(IPC_VRING_MEM_ADDR, IPC_VRING_MEM_ADDR, IPC_VRING_MEM_SIZE, &attrs);
    if(retVal == SystemP_FAILURE)
    {
        goto mmu_exit;
    }

	retVal = MmuP_map(DDR_C7x_1_IPC_ADDR, DDR_C7x_1_IPC_ADDR, 2*DDR_C7x_1_IPC_SIZE, &attrs);
    if(retVal == SystemP_FAILURE)
    {
        goto mmu_exit;
    }

    retVal = MmuP_map(TIOVX_LOG_RT_MEM_ADDR, TIOVX_LOG_RT_MEM_ADDR, TIOVX_LOG_RT_MEM_SIZE, &attrs);
    if(retVal == SystemP_FAILURE)
    {
        goto mmu_exit;
    }

    retVal = MmuP_map(DDR_C7X_1_LOCAL_HEAP_NON_CACHEABLE_ADDR, DDR_C7X_1_LOCAL_HEAP_NON_CACHEABLE_ADDR, DDR_C7X_1_LOCAL_HEAP_NON_CACHEABLE_SIZE, &attrs);
    if(retVal == SystemP_FAILURE)
    {
        goto mmu_exit;
    }

    retVal = MmuP_map(DDR_C7X_1_SCRATCH_NON_CACHEABLE_ADDR, DDR_C7X_1_SCRATCH_NON_CACHEABLE_ADDR, DDR_C7X_1_SCRATCH_NON_CACHEABLE_SIZE, &attrs);
    if(retVal == SystemP_FAILURE)
    {
        goto mmu_exit;
    }

mmu_exit:
    if(retVal == SystemP_FAILURE)
    {
        g_app_rtos_c7x_mmu_map_error++;
    }
#endif

    return;
}

void appCacheInit()
{
    /* Going with default cache setting on reset */
    /* L1P - 32kb$, L1D - 64kb$, L2 - 0kb$ */
#if defined(ENABLE_C7X_CACHE_WRITE_THROUGH)
    configureC7xL1DCacheAsWriteThrough();
#endif

    setC7xSnoopCfgReg();

}

#if !defined(MCU_PLUS_SDK)
void InitMmu(void)
#else
void MmuP_setConfig(void)
#endif
{
    /* This is for debug purpose - see the description of function header */
    g_app_rtos_c7x_mmu_map_error = 0;

    /* There is no secure mode in C7504 */
    appMmuMap(UFALSE);

    /* Initialize clec */
    HwiP_configClecAccessCtrl();

    appCacheInit();
}

uint64_t appUdmaVirtToPhyAddrConversion(const void *virtAddr,
                                      uint32_t chNum,
                                      void *appData)
{

  return (uint64_t)virtAddr;
}
