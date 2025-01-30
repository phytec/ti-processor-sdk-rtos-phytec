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
#include <getopt.h>
#include <errno.h>
#include <tsn_combase/combase.h>
#include <tsn_unibase/unibase_binding.h>
#include "yangs/yang_db_runtime.h"
#include "yangs/yang_modules.h"
#include "yangs/yang_node.h"
#include "ucman.h"
#include "uniconfmon_thread.h"

typedef char semname_t[26]; // strlen("/ucmonsem") + 16 + 1 = 26
typedef struct ucmonopt{
	int intervalms;
	int nummonitem;
	const char **monitems;
	const char *dbname;
	bool semnotice;
	UC_NOTICE_SIG_T **nsems;
	semname_t *semnames;
	int waitdb;
	int64_t waitnum;
	int waitnsize;
	bool hwaction;
	char *writedata;
	char *hints;
	const char *conf;
	bool thread;
	bool mstr;
} ucmonopt_t;

UB_SD_GETMEM_DEF_EXTERN(YANGINIT_GEN_SMEM);

/*
 * this monitor program won't be embedded, and static memory allocation is rarely needed.
 * Just in case, define here to monitor 10 items.
 * UC_NOTICE_SIGNUM is defined in 'uc_notice.h'
 */
#define UNICONFMON_MONITEMS uniconfmon_monitems
UB_SD_GETMEM_DEF(UNICONFMON_MONITEMS, sizeof(char*), UC_NOTICE_SIGNUM);
#define UNICONFMON_SEMNAMES uniconfmon_semnames
UB_SD_GETMEM_DEF(UNICONFMON_SEMNAMES, sizeof(semname_t), UC_NOTICE_SIGNUM);
#define UNICONFMON_STRINGS uniconfmon_strings
UB_SD_GETMEM_DEF(UNICONFMON_STRINGS, 16, 16); // 16*16=256bytes likely enoufh

static int print_usage(char *pname, ucmonopt_t *optd)
{
	char *s;
	s=strrchr(pname,'/');
	if(!s++){s=pname;}
	UCMON_CONSOLE_PRINT("%s [options] [write_data]\n", s);
	UCMON_CONSOLE_PRINT("if write_data exits, the data is written to the first '-n' item\n");
	UCMON_CONSOLE_PRINT("-h|--help: this help\n");
	UCMON_CONSOLE_PRINT("-p|--dbname database_filename, default=%s\n",
			 optd->dbname);
	UCMON_CONSOLE_PRINT("-w|--waitdb wait time for uniconf in msec, default=0\n");
	UCMON_CONSOLE_PRINT("-n|--name monitor_item_name, default=None\n");
	UCMON_CONSOLE_PRINT("    to monitor multiple items, set this option multiple times\n");
	UCMON_CONSOLE_PRINT("-i|--interval read_interval(msec unit), default=0\n");
	UCMON_CONSOLE_PRINT("    set 0 for interactive mode, set -1 for oneshot mode\n");
	UCMON_CONSOLE_PRINT("-s|--semnotice get a notice by a named semaphore from uniconf.\n");
	UCMON_CONSOLE_PRINT("    with this option, the 'interval' is not used.\n");
	UCMON_CONSOLE_PRINT("-u|--waitnum  with '-w' and '-n', "
			 "wait integer item become this number.\n");
	UCMON_CONSOLE_PRINT("-t|--waitnsize 8|16|32|64 with '-u', "
			 "size of 'wait integer', default=8\n");
	UCMON_CONSOLE_PRINT("    no this option and '-w','-n', "
			 "wait appearance of the item in the DB\n");
	UCMON_CONSOLE_PRINT("-a|--hwaction:ask an action to uniconf\n");
	UCMON_CONSOLE_PRINT("--hints:use this hints to print value\n");
	UCMON_CONSOLE_PRINT("-c|--conf config_file:read a config file\n");
	UCMON_CONSOLE_PRINT("-m|--mstr mulitple strings in one value\n");
	return -1;
}

static int set_options(ucmonopt_t *optd, int argc, char *argv[])
{
	int oc;
	int res=-1;
	struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"dbname", required_argument, 0, 'p'},
		{"name", required_argument, 0, 'n'},
		{"interval", required_argument, 0, 'i'},
		{"semnotice", no_argument, 0, 's'},
		{"waitdb", required_argument, 0, 'w'},
		{"waitnum", required_argument, 0, 'u'},
		{"waitnsize", required_argument, 0, 't'},
		{"hwaction", no_argument, 0, 'a'},
		{"hints", required_argument, 0, 0},
		{"conf", required_argument, 0, 'c'},
		{"mstr", no_argument, 0, 'm'},
		{NULL, 0, 0, 0},
	};
	while((oc=getopt_long(argc, argv, "hi:n:p:sw:u:at:c:m", long_options, NULL))!=-1){
		switch(oc){
		case 'i':
			optd->intervalms=strtol(optarg, NULL, 0);
			break;
		case 'n':
			optd->monitems=
				UB_SD_REGETMEM(UNICONFMON_MONITEMS, optd->monitems,
					      sizeof(char*)*(optd->nummonitem+1));
			optd->nsems=
				(UC_NOTICE_SIG_T**) uc_notice_sig_reallocate(false, optd->nsems,
							 optd->nummonitem+1);
			optd->semnames=
				UB_SD_REGETMEM(UNICONFMON_SEMNAMES, optd->semnames,
					      sizeof(semname_t)*(optd->nummonitem+1));
			sprintf(optd->semnames[optd->nummonitem], "/ucmonsem%16lx",
				ub_rt_gettime64());
			optd->monitems[optd->nummonitem++]=optarg;
			break;
		case 'p':
			if(optarg && optarg[strlen(optarg)-1]=='/'){
				// remove trailing '/'
				optarg[strlen(optarg)-1]=0;
			}
			optd->dbname=optarg;
			break;
		case 's':
			optd->semnotice=true;
			break;
		case 'w':
			optd->waitdb=strtol(optarg, NULL, 0);
			break;
		case 'u':
			optd->waitnum=strtol(optarg, NULL, 0);
			break;
		case 't':
			optd->waitnsize=strtol(optarg, NULL, 0);
			break;
		case 'a':
			optd->hwaction=true;
			break;
		case 0:
			optd->hints=optarg;
			break;
		case 'c':
			optd->conf=optarg;
			break;
		case 'm':
			optd->mstr=true;
			break;
		case 'h':
		default:
			return print_usage(argv[0], optd);
		}
	}
	res=optind;
	if(argc>res){optd->writedata=argv[res];}
	optind=0;
	return res;
}

static char *read_line(int tout_ms)
{
	static char rbuf[256];
	char *pb;
	int rsize;
	rbuf[0]=0;
	UCMON_CONSOLE_READ(rsize, (uint8_t*)rbuf, sizeof(rbuf)-1, tout_ms);
	if(rsize<0){
		UB_LOG(UBL_ERROR,"%s:console read error %s\n", __func__,strerror(errno));
		return NULL;
	}
	pb=strchr(rbuf,'\n');
	if(pb){*pb=0;}
	return rbuf;
}

static void print_value(int vtype, void *value, uint32_t vsize, char *hints,
			bool ucsign, bool mstr)
{
	char *vstr="";
	uint32_t csize=0;
	if(vtype>=0){vstr=yang_value_string(vtype, value, vsize, 0, hints);}
	while(csize<vsize){
		if(ucsign){UCMON_CONSOLE_PRINT("uniconfmon ");}
		if(vstr[0]==0){
			ub_hexdump(true, true, value, vsize, 0);
		}else{
			UCMON_CONSOLE_PRINT("%s", vstr);
			if(ucsign){UCMON_CONSOLE_PRINT("\n");}
		}
		if(!mstr || (vstr[0]==0)){return;}
		csize+=strlen(vstr)+1;
		vstr+=strlen(vstr)+1;
		if(csize>=vsize){return;}
		UCMON_CONSOLE_PRINT("\n");
	}
}

static void check_allitems(ucmonopt_t *optd, uc_dbald *dbald,
			   yang_db_runtime_dataq_t *ydrd)
{
	uint8_t key1[1]={XL4_EXTMOD_RW};
	uint8_t key2[1]={XL4_DATA_RO-1};
	void *key, *value;
	uint32_t ksize, vsize;
	int i, vtype;
	uc_range *range;
	char *prstr;

	for(i=0;i<2;i++){
		range=uc_get_range(dbald, key1, 1, key2, 1);
		if(!range){continue;}
		if(i==0){UCMON_CONSOLE_PRINT("config items\n");}
		else{UCMON_CONSOLE_PRINT("status items\n");}
		while(true){
			if(uc_get_keyvalue_in_range(dbald, range, &key, &ksize,
						    &value, &vsize,
						    UC_DBAL_FORWARD)){break;}
			if(yang_db_runtime_getkeyvkstr(dbald, key, ksize, &prstr)){
				continue;
			}
			vtype=yang_db_runtime_get_vtype(dbald, key);
			UCMON_CONSOLE_PRINT("%s ", prstr);
			print_value(vtype, value, vsize, prstr,
				    false, optd->mstr);
			UB_SD_RELMEM(YANGINIT_GEN_SMEM, prstr);
			UCMON_CONSOLE_PRINT("\n");
		}
		UCMON_CONSOLE_PRINT("----------\n");
		key1[0]+=0x80;
		key2[0]+=0x80;
		uc_get_range_release(dbald, range);
		uc_dbal_releasedb(dbald);
	}
}

static int cmp_kvs(uint8_t *vp, void **kvs, uint8_t *kss)
{
	int i;
	for (i=0;i<UC_MAX_KV_DEPTH;i++){
		if(kss[i]==0){return 0;}
		if(kvs[i]==NULL){
			vp=&vp[vp[0]+1];
			continue;
		}
		if(kss[i]!=vp[0]){return 1;}
		if(memcmp(&vp[1], kvs[i], kss[i])!=0){return 1;}
		vp=&vp[vp[0]+1];
	}
	return -1;
}

static void check_rangeitems(ucmonopt_t *optd, uc_dbald *dbald,
			     yang_db_runtime_dataq_t *ydrd)
{
	void *key;
	uint32_t ksize;
	void *value=NULL;
	uint32_t vsize=0;
	void *kvs[UC_MAX_KV_DEPTH+1];
	uint8_t kss[UC_MAX_KV_DEPTH];
	uint8_t akey[UC_MAX_AP_DEPTH];
	uc_range *range;
	uint32_t i;
	char *prstr;
	int vtype;
	int j;

	for(j=0;j<2;j++){
		kvs[0]=NULL;
		range=yang_db_runtime_range_fromline(ydrd, optd->monitems[0], kvs, kss, (j==1));
		if(range==NULL){continue;}
		for(i=0;i<UC_MAX_KV_DEPTH;i++){
			if(kss[i]==0){break;}
		}
		if(j==0){UCMON_CONSOLE_PRINT("config items\n");}
		else{UCMON_CONSOLE_PRINT("status items\n");}
		while(true){
			if(uc_get_keyvalue_in_range(dbald, range, &key, &ksize,
						    &value, &vsize,
						    UC_DBAL_FORWARD)){break;}
			if(kss[0]!=0){
				for(i=0;i<ksize;i++){
					if(((uint8_t *)key)[i]==255){break;}
				}
				if(cmp_kvs(&((uint8_t *)key)[i+1], kvs, kss)){
					continue;
				}
			}
			if(ksize>2 && ((uint8_t*)key)[0]==0 && ((uint8_t*)key)[1]==0){
				// *key is immutable
				// value type, valuekey type case, it is not terminated with 255
				for(i=ksize-1;i>=2;i--){
					// if '0':valuekey is there, it should be skipped
					if(((uint8_t*)key)[i]==0){
						break;
					}
				}
				if(i>=2){continue;} // skip valuekey
				memcpy(akey, &((uint8_t*)key)[2], ksize-2);
				akey[ksize-2]=255;
				ksize-=1;
				key=akey;
				value=NULL;
			}
			if(yang_db_runtime_getkeyvkstr(dbald, key, ksize, &prstr)){
				continue;
			}

			vtype=yang_db_runtime_get_vtype(dbald, key);
			UCMON_CONSOLE_PRINT("%s ", prstr);
			if(value!=NULL){print_value(vtype, value, vsize, prstr,
						    false, optd->mstr);}
			UB_SD_RELMEM(YANGINIT_GEN_SMEM, prstr);
			UCMON_CONSOLE_PRINT("\n");
		}
		UCMON_CONSOLE_PRINT("----------\n");
		uc_get_range_release(dbald, range);
		uc_dbal_releasedb(dbald);
	}
}

static void check_monitems(ucmonopt_t *optd, uc_dbald *dbald, uc_notice_data_t *ucntd,
			   yang_db_runtime_dataq_t *ydrd)
{
	int i;
	int res;
	void *value=NULL;
	uint32_t vsize=0;
	char *kstr;

	if(optd->nummonitem==0){return check_allitems(optd, dbald, ydrd);}
	if(optd->monitems[0][strlen(optd->monitems[0])-1]=='/'){
		return check_rangeitems(optd, dbald, ydrd);
	}
	for(i=0;i<optd->nummonitem;i++){
		if(optd->writedata && i==0){
			kstr=UB_SD_GETMEM(UNICONFMON_STRINGS, strlen(optd->monitems[i])+1);
			if(!kstr){continue;}
			strcpy(kstr, optd->monitems[i]);
			(void)yang_db_runtime_put_oneline(ydrd, kstr,
							  optd->writedata,
							  YANG_DB_ONHW_NOACTION);
			if(optd->hwaction){
				(void)yang_db_runtime_askaction(ydrd, ucntd);
			}
			UB_SD_RELMEM(UNICONFMON_STRINGS, kstr);
		}
		if(optd->monitems[i][strlen(optd->monitems[i])-1]=='-'){
			char *dstr;
			dstr=strdup(optd->monitems[i]);
			res=yang_db_runtime_put_oneline(ydrd, dstr, NULL,
							YANG_DB_ONHW_NOACTION);
			free(dstr);
		}else{
			res=yang_db_runtime_get_oneline(ydrd, optd->monitems[i], &value,
							&vsize);
		}
		if(res<0){
			UCMON_CONSOLE_PRINT("no such item:%s\n", optd->monitems[i]);
		}else if(value){
			print_value(res, value, vsize, optd->hints,
				    true, optd->mstr);
		}
	}
	return;
}

static int register_notice(ucmonopt_t *optd, uc_dbald *dbald, uc_notice_data_t *ucntd,
			   yang_db_runtime_dataq_t *ydrd)
{
	int i;
	for(i=0;i<optd->nummonitem;i++){
		if(yang_db_runtime_notice_register(ydrd, ucntd, optd->monitems[i],
						   optd->semnames[i], &optd->nsems[i])){
			return -1;
		}
	}
	return 0;
}

static int deregister_notice(ucmonopt_t *optd, uc_dbald *dbald, uc_notice_data_t *ucntd)
{
	int i;
	for(i=0;i<optd->nummonitem;i++){
		if(uc_nc_notice_deregister_all(ucntd, dbald, optd->semnames[i])){
			return -1;
		}
	}
	return 0;
}

static int check_notice(ucmonopt_t *optd, uc_dbald *dbald, uc_notice_data_t *ucntd)
{
	int i;
	int res;
	int ni=-1;
	uint8_t key[UC_MAX_KEYSIZE];
	void *value=NULL;
	uint32_t ksize, vsize;
	uint8_t vtype;
	for(i=0;i<optd->nummonitem;i++){
		if(!uc_notice_sig_check(false, optd->nsems[i], 0, __func__)){
			ni=i;
			break;
		}
	}
	if(ni<0){return 0;}
	// received a notice
	UB_LOG(UBL_DEBUG, "%s:a noitce from %s\n", __func__, optd->monitems[ni]);
	// get a key of signaled data
	res=uc_nc_get_notice_act(ucntd, dbald, optd->semnames[ni], key, &ksize);
	if(res){
		UB_LOG(UBL_ERROR, "%s:get a signal, but no data in uc_nc_get_notice_act, "
		       "res=%d\n", __func__, res);
		return 0;
	}
	// get value type
	if(ksize+2>UC_MAX_KEYSIZE){return -1;}
	memmove(&key[2], key, ksize);
	key[0]=XL4_DATA_RW;
	key[1]=YANG_VALUE_TYPES;
	for(i=0;i<(int)ksize+2;i++){
		if(key[i]==255){break;}
	}
	res=uc_dbal_get(dbald, key, i, &value, &vsize);
	if(res){
		UB_LOG(UBL_ERROR, "%s:failed to get vtype, res=%d\n", __func__, res);
		goto erexit;
	}
	vtype=*((uint8_t*)value);
	uc_dbal_get_release(dbald, key, ksize+2, value, vsize);
	// get a value of the key
	res=uc_dbal_get(dbald, &key[2], ksize, &value, &vsize);
	if(res){
		UB_LOG(UBL_ERROR, "%s:failed in uc_dbal_get, res=%d\n", __func__, res);
		goto erexit;
	}
	print_value(vtype, value, vsize, NULL,
		    true, optd->mstr);
	uc_dbal_get_release(dbald, key, ksize, value, vsize);
	res=0;
erexit:
	return res;
}

// return 1: no wait item, 0: got the wait item, -1: can't get the wait item
static int main_loop(ucmonopt_t *optd)
{
	uc_dbald *dbald;
	uc_notice_data_t *ucntd=NULL;
	yang_db_runtime_dataq_t *ydrd=NULL;
	int res=-1;
	char *line;
	const char *convf;
	uint8_t callmode;

	callmode=optd->thread?UC_CALLMODE_THREAD:0;
	if(uniconf_ready(optd->dbname, callmode, optd->waitdb)){
		return -1;
	}
	if((optd->intervalms<0) && (optd->nummonitem==1) &&
	   (strcmp(optd->monitems[0],"/")==0)){
		// this checks if uniconf is running
		return 0;
	}
	dbald=uc_dbal_open(optd->dbname, "w", callmode);
	if(!dbald){
		UB_LOG(UBL_ERROR, "%s:can't open the db:%s\n", __func__, optd->dbname);
		return -1;
	}
	ydrd=yang_db_runtime_init(dbald, NULL);
	if(!ydrd){goto erexit;}
	if((optd->intervalms<0) && (optd->nummonitem==1) && (optd->waitdb>0)){
		if(optd->waitnum!=INT64_MAX){
			res=yang_db_runtime_waititem(ydrd, optd->monitems[0],
						     &optd->waitnum,
						     optd->waitnsize/8, optd->waitdb);
			goto erexit;
		}
		res=yang_db_runtime_waititem(ydrd, optd->monitems[0], NULL, 0, optd->waitdb);
		goto erexit;
	}

	if(optd->semnotice || optd->hwaction){
		ucntd=uc_notice_init(callmode, optd->dbname);
		if(!ucntd){goto erexit;}
		if(optd->semnotice){
			if(register_notice(optd, dbald, ucntd, ydrd)){goto erexit;}
			optd->intervalms=10;
		}
	}

	if(optd->conf){
		if(strstr(optd->conf,".xml")!=NULL){
			convf=convxml2conf_getconf(ydrd, optd->conf, NULL);
			if(convf!=NULL){
				res=yang_db_runtime_readfile(ydrd, convf, ucntd);
			}
		}else{
			res=yang_db_runtime_readfile(ydrd, optd->conf, ucntd);
		}
		goto erexit;
	}

	while(true){
		if(optd->semnotice){
			if(check_notice(optd, dbald, ucntd)){break;}
		}else{
			check_monitems(optd, dbald, ucntd, ydrd);
		}
		uc_dbal_releasedb(dbald);
		if(optd->intervalms<0){break;}
		line=read_line(optd->intervalms);
		if(!line){goto erexit;}
		if(!strcmp(line, "q")){break;}
		if(!optd->semnotice && *line){
			optd->nummonitem=1;
			optd->monitems[0]=line;
			continue;
		}
		ub_log_flush();
	}
	res=0;
erexit:
	if(ucntd) {
		deregister_notice(optd, dbald, ucntd);
		uc_notice_close(ucntd, callmode);
	}
	if(ydrd){yang_db_runtime_close(ydrd);}
	uc_dbal_close(dbald, callmode);
	return res;
}

#ifndef UCMON_UDP_THREAD
#define UCMON_MAIN main
#endif
int UCMON_MAIN(int argc, char *argv[])
{
	int res=-1;
	ucmonopt_t optd;
	unibase_init_para_t init_para;

	memset(&optd, 0, sizeof(optd));
	optd.waitnum=INT64_MAX;
	optd.waitnsize=8;
	optd.monitems=UB_SD_GETMEM(UNICONFMON_MONITEMS, sizeof(char*));
	optd.dbname="uniconfdb";
	optd.intervalms=-1; // one shot mode is more commonly used
	// in thread mode, this is called with argv[0]="uniconfmot"
	if(strcmp(argv[0], "uniconfmot")==0){optd.thread=true;}
	if(!optd.thread){
		ubb_default_initpara(&init_para);
		init_para.ub_log_initstr=UBL_OVERRIDE_ISTR("4,ubase:45,cbase:45,uconf:46",
							   "UBL_UNICONF");
		unibase_init(&init_para);
		ubb_memory_out_init(NULL, 0);
	}
	if(set_options(&optd, argc, argv)<0) goto erexit;
	res=main_loop(&optd);
erexit:
	if(optd.monitems){UB_SD_RELMEM(UNICONFMON_MONITEMS, optd.monitems);}
	if(optd.semnames){UB_SD_RELMEM(UNICONFMON_SEMNAMES, optd.semnames);}
	if(optd.nsems){uc_notice_sig_reallocate(false, optd.nsems, 0);}
	if(!optd.thread){
		unibase_close();
	}
	return res;
}
