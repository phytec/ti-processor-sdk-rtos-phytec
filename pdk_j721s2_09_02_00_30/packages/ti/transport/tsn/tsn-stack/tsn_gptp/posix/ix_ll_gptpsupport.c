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
#include <string.h>
#include <errno.h>
#include <linux/net_tstamp.h>
#include <linux/sockios.h>
#include <linux/ethtool.h>
#include "ll_gptpsupport.h"
#include "gptpclock.h"

int ll_set_hw_timestamping(CB_SOCKET_T cfd, const char *dev)
{
	struct ifreq hwtstamp;
	struct hwtstamp_config hwconfig, hwconfig_requested;
	int so_timestamping_flags = 0;
	int res;

	(void)memset(&hwtstamp, 0, sizeof(hwtstamp));
	if(strstr(dev, CB_VIRTUAL_ETHDEV_PREFIX)==dev){
		(void)strcpy(hwtstamp.ifr_name, "lo");
	}else{
		(void)snprintf(hwtstamp.ifr_name, sizeof(hwtstamp.ifr_name), "%s", dev);
	}
	hwtstamp.ifr_data = (void *)&hwconfig;

	(void)memset(&hwconfig, 0, sizeof(hwconfig));
	hwconfig.tx_type = HWTSTAMP_TX_ON;
	hwconfig.rx_filter = HWTSTAMP_FILTER_PTP_V2_L2_EVENT;
	hwconfig_requested = hwconfig;

	res=ioctl(cfd, SIOCSHWTSTAMP, &hwtstamp);
	if(res<0){
		hwconfig.rx_filter = HWTSTAMP_FILTER_PTP_V2_EVENT;
		hwconfig_requested = hwconfig;
		res=ioctl(cfd, SIOCSHWTSTAMP, &hwtstamp);
	}

	if (res >= 0) {
		UB_LOG(UBL_INFO,"%s:SIOCSHWTSTAMP: tx_type %d requested, got %d; "
		       "rx_filter %d requested, got %d\n",__func__,
		       hwconfig_requested.tx_type, hwconfig.tx_type,
		       hwconfig_requested.rx_filter, hwconfig.rx_filter);

		so_timestamping_flags |= SOF_TIMESTAMPING_TX_HARDWARE;
		so_timestamping_flags |= SOF_TIMESTAMPING_RX_HARDWARE;
		so_timestamping_flags |= SOF_TIMESTAMPING_SYS_HARDWARE;
		so_timestamping_flags |= SOF_TIMESTAMPING_RAW_HARDWARE;
	} else {
		UB_LOG(UBL_INFO,"HW TIMESTAMPING is not available, use software TIMESTAMPING\n");
		so_timestamping_flags |= SOF_TIMESTAMPING_TX_SOFTWARE;
		so_timestamping_flags |= SOF_TIMESTAMPING_RX_SOFTWARE;
		so_timestamping_flags |= SOF_TIMESTAMPING_SOFTWARE;
	}

	if (setsockopt(cfd, SOL_SOCKET, SO_TIMESTAMPING,
		       &so_timestamping_flags, sizeof(so_timestamping_flags)) < 0) {
		UB_LOG(UBL_ERROR,"%s:setsockopt SO_TIMESTAMPING, %s\n",__func__, strerror(errno));
		return -1;
	}

	return 0;
}

int ll_close_hw_timestamping(CB_SOCKET_T cfd, const char *dev)
{
	struct ifreq hwtstamp;
	struct hwtstamp_config hwconfig;

	UB_LOG(UBL_DEBUG,"%s:\n",__func__);
	(void)memset(&hwtstamp, 0, sizeof(hwtstamp));
	if(strstr(dev, CB_VIRTUAL_ETHDEV_PREFIX)==dev){
		(void)strcpy(hwtstamp.ifr_name, "lo");
	}else{
		(void)snprintf(hwtstamp.ifr_name, sizeof(hwtstamp.ifr_name), "%s", dev);
	}
	hwtstamp.ifr_data = (void *)&hwconfig;

	(void)memset(&hwconfig, 0, sizeof(hwconfig));
	hwconfig.tx_type = HWTSTAMP_TX_OFF;
	hwconfig.rx_filter = HWTSTAMP_FILTER_NONE;
	return ioctl(cfd, SIOCSHWTSTAMP, &hwtstamp);
}

int ll_txmsg_timestamp(void *p, int64_t *ts64)
{
	struct cmsghdr *cmsg;
	struct timespec *tmp;
	int level, type;
	struct timespec *ts = NULL;
	struct msghdr *msg=(struct msghdr *)p;

	for (cmsg = CMSG_FIRSTHDR(msg); cmsg != NULL; cmsg = CMSG_NXTHDR(msg, cmsg)) {
		level = cmsg->cmsg_level;
		type  = cmsg->cmsg_type;

		if ((SOL_SOCKET == level) && (SO_TIMESTAMPING == type)) {

			if (cmsg->cmsg_len < sizeof(*tmp)*3u) {
				UB_LOG(UBL_ERROR,"received short so_timestamping\n");
				return -1;
			}
			tmp = (struct timespec*)CMSG_DATA(cmsg);
			if (tmp[1].tv_sec!=0) {
				ts = &tmp[1];
				UB_LOG(UBL_DEBUG,"HW SYS Tx TIMESTAMP %ldsec, %ldnsec\n",
				       ts->tv_sec, ts->tv_nsec);
			}else if (tmp[2].tv_sec!=0) {
				ts = &tmp[2];
				UB_LOG(UBL_DEBUG,"HW RAW Tx TIMESTAMP %ldsec, %ldnsec\n",
				       ts->tv_sec, ts->tv_nsec);
			}else if (tmp[0].tv_sec!=0) {
				ts = &tmp[0];
				UB_LOG(UBL_DEBUG,"SW Tx TIMESTAMP %ldsec, %ldnsec\n",
				       ts->tv_sec, ts->tv_nsec);
			}else{}

		}
	}
	if(ts!=NULL) {
		*ts64=UB_TS2NSEC(*ts);
		return 0;
	}
	return 1;
}

int ll_recv_timestamp(void *p, int64_t *ts64)
{
	struct timeval *tv=NULL;
	struct cmsghdr *cmsg;
	bool gotts=false;
	struct msghdr *msg=(struct msghdr *)p;

	for (cmsg = CMSG_FIRSTHDR(msg); cmsg != NULL;
	     cmsg = CMSG_NXTHDR(msg, cmsg)) {
		struct timespec *stamp;
		if (cmsg->cmsg_level != SOL_SOCKET){continue;}
		switch (cmsg->cmsg_type){
		case SCM_TIMESTAMP:
			tv = (struct timeval *)CMSG_DATA(cmsg);
			*ts64=UB_TV2NSEC(*tv);
			gotts=true;
			break;
		case SO_TIMESTAMPING:
			/* array of three time stamps: software, HW, raw HW */
			stamp = (struct timespec*)CMSG_DATA(cmsg);

			if (cmsg->cmsg_len < sizeof(*stamp)*3u) {
				UB_LOG(UBL_ERROR,"received short SO_TIMESTAMPING (%d/%d)\n",
				       (int)cmsg->cmsg_len, (int)sizeof(*stamp)*3);
				return -1;
			}
			/* look at second element in array which is the HW tstamp */
			stamp++;
			if (stamp->tv_sec!=0) {
				*ts64=UB_TS2NSEC(*stamp);
				UB_LOG(UBL_DEBUG,"HW SYS Rx TIMESTAMP %ldsec, %ldnsec\n",
				       stamp->tv_sec, stamp->tv_nsec);
				gotts=true;
				break;
			}
			/* No SYS HW time stamp, look for a RAW HW time stamp. */
			stamp++;
			if (stamp->tv_sec!=0) {
				*ts64=UB_TS2NSEC(*stamp);
				UB_LOG(UBL_DEBUG,"HW RAW Rx TIMESTAMP %ldsec, %ldnsec\n",
				       stamp->tv_sec, stamp->tv_nsec);
				gotts=true;
				break;
			}
			/* No HW time stamp, look for a SW time stamp. */
			stamp--;
			stamp--;
			if (stamp->tv_sec!=0) {
				*ts64=UB_TS2NSEC(*stamp);
				UB_LOG(UBL_DEBUG,"SW Rx TIMESTAMP %ldsec, %ldnsec\n",
				       stamp->tv_sec, stamp->tv_nsec);
				gotts=true;
				break;
			}
			UB_LOG(UBL_ERROR,"Time stamping for Rx Not received!!!\n");
			break;
		default:
			break;
		}
	}
	if(gotts){return 0;}
	return 1;
}
