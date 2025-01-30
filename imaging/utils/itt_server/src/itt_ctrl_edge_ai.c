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

#include <itt_server.h>
#include <itt_srvr_remote.h>
#include <sys/stat.h>
#include <app_iss.h>

#define TIOVX_MODULES_MAX_FNAME (256u)

static ITTServerEdgeAIObj g_ITTobj;
static pthread_mutex_t lock;
uint8_t gEdgeAI = 0;

static vx_status writeRawImageEdgeAI(char* file_name, tivx_raw_image image)
{
    vx_status status;

    status = vxGetStatus((vx_reference)image);

    if((vx_status)VX_SUCCESS == status)
    {
        FILE * fp = fopen(file_name,"wb");
        vx_int32  j;

        if(fp == NULL)
        {
            ITT_PRINTF("Unable to open file %s \n", file_name);
            return (VX_FAILURE);
        }

        {
            vx_uint32 width, height;
            vx_imagepatch_addressing_t image_addr;
            vx_rectangle_t rect;
            vx_map_id map_id;
            void *data_ptr;
            vx_uint32 bpp = 1;
            vx_uint32 num_bytes;
            tivx_raw_image_format_t format[3];
            vx_int32 plane, num_planes, plane_size;
            vx_uint32 num_exposures;
            vx_bool line_interleaved = vx_false_e;

            tivxQueryRawImage(image, TIVX_RAW_IMAGE_WIDTH, &width, sizeof(vx_uint32));
            tivxQueryRawImage(image, TIVX_RAW_IMAGE_HEIGHT, &height, sizeof(vx_uint32));
            tivxQueryRawImage(image, TIVX_RAW_IMAGE_FORMAT, &format, sizeof(format));
            tivxQueryRawImage(image, TIVX_RAW_IMAGE_NUM_EXPOSURES, &num_exposures, sizeof(num_exposures));
            tivxQueryRawImage(image, TIVX_RAW_IMAGE_LINE_INTERLEAVED, &line_interleaved, sizeof(line_interleaved));

            if(line_interleaved == vx_true_e)
            {
                num_planes = 1;
            }
            else
            {
                num_planes = num_exposures;
            }

            if( format[0].pixel_container == TIVX_RAW_IMAGE_16_BIT )
            {
                bpp = 2;
            }
            else if( format[0].pixel_container == TIVX_RAW_IMAGE_8_BIT )
            {
                bpp = 1;
            }
            else if( format[0].pixel_container == TIVX_RAW_IMAGE_P12_BIT )
            {
                bpp = 0;
            }

            rect.start_x = 0;
            rect.start_y = 0;
            rect.end_x = width;
            rect.end_y = height;

            for (plane = 0; plane < num_planes; plane++)
            {
                tivxMapRawImagePatch(image,
                    &rect,
                    plane,
                    &map_id,
                    &image_addr,
                    &data_ptr,
                    VX_READ_ONLY,
                    VX_MEMORY_TYPE_HOST,
                    TIVX_RAW_IMAGE_PIXEL_BUFFER
                    );

                uint8_t *pIn = (uint8_t *)data_ptr;
                num_bytes = 0;
                if(line_interleaved == vx_true_e)
                {
                    for (j = 0; j < (image_addr.dim_y * num_exposures); j++)
                    {
                        num_bytes += fwrite(pIn, 1, image_addr.dim_x * bpp, fp);
                        pIn += image_addr.stride_y;
                    }
                }
                else
                {
                    for (j = 0; j < image_addr.dim_y; j++)
                    {
                        num_bytes += fwrite(pIn, 1, image_addr.dim_x * bpp, fp);
                        pIn += image_addr.stride_y;
                    }
                }

                plane_size = (image_addr.dim_y * image_addr.dim_x* bpp);

                if(num_bytes != plane_size)
                    printf("Plane [%d] bytes written = %d, expected = %d\n", plane, num_bytes, plane_size);

                tivxUnmapRawImagePatch(image, map_id);
            }
        }

        fclose(fp);
    }

    return(status);
}

static vx_status writeImageEdgeAI(char* file_name, vx_image img)
{
    vx_status status;

    status = vxGetStatus((vx_reference)img);

    if((vx_status)VX_SUCCESS == status)
    {
        FILE * fp = fopen(file_name,"wb");
        vx_int32  j;

        if(fp == NULL)
        {
            printf("Unable to open file %s \n", file_name);
            return (VX_FAILURE);
        }

        {
            vx_rectangle_t rect;
            vx_imagepatch_addressing_t image_addr;
            vx_map_id map_id;
            void * data_ptr;
            vx_uint32  img_width;
            vx_uint32  img_height;
            vx_uint32  num_bytes = 0;
            vx_size    num_planes;
            vx_uint32  plane;
            vx_uint32  plane_size;
            vx_df_image img_format;

            vxQueryImage(img, VX_IMAGE_WIDTH, &img_width, sizeof(vx_uint32));
            vxQueryImage(img, VX_IMAGE_HEIGHT, &img_height, sizeof(vx_uint32));
            vxQueryImage(img, VX_IMAGE_PLANES, &num_planes, sizeof(vx_size));
            vxQueryImage(img, VX_IMAGE_FORMAT, &img_format, sizeof(vx_df_image));

            for (plane = 0; plane < num_planes; plane++)
            {
                rect.start_x = 0;
                rect.start_y = 0;
                rect.end_x = img_width;
                rect.end_y = img_height;
                status = vxMapImagePatch(img,
                                        &rect,
                                        plane,
                                        &map_id,
                                        &image_addr,
                                        &data_ptr,
                                        VX_READ_ONLY,
                                        VX_MEMORY_TYPE_HOST,
                                        VX_NOGAP_X);

                ITT_PRINTF("image_addr.dim_x = %d\n ", image_addr.dim_x);
                ITT_PRINTF("image_addr.dim_y = %d\n ", image_addr.dim_y);
                ITT_PRINTF("image_addr.step_x = %d\n ", image_addr.step_x);
                ITT_PRINTF("image_addr.step_y = %d\n ", image_addr.step_y);
                ITT_PRINTF("image_addr.stride_y = %d\n ", image_addr.stride_y);
                ITT_PRINTF("image_addr.stride_x = %d\n ", image_addr.stride_x);
                ITT_PRINTF("\n");

                num_bytes = 0;
                for (j = 0; j < (image_addr.dim_y/image_addr.step_y); j++)
                {
                    num_bytes += image_addr.stride_x * fwrite(data_ptr, image_addr.stride_x, (image_addr.dim_x/image_addr.step_x), fp);
                    data_ptr += image_addr.stride_y;
                }

                plane_size = (image_addr.dim_y/image_addr.step_y) * ((image_addr.dim_x * image_addr.stride_x)/image_addr.step_x);

                if(num_bytes != plane_size)
                    printf("Plane [%d] bytes written = %d, expected = %d\n", plane, num_bytes, plane_size);

                vxUnmapImagePatch(img, map_id);
            }

        }

        fclose(fp);
    }

    return(status);
}

static char *app_get_test_file_path()
{
    char *tivxPlatformGetEnv(char *env_var);

    #if defined(SYSBIOS)
    return tivxPlatformGetEnv("VX_TEST_DATA_PATH");
    #else
    return getenv("VX_TEST_DATA_PATH");
    #endif
}

int32_t save_debug_images_edge_ai(ITTServerEdgeAIObj *obj)
{
    int num_bytes_io = 0;
    static int file_index = 0;
    char raw_image_fname[TIOVX_MODULES_MAX_FNAME];
    char yuv_image_fname[TIOVX_MODULES_MAX_FNAME];
    char failsafe_test_data_path[3] = "./";
    char * test_data_path = app_get_test_file_path();
    struct stat s;

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

    snprintf(raw_image_fname, TIOVX_MODULES_MAX_FNAME, "%s/%s_%04d.raw", test_data_path, "img", file_index);
    printf("RAW file name %s \n", raw_image_fname);
    if (*(obj->obj_viss.raw_image_handle) != NULL)
    {
        num_bytes_io = writeRawImageEdgeAI(raw_image_fname, *(obj->obj_viss.raw_image_handle));
        if(num_bytes_io < 0)
        {
            printf("Error writing to RAW file \n");
            return VX_FAILURE;
        }
    }
    else
    {
        FILE * fp = fopen(raw_image_fname,"wb");
        if(fp == NULL)
        {
            ITT_PRINTF("Unable to open file %s \n", raw_image_fname);
            return (VX_FAILURE);
        }
        fclose(fp);
    }

    snprintf(yuv_image_fname, TIOVX_MODULES_MAX_FNAME, "%s/%s_%04d.yuv", test_data_path, "img_viss", file_index);
    printf("YUV file name %s \n", yuv_image_fname);
    if (*(obj->obj_viss.yuv_image_handle) != NULL)
    {
        num_bytes_io = writeImageEdgeAI(yuv_image_fname, *(obj->obj_viss.yuv_image_handle));
        if(num_bytes_io < 0)
        {
            printf("Error writing to VISS NV12 file \n");
            return VX_FAILURE;
        }
    }
    else
    {
        FILE * fp = fopen(yuv_image_fname,"wb");
        if(fp == NULL)
        {
            ITT_PRINTF("Unable to open file %s \n", yuv_image_fname);
            return (VX_FAILURE);
        }
        fclose(fp);
    }

    file_index++;
    return (file_index-1);
}

int32_t itt_register_object(vx_context context,
                              vx_node *node,
                              tivx_raw_image *raw_image_handle,
                              vx_image *yuv_image_handle, 
                              uint8_t object_name)
{
    int32_t status = 0;

    switch(object_name)
    {
        case VISS:
            g_ITTobj.obj_viss.context = context;
            g_ITTobj.obj_viss.node = node;
            g_ITTobj.obj_viss.raw_image_handle = raw_image_handle;
            g_ITTobj.obj_viss.yuv_image_handle = yuv_image_handle;
            status = VX_SUCCESS;
            break;
        case LDC:
            g_ITTobj.obj_ldc.context = context;
            g_ITTobj.obj_ldc.node = node;
            g_ITTobj.obj_ldc.raw_image_handle = raw_image_handle;
            g_ITTobj.obj_ldc.yuv_image_handle = yuv_image_handle;
            status = VX_SUCCESS;
            break;
        default:
            status = VX_FAILURE;
    }

    return status;
}

int32_t itt_handle_dcc(ITTServerEdgeAIObj *obj, uint8_t* dcc_buf, uint32_t dcc_buf_size)
{
    int32_t status = 0;

    dcc_component_header_type *header_data;
    header_data = (dcc_component_header_type*)dcc_buf;

    if(1U == is_viss_plugin(header_data->dcc_descriptor_id))
    {

        if((g_ITTobj.obj_viss.context == NULL) && (g_ITTobj.obj_viss.node == NULL))
        {
            printf("VISS context or node NULL!!\n");
        }
        else
        {
            /* Trigger VISS udpate dcc callback */
            status = appUpdateVpacDcc(dcc_buf, dcc_buf_size, g_ITTobj.obj_viss.context,
                *(g_ITTobj.obj_viss.node) /* node_viss */, 0,
                NULL /* node_aewb*/, 0,
                NULL /* node_ldc */, 0
             );
        }
    }
    else if(1U == is_ldc_plugin(header_data->dcc_descriptor_id))
    {
        if((g_ITTobj.obj_ldc.context == NULL) && (g_ITTobj.obj_ldc.node == NULL))
        {
            printf("LDC context or node NULL!!\n");
        }
        else
        {
            /* Trigger LDC update dcc callback */
            status = appUpdateVpacDcc(dcc_buf, dcc_buf_size, g_ITTobj.obj_ldc.context,
                NULL /* node_viss */, 0,
                NULL /* node_aewb*/, 0,
                *(g_ITTobj.obj_ldc.node) /* node_ldc */, 0
             );
        }
    }
    else
    {
        printf("Plugin not supported!");
    }

    return status;
}

int32_t itt_server_edge_ai_init()
{
    pthread_mutex_lock(&lock);

    int32_t status = 0;
    static uint8_t ITTinit = 0;
    gEdgeAI = 1;

    if(!ITTinit)
    {        
        /* Initializes ITT server thread on A72/Linux */
        status = itt_server_init((void*)&g_ITTobj, (void*)save_debug_images_edge_ai, (void*)itt_handle_dcc);
        if(status != 0)
        {
            printf("Warning : Failed to initialize ITT server. Live tuning will not work \n");
        }
        else
        {
            /* Initializes I2C bus for reading and writting of camera registers */
            status = i2cInit();
            if(status!=0)
            {
                printf("Warning: Failed to initialize i2c bus. Register read/write will not work !!!\n");
            }

            /* Initializes remote server for 2A control */
            status = IttRemoteServer_Init();
            if(status!=0)
            {
                printf("Warning: Failed to create remote ITT server failed. Live tuning will not work !!!\n");
            }
        }

        ITTinit = 1;
    }

    pthread_mutex_unlock(&lock);
    
    return (status);
}
