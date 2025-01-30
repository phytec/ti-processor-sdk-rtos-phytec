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

/*******************************************************************************
 *  INCLUDE FILES
 *******************************************************************************
 */
#include <network_api.h>
char *inet_ntoa(struct in_addr in);

int32_t Network_read(Network_SockObj *pObj, uint8_t *dataBuf, uint32_t *dataSize)
{
    int32_t status = 0;
    uint32_t tmpDataSize;
    int32_t actDataSize = 0;

    tmpDataSize = *dataSize;

    while(tmpDataSize > 0U)
    {
        actDataSize = recv(pObj->connectedSockFd, (void *)dataBuf, tmpDataSize, 0);
        if(actDataSize <= 0)
        {
            *dataSize = 0U;
            status = -1;
            printf("%s: tmpDataSize = 0x%x recv returned 0x%x \n", __FUNCTION__, tmpDataSize, actDataSize);
            break;
        }
        else
        {
            dataBuf += actDataSize;
            tmpDataSize -= (uint32_t)actDataSize;
        }
    }

    return status;
}


int32_t Network_write(Network_SockObj *pObj, uint8_t *dataBuf, uint32_t dataSize)
{
    int32_t status = 0;
    int32_t actDataSize=0;

    while (dataSize > 0) {
        actDataSize = (int32_t)send(pObj->connectedSockFd, dataBuf,
            (size_t)dataSize, 0);

        if (actDataSize <= 0)
        {
            break;
        }
        else
        {
            dataBuf += actDataSize;
            dataSize -= (uint32_t)actDataSize;
        }
    }

    if (dataSize > 0) {
        status = -1;
    }

    return status;
}

int32_t Network_open(Network_SockObj *pObj, uint32_t port)
{
    int32_t status = 0;
    struct sockaddr_in   sin1;
    int32_t option = 1;
    struct ifreq ifr;
    int fd;

    pObj->connectedSockFd = -1;
    pObj->port = port;
    pObj->sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if( pObj->sockFd < 0)
    {
        printf(" NETWORK: Unable to open socket (port=%d)!!!\n", port);
        status = -1;
    }
    else
    {
        /* Bind to the specified Server port */
        bzero( &sin1, sizeof(struct sockaddr_in) );
        sin1.sin_family      = AF_INET;
        sin1.sin_addr.s_addr = INADDR_ANY;
        sin1.sin_port        = (uint16_t)(htons((uint16_t)(pObj->port)));

        setsockopt(pObj->sockFd, SOL_SOCKET, SO_REUSEADDR, &option,
            sizeof(option));

        if (bind(pObj->sockFd, (struct sockaddr *)&sin1, sizeof(sin1)) < 0)
        {
            printf(" NETWORK: Unable to bind() (port=%d) !!!\n", port);
            close(pObj->sockFd);
            pObj->sockFd = -1;
            status = -1;
        }
        else
        {
            if(listen(pObj->sockFd, 5) < 0)
            {
                close(pObj->sockFd);
                pObj->sockFd = -1;
                status = -1;
            }
        }
    }

    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);
    printf(" NETWORK: Opened at IP Addr = %s, socket port=%d!!!\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr), port);
    return status;
}

int32_t Network_close(Network_SockObj *pObj, Bool closeServerSock)
{
    if(pObj->connectedSockFd != -1)
    {
        close(pObj->connectedSockFd);
        pObj->connectedSockFd = -1;
    }

    if(closeServerSock)
    {
        if (pObj->sockFd != -1)
        {
            close(pObj->sockFd);
            pObj->sockFd = -1;
        }
    }

    return 0;
}

int32_t Network_waitConnect(Network_SockObj *pObj, uint32_t timeout)
{
    int             status;
    struct timeval  timeout1;
    fd_set          master_set;

    FD_ZERO(&master_set);
    FD_SET(pObj->sockFd, &master_set);

    timeout1.tv_sec  = timeout;
    timeout1.tv_usec = 0;

    status = select(pObj->sockFd + 1, &master_set, NULL, NULL, &timeout1);
    if (status < 0)
    {
        printf(" NETWORK: Select Failed\n");
        status = -1;
    }
    else if (status == 0)
    {
        printf(" NETWORK: Timeout\n");
        status = 0;
    }
    else
    {
        if (FD_ISSET(pObj->sockFd, &master_set))
        {
            pObj->connectedSockFd = accept(pObj->sockFd, 0, 0);

            if (pObj->connectedSockFd < 0)
            {
                status = 1;
            }
        }
    }

    /* NO connection, retry */
    return status;
}

int32_t Network_sessionOpen(void *handle)
{
    return 0;
}

int32_t Network_sessionClose(void *handle)
{
    return 0;
}

