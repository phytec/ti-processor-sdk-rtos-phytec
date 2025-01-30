/*
 * Copyright (c) 2023 Texas Instruments Incorporated
 * Copyright (c) 2023 Excelfore Corporation (https://excelfore.com)
 *
 * All rights reserved not granted herein.
 * Limited License.
 *
 * Texas Instruments Incorporated grants a world-wide, royalty-free,
 * non-exclusive license under copyrights and patents it now or hereafter
 * owns or controls to make, have made, use, import, offer to sell and sell ("Utilize")
 * this software subject to the terms herein. With respect to the foregoing patent
 * license, such license is granted solely to the extent that any such patent is necessary
 * to Utilize the software alone. The patent license shall not apply to any combinations which
 * include this software, other than combinations with devices manufactured by or for TI ("TI Devices").
 * No hardware patent is licensed hereunder.
 *
 * Redistributions must preserve existing copyright notices and reproduce this license (including the
 * above copyright notice and the disclaimer and (if applicable) source code license limitations below)
 * in the documentation and/or other materials provided with the distribution
 *
 * Redistribution and use in binary form, without modification, are permitted provided that the following
 * conditions are met:
 *
 * * No reverse engineering, decompilation, or disassembly of this software is permitted with respect to any
 * software provided in binary form.
 * * any redistribution and use are licensed by TI for use only with TI Devices.
 * * Nothing shall obligate TI to provide you with source code for the software licensed and provided to you in object code.
 *
 * If software source code is provided to you, modification and redistribution of the source code are permitted
 * provided that the following conditions are met:
 *
 * * any redistribution and use of the source code, including any resulting derivative works, are licensed by
 * TI for use only with TI Devices.
 * * any redistribution and use of any object code compiled from the source code and any resulting derivative
 * works, are licensed by TI for use only with TI Devices.
 *
 * Neither the name of Texas Instruments Incorporated nor the names of its suppliers may be used to endorse or
 * promote products derived from this software without specific prior written permission.
 *
 * DISCLAIMER.
 *
 * THIS SOFTWARE IS PROVIDED BY TI AND TI"S LICENSORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL TI AND TI"S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
*/
#include <errno.h>
#include <tsn_uniconf/yangs/yang_db_access.h>
#include "ix_gptpnet_private.h"
#include <tsn_uniconf/yangs/ietf-interfaces_access.h>

int read_netdev_event(gptpnet_data_t *gpnet, int dvi)
{
	struct iovec vec[1];
	struct msghdr msg;
	char control[512];
	unsigned char buf[GPTPNET_FRAME_SIZE];
	int res;
	netdevice_t *ndev=&gpnet->netdevices[dvi];

	vec[0].iov_base = buf;
	vec[0].iov_len = sizeof(buf);
	(void)memset(&msg, 0, sizeof(msg));
	msg.msg_iov = vec;
	msg.msg_iovlen = 1;
	msg.msg_control = control;
	msg.msg_controllen = sizeof(control);

	res = CB_SOCK_RECVMSG(ndev->fd, &msg, MSG_ERRQUEUE | MSG_DONTWAIT);
	if(res > 0){
		return read_txts(gpnet, dvi, &msg, res);
	}else if(res == 0){
		UB_LOG(UBL_ERROR,"%s:deviceIndex=%d, recvmsg for EQ returned 0\n",
		       __func__, dvi);
		return -1;
	}else if(errno!=EAGAIN ) {
		UB_LOG(UBL_ERROR,"%s:deviceIndex=%d, recvmsg for EQ failed: %s\n",
		       __func__, dvi, strerror(errno));
		return -1;
	}else{}

	res = CB_SOCK_RECVMSG(ndev->fd, &msg, MSG_DONTWAIT);
	if (res > 0){
		return read_recdata(gpnet, dvi, &msg, res);
	}else if(res == 0){
		UB_LOG(UBL_ERROR,"%s:deviceIndex=%d, recvmsg returned 0\n", __func__, dvi);
		return -1;
	}else if(errno!=EAGAIN ) {
		if(ndev->ovip_port && (errno==ECONNREFUSED)){
			// need to wait a connection
			return 1;
		}
		UB_LOG(UBL_ERROR,"%s:deviceIndex=%d, recvmsg failed: %s\n",
		       __func__, dvi, strerror(errno));
		return -1;
	}else{}

	return 1;
}
