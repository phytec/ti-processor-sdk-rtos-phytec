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

static int32_t get_next_line(char* buf, char* line_str, uint8_t max_line_len)
{
    int i=0;
    while(buf[i] != '#')
    {
        line_str[i] = buf[i];
        i++;

        if(i == max_line_len)
        {
            return -1;
        }
    }

    line_str[i] = 0;

    while(buf[i] != '\n')
    {
        i++;
        if(i == max_line_len)
        {
            return -2;
        }
    }
    return i+1;/*Including new line char*/
}
static uint8_t cmd_param_dev_ctrl[CMD_PARAM_SIZE];
#define MAX_LINE_SIZE (128)
static char line_str[MAX_LINE_SIZE];
static char line_str_out[MAX_LINE_SIZE];

void itt_ctrl_cmdHandlerIssDevCtrl(char *cmd, uint32_t prmSize)
{
    /*Generic device control*/
    /*Currently programmed to receive text and send the same text back*/
    /*This maybe enhanced in future to read/write a list of register values*/
    int32_t status;
    char *prm;
    char *prm_out;
    uint32_t prmOutSize = 0;
    uint8_t * cmd_ptr;
    uint32_t * ptr32;
    char *ptr8;
    uint16_t chId;
    uint16_t devType;
    uint16_t i2cSlaveAddr = 0x32;
    uint16_t regAddr;
    uint16_t regValue;
    int32_t count = 0;
    char *token;
    char s[]=" \t \n";
    IMAGE_SENSOR_CTRLCMD ctrlCmd = IMAGE_SENSOR_CTRLCMD_DEBUG;
    uint16_t bytesAvailable = 0;
    uint8_t max_line_size = MAX_LINE_SIZE;
    char dummy_name[ISS_SENSORS_MAX_NAME] = "dummy";

    prm = malloc(prmSize);
    if(NULL == prm)
    {
        printf("Failed to allocate %d bytes for prm \n", prmSize);
        return;
    }
    prm_out = malloc(prmSize);
    if(NULL == prm_out)
    {
        printf("Failed to allocate %d bytes  for prm_out \n", prmSize);
        free(prm);
        return;
    }
    ptr8 = prm;
    bytesAvailable = prmSize;

    IttCtrl_readParams((uint8_t *)prm, prmSize);

    //# Command (RD/WR)  CH_ID      DEVICE   REG_ADDR REG_VAL

    while( count >= 0)
    {
        count = get_next_line(ptr8, line_str, max_line_size);
        if (count == 0)
        {
            /*Comment : nothing to do*/
        }else
        {
            if(count > 0)
            {
                bytesAvailable -= count;
            }
            if(max_line_size > bytesAvailable)
            {
                max_line_size = bytesAvailable;
            }
            token = strtok(line_str, s);
            if(NULL == token)
            {
                /*White space : nothing to do*/
            }else
            {
                uint32_t rw_flag = 0xFF;

                if(strcmp(token, "WR")==0)
                {
                    rw_flag = 1;
                }
                else if(strcmp(token, "RD")==0)
                {
                    rw_flag = 0;
                }
                else
                {
                    rw_flag = 0; /*Unsupported command. Forcing to read*/
                    printf("Illegal token %s \n", token);
                }

                token = strtok(NULL, s);
                if(NULL == token)
                {
                    printf("Failed to read chId \n");
                    free(prm);
                    free(prm_out);
                    return;
                }
                chId = strtol(token, NULL, 16);
                printf("chId = [0x%x]  ", chId);

                token = strtok(NULL, s);
                if(NULL == token)
                {
                    printf("Failed to read devType \n");
                    free(prm);
                    free(prm_out);
                    return;
                }
                devType= strtol(token, NULL, 16);
                printf("devType = [0x%x]  ", devType);

                token = strtok(NULL, s);
                if(NULL == token)
                {
                    printf("Failed to read regAddr \n");
                    free(prm);
                    free(prm_out);
                    return;
                }
                regAddr = strtol(token, NULL, 16);
                printf("register address = [0x%x]  ", regAddr);

                token = strtok(NULL, s);
                if(NULL == token)
                {
                    printf("Failed to read regValue \n");
                    free(prm);
                    free(prm_out);
                    return;
                }
                regValue = strtol(token, NULL, 16);
                printf("register value = [0x%x]\n", regValue);

                memset(cmd_param_dev_ctrl, 0xAB, CMD_PARAM_SIZE);
                cmd_ptr = (uint8_t *)cmd_param_dev_ctrl;

                memcpy(cmd_ptr, dummy_name, strlen(dummy_name)+1);
                cmd_ptr += ISS_SENSORS_MAX_NAME;

                ptr32 = (uint32_t *)cmd_ptr;
                *ptr32 = chId; /*channel ID */
                cmd_ptr += sizeof(uint32_t);

                memcpy(cmd_ptr, &ctrlCmd, sizeof(IMAGE_SENSOR_CTRLCMD));
                cmd_ptr += sizeof(IMAGE_SENSOR_CTRLCMD);

                ptr32 = (uint32_t *)cmd_ptr;
                if(1U == rw_flag)
                {
                    *ptr32 = 1U; /*ReadWrite Flag = 1 for Write*/
                }
                else
                {
                    *ptr32 = 0U; /*ReadWrite Flag = 0 for Read*/
                }
                cmd_ptr += sizeof(uint32_t);

                ptr32 = (uint32_t *)cmd_ptr;
                *ptr32 = devType; /*Device Type*/
                cmd_ptr += sizeof(uint32_t);

                ptr32 = (uint32_t *)cmd_ptr;
                *ptr32 = regAddr; /*Register Address*/

                status = appRemoteServiceRun(
                    APP_IPC_RTOS_CPU ,
                    IMAGE_SENSOR_REMOTE_SERVICE_NAME,
                    IM_SENSOR_CMD_CTL,
                    (void*)cmd_param_dev_ctrl,
                    CMD_PARAM_SIZE,
                    0);

                if(status != 0)
                {
                    printf("Error : appRemoteServiceRun returned %d \n", status);
                }
                if(0 == rw_flag)
                {
                    ptr32 = (uint32_t *)cmd_param_dev_ctrl;
                    sprintf(line_str_out, "%x %x %x\n", i2cSlaveAddr, regAddr, *ptr32);
                    memcpy(prm_out + prmOutSize, line_str_out, strlen(line_str_out));
                    prmOutSize += strlen(line_str_out);
                }
            }
            ptr8 += count;
        }

    }

    /* send response */
    IttCtrl_writeParams((uint8_t *)prm_out, prmOutSize, 0);
    free(prm);
    free(prm_out);
}
