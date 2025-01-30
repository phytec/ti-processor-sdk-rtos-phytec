/*
*
* Copyright (c) 2023 Texas Instruments Incorporated
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


/** @file file_io_tb.c
*
*  @brief  This file contains code for Fast File IO Utility.
*
*
*  @date   June 2023
*
*  Description
*    This file contains test code to Network based fast file IO
*/

#if (!HOST_EMULATION)

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <stdarg.h>

#include "app_fileio_core.h"
#include <stdint.h>
#include "app_mem.h"
#include "app_fileio.h"
#include "app_fileio_priv.h"
#include "utils/console_io/include/app_log.h"

static int32_t fileStatus[MAX_NUM_FILES];

static app_fileio_wr_obj_t g_app_fileio_wr_obj;

static int32_t allocFile();
static void releaseFile(int32_t fileCtr);

static int32_t allocFile()
{
    int32_t ctr = -1;
    uint32_t i;

    for(i = 0; i < MAX_NUM_FILES; i++)
    {
        if(fileStatus[i] == 0)
        {
            fileStatus[i] = 1;
            ctr = i;
            break;
        }
    }

    return ctr;
}

static void releaseFile(int32_t fileCtr)
{
    fileStatus[fileCtr] = 0;
}

/*File I/O Functions Start Here*/
int32_t appFileIOWrInit(app_fileio_init_prm_t * prm)
{
    int32_t status = 0;
    int32_t ctr;
    app_fileio_wr_obj_t *obj = &g_app_fileio_wr_obj;

    obj->cpu_shared_mem = NULL;

    appFileIOWrCreateLock(obj);

    if(prm->self_cpu_index >= APP_FILEIO_MAX_CPUS)
    {
        status = -1;
    }
    if(status == 0)
    {
        app_fileio_shared_mem_t *shared_mem = appFileIOMemMap(prm->shared_mem, sizeof(app_fileio_shared_mem_t));
        if(shared_mem!=NULL)
        {
            uint32_t i;

            obj->shared_mem = shared_mem;

            obj->cpu_shared_mem = &shared_mem->cpu_shared_mem[prm->self_cpu_index];

            obj->cpu_shared_mem->fileio_cpu_name[0] = 0;

            for(i=0; i<APP_FILEIO_MAX_CPU_NAME; i++)
            {
                obj->cpu_shared_mem->fileio_cpu_name[i] = prm->self_cpu_name[i];
                for(ctr = 0; ctr < MAX_NUM_FILES; ctr++)
                {
                    fileStatus[ctr] = 0;
                }
            }
        }
        else
        {
            status = -1;
        }
    }
    return status;
}

/* Open a file */
int32_t ti_fileio_fopen(app_fileio_cpu_shared_mem_t * cpu_shared_mem, const char *filename, const char *mode)
{
    OpStruct_t * opStruct = (OpStruct_t *)(&cpu_shared_mem->opStruct);
    int32_t fileCtr = -1;
    int32_t status = (int32_t )APP_FILEIO_STATUS_OK;

    if((!strlen(filename)) || (!strlen(mode)))
    {
        status = (int32_t)APP_FILEIO_STATUS_ERROR;
    }
    
    if(status == (int32_t )APP_FILEIO_STATUS_OK)
    {
        fileCtr = allocFile();
        if(fileCtr == -1)
        {
            status = (int32_t)APP_FILEIO_STATUS_ERROR;
        }
    }

    if(status == (int32_t )APP_FILEIO_STATUS_OK)
    {
        cpu_shared_mem->fp[fileCtr] = NULL;

        opStruct->opCode = TI_FILEIO_OPCODE_FOPEN;
        strncpy(opStruct->fileName, filename, APP_TRACE_FILE_NAME_SIZE-1);
        strncpy(opStruct->mode, mode, APP_FILE_MODE_SIZE-1);
        
        cpu_shared_mem->fileCtr = fileCtr;

        cpu_shared_mem->writeIndicatorFlag = 1;
        while(cpu_shared_mem->writeIndicatorFlag == 1)
        {

        };
    }

    return fileCtr;
}

/* Close a file */
int32_t ti_fileio_fclose(app_fileio_cpu_shared_mem_t * cpu_shared_mem, int32_t fileCtr)
{
    int8_t r = -1;
    int32_t status = (int32_t)APP_FILEIO_STATUS_OK;
    OpStruct_t * opStruct = (OpStruct_t *)(&cpu_shared_mem->opStruct);
    if(! cpu_shared_mem->fp[fileCtr])
    {
        cpu_shared_mem->retValue = EOF;
        releaseFile(fileCtr);
        status = (int32_t)APP_FILEIO_STATUS_ERROR;
    }

    if(status == (int32_t )APP_FILEIO_STATUS_OK)
    {
        cpu_shared_mem->retValue = 0;
        opStruct->opCode = TI_FILEIO_OPCODE_FCLOSE;
        opStruct->fid    = (void *)cpu_shared_mem->fp[fileCtr];
        cpu_shared_mem->writeIndicatorFlag = 1;
        while(cpu_shared_mem->writeIndicatorFlag == 1)
        {
            
        };

        r = (int32_t)(cpu_shared_mem->retValue);

        releaseFile(fileCtr);
    }

    return r;
}

/* Read a file */
size_t ti_fileio_fread(app_fileio_cpu_shared_mem_t * cpu_shared_mem,  size_t size, size_t count, int32_t fileCtr, int32_t offset)
{
    size_t   bytestoRead;
    OpStruct_t * opStruct = (OpStruct_t *)(&cpu_shared_mem->opStruct);
    void * fp = (void *)cpu_shared_mem->fp[fileCtr];

    int32_t status = (int32_t)APP_FILEIO_STATUS_OK;

    if(!fp)
    {
        bytestoRead = 0;
        status = (int32_t)APP_FILEIO_STATUS_ERROR;
    }
    
    if(status == (int32_t )APP_FILEIO_STATUS_OK)
    {
        opStruct->opCode = TI_FILEIO_OPCODE_FREAD;
        opStruct->fid    = fp;
        opStruct->size   = size;
        opStruct->count  = count;
        opStruct->offset = offset;

        bytestoRead = size * count;

        cpu_shared_mem->writeIndicatorFlag = 1;
        while(cpu_shared_mem->writeIndicatorFlag == 1)
        {

        };
    }

    return bytestoRead;
}

/* Write to a file */
size_t ti_fileio_fwrite(app_fileio_cpu_shared_mem_t * cpu_shared_mem, size_t size, size_t count, int32_t fileCtr)
{
    size_t    bytestoWrite;
    int32_t status = (int32_t)APP_FILEIO_STATUS_OK;
    OpStruct_t * opStruct = (OpStruct_t *)(&cpu_shared_mem->opStruct);
    void * fp = (void *)cpu_shared_mem->fp[fileCtr];

    if(!fp)
    {
        bytestoWrite = 0;
        status = (int32_t)APP_FILEIO_STATUS_ERROR;
    }

    if(status == (int32_t )APP_FILEIO_STATUS_OK)
    {
        opStruct->opCode = TI_FILEIO_OPCODE_FWRITE;
        opStruct->fid    = fp;
        opStruct->size   = size;
        opStruct->count  = count;

        bytestoWrite = size * count;

        cpu_shared_mem->writeIndicatorFlag = 1;
        while(cpu_shared_mem->writeIndicatorFlag == 1)
        {

        };
    }
    return bytestoWrite;
}


/* The following commented out section of code contains support for more File operations, but it is not validated
    Can be updated and uncommented if required based on more validation */
#if 0
int8_t ti_fileio_fseek(app_fileio_cpu_shared_mem_t * cpu_shared_mem, int32_t offset, int32_t origin, int32_t fileCtr)
{
    int8_t    r = 0;
    int32_t status = (int32_t)APP_FILEIO_STATUS_OK;
    OpStruct_t * opStruct = (OpStruct_t *)(&cpu_shared_mem->opStruct);
    void * fp = (void *)cpu_shared_mem->fp[fileCtr];

    if(!fp)
    {
        r = -1;
        status = (int32_t)APP_FILEIO_STATUS_ERROR;
    }

    if(status == (int32_t )APP_FILEIO_STATUS_OK)
    {
        opStruct->opCode = TI_FILEIO_OPCODE_FSEEK;
        opStruct->fid    = fp;
        opStruct->offset = offset;
        opStruct->count  = origin;

        cpu_shared_mem->writeIndicatorFlag = 1;
        while(cpu_shared_mem->writeIndicatorFlag == 1)
        {

        };
        r = cpu_shared_mem->retValue;
    }

    return r;
}


int32_t ti_fileio_ftell(app_fileio_cpu_shared_mem_t * cpu_shared_mem, int32_t fileCtr)
{
    int32_t   r = 0;
    int32_t status = (int32_t)APP_FILEIO_STATUS_OK;
    OpStruct_t * opStruct = (OpStruct_t *)(&cpu_shared_mem->opStruct);
    void * fp = (void *)cpu_shared_mem->fp[fileCtr];

    if(!fp)
    {
        r = -1;
        status = (int32_t)APP_FILEIO_STATUS_ERROR;
    }

    if(status == (int32_t )APP_FILEIO_STATUS_OK)
    {
        opStruct->opCode = TI_FILEIO_OPCODE_FTELL;
        opStruct->fid    = fp;
        cpu_shared_mem->writeIndicatorFlag = 1;
        while(cpu_shared_mem->writeIndicatorFlag == 1)
        {

        };
        r = cpu_shared_mem->retValue;
    }
    return r;
}

char * ti_fileio_fgets(app_fileio_cpu_shared_mem_t * cpu_shared_mem, char *str, int32_t num, int32_t fileCtr)
{
    char     *ret = NULL;
    int32_t status = (int32_t)APP_FILEIO_STATUS_OK;
    OpStruct_t * opStruct = (OpStruct_t *)(&cpu_shared_mem->opStruct);
    void * fp = (void *)cpu_shared_mem->fp[fileCtr];

    if(!fp)
    {
        ret = NULL;
        status = (int32_t)APP_FILEIO_STATUS_ERROR;
    }

    if(!str)
    {
        ret = NULL;
        status = (int32_t)APP_FILEIO_STATUS_ERROR;
    }

    if(status == (int32_t )APP_FILEIO_STATUS_OK)
    {
        opStruct->opCode = TI_FILEIO_OPCODE_FGETS;
        opStruct->fid    = fp;
        opStruct->size   = num;

        /* fill_msg_q(msg_q, 1, 1, str, num);  */  
        while(cpu_shared_mem->writeIndicatorFlag == 1)
        {

        };

        ret = str;
    }
    return ret;
}
#endif

int32_t appWriteBinToFile(const char * filename, void * ptr, int32_t size, int32_t count)
{
    uint32_t cookie;
    app_fileio_wr_obj_t *obj = &g_app_fileio_wr_obj;

    int32_t fileCtr = -1;

    cookie = appFileIOWrLock(obj);
    
    app_fileio_cpu_shared_mem_t * cpu_shared_mem = obj->cpu_shared_mem;
    uint8_t * sharedPtr = (uint8_t *)(&cpu_shared_mem->fileio_mem[0]);
    
    char fileNameWithPrefix[APP_TRACE_FILE_NAME_SIZE*3];
    char fileLocalName[APP_TRACE_FILE_NAME_SIZE], dirName[APP_TRACE_FILE_NAME_SIZE];
    strncpy(fileLocalName, filename, APP_TRACE_FILE_NAME_SIZE-1);
    strncpy(dirName, filename, APP_TRACE_FILE_NAME_SIZE-1);
    fileLocalName[APP_TRACE_FILE_NAME_SIZE-1] = '\0';
    char * fileNamePtr = getFileNameFromPath(fileLocalName);
    getDirFromPath(dirName);
    sprintf(fileNameWithPrefix, "%s/%s_%s", (char *)dirName, (char *)cpu_shared_mem->fileio_cpu_name, fileNamePtr);

    /* Copy multiple times if data to copy > shared buffer size */
    int32_t numCopies = 0;
    int32_t totalSize = size * count;
    uint8_t * bufToCopy = (uint8_t *)ptr;
    int32_t i;
    
    /* Open file in write mode open afresh and prevent appending traces to existing file */
    fileCtr = ti_fileio_fopen(cpu_shared_mem, fileNameWithPrefix, "wb");
    ti_fileio_fclose(cpu_shared_mem, fileCtr);
    /* Close and reopen in append mode to do staggered write of entire data */
    fileCtr = ti_fileio_fopen(cpu_shared_mem, fileNameWithPrefix, "ab");
    numCopies = totalSize / APP_FILEIO_PER_CPU_MEM_SIZE;

    for(i = 0; i < numCopies; i++)
    {
        memcpy(sharedPtr, bufToCopy, APP_FILEIO_PER_CPU_MEM_SIZE); /* Copy from remote core internal trace data ptr to shared ptr */
        bufToCopy += APP_FILEIO_PER_CPU_MEM_SIZE;
        ti_fileio_fwrite(cpu_shared_mem, 1, APP_FILEIO_PER_CPU_MEM_SIZE, fileCtr);
    }
    if((i == numCopies) && (totalSize % APP_FILEIO_PER_CPU_MEM_SIZE != 0)) /* Remnant data */
    {
        memcpy(sharedPtr, bufToCopy, totalSize % APP_FILEIO_PER_CPU_MEM_SIZE);
        ti_fileio_fwrite(cpu_shared_mem, 1, (int32_t)(totalSize % APP_FILEIO_PER_CPU_MEM_SIZE), fileCtr);
    }

    ti_fileio_fclose(cpu_shared_mem, fileCtr);

    appFileIOWrUnLock(obj, cookie);
    return 0;
}

int32_t appReadBinFromFile(const char * filename, void * ptr, int32_t size, int32_t count)
{
    uint32_t cookie;
    app_fileio_wr_obj_t *obj = &g_app_fileio_wr_obj;

    int32_t fileCtr = -1;

    cookie = appFileIOWrLock(obj);
    
    app_fileio_cpu_shared_mem_t * cpu_shared_mem = obj->cpu_shared_mem;
    uint8_t * sharedPtr = (uint8_t *)(&cpu_shared_mem->fileio_mem[0]);
    
    char fileNameWithPrefix[APP_TRACE_FILE_NAME_SIZE*3];
    char fileLocalName[APP_TRACE_FILE_NAME_SIZE], dirName[APP_TRACE_FILE_NAME_SIZE];
    strncpy(fileLocalName, filename, APP_TRACE_FILE_NAME_SIZE-1);
    strncpy(dirName, filename, APP_TRACE_FILE_NAME_SIZE-1);
    fileLocalName[APP_TRACE_FILE_NAME_SIZE-1] = '\0';
    char * fileNamePtr = getFileNameFromPath(fileLocalName);
    getDirFromPath(dirName);
    sprintf(fileNameWithPrefix, "%s/%s_%s", (char *)dirName, (char *)cpu_shared_mem->fileio_cpu_name, fileNamePtr);

    /* Copy multiple times if data to copy > shared buffer size */
    int32_t numCopies = 0, offset = 0;
    int32_t totalSize = size * count;
    uint8_t * bufToCopy = (uint8_t *)ptr;
    int32_t i;

    fileCtr = ti_fileio_fopen(cpu_shared_mem, fileNameWithPrefix, "rb");

    if(totalSize > APP_FILEIO_PER_CPU_MEM_SIZE) /* Staggered read of data in multiple iterations */
    {
        numCopies = totalSize / APP_FILEIO_PER_CPU_MEM_SIZE;

        for(i = 0; i < numCopies; i++)
        {
            ti_fileio_fread(cpu_shared_mem, 1, APP_FILEIO_PER_CPU_MEM_SIZE, fileCtr, offset);
            memcpy(bufToCopy, sharedPtr, APP_FILEIO_PER_CPU_MEM_SIZE); /* Copy from shared ptr to remote core data ptr */
            bufToCopy += APP_FILEIO_PER_CPU_MEM_SIZE;
            offset += APP_FILEIO_PER_CPU_MEM_SIZE;
        }
        if((i == numCopies) && (totalSize % APP_FILEIO_PER_CPU_MEM_SIZE != 0))
        {
            ti_fileio_fread(cpu_shared_mem, 1, (int32_t)(totalSize % APP_FILEIO_PER_CPU_MEM_SIZE), fileCtr, offset);
            memcpy(bufToCopy, sharedPtr, totalSize % APP_FILEIO_PER_CPU_MEM_SIZE);
        }
    }
    else
    {
        ti_fileio_fread(cpu_shared_mem, size, count, fileCtr, 0);
        memcpy(ptr, sharedPtr, size * count);
    }
    ti_fileio_fclose(cpu_shared_mem, fileCtr);

    appFileIOWrUnLock(obj, cookie);
    return 0;
}

int32_t  appFileIOWrDeInit()
{
    int32_t status = 0;

    return status;
}

#endif /* !HOST_EMULATION */


