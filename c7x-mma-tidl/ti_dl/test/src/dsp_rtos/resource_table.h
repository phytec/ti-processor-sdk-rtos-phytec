#include <ti/drv/ipc/include/ipc_rsctypes.h>

#include "memmap.h"

#define RPMSG_VRING_SIZE   (0x10000)

#define C7X_1_MEM_RPMSG_VRING0         (DDR_C7x_1_IPC_ADDR)
#define C7X_1_MEM_RPMSG_VRING1         (DDR_C7x_1_IPC_ADDR  + RPMSG_VRING_SIZE)

/*
 * Sizes of the virtqueues (expressed in number of buffers supported,
 * and must be power of 2)
 */
#define C7X_RPMSG_VQ0_SIZE      256
#define C7X_RPMSG_VQ1_SIZE      256

/* flip up bits whose indices represent features we support */
#define RPMSG_C7X_DSP_FEATURES  1

#if (defined (FREERTOS))
#include "stddef.h"
#pragma DATA_SECTION(Ipc_traceBuffer, ".tracebuf")
#define IPC_TRACE_MAX_LINE_LENGTH     (512)
#define IPC_TRACE_BUFFER_MAX_SIZE     (0x80000)
char Ipc_traceBuffer[IPC_TRACE_BUFFER_MAX_SIZE];
#define TRACEBUFADDR ((uintptr_t)&Ipc_traceBuffer)
#else
extern char xdc_runtime_SysMin_Module_State_0_outbuf__A;
#define TRACEBUFADDR ((uintptr_t)&xdc_runtime_SysMin_Module_State_0_outbuf__A)
#endif

const Ipc_ResourceTable ti_ipc_remoteproc_ResourceTable __attribute__ ((section (".resource_table"), aligned (4096))) = 
{
    1,                   /* we're the first version that implements this */
    NUM_ENTRIES,         /* number of entries in the table */
    0, 0,                /* reserved, must be zero */

    /* offsets to entries */
    {
        offsetof(Ipc_ResourceTable, rpmsg_vdev),
        offsetof(Ipc_ResourceTable, trace),
    },

    /* rpmsg vdev entry */
    {
        TYPE_VDEV, VIRTIO_ID_RPMSG, 0,
        RPMSG_C7X_DSP_FEATURES, 0, 0, 0, 2, { 0, 0 },
        /* no config data */
    },
    /* the two vrings */
    { C7X_1_MEM_RPMSG_VRING0, 4096, C7X_RPMSG_VQ0_SIZE, 1, 0 },
    { C7X_1_MEM_RPMSG_VRING1, 4096, C7X_RPMSG_VQ1_SIZE, 2, 0 },

    {
        (TRACE_INTS_VER1 | TYPE_TRACE), TRACEBUFADDR, 0x80000, 0, "trace:r5f0",
    },
};

