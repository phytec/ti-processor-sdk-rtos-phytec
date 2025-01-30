/*
 *
 * Copyright (c) 2020 Texas Instruments Incorporated
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

#include "itt_priv.h"
#include <sys/stat.h>

extern void * g_appObj;
extern void * g_appFileSaveCallback;
#define MAX_FNAME 128

static char *app_get_test_file_path();

static char *app_get_test_file_path()
{
    char *tivxPlatformGetEnv(char *env_var);

    #if defined(SYSBIOS) || defined(FREERTOS) || defined(SAFERTOS)
    return tivxPlatformGetEnv("VX_TEST_DATA_PATH");
    #else
    return getenv("VX_TEST_DATA_PATH");
    #endif
}

void itt_ctrl_cmdHandlerIssFileSave(char *cmd, uint32_t prmSize)
{
    AlgItt_GetSaveFrameStatus saveFrameStatus;
    int32_t command = -1;/*0=RAW, 1=YUV*/
    int32_t status = 0;
    int file_index = 0;
    int (*filesave_callback)(void*) = g_appFileSaveCallback;
    char raw_image_fname[MAX_FNAME];
    char viss_out_image_fname[MAX_FNAME];
    char * test_data_path = NULL;
    struct stat s;
    char failsafe_test_data_path[3] = "./";

    if (0 == strcmp(cmd, "iss_raw_save"))
    {
        command = 0;
    }
    else if (0 == strcmp(cmd, "iss_yuv_save"))
    {
        command = 1;
    }
    else
    {
        printf("Unknown command %s \n", cmd);
        printf("Returning without sending image \n");
        return;
    }

    test_data_path = app_get_test_file_path();
    if(NULL == test_data_path)
    {
        printf("Test data path is NULL. Defaulting to current folder \n");
        test_data_path = failsafe_test_data_path;
    }
    if (stat(test_data_path, &s))
    {
        printf("Test data path %s does not exist. Defaulting to current folder \n", test_data_path);
        test_data_path = failsafe_test_data_path;
    }

    file_index = filesave_callback(g_appObj);
    ITT_PRINTF("g_appFileSaveCallback returned %d \n ", file_index);

    snprintf(raw_image_fname, MAX_FNAME, "%s/%s_%04d.raw", test_data_path, "img", file_index);
    ITT_PRINTF("RAW file name %s \n", raw_image_fname);

    snprintf(viss_out_image_fname, MAX_FNAME, "%s/%s_%04d.yuv", test_data_path, "img_viss", file_index);
    ITT_PRINTF("YUV file name %s \n", viss_out_image_fname);

    /* alloc tmp buffer for parameters */
    if(prmSize == 0)
    {
        char* fname;
        FILE *fp;
        int file_size = 0;

        switch (command)
        {
            case 0:
                fname = raw_image_fname;
                break;
            case 1:
                fname = viss_out_image_fname;
                break;
            default:
                printf("Error : Unknown command %s \n", cmd);
                printf("Error : Returning without sending image \n");
            	return;
        }

        fp = fopen(fname, "rb");
        if(NULL == fp)
        {
            printf("%s Error: Failed to open file %s. Returning without sending image \n", __FUNCTION__, fname);
        }
    	else
        {
        	uint8_t *filebuf = NULL;
          	fseek(fp, 0, SEEK_END);
            file_size = ftell(fp);
            filebuf = (uint8_t *)malloc(sizeof(uint8_t )*(file_size));
            if(NULL != filebuf)
            {
                memset(filebuf, 0xAB, file_size);
                fseek(fp, 0, SEEK_SET);
                fread(filebuf, sizeof(uint8_t ), file_size, fp);

                saveFrameStatus.bufAddr = filebuf;
                saveFrameStatus.bufSize = file_size;

                /* send response */
                IttCtrl_writeParams((uint8_t *)(saveFrameStatus.bufAddr), saveFrameStatus.bufSize, status);
                ITT_PRINTF(" %s: SUCCESS !!!\n", __FUNCTION__);
                free(filebuf);
            }
            else
            {
                printf(" %s Error: Failed to malloc %d bytes.!!!\n", __FUNCTION__, file_size);
            }
            fclose(fp);
        }
    }
    else
    {
        printf(" %s Error: %s: Insufficient parameters (%d bytes) specified !!!\n", __FUNCTION__, cmd, prmSize);
    }
}

