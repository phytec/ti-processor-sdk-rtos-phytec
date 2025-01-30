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
#include <signal.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <tsn_combase/combase.h>
#include <tsn_unibase/unibase_binding.h>
#include "yangs/yang_modules.h"
#include "yangs/yang_node.h"
#include "ucman.h"

typedef struct chdbhopt{
	const char *dbname;
	char *dlogfile;
	int intervalms;
	int toutms;
	int dtms;
} chdbhopt_t;

static int print_usage(char *pname, chdbhopt_t *optd)
{
	char *s;
	s=strrchr(pname,'/');
	if(!s++){s=pname;}
	UB_CONSOLE_PRINT("%s [options] [write_data]\n", s);
	UB_CONSOLE_PRINT("-h|--help: this help\n");
	UB_CONSOLE_PRINT("-p|--dbname database_filename, default=%s\n", optd->dbname);
	UB_CONSOLE_PRINT("-i|--interval read_interval(msec unit), default=%d\n",
			 optd->intervalms);
	UB_CONSOLE_PRINT("-t|--toutms tout_ms, print if it is longer than this msec, default=%d\n",
			 optd->toutms);
	UB_CONSOLE_PRINT("-d|--dtms tout_ms, quite after this dime, default=%d\n", optd->dtms);
	UB_CONSOLE_PRINT("-f|--dlogfile filename(to save logmemory) default=%s\n",
			 optd->dlogfile);

	return -1;
}

static int set_options(chdbhopt_t *optd, int argc, char *argv[])
{
	int oc;
	struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"dbname", required_argument, 0, 'p'},
		{"toutms", required_argument, 0, 't'},
		{"intervalms", required_argument, 0, 'i'},
		{"dlogfile", required_argument, 0, 'f'},
		{NULL, 0, 0, 0},
	};
	while((oc=getopt_long(argc, argv, "hp:t:i:f:", long_options, NULL))!=-1){
		switch(oc){
		case 'i':
			optd->intervalms=strtol(optarg, NULL, 0);
			break;
		case 'p':
			optd->dbname=optarg;
			break;
		case 'f':
			optd->dlogfile=optarg;
			break;
		case 't':
			optd->toutms=strtol(optarg, NULL, 0);
			break;
		case 'h':
		default:
			return print_usage(argv[0], optd);
		}
	}
	return 0;
}

static bool stoprun=false;
static void signal_handler(int sig)
{
	stoprun=true;
}

static int check_dbhold(chdbhopt_t *optd)
{
	uc_dbald *dbald;
	uint8_t uc_rap[3]={XL4_DATA_RO, UC_READY, 255};
	void *value;
	uint32_t vsize=0;
	uint64_t ts0, ts1, ts2, tss;
	int tdiff;
	int res=0;
	dbald=uc_dbal_open(optd->dbname, "w", 0);
	if(!dbald){
		UB_LOG(UBL_ERROR, "%s:can't open the db:%s\n", __func__, optd->dbname);
		return -1;
	}
	ts0=ub_rt_gettime64();
	tss=ts0;
	UB_TLOG(UBL_INFO, "Start monitoring\n");
	while(!stoprun){
		ts1=ub_rt_gettime64();
		if(uc_dbal_get(dbald, uc_rap, 3, &value, &vsize)!=0){
			UB_LOG(UBL_ERROR, "%s:uniconf is not ready status\n", __func__);
			res=-1;
			break;
		}
		ts2=ub_rt_gettime64();
		uc_dbal_get_release(dbald, uc_rap, 3, value, vsize);
		uc_dbal_releasedb(dbald);
		tdiff=(ts2-ts1)/UB_MSEC_NS;
		if(tdiff>optd->toutms){
			UB_TLOG(UBL_INFO, "DB holding too long %dmsec\n", tdiff);
		}
		tss+=optd->intervalms*UB_MSEC_NS;
		if(optd->dtms>0 && (int64_t)(tss-ts0)>optd->dtms*UB_MSEC_NS){
			break;
		}
		if(tss>ts2){
			usleep((tss-ts2)/UB_USEC_NS);
		}
	}
	UB_TLOG(UBL_INFO, "Closing\n");
	uc_dbal_close(dbald, 0);
	return res;
}

int main(int argc, char *argv[])
{
	int res=-1;
	chdbhopt_t optd;
	unibase_init_para_t init_para;
	struct sigaction sigact;
	ubb_default_initpara(&init_para);
	init_para.ub_log_initstr=UBL_OVERRIDE_ISTR("4,ubase:45,cbase:45,uconf:46",
						   "UBL_UNICONF");
	unibase_init(&init_para);
	memset(&optd, 0, sizeof(optd));
	optd.toutms=50;
	optd.intervalms=50;
	if(set_options(&optd, argc, argv)<0){goto erexit;}
	if(optd.dbname==NULL){goto erexit;}
	if(optd.dlogfile==NULL){optd.dlogfile="check_dbhold_debugmem.log";}
	if(uniconf_ready(optd.dbname, 0, 500)){goto erexit;}
	sigact.sa_handler=signal_handler;
	sigaction(SIGINT, &sigact, NULL);
	sigaction(SIGTERM, &sigact, NULL);
	res=check_dbhold(&optd);
erexit:
	if(optd.dlogfile && ubb_memory_file_out(optd.dlogfile)){
		UB_LOG(UBL_ERROR, "%s:can't write the memory log into a file\n", __func__);
	}
	ubb_memory_out_close();
	unibase_close();
	return res;
}
