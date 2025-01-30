/*
*
* Copyright (c) {2015 - 2022} Texas Instruments Incorporated
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

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "itidl_ti.h"

typedef struct
{
  char   fmt[20];
  char   fmtVal[20];
  char   string[64];
  int32_t profileIdx   ;
} sTIDLProfilePrintInfo_t ;

static sTIDLProfilePrintInfo_t gProfilePrintInfo[] =
{
  {"%6s,"   , "%6"  PRIu64 ","  , "Layer"                 ,  -1},
  {"%15s,"  , "%15" PRIu64 ","  , "Layer Cycles"          ,  TIDL_PROFILE_LAYER},
  {"%15s,"  , "%15" PRIu64 ","  , "kernelOnlyCycles"      ,  TIDL_PROFILE_KERNEL_ONLY},
  {"%15s,"  , "%15" PRIu64 ","  , "coreLoopCycles"        ,  TIDL_PROFILE_CORE_LOOP},
  {"%15s,"  , "%15" PRIu64 ","  , "LayerSetupCycles"      ,  TIDL_PROFILE_LAYER_SETUP},
  {"%15s,"  , "%15" PRIu64 ","  , "dmaPipeupCycles"       ,  TIDL_PROFILE_DMA_PIPEUP},
  {"%18s,"  , "%18" PRIu64 ","  , "dmaPipeDownCycles"     ,  TIDL_PROFILE_DMA_PIPEDOWN},
  {"%15s,"  , "%15" PRIu64 ","  , "PrefetchCycles"        ,  TIDL_PROFILE_PREFETCH},
  {"%18s,"  , "%18" PRIu64 ","  , "copyKerCoeffCycles"    ,  TIDL_PROFILE_KERNEL_COPY},
  {"%15s,"  , "%15" PRIu64 ","  , "LayerDeinitCycles"     ,  TIDL_PROFILE_LAYER_DEINIT},
  {"%15s,"  , "%15" PRIu64 ","  , "LastBlockCycles"       ,  TIDL_PROFILE_LAST_BLOCK_CYCLES},
  {"%15s,"  , "%15" PRIu64 ","  , "paddingTrigger"        ,  TIDL_PROFILE_LAYER_PADDING_TRIGGER},
  {"%15s,"  , "%15" PRIu64 ","  , "paddingWait"           ,  TIDL_PROFILE_LAYER_PADDING_WAIT},
  {"%15s,"  , "%15" PRIu64 ","  , "LayerWithoutPad"       ,  TIDL_PROFILE_LAYER_WITHOUT_PADDING},
  {"%15s,"  , "%15" PRIu64 ","  , "LayerHandleCopy"       ,  TIDL_PROFILE_LAYER_SETUP_HANDLE_COPY},
  {"%15s,"  , "%15" PRIu64 ","  , "BackupCycles"          ,  TIDL_PROFILE_BACKUP},
  {"%15s,"  , "%15" PRIu64 ","  , "RestoreCycles"         ,  TIDL_PROFILE_RESTORE},
  {"%15s,"  , "%15" PRIu64 ","  , "Multic7xContextCopyCycles",  TIDL_PROFILE_CONTEXT_CROSSCORE_COPY}
};

int64_t TIDLRT_ReadProfileData(TIDL_LayerMetaData *pMetaData, 
  int32_t profileIdx, int32_t layerNum)
{
  int64_t retVal = (int64_t) layerNum ;
  if(profileIdx != -1){
    retVal = pMetaData[layerNum].profilePoint[profileIdx];
  }
  return retVal;
}

/* Print profiling meta data to console and also log it to a debug performance trace file */
void TIDLRT_LogMetaData(TIDL_outArgs *outArgsPtr, char* baseName)
{
  char traceFileName[512] = {0};
  int32_t numProfilePoints, i, j;
  uint64_t profileVal;
  uint64_t sumOfLayerCycles = 0;

  FILE *fp = NULL;
  /*Add error handling:*/
  strcpy(&traceFileName[0], baseName);
  strcat(&traceFileName[0], "perf.csv");
  fp = fopen(&traceFileName[0],"w");
  if(fp == NULL)
  {
    printf("Error opening performance debug file\n");
  }

  numProfilePoints = (int32_t)(sizeof(gProfilePrintInfo)/sizeof(sTIDLProfilePrintInfo_t));
  for ( j= 0; j < numProfilePoints; j++)
  {
    printf(gProfilePrintInfo[j].fmt, gProfilePrintInfo[j].string);
    fprintf(fp, gProfilePrintInfo[j].fmt, gProfilePrintInfo[j].string);
  }
  printf("\n");
  fprintf(fp, "\n");
  for(i = 0 ; i < outArgsPtr->numLayers ; i++)
  {
    /* Data layers will get assigned execId = -1 by c7x-tidl */
    if (outArgsPtr->metaDataLayer[i].layerExecId != -1)
    {
      for ( j= 0; j < numProfilePoints; j++)
      {
        int32_t profileIdx = gProfilePrintInfo[j].profileIdx ;
        profileVal = (profileIdx == -1) ? (outArgsPtr->metaDataLayer[i].layerExecId) : (outArgsPtr->metaDataLayer[i].profilePoint[profileIdx]);
        printf(gProfilePrintInfo[j].fmtVal, profileVal);
        fprintf(fp, gProfilePrintInfo[j].fmtVal, profileVal);
      }
      printf("\n");
      fprintf(fp, "\n");
    }
    sumOfLayerCycles += 
    TIDLRT_ReadProfileData(outArgsPtr->metaDataLayer, 
        TIDL_PROFILE_LAYER, i) ;
  }
  printf(" Sum of Layer Cycles %lu \n", sumOfLayerCycles);
  fprintf(fp, " Sum of Layer Cycles %lu \n", sumOfLayerCycles);

  if(fp)
  {
    fclose(fp);
  }

  return ;
}
