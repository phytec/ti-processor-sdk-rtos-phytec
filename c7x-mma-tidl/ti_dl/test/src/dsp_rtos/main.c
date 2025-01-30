#include <stdio.h>
#include <string.h>

#include <ti/csl/soc.h>
#include <ti/csl/csl_clec.h>
#include <ti/drv/sciclient/sciclient.h>

#if (defined (FREERTOS))
#include <ti/osal/osal.h>
#include <ti/osal/TaskP.h>
#include <ti/osal/HwiP.h>
#include <ti/osal/CacheP.h>
#include <ti/csl/arch/c7x/Cache.h>
#include <ti/csl/arch/c7x/Hwi.h>
#else
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/family/c7x/Hwi.h>
#include <ti/sysbios/family/c7x/Cache.h>
#endif

#include "memmap.h"
#include "platform_common.h"

#pragma DATA_SECTION (createParams, ".createParams");
TIDL_CreateParams  createParams;
#pragma DATA_SECTION (gIOParams,    ".gIOParams");
sTIDL_IOBufDesc_t  gIOParams;
#pragma DATA_SECTION (L1_SCRATCH,  ".l1ScratchBuf");
uint8_t L1_SCRATCH[L1_MEM_SIZE];
#pragma DATA_SECTION (L2_SCRATCH,  ".l2ScratchBuf");
uint8_t L2_SCRATCH[L2_MEM_SIZE];
#pragma DATA_SECTION (L3_SCRATCH,  ".l3ScratchBuf");
uint8_t L3_SCRATCH[L3_MEM_SIZE];
#pragma DATA_SECTION (L4_SCRATCH,  ".l4ScratchBuf");
uint8_t L4_SCRATCH[L4_MEM_SIZE];

uint8_t *L1Scratch;
uint8_t *L2Scratch;
uint8_t *L3Scratch;
uint8_t *L4Scratch;

#ifdef __C7100__
#define TIDL_CLEC_BASE_ADDR CSL_COMPUTE_CLUSTER0_CLEC_REGS_BASE
#else
#define TIDL_CLEC_BASE_ADDR (0x78000000UL)
#endif

int32_t tidlMain(int32_t argc, char **argv);
void fileio_init(void);

static uint8_t tidlTskStackMain[64*1024]
__attribute__ ((section(".bss:taskStackSection")))
__attribute__ ((aligned(8192)))
    ;

#if (defined (FREERTOS))
static void tidlMainWrapper(void* arg0, void* arg1)
#else
void tidlMainWrapper(UArg arg0, UArg arg1)
#endif
{
	int argc = 0;
	char **argv = NULL;
	FILE *fp;
	Sciclient_ConfigPrms_t sciClientCfg;

	Sciclient_configPrmsInit(&sciClientCfg);
	Sciclient_init(&sciClientCfg);

	L1Scratch = L1_SCRATCH;
	L2Scratch = L2_SCRATCH;
	L3Scratch = L3_SCRATCH;
	L4Scratch = L4_SCRATCH;

	fileio_init();

	/* Read a file named "arguments"                  */
	/* the file can be off the form:                  */
	/* appname --numFrames 1 --writeTraceLevel 1      */
	/* If no such file exists, fallback to argc = 1   */
	/*                                                */
	fp = fopen("arguments", "r");
	if(fp) {
		char *line = malloc(32 * 1024);
		char *res, *str;

		fgets(line, 32 * 1024, fp);
		if(line[strlen(line) - 1] == '\n') {
			line[strlen(line) - 1] = '\0';
		}
		line = realloc(line, strlen(line) + 1);

		str = line;
		while(res = strtok((char *)str, " "))
		{
			str = NULL;
			argv = (char **)realloc(argv, (argc + 1) * sizeof(char *));
			argv[argc] = res;
			argc++;
		}

		fclose(fp);
	}

	/* If we could not parse file or no file */
	if(!argc) {
		argc = 1;
		argv = malloc(sizeof(char *));
		argv[0] = "application";
	}

	/* add a trailing NULL */
	argv = (char **)realloc(argv, (argc + 1) * sizeof(char *));
	argv[argc] = NULL;

	tidlMain(argc, argv);
}

void tidlC7xClecInitDru(void)
{
    CSL_ClecEventConfig   cfgClec;
    CSL_CLEC_EVTRegs     *clecBaseAddr = (CSL_CLEC_EVTRegs*)TIDL_CLEC_BASE_ADDR;

    uint32_t eventNum;
    uint32_t dru_start_event = 192;
    uint32_t numDRUChannels   = 16;
    /* program CLEC events from DRU used for polling by TIDL
     * to map to required events in C7x
     */
    for(eventNum=dru_start_event; eventNum<(dru_start_event+numDRUChannels); eventNum++)
    {
        /* Configure CLEC */
        cfgClec.secureClaimEnable = FALSE;
        cfgClec.evtSendEnable     = TRUE;
        cfgClec.rtMap             = CSL_CLEC_RTMAP_CPU_ALL;
        cfgClec.extEvtNum         = 0;
        cfgClec.c7xEvtNum         = (eventNum-dru_start_event)+32;
        CSL_clecConfigEvent(clecBaseAddr, eventNum, &cfgClec);
    }
}

/* To set C71 timer interrupts */
void tidlTimerInterruptInit(void)
{
    CSL_ClecEventConfig   cfgClec;
    CSL_CLEC_EVTRegs     *clecBaseAddr = (CSL_CLEC_EVTRegs*)TIDL_CLEC_BASE_ADDR;

    uint32_t input         = 1249; /* Used for Timer Interrupt */
    uint32_t corepackEvent = 15;

    /* Configure CLEC */
    cfgClec.secureClaimEnable = FALSE;
    cfgClec.evtSendEnable     = TRUE;
    cfgClec.rtMap             = CSL_CLEC_RTMAP_CPU_ALL;
    cfgClec.extEvtNum         = 0;
    cfgClec.c7xEvtNum         = corepackEvent;
    CSL_clecConfigEvent(clecBaseAddr, input, &cfgClec);
    CSL_clecConfigEventLevel(clecBaseAddr, input, 0); /* configure interrupt as pulse */
    Hwi_setPriority(corepackEvent, 1);
}

void tidlC7xClecInitForNonSecAccess(void)
{
    CSL_ClecEventConfig   cfgClec;
    CSL_CLEC_EVTRegs     *clecBaseAddr = (CSL_CLEC_EVTRegs*)TIDL_CLEC_BASE_ADDR;

    uint32_t max_inputs      = 2048;
    uint32_t dru_start_event = 192;
    uint32_t numDRUChannels   = 16;
    uint32_t i;

    /* make secure claim bit to FALSE so that after we switch to non-secure mode
     * we can program the CLEC MMRs
     */
    for(i=0; i<max_inputs; i++)
    {
        cfgClec.secureClaimEnable = FALSE;
        cfgClec.evtSendEnable     = FALSE;
        cfgClec.rtMap             = CSL_CLEC_RTMAP_DISABLE;
        cfgClec.extEvtNum         = 0;
        cfgClec.c7xEvtNum         = 0;
        CSL_clecConfigEvent(clecBaseAddr, i, &cfgClec);
    }
    /* program CLEC events from DRU used for polling by TIDL
     * to map to required events in C7x
     */
    for(i=dru_start_event; i<(dru_start_event+numDRUChannels); i++)
    {
        /* Configure CLEC */
        cfgClec.secureClaimEnable = FALSE;
        cfgClec.evtSendEnable     = TRUE;
        cfgClec.rtMap             = CSL_CLEC_RTMAP_CPU_ALL;
        cfgClec.extEvtNum         = 0;
        cfgClec.c7xEvtNum         = (i-dru_start_event)+32;
        CSL_clecConfigEvent(clecBaseAddr, i, &cfgClec);
    }
}

void tidlCacheInit()
{
#if (defined (FREERTOS))    
  #if defined (__C7100__)  || defined (__C7120__)
     Cache_Size  cacheSize;
    /* init cache size here, since this needs to be done in secure mode */
    cacheSize.l1pSize = Cache_L1Size_32K;
    cacheSize.l1dSize = Cache_L1Size_32K;
    cacheSize.l2Size  = Cache_L2Size_64K;
  #endif
#else
    ti_sysbios_family_c7x_Cache_Size  cacheSize;
    
    /* init cache size here, since this needs to be done in secure mode */
    cacheSize.l1pSize = ti_sysbios_family_c7x_Cache_L1Size_32K;
    cacheSize.l1dSize = ti_sysbios_family_c7x_Cache_L1Size_32K;
    cacheSize.l2Size  = ti_sysbios_family_c7x_Cache_L2Size_64K;
#endif   
  #if defined (__C7100__)  || defined (__C7120__)
    Cache_setSize(&cacheSize);
  #endif
}

void InitMmu(void)
{
    tidlC7xClecInitForNonSecAccess();

    tidlMmuMap(false);
    tidlMmuMap(true);
    tidlCacheInit();
}

void tidlIdleLoop(void)
{
   __asm(" IDLE");
}

void tidlPerfStatsBiosLoadUpdate(void)
{
}
#if 0
int32_t debug_printf(const char *format, ...)
{
	va_list ap;
	int32_t ret;

	va_start(ap, format);
	ret = System_vprintf(format, ap);
	va_end(ap);

	return ret;
}
#endif

void * tidltb_alignMalloc(int32_t size, int32_t alignment)
{
  return memalign(alignment, size);
}

void tidltb_alignFree(void * ptr)
{
  free(ptr);
}


uint64_t tidltb_virtToPhyAddrConversion(const void *virtAddr,
                                      uint32_t chNum,
                                      void *appData)
{
  uint64_t phyAddr = (uint64_t)virtAddr;
  return phyAddr;
}

void tidltb_invalidateL1Dcache()
{
	Cache_wbInvL1dAll();
	return ;
}

#if (defined (FREERTOS))
typedef TaskP_Params Task_Params;
typedef TaskP_Handle Task_Handle;
#endif
int main(int argc, char **argv)
{
    Task_Params tskParams;
    Task_Handle task;

#if (defined (FREERTOS))       
    OS_init();
    tidlC7xClecInitDru();
    TaskP_Params_init(&tskParams);    
    tskParams.priority = 8u;
    tskParams.stack = tidlTskStackMain;
    tskParams.stacksize = sizeof (tidlTskStackMain);
    task = TaskP_create(tidlMainWrapper, &tskParams);
    if(NULL == task)
    {
        OS_stop();
    }
     OS_start();
#else
    tidlTimerInterruptInit();
    Task_Params_init(&tskParams);
    tskParams.priority = 8u;
    tskParams.stack = tidlTskStackMain;
    tskParams.arg0 = (UArg)NULL;
    tskParams.arg1 = (UArg)NULL;
    tskParams.stackSize = sizeof (tidlTskStackMain);
    task = Task_create(tidlMainWrapper, &tskParams, NULL);
    if(NULL == task)
    {
        BIOS_exit(0);
    }
    BIOS_start();
#endif
    return 0;
}
