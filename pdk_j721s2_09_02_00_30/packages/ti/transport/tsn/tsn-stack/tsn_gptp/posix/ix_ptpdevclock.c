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
#include "ll_gptpsupport.h"
#include <time.h>
#include <sys/timex.h>

#define PTPDEV_CLOCKFD 3
#define FD_TO_CLOCKID(ptpfd) ((~(clockid_t) (ptpfd) << 3) | PTPDEV_CLOCKFD)

PTPFD_TYPE ptpdev_clock_open(char *ptpdev, int permission)
{
	return open(ptpdev, permission);
}

int ptpdev_clock_close(PTPFD_TYPE fd)
{
	return close(fd);
}

int ptpdev_clock_gettime(PTPFD_TYPE fd, int64_t *ts)
{
	struct timespec tspec;

	clock_gettime(FD_TO_CLOCKID(fd), &tspec);
	*ts = UB_TS2NSEC(tspec);

	return 0;
}

int ptpdev_clock_settime(PTPFD_TYPE fd, int64_t *ts)
{
	struct timespec tspec;

	UB_NSEC2TS(*ts,tspec);
	clock_settime(FD_TO_CLOCKID(fd), &tspec);

	return 0;
}

int ptpdev_clock_adjtime(PTPFD_TYPE ptpfd, int adjppb)
{
	struct timex tmx;

	(void)memset(&tmx, 0, sizeof(tmx));
	tmx.modes=ADJ_FREQUENCY;
	tmx.freq=(long)(adjppb * 65.536);

	return clock_adjtime(FD_TO_CLOCKID(ptpfd), &tmx);
}

int ptpdev_clock_setoffset(PTPFD_TYPE fd, int64_t offset)
{
	struct timex tmx;

	if (!offset){return 0;}

	(void)memset(&tmx, 0, sizeof(tmx));
	/* The ADJ_NANO mode is used, then tv_usec will contain a nanosec unit */
	tmx.modes = ADJ_SETOFFSET | ADJ_NANO;
	if (offset > 0) {
		tmx.time.tv_sec = offset / UB_SEC_NS;
		tmx.time.tv_usec = offset % UB_SEC_NS;
	} else {
		int64_t positive_offset = -offset;
		/*
		 * With negative offset, tv_sec points to the earlier
		 * second, and tv_usec counts the nanoseconds since
		 * then, so tv_usec is always a positive number.
		 */
		tmx.time.tv_sec = -((positive_offset - 1) / UB_SEC_NS) - 1;
		tmx.time.tv_usec = UB_SEC_NS - ((positive_offset - 1) % UB_SEC_NS) - 1;
	}
	return clock_adjtime(FD_TO_CLOCKID(fd), &tmx);
}
