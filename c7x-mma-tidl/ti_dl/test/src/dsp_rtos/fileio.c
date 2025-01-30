#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>

#include <ti/drv/ipc/ipc.h>

#include <c6x_migration.h>
#include <c7x.h>

#include "memmap.h"
#include "resource_table.h"
#include "platform_common.h"

uint32_t fileio_rpmsg_endpt = 0;
RPMessage_Handle fileio_rpmsg_handle;
uint32_t fileio_rpmsg_remote_endpt = 0;

#define IPC_RPMESSAGE_OBJ_SIZE  256U
#define RPMSG_DATA_SIZE         (256U*512U + IPC_RPMESSAGE_OBJ_SIZE)
#define VQ_BUF_SIZE             2048U

uint8_t  gSysVqBuf[VQ_BUF_SIZE]  __attribute__ ((section ("ipc_data_buffer"), aligned (8)));
uint8_t  gRspBuf[RPMSG_DATA_SIZE]  __attribute__ ((section ("ipc_data_buffer"), aligned (8)));
uint8_t  gCntrlBuf[RPMSG_DATA_SIZE]  __attribute__ ((section ("ipc_data_buffer"), aligned (8)));

static uint8_t ipcCntrlTskStack[64*1024]
__attribute__ ((section(".bss:taskStackSection")))
__attribute__ ((aligned(8192)))
    ;

#define COMMAND_OPEN     (0x01)
#define COMMAND_CLOSE    (0x02)
#define COMMAND_READ     (0x04)
#define COMMAND_WRITE    (0x08)
#define COMMAND_LSEEK    (0x10)

struct open_message {
	uint64_t command;
	int64_t  dev_fd;
	uint64_t flags;
	uint64_t filename_ptr;
	uint64_t filename_len;
} __attribute__((packed));

struct read_message {
	uint64_t command;
	uint64_t dev_fd;
	int64_t  count;
	uint64_t buffer_ptr;
} __attribute__((packed));

struct write_message {
	uint64_t command;
	uint64_t dev_fd;
	int64_t  count;
	uint64_t buffer_ptr;
} __attribute__((packed));

struct close_message {
	uint64_t command;
	uint64_t dev_fd;
}  __attribute__((packed));

struct lseek_message {
	uint64_t command;
	uint64_t dev_fd;
	int64_t  offset;
	uint64_t origin;
} __attribute__((packed));


uint32_t selfProcId = IPC_C7X_1;
uint32_t remoteProc[] =
{
    IPC_MPU1_0
};

static inline void Cache_wbInv(void * blockPtr, int32_t byteCnt)
{
    __se_cache_op(blockPtr, __DCCIC, byteCnt);
    _mfence();
    _mfence();
}

void ipc_init(void)
{
    Ipc_VirtIoParams  vqParam;
    RPMessage_Params  cntrlParam;
    RPMessage_Params  params;
    uint32_t          remote_endpt;
    uint32_t          remote_procid;
    uint16_t          len;
    uint8_t           sync_message[512];

    /* Step1 : Initialize the multiproc */
    Ipc_mpSetConfig(selfProcId, 1, &remoteProc[0]);

    /* Step2 : Initialize IPC */
    Ipc_init(NULL);

    /* Step3 : Wait for remote */
    Ipc_loadResourceTable((void*)&ti_ipc_remoteproc_ResourceTable);
    while(!Ipc_isRemoteReady(IPC_MPU1_0));

    /* Step4 : Initialize VQ
     * We are not doing RTOS <-> RTOS IPC
     * So no need to give a valid vringBaseAddr
     */
    assert(VQ_BUF_SIZE >= Ipc_getVqObjMemoryRequiredPerCore());
    vqParam.vqObjBaseAddr = &gSysVqBuf[0];
    vqParam.vqBufSize     = VQ_BUF_SIZE;
    vqParam.vringBaseAddr = NULL;
    vqParam.vringBufSize  = IPC_VRING_BUFFER_SIZE;
    vqParam.timeoutCnt    = 0xffffffff;
    Ipc_initVirtIO(&vqParam);

    /* Step4: Initialize RPMSG */
    RPMessageParams_init(&cntrlParam);
    cntrlParam.buf         = &gCntrlBuf[0];
    cntrlParam.bufSize     = RPMSG_DATA_SIZE;
    cntrlParam.stackBuffer = &ipcCntrlTskStack[0];
    cntrlParam.stackSize   = sizeof(ipcCntrlTskStack);
    RPMessage_init(&cntrlParam);

    RPMessageParams_init(&params);
    params.requestedEndpt = 13;
    params.buf = &gRspBuf[0];
    params.bufSize = RPMSG_DATA_SIZE;
    fileio_rpmsg_handle = RPMessage_create(&params, &fileio_rpmsg_endpt);

    RPMessage_announce(RPMESSAGE_ALL, fileio_rpmsg_endpt, "rpmsg_chrdev");

    RPMessage_recv(fileio_rpmsg_handle, &sync_message, &len, &remote_endpt, &remote_procid, IPC_RPMESSAGE_TIMEOUT_FOREVER);
    fileio_rpmsg_remote_endpt = remote_endpt;
}

void fileio_init(void)
{
	ipc_init();
}

int fileio_HOSTopen(const char *path, unsigned flags, int llv_fd)
{
	struct open_message open_msg;
	uint32_t          remote_endpt;
	uint32_t          remote_procid;
	uint16_t          len;
	char             *filename_ptr;
	uint32_t          filename_len;

	//debug_printf("%s: (%s, %x, %d)\n", __func__, path, flags, llv_fd);

	filename_len = strlen(path);
	filename_ptr = malloc(filename_len + 1);
	filename_len = snprintf(filename_ptr, filename_len + 1, "%s", path) + 1;

	remote_procid = IPC_MPU1_0;

	open_msg.command = COMMAND_OPEN;
	open_msg.flags = flags;
	open_msg.dev_fd = llv_fd;
	open_msg.filename_ptr = (uint64_t)filename_ptr;
	open_msg.filename_len = filename_len;

	Cache_wbInv((void *)filename_ptr, filename_len);

	RPMessage_send(fileio_rpmsg_handle, remote_procid, fileio_rpmsg_remote_endpt, fileio_rpmsg_endpt, &open_msg, sizeof(open_msg));
	RPMessage_recv(fileio_rpmsg_handle, &open_msg, &len, &remote_endpt, &remote_procid, IPC_RPMESSAGE_TIMEOUT_FOREVER);

	Cache_wbInv((void *)filename_ptr, filename_len);

	free(filename_ptr);
	return open_msg.dev_fd;
}

int fileio_HOSTread(int dev_fd, char *buf, unsigned count)
{
	struct read_message read_msg;
	uint32_t          remote_endpt;
	uint32_t          remote_procid;
	uint16_t          len;

	//debug_printf("%s: (%d, %p, %d)\n", __func__, dev_fd, buf, count);

	remote_procid = IPC_MPU1_0;

	read_msg.command = COMMAND_READ;
	read_msg.dev_fd = dev_fd;
	read_msg.count = count;
	read_msg.buffer_ptr = (uint64_t)tidltb_virtToPhyAddrConversion(buf, 0,  NULL);

	Cache_wbInv((void *)buf, read_msg.count);

	RPMessage_send(fileio_rpmsg_handle, remote_procid, fileio_rpmsg_remote_endpt, fileio_rpmsg_endpt, &read_msg, sizeof(read_msg));
	RPMessage_recv(fileio_rpmsg_handle, &read_msg, &len, &remote_endpt, &remote_procid, IPC_RPMESSAGE_TIMEOUT_FOREVER);

	Cache_wbInv((void *)buf, read_msg.count);

	return read_msg.count;
}

int fileio_HOSTwrite(int dev_fd, const char *buf, unsigned count)
{
	struct write_message write_msg;
	uint32_t          remote_endpt;
	uint32_t          remote_procid;
	uint16_t          len;

	//debug_printf("%s: (%d, %p, %d)\n", __func__, dev_fd, buf, count);

	remote_procid = IPC_MPU1_0;

	write_msg.command = COMMAND_WRITE;
	write_msg.dev_fd = dev_fd;
	write_msg.count = count;
	write_msg.buffer_ptr = (uint64_t)tidltb_virtToPhyAddrConversion(buf, 0,  NULL);

	Cache_wbInv((void *)buf, write_msg.count);

	RPMessage_send(fileio_rpmsg_handle, remote_procid, fileio_rpmsg_remote_endpt, fileio_rpmsg_endpt, &write_msg, sizeof(write_msg));
	RPMessage_recv(fileio_rpmsg_handle, &write_msg, &len, &remote_endpt, &remote_procid, IPC_RPMESSAGE_TIMEOUT_FOREVER);

	Cache_wbInv((void *)buf, write_msg.count);

	return write_msg.count;
}

int fileio_HOSTclose(int dev_fd)
{
	struct close_message close_msg;
	uint32_t          remote_endpt;
	uint32_t          remote_procid;
	uint16_t          len;

	//debug_printf("%s: (%d)\n", __func__, dev_fd);

	remote_procid = IPC_MPU1_0;

	close_msg.command = COMMAND_CLOSE;
	close_msg.dev_fd = dev_fd;

	RPMessage_send(fileio_rpmsg_handle, remote_procid, fileio_rpmsg_remote_endpt, fileio_rpmsg_endpt, &close_msg, sizeof(close_msg));
	RPMessage_recv(fileio_rpmsg_handle, &close_msg, &len, &remote_endpt, &remote_procid, IPC_RPMESSAGE_TIMEOUT_FOREVER);

	return close_msg.dev_fd;
}

off_t fileio_HOSTlseek(int dev_fd, off_t offset, int origin)
{
	struct lseek_message lseek_msg;
	uint32_t          remote_endpt;
	uint32_t          remote_procid;
	uint16_t          len;

	//debug_printf("%s: (%d, %d, %d)\n", __func__, dev_fd, offset, origin);

	remote_procid = IPC_MPU1_0;

	lseek_msg.command = COMMAND_LSEEK;
	lseek_msg.dev_fd = dev_fd;
	lseek_msg.origin = origin;
	lseek_msg.offset = offset;

	RPMessage_send(fileio_rpmsg_handle, remote_procid, fileio_rpmsg_remote_endpt, fileio_rpmsg_endpt, &lseek_msg, sizeof(lseek_msg));
	RPMessage_recv(fileio_rpmsg_handle, &lseek_msg, &len, &remote_endpt, &remote_procid, IPC_RPMESSAGE_TIMEOUT_FOREVER);

	return lseek_msg.offset;
}

int fileio_setvbuf(FILE * __restrict _fp, char * __restrict _buf,
			 int _type, size_t _size)
{
   if (_fp->fd == -1 || (_type != _IONBF && _size <= 0))
   {
      return (EOF);
   }

   if (_fp->buf)
   {
      if(_STCHK(_fp, _BUFFALOC)) free((_fp->buf)-1);
      _UNSET(_fp, _BUFFALOC);
      _fp->buf = NULL;
      _fp->pos = NULL;
      _fp->bufend = NULL;
      _fp->buff_stop = NULL;
   }

   _UNSET(_fp, (_IOLBF | _IOFBF | _IONBF));
   _SET(_fp, _IONBF);

   return (0);
}
