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
#include <stdlib.h>
#include <errno.h>
#include <tsn_unibase/unibase.h>
#include <tsn_combase/combase_link.h>
#include <tsn_uniconf/yangs/yang_db_access.h>
#include "gptpnet.h"
#include "gptpclock.h"
#include "ix_gptpnet_private.h"
#include "gptpconf/gptpgcfg.h"
#include "gptpcommon.h"

#ifndef GPTPNET_PLATFORM_INIT
#define GPTPNET_PLATFORM_INIT
#endif //GPTPNET_PLATFORM_INIT

#ifndef GPTPNET_PLATFORM_CLOSE
#define GPTPNET_PLATFORM_CLOSE
#endif //GPTPNET_PLATFORM_CLOSE

#ifndef GPTPNET_PLATFORM_EVENT
#define GPTPNET_PLATFORM_EVENT
#endif //GPTPNET_PLATFORM_EVENT

/* allow to overwrite the socket write function */
#ifndef GPTPNET_PLATFORM_WRITE
#define GPTPNET_PLATFORM_WRITE(ndev, length) \
	CB_SOCK_WRITE(ndev->fd, &ndev->sbuf, length)
#endif //GPTPNET_PLATFORM_WRITE

extern char *PTPMsgType_debug[16];

// gptpgcfg_link_check is in gptpgcfg.c
// to make gptpgcfg.h public, it is  excluded from gptpgcfg.h and need 'extern' here
extern int gptpgcfg_link_check(uint8_t gptpInstanceIndex, gptpnet_data_netlink_t *edtnl);
extern int gptpgcfg_get_ptpdev(uint8_t gptpInstanceIndex, uint8_t domainIndex,
                               char *netdev, char *value, uint32_t vsize);

// determine ptpdev to be used for the corresponding netdev
// pseudocode:
//   - get ptpdev value from configuration
//   - if not configured, get from platform mapping via combase
//   - if success return 0, otherwise error, return -1
static int get_ptpdev_from_netdev(uint8_t gptpInstanceIndex, char *netdev,
                                  char * ptpdev, uint32_t ptpdev_len)
{
	if(!gptpgcfg_get_ptpdev(gptpInstanceIndex, 0, netdev, ptpdev, ptpdev_len)&&
		strlen(ptpdev)>0){
		return 0;
	}
	return cb_get_ptpdev_from_netdev(netdev, ptpdev);
}

static int onenet_init(uint8_t gptpInstanceIndex, netdevice_t *ndev, const char *netdev)
{
	cb_rawsock_paras_t llrawp;
	cb_rawsock_ovip_para_t ovipp;
	const ub_macaddr_t destmac = GPTP_MULTICAST_DEST_ADDR;
	int res=0;

	if(netdev!=NULL){
		(void)snprintf(ndev->nlstatus.devname, CB_MAX_NETDEVNAME, "%s", netdev);
	}
	if(!get_ptpdev_from_netdev(gptpInstanceIndex, ndev->nlstatus.devname,
	                           ndev->nlstatus.ptpdev, sizeof(ndev->nlstatus.ptpdev))){
		res=1;
	}else{
		ndev->nlstatus.ptpdev[0]=0;
	}
	UB_LOG(UBL_DEBUG,"%s: netdev=%s mapped to ptpdev=%s\n",
	       __func__, ndev->nlstatus.devname, ndev->nlstatus.ptpdev);

	ndev->txtslost_time = gptpgcfg_get_intitem(
		gptpInstanceIndex, XL4_EXTMOD_XL4GPTP_TXTS_LOST_TIME,
		YDBI_CONFIG);
	(void)memset(&llrawp, 0, sizeof(llrawp));
	llrawp.dev=ndev->nlstatus.devname;
	llrawp.proto=ETH_P_1588;
	llrawp.vlan_proto=0;
	llrawp.priority= gptpgcfg_get_intitem(
		gptpInstanceIndex, XL4_EXTMOD_XL4GPTP_SOCKET_TXPRIORITY,
		YDBI_CONFIG);
	llrawp.rw_type=CB_RAWSOCK_RDWR;
	if(ndev->ovip_port!=0u){
		(void)memset(&ovipp, 0, sizeof(ovipp));
		ovipp.laddr=(127u<<24u)|(0u<<16u)|(0u<<8u)|1u;
		ovipp.daddr=(127u<<24u)|(0u<<16u)|(0u<<8u)|1u;
		ovipp.lport=ndev->ovip_port;
		ovipp.dport=((ndev->ovip_port%2u)!=0u)?(ndev->ovip_port-1u):(ndev->ovip_port+1u);
		llrawp.ovipp=&ovipp;
		llrawp.sock_mode=CB_SOCK_MODE_OVIP;
	}
	gptpgcfg_releasedb(gptpInstanceIndex);
	if(cb_rawsock_open(&llrawp, &ndev->fd, &ndev->addr, &ndev->mtusize,
			   ndev->sbuf.ehd.H_SOURCE)!=0){return -1;}
	memcpy(ndev->sbuf.ehd.H_DEST, destmac, ETH_ALEN);
	ndev->sbuf.ehd.H_PROTO = htons(ETH_P_1588);
	if(cb_reg_multicast_address(ndev->fd, ndev->nlstatus.devname, ndev->sbuf.ehd.H_DEST, 0)!=0) {
		UB_LOG(UBL_ERROR,"failed to add multicast address");
		goto erexit;
	}
	if(ll_set_hw_timestamping(ndev->fd, ndev->nlstatus.devname)!=0){goto erexit;}
	eui48to64(ndev->sbuf.ehd.H_SOURCE, ndev->nlstatus.portid,NULL);
	return res;
erexit:
	CB_SOCK_CLOSE(ndev->fd);
	ndev->fd = CB_SOCKET_INVALID_VALUE;
	return -1;
}

static int onenet_activate(gptpnet_data_t *gpnet, int ndevIndex)
{
	netdevice_t *ndev=&gpnet->netdevices[ndevIndex];
	void *value;
	uint64_t speed=0;

	ndev->nlstatus.up=0;
	YDBI_GET_ITEM_VSUBST(uint8_t*, ifk1vk0, ndev->nlstatus.up, value,
			     ndev->nlstatus.devname, IETF_INTERFACES_OPER_STATUS, YDBI_STATUS);
	ndev->nlstatus.duplex=CB_DUPLEX_FULL;
	YDBI_GET_ITEM_VSUBST(uint32_t*, ifk1vk0, ndev->nlstatus.duplex, value,
			     ndev->nlstatus.devname, IETF_INTERFACES_DUPLEX, YDBI_STATUS);

	YDBI_GET_ITEM_VSUBST(uint64_t*, ifk1vk0, speed, value,
			     ndev->nlstatus.devname, IETF_INTERFACES_SPEED, YDBI_STATUS);
	ndev->nlstatus.speed=speed/1000000u;
	if(ndev->nlstatus.speed == 0u){ndev->nlstatus.up = false;}
	UB_LOG(UBL_INFO, "%s:%s status=%d, duplex=%d, speed=%dMbps\n", __func__,
			ndev->nlstatus.devname, ndev->nlstatus.up, ndev->nlstatus.duplex,
			ndev->nlstatus.speed);
	if(!gpnet->cb_func || !ndev->nlstatus.up){return 0;}
	return gpnet->cb_func(gpnet->cb_data, ndevIndex+1, GPTPNET_EVENT_DEVUP,
			      &gpnet->event_ts64, &ndev->nlstatus);
}

int read_recdata(gptpnet_data_t *gpnet, int dvi, struct msghdr *msg, int len)
{
	uint8_t *buf=msg->msg_iov[0].iov_base;
	event_data_recv_t edtrecv;
	(void)memset(&edtrecv, 0, sizeof(edtrecv));
	if ((msg->msg_flags & MSG_TRUNC)!=0) {
		UB_LOG(UBL_ERROR,"deviceIndex=%d, received truncated message\n", dvi);
		return -1;
	}
	if ((msg->msg_flags & MSG_CTRUNC)!=0) {
		UB_LOG(UBL_ERROR,"deviceIndex=%d, received truncated ancillary data\n", dvi);
		return -1;
	}
	edtrecv.recbptr=&buf[ETH_HLEN];
	edtrecv.domain=PTP_HEAD_DOMAIN_NUMBER(&buf[ETH_HLEN]);
	edtrecv.msgtype=PTP_HEAD_MSGTYPE(&buf[ETH_HLEN]);
	if(edtrecv.msgtype<8u){
		if(ll_recv_timestamp(msg, &edtrecv.ts64)!=0) {
			UB_LOG(UBL_ERROR,"%s:deviceIndex=%d, no Rx timestamp for msgtype=%s,"
			       " domain=%d\n",
			       __func__, dvi, PTPMsgType_debug[edtrecv.msgtype], edtrecv.domain);
			return -1;
		}
		if(gpnet->netdevices[dvi].ovip_port && (edtrecv.msgtype==0u)){
			edtrecv.ts64+=gptpclock_d0ClockfromRT(gpnet->gptpInstanceIndex, dvi+1);
		}
	}
	if(!gpnet->cb_func){return -1;}
	return gpnet->cb_func(gpnet->cb_data, dvi+1, GPTPNET_EVENT_RECV,
				      &gpnet->event_ts64, &edtrecv);
}

int read_txts(gptpnet_data_t *gpnet, int dvi, struct msghdr *msg, int len)
{
	netdevice_t *ndev;
	uint8_t *buf=msg->msg_iov[0].iov_base;
	event_data_txts_t edtxts;

	// once the TxTS is captured, the guard time is not needed
	gpnet->netdevices[dvi].guard_time=0;

	(void)memset(&edtxts, 0, sizeof(edtxts));
	if(len < 48) {
		UB_LOG(UBL_ERROR,"%s:deviceIndex=%d, recvmsg returned only %d bytes\n",
		       __func__, dvi, len);
		return -1;
	}
	if(gpnet->netdevices[dvi].ovip_port!=0u){buf=&buf[ETH_HLEN+20+8];}
	uint16_t sd;
	memcpy(&sd, &buf[12], 2);
	if(ntohs(sd)!=ETH_P_1588){
		UB_LOG(UBL_DEBUG,
		       "%s:deviceIndex=%d, not ETH_P_1588 packet 0x%02X%02X\n",
		       __func__, dvi, buf[12], buf[13]);
		return -1;
	}

	edtxts.msgtype=PTP_HEAD_MSGTYPE(&buf[ETH_HLEN]);
	edtxts.seqid=(uint16_t)PTP_HEAD_SEQID(&buf[ETH_HLEN]);
	edtxts.domain=PTP_HEAD_DOMAIN_NUMBER(&buf[ETH_HLEN]);
	if(edtxts.msgtype >= 8u){
		UB_LOG(UBL_DEBUG,"deviceIndex=%d, msgtype:%d is not Event, ignore this\n",
		       dvi, edtxts.msgtype);
		return -1;
	}

	if(ll_txmsg_timestamp(msg, &edtxts.ts64)!=0){return -1;}
	if(gpnet->netdevices[dvi].ovip_port && (edtxts.msgtype==0u)){
		edtxts.ts64+=gptpclock_d0ClockfromRT(gpnet->gptpInstanceIndex, dvi+1);
	}
	if(!gpnet->cb_func){return -1;}
	ndev=&gpnet->netdevices[dvi];
	ndev->waiting_txts=false;
	gpnet->cb_func(gpnet->cb_data, dvi+1, GPTPNET_EVENT_TXTS,
		       &gpnet->event_ts64, &edtxts);
	return 0;
}

static int find_netdev(netdevice_t *devices, int dnum, char *netdev)
{
	int i;
	for(i=0;i<dnum;i++){
		if(!strcmp(netdev, devices[i].nlstatus.devname)){return i;}
	}
	return -1;
}

/*
  reopen is needed for workaround of Intel i225 disconnection after CBS setting
  normal disconnection and re-connection doesn't need reopening the socket.
*/
static int reopen_socket(gptpnet_data_t *gpnet, int ndevIndex, bool no_open)
{
	netdevice_t *ndev=&gpnet->netdevices[ndevIndex];
	UB_TLOG(UBL_INFO, "%s:device=%s\n", __func__, ndev->nlstatus.devname);
	if(ndev->fd!=CB_SOCKET_INVALID_VALUE){
		close(ndev->fd);
		ndev->fd=CB_SOCKET_INVALID_VALUE;
	}
	if(no_open){return 0;}
	return onenet_init(gpnet->gptpInstanceIndex, ndev, NULL);
}

static void check_guard_expire(gptpnet_data_t *gpnet, int64_t ts64)
{
	int i;
	uint32_t gtms=0;
	void *value;
	gptpnet_data_netlink_t edtnl;
	char *devname;
	for(i=0;i<gpnet->num_netdevs;i++){
		if(!gpnet->netdevices[i].nlstatus.downts){
			// not holding EVENT_DEVDOWN
			continue;
		}
		gtms=0;
		devname=gpnet->netdevices[i].nlstatus.devname;
		YDBI_GET_ITEM_VSUBST(uint32_t*, ifk1vk0, gtms, value, devname,
				     IETF_INTERFACES_DISCON_WORKAROUND_TIME,
				     YDBI_CONFIG);
		if((ts64-gpnet->netdevices[i].nlstatus.downts)/UB_MSEC_NS>gtms){
			// holding time is expired, issue EVENT_DEVDOWN
			UB_TLOG(UBL_INFO, "%s:%s expired DOWN\n",__func__, devname);
			(void)memset(&edtnl, 0, sizeof(event_data_netlink_t));
			memcpy(edtnl.devname, devname, strlen(devname)+1);
			gpnet->netdevices[i].nlstatus.downts=0;
			gpnet->cb_func(gpnet->cb_data, i+1, GPTPNET_EVENT_DEVDOWN, &ts64,
				       &edtnl);
		}

	}
}

static int gptpnet_link_check(gptpnet_data_t *gpnet, int64_t ts64)
{
	int res;
	gptpnet_data_netlink_t edtnl;
	int ndevIndex;
	gptpnet_event_t event;
	uint32_t gtms=0;
	void *value;
	check_guard_expire(gpnet, ts64);
	res=gptpgcfg_link_check(gpnet->gptpInstanceIndex, &edtnl);
	if(res<0){return -1;}
	if(res!=0){return 0;}
	ndevIndex=find_netdev(gpnet->netdevices, gpnet->num_netdevs, edtnl.devname);
	YDBI_GET_ITEM_VSUBST(uint32_t*, ifk1vk0, gtms, value, edtnl.devname,
			     IETF_INTERFACES_DISCON_WORKAROUND_TIME,
			     YDBI_CONFIG);
	if(edtnl.up!=0u){
		if(gtms==0){
			UB_TLOG(UBL_INFO, "%s:%s UP\n",__func__, edtnl.devname);
			event=GPTPNET_EVENT_DEVUP;
		}else if(gpnet->netdevices[ndevIndex].nlstatus.downts==0){
			UB_TLOG(UBL_INFO, "%s:%s expired UP\n",__func__, edtnl.devname);
			reopen_socket(gpnet, ndevIndex, false); // reopen
			event=GPTPNET_EVENT_DEVUP;
		}else{
			UB_TLOG(UBL_INFO, "%s:%s GUARDUP\n",__func__, edtnl.devname);
			gpnet->netdevices[ndevIndex].nlstatus.downts=0;
			reopen_socket(gpnet, ndevIndex, false); // reopen
			event=GPTPNET_EVENT_GUARDUP;
		}
	}else{
		if(gtms==0){
			UB_TLOG(UBL_INFO, "%s:%s DOWN\n",__func__, edtnl.devname);
			event=GPTPNET_EVENT_DEVDOWN;
			gpnet->netdevices[ndevIndex].nlstatus.downts=0;
		}else{
			UB_TLOG(UBL_INFO, "%s:%s GUARDDOWN\n",__func__, edtnl.devname);
			gpnet->netdevices[ndevIndex].nlstatus.downts=ts64;
			// holding EVENT_DEVDOWN for gtms time
			reopen_socket(gpnet, ndevIndex, true); // close
			event=GPTPNET_EVENT_GUARDDOWN;
		}
	}
	return gpnet->cb_func(gpnet->cb_data, ndevIndex+1, event, &ts64, &edtnl);
}

static int gptpnet_catch_event(gptpnet_data_t *gpnet)
{
	fd_set rfds;
	int maxfd=0;
	int64_t ts64, tstout64;
	struct timeval tvtout;
	int res=0;
	int i;
	static int64_t last_ts64=0;

	ts64=ub_mt_gettime64();
	(void)gptpnet_link_check(gpnet, ts64);
	FD_ZERO(&rfds);
	for(i=0;i<gpnet->num_netdevs;i++){
		if(CB_SOCKET_VALID(gpnet->netdevices[i].fd)){
			FD_SET(gpnet->netdevices[i].fd, &rfds);
		}
		maxfd=UB_MAX(maxfd, gpnet->netdevices[i].fd);
	}

	tstout64=ts64-last_ts64;
	// every 10 seconds, print clock parameters for debug
	if(tstout64>(10*UB_SEC_NS)){
		gptpclock_print_clkpara(gpnet->gptpInstanceIndex, UBL_INFO);
		last_ts64=ts64;
	}

	if(gpnet->next_tout64!=0){
		tstout64=gpnet->next_tout64-ts64;
		if(tstout64<0){
			gpnet->next_tout64=0;
			UB_LOG(UBL_DEBUG,"%s:call missed or extra TIMEOUT CB\n", __func__);
			res = gpnet->cb_func(gpnet->cb_data, 0, GPTPNET_EVENT_TIMEOUT,
					     &ts64, NULL);
			return 0;
		}
	} else {
		gpnet->next_tout64=((ts64 / GPTPNET_INTERVAL_TIMEOUT_NSEC) + 1) *
			GPTPNET_INTERVAL_TIMEOUT_NSEC;
	}
	gptpgcfg_releasedb(gpnet->gptpInstanceIndex);
	UB_NSEC2TV(gpnet->next_tout64-ts64, tvtout);
	res=select(maxfd+1, &rfds, NULL, NULL, &tvtout);
	if(res == -1){
		UB_LOG(UBL_ERROR,"%s:select error %s\n", __func__, strerror(errno));
		return -1;
	}
	gpnet->event_ts64=ub_mt_gettime64();

	if(res == 0){
		if(!gpnet->cb_func){return -1;}
		gpnet->next_tout64=0;
		res = gpnet->cb_func(gpnet->cb_data, 0, GPTPNET_EVENT_TIMEOUT,
				     &gpnet->event_ts64, NULL);
		return res;
	}
	for(i=0;i<gpnet->num_netdevs;i++){
		if(CB_SOCKET_VALID(gpnet->netdevices[i].fd) &&
		   FD_ISSET(gpnet->netdevices[i].fd, &rfds)){
			while(!read_netdev_event(gpnet, i)){;}
		}
	}
	return res;
}

gptpnet_data_t *gptpnet_init(uint8_t gptpInstanceIndex, gptpnet_cb_t cb_func,
			     void *cb_data, const char *netdev[], uint8_t num_ports,
			     char *master_ptpdev)
{
	gptpnet_data_t *gpnet;
	int i;
	int first_devwptp=-1;
	int res;

	GPTPNET_PLATFORM_INIT;

	gpnet=UB_SD_GETMEM(GPTP_MEDIUM_ALLOC, sizeof(gptpnet_data_t));
	if(ub_assert_fatal(gpnet!=NULL, __func__, "malloc")){return NULL;}
	(void)memset(gpnet, 0, sizeof(gptpnet_data_t));
	gpnet->gptpInstanceIndex=gptpInstanceIndex;
	gpnet->num_netdevs=num_ports;
	gpnet->netdevices=UB_SD_GETMEM(GPTP_MEDIUM_ALLOC, num_ports * sizeof(netdevice_t));
	if(ub_assert_fatal(gpnet->netdevices, __func__, "malloc")){
		UB_SD_RELMEM(GPTP_MEDIUM_ALLOC, gpnet);
		return NULL;
	}
	(void)memset(gpnet->netdevices, 0, num_ports * sizeof(netdevice_t));
	for(i=0;i<gpnet->num_netdevs;i++){
		if(strstr(netdev[i], CB_VIRTUAL_ETHDEV_PREFIX)==netdev[i]){
			gpnet->netdevices[i].ovip_port =
				gptpgcfg_get_intitem(
					gpnet->gptpInstanceIndex,
					XL4_EXTMOD_XL4GPTP_OVIP_MODE_STRT_PORTNO,
					YDBI_CONFIG)+(i*2);
		}
		res=onenet_init(gptpInstanceIndex,
				&gpnet->netdevices[i], netdev[i]);
		if(res==0){continue;}
		if(res<0) {
			UB_LOG(UBL_ERROR, "networkd device:%s can't be opened\n",netdev[i]);
			continue;
		}
		if(first_devwptp<0){first_devwptp=i;}
		// if master_ptpdev option is set, use it as the first device
		if(master_ptpdev && !strcmp(master_ptpdev,
					    gpnet->netdevices[i].nlstatus.ptpdev)){
			first_devwptp=i;
		}
	}
	if(first_devwptp<0){
		UB_LOG(UBL_ERROR,"%s:ptp device is needed for at least one network device\n",
		       __func__);
		(void)gptpnet_close(gpnet);
		return NULL;
	}
	if(first_devwptp>0){
		netdevice_t swapdev;
		// swap to make the first device have ptpdev
		memcpy(&swapdev, &gpnet->netdevices[0], sizeof(netdevice_t));
		memcpy(&gpnet->netdevices[0], &gpnet->netdevices[first_devwptp],
		       sizeof(netdevice_t));
		memcpy(&gpnet->netdevices[first_devwptp], &swapdev, sizeof(netdevice_t));
	}
	for(i=0;i<gpnet->num_netdevs;i++){
		if(gpnet->netdevices[i].nlstatus.ptpdev[0]!=0){
			UB_LOG(UBL_INFO, "%s: netdev index=%d devname=%s, ptpdev=%s\n",
			       __func__, i, gpnet->netdevices[i].nlstatus.devname,
			       gpnet->netdevices[i].nlstatus.ptpdev);
			// netdev index=0 must have ptpdev
			continue;
		}
		UB_LOG(UBL_INFO, "%s:network device %s doesn't have a ptp device, "
		       "use the first ptp device\n", __func__,
		       gpnet->netdevices[i].nlstatus.devname);
		(void)strcpy(gpnet->netdevices[i].nlstatus.ptpdev,
		       gpnet->netdevices[0].nlstatus.ptpdev);
	}
	gpnet->cb_func=cb_func;
	gpnet->cb_data=cb_data;
	gpnet->event_ts64=ub_mt_gettime64();
	if(gptpgcfg_set_netdevs(gpnet->gptpInstanceIndex, netdev, gpnet->num_netdevs)!=0){
		(void)gptpnet_close(gpnet);
		return NULL;
	}
	return gpnet;
}

int gptpnet_activate(gptpnet_data_t *gpnet)
{
	int i;
	for(i=0;i<gpnet->num_netdevs;i++){
		(void)onenet_activate(gpnet, i);
	}
	return 0;
}

int gptpnet_close(gptpnet_data_t *gpnet)
{
	int i;

	GPTPNET_PLATFORM_CLOSE;

	UB_LOG(UBL_DEBUGV, "%s:\n",__func__);
	if(!gpnet){return -1;}
	gptpgcfg_remove_netdevs(gpnet->gptpInstanceIndex);
	if(gpnet->netdevices!=NULL){
		for(i=0;i<gpnet->num_netdevs;i++){
			if(!CB_SOCKET_VALID(gpnet->netdevices[i].fd)){continue;}
			CB_SOCK_CLOSE(gpnet->netdevices[i].fd);
		}
	}
	UB_SD_RELMEM(GPTP_MEDIUM_ALLOC, gpnet->netdevices);
	UB_SD_RELMEM(GPTP_MEDIUM_ALLOC, gpnet);
	return 0;
}

int gptpnet_eventloop(gptpnet_data_t *gpnet, bool *stoploop)
{
	while(!*stoploop){

		GPTPNET_PLATFORM_EVENT;

		(void)gptpnet_catch_event(gpnet);
		if(ub_fatalerror()){return -1;}
	}
	return 0;
}

uint8_t *gptpnet_get_sendbuf(gptpnet_data_t *gpnet, int ndevIndex)
{
	return gpnet->netdevices[ndevIndex].sbuf.pdata;
}

int gptpnet_send(gptpnet_data_t *gpnet, int ndevIndex, uint16_t length)
{
	char *msg;
	int msgtype;
	uint64_t cts64;
	netdevice_t *ndev;

	if(length>(uint16_t)GPTP_MAX_PACKET_SIZE){
		UB_LOG(UBL_ERROR, "%s:deviceIndex=%d, length=%d is too big\n",
		       __func__, ndevIndex, length);
		return -1;
	}
	ndev=&gpnet->netdevices[ndevIndex];
	msgtype=PTP_HEAD_MSGTYPE(ndev->sbuf.pdata);
	if(msgtype<=15){
		msg=PTPMsgType_debug[msgtype];
	}else{
		msg="unknow";
	}

	cts64=ub_mt_gettime64();
	if(ndev->waiting_txts && (msgtype<8)){
		uint16_t seqid=PTP_HEAD_SEQID(ndev->sbuf.pdata);
		uint8_t domain=PTP_HEAD_DOMAIN_NUMBER(ndev->sbuf.pdata);
		if(ndev->waiting_txts_tout < cts64){
			UB_TLOG(UBL_INFO, "%s:timed out, waiting_txts, send msg=%s, "
				 "dom=%d, sqid=%d, waiting=%s\n",
				 __func__, msg, domain, seqid,
				 PTPMsgType_debug[ndev->waiting_txts_msgtype]);
			ndev->waiting_txts=false;
		}else{
			UB_TLOG(UBL_DEBUG, "%s:waiting_txts, defer this msg=%s, dom=%d, "
				 "sqid=%d, waiting=%s\n", __func__, msg, domain, seqid,
				 PTPMsgType_debug[ndev->waiting_txts_msgtype]);
			// make sure it will be sent in a short time
			gptpnet_extra_timeout(gpnet, 0);
			return -1;
		}
	}
	UB_LOG(UBL_DEBUGV, "SEND:deviceIndex=%d, msgtype=%s\n", ndevIndex, msg);
	if(ndev->guard_time > cts64){
		UB_TLOG(UBL_DEBUG, "%s:defered by gurad_time:deviceIndex=%d, msgtype=%s\n",
			 __func__, ndevIndex, msg);
		gptpnet_extra_timeout(gpnet, 0); // make sure it will be sent in a short time
		return -1;
	}
	ndev->guard_time= cts64 + (uint64_t)gptpgcfg_get_intitem(
		gpnet->gptpInstanceIndex, XL4_EXTMOD_XL4GPTP_AFTERSEND_GUARDTIME,
		YDBI_CONFIG);
	if(msgtype<8) {
		ndev->waiting_txts=true;
		// to let this timeout happen before the other point of TXTS_LOST_TIME,
		// subtract 1msec
		ndev->waiting_txts_tout=cts64 + (uint64_t)gptpgcfg_get_intitem(
			gpnet->gptpInstanceIndex, XL4_EXTMOD_XL4GPTP_TXTS_LOST_TIME,
			YDBI_CONFIG) -
			1000000u;
		ndev->waiting_txts_msgtype=msgtype;
	}
	return GPTPNET_PLATFORM_WRITE(ndev, length+sizeof(CB_ETHHDR_T));
}

char *gptpnet_ptpdev(gptpnet_data_t *gpnet, int ndevIndex)
{
	return gpnet->netdevices[ndevIndex].nlstatus.ptpdev;
}

int gptpnet_num_netdevs(gptpnet_data_t *gpnet)
{
	return gpnet->num_netdevs;
}

uint8_t *gptpnet_portid(gptpnet_data_t *gpnet, int ndevIndex)
{
	return gpnet->netdevices[ndevIndex].nlstatus.portid;
}

void gptpnet_create_clockid(gptpnet_data_t *gpnet, uint8_t *id,
			    int ndevIndex, int8_t domainNumber)
{
	memcpy(id, gpnet->netdevices[ndevIndex].nlstatus.portid, sizeof(ClockIdentity));
	if(domainNumber==0){return;}
	id[3]=0;
	id[4]=domainNumber;
}

int gptpnet_get_nlstatus(gptpnet_data_t *gpnet, int ndevIndex, event_data_netlink_t *nlstatus)
{
	if((ndevIndex < 0) || (ndevIndex >= gpnet->num_netdevs)){
		UB_LOG(UBL_ERROR, "%s:ndevIndex=%d doesn't exist\n",__func__, ndevIndex);
		return -1;
	}
	memcpy(nlstatus, &gpnet->netdevices[ndevIndex].nlstatus, sizeof(event_data_netlink_t));
	return 0;
}

uint64_t gptpnet_txtslost_time(gptpnet_data_t *gpnet, int ndevIndex)
{
	/* give up to read TxTS, if it can't be captured in this time */
	return gpnet->netdevices[ndevIndex].txtslost_time;
}

void gptpnet_extra_timeout(gptpnet_data_t *gpnet, int toutns)
{
	if(toutns==0){
		toutns=gptpgcfg_get_intitem(
		gpnet->gptpInstanceIndex, XL4_EXTMOD_XL4GPTP_GPTPNET_EXTRA_TOUTNS,
		YDBI_CONFIG);
	}
	gpnet->next_tout64=ub_mt_gettime64();
	gpnet->next_tout64+=toutns;
}

int gptpnet_tsn_schedule(gptpnet_data_t *gpnet, uint32_t aligntime, uint32_t cycletime)
{
	/* IEEE 802.1qbv (time-aware traffic shaping) not yet supported */
	return 0;
}
