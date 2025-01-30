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
/*
 * PTP Clock Verification Tool
 * ============================
 * This tool is to check if the PTP clock driver is working properly.
 *
 * Functionality:
 * --------------
 * The tool performs the following functions:
 *   1. Retrieve and display the current PTP time, either once or multiple times.
 *   2. Verify the PTP timestamp to ensure it is increasing within a reasonable range.
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <fcntl.h>
#include <string.h>
#include <getopt.h>
#include <inttypes.h>
#include <linux/ptp_clock.h>
#include <time.h>

#define DEVICE_NAME "/dev/ptp0"

#define NSEC_PER_SEC       1000000000UL
#define MSEC_PER_NSEC      1000000UL
#define USEC_PER_NSEC      1000UL
#define MSEC_PER_SEC       1000UL

#define NSAMPLES_DEFAULT   1000000UL

#define MAX_PTP_ERROR_PPM 1000
#define MAX_PTP_DIFF_NSEC (2*MSEC_PER_NSEC)
#define MIN_PTP_DIFF_NSEC (1*USEC_PER_NSEC)

#ifndef CLOCK_INVALID
#define CLOCK_INVALID -1
#endif

static clockid_t get_ptpclockid(int fd)
{
#define CLOCKFD 3
	return (((unsigned int) ~fd) << 3) | CLOCKFD;
}

struct pcheckopt {
	char *dev;
	uint64_t nsamples;
	int multi_shots;
	uint64_t min_diff_nsec;
	uint64_t max_diff_nsec;
	uint64_t max_ppm;
};

static int print_usage(char *pname, struct pcheckopt *pcheckopt)
{
	char *s;
	s = strchr(pname,'/');
	if(!s) {
		s = pname;
	}

	printf("%s [options]\n", s);
	printf("-h|--help: this help\n");
	printf("-d|--dev: ptp device: default %s\n", pcheckopt->dev);
	printf("-o|--one: one shot of ptp getts and exit\n");
	printf("-m|--multi: shots: multiple shots of ptp getts and exit, default %d\n",
		   pcheckopt->multi_shots);
	printf("-s|--sample: nsamples: clock check times, default %"PRIu64"\n",
		   pcheckopt->nsamples);
	printf("-i|--min: min ptp tsdiff, default %"PRIu64" nsec\n",
		   pcheckopt->min_diff_nsec);
	printf("-a|--max: max ptp tsdiff, default %"PRIu64" nsec\n",
		   pcheckopt->max_diff_nsec);
	printf("-p|--maxppm: max ppm, default %"PRIu64" ppm\n",
		   pcheckopt->max_ppm);
	return -1;
}

static int set_options(struct pcheckopt *pcheckopt, int argc, char *argv[])
{
	int oc;
	int res=0;
	char *endptr;
	struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"dev", required_argument, 0, 'd'},
		{"one", no_argument, 0, 'o'},
		{"sample", required_argument, 0, 's'},
		{"multi", required_argument, 0, 'm'},
		{"min", required_argument, 0, 'i'},
		{"max", required_argument, 0, 'a'},
		{"maxppm", required_argument, 0, 'p'},
		{0, 0, 0, 0}
	};

	while((oc=getopt_long(argc, argv, "hd:s:om:i:a:p:", long_options, NULL))!=-1){
		switch(oc){
		case 'd':
			pcheckopt->dev = optarg;
			break;
		case 's':
			pcheckopt->nsamples = strtoull(optarg, &endptr, 10);
			break;
		case 'o':
			pcheckopt->multi_shots = 1;
			break;
		case 'i':
			pcheckopt->min_diff_nsec = strtoull(optarg, &endptr, 10);
			break;
		case 'a':
			pcheckopt->max_diff_nsec = strtoull(optarg, &endptr, 10);
			break;
		case 'p':
			pcheckopt->max_ppm = strtoull(optarg, &endptr, 10);
			break;
		case 'm':
			pcheckopt->multi_shots = atoi(optarg);
			if (pcheckopt->multi_shots <= 0) {
				printf("wrong multi_shots value %d\n", pcheckopt->multi_shots);
				print_usage(argv[0], pcheckopt);
				return -1;
			}
			break;
		case 'h':
		default:
			print_usage(argv[0], pcheckopt);
			return -1;
		}
	}
	return res;
}

static uint64_t clock_gettime_ns(clockid_t clkid)
{
	struct timespec nowts;
	if(clock_gettime(clkid, &nowts)<0){
		printf("clock_gettime:%s\n", strerror(errno));
		return 0;
	}
	return ((uint64_t)nowts.tv_sec * NSEC_PER_SEC + (uint64_t)nowts.tv_nsec);
}

static uint64_t sysclock_gettime_ns(void)
{
	return clock_gettime_ns(CLOCK_MONOTONIC);
}

static uint64_t ptpclock_gettime_ns(clockid_t clkid)
{
	return clock_gettime_ns(clkid);
}

int main(int argc, char *argv[])
{
	int fd;
	int i;

	uint64_t prev_pts = 0;
	uint64_t pts;
	uint64_t first_pts = 0;
	uint64_t last_pts = 0;

	uint64_t first_sts = 0;
	uint64_t last_sts = 0;

	uint64_t offset_pts;
	uint64_t offset_sts;
	int64_t offset_diff;
	int64_t ppm;

	bool result = true;

	uint64_t counter = 0;
	int64_t diff;
	clockid_t ptp_clkid;
	struct pcheckopt pcheckopt = {
		.dev = DEVICE_NAME,
		.nsamples = NSAMPLES_DEFAULT,
		.min_diff_nsec = MIN_PTP_DIFF_NSEC,
		.max_diff_nsec = MAX_PTP_DIFF_NSEC,
		.multi_shots = 0,
		.max_ppm = MAX_PTP_ERROR_PPM,
	};

	if(set_options(&pcheckopt, argc, argv)) {
		return -1;
	}

	fd = open(pcheckopt.dev, O_RDONLY);
	if (fd < 0) {
		printf("Open %s: %s\n", pcheckopt.dev, strerror(errno));
		return -1;
	}

	ptp_clkid = get_ptpclockid(fd);

	for (i = 0; i < pcheckopt.multi_shots; i++) {
		pts = ptpclock_gettime_ns(ptp_clkid);
		printf("ptpts:%"PRIu64" nsec\n", pts);
	}
	if (pcheckopt.multi_shots > 0) {
		close(fd);
		return 0;
	}

	printf("Option: ptpdev=%s samples=%"PRIu64 " min_diff=%"PRIu64
		   "nsec max_diff=%"PRIu64"nsec max_ppm=%"PRIu64"\n",
		   pcheckopt.dev, pcheckopt.nsamples,
		   pcheckopt.min_diff_nsec,
		   pcheckopt.max_diff_nsec,
		   pcheckopt.max_ppm);

	printf("Start reading: ptpts_now=%"PRIu64"\n", ptpclock_gettime_ns(ptp_clkid));

	first_pts = ptpclock_gettime_ns(ptp_clkid);
	first_sts = sysclock_gettime_ns();

	while (counter < pcheckopt.nsamples) {
		pts = ptpclock_gettime_ns(ptp_clkid);
		if (pts == 0) {
			printf("Invalid ptpts=0!!!\n");
			break;
		}
		if (prev_pts >= pts) {
			diff = pts - prev_pts;
			printf("!!!!pts is decreased or stalled:now=%"PRIu64
				   " previous=%"PRIu64" diff=%"PRIi64"\n",
				   pts, prev_pts, diff);
			break;
		} else if (prev_pts) {
			diff = pts - prev_pts;
			if (diff >= (int64_t)pcheckopt.max_diff_nsec) {
				printf("!!!!diff too much:now=%"PRIu64
					   " previous=%"PRIu64" diff=%"PRIi64"\n",
					   pts, prev_pts, diff);
				break;
			} else if (diff <= (int64_t)(pcheckopt.min_diff_nsec)) {
				printf("!!!!diff too small:now=%"PRIu64
					   " previous=%"PRIu64" diff=%"PRIi64"\n",
					   pts, prev_pts, diff);
				break;
			}
		}
		prev_pts = pts;

		counter++;
		if (counter % (pcheckopt.nsamples/10) == 0) {
			printf("Test run %"PRIu64"\%% OK\n",
				   (counter*100)/pcheckopt.nsamples);
		}
	}
	last_pts = ptpclock_gettime_ns(ptp_clkid);
	last_sts = sysclock_gettime_ns();

	if (counter != pcheckopt.nsamples) {
		result = false;
	}

	printf("End reading: %"PRIu64" samples ptpts_now=%"PRIu64"\n", counter, pts);

	/* check ppm */
	offset_pts = last_pts - first_pts;
	offset_sts = last_sts - first_sts;
	offset_diff = (int64_t)(offset_pts - offset_sts);
	ppm = (offset_diff*(int64_t)1000000)/(int64_t)offset_sts;

	printf("Offset to sysclock=%"PRIi64"nsec (%"PRIi64"ppm) duration=%"PRIu64"nsec\n",
		   offset_diff, ppm, offset_sts);

	if (llabs(ppm) > pcheckopt.max_ppm) {
		printf("ppm go beyond the allowed range!!!!!!\n");
		result = false;
	}

	printf("Test result: %s\n", result ? "OK" : "**FAILED**");

	close(fd);

	return 0;
}
