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
#include <stdio.h>
#include <setjmp.h>
#include <cmocka.h>
#include "gptpclock.h"

typedef struct ptpfd_index {
	const char *ptpdev;
	int fd;
	int clockid;
	int64_t phase_adj;
	int freq_adj;
	int64_t lastts;
	int64_t lastpts;
} ptpfd_index_t;

#define MAX_PTPFD_NUM 4
static ptpfd_index_t ptpd[5]={
	{GPTP_VIRTUAL_PTPDEV_PREFIX"0", 10, FD_TO_CLOCKID(10), 0, 0},
	{GPTP_VIRTUAL_PTPDEV_PREFIX"1", 11, FD_TO_CLOCKID(11), 0, 0},
	{GPTP_VIRTUAL_PTPDEV_PREFIX"2", 12, FD_TO_CLOCKID(12), 0, 0},
	{GPTP_VIRTUAL_PTPDEV_PREFIX"3", 13, FD_TO_CLOCKID(13), 0, 0},
	{NULL, -1, 0, 0},
};

static int ptpd_index_from_ptpfd(int ptpfd)
{
	int i;
	for(i=0;i<MAX_PTPFD_NUM;i++){
		if(ptpd[i].fd==ptpfd){return i;}
	}
	return -1;
}

static int ptpd_index_from_ptpdev(char *ptpdev)
{
	int i;
	for(i=0;i<MAX_PTPFD_NUM;i++){
		if(!strcmp(ptpd[i].ptpdev, ptpdev)){return i;}
	}
	return -1;
}

int __wrap_clock_gettime(clockid_t clk_id, struct timespec *tp)
{
	int i;
	uint64_t ts64;
	int64_t dts64, dpts64;
	switch(clk_id){
	case CLOCK_REALTIME:
		ts64=ub_rt_gettime64();
		UB_NSEC2TS(ts64, *tp);
		return 0;
	case CLOCK_MONOTONIC:
		ts64=ub_mt_gettime64();
		UB_NSEC2TS(ts64, *tp);
		return 0;
	default:
		for(i=0;i<MAX_PTPFD_NUM;i++){
			if(ptpd[i].clockid!=clk_id){continue;}
			ts64=ub_mt_gettime64();
			if(!ptpd[i].lastts){
				ptpd[i].lastpts=ts64;
				ptpd[i].lastts=ts64;
				UB_NSEC2TS(ts64, *tp);
				return 0;
			}
			dts64=ts64-ptpd[i].lastts;
			dpts64=dts64*ptpd[i].freq_adj/UB_SEC_NS;
			ptpd[i].lastts=ts64;
			ts64=ptpd[i].lastpts+dts64+dpts64;
			ptpd[i].lastpts=ts64;
			UB_NSEC2TS(ts64, *tp);
			return 0;
		}
	}
	return -1;
}

int __wrap_clock_settime(clockid_t clk_id, const struct timespec *tp)
{
	int i;
	int64_t ts64;
	for(i=0;i<MAX_PTPFD_NUM;i++){
		if(ptpd[i].clockid!=clk_id){continue;}
		ts64=UB_TS2NSEC(*tp);
		ptpd[i].lastpts=ts64;
		ptpd[i].lastts=ub_mt_gettime64();
		return 0;
	}
	return -1;
}

int __wrap_gptp_clock_adjtime(int ptpfd, int adjppb)
{
	int i;
	if((i=ptpd_index_from_ptpfd(ptpfd))<0){return -1;}
	ptpd[i].freq_adj=adjppb;
	return 0;
}

#define PTPCLOCK_OPEN_TOUT 100 // msec
ptpclock_state_t __wrap_gptp_get_ptpfd(char *ptpdev, int *ptpfd)
{
	int i;
	if((i=ptpd_index_from_ptpdev(ptpdev))<0){return PTPCLOCK_NOWORK;}
	*ptpfd=ptpd[i].fd;
	return (ptpclock_state_t)mock();
}

int __wrap_gptp_close_ptpfd(int ptpfd)
{
	if(ptpd_index_from_ptpfd(ptpfd)<0){return -1;}
	return 0;
}
