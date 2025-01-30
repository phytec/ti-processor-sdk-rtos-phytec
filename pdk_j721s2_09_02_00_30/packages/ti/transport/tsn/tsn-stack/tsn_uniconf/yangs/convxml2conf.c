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
#include <tsn_combase/combase.h>
#include <tsn_unibase/unibase.h>
#include "yang_db_runtime.h"

#define OUTPUTFILENAME "/tmp/uniconfmon_tmp.conf"
#define UNICONFMON_READFILE uniconfmon_readfile
#define RBUF_SIZE 512
UB_SD_GETMEM_DEF(UNICONFMON_READFILE, 128, 14);
#define MAX_TAGNAME_SIZE 48
typedef struct xml_data{
	char *pbuf;
	int rsp;
	int sp;
	int ep;
	char *btag;
	char *etag;
	char *betag;
	int xlevel;
	int xwlevel;
	int ptag_xlevel;
	char ptag[MAX_TAGNAME_SIZE];
	char petag[MAX_TAGNAME_SIZE];
	char *vstr;
	char *attrp;
	int levelkn;
	char skipchar;
}xml_data_t;

static const char *outputfilename=OUTPUTFILENAME;
UB_SD_GETMEM_DEF_EXTERN(YANGINIT_GEN_SMEM);

static int read_next(void *inf, xml_data_t *xmd, char *rbuf[])
{
	int rsize;
	rsize=ub_fioread(inf, rbuf[1], RBUF_SIZE);
	if(rsize<0){
		UB_LOG(UBL_ERROR, "%s:read error\n", __func__);
		return -1;
	}
	if(rsize==0){return -1;}
	rbuf[1][rsize]=0;
	UB_LOG(UBL_DEBUG, "%s:rsize=%d, rsp=%d, ep=%d\n", __func__, rsize, xmd->rsp, xmd->ep);
	memmove(rbuf[0], &rbuf[0][xmd->rsp], xmd->ep-xmd->rsp);
	xmd->ep-=xmd->rsp;
	if(xmd->ep>=RBUF_SIZE){
		UB_LOG(UBL_ERROR, "%s:not enough consumed, rsp=%d, ep=%d\n",
		       __func__, xmd->rsp, xmd->ep);
		return -1;
	}
	memcpy(&rbuf[0][xmd->ep], rbuf[1], rsize);
	xmd->ep+=rsize;
	rbuf[0][xmd->ep]=0;
	xmd->btag=((xmd->btag-xmd->pbuf) >= xmd->rsp)?xmd->btag-xmd->rsp:NULL;
	xmd->etag=((xmd->etag-xmd->pbuf) >= xmd->rsp)?xmd->etag-xmd->rsp:NULL;
	xmd->vstr=((xmd->vstr-xmd->pbuf) >= xmd->rsp)?xmd->vstr-xmd->rsp:NULL;
	xmd->sp-=xmd->rsp;
	xmd->rsp=0;
	return 0;
}

/*
 * <Abcd [x=Xyz]>Nvalue...: btag:A, etag,betag:NULL, vstr:N, attrp=X
 * </Abcd>: tag:NULL, etag:A, btag,betag:NULL,  vstr:N(previously set), attrp:X(previously set)
 * <Abcd [x=Xyz]/>: betag:A, btag,etag:NULL, vstr:NULL, attrp:NULL, attrp=X
 * xmd->xlevel +1, with btag or betag. -1 is needed out of this function
 */
static int xml_one_tag(xml_data_t *xmd)
{
	char *bp;
	int nsb, esb;
	if(xmd->skipchar==0){
		bp=strchr(&xmd->pbuf[xmd->sp], '<');
		if(bp==NULL){return 1;} // no '<', need to read data
		if(bp[1]=='?' || (bp[1]=='!' && bp[2]=='-')){
			// '<?' or '<!'
			xmd->skipchar=bp[1];
		}
		nsb=bp-xmd->pbuf;
	}else{
		nsb=xmd->sp;
	}
	bp=strchr(&xmd->pbuf[nsb], '>');
	if(bp==NULL){
		if(xmd->skipchar!=0){
			xmd->sp+=strlen(&xmd->pbuf[xmd->sp])+1;
			xmd->rsp=xmd->sp;
			return 1;
		}
		return -1; // invalid data
	}
	bp[0]=0;
	esb=bp-xmd->pbuf;
	xmd->sp=esb+1;// next sp
	UB_LOG(UBL_DEBUG, "%s:rsp: %d -> %d\n", __func__, xmd->rsp, xmd->sp);
	xmd->rsp=xmd->sp;
	if(xmd->pbuf[xmd->sp]==0){xmd->sp++;}
	if(xmd->skipchar!=0){
		if(xmd->skipchar=='?' && xmd->pbuf[esb-1]=='?'){
			xmd->skipchar=0;
		}
		if(xmd->skipchar=='!' && xmd->pbuf[esb-1]=='-'){
			xmd->skipchar=0;
		}
		return 1;
	}
	if(xmd->pbuf[nsb+1]=='/'){
		if(xmd->xlevel==0){
			return 0;
		}
		xmd->pbuf[nsb]=0;
		xmd->btag=NULL;
		xmd->etag=&xmd->pbuf[nsb+2];
		xmd->betag=NULL;
	}else{
		xmd->xlevel++;
		xmd->btag=&xmd->pbuf[nsb+1];
		xmd->etag=NULL;
		xmd->betag=NULL;
		xmd->attrp=NULL;
		xmd->vstr=NULL;
	}
	if(xmd->btag){
		// check inside the btag
		bp=strchr(xmd->btag, ' ');
		if(bp!=NULL){
			bp[0]=0;
			bp=&bp[1];
			xmd->attrp=bp;
		}else{
			bp=xmd->btag;
		}
		xmd->vstr=&bp[strlen(bp)+1];// point of 'V' in  <abc>V...
		if(xmd->pbuf[esb-1]=='/'){
			// this is betag like <abc/>
			xmd->pbuf[esb-1]=0;
			xmd->betag=xmd->btag;
			xmd->btag=NULL;
			xmd->vstr=NULL;
		}
	}
	return 2; // go next step
}

static int xml_get_namespace(yang_db_runtime_dataq_t *ydrd,
			     xml_data_t *xmd)
{
	char *bp, *cp;
	bp=xmd->attrp;
	if(bp==NULL){return 1;} // go to next line
	bp=strstr(bp, "xmlns");
	if(bp==NULL){return 1;} // go to next line
	if(strstr(bp, "http://")!=NULL){
		bp=strrchr(bp, '/');
		if(bp==NULL){return -1;}
	}else{
		bp=strrchr(bp, ':');
		if(bp==NULL){return -1;}
	}
	bp=&bp[1];
	cp=strchr(bp, '"');
	if(cp==NULL){return -1;}
	cp[0]=0;
	// got name space in bp
	if(xmd->btag==NULL ||
	   strstr(xmd->btag, "edit-config")!=NULL ||
	   strstr(xmd->btag, "data")!=NULL){
		return -1;
	}
	if(yang_db_runtime_proc_nodestring(ydrd, true, bp, NULL)!=0){
		return -1;
	}
	if(yang_db_runtime_proc_nodestring(ydrd, false, xmd->btag, NULL)!=0){
		return -1;
	}
	UB_LOG(UBL_DEBUG, "%s:/%s/%s\n", __func__, bp, xmd->btag);
	return 0;
}

static int push_one_ptag(yang_db_runtime_dataq_t *ydrd, xml_data_t *xmd)
{
	int res;
	res=yang_db_runtime_proc_nodestring(ydrd, false, xmd->ptag, NULL);
	if(res<0){
		UB_LOG(UBL_ERROR, "%s:%s, res=%d\n", __func__, xmd->ptag, res);
		return res;
	}
	// res is added key number
	xmd->levelkn+=res;
	xmd->ptag[0]=0;
	return 0;
}

static int xml_get_level(char *p)
{
	int uplevel=0;
	int d=0;
	char *sp=strchr(p, ' ');
	while(p[0]!=0){
		if(strstr(p, "..")==p){
			d=-1;
		}else{
			d=0;
		}
		uplevel+=d;
		p=strchr(p, '/');
		if(p==NULL){break;}
		if((sp!=NULL) && (p>=sp)){break;} // don't search '/' after a space
		if(d==0){uplevel++;}
		p++;
	}
	return uplevel;
}

// return a next char of n-th '/'
static char *xml_get_nlevel(char *p, int n)
{
	while(*p!=0){
		p=strchr(p, '/');
		if(p==NULL){return NULL;}
		p++;
		if(--n<=0){return p;}
	}
	return NULL;
}

// return 0:normal, 1:lastline, 2:eof got from kstr, 3:eof, 4:no data line, -1:error
#define LBUF_SIZE RBUF_SIZE
static int xml_tempfile_nextline(void *inf, char *lbuf, char *kstr)
{
	int i;
	int count=0;
	if(inf==NULL){
		lbuf[0]=0;
		strcpy(lbuf, kstr);
		return 2;
	}
	for(i=0;i<LBUF_SIZE;i++){
		if((ub_fioread(inf, &lbuf[i], 1)!=1)){
			// got eof
			lbuf[i]=0;
			if(i==0){
				strcpy(lbuf, kstr);
				return 2;
			}
			if(count==0){return 3;}
			return 1;
		}
		if(lbuf[i]=='\n'){
			lbuf[i]=0;
			if(count==0){return 4;}
			return 0;
		}
		if(lbuf[i]!=' '){count++;}
	}
	UB_LOG(UBL_ERROR, "%s:too long line\n", __func__);
	return -1; // too long line
}

static int xml_tempfile_mod(void *inf, void *outf, xml_data_t *xmd, char *kstr)
{
	int dl, vbc;
	int cxlevel=0;
	int nxlevel=xmd->xlevel;
	char lbuf[LBUF_SIZE];
	char *p, *q;
	bool found=false;
	bool stop=false;
	int res;
	UB_LOG(UBL_DEBUG, "%s: nxlevel=%d, kstr=%s, etag=%s, vstr=%s\n",
	       __func__, nxlevel, kstr, xmd->etag, xmd->vstr);
	while(!stop){
		res=xml_tempfile_nextline(inf, lbuf, kstr);
		if(res==-1){return -1;}
		if(res==3){break;}
		if(res==4){continue;}
		if(res==2){
			if(found){break;}
			// the lbuf is copied from kstr, need to adjust level
			stop=true;
			p=xml_get_nlevel(lbuf, cxlevel);
			if(p==NULL){break;}
			if(cxlevel==0){p--;}
			memmove(lbuf, p, strlen(p)+1);
			cxlevel=nxlevel;
			xmd->xwlevel=cxlevel;
		}
		if(res==0){
			cxlevel+=xml_get_level(lbuf);
			if((cxlevel<nxlevel) || found){
				ub_fiowrite(outf, lbuf, strlen(lbuf));
				ub_fiowrite(outf, "\n", 1);
				continue;
			}
		}
		dl=cxlevel-nxlevel; // need this number of '/' from bottom
		vbc=0; // count number of '|' from bottom
		res=-1;
		// start from the bottom of lbuf string
		for(p=lbuf+strlen(lbuf)-1;p>=lbuf;p--){
			if(dl!=0){
				if(*p=='/'){dl--;}
				if(dl>0){continue;}
			}
			// dl==0
			if(*p=='/'){p--;}
			for(q=p;q>=lbuf;q--){
				if(*q=='|'){
					vbc++;
					if(vbc>1){break;}
				}
				if(*q=='/'){break;}
			}
			if(vbc==0){
				res=0;
				break;
			}
			if((q<lbuf)||(*q=='/')){
				res=0;
				break; // not found, res=-1
			}
			p=q;
			if(vbc<=1){continue;} // check from 2nd '|'
			if(strstr(p+1, xmd->etag)==p+1){
				// found
				q=p+1+strlen(xmd->etag)+1;// next ':'
				if(q[0]!='|'){
					// already written, search next
					res=0;
					break;
				}
				ub_fiowrite(outf, lbuf, q-lbuf);
				ub_fiowrite(outf, xmd->vstr, strlen(xmd->vstr));
				ub_fiowrite(outf, q, strlen(q));
				ub_fiowrite(outf, "\n", 1);
				found=true;
				res=0;
				break;
			}
		}
		if((res!=0) && (dl==0)){
			UB_LOG(UBL_ERROR, "%s:cxlevel=%d, nxlevel=%d, not found kvname, %s, %s\n",
			       __func__, cxlevel, nxlevel, xmd->etag, kstr);
			return -1;
		}
		if(!found){
			ub_fiowrite(outf, lbuf, strlen(lbuf));
			ub_fiowrite(outf, "\n", 1);
		}
	}
	if(found){return 0;}
	return -1;
}

static void debug_file_print() __attribute__((unused));
static void debug_file_print()
{
	char c;
	void *outf;
	printf("=====================\n");
	outf=ub_fioopen(outputfilename, "r");
	while(ub_fioread(outf, &c, 1)==1){
		printf("%c", c);
	}
	printf("\n");
	printf("=====================\n");
	ub_fioclose(outf);
}

static int xml_tempfile_adjust_level(void *outf, xml_data_t *xmd, char *kstr)
{
	int i;
	char *p;
	if(xmd->xwlevel>xmd->xlevel){
		for(i=0;i<(xmd->xwlevel-xmd->xlevel);i++){
			ub_fiowrite(outf, "../", 3);
		}
	}else if(xmd->xwlevel<xmd->xlevel){
		// write new added level,
		// e.g. /a/b/c/Na/Nb/Nc/ -> write "Na/Nb/Nc/" as a new part
		i=0;
		for(p=kstr+strlen(kstr)-1;p>=xmd->pbuf;p--){
			if(*p=='/'){
				if(i==(xmd->xlevel - xmd->xwlevel)){
					if(xmd->xwlevel>0){p++;}
					break;
				}
				i++;
			}
		}
		if(p<xmd->pbuf){return -1;}
		ub_fiowrite(outf, p, strlen(p));
	}
	return 0;
}

static int xml_tempfile_append(xml_data_t *xmd, char *kstr, bool vkmode, char *suffix)
{
	//xmd->etag, xmd->vstr, xmd->xlevel(new level)
	void *outf, *inf=NULL;
	int res=-1;
	char *tempfname;

	if(strcmp(&kstr[strlen(kstr)-8], "valuekey")!=0){return -1;}
	kstr[strlen(kstr)-8]=0;

	if(!vkmode){
		outf=ub_fioopen(outputfilename, "a");
	}else{
		char c;
		inf=ub_fioopen(outputfilename, "r");
		if(inf){
			tempfname=UB_SD_GETMEM(UNICONFMON_READFILE, strlen(outputfilename)+2);
			if(ub_assert_fatal(tempfname!=NULL, __func__, NULL)){
				return -1;
			}
			strcpy(tempfname, outputfilename);
			tempfname[strlen(outputfilename)]='~';
			tempfname[strlen(outputfilename)+1]=0;
			outf=ub_fioopen(tempfname, "w");
			if(outf==NULL){
				UB_LOG(UBL_ERROR, "%s:can't open tempfile:%s\n",
				       __func__, tempfname);
				UB_SD_RELMEM(UNICONFMON_READFILE, tempfname);
				return -1;
			}
			while(ub_fioread(inf, &c, 1)==1){ub_fiowrite(outf, &c, 1);}
			ub_fioclose(outf);
			ub_fioclose(inf);
			inf=ub_fioopen(tempfname, "r");
			UB_SD_RELMEM(UNICONFMON_READFILE, tempfname);
		}
		outf=ub_fioopen(outputfilename, "w");
	}
	if(outf==NULL){
		UB_LOG(UBL_ERROR, "%s:can't open file=%s\n", __func__, outputfilename);
		return -1;
	}
	if(vkmode){
		res=xml_tempfile_mod(inf, outf, xmd, kstr);
		goto erexit;
	}
	if(xml_tempfile_adjust_level(outf, xmd, kstr)){return -1;}
	ub_fiowrite(outf, xmd->etag, strlen(xmd->etag));
	if(suffix!=NULL){ub_fiowrite(outf, suffix, strlen(suffix));}
	ub_fiowrite(outf, " ", 1);
	ub_fiowrite(outf, xmd->vstr, strlen(xmd->vstr));
	ub_fiowrite(outf, "\n", 1);
	xmd->xwlevel=xmd->xlevel;
	res=0;
erexit:
	ub_fioclose(outf);
	//debug_file_print();
	if(inf){ub_fioclose(inf);}
	return res;
}

static int xml_tempfile_del_append(xml_data_t *xmd, char *kstr)
{
	void *outf;
	int res;
	outf=ub_fioopen(outputfilename, "a");
	if(outf==NULL){return -1;}
	res=xml_tempfile_adjust_level(outf, xmd, kstr);
	if(res==0){
		ub_fiowrite(outf, xmd->etag, strlen(xmd->etag));
		ub_fiowrite(outf, "-\n", 2);
		xmd->xwlevel=xmd->xlevel;
	}
	ub_fioclose(outf);
	return res;
}

static int xml_tempfile_movenode(char *tag, int n)
{
	int i;
	void *outf;
	outf=ub_fioopen(outputfilename, "a");
	if(outf==NULL){return -1;}
	for(i=0;i<n;i++){
		ub_fiowrite(outf, "../", 3);
	}
	//ub_fiowrite(outf, tag, strlen(tag));
	ub_fiowrite(outf, "\n", 1);
	ub_fioclose(outf);
	return 0;
}

static int proc_ptag(yang_db_runtime_dataq_t *ydrd, xml_data_t *xmd, char *tag)
{
	if(xmd->ptag[0]==0){return 0;}
	if(tag && strcmp(xmd->ptag, tag)==0){return 0;}
	if(xmd->ptag_xlevel<xmd->xwlevel){
		xml_tempfile_movenode(xmd->ptag, xmd->xwlevel-xmd->ptag_xlevel);
		xmd->xwlevel=xmd->ptag_xlevel;
	}
	return push_one_ptag(ydrd, xmd);
}

const char *convxml2conf_getconf(yang_db_runtime_dataq_t *ydrd, const char *ifname,
				 const char *ofname)
{
	void *inf;
	char *rbuf[2];
	int res=-1;
	bool started=false;
	xml_data_t xmd;
	int i;
	char *rstr;
	bool vkmode;
	char *suffix=NULL;
	memset(&xmd, 0, sizeof(xmd));
	rbuf[0]=UB_SD_GETMEM(UNICONFMON_READFILE, RBUF_SIZE*2);
	rbuf[1]=UB_SD_GETMEM(UNICONFMON_READFILE, RBUF_SIZE);
	if(ub_assert_fatal((rbuf[0]!=NULL && rbuf[1]!=NULL), __func__, NULL)){
		return NULL;
	}
	xmd.pbuf=rbuf[0];
	if(ofname!=NULL){
		outputfilename=ofname;
	}
	inf=ub_fioopen(outputfilename, "w");
	ub_fioclose(inf); // truncate an existing tempfile
	inf=ub_fioopen(ifname, "r");
	if(inf==NULL){
		UB_LOG(UBL_ERROR, "%s:can't open file=%s\n", __func__, ifname);
		return NULL;
	}
	xmd.pbuf[0]=0;
	while(true){
		res=-1;
		for(i=xmd.sp;i<xmd.ep;i++){
			if(xmd.pbuf[i]=='\n'){
				if(i<((int)strlen(&xmd.pbuf[xmd.sp])+xmd.sp)){
					xmd.pbuf[i]=0;
				}
				break;
			}
		}
		if(i==xmd.ep){
			// no end of line, need to read data
			if(read_next(inf, &xmd, rbuf)){break;}
			continue;
		}
		xmd.pbuf[xmd.ep]=0;
		res=xml_one_tag(&xmd);
		if(res<=0){break;}
		if(res==1){continue;}
		if(!started){
			xmd.xlevel=1;
			res=xml_get_namespace(ydrd, &xmd);
			if(res==0){started=true;}
			continue;
		}
		if((xmd.etag || xmd.betag) && xmd.attrp &&
		   (strstr(xmd.attrp, "delete")!=NULL)){
			if(xmd.etag==NULL){xmd.etag=xmd.betag;}
			xmd.attrp=NULL;
			res=yang_db_runtime_proc_nodestring(ydrd, false, xmd.etag, NULL);
			if(res==-3){break;}
			if(yang_db_runtime_state_keyvkstr(ydrd, &rstr)==0){
				xml_tempfile_del_append(&xmd, rstr);
				UB_SD_RELMEM(YANGINIT_GEN_SMEM, rstr);
			}
			xmd.xlevel--;
			yang_db_runtime_proc_nodestring(ydrd, false, "..", NULL);
			continue;
		}
		if(xmd.etag && xmd.vstr){
			UB_LOG(UBL_DEBUG, "etag with vstr, xlevel=%d, etag=%s, vstr=%s\n",
			       xmd.xlevel, xmd.etag, xmd.vstr);
			if(proc_ptag(ydrd, &xmd, xmd.etag)){break;}
			vkmode=false;
			res=yang_db_runtime_proc_nodestring(
				ydrd, false, xmd.etag, xmd.vstr);
			if(res==-1){
				// added value key
				xmd.levelkn-=1;
				UB_LOG(UBL_DEBUG, "added valuekey %s:%s xlevel=%d, levelkn=%d\n",
				       xmd.etag, xmd.vstr, xmd.xlevel, xmd.levelkn);
				xmd.petag[0]=0;
				xmd.ptag[0]=0;
				vkmode=true;
			}else if(res==-2){
				// added leaf, need to keep value
				if(xmd.levelkn==0){
					UB_LOG(UBL_DEBUG, "%s, push one data, %d\n",
					       xmd.etag, xmd.xlevel);
				}else if(xmd.levelkn>0){
					UB_LOG(UBL_DEBUG, "%s, cash one data, %d\n",
					       xmd.etag, xmd.xlevel);
				}else{
					UB_LOG(UBL_ERROR, "%s:%s, xmd.xlevel=%d\n",
					       __func__, xmd.etag, xmd.xlevel);
					break;
				}
				// petag is used to check repeating the same leaf config
				// it must be leaf-list
				if(strcmp(xmd.petag, xmd.etag)==0){
					suffix="+";
				}else{
					ub_strncpy(xmd.petag, xmd.etag, sizeof(xmd.petag));
				}
				xmd.ptag[0]=0;
			}else{
				UB_LOG(UBL_ERROR, "%s:%s, res=%d\n", __func__, xmd.etag, res);
				break;
			}
			if(yang_db_runtime_state_keyvkstr(ydrd, &rstr)==0){
				xml_tempfile_append(&xmd, rstr, vkmode, suffix);
				UB_SD_RELMEM(YANGINIT_GEN_SMEM, rstr);
			}
			suffix=NULL;
			xmd.xlevel--;
			xmd.vstr=NULL;
			continue;
		}
		if(xmd.etag){
			// no vstr case, just 'one up'
			res=yang_db_runtime_proc_nodestring(ydrd, false, "..", NULL);
			UB_LOG(UBL_DEBUG, "%s, one up, -> %d\n", xmd.etag, xmd.xlevel);
			xmd.xlevel--;
			xmd.petag[0]=0;
			continue;
		}
		if(xmd.btag){
			// get a new tag, 'one down'
			// if ptag exists, process it first.
			// tag is processed at 'etag' time,
			// if 'etag' does't come after this, push it to ptag
			if(proc_ptag(ydrd, &xmd, NULL)){break;}
			ub_strncpy(xmd.ptag, xmd.btag, MAX_TAGNAME_SIZE);
			xmd.ptag_xlevel=xmd.xlevel;
			UB_LOG(UBL_DEBUG, "%s, one down, -> %d\n", xmd.btag, xmd.xlevel);
			continue;
		}
		if(xmd.betag){
			UB_LOG(UBL_DEBUG, "btag, xlevel=%d, betag=%s\n",
			       xmd.xlevel, xmd.betag);
			if(proc_ptag(ydrd, &xmd, NULL)){break;}
			xmd.xlevel--;
			xmd.petag[0]=0;
			continue;
		}
	}
	ub_fioclose(inf);
	UB_SD_RELMEM(UNICONFMON_READFILE, rbuf[0]);
	UB_SD_RELMEM(UNICONFMON_READFILE, rbuf[1]);
	if(res){return NULL;}
	return outputfilename;
}
