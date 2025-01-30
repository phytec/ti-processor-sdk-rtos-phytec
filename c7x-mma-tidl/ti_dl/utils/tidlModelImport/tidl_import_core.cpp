/*
*
* Copyright (c) {2015 - 2020} Texas Instruments Incorporated
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <fstream>
#include <sstream>

#ifdef _WIN32
#include <asprintf.h>
#else
#include<dirent.h>
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/message.h>
#include <google/protobuf/text_format.h>
using namespace std;
using ::google::protobuf::Message;

#include "ti_dl.h"
#include "tidl_import_api.h"
#include "tidl_import_config.h"
#include "tidl_import_common.h"
#include "tidl_custom_import.h"
#include "tidl_import_common_model_check.h"
#include "perfsim.h"
#include "tidl_deviceInfo.h"
#include "tidl_import_quantize.h"
#include "tidl_import_common_tidlrt_osrt.h"
#include "gc.h"
#include "gc_helper.h"

sTIDL_OrgNetwork_t  orgTIDLNetStructure;
sTIDL_OrgNetwork_t tempTIDLNetStructure;
sTIDL_Network_t tIDLNetStructure;
unordered_set<int32_t> unsupportedLayersForOTF = { TIDL_Deconv2DLayer };
unordered_set<int32_t> padUnsupportedLayers = { TIDL_ReshapeLayer };
char inDataNames[TIDL_MAX_ALG_IN_BUFS][TIDL_MAX_DATA_NAME];
char outDataNames[TIDL_MAX_ALG_OUT_BUFS][TIDL_MAX_DATA_NAME];
char outMetaLayerNames[TIDL_MAX_ALG_OUT_BUFS][TIDL_MAX_DATA_NAME];
int32_t numNetInData = 0;
int32_t numNetOutData = 0;
char inConfigFilename[2*FILE_NAME_SIZE];
tidl_import_config gParams;
sTIDL_multiCoreBatchProcessing multiCoreBatchProcInfo;
extern int32_t numTFMetaLayers;

char* getFileNameFromPath(char* path)
{
  for (int32_t i = strlen(path) - 1; i; i--)
  {
    if ((path[i] == '/') || (path[i] == '\\'))
    {
      return &path[i + 1];
    }
  }
  return path;
}

void getDirFromPath(char* path)
{
  for (int32_t i = strlen(path) - 1; i; i--)
  {
    if ((path[i] == '/') || (path[i] == '\\'))
    {
      path[i] = '\0';
      return;
    }
  }
  path[0] = '\0';
  return;
}
void getDosPath(char* orgpath, char * dosPath)
{
  strcpy(dosPath, orgpath);
  for (int32_t i = 0; i < strlen(dosPath); i++)
  {
    if (dosPath[i] == '/')
    {
      dosPath[i] = '\\';
    }
  }
  return;
}

void getAbsPath(char* path, char * absPath)
{
  char syscmd[500];
  char dosPath[500];
#ifdef _WIN32
  getDosPath(path, dosPath);
  sprintf(syscmd, "dir /b /s %s", dosPath);
#else
  sprintf(syscmd, "readlink -f %s", path);
#endif

#ifdef _WIN32
  FILE * fp = _popen(syscmd,  "r");
#else
  FILE * fp = popen(syscmd,  "r");
#endif

  if (fp == NULL)
  {
    printf("Error while runing command : %s", syscmd);
  }
  fscanf(fp, "%s", absPath);
  fclose(fp);
  return;
}

void setDefaultParams(tidl_import_config * params)
{
  int32_t i;
  params->randParams          = 0;
  params->modelType           = 0; // 0 - caffe, 1- tensorFlow
  params->quantizationStyle   = TIDL_QuantStyleNP2Fixed;
  params->calibrationOption = 0;
  params->activationRangeMethod = TIDL_ActivationRangeMethodHistogram;
  params->weightRangeMethod = TIDL_WeightRangeMethodMedian;
  params->percentileActRangeShrink = 0.01;
  params->percentileWtRangeShrink = 0.01;
  params->biasCalibrationFactor = 0.05;
  params->biasCalibrationIterations = -1;
  params->quantRoundAdd       = 50; // 0 - caffe, 1- tensorFlow
  params->numParamBits        = 8;
  params->inFileFormat        = 2; // 0 - Encoded, 1- RAW
  params->numFrames           = -1;
  params->numFramesBiasCalibration = -1;  // number of frames to be used for bias calibration
  params->foldBnInConv2D      = 1;
  params->foldPreBnConv2D      = 1;
  params->foldEltWiseInConv2D = 0;
  params->foldMaxPoolInConv2D = 0;
  params->foldDepthToSpaceInConv2D = 1;
  params->postProcType        = 0;
  params->postProcDataId      = 0;
  params->numFeatureBits      = 8;
  params->metaArchType        =-1;
  params->debugTraceLevel     = 0;
  params->writeTraceLevel     = 0;
  params->quantRangeUpdateFactor       = -1.0;
  params->compileConstraintsFlag = DEFAULT_COMPILE_CONSTRAINT_NC_FLAGS;
  params->executeNetworkCompiler = 1;
  params->executeQuantsTool   = 1;
  params->enableHighResOptimization = 0;
  params->enableCustomLayers = 0;
  params->msmcSizeKB = -1;
  params->deviceName = -1;
  params->numCores = 1;
  params->inferenceMode = TIDL_inferenceModeDefault;
  params->quantRangeExpansionFactor = 1.0;
  params->addDataConvertToNet = 0;
  params->mixedPrecisionFactor = -1;
  params->modelGroupId = 0 ;
  params->enableOtfPad = 0;
  params->isQuantParamsToBeExported = false;
  params->preQuantizedModel = 0;
  params->enableBiasFusion = 0;

#ifdef _WIN32
  strcpy((char*)params->tidlStatsTool,"..\\..\\test\\PC_dsp_test_dl_algo.out.exe");
  strcpy((char*)params->perfSimTool,  "..\\..\\utils\\perfsim\\ti_cnnperfsim.out.exe");
  strcpy((char*)params->graphVizTool, "..\\..\\utils\\tidlModelGraphviz\\out\\tidl_graphVisualiser.out.exe");
  strcpy((char*)params->modelDumpTool, "..\\..\\utils\\tidlModelDump\\out\\tidl_dump.out.exe");
  strcpy((char*)params->perfSimConfig, "..\\..\\test\\testvecs\\config\\import\\device_config.cfg");
#else
  strcpy((char*)params->tidlStatsTool, "../../test/PC_dsp_test_dl_algo.out");
  strcpy((char*)params->perfSimTool, "../../utils/perfsim/ti_cnnperfsim.out");
  strcpy((char*)params->graphVizTool, "../../utils/tidlModelGraphviz/out/tidl_graphVisualiser.out");
  strcpy((char*)params->modelDumpTool, "../../utils/tidlModelDump/out/tidl_dump.out");
  strcpy((char*)params->perfSimConfig, "../../test/testvecs/config/import/device_config.cfg");
#endif

  strcpy((char*)params->inDataNamesList, "");
  strcpy((char*)params->outDataNamesList, "");
  strcpy((char*)params->singleCoreLayersNames, "");
  strcpy((char*)params->outputFeature16bitNamesList, "");
  strcpy((char*)params->params16bitNamesList, "");
  strcpy((char*)params->fileNameGrpInfo, "");

  for (i = 0; i < TIDL_MAX_ALG_IN_BUFS; i++)
  {
    params->inElementType[i] = TIDL_UnsignedChar;
    params->rawDataInElementType[i] = -1;
    params->inZeroPoint[i] = 0;
    params->inLayout[i] = TIDL_LT_NCHW;
    params->inQuantFactor[i] =  1.0;
    params->inWidth[i]       = -1;
    params->inHeight[i]      = -1;
    params->inNumChannels[i] = -1;
    params->inDIM2[i]      = 1;
    params->inDIM1[i]      = 1;
    params->numBatches[i] = 1;
    params->resizeWidth[i]  = -1;
    params->resizeHeight[i] = -1;
    params->inResizeType[i] = TIDL_inResizeTypeDefault;
    params->inDataFormat[i] = TIDL_inDataFormatRGBPlanar;
    params->reserved[i]     = 0;
    params->inDataNorm[i]   = 0;
    params->inYuvFormat[i]  = NOT_VALID;
    params->inDataPadInTIDL[i]  = 0;
  }
  for (i = 0; i < TIDL_MAX_ALG_OUT_BUFS; i++)
  {
    params->outElementSize[i] = -1;
    params->outElementType[i] = -1;
   /* set outTensorScale 0, this will be used to distinguish
     whether user expects the outTensorScale be computed by TIDL or want TIDL
     to honor user given value */
    params->outTensorScale[i] = 0.0;
    params->outZeroPoint[i]   = 0;
    params->outLayout[i]      = TIDL_LT_NCHW;

  }
  params->ddrLayers[0] = -1;

  multiCoreBatchProcInfo.numBatches = 1;
  multiCoreBatchProcInfo.numCores = 1;

}

/**
----------------------------------------------------------------------------
@ingroup    TIDL_Import
@fn         tidlValidateImportParams
@brief      Function validates input parameters related to tidl import
            sets appropriate error in response to violation from
            expected values.

@param      params : TIDL Create time parameters
@remarks    None
@return     Error related to parameter.
----------------------------------------------------------------------------
*/
int32_t tidlValidateImportParams(tidl_import_config * params)
{
  /* randParams can be either 0 or 1*/
  if(params->foldMaxPoolInConv2D == 1)
  {
     params->foldMaxPoolInConv2D = 0;
     printf("\n foldMaxPoolInConv2D is NOT suported in the current release. Disabling Now");
  }
  if(params->foldEltWiseInConv2D == 1)
  {
     params->foldEltWiseInConv2D = 0;
     printf("\n foldEltWiseInConv2D is NOT suported in the current release. Disabling Now");
  }
  if(params->numParamBits == 32)
  {
    params->numFeatureBits = 32;
  }

  if((params->numParamBits > 8) && (params->numFeatureBits <= 8))
  {
    params->numFeatureBits = 16;
  }
  if((params->numParamBits <= 8) && (params->numFeatureBits > 8))
  {
    params->numParamBits = 12;
  }
  /* Set inElementType based on numFeatureBits if not set in config file */
  if ((params->numFeatureBits > 8) && ((gParams.addDataConvertToNet & ADD_DC_LAYER_AT_INPUT) == 0))
  {
    for (int32_t i = 0; i < TIDL_MAX_ALG_IN_BUFS; i++)
    {
      if((params->inElementType[i] == TIDL_SinglePrecFloat) ||
         (params->inElementType[i] == TIDL_SignedWord) ||
         (params->inElementType[i] == TIDL_UnsignedWord)
        )
      {
        continue;
      }
      if(params->numFeatureBits == 32)
      {
        params->inElementType[i] = TIDL_SinglePrecFloat;
      }
      else if(params->inElementType[i] == TIDL_UnsignedChar)
      {
        params->inElementType[i] = TIDL_UnsignedShort;
      }
      else if(params->inElementType[i] == TIDL_SignedChar)
      {
        params->inElementType[i] = TIDL_SignedShort;
      }
    }
  }

  if((params->randParams != 0) && (params->randParams != 1))
  {
    printf("\n Invalid randParams setting : set either 0 or 1");
    return -1;
  }
  /* modelType must be one of the supported types */
  else if((params->modelType != 0) && (params->modelType != 1) && (params->modelType != 2)
          && (params->modelType != 3) && (params->modelType != 4) && (params->modelType != 5)&& (params->modelType != 6))
  {
    printf("\n Invalid modelType parameter setting : set either 0 or 1 or 2 or 3 or 4 or 5 or 6");
    return -1;
  }
  /* Currently quantizationStyle = 1 is supported */
  /*else if(params->quantizationStyle != 1)
  {
    printf("\n Invalid quantizationStyle parameter setting : set it to 1");
    return -1;
  }*/
  /* quantRoundAdd can be 0 to 100 */
  else if((params->quantRoundAdd < 0) || (params->quantRoundAdd > 100))
  {
    printf("\n Invalid quantRoundAdd parameter setting : set it 0 to 100");
    return -1;
  }
  /* numParamBits can be 4 to 12 */
  else if(((params->numParamBits < 4) || (params->numParamBits > 16)) && (params->numParamBits != 32))
  {
    printf("\n Invalid numParamBits parameter setting : set it 4 to 16 or 32 for Float mode");
    return -1;
  }
  /* inFileFormat can be either 0 or 1*/
  else if((params->inFileFormat < 0) && (params->inFileFormat > 2))
  {
    printf("\n Invalid inFileFormat parameter setting");
    return -1;
  }
  /* numFrames can be >0  */
  else if(params->numFrames < -1)
  {
    printf("\n Invalid numFrames parameter setting : set it to >0 ");
    return -1;
  }
  else if(params->numFramesBiasCalibration < -1)
  {
    printf("\n Invalid numFramesBiasCalibration parameter setting : set it to >0 ");
    return -1;
  }
  /* foldBnInConv2D can be either 0 or 1*/
  else if((params->foldBnInConv2D != 0) && (params->foldBnInConv2D != 1))
  {
    printf("\n Invalid foldBnInConv2D parameter setting : set either 0 or 1");
    return -1;
  }
  else if((params->foldPreBnConv2D != 0) && (params->foldPreBnConv2D != 1) && (params->foldPreBnConv2D != 2))
  {
    printf("\n Invalid foldPreBnConv2D parameter setting : set either 0, 1 or 2");
    return -1;
  }
  /* inElementType can be either 0 or 1*/
  else if((params->inElementType[0] < 0) && (params->inElementType[0] > 3 ))
  {
    printf("\n Invalid inElementType parameter setting : set either 0 to 3");
    return -1;
  }
  /* inQuantFactor can be >0  */
  else if(params->inQuantFactor[0] <= 0)
  {
    printf("\n Invalid inQuantFactor parameter setting : set it to >0 ");
    return -1;
  }
  /* inWidth can be >0  */
  else if((params->inWidth[0] < -1) || (params->inWidth[0] == 0))
  {
    printf("\n Invalid inWidth parameter setting : set it to >0 ");
    return -1;
  }
  /* inHeight can be >0  */
  else if((params->inHeight[0] < -1) || (params->inHeight[0] == 0))
  {
    printf("\n Invalid inHeight parameter setting : set it to >0 ");
    return -1;
  }
  /* inNumChannels must be >= 1 */
  else if((params->inNumChannels[0] < -1) || (params->inNumChannels[0] == 0))
  {
    printf("\n Invalid inNumChannels parameter setting: inNumChannels must be non-zero and positive");
    return -1;
  }
  else if((params->numBatches[0] < -1) || (params->numBatches[0] == 0) || (params->numBatches[0] > 1024))
  {
    printf("\n Invalid numBatches parameter setting : set it 1 to 1024 ");
    return -1;
  }
#if defined (SOC_J784S4) || defined (SOC_J722S)
  else if((params->numCores != 1) && (params->numCores != 2) && (params->numCores != 3) && (params->numCores != 4))
  {
    printf("\n Invalid numCores parameter setting : Number of cores must be either 1");

    for(int i = 2;i <= TIDL_MAX_NUM_CORES;i++) 
    {
      printf(", %d", i); /*prints upto the number of cores for the device */
    }
    printf("\n");

    return -1;
  }
  else if((params->inferenceMode != 0) && (params->inferenceMode != 1) && (params->inferenceMode != 2))
  {
    printf("\n Invalid inferenceMode parameter setting : Must be either 0, 1 or 2 \n");
    return -1;
  }
  else if((params->numCores) > TIDL_MAX_NUM_CORES) /*the macro value TIDL_MAX_NUM_CORES is decided based on device*/
  {
    printf("\n Invalid numCores : Device has total %d cores \n", TIDL_MAX_NUM_CORES);
    return -1;
  }
  else if(params->inferenceMode == 2 && params->numCores == 1) /*when runing on one core inference mode must be 0*/
  {
    printf("\n inferenceMode = 2 is supported only for numCores > 1, please use inferenceMode = 0 for numCores = 1 \n");
    return -1;
  }
#else
  else if((params->numCores != 1))
  {
    printf("\n Invalid numCores parameter setting : Number of cores must be 1");
    return -1;
  }
  else if(params->inferenceMode != 0)
  {
    printf("\n Invalid inferenceMode parameter setting : Must be 0");
    return -1;
  }
#endif
  else if(params->inferenceMode == TIDL_inferenceModeHighThroughput) /* Multi core batch processing checks */
  {
    // if(params->deviceName != TIDL_TDA4VH) /* TODO_NG : Enable this check after adding separate device name for TDA4VH */
    // {
    //   printf("\nInference mode 'TIDL_inferenceModeHighThroughput' is not supported for this device\n");
    //   return -1;
    // }
    if((params->numBatches[0] > params->numCores) && ((params->numBatches[0] % params->numCores) != 0)) /* TODO_NG : Update this to multiple of numCores when support added */
    {
      printf("\n\n**** Number of batches - %d - must be multiple of number of cores - %d - Current configuration is not supported !! ****\n\n", params->numBatches[0], params->numCores);
      return -1;
    }
    if( ! ((params->numBatches[0] > 1) && (params->numCores > 1)) )
    {
      printf("\n\n**** Running 'TIDL_inferenceModeHighThroughput' inference mode with single batch which is not optimal. Recommended to use inferenceMode = 0 ****\n\n");
    }
    if((params->modelType == TIDL_IMPORT_MODEL_FORMAT_CAFFE) || (params->modelType == TIDL_IMPORT_MODEL_FORMAT_TENSORFLOW) ||
    (params->modelType == TIDL_IMPORT_MODEL_FORMAT_TFLITE) || (params->modelType == TIDL_IMPORT_MODEL_FORMAT_ONNX))
    {
      /* Batch processing only supported for inFileFormat = 2 - check required only for standalone mode. OSRT does not use inFileFormat option */
      if(params->inFileFormat != 2)
      {
        printf("\n\n**** Parameter inFileFormat must be 2 for multi core batch processing ****\n\n");
        return -1;
      }
    }
  }
  else if((params->inferenceMode == 0) && (params->numCores > 1))
  {
    printf("\n\n**** Only numCores = 1 supported for inferenceMode = 0 ****\n\n");
    return -1;
  }
  else if(params->inferenceMode == TIDL_inferenceModeLowLatency)
  {
    if ((params->enableHighResOptimization > 0U           ) ||
      ((params->compileConstraintsFlag & 0x80)== 0x80    ) ||
      (strlen((const char*)params->fileNameGrpInfo) != 0 ))
    {
      printf("\n\n**** ERROR : High resolution optimization is currently not enabled for inference mode = 2, please use inference mode = 0 ****\n\n");
      return -1;
    }

    if(params->deviceName == TIDL_AM62A) /*this check if for J722S which currently supoorts batch mode on multi-core but does not support low latency*/
    {
      printf("\nInference mode 'TIDL_inferenceModeLowLatency' is not supported for this device\n");
      return -1;
    }
  }
  else
  {
    return 0;
  }

  return 0;

}

#define TIDL_CFG_MAX_LINE_SIZE (3000)
int32_t getNumberOfLinesIntheFile(char * fileName)
{
  FILE * fp1;
  int32_t i, lineCnt = 0;
  char line[TIDL_CFG_MAX_LINE_SIZE];

  fp1 = fopen((const char *)fileName, "r");
  if (fp1 == NULL)
  {
    printf("Could not open %s file for reading \n", fileName);
    return 0;
  }
  while (!feof(fp1))
  {
    fgets(line, TIDL_CFG_MAX_LINE_SIZE, fp1);
    lineCnt++;
  }
  return(lineCnt);
}

int32_t getAVXKernelEnv()
{
int32_t flowCtrl = 3;
#if (HOST_EMULATION)
    char *str;
    str = getenv("TIDL_RT_AVX_REF");
    if(!str)
    {
        flowCtrl |= 0x00000020;
    }
    else
    {
        int32_t
        env = atoi(str);
        if((env != 0) && (env != 1) && (env != 3))
        {
          printf("TIDL_RT_AVX_REF is expected to be either 0 or 1 or 3. -  %d is not supported. Setting it to zero\n", env);
          env = 0;
        }
        if(env)
        {
          flowCtrl |= env << 5;
        }
   }
#endif
    return flowCtrl;
}

void tidlQuantStatsTool(tidl_import_config * params)
{
  FILE * fp;
  char sysCommand[500];
  char orgPath[500];
  char absPath[500];
  char fileName[500];
  char qsFileName[500];
  char cfgFileName[500];
  char dirName[500];
  char outDirName[500];

  getAbsPath((char *)params->outputNetFile, outDirName);
  getDirFromPath(outDirName);

  sprintf(qsFileName, "%s/%s.qunat_stats_config.txt", outDirName,getFileNameFromPath(inConfigFilename));
  fp = fopen(qsFileName, "w+");
  if(fp== NULL)
  {
    printf("Could not open config  file %s  \n", qsFileName);
    return;
  }
  fprintf(fp, "inFileFormat    = %d\n",params->inFileFormat);
  fprintf(fp, "numFrames   = %d\n",params->numFrames);
  fprintf(fp, "postProcType   = %d\n",params->postProcType);
  fprintf(fp, "postProcDataId   = %d\n", params->postProcDataId);
  fprintf(fp, "quantRangeUpdateFactor   = %f\n", params->quantRangeUpdateFactor);
  getAbsPath((char *)params->inData, absPath);
  fprintf(fp, "inData   = %s\n",absPath);
  fprintf(fp, "outData   = \"%s/%s_stats_tool_out.bin\"\n", outDirName,getFileNameFromPath(inConfigFilename));

  getAbsPath((char *)params->outputNetFile, absPath);
  fprintf(fp, "netBinFile     = %s\n", absPath);

  sprintf(fileName, "%s%d.bin", (char *)params->outputParamsFile, 1);
  getAbsPath(fileName, absPath);
  fprintf(fp, "ioConfigFile        = %s\n", absPath);
  fprintf(fp, "flowCtrl               = %d\n", getAVXKernelEnv());
  fprintf(fp, "writeTraceLevel        = %d\n", params->writeTraceLevel);
  fprintf(fp, "debugTraceLevel        = %d\n", params->debugTraceLevel);
  fprintf(fp, "traceDumpBaseName        = \"%s/%s\n", outDirName,getFileNameFromPath(inConfigFilename));


  fclose(fp);

  strcpy(orgPath, qsFileName);
  getAbsPath(orgPath, absPath);

  strcpy(fileName, getFileNameFromPath((char *)params->tidlStatsTool));

  getAbsPath((char *)params->tidlStatsTool, dirName);
  getDirFromPath(dirName);

#ifdef _WIN32
  sprintf(sysCommand, "cd %s && %s s:%s", dirName, fileName, absPath);
#else
  sprintf(sysCommand, "cd %s && ./%s s:%s", dirName, fileName, absPath);
#endif
  if(params->debugTraceLevel > 0)
  {
    printf("%s\n", sysCommand);
  }
  system(sysCommand);

  return;

}

int64_t tidl_getModelSize(uint8_t * fileString)
{
  FILE * fptr;
  int64_t netSize;
  fptr = fopen((const char *)fileString, "rb");
  if (fptr)
  {
    fseek(fptr, 0L, SEEK_END);
    netSize = ftell(fptr);
    fclose(fptr);
    return netSize;
  }
  else
  {
    printf("Could Not Open Files %s\n", fileString);
    return -1;
  }

}

int tidlRunGraphVizTool(tidl_import_config * params)
{
  FILE * fp;
  char sysCommand[500];
  char absPath[500];

  fp = fopen((const char *)params->graphVizTool, "r");
  if (fp == NULL)
  {
    //printf("INFO : Couldn't open graphVizTool file: %s , Skipping Visualization \n", params->graphVizTool);
    return(0);
  }
  fclose(fp);

  getAbsPath((char *)params->graphVizTool, absPath);
  sprintf(sysCommand, "%s %s", absPath, params->outputNetFile);
  if(gParams.debugTraceLevel > 0)
  {
    printf("%s\n", sysCommand);
  }
  system(sysCommand);
  return 0;
}

int tidlRunModelDumpTool(tidl_import_config * params)
{
  FILE * fp;
  char sysCommand[2048];
  char absPath[500];
  char dumpFileName[500];

  fp = fopen((char *)params->modelDumpTool, "r");
  if (fp == NULL)
  {
    //printf("INFO : Couldn't open modelDumpTool file: %s , Skipping\n", params->modelDumpTool);
    return(0);
  }
  fclose(fp);
  getAbsPath((char *)params->modelDumpTool, absPath);

  strcpy(dumpFileName, (char*)params->outputNetFile);
  strcat(dumpFileName, ".txt");

  sprintf(sysCommand, "%s -model %s -io %s1.bin -perfsim -o %s",
          absPath, params->outputNetFile, params->outputParamsFile, dumpFileName);
  if(gParams.debugTraceLevel > 0)
  {
    printf("%s\n", sysCommand);
  }
  system(sysCommand);
  return 0;
}

int tidlWriteTensorNamesToFile(tidl_import_config * params, const char* suffix)
{
  FILE  * fp;
  char *fname;
  asprintf(&fname, "%s%s.layer_info.txt", params->outputNetFile, suffix);
  FILE *layerInfoFile = fopen(fname, "w");
  free(fname);
  int32_t i;
  int64_t modelSize = tidl_getModelSize(params->outputNetFile);
  sTIDL_Network_t * tidlNet = (sTIDL_Network_t *)malloc(modelSize);
  if (tidlNet == NULL)
  {
    printf("Could not Allocate memory for model read\n");
    return 0;
  }
  fp = fopen((const char *)params->outputNetFile, "rb");
  if (fp)
  {
    fread(tidlNet, 1, modelSize, fp);
    fclose(fp);
  }
  else
  {
    printf("Could not open %s\n", params->outputNetFile);
    return 0;
  }

  for (i = 0; i < tidlNet->numLayers; i++)
  {
    fprintf(layerInfoFile, "%d %d %s \n", i, tidlNet->TIDLLayers[i].outData.dataId,
      TIDL_getOutDataName(&orgTIDLNetStructure, tidlNet->TIDLLayers[i].outData.dataId));
  }
  free(tidlNet);
  fclose(layerInfoFile);
  return 0;
}

/** This function removes the string "tidl_net_" and ".bin" from the net bin file names to reduce the folder name size for perfsim
 * NOTE : THIS FUNCTION MUST BE IN SYNC WITH THE FUNCTION WITH SAME NAME IN PERFSIM
 */
std::vector<string> TIDL_shortenFolderNameForPerfSim(char * dirName)
{
  string netFilePath = dirName;
#ifdef _WIN32
    string netFileName = netFilePath.substr(netFilePath.find_last_of("\\/") + 1);
    string netDirName = netFilePath.substr(0, netFilePath.find_last_of("\\/"));
#else
    string netFileName = netFilePath.substr(netFilePath.find_last_of("/") + 1);
    string netDirName = netFilePath.substr(0, netFilePath.find_last_of("/"));
#endif

  if(netFileName.find("tidl_net_") != std::string::npos) netFileName.replace(netFileName.find("tidl_net_"), 9, "");
  if(netFileName.find(".bin") != std::string::npos) netFileName.replace(netFileName.find(".bin"), 4, "");
  std::vector<string> netInfo;
  netInfo.push_back(netDirName);
  netInfo.push_back(netFileName);
  return netInfo;
}

/** This function calculates network level performance
 *  by adding level performance
 * */
float TIDL_getPerfsimPerformance(sPerfSim_t * perfSimInfo)
{
  double perfTotal = 0.0;
  for(int i = 0; i < orgTIDLNetStructure.numLayers; i++)
  {
    perfTotal += perfSimInfo->procTime[i];
  }
  return (float)(perfTotal);
}

float tidlRunPerfSimTool(tidl_import_config * params)
{
  FILE * fp;
  char sysCommand[500];
  char orgPath[500];
  char absPath[500];
  char fileName[500];
  char psFileName[500];
  char dirName[500];
  sPerfSim_t * perSimInfo = (sPerfSim_t *)malloc(sizeof(sPerfSim_t));
  sPerfSim_t * orderedPerSimInfo = (sPerfSim_t *)malloc(sizeof(sPerfSim_t));

  int i, j, k, l, foundInData;
  int32_t currLayersGroupId = 1;

  fp = fopen((const char *)params->perfSimConfig, "r");
  if (fp == NULL)
  {
    printf("INFO : Couldn't open perfSimConfig file: %s , Skipping Performance Simulation \n", params->perfSimConfig);
    return(0);
  }
  fclose(fp);

  fp = fopen((const char *)params->perfSimTool, "r");
  if (fp == NULL)
  {
    printf("Couldn't open perfSimTool file: %s  \n", params->perfSimTool);
    return(0);
  }
  fclose(fp);


  getAbsPath((char *)params->perfSimConfig, absPath);

  getAbsPath((char *)params->outputNetFile, dirName);
  getDirFromPath(dirName);

#ifdef _WIN32
  sprintf(psFileName, "%s\\%s.perf_sim_config.txt", dirName, getFileNameFromPath(inConfigFilename));
  sprintf(sysCommand, "copy /Y %s %s", absPath, psFileName);
#else
  sprintf(psFileName, "%s/%s.perf_sim_config.txt", dirName, getFileNameFromPath(inConfigFilename));
  sprintf(sysCommand, "cp %s %s", absPath, psFileName);
#endif
  if(gParams.debugTraceLevel > 0)
  {
    printf("%s\n", sysCommand);
  }
  system(sysCommand);


  fp = fopen(psFileName, "a+");
  if (fp == NULL)
  {
    printf("Could not open config  file %s  \n", psFileName);
    return 0;
  }
  getAbsPath((char *)params->outputNetFile, absPath);
  fprintf(fp, "FILENAME_NET     = %s\n", absPath);
  fprintf(fp, "FILEFORMAT_NET     = -1\n");
  if(strcmp((char *)params->fileNameGrpInfo,"") != 0)
  {
    getAbsPath((char *)params->fileNameGrpInfo, absPath);
    fprintf(fp, "FILENAME_GRPINFO     = %s\n", absPath);
  }

  if (params->msmcSizeKB > 0 )
  {
    fprintf(fp, "MSMCSIZE_KB     = %d\n", params->msmcSizeKB);
  }
  if (params->deviceName != -1 )
  {
    fprintf(fp, "DEVICE_NAME     = %d\n", params->deviceName);
  }
  if (params->numCores != 1 )
  {
    fprintf(fp, "NUMCORES     = %d\n", params->numCores);
  }

#ifdef _WIN32
  fprintf(fp, "OUTPUT_DIR     = %s\\%s\n", dirName,getFileNameFromPath((char *)params->outputNetFile));
#else
  fprintf(fp, "OUTPUT_DIR     = %s/%s\n", dirName,getFileNameFromPath((char *)params->outputNetFile));
#endif

  if ((params->numParamBits > 8) || (params->numFeatureBits > 8))
  {
	  fprintf(fp, "DATATYPE     =   1\n");
  }
  if(params->ddrLayers[0] != -1) {
      int32_t *ddrLayers = &params->ddrLayers[0];

	  fprintf(fp, "DDRLAYERS    = ");
      while(*ddrLayers >= 0) {
          fprintf(fp, "%d ", *ddrLayers);
          ddrLayers++;
      }
	  fprintf(fp, "-1\n");
  }
  fclose(fp);

  strcpy(orgPath, psFileName);
  getAbsPath(orgPath, absPath);

  strcpy(fileName, getFileNameFromPath((char *)params->perfSimTool));

  getAbsPath((char *)params->perfSimTool, dirName);
  getDirFromPath(dirName);

  /* Add default compile  constraint incase user gives something else from the config file */
  params->compileConstraintsFlag |= DEFAULT_COMPILE_CONSTRAINT_NC_FLAGS;

#ifdef _WIN32
  sprintf(sysCommand, "cd %s && %s %s %d %d 2", dirName, fileName, absPath, params->compileConstraintsFlag, params->reserved[0]);
#else
  sprintf(sysCommand, "cd %s && ./%s %s %d %d 2", dirName, fileName, absPath, params->compileConstraintsFlag, params->reserved[0]);
#endif
  if(gParams.debugTraceLevel > 0)
  {
    printf("%s\n", sysCommand);
  }
  system(sysCommand);

  getAbsPath((char *)params->outputNetFile, dirName);

  vector<string> netInfo = TIDL_shortenFolderNameForPerfSim(dirName);
  string netDirName = netInfo[0];
  string netFileName = netInfo[1];
#ifdef _WIN32
  string perfSimInfoFile = netDirName + "\\" + netFileName + "\\perfSimInfo.bin";
#else
  string perfSimInfoFile = netDirName + "/" + netFileName + "/perfSimInfo.bin";
#endif
  fp = fopen(perfSimInfoFile.c_str(), "rb");
  if (fp)
  {
    fread(perSimInfo, 1, sizeof(sPerfSim_t), fp);
    fclose(fp);

#if 0 //Why to remove the bin file generated, its useful for dump comparison
#ifdef _WIN32
  sprintf(sysCommand, "del %s", netDirName.c_str());
#else
  sprintf(sysCommand, "rm %s", netDirName.c_str());
#endif
  system(sysCommand);
#endif

  }
  else
  {
    printf("Could not open %s\n", perfSimInfoFile.c_str());
    return 0;
  }
  memcpy(orderedPerSimInfo, perSimInfo, sizeof(sPerfSim_t));

  float networkPerf = TIDL_getPerfsimPerformance(perSimInfo);

  int64_t modelSize = tidl_getModelSize(params->outputNetFile);
  sTIDL_Network_t * tidlNet = (sTIDL_Network_t *)malloc(modelSize);
  if(tidlNet == NULL)
  {
    printf("Could not Allocate memory for model read\n");
    return 0;
  }
  fp = fopen((const char *)params->outputNetFile, "rb");
  if (fp)
  {
    fread(tidlNet, 1, modelSize, fp);
    fclose(fp);
  }
  else
  {
    printf("Could not open %s\n", params->outputNetFile);
    return 0;
  }

  tidlNet->deviceName = perSimInfo->simConfig.deviceName; //Copying over the deviceName into the n/w structure
  tidlNet->preEmptWorstCaseDelay = (float32_tidl)perSimInfo->preEmptWorstCaseDelay; //Copying preEmptWorstCaseDelay into the n/w structure
  tidlNet->modelGroupId = params->modelGroupId ;
  tidlNet->inferenceMode = params->inferenceMode;
  tidlNet->numCores = params->numCores;

  /* Reduction of dbPayLoad[MAX_DB_COMMON_SIZE] which located at the end of the sGCCommonDataBase_t which is located at the end of sGraphCompilerOutArgs_t*/
  /* Decrease the size of TIDL_NET_GC_BUF in ALG_PERSIST_DDR_NET_MEMREC */
  sGCCommonDataBase_t *commonDataBase = (sGCCommonDataBase_t *)getGCDataBase(&orderedPerSimInfo->graphCompOut,DATABASE_COMMON);
  int32_t size = sizeof(orderedPerSimInfo->graphCompOut) - MAX_GC_DB_SIZE + orderedPerSimInfo->graphCompOut.totalDataBaseSize;
  tidlNet->netBufInfo[TIDL_NET_GC_BUF].size = size ;
  /* Cannot use modelSize = tidl_getModelSize(params->outputNetFile) as is for below calculation of updated optimized model size, 
      since it would get updated/optimized with each NC run (creating issues when NC is run multiple times)
      Need to use modelSizeWithMaxNC instead */
  modelSize = orgTIDLNetStructure.modelSizeWithMaxNC - sizeof(orderedPerSimInfo->graphCompOut) + tidlNet->netBufInfo[TIDL_NET_GC_BUF].size;

  tidlNet->graphCompilerInfoOffset = tidlNet->netBufInfo[TIDL_NET_GC_BUF].offset ;
  memcpy((uint8_t*)tidlNet + tidlNet->graphCompilerInfoOffset , &orderedPerSimInfo->graphCompOut,
    tidlNet->netBufInfo[TIDL_NET_GC_BUF].size);

  int32_t currAlgLayer = 0;
  int32_t wlRepeatIter;
  sWorkloadUnit_t  *workloadUnit = NULL;
  sGetLayerIdContext_t getLayerIdContext;
  int32_t layerIdxCount = 1;
  sGraphCompilerOutArgs_t *gcOutArgs = NULL;
  sGraphCompilerOutArgsCore_t *gcOut = NULL;
  sGCCommonDataBase_t *commonDatabase = NULL;
  sGCDataBase_t           *gcDataBase = NULL;
  sWorkloadSuperGroup_t *wlSuperGrp = NULL;
  int32_t visited[TIDL_NUM_MAX_LAYERS] = {0};
  sGCHelperHandle           *gcHelperHandle = NULL;
  sGCHelperHandle            gcHelperHandleObj;

  if ( tidlNet->netBufInfo[TIDL_NET_GC_BUF].offset != 0 )
  {
    gcOutArgs  = (sGraphCompilerOutArgs_t *)get_int8_t_pointer((int8_t *)(tidlNet), tidlNet->netBufInfo[TIDL_NET_GC_BUF].offset);
    commonDatabase = (sGCCommonDataBase_t *)getGCDataBase(gcOutArgs,DATABASE_COMMON);
    gcOut = (sGraphCompilerOutArgsCore_t *)getGCDataBase(gcOutArgs , DATABASE_GCOUT_1);
    gcDataBase = &gcOut->dataBase;

    sBufParams_t* bufNodesDB = (sBufParams_t*) &commonDatabase->dbPayLoad[commonDatabase->dbParams[DB_COMMON_BUF].startOffset];
    sWorkloadUnit_t* workLoadUnitDB = (sWorkloadUnit_t*) &gcDataBase->dbPayLoad[gcDataBase->dbParams[DB_WL].startOffset];
    // sWorkloadUnitAuxilary_t* auxilaryWorkloadDB = (sWorkloadUnitAuxilary_t*) gcDataBase->dbPayLoad[gcDataBase->dbParams[DB_BUF].size +
    //                                                                       gcDataBase->dbParams[DB_WL].size];
    sWorkloadUnitAuxilary_t* auxilaryWorkloadDB = (sWorkloadUnitAuxilary_t*) &gcDataBase->dbPayLoad[gcDataBase->dbParams[DB_AUXWL].startOffset];

    gcHelperHandle = &gcHelperHandleObj;
    gcOutArgs->commonDatabaseInfo.bufDB.pBufList = bufNodesDB;
    gcOut->dataBaseInfo.wlDB.pWLList = workLoadUnitDB;
    gcOut->dataBaseInfo.wlAuxDB.pWLAuxList = auxilaryWorkloadDB;
    initGCHelperHandle(gcHelperHandle, tidlNet, &gcOut->dataBaseInfo, &gcOutArgs->commonDatabaseInfo);

    wlSuperGrp = &gcOut->superWorkload;
  }

  for (i = 0; i < tidlNet->numLayers; i++)
  {
    if(tidlNet->TIDLLayers[i].layerType == TIDL_DataLayer && tidlNet->TIDLLayers[i].numOutBufs > 0)
    {
      int32_t nextLyrIdx = tidl_getOutLayer(tidlNet, tidlNet->numLayers, tidlNet->TIDLLayers[i].outData.dataId);
      sBufParams_t *bufParams;
      /*we get the outbufParams of the Data-Layer*/
      bufParams = TIDL_getOutBufParams(tidlNet, i, tidlNet->TIDLLayers[i].layersGroupId);
      tidlNet->TIDLLayers[i].outData.pitch[TIDL_LINE_PITCH] = tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_WIDTH] + bufParams->padParams.cols;
      tidlNet->TIDLLayers[i].outData.pitch[TIDL_CHANNEL_PITCH] = bufParams->bufWidth;
      /* Batches are expected to be arranged along the channel axis. So the next batch is expected to start one channel pitch after the last channel of the previous batch */
      tidlNet->TIDLLayers[i].outData.pitch[TIDL_DIM2_PITCH] = tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_NUMCH] * tidlNet->TIDLLayers[i].outData.pitch[TIDL_CHANNEL_PITCH];
      tidlNet->TIDLLayers[i].outData.pitch[TIDL_DIM1_PITCH]= tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_DIM2] * tidlNet->TIDLLayers[i].outData.pitch[TIDL_DIM2_PITCH];
      tidlNet->TIDLLayers[i].outData.pitch[TIDL_ROI_PITCH]= tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_DIM1] * tidlNet->TIDLLayers[i].outData.pitch[TIDL_DIM1_PITCH];
      tidlNet->TIDLLayers[i].outData.padW = bufParams->padParams.cols;
      tidlNet->TIDLLayers[i].outData.padH = bufParams->padParams.rows;

      if((tidlNet->TIDLLayers[i].layerType == TIDL_DataLayer) && (tidlNet->TIDLLayers[nextLyrIdx].layerType == TIDL_DataConvertLayer) &&
        (tidlNet->TIDLLayers[nextLyrIdx].layerParams.dataConvertParams.layout == TIDL_LT_NHWC))
      {
          tidlNet->TIDLLayers[i].outData.pitch[TIDL_DIM2_PITCH] = tidlNet->TIDLLayers[i].outData.pitch[TIDL_CHANNEL_PITCH] * tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_HEIGHT];
          tidlNet->TIDLLayers[i].outData.pitch[TIDL_DIM1_PITCH]= tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_DIM2] * tidlNet->TIDLLayers[i].outData.pitch[TIDL_DIM2_PITCH];
          tidlNet->TIDLLayers[i].outData.pitch[TIDL_ROI_PITCH]= tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_DIM1] * tidlNet->TIDLLayers[i].outData.pitch[TIDL_DIM1_PITCH];
      }
    }
  }

  getLayerIdInit(&getLayerIdContext,
                    0,
                    -1,
                    tidlNet,
                    gcHelperHandle,
                    wlSuperGrp);

  while(1)
  {

    i = getLayerIdToExecute(&getLayerIdContext,
                                     0,
                                     &wlRepeatIter,
                                     &currAlgLayer,
                                     &workloadUnit);

    if ( i == NOT_VALID)
    {
      break;
    }

    int32_t isnextLyrReshape = 0;
    int32_t nextLyrIdx = tidl_getOutLayer(tidlNet, tidlNet->numLayers, tidlNet->TIDLLayers[i].outData.dataId);
    if(nextLyrIdx != -1)
        isnextLyrReshape = (tidlNet->TIDLLayers[nextLyrIdx].layerType == TIDL_ReshapeLayer) && (tidlNet->TIDLLayers[i].layerType != TIDL_DataConvertLayer);

    if( visited[tidlNet->TIDLLayers[i].outData.dataId] != 1 && tidlNet->TIDLLayers[i].layerType != TIDL_DataLayer)
    {
      /* Batch processing exit conditions for Large feature map and ST cases */
      if (params->numBatches[0] > 1)
      {
        int32_t numSplits = getNumSplits((const void *)workloadUnit);
        if (numSplits > 1)
        {
          printf("ERROR: Batch size of %d is not supported for your network configuration, please reduce the batch size\n", params->numBatches[0]);
          exit(-1);
        }
      }

      //printf("%2d : | %d %d | %d %d |\n", i, tidlNet->TIDLLayers[i].outData.padW, perSimInfo->sdataFlowInfo[i].bufInfo[OUT_FEAT_MAP][WRITE].padC,
      if(tidlNet->TIDLLayers[i].numOutBufs > 0)
      {
        sLink_t *link, *procLink=NULL, *outputDmaLink = NULL, *inputDmaLink=NULL;
        sBufParams_t *bufParams;
        for (int32_t linkIdx = 0; linkIdx < workloadUnit->numLinks; linkIdx++)
        {
          link = getLinkPtr(workloadUnit, NOT_VALID, linkIdx);
          if(link->type == LINK_PROC)
          {
            procLink = getLinkPtr(workloadUnit, NOT_VALID, linkIdx);
          }
          if(link->subType == LINK_X_FM_OUT_PART_TO_FULL)
          {
            outputDmaLink = getLinkPtr(workloadUnit, NOT_VALID, linkIdx);
          }
          if(link->subType == LINK_X_FM_IN_FULL_TO_PART)
          {
            inputDmaLink = getLinkPtr(workloadUnit, NOT_VALID, linkIdx);
          }
        }

        if(outputDmaLink == NULL && procLink == NULL)
        {
          bufParams = getBufParamsFromBufIndex(gcHelperHandle, inputDmaLink->sink.bufDBindex);
        }
        else if(outputDmaLink == NULL)
        {
          bufParams = getBufParamsFromBufIndex(gcHelperHandle, procLink->sink.bufDBindex);
        }
        else
        {
          bufParams = getBufParamsFromBufIndex(gcHelperHandle, outputDmaLink->sink.bufDBindex);
        }

        tidlNet->TIDLLayers[i].outData.pitch[TIDL_LINE_PITCH] = tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_WIDTH] + bufParams->padParams.cols;
        tidlNet->TIDLLayers[i].outData.pitch[TIDL_CHANNEL_PITCH] = bufParams->bufWidth;
        /* Batches are expected to be arranged along the channel axis. So the next batch is expected to start one channel pitch after the last channel of the previous batch */
        tidlNet->TIDLLayers[i].outData.pitch[TIDL_DIM2_PITCH] = tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_NUMCH] * tidlNet->TIDLLayers[i].outData.pitch[TIDL_CHANNEL_PITCH];
        tidlNet->TIDLLayers[i].outData.pitch[TIDL_DIM1_PITCH]= tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_DIM2] * tidlNet->TIDLLayers[i].outData.pitch[TIDL_DIM2_PITCH];
        tidlNet->TIDLLayers[i].outData.pitch[TIDL_ROI_PITCH]= tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_DIM1] * tidlNet->TIDLLayers[i].outData.pitch[TIDL_DIM1_PITCH];
        tidlNet->TIDLLayers[i].outData.padW = bufParams->padParams.cols;
        tidlNet->TIDLLayers[i].outData.padH = bufParams->padParams.rows;

        if(tidlNet->TIDLLayers[i].layerType == TIDL_ReshapeLayer)
        {
          tidlNet->TIDLLayers[i].outData.pitch[TIDL_CHANNEL_PITCH] = tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_WIDTH] * tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_HEIGHT];
          tidlNet->TIDLLayers[i].outData.pitch[TIDL_DIM2_PITCH] = tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_NUMCH] * tidlNet->TIDLLayers[i].outData.pitch[TIDL_CHANNEL_PITCH];
          tidlNet->TIDLLayers[i].outData.pitch[TIDL_DIM1_PITCH]= tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_DIM2] * tidlNet->TIDLLayers[i].outData.pitch[TIDL_DIM2_PITCH];
          tidlNet->TIDLLayers[i].outData.pitch[TIDL_ROI_PITCH]= tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_DIM1] * tidlNet->TIDLLayers[i].outData.pitch[TIDL_DIM1_PITCH];
        }
        if(tidlNet->TIDLLayers[i].layerType == TIDL_DataConvertLayer)
        {
          if (tidlNet->TIDLLayers[i].layerParams.dataConvertParams.layout == TIDL_LT_NCHW && tidlNet->TIDLLayers[i].layerParams.dataConvertParams.outLayout == TIDL_LT_NHWC)
          {
            tidlNet->TIDLLayers[i].outData.pitch[TIDL_DIM2_PITCH] = tidlNet->TIDLLayers[i].outData.pitch[TIDL_CHANNEL_PITCH] * tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_HEIGHT];
            tidlNet->TIDLLayers[i].outData.pitch[TIDL_DIM1_PITCH]= tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_DIM2] * tidlNet->TIDLLayers[i].outData.pitch[TIDL_DIM2_PITCH];
            tidlNet->TIDLLayers[i].outData.pitch[TIDL_ROI_PITCH]= tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_DIM1] * tidlNet->TIDLLayers[i].outData.pitch[TIDL_DIM1_PITCH];
          }
        }

        if((tidlNet->TIDLLayers[i].layerType == TIDL_PoolingLayer) && (tidlNet->TIDLLayers[i].layerParams.poolParams.kernelW == 0))
        {
          /* The TIDL_LINE_PITCH updated above should be restored back incase of golbalAvg pooling layer is the last layer in the network with pad > 0 */
          if (tidltb_isOutDataBuff(tidlNet, tidlNet->TIDLLayers[i].outData.dataId,currLayersGroupId))
          {
            tidlNet->TIDLLayers[i].outData.pitch[TIDL_LINE_PITCH] = tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_WIDTH];
          }
          /* If pad=0, we treat the output as flattened, i.e, as 1x1x1xC instead of 1xCx1x1 */
          if ((tidlNet->TIDLLayers[i].outData.padW == 0) && (tidlNet->TIDLLayers[i].outData.padH == 0))
          {
            tidlNet->TIDLLayers[i].outData.pitch[TIDL_CHANNEL_PITCH] = tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_WIDTH];
          }
          tidlNet->TIDLLayers[i].outData.pitch[TIDL_ROI_PITCH]  = tidlNet->TIDLLayers[i].outData.pitch[TIDL_CHANNEL_PITCH]*tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_NUMCH];
        }
        else if(tidlNet->TIDLLayers[i].layerType == TIDL_InnerProductLayer)
        {
          // tidlNet->TIDLLayers[i].outData.pitch[TIDL_CHANNEL_PITCH] =  tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_HEIGHT] * tidlNet->TIDLLayers[i].outData.pitch[TIDL_LINE_PITCH];

          // tidlNet->TIDLLayers[i].outData.pitch[TIDL_ROI_PITCH] =  tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_NUMCH] * tidlNet->TIDLLayers[i].outData.pitch[TIDL_CHANNEL_PITCH];
        }

        if(isnextLyrReshape)
        {
          tidlNet->TIDLLayers[i].outData.pitch[TIDL_CHANNEL_PITCH] = tidlNet->TIDLLayers[i].outData.pitch[TIDL_LINE_PITCH] * tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_HEIGHT];
          tidlNet->TIDLLayers[i].outData.pitch[TIDL_DIM2_PITCH]  = tidlNet->TIDLLayers[i].outData.pitch[TIDL_CHANNEL_PITCH]*tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_NUMCH];
          tidlNet->TIDLLayers[i].outData.pitch[TIDL_DIM1_PITCH]= tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_DIM2] * tidlNet->TIDLLayers[i].outData.pitch[TIDL_DIM2_PITCH];
          tidlNet->TIDLLayers[i].outData.pitch[TIDL_ROI_PITCH]= tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_DIM1] * tidlNet->TIDLLayers[i].outData.pitch[TIDL_DIM1_PITCH];
        }

      }
      visited[tidlNet->TIDLLayers[i].outData.dataId] = 1;
    }
  }

  uint32_t alignedModelSize = modelSize;


#if 0
  /* We don't want to copy dataFlowInfo for MAX_LAYERS but we still want to
  copy other output in sPerfSim_t hence find the size of only relevant information
  needed during TIDL inference. The assumption is nothing beyond sdataFlowInfo will
  be used by TIDL */
  sBuffer_t buf;
  sBuffer_t bufGc;
  buf.ptr = (void*)orderedPerSimInfo;
  buf.bufSize = sizeof(sPerfSim_t ) -
                    sizeof(orderedPerSimInfo->layerExecutionOrder) -
                    sizeof(orderedPerSimInfo->sdataFlowInfo) -
                    sizeof(orderedPerSimInfo->graphCompOut) +
                    (sizeof(sDataFlowInfo_t) * tidlNet->numLayers);

  tidlNet->dataFlowInfo = TIDL_alignParamsWrite(NULL, &buf, &alignedModelSize, 1);
  tidlNet->netBufInfo[TIDL_NET_COMPILER_BUF].offset  = tidlNet->dataFlowInfo;
  tidlNet->netBufInfo[TIDL_NET_COMPILER_BUF].size = buf.bufSize ;

  //:TODO:Currently write the complete graphCompOut but eventually should limit to current number of cores and number of actual layers
  bufGc.bufSize = sizeof(orderedPerSimInfo->graphCompOut);
  bufGc.ptr = (void *)&orderedPerSimInfo->graphCompOut;
  tidlNet->graphCompilerInfoOffset = TIDL_alignParamsWrite(NULL, &bufGc, &alignedModelSize, 1);
  tidlNet->netBufInfo[TIDL_NET_GC_BUF].offset  = tidlNet->graphCompOut;
  tidlNet->netBufInfo[TIDL_NET_GC_BUF].size = bufGc.bufSize ;

  fp = fopen((const char *)params->outputNetFile, "wb+");
  if (fp)
  {
    fwrite(tidlNet, 1, modelSize, fp);
  }
  else
  {
    printf("Could not open %s\n", params->outputNetFile);
    return 0;
  }
  alignedModelSize = modelSize;
  TIDL_alignParamsWrite(fp, &buf, &alignedModelSize, 1);
  TIDL_alignParamsWrite(fp, &bufGc, &alignedModelSize, 1);
#else

  tidlNet->dataFlowInfo = tidlNet->netBufInfo[TIDL_NET_COMPILER_BUF].offset ;
  memcpy((uint8_t*)tidlNet + tidlNet->dataFlowInfo , orderedPerSimInfo,
    tidlNet->netBufInfo[TIDL_NET_COMPILER_BUF].size);

  fp = fopen((const char *)params->outputNetFile, "wb+");
  if (fp)
  {
    fwrite(tidlNet, 1, modelSize, fp);
  }
  else
  {
    printf("Could not open %s\n", params->outputNetFile);
    return 0;
  }

#endif

  TIDL_writeInfo(tidlNet,
                        &orgTIDLNetStructure,
                        (const char *)params->outputParamsFile,
                        tidlNet->numLayers,
                        1,
                        orderedPerSimInfo,
                        &gParams);

  if(gParams.debugTraceLevel > 0){
    printf("Network buffer segments:\n");
    for(int32_t idx = 0 ; idx < TIDL_NET_TOTAL_BUF ; idx++)
    {
      printf("buf(%d), offset(%8d), size(%8d)\n", idx,
        tidlNet->netBufInfo[idx].offset, tidlNet->netBufInfo[idx].size);
    }
  }

  for (i = 0; i < orgTIDLNetStructure.numLayers; i++)
  {
    if(((perSimInfo->checkErrorFlag[i] & 0xFF) == 1) && ((gParams.deviceName & 0xFF) == TIDL_AM62A))
    {
      printf("ERROR: Layer - %s - is not supported on accelerator due to internal memory allocation constraints in this particular scenario. Please re-try compilation using open source runtimes offering and specify deny_list:layer_name:%s as part of compilation options\n",
                  (char *)orgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0], (char *)orgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0]);
    }
    else if( ((perSimInfo->checkErrorFlag[i] & 0xFF) == 1) && ((gParams.deviceName & 0xFF) == TIDL_TDA4AEP || (gParams.deviceName & 0xFF) == TIDL_TDA4AMPlus) )
    {
      gParams.enableOtfPad = 0;
      break;
    }
  }

  if ( orderedPerSimInfo != NULL )
  {
    free(orderedPerSimInfo);
  }
  free(perSimInfo);
  free(tidlNet);
  fclose(fp);
  return networkPerf;

}


/* Copy tensor stats from float model after STATS_COLLECTION_FLOAT calibration
 *                   to   quantized model
 * skip_input_datalayers: we don't want to update the input DataLayers
 */
void TIDL_copyTensorStats(sTIDL_OrgNetwork_t * pOrgTIDLNetStructureDst,
                          sTIDL_OrgNetwork_t * pOrgTIDLNetStructureSrc,
                          int32_t skip_input_datalayers)
{
  int32_t i, layerIdx;
  pOrgTIDLNetStructureDst->quantStats = pOrgTIDLNetStructureSrc->quantStats;
  for ( layerIdx = 0; layerIdx <pOrgTIDLNetStructureSrc->numLayers; layerIdx++)
  {
    sTIDL_LayerPC_t& srcLayer = pOrgTIDLNetStructureSrc->TIDLPCLayers[layerIdx];
    sTIDL_LayerPC_t& dstLayer = pOrgTIDLNetStructureDst->TIDLPCLayers[layerIdx];
    int32_t dataConvertInput, dataConvertOutput;
    if (skip_input_datalayers && srcLayer.layerType == TIDL_DataLayer &&
                                 srcLayer.numInBufs == -1)
      continue;

    dataConvertInput = 0;
    dataConvertOutput = 0;

    if ( srcLayer.layerType == TIDL_DataConvertLayer )
    {
      const sTIDL_dataConvertParams_t *params = &dstLayer.layerParams.dataConvertParams;
      if ( params->type == TIDL_DC_TYPE_INPUT )
      {
        dataConvertInput = 1;
      }
      else
      {
        dataConvertOutput = 1;
      }

    }

    for ( i = 0; i < srcLayer.numInBufs; i++)
    {
      dstLayer.inData[i].maxTensorValue = srcLayer.inData[i].maxTensorValue;
      dstLayer.inData[i].minTensorValue = srcLayer.inData[i].minTensorValue;
      if ( dataConvertInput != 1 )
      {
        /* Do not overwrite the tensorScale if its user given i.e. when its input data convert layer */
        dstLayer.inData[i].tensorScale    = srcLayer.inData[i].tensorScale;
      }
      dstLayer.inData[i].roundBits      = srcLayer.inData[i].roundBits;

    }

    for ( i = 0; i < srcLayer.numOutBufs; i++)
    {
      dstLayer.outData[i].maxTensorValue = srcLayer.outData[i].maxTensorValue;
      dstLayer.outData[i].minTensorValue = srcLayer.outData[i].minTensorValue;
      dstLayer.outData[i].roundBits      = srcLayer.outData[i].roundBits;
      if ( dataConvertOutput != 1 )
      {
        /* Do not overwrite the tensorScale if its user given i.e. when its output data convert layer */
        dstLayer.outData[i].tensorScale    = srcLayer.outData[i].tensorScale;
      }
    }
  }
}
int32_t TIDL_readQuantStats(sTIDL_OrgNetwork_t * pOrgTIDLNetStructure, tidl_import_config * params, uint32_t numLayers)
{
  int32_t layerIdx;
  int32_t idx;

  FILE  * fp;
  int32_t i;
  int32_t isQuantStatsAvail = 0;
  int64_t modelSize = tidl_getModelSize(params->outputNetFile);
  sTIDL_Network_t * tidlNet = (sTIDL_Network_t *)malloc(modelSize);
  if (tidlNet == NULL)
  {
    printf("Could not Allocate memory for model read\n");
    return -1;
  }
  fp = fopen((const char *)params->outputNetFile, "rb");
  if (fp)
  {
    fread(tidlNet, 1, modelSize, fp);
    fclose(fp);
  }
  else
  {
    printf("Could not open %s\n", params->outputNetFile);
    return -1;
  }

  pOrgTIDLNetStructure->quantStats = TIDL_QUANT_STATS_NONE;

  if ( tidlNet->isQuantStatsAvailable == 1 )
  {
    pOrgTIDLNetStructure->quantStats = TIDL_QUANT_STATS_FIXED;
    if (tidlNet->weightsElementSize == 4 )
    {
      pOrgTIDLNetStructure->quantStats = TIDL_QUANT_STATS_FLOAT;
    }

    for ( layerIdx = 0; layerIdx < numLayers; layerIdx++)
    {
      for ( idx = 0; idx < tidlNet->TIDLLayers[layerIdx].numInBufs; idx++)
      {
        const sTIDL_DataParams_t * inDataParms =  TIDL_getOutDataInNet(tidlNet, tidlNet->TIDLLayers[layerIdx].inData[idx]);
        pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].inData[idx].maxTensorValue =
            inDataParms->maxTensorValue;
        pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].inData[idx].minTensorValue=
            inDataParms->minTensorValue;
        pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].inData[idx].tensorScale =
            inDataParms->tensorScale;
        pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].inData[idx].roundBits =
            inDataParms->roundBits;
      }

      for ( idx = 0; idx < tidlNet->TIDLLayers[layerIdx].numOutBufs; idx++)
      {
        pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].outData[idx].maxTensorValue =
            tidlNet->TIDLLayers[layerIdx].outData.maxTensorValue;
        pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].outData[idx].minTensorValue=
            tidlNet->TIDLLayers[layerIdx].outData.minTensorValue;
        pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].outData[idx].tensorScale =
            tidlNet->TIDLLayers[layerIdx].outData.tensorScale;
        pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].outData[idx].roundBits =
            tidlNet->TIDLLayers[layerIdx].outData.roundBits;

      }
    }
  }

  isQuantStatsAvail = tidlNet->isQuantStatsAvailable;

  if ( tidlNet != NULL )
  {
    free(tidlNet);
  }
  return (isQuantStatsAvail);
}

int tidlRunQuantStatsTool(sTIDL_OrgNetwork_t * pOrgTIDLNetStructure,
                                                sTIDL_Network_t      *  pTIDLNetStructure,
                                                tidl_import_config * params,
                                                uint32_t numLayers)
{
  FILE * fp;
  int32_t quantStatus;

  if(!params->executeQuantsTool)
  {
    return 0;
  }
  fp = fopen((const char *)params->inData, "r");
  if (fp == NULL)
  {
    printf("INFO : Couldn't open inData file: %s  , Skipping Range Collection for Quantization \n", params->inData);
    return(0);
  }
  fclose(fp);

  fp = fopen((const char *)params->tidlStatsTool, "r");
  if (fp == NULL)
  {
    printf("Couldn't open tidlStatsTool file: %s  \n", params->tidlStatsTool);
    return(0);
  }
  fclose(fp);

  if (params->numFrames == -1)
  {
    if ((params->inFileFormat == 2) || (params->inFileFormat == 5))
    {
     params->numFrames = getNumberOfLinesIntheFile((char *)params->inData);
    }
    else if ((params->inFileFormat == 0) || (params->inFileFormat == 1) || (params->inFileFormat == 3))
    {
      params->numFrames = 1;
    }
  }
  if (params->numFrames > 0)
  {
    printf("\n~~~~~Running TIDL in PC emulation mode to collect Activations range for each layer~~~~~\n");
    tidlQuantStatsTool(params);
  }

  /* Read back the stats from the model written after quants stats tool */
 quantStatus = TIDL_readQuantStats(pOrgTIDLNetStructure, params, pOrgTIDLNetStructure->numLayers);
  if (quantStatus != -1 )
  {
    pTIDLNetStructure->isQuantStatsAvailable = quantStatus;
  }
  return 0;
}

void tidl_updateNetPitch(sTIDL_Network_t * tidlNet)
{
  int32_t i, j;
  for (i = 0; i < tidlNet->numLayers; i++)
  {
    for (j = 0; j < tidlNet->TIDLLayers[i].numOutBufs; j++)
    {
      tidlNet->TIDLLayers[i].outData.pitch[TIDL_LINE_PITCH] = tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_WIDTH] +
        tidlNet->TIDLLayers[i].outData.padW;
      int32_t isPadW = tidlNet->TIDLLayers[i].outData.padW ? 1 : 0;
      tidlNet->TIDLLayers[i].outData.pitch[TIDL_CHANNEL_PITCH] = tidlNet->TIDLLayers[i].outData.pitch[TIDL_LINE_PITCH] * (tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_HEIGHT] + tidlNet->TIDLLayers[i].outData.padH * 2 + isPadW);
      tidlNet->TIDLLayers[i].outData.pitch[TIDL_DIM2_PITCH] = tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_NUMCH] * tidlNet->TIDLLayers[i].outData.pitch[TIDL_CHANNEL_PITCH];
      tidlNet->TIDLLayers[i].outData.pitch[TIDL_DIM1_PITCH]= tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_DIM2] * tidlNet->TIDLLayers[i].outData.pitch[TIDL_DIM2_PITCH];
      tidlNet->TIDLLayers[i].outData.pitch[TIDL_ROI_PITCH]= tidlNet->TIDLLayers[i].outData.dimValues[TIDL_DIM_DIM1] * tidlNet->TIDLLayers[i].outData.pitch[TIDL_DIM1_PITCH];
    }
  }
}

sTIDL_tfOutReshapeMap_t sTIDL_OutReshapeTable[] =
{
  { TIDL_DataLayer                     ,  TIDL_tfOutReshapeDataLayer },
  { TIDL_ConvolutionLayer              ,  TIDL_tfOutReshapeConvLayer },
  { TIDL_PoolingLayer                  ,  TIDL_tfOutReshapePoolingLayer },
  { TIDL_ReLULayer                     ,  TIDL_tfOutReshapeRelu },
  { TIDL_PReLULayer                    ,  TIDL_tfOutReshapePRelu },
  { TIDL_EltWiseLayer                  ,  TIDL_tfOutReshapeEltwise },
  { TIDL_InnerProductLayer             ,  TIDL_tfOutReshapeIPLayer },
  { TIDL_SoftMaxLayer                  ,  TIDL_tfOutReshapeSoftmax },
  { TIDL_BatchNormLayer                ,  TIDL_tfOutReshapeBN },
  { TIDL_BiasLayer                     ,  TIDL_tfOutReshapeIdentity },
  { TIDL_ScaleLayer                    ,  TIDL_tfOutReshapeIdentity },
  { TIDL_Deconv2DLayer                 ,  TIDL_tfOutReshapeDeConvLayer },
  { TIDL_ConcatLayer                   ,  TIDL_tfOutReshapeConcatLayer },
  { TIDL_SplitLayer                    ,  TIDL_tfOutReshapeSliceLayer },
  { TIDL_SliceLayer                    ,  TIDL_tfOutReshapeSliceLayer },
  { TIDL_CropLayer                     ,  TIDL_tfOutReshapeCropLayer },
  { TIDL_FlattenLayer                  ,  TIDL_tfOutReshapeFlattenLayer },
  { TIDL_DropOutLayer                  ,  TIDL_tfOutReshapeIdentity },
  { TIDL_ArgMaxLayer                   ,  TIDL_tfOutReshapeArgmaxLayer },
  { TIDL_DetectionOutputLayer          ,  TIDL_tfOutReshapeDetOutLayer },
  { TIDL_ShuffleChannelLayer           ,  TIDL_tfOutReshapeIdentity },
  { TIDL_ResizeLayer                   ,  TIDL_tfOutReshapeResize },
  { TIDL_RoiPoolingLayer               ,  TIDL_tfOutReshapeRoiPoolingLayer },
  { TIDL_OdPostProcessingLayer         ,  TIDL_tfOutReshapeOdPostProcessingLayer },
  { TIDL_DepthToSpaceLayer             ,  TIDL_tfOutReshapeDeptoSpace },
  { TIDL_SigmoidLayer                  ,  TIDL_tfOutReshapeSigmoid },
  { TIDL_PadLayer                      ,  TIDL_tfOutReshapePadLayer },
  { TIDL_ColorConversionLayer          ,  TIDL_tfOutReshapeColorConversionLayer },
  { TIDL_OdOutputReformatLayer         ,  TIDL_tfOutReshapeOdOutputReformatLayer },
  { TIDL_DataConvertLayer              ,  TIDL_tfOutReshapeDataConvert },
  { TIDL_CustomLayer                   ,  TIDL_tfOutReshapeCustomLayer },
  { TIDL_BatchReshapeLayer             ,  TIDL_tfOutReshapeBatchReshape },
  { TIDL_ReduceLayer                   ,  TIDL_tfOutReshapeReduceMaxLayer },
  { TIDL_ScatterElementsLayer          ,  TIDL_tfOutReshapeScatterElementsLayer},
  { TIDL_SqueezeLayer                  ,  TIDL_tfOutReshapeSqueeze },
  { TIDL_TanhLayer                     ,  TIDL_tfOutReshapeTanh },
  { TIDL_HardSigmoidLayer              ,  TIDL_tfOutReshapeHardSigmoid },
  { TIDL_ELULayer                      ,  TIDL_tfOutReshapeELU },
  { TIDL_ReshapeLayer                  ,  TIDL_tfOutReshapeReshapeLayer },
  { TIDL_ConstDataLayer                ,  TIDL_tfOutReshapeConstDataLayer },
  { TIDL_GatherLayer                   ,  TIDL_tfOutReshapeGatherLayer},
  { TIDL_TransposeLayer                ,  TIDL_tfOutReshapeTransposeLayer },
  { TIDL_LayerNormLayer                 ,  TIDL_tfOutReshapeIdentity },
  { TIDL_UnsupportedLayer              ,  TIDL_tfOutReshapeIdentity },
  { TIDL_PriorBoxLayer                 ,  TIDL_tfOutReshapeIdentity },
  { TIDL_PermuteLayer                  ,  TIDL_tfOutReshapeIdentity },
  { TIDL_ShapeLayer                    ,  TIDL_tfOutReshapeIdentity },
  { TIDL_ClipLayer                     ,  TIDL_tfOutReshapeClip },
  { TIDL_MinimumLayer                  ,  TIDL_tfOutReshapeIdentity },
  { TIDL_LeakyReluLayer                ,  TIDL_tfOutReshapeIdentity },
  { TIDL_IdentityLayer                 ,  TIDL_tfOutReshapeIdentity },
  { TIDL_BatchToSpaceLayer             ,  TIDL_tfOutReshapeBatchtoSpace },
  { TIDL_SpaceToBatchLayer             ,  TIDL_tfOutReshapeSpacetoBatch },
  { TIDL_PackLayer                     ,  TIDL_tfOutReshapeIdentity },
  { TIDL_DequantizeLayer               ,  TIDL_tfOutReshapeIdentity },
  { TIDL_QuantizeLayer                 ,  TIDL_tfOutReshapeIdentity },
  { TIDL_SqrtLayer                     ,  TIDL_tfOutReshapeIdentity },
  { TIDL_ReduceMeanLayer               ,  TIDL_tfOutReshapeIdentity },
  { TIDL_PowLayer                      ,  TIDL_tfOutReshapeIdentity },
  { TIDL_DivLayer                      ,  TIDL_tfOutReshapeIdentity },
  { TIDL_SubLayer                      ,  TIDL_tfOutReshapeIdentity },
  { TIDL_PatchMergeLayer               ,  TIDL_tfOutReshapeIdentity },
  { TIDL_CastLayer                     ,  TIDL_tfOutReshapeIdentity }
};

int32_t tidl_updateHighResOptimization(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i;
  for(i=0; i< layerIndex; i++)
  {
    if((pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_CustomLayer) ||
       (pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_PadLayer) ||
       (gParams.numFeatureBits > 8))
    {
      gParams.enableHighResOptimization = 0;
      break;
    }
  }

  return 0;
}

bool tidl_isYuvLayerPresent(void)
{
  bool isYuvLayerPresent = false;
  int32_t i;
  for(i=0; i<TIDL_NUM_IN_BUFS; i++)
  {
    if(TIDL_inYuvFormatYuv420_NV12 == gParams.inYuvFormat[i])
    {
      isYuvLayerPresent = true;
      break;
    }
  }
  return isYuvLayerPresent;
}

void tidl_printOpsList(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure)
{
  printf("printing Current net\n");
  for (int i = 0; i < pOrgTIDLNetStructure.numLayers; i++)
  {
    printf("%5d|%-30s|%-50s|%-50s|%5d|%5d|\n", i, TIDL_LayerString[pOrgTIDLNetStructure.TIDLPCLayers[i].layerType], pOrgTIDLNetStructure.TIDLPCLayers[i].inDataNames[0], pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0], pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dataId, pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dataId);
  }
}
void  tidl_resetBatchPadValues(sTIDL_OrgNetwork_t * tIDLNetStructure)
{
  int32_t i, j;
  for (i = 0; i < tIDLNetStructure->numLayers; i++)
  {
    for (j = 0; j < TIDL_NUM_IN_BUFS; j++)
    {
      tIDLNetStructure->TIDLPCLayers[i].inData[j].batchPadW = 0;
      tIDLNetStructure->TIDLPCLayers[i].inData[j].batchPadH = 0;
    }
    for (j = 0; j < TIDL_NUM_OUT_BUFS; j++)
    {
      tIDLNetStructure->TIDLPCLayers[i].outData[j].batchPadW = 0;
      tIDLNetStructure->TIDLPCLayers[i].outData[j].batchPadH = 0;
    }
  }
}

int32_t doesNetworkContainLayer(sTIDL_OrgNetwork_t &orgTIDLNetStructure, int32_t layerType)
{
  int32_t numLayers = orgTIDLNetStructure.numLayers;
  int32_t doesNwContainLayer = 0;
  int32_t i;
  for(i = 0; i < numLayers; i++)
  {
    if(orgTIDLNetStructure.TIDLPCLayers[i].layerType == layerType)
    {
      doesNwContainLayer = 1;
      break;
    }
  }
  return doesNwContainLayer;
}

int32_t doesNetworkContainDeconv16Bit(sTIDL_OrgNetwork_t &orgTIDLNetStructure)
{
  int32_t numLayers = orgTIDLNetStructure.numLayers;
  int32_t doesNwContainDeconv16bit = 0;
  int32_t weightElemBytes = 1U;
  int32_t i;
  for(i = 0; i < numLayers; i++)
  {
    if(orgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_Deconv2DLayer)
    {
      weightElemBytes = ((orgTIDLNetStructure.TIDLPCLayers[i].weightsElementSizeInBits + 7) / 8);
      if(weightElemBytes > 1U)
      {
        doesNwContainDeconv16bit = 1U;
        break;
      }
    }
  }
  return doesNwContainDeconv16bit;
}

int32_t doesNetworkContainUnsupportedConvolutionConfig(sTIDL_OrgNetwork_t &orgTIDLNetStructure)
{
  int32_t numLayers = orgTIDLNetStructure.numLayers;
  int32_t status = 0;
  int32_t i;
  for(i = 0; i < numLayers; i++)
  {
    if(orgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_ConvolutionLayer)
    {
      if((orgTIDLNetStructure.TIDLPCLayers[i].layerParams.convParams.padW > orgTIDLNetStructure.TIDLPCLayers[i].inData->dimValues[TIDL_DIM_WIDTH]) && (orgTIDLNetStructure.TIDLPCLayers[i].layerParams.convParams.numGroups != orgTIDLNetStructure.TIDLPCLayers[i].layerParams.convParams.numInChannels))
      {
        status = 1;
        break;
      }
      else if(orgTIDLNetStructure.TIDLPCLayers[i].layerParams.convParams.strideW > 2 || orgTIDLNetStructure.TIDLPCLayers[i].layerParams.convParams.strideH > 2)
      {
        status = 1;
        break;
      }
    }
  }
  return status;
}

int32_t doesNetworkContainD2S(sTIDL_OrgNetwork_t &orgTIDLNetStructure)
{
  int32_t numLayers = orgTIDLNetStructure.numLayers;
  int32_t doesNetworkContainD2SConv = 0;
  int32_t i;
  for(i = 0; i < numLayers; i++)
  {
    if(orgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_ConvolutionLayer)
    {
      if(orgTIDLNetStructure.TIDLPCLayers[i].layerParams.convParams.enableDepthToSpace == 1U)
      {
        doesNetworkContainD2SConv = 1;
        break;
      }
    }
  }
  return doesNetworkContainD2SConv;
}

/*Toggle OTF Padding "ON" for Supported Cases:*/
void TIDL_correctDeviceConfig(sTIDL_OrgNetwork_t &orgTIDLNetStructure)
{
  bool disableOtf = false;
  if((gParams.deviceName == TIDL_TDA4AEP) || (gParams.deviceName == TIDL_TDA4AMPlus))
  {
    /*Evaluate whether OTF should be enabled*/
    if ((gParams.enableHighResOptimization > 0U           ) ||
       ((gParams.compileConstraintsFlag & 0x80)== 0x80    ) ||
       (strlen((const char*)gParams.fileNameGrpInfo) != 0 ))
    {
      /*High Resolution optimizations*/
      if(doesNetworkContainLayer(orgTIDLNetStructure, TIDL_Deconv2DLayer))
      {
        disableOtf = true;
      }
      else
      {
        disableOtf = false;
      }
    }
    else if(doesNetworkContainLayer(orgTIDLNetStructure, TIDL_DepthToSpaceLayer))
    {
      /*Pixel Shuffle*/
      disableOtf = true;
    }
    else if(doesNetworkContainLayer(orgTIDLNetStructure, TIDL_CustomLayer))
    {
      /*Custom Layer*/
      disableOtf = true;
    }
    else if(doesNetworkContainDeconv16Bit(orgTIDLNetStructure))
    {
      /*16-bit deconvolution Layer*/
      disableOtf = true;
    }
    else if(doesNetworkContainUnsupportedConvolutionConfig(orgTIDLNetStructure))
    {
      /*Unsupported padding configuration*/
      disableOtf = true;
    }
    else if(doesNetworkContainD2S(orgTIDLNetStructure))
    {
      /*D2S*/
      disableOtf = true;
    }
    if(!disableOtf)
    {
      /*Enable OTF:*/
      gParams.deviceName = gParams.deviceName | TIDL_OTF_FLAG_BIT ;
      gParams.enableOtfPad = 1U;
    }
  }
  else if(gParams.deviceName == TIDL_AM62A)
  {
    /*Enable OTF:*/
    gParams.deviceName = gParams.deviceName | TIDL_OTF_FLAG_BIT;
    gParams.enableOtfPad = 1U;
    /*Disable supertiling for AM62A*/
    if((gParams.enableHighResOptimization > 0U           ) ||
       ((gParams.compileConstraintsFlag & 0x80)== 0x80    ) ||
       (strlen((const char*)gParams.fileNameGrpInfo) != 0 ))
    {
      //gParams.enableHighResOptimization = 0U;
      //gParams.compileConstraintsFlag &= ~(0x80);
    }
  }
  else if((gParams.deviceName & TIDL_OTF_FLAG_BIT) == TIDL_OTF_FLAG_BIT)
  {
    gParams.enableOtfPad = 1U;
  }
}


void tidl_optimizeNet(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t &layerIndex, int32_t &dataIndex)
{
  if(gParams.debugTraceLevel > 0)
  {
    printf("Running tidl_optimizeNet \n");
  }
  pOrgTIDLNetStructure.numLayers = layerIndex;

  if(gParams.inferenceMode == TIDL_inferenceModeHighThroughput) /* multi core batch processing */
  {
    /* Save original number of batches and cores */
    multiCoreBatchProcInfo.numBatches = gParams.numBatches[0];
    /** numCores -- e.g. if numCores = 4 and numBatches = 2, set numCores to 2 instead of 4
     * This value is used to set numVirtualCores in IO params which determines number of instances of single batch to create and must be min(numCores, numBatches)
    */
    multiCoreBatchProcInfo.numCores = (gParams.numBatches[0] > gParams.numCores) ? gParams.numCores : gParams.numBatches[0];
    /* Should be treated as single batch for TIDL compilation, but capture the parameters as part of gIOParams */
    for(int i = 0; i < TIDL_NUM_IN_BUFS; i++)
    {
      gParams.numBatches[i] = 1;
    }
    gParams.numCores = 1;
  }

  tidl_addConstDataLayers (orgTIDLNetStructure, orgTIDLNetStructure.numLayers, &dataIndex, &gParams);
  tidl_addInDataLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers, &dataIndex);
  tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);

  // Update input data shape with import config file
  tidl_fillInDataLayerShape(orgTIDLNetStructure, &gParams, orgTIDLNetStructure.numLayers);

  // Split single outDataLayer with multi-data to multiple outDataLayers
  tidl_splitOutDataLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure,
                             orgTIDLNetStructure.numLayers);

  //layerNorm:
  tidl_mergeLayerNormLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);
  tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);

  //gelu
  tidl_mergeGeluLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);
  tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);

  //Merge Q/DQ Layers (ONNX)
  if(gParams.preQuantizedModel)
  {
    tidl_mergeReluLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
    tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);
    tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);
    tidl_mergeInitializerDQLayers(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
    tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);
    //tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);
    /*For DQ's associated with eltwise, const etc add input buffers:*/
    //tidl_addConstDataLayers (orgTIDLNetStructure, orgTIDLNetStructure.numLayers, &dataIndex, &gParams);
    //tidl_addInDataLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers, &dataIndex);
    tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);
    tidl_mergeQDQLayers(orgTIDLNetStructure, orgTIDLNetStructure.numLayers, false);
    tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);
    tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);
    tidl_mergeQDQLayers(orgTIDLNetStructure, orgTIDLNetStructure.numLayers, true);
    tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);
    tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);
  }

  if(true == tidl_isYuvLayerPresent())
  {
    tidl_addLayersForUVConversion(orgTIDLNetStructure, &dataIndex, &gParams);
    tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);
  }

  /* Add Normalization layer if user requested the same */
  if ((gParams.inDataNorm[0]) && (false == tidl_isYuvLayerPresent()))
  {
    tidl_addNormLayerToInData(orgTIDLNetStructure, orgTIDLNetStructure.numLayers, &dataIndex, &gParams);
    tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);
  }

  // Parse Shuffle Layer (ONNX ONLY)
  if(gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TFLITE_RT || gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TFLITE)
  {
    tidl_FindHardSigmoidLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  }
  tidl_FindOnnxShuffleLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);

  // Remove Padding Layer, not appliable in Caffe (ONNX ONLY)
  tidl_mergePadLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);

  // Remove Split Layers
  tidl_mergeSplitLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);

  //Replace Pack-Pack-Reshape with NN resize
  tidl_mergePackToNNResizeLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);
  tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);

  if(!tidlInputTensorDimCheck(orgTIDLNetStructure))
  {
    exit(0);
  }

  if(gParams.debugTraceLevel > 0)
  {
    tidl_printOpsList(orgTIDLNetStructure);
  }
  // Call Reshape functions to determine the input/output shape of each layer.
  tidl_updateOutDataShape(orgTIDLNetStructure, 0, orgTIDLNetStructure.numLayers, (sTIDL_tfOutReshapeMap_t *)&sTIDL_OutReshapeTable);
  
  /** Fuse Patch Merging Block */
  tidl_fusePatchMergingBlock (orgTIDLNetStructure, &dataIndex, orgTIDLNetStructure.numLayers);

  // Change the slice axis to channel dim if it is on higher dimensions
  tidl_reduceSliceDims (orgTIDLNetStructure, &dataIndex, orgTIDLNetStructure.numLayers);
  tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);

  // Duplicate Slice Layer. 3 layers for 3 outputs.
  tidl_duplicateSliceLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);

  tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);

  // Split Resize layer into multiple layers when resize ratio is > 4x.
  tidl_splitResizeLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers, dataIndex);
  tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);

  tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);
  tidl_convertPoolToIdentityLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  tidl_convertSqueezeToFlattenLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);

  tidl_mergeIdentitytLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);

  tidl_mergeReluLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);

  tidl_mergeBiasLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);

  tidl_convertBiasToBNLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);

  tidl_mergeBNLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);

  tidl_mergeReluLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);

  tidl_optimizeGlobalAveragePoolingLayers (orgTIDLNetStructure, &dataIndex, orgTIDLNetStructure.numLayers);
  tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);

  tidl_optimizePatchEmbedding(orgTIDLNetStructure, orgTIDLNetStructure.numLayers, &dataIndex);
  tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);
  tidl_updateOutDataShape(orgTIDLNetStructure, 0, orgTIDLNetStructure.numLayers, (sTIDL_tfOutReshapeMap_t *)&sTIDL_OutReshapeTable);

  /* Self-Attention Optimization: MutMal B transpose */
  /** Absorb B-side transpose in Innerproduct if present
   * Provided the transpose is swapping height and width dimension
  */
  tidl_InnerProductBTranspose (orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);
  tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);


  if(gParams.foldMaxPoolInConv2D == 1)
  {
    tidl_mergePoolingLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  }
  else if(gParams.foldDepthToSpaceInConv2D == 1)
  {
    tidl_mergeDepthToSpaceLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  }

  tidl_removeDivideByOneLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);
  tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);

  tidl_mergeBatchToSpaceInDWConvLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);
  tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);

  tidl_merge1x1MaxPoolingLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);

  tidl_merge1x1ResizeLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);

  tidl_mergeFlattenLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);

  tidl_mergeMinimumLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);

  tidl_mergeClipLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);

  tidl_convertRelUToBNLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  tidl_FindFlattenLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);

  tidl_convertConv2DToIpLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers, (sTIDL_tfOutReshapeMap_t *)&sTIDL_OutReshapeTable);
  tidl_mergeFalttenLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);

  tidl_mergeReshapeLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers, (sTIDL_tfOutReshapeMap_t *)&sTIDL_OutReshapeTable);
  tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);


  if (gParams.numBatches[0] > 1)
  {
    /* Check if all the layers in the network are supported for batch processing if not exit from import tool */
    for (int32_t i = 0; i < orgTIDLNetStructure.numLayers; i++)
    {
      if (TIDL_doesLayerSupportBatchProcessing(&orgTIDLNetStructure.TIDLPCLayers[i]) == -1)
      {
        printf("ERROR: Batch processing is not supported for this %s, so exiting from import \n", TIDL_LayerString[orgTIDLNetStructure.TIDLPCLayers[i].layerType]);
        exit(-1);
      }
    }
    if (gParams.enableHighResOptimization == 0) // check for ST is disabled
    {
      int32_t layerAdded = 0;
      int32_t layerIdx = 0;
      int32_t batchPadTotal = 1;
      do
      {
        layerAdded = tidl_addBatchReshapeLayerForBatchProcessing(orgTIDLNetStructure, &dataIndex, orgTIDLNetStructure.numLayers, &layerIdx, &batchPadTotal);
        if (layerAdded == -1)
        {
          printf("ERROR: Batch processing can not support for these set of Layers \n");
          exit(-1);
        }
        tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);
      }while(layerAdded);
      tidl_resetBatchPadValues(&orgTIDLNetStructure);
      tidl_initAndUpdateBatchPadRequirements(&orgTIDLNetStructure);
      tidl_updateOutDataShape(orgTIDLNetStructure, 0, orgTIDLNetStructure.numLayers, (sTIDL_tfOutReshapeMap_t *)&sTIDL_OutReshapeTable);
    }
    else
    {
      printf("ERROR: Batch processing is not supported with super tiling case \n");
      exit(-1);
    }
  }

  /*
   * Parsing SSD Structure
   * Steps:
   * 1. Merge Softmax Layer into DetectionOut Layer
   * 2. Merge Reshape Layer into DetectionOut Layer
   * 3. Handle width-wise Concat shape
   * 4. Remove Permute Layer
   * 5. Remove all Priorbox Layer, and send params to DetectionOut Layer.
   *    Search all concat layer with priorbox input and detection out output.
   *    Parse each priorbox in order.
   */
   if  (gParams.metaArchType == -1)
   {
      tidl_mergeDetectionoutLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
      tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);
   }

  tidl_addPadLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers, &dataIndex, &gParams);
  tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);

  if(gParams.foldPreBnConv2D)
  {
    tidl_mergePreBNLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
    tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);
  }

  if(true == tidl_isYuvLayerPresent())
  {
    tidl_mergeYUVtoRGBConversion(orgTIDLNetStructure, &dataIndex, &gParams);
    tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);
  }

  if ( (gParams.metaArchType == TIDL_metaArchTIDLSSD) ||
       (gParams.metaArchType == TIDL_metaArchTFSSD)   ||
       (gParams.metaArchType == TIDL_metaArchTFFasterRcnn) ||
       (gParams.metaArchType == TIDL_metaArchTIDLYolo) ||
       (gParams.metaArchType == TIDL_metaArchTIDLRetinaNet) ||
       (gParams.metaArchType == TIDL_metaArchTIDLYoloV5)    ||
       (gParams.metaArchType == TIDL_metaArchTIDLYoloV8)    ||
       (gParams.metaArchType == TIDL_metaArchTIDLCenterPoint)    ||
       (gParams.metaArchType == TIDL_metaArchTIDL3DOD)  ||
       (gParams.metaArchType == TIDL_metaArchTfliteRtSsdDetectionLayer)
       ) //TODO : can we just write (gParams.metaArchType != -1) instead of all above conditions
  {
    if(gParams.metaArchType == TIDL_metaArchTfliteRtSsdDetectionLayer)
    {
      numTFMetaLayers = 1;
    }

    int32_t numDetlayer = numTFMetaLayers;

    for(int32_t i = 0; i < numTFMetaLayers; i++)
    {
      tidl_addMetaArchLayersToNet(orgTIDLNetStructure, orgTIDLNetStructure.numLayers, &dataIndex, &gParams, i);
      tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);
      tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);
    }
  }

  if (gParams.metaArchType != TIDL_metaArchTIDLYoloV5)
  {
    tidl_removeConcatFlattenLayerInDetNet(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  }
  tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);
  tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);

  if(gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TFLITE_RT && gParams.metaArchType != -1)
  {
     tidl_AddTfODOutputLayers(orgTIDLNetStructure, orgTIDLNetStructure.numLayers, &dataIndex, gParams.metaArchType);
  }
  if(gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_ONNX_RT ||
     gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TVM_RELAY)
  {
    tidl_AddOnnxODOutputLayers(orgTIDLNetStructure, orgTIDLNetStructure.numLayers, &dataIndex);
  }
  tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);

  if (gParams.addDataConvertToNet)
  {
    if(false == tidl_isYuvLayerPresent())
    {
      tidl_addDataConvertLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers, &dataIndex, &gParams);
      tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);
      /*updating dataType after inserting dataConvertLayer to ensure the dataType changes are propagated to furteher layer*/
      tidl_updateOutDataShape(orgTIDLNetStructure, 0, orgTIDLNetStructure.numLayers, (sTIDL_tfOutReshapeMap_t *)&sTIDL_OutReshapeTable);
    }
    else
    {
      printf("Warning :: addDataConvertToNet is ignored as this feature is not supported for YUV inputs \n");
    }
  }

  layerIndex = orgTIDLNetStructure.numLayers;
  
  /*VTFR Transpose Handling:*/
  /*A: Support for optimized softmax:
      -> Softmax (axis=6) -> == -> Transpose(0,1->1,0) -> Softmax (axis=5) -> Transpose(1,0->0,1) */
  tidl_optimizeSoftmaxAxis(orgTIDLNetStructure, &dataIndex, orgTIDLNetStructure.numLayers);
  tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);
  /*End of transpose handling*/

  tidl_fuseTransposeMatMul(orgTIDLNetStructure, orgTIDLNetStructure.numLayers, &dataIndex);
  tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);

  
  /*Fuse bias to Matmul for asymmetric quantization (& devices which allow it):*/
  if (gParams.quantizationStyle == TIDL_QuantStyleAsymNP2 && gParams.numParamBits <= 8U)
  {
    tidl_FindMatMulBias (orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
    tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);
    tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);
    /*Layernorm Beta & Gamma optimization:*/
    tidl_fuseLayerNormBetaGamma(orgTIDLNetStructure, orgTIDLNetStructure.numLayers, &dataIndex);
    tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);
    tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);
    /**************************************/
  }

  tidl_mergeNoOpReshapeLayers (orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);
  tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);

  tidl_optimizeSqueezeLayers (orgTIDLNetStructure, &dataIndex, orgTIDLNetStructure.numLayers);
  tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);
  tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);
  
  tidl_optimizeTransposeLayers (orgTIDLNetStructure, &dataIndex, orgTIDLNetStructure.numLayers);

  bool isReshape = tidl_findReshapeinNetwork(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  if(isReshape)
  {
    tidl_addDataConvertLayerForOTFsupport(orgTIDLNetStructure, orgTIDLNetStructure.numLayers, &dataIndex, &gParams, padUnsupportedLayers);
    tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);
  }


  /*
   * Parsing Softmax/InnerProduct Structure
   * Steps:
   * 1. Change the shape of pooling output & IP input.
   * 2. Change the shape of pooling output & Softmax input.
   *
   * Support 2 formats:
   * 1. global_pooling: true
   * 2. 7x7 avg pooling with channelx7x7 blob
   */
  tidl_convertSoftMaxLayerInputShape(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);

  TIDL_convertDeconv2DtoConv(orgTIDLNetStructure, layerIndex);
  tidl_poolPadParams(orgTIDLNetStructure, layerIndex);

  TIDL_correctDeviceConfig(orgTIDLNetStructure);

  if(TIDL_isPadOTF(gParams.deviceName))
  {
    tidl_addDataConvertLayerForOTFsupport(orgTIDLNetStructure, orgTIDLNetStructure.numLayers, &dataIndex, &gParams, unsupportedLayersForOTF);
    tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);
  }

  if(TIDL_isPadOTF(gParams.deviceName))
  {
    tidl_addCropLayerForValidPadSupport(orgTIDLNetStructure, orgTIDLNetStructure.numLayers, &dataIndex, &gParams);
    tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);
  }

  if(tidl_doesNetworkHasMatmulLayer(orgTIDLNetStructure))
  {
    if(gParams.inferenceMode == TIDL_inferenceModeLowLatency && gParams.numCores > 1)
    {
      printf("\n Warning : TIDL_inferenceModeLowLatency is not supported with matmul layer, current execution is running without low latency mode, if you want to use multiple cores then set inferenceMode to TIDL_inferenceModeHighThroughput"); 
      gParams.inferenceMode = TIDL_inferenceModeDefault;
      gParams.numCores = 1;
    }
    else if(gParams.enableHighResOptimization == 1)
    {
      printf("\n Warning : High Resolution optimization is not available for matmul layer, current execution is running without high resolution optimization"); 
      gParams.enableHighResOptimization = 0;
    }
    else if(strcmp((char *)gParams.outputFeature16bitNamesList,"") != 0)
    {
      printf("\n ERROR: Mixed Precision is not supported with matmul layer");
      exit(-1);
    }
  }

  if(gParams.inferenceMode == TIDL_inferenceModeLowLatency && gParams.numCores > 1)
  {
    tidl_updateMultiCoreState(orgTIDLNetStructure, &gParams);
    /* IMP :: NOTE :: This function should be called after all layers addition/merge is complete, layer deletion post this state is till ok */
    tidl_addCropLayerForMultiCore(orgTIDLNetStructure, orgTIDLNetStructure.numLayers, &dataIndex, gParams.quantizationStyle, gParams.numCores);
    tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);
  }
  tidl_makeDataIdLayerIdSame(&orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  tidl_updateHighResOptimization(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  tidl_updateScratchMemoryRequirement(orgTIDLNetStructure);

  if(gParams.debugTraceLevel > 0)
  {
    printf("Completed tidl_optimizeNet \n");
  }
  layerIndex = orgTIDLNetStructure.numLayers;
}

int32_t updatePadAndWriteModel(sTIDL_OrgNetwork_t * pTidlOrigNet,
                                                             sTIDL_Network_t  * pTidlNet,
                                                             tidl_import_config * configParams)
{
  int32_t numLayers = pTidlOrigNet->numLayers;

  int32_t tiLayerIndex = tidl_copyPCNetToDeviceNet(pTidlOrigNet,
                                                                              pTidlNet,
                                                                              configParams,
                                                                              numLayers);

  tidl_addOutDataLayer(pTidlNet , tiLayerIndex);

  TIDL_fillDataBufPadRequirements(pTidlNet);
  tidl_updateNetPitch(pTidlNet);
  /* Offsets in the Net needs to be updated before writing the net to file */
  TIDL_writeModel(pTidlNet, pTidlOrigNet, (const char *)NULL, numLayers, configParams);
  TIDL_writeModel(pTidlNet, pTidlOrigNet, (const char *)(configParams)->outputNetFile, numLayers, configParams);
  TIDL_writeInfo(pTidlNet,  pTidlOrigNet,  (const char *)(configParams)->outputParamsFile, numLayers, 1, NULL, configParams);

  pTidlOrigNet->modelSizeWithMaxNC = tidl_getModelSize((configParams)->outputNetFile);

  return 0;
}

int32_t TIDL_doesLayerSupportBatchProcessing(sTIDL_LayerPC_t * layer)
{

  int32_t doesLayerSupportBatchProcessing = -1;

  if ( layer->layerType == TIDL_ConvolutionLayer )
  {
    doesLayerSupportBatchProcessing = 1;
  }
  else if ( layer->layerType == TIDL_PoolingLayer)
  {
    doesLayerSupportBatchProcessing = 1;
    if (layer->layerParams.poolParams.kernelW == 0)
      doesLayerSupportBatchProcessing = 0;
  }
  else if ( layer->layerType == TIDL_BatchNormLayer)
  {
    doesLayerSupportBatchProcessing = 1;
  }
  else if ( layer->layerType == TIDL_EltWiseLayer)
  {
    doesLayerSupportBatchProcessing = 1;
  }
  else if ( layer->layerType == TIDL_ConcatLayer)
  {
    doesLayerSupportBatchProcessing = 1;
  }
  else if ( layer->layerType == TIDL_InnerProductLayer)
  {
    doesLayerSupportBatchProcessing = 0;
  }
  else if ( layer->layerType == TIDL_SoftMaxLayer)
  {
    doesLayerSupportBatchProcessing = 0;
  }
  else if ( layer->layerType == TIDL_DataLayer)
  {
    doesLayerSupportBatchProcessing = 0;
  }
  else if ( layer->layerType == TIDL_ConstDataLayer)
  {
    doesLayerSupportBatchProcessing = 0;
  }
  else if ( layer->layerType == TIDL_BatchReshapeLayer)
  {
    doesLayerSupportBatchProcessing = 0;
  }
  else if ( layer->layerType == TIDL_ReshapeLayer)
  {
    doesLayerSupportBatchProcessing = 0;
  }
  else if ( layer->layerType == TIDL_TransposeLayer)
  {
    doesLayerSupportBatchProcessing = 0;
  }
  else if (layer->layerType == TIDL_SliceLayer)
  {
    /*
    check if slice axis is on batch
    */
    if (layer->layerParams.sliceParams.axis == TIDL_DIM_BATCH)
    {
      doesLayerSupportBatchProcessing = 0;
    }
    else
    {
      doesLayerSupportBatchProcessing = 1;
    }
  }
  else
  {
    /* Layers can't support batch Processing so exit from import */
    doesLayerSupportBatchProcessing = -1;
  }

  return doesLayerSupportBatchProcessing;
}

std::string TIDL_augmentCharArrayWithSuffix(char * oldArr, std::string augStr)
{
  string oldStr = oldArr;
  return (oldStr + augStr);
}

/** Used as part of algorithm for automating mixed precision
 * The algorithm requires the following to be done multiple times:
 * Quantize network (calibration)
 * Run inference to collect layer level trace data and perfsim performance
 * This function accepts "dumpTrace" as a parameter to indicate whether layer level traces are required or not
 * */
float TIDL_quantizeInferPerfsim(tidl_import_config * configParams, int32_t layerIndex, bool dumpTrace = false)
{
  TIDL_import_quantize(layerIndex);
  //dumpTrace is a boolean which which will signify whether we want to dump the trace of all layer
  // corresponding to the current mode(float,8bit , MP) for use in calculation of impact factors
  if(dumpTrace){
    configParams->writeTraceLevel = 3;
  }
  tidlRunQuantStatsTool(&orgTIDLNetStructure,
                                      &tIDLNetStructure,
                                      configParams,
                                      orgTIDLNetStructure.numLayers);
  float networkTime = tidlRunPerfSimTool(configParams);
  return networkTime;
}

void TIDL_restoreOrgNetAndConfigParams(sTIDL_OrgNetwork_t * orgTIDLNetStructureOrig, tidl_import_config * configParamsOrig)
{
  orgTIDLNetStructure = *orgTIDLNetStructureOrig;
  TIDL_allocAndCopyModelParams(&orgTIDLNetStructure, orgTIDLNetStructureOrig, orgTIDLNetStructure.numLayers);
  gParams = *configParamsOrig;
}


/*
This function returns the file name in the folder referenced by path
and contains word1 and word2 in the file name
*/
std::string TIDL_getFileNameContainingWords(std::string path,string word1,string word2){
#ifdef _WIN32
  return 0;
#else

  struct dirent *entry;
  DIR *dir = opendir((const char *)path.c_str());
  // directory doesn't exists hence returing empty string
  if(dir == NULL){
    return "";
  }
  std::string reqFileName = "";
  while((entry = readdir(dir)) != NULL){
    std::string fileName = entry->d_name;
    if(fileName.find(word1) != string::npos and fileName.find(word2) != string::npos){
      reqFileName = fileName;
      break;
    }
  }
  closedir(dir);
  return reqFileName;
#endif
}

/*
This function updates the gParams parameter as per the passed value
*/
void TIDL_updateConfigParameters(
  tidl_import_config * configParams,
  int32_t biasCalibrationIterations,
  int32_t numFramesBiasCalibration,
  int32_t calibrationOption,
  int32_t numFeatureBits,
  int32_t numParamBits,
  int32_t numFrames
) {
  /* change the configParams as specified as arguments to function */
  if(biasCalibrationIterations != -1)
  {
    if(biasCalibrationIterations < 1)
    {
      configParams->biasCalibrationIterations = 1;
      printf("Warning : Using only 1 iteration for automated mixed precision \n");
    }
    else
    {
      configParams->biasCalibrationIterations = biasCalibrationIterations;
    }
  }

  if(numFramesBiasCalibration != -1)
  {
    if(numFramesBiasCalibration < 1)
    {
      configParams->numFramesBiasCalibration = 1;
      printf("Warning : Using only 1 frame for automated mixed precision \n");
    }
    else
    {
      configParams->numFramesBiasCalibration = numFramesBiasCalibration;
    }
  }

  if(calibrationOption != -1){
    configParams->calibrationOption = calibrationOption;
  }

  if(numFeatureBits != -1){
    configParams->numFeatureBits = numFeatureBits;
  }

  if(numParamBits != -1){
    configParams->numParamBits = numParamBits;
  }

  if(numFrames != -1)
  {
    if(numFrames < 1)
    {
      configParams->numFrames = 1;
      printf("Warning : Using only 1 frame for automated mixed precision \n");
    }
    else
    {
      configParams->numFrames = numFrames;
    }
  }
}


/*
This function calculates the MSE Error from the data present in the float vectors
data1 and data2
*/
float TIDL_calculateMeanSquaredErrorByVectors(vector<vector<float>>& data1, vector<vector<float>>& data2){
  float mse = 0.0;
  vector<float> totalError(data1.size(), 0.0);
  for(int i = 0; i < data1.size(); i++)
  {
    for(int j = 0; j < data1[i].size(); j++)
    {
      totalError[i] += (data1[i][j] - data2[i][j]) * (data1[i][j] - data2[i][j]);
    }
    totalError[i] /= (float)(data1[i].size());
  }

  for(int i = 0; i < totalError.size(); i++)
  {
    mse += totalError[i];
  }
  mse = mse / (float)(totalError.size());

  return mse;
}

/*
This function calculates the standard deviation of an array of number
*/
float TIDL_calculateStandardDeviation(vector<float>& nums){
  float sum = 0.0;
  int n = nums.size();
  for(int i = 0; i < n; i++){
    sum += nums[i];
  }
  float mean = sum / n;
  float standardDeviation = 0.0;
  for(int i = 0; i < n; i++){
    standardDeviation += pow(nums[i] - mean,2);
  }
  standardDeviation = standardDeviation/n;
  standardDeviation = sqrt(standardDeviation);
  return standardDeviation;
}

float TIDL_calculateMeanAbsoluteErrorByVectorsHelper(vector<float>& x,vector<float>& y){
  float eps = 1e-5;
  float standardDeviation = TIDL_calculateStandardDeviation(x);
  vector<float> diff;
  for(int i = 0; i < x.size(); i++){
    float val = (abs(x[i] - y[i]) + eps) / (standardDeviation + eps);
    diff.push_back(val);
  }
  int cnt = 0;
  float sum = 0;
  for(int i = 0; i < diff.size(); i++){
    if(abs(diff[i]) < 6.0){
      sum += diff[i];
      cnt++;
    }
  }
  return sum / (float)cnt;
}

float TIDL_calculateMeanAbsoluteErrorByVectors(vector<vector<float>>& data1,vector<vector<float>>& data2){
  float error = 0.0;
  vector<float> totalError(data1.size(), 0.0);
  for(int i = 0; i < data1.size(); i++){
    totalError[i] = TIDL_calculateMeanAbsoluteErrorByVectorsHelper(data1[i],data2[i]);
  }
  for(int i = 0; i < totalError.size(); i++){
    error += totalError[i];
  }
  error = error / (float)(totalError.size());
  return error;
}

/** Calculate distance metric given input data in the form of vectors for specified metric argument
 * Currently supported metrics : (1) MSE = (sum ((x - y)^2)/n and (2) |x - y|/std(x)
*/
float TIDL_calculateErrorByVectors(vector<vector<float>> data1, vector<vector<float>> data2, string errorType){
  if(errorType == "MSE"){
    return TIDL_calculateMeanSquaredErrorByVectors(data1, data2);
  }
  else if(errorType == "MAE"){
    return TIDL_calculateMeanAbsoluteErrorByVectors(data1, data2);
  }
  else
  {
    printf("***** Distance metric specified not valid : Please provide valid distance metric\nProceeding with MSE metric \n");
    return TIDL_calculateMeanSquaredErrorByVectors(data1, data2);
  }
}

/** This function gets data from traces dumped for layers with dataId specified in outputLayerIndices
 * And normalizes the traces for further distance metric computation
 * */
vector<vector<float>> TIDL_readAndNormalizeTraces(string modePrefix,char outDirName[],vector<int>& outputLayerIndices)
{
  vector<vector<float>> values;
  for(int i = 0; i < (int)outputLayerIndices.size(); i++){
    int dataId = outputLayerIndices[i];
    string dataIdString = to_string(dataId);
    while(dataIdString.size() < 4){
      dataIdString = "0" + dataIdString;
    }
    dataIdString = modePrefix + dataIdString;
    string fname = TIDL_getFileNameContainingWords(outDirName,dataIdString,"_float.bin");
    string absFilePath = outDirName;
    absFilePath = absFilePath + "/" + fname;
    cout << absFilePath << endl;
    vector<float> value;
    float val;
    std::ifstream fin(absFilePath, std::ios::binary);
    while(fin.read(reinterpret_cast<char*>(&val),sizeof(float))){
      value.push_back(val);
    }
    values.push_back(value);
  }
  vector<float> errors;
  int minSize = INT_MAX;
  for(int i = 0; i < (int)values.size(); i++)
  {
    float maxValue = -(FLT_MAX);
    float minValue = FLT_MAX;
    for(int j = 0; j < (int)values[i].size(); j++)
    {
      maxValue = max(maxValue,values[i][j]);
      minValue = min(minValue,values[i][j]);
    }
    for(int j = 0; j < (int)values[i].size(); j++)
    {
      values[i][j] = (values[i][j] - minValue) / (maxValue - minValue);
    }
  }
  return values;
}

/** Main algorithm for automating mixed precision
 * Sort the impact factors in descending order
 * Keep adding the corresponding layers to 16 bit list sequentially till performance stays below limit specified by mixed precision factor
 * Return list of layers to be put in 16 bit
 * */
string TIDL_getLayersToKeepIn16BitByImpactFactors(sTIDL_OrgNetwork_t * orgTIDLNetStructureOrig,tidl_import_config * configParamsOrig,
                                                  vector<pair<float,int>>& impactFactors, uint32_t layerIndex,
                                                  float networkTimeFor8Bit, string outDirNameStr, string netFileName, string ioFileName)
{
  ofstream loggerFile;
  loggerFile.open(outDirNameStr + "/layers_selection.txt");
  loggerFile << "Information Regarding which layers are getting added" << endl;
  TIDL_restoreOrgNetAndConfigParams(orgTIDLNetStructureOrig, configParamsOrig);
  strcpy((char *)gParams.outputNetFile, netFileName.c_str());
  strcpy((char *)gParams.outputParamsFile, ioFileName.c_str());
  sort(impactFactors.begin(),impactFactors.end(),greater<pair<float,int>>());
  float mixedPrecisionFactor = gParams.mixedPrecisionFactor;
  loggerFile << "Mixed precision factor = " << mixedPrecisionFactor << endl;
  float cur_mixed_precision_factor = 0;
  int i = 0;
  std::string layers = (char *)gParams.outputFeature16bitNamesList;
  std::string layersOld = (char *)gParams.outputFeature16bitNamesList;
  std::string layerIdsIncluded = "";
  std::string layerIdsIncludedOld = "";
  int iteration = 0;
  while (i < impactFactors.size() && cur_mixed_precision_factor < mixedPrecisionFactor)
  {
    if(layers.find((char *)orgTIDLNetStructureOrig->TIDLPCLayers[impactFactors[i].second].outDataNames[0]) != string::npos){
      i++;
      continue;
    }
    if(layers.size() == 0){
      layers = (char *)orgTIDLNetStructureOrig->TIDLPCLayers[impactFactors[i].second].outDataNames[0];
      layerIdsIncluded = to_string(impactFactors[i].second);
    } else {
      layers += ",";
      layers += (char *)orgTIDLNetStructureOrig->TIDLPCLayers[impactFactors[i].second].outDataNames[0];
      layerIdsIncluded += ",";
      layerIdsIncluded += to_string(impactFactors[i].second);
    }
    strcpy((char *)gParams.outputFeature16bitNamesList,(char *)layers.c_str());
    TIDL_updateConfigParameters(&gParams,1,1,0,-1,-1,1); //Calibration option set to 0 since only perf data required, not accuracy
    float networkTime = TIDL_quantizeInferPerfsim(&gParams, layerIndex);
    cur_mixed_precision_factor = (networkTime / networkTimeFor8Bit);
    if(cur_mixed_precision_factor > mixedPrecisionFactor) /* Do not include current layer for mixed precision */
    {
      layers = layersOld;
      layerIdsIncluded = layerIdsIncludedOld;
    }
    else
    {
      layersOld = layers;
      layerIdsIncludedOld = layerIdsIncluded;
    }
    loggerFile << "data id of layer added in " << iteration << " = " << orgTIDLNetStructure.TIDLPCLayers[impactFactors[i].second].outData[0].dataId << endl;
    loggerFile << "data id of layer added in " << iteration << " = " << (char *)orgTIDLNetStructure.TIDLPCLayers[impactFactors[i].second].outDataNames[0] << endl;
    loggerFile << "overall added layers dataIds " << layerIdsIncluded << endl;
    loggerFile << "overall added layers names " << layers << endl;
    loggerFile << "current mixed precision factor " << cur_mixed_precision_factor << endl;
    i++;
    iteration++;
  }
  return layers;
}


/** Specify layer at which distance metric to be calculated
 * Classification : Layer before softmax (If softmax last layer of network)
 * Object detection : Convolution heads - inputs of Detection output layer
 * Segmentation : Layer before argmax layer (if argmax last layer of network)
 * Else, use last layer of network
 * */
std::vector<int32_t> TIDL_getLayerIndicesToBeUsedForCalculatingDistanceMetric(int32_t layerIndex)
{
  vector<int> outputLayerIndices;
  for(int i = orgTIDLNetStructure.numLayers - 1; i >= 0; i--)
  {
    int32_t  idx_next;

    /* Check for OD networks */
    if(orgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_DetectionOutputLayer)
    {
      int32_t idx_next = tidl_getOutLayer(orgTIDLNetStructure, layerIndex, orgTIDLNetStructure.TIDLPCLayers[i].outData[0].dataId);
      sTIDL_LayerPC_t &nextLayer = orgTIDLNetStructure.TIDLPCLayers[idx_next];
      if(((nextLayer.layerType == TIDL_DataLayer) && (nextLayer.numOutBufs == -1)) || (nextLayer.layerType == TIDL_OdOutputReformatLayer))
      {
        for(int j = 0; j < orgTIDLNetStructure.TIDLPCLayers[i].numInBufs; j++)
        {
          outputLayerIndices.push_back(orgTIDLNetStructure.TIDLPCLayers[i].inData[j].dataId);
        }
      }
    }
    /* Check for classification networks */
    if(orgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_SoftMaxLayer)
    {
      idx_next = tidl_getOutLayer(orgTIDLNetStructure, layerIndex, orgTIDLNetStructure.TIDLPCLayers[i].outData[0].dataId);
      sTIDL_LayerPC_t &nextLayer = orgTIDLNetStructure.TIDLPCLayers[idx_next];
      if((nextLayer.layerType == TIDL_DataLayer) && (nextLayer.numOutBufs == -1))
      {
        for(int j = 0; j < orgTIDLNetStructure.TIDLPCLayers[i].numInBufs; j++)
        {
          outputLayerIndices.push_back(orgTIDLNetStructure.TIDLPCLayers[i].inData[j].dataId);
        }
      }
    }
    /* Check for segmentation networks */
    if(orgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_ArgMaxLayer)
    {
      idx_next = tidl_getOutLayer(orgTIDLNetStructure, layerIndex, orgTIDLNetStructure.TIDLPCLayers[i].outData[0].dataId);
      sTIDL_LayerPC_t &nextLayer = orgTIDLNetStructure.TIDLPCLayers[idx_next];
      if((nextLayer.layerType == TIDL_DataLayer) && (nextLayer.numOutBufs == -1))
      {
        for(int j = 0; j < orgTIDLNetStructure.TIDLPCLayers[i].numInBufs; j++)
        {
          outputLayerIndices.push_back(orgTIDLNetStructure.TIDLPCLayers[i].inData[j].dataId);
        }
      }
    }
  }
  if(outputLayerIndices.size() == 0) //None of above present, then use all the output layers of network for distance metric computation
  {
    for(int i = 0; i < orgTIDLNetStructure.numLayers; i++)
    {
      int32_t  idx_next = tidl_getOutLayer(orgTIDLNetStructure, layerIndex, orgTIDLNetStructure.TIDLPCLayers[i].outData[0].dataId);
      sTIDL_LayerPC_t &nextLayer = orgTIDLNetStructure.TIDLPCLayers[idx_next];
      if((nextLayer.layerType == TIDL_DataLayer) && (nextLayer.numOutBufs == -1))
      {
        outputLayerIndices.push_back(i);
      }
    }
  }
  return outputLayerIndices;
}

/** This function computes the distance metric and perfsim performance time needed for computing impact factors
 * Returns the distance metric and networkTime
*/
float TIDL_getDistanceAndPerfData(sTIDL_OrgNetwork_t * orgTIDLNetStructureOrig, tidl_import_config * configParamsOrig, std::string modeSuffix,
                               char inConfigFileNameOrig[], int32_t layerIndex, char outDirName[], std::string netFileName, std::string ioFileName, std::vector<int32_t> outputLayerIndices,
                               vector<vector<float>> error32BitVector, float * networkTime,int i)
{
  TIDL_restoreOrgNetAndConfigParams(orgTIDLNetStructureOrig, configParamsOrig);
  /* Restore net and io names set in main function for auto mixed precision */
  strcpy((char *)gParams.outputNetFile, netFileName.c_str());
  strcpy((char *)gParams.outputParamsFile, ioFileName.c_str());

  strcpy(inConfigFilename, TIDL_augmentCharArrayWithSuffix(inConfigFileNameOrig, modeSuffix).c_str());
  TIDL_updateConfigParameters(&gParams, gParams.biasCalibrationIterations/4, gParams.numFramesBiasCalibration/4, 7, 8, 8, gParams.numFramesBiasCalibration/4);
  if(i != -1){
    strcpy((char *)gParams.outputFeature16bitNamesList, (char *)orgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0]);
  }

  *networkTime = TIDL_quantizeInferPerfsim(&gParams, layerIndex, true);

  // get distance metric
  vector<vector<float>> errorVector = TIDL_readAndNormalizeTraces(modeSuffix,outDirName,outputLayerIndices);
  float distance = TIDL_calculateErrorByVectors(error32BitVector,errorVector,"MSE");

  return distance;
}

/** Main function for performing automated mixed precision */
void TIDL_executeAutomatedMixedPrecision(uint32_t layerIndex, sTIDL_OrgNetwork_t * orgTIDLNetStructureOrig, tidl_import_config * configParamsOrig)
{
  /* Check if value of mixed precision factor is valid */
  if(gParams.mixedPrecisionFactor <= 1)
  {
    printf("\n\n**** WARNING : Mixed precision factor <= 1 is not a valid argument, proceeding without automated mixed precisin **** \n\n");
    return;
  }

  std::vector<int32_t> outputLayerIndices = TIDL_getLayerIndicesToBeUsedForCalculatingDistanceMetric(layerIndex);

  /* Take backup of inConfigFilename */
  char inConfigFileNameOrig[2*FILE_NAME_SIZE];
  strcpy(inConfigFileNameOrig, inConfigFilename);

  char outDirName[500];

  /** Create a separate folder for saving auto mixed precision related artifacts */
  strcpy(outDirName, (char *)gParams.outputNetFile);
  strcat(outDirName, "_autoMixedPrecArtifacts");

  std::string outDirNameStr(outDirName);
  system(("mkdir " + outDirNameStr).c_str());

  /* Update net and params file names to be inside the above create auto mixed precision folder
     Since the above folder is specific to the model, it is ok not to have model name as part of net and params file names */
  system(("touch " + outDirNameStr + "/tidl_net_model.bin").c_str()); /* getAbsPath requires actual file to be present in folder */
  strcpy((char *)gParams.outputNetFile, (outDirNameStr + "/tidl_net_model.bin").c_str());
  strcpy((char *)gParams.outputParamsFile, (outDirNameStr + "/tidl_io_model_").c_str());
  std::string netFileName = (char *)gParams.outputNetFile;
  std::string ioFileName = (char *)gParams.outputParamsFile;


  ofstream loggerFile;
  loggerFile.open(outDirNameStr + "/impact_factor_calculation.txt");
  /***** Run entire model in FLOATING POINT mode *****/

  strcpy(inConfigFilename, TIDL_augmentCharArrayWithSuffix(inConfigFileNameOrig, "_float").c_str());
  sTIDL_Network_t * tidlNetStructureFloat = new sTIDL_Network_t;
  TIDL_updateConfigParameters(&gParams,-1,-1,-1,-1,-1,gParams.numFramesBiasCalibration/4);
  gParams.writeTraceLevel = 3;
  TIDL_quantStatsFixedOrFloat(&orgTIDLNetStructure,
                                  tidlNetStructureFloat,
                                  &gParams,
                                  STATS_COLLECTION_FLOAT);
  vector<vector<float>> error32BitVector = TIDL_readAndNormalizeTraces("_float",outDirName,outputLayerIndices);
  delete tidlNetStructureFloat;

  /****** Run model with ALL LAYERS IN 8 BITS *****/

  std::string modeSuffix = "_8bit";
  float networkTimeFor8Bit = 0.0;
  float errorIn8Bit = TIDL_getDistanceAndPerfData(orgTIDLNetStructureOrig, configParamsOrig, modeSuffix, inConfigFileNameOrig, layerIndex,
                            outDirName, netFileName, ioFileName, outputLayerIndices, error32BitVector, &networkTimeFor8Bit,-1);

  loggerFile << "MODE: 8 bit" << endl;
  loggerFile << "Error = " << errorIn8Bit << endl;
  loggerFile << "-----------------------------------------------" << endl;

  /***** Loop over all layers to run mixed precision with individual layers in 16 bit precision ****/

  vector<pair<float,int>> impactFactors;
  loggerFile << "Mixed Precision Mode " << endl;
  for(int i = 0; i < orgTIDLNetStructure.numLayers; i++)
  {
    if((orgTIDLNetStructure.TIDLPCLayers[i].layerType != TIDL_DataLayer) && (orgTIDLNetStructure.TIDLPCLayers[i].layerType != TIDL_DataConvertLayer)) // don't set data layer to 16 bit precision
    {
      std::string modeSuffix = "_MP_layer_" + to_string(i);
      float networkTimeForMPLayer = 0.0;
      float errorInMPLayer = TIDL_getDistanceAndPerfData(orgTIDLNetStructureOrig, configParamsOrig, modeSuffix, inConfigFileNameOrig, layerIndex,
                                outDirName, netFileName, ioFileName, outputLayerIndices, error32BitVector, &networkTimeForMPLayer,i);
      //Calculate impact factor for layer i
      float eps = 1e-5;
      float impactFactor = (errorIn8Bit - errorInMPLayer + eps)/ ((networkTimeForMPLayer - networkTimeFor8Bit + eps) * 1e-6); // network time converted to seconds
      loggerFile << "Data Id = " << orgTIDLNetStructure.TIDLPCLayers[i].outData[0].dataId << endl;
      loggerFile << "Layer Name = " << (char *)orgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0] << endl;
      loggerFile << "Error = " << errorInMPLayer << endl;
      loggerFile << "Impact factor = " << impactFactor << endl;
      loggerFile << "--------------------------------------------------" << endl;
      impactFactors.push_back(make_pair(impactFactor,i));
    }
  }

  //Algorithm to find layers to be put in 16 bit
  string layersToKeepIn16Bit = TIDL_getLayersToKeepIn16BitByImpactFactors(orgTIDLNetStructureOrig, configParamsOrig, impactFactors,
                                                                            layerIndex, networkTimeFor8Bit, outDirNameStr, netFileName, ioFileName);
  /* Restore original model/parameters/variables for subsequent import */
  TIDL_restoreOrgNetAndConfigParams(orgTIDLNetStructureOrig, configParamsOrig);

  // Restore the inConfigFilename
  strcpy(inConfigFilename, inConfigFileNameOrig);

  //Populate layers to be put in 16 bit for subsequent final import
  strcpy((char *)gParams.outputFeature16bitNamesList,(char *)layersToKeepIn16Bit.c_str());

  // Deletes all the artifacts of automated mixed precision algorithm
  printf("Removing mixed precision artifacts\n");
  std:string configFileName = getFileNameFromPath(inConfigFileNameOrig);
  /* Copy log files to another location before deleting artifacts */
  system(("mv " + outDirNameStr + "/layers_selection.txt " + outDirNameStr + "/../" + configFileName + "_layers_selection.txt").c_str());
  system(("mv " + outDirNameStr + "/impact_factor_calculation.txt " + outDirNameStr + "/../" + configFileName + "_impact_factor_calculation.txt").c_str());
  /* Delete the auto mixed prec artifacts folder */
  system(("rm -rf " + outDirNameStr).c_str());
}

int32_t mergereshapeLayers(sTIDL_OrgNetwork_t& orgTIDLNetStructure)
{
  /*
   * This function merges consecutive reshape layers, only the last reshape has effect, so it removes any
   * prior reshape layers if present.
  */
  int32_t numLayers = orgTIDLNetStructure.numLayers;
  int32_t inIdx = 0, innerIdx = 0, merged = 0;
  for(int32_t i=0;i<numLayers;i++)
  {
    if(orgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_ReshapeLayer)
    {
      inIdx = tidl_getInLayer(orgTIDLNetStructure, numLayers, orgTIDLNetStructure.TIDLPCLayers[i].inData[0].dataId);
      if(orgTIDLNetStructure.TIDLPCLayers[inIdx].layerType == TIDL_ReshapeLayer)
      {
        /* Consecutive reshape layers can be merged */
        innerIdx = tidl_getInLayer(orgTIDLNetStructure, numLayers, orgTIDLNetStructure.TIDLPCLayers[inIdx].inData[0].dataId);
        if(innerIdx != -1)
        {
          orgTIDLNetStructure.TIDLPCLayers[i].inData[0].dataId = orgTIDLNetStructure.TIDLPCLayers[innerIdx].outData[0].dataId;
          orgTIDLNetStructure.TIDLPCLayers[inIdx].numInBufs = -1;
          orgTIDLNetStructure.TIDLPCLayers[inIdx].numOutBufs = -1;
          merged = 1;
        }
      }
    }
  }
  return merged;
}

int32_t tidl_checkIODCLayers(int32_t dataConvertIdx, int32_t numLayers)
{
  /** Do not remove the First and Last DataConvert Layers*/
  int32_t IOCheck = 1;
  if (dataConvertIdx == 1 || dataConvertIdx == numLayers - 2)
  {
    IOCheck = 0;
  }
  return IOCheck;
}
int32_t tidl_checkLayoutForDCLayers(sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t dataConvertIdx)
{
  /** Remove the DC Layers if there is no data movement because of the layout change*/
  int32_t layoutCheck = 1, notOne = 0;
  sTIDL_LayerPC_t& layer = pOrgTIDLNetStructure.TIDLPCLayers[dataConvertIdx];

  if (layer.layerParams.dataConvertParams.layout != layer.layerParams.dataConvertParams.outLayout)
  {
    /* remove dataconvert if input is flattened */
    for (int32_t i=1;i<TIDL_DIM_MAX;i++)
    {
      if (layer.inData[0].dimValues[i] != 1 )
        notOne++;
    }
    if (notOne > 1)
    {
      layoutCheck = 0;
    }
  }
  return layoutCheck;
}
int32_t tidl_checkPadForDCLayers(sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t dataConvertIdx, sTIDL_Network_t *tidlNet)
{
  /** Remove the DataConvert Layers if there is no Pad Addition/Removal*/
  int32_t padCheck = 1;
  int32_t outDataId = pOrgTIDLNetStructure.TIDLPCLayers[dataConvertIdx].outData[0].dataId;
  int32_t tidlLayerIdx = -1;
  for(int32_t i=0;i<tidlNet->numLayers;i++)
  {
    if(tidlNet->TIDLLayers[i].outData.dataId == outDataId)
    {
      tidlLayerIdx = i;
      break;
    }
  }
  if (tidlLayerIdx == -1)
  {
    printf("Cannot find equivalent data convert in tidlNet\n");
  }
  int32_t inLayerIdx  = tidl_getInLayer(tidlNet, tidlNet->numLayers, tidlNet->TIDLLayers[tidlLayerIdx].inData[0]);
  int32_t outLayerIdx = tidl_getOutLayer(tidlNet, tidlNet->numLayers, tidlNet->TIDLLayers[tidlLayerIdx].outData.dataId);
  //int32_t inPad  = tidlNet->TIDLLayers[inLayerIdx].outData.padH != 0 || tidlNet->TIDLLayers[inLayerIdx].outData.padW != 0;
  //int32_t outPad = tidlNet->TIDLLayers[outLayerIdx].outData.padH != 0 || tidlNet->TIDLLayers[outLayerIdx].outData.padW != 0;
  bool HPad =  tidlNet->TIDLLayers[inLayerIdx].outData.padH != tidlNet->TIDLLayers[outLayerIdx].outData.padH;
  bool WPad =  tidlNet->TIDLLayers[inLayerIdx].outData.padW != tidlNet->TIDLLayers[outLayerIdx].outData.padW;
  if (HPad || WPad)
  {
    padCheck = 0;
  }
  return padCheck;
}
bool TIDL_removeDCLayersfromPCNet(sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, tidl_import_config *params)
{
  bool removed = false;
  int32_t numLayers = pOrgTIDLNetStructure.numLayers, outLayerIdx;
  FILE * fp;
  int64_t modelSize = tidl_getModelSize(params->outputNetFile);

  sTIDL_Network_t * tidlNet = (sTIDL_Network_t *)malloc(modelSize);
  if(tidlNet == NULL)
  {
    printf("Could not Allocate memory for model read\n");
    return 0;
  }
  fp = fopen((const char *)params->outputNetFile, "rb");
  if (fp)
  {
    fread(tidlNet, 1, modelSize, fp);
    fclose(fp);
  }
  else
  {
    printf("Could not open %s\n", params->outputNetFile);
    return 0;
  }

  for (int32_t i=0;i<numLayers;i++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_DataConvertLayer)
    {
      sTIDL_LayerPC_t& layer = pOrgTIDLNetStructure.TIDLPCLayers[i];
      if (layer.layerParams.dataConvertParams.type == TIDL_DC_TYPE_INTERMEDIATE)
      {
        int32_t padCheck=0, layoutCheck=0, IOCheck = 0;

        padCheck    = tidl_checkPadForDCLayers(pOrgTIDLNetStructure, i, tidlNet);
        layoutCheck = tidl_checkLayoutForDCLayers(pOrgTIDLNetStructure, i);
        IOCheck     = tidl_checkIODCLayers(i, numLayers);

        if(padCheck && layoutCheck && IOCheck)
        {
          outLayerIdx = tidl_getOutLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, layer.outData[0].dataId);
          do
          {
            for(int j=0;j<pOrgTIDLNetStructure.TIDLPCLayers[outLayerIdx].numInBufs;j++)
            {
              if(pOrgTIDLNetStructure.TIDLPCLayers[outLayerIdx].inData[j].dataId == layer.outData[0].dataId)
              {
                pOrgTIDLNetStructure.TIDLPCLayers[outLayerIdx].inData[j].dataId = layer.inData[0].dataId;
              }
            }
            outLayerIdx = tidl_getOutLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, layer.outData[0].dataId);
          } while (outLayerIdx != -1);

          removed = true;
          layer.numInBufs = -1;
          layer.numOutBufs = -1;
        }
      }
    }
  }

  if (removed == true)
  {
    tidl_removeMergedLayersFromNet(&pOrgTIDLNetStructure, &tempTIDLNetStructure);
    tidl_makeDataIdLayerIdSame(&pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers);
    numLayers = pOrgTIDLNetStructure.numLayers;
    updatePadAndWriteModel(&pOrgTIDLNetStructure, tidlNet, params);
  }

  int32_t merged = mergereshapeLayers(pOrgTIDLNetStructure);
  if(merged)
  {
    tidl_removeMergedLayersFromNet(&pOrgTIDLNetStructure, &tempTIDLNetStructure);
    tidl_makeDataIdLayerIdSame(&pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers);
    numLayers = pOrgTIDLNetStructure.numLayers;
    updatePadAndWriteModel(&pOrgTIDLNetStructure, tidlNet, params);
  }
  return removed;
}

// This function post-processes the network after it has been imported and optimized.
uint32_t TIDL_import_backend(uint32_t layerIndex)
{
  /* At this point model is frozen */
  int32_t i;
  /* If user has not set any value for raw element type then use the default
  which is same as input element type */
  for ( i = 0; i < TIDL_MAX_ALG_IN_BUFS; i++)
  {
    if ( ( gParams.inFileFormat == 1 ) || ( gParams.inFileFormat == 3 ) )
    {
      if ( gParams.rawDataInElementType[i] == -1 )
      {
        gParams.rawDataInElementType[i]   = gParams.inElementType[i];
        if(gParams.debugTraceLevel > 0)
        {
          printf("Raw data format used is same as inElementType %d \n", gParams.inElementType[i]);
        }
      }
    }
  }

  /* Below automated mixed precision algorithm finds the layers to be computed in 16 bits and sets as part of gParams.outputFeature16bitNamesList
   so final import is done with this setting */

  if(gParams.numParamBits < 16)
  /** Automated mixed precision not needed if running floating point mode -
   * this is protected to prevent running MP when doing subgraph input/output layers data for OSRT
   * */
  {
    if(gParams.mixedPrecisionFactor != -1) /* automated mixed precision not executed if mixedPrecisionFactor is not defined i.e. -1 */
    {
      /* Take backup of original gParams and orgTIDLNetStructure before running automated mixed precision pass */
      tidl_import_config configParamsOrig = gParams;
      sTIDL_OrgNetwork_t * orgTIDLNetStructureOrig = new sTIDL_OrgNetwork_t;
      *orgTIDLNetStructureOrig = orgTIDLNetStructure;
      TIDL_allocAndCopyModelParams(orgTIDLNetStructureOrig, &orgTIDLNetStructure, orgTIDLNetStructure.numLayers);

      /* Execute the algorithm */
      TIDL_executeAutomatedMixedPrecision(layerIndex, orgTIDLNetStructureOrig, &configParamsOrig);

      delete orgTIDLNetStructureOrig;
    }
    TIDL_importBitDepthProtoTxt(&orgTIDLNetStructure, &gParams);
  }

  /* Run quantization and perfsim based on modified gParams */
  TIDL_import_quantize(layerIndex);
  int32_t dimcheck = tidlModelTensorDimCheck(orgTIDLNetStructure);

  /* Save calib run's copy.  perfsim run could update network layer ordering */
  if (gParams.modelType == 4 /* TVM Relay */)
    tidlWriteTensorNamesToFile(&gParams, "_calib");
  if(gParams.executeNetworkCompiler && dimcheck)
  {
    tidlRunPerfSimTool(&gParams);
    if(((gParams.deviceName & TIDL_OTF_FLAG_BIT) == TIDL_OTF_FLAG_BIT) && gParams.enableOtfPad == 0)
    {
      printf("Rerunning network compiler\n");
      gParams.deviceName &= 0xFF;
      tidlRunPerfSimTool(&gParams);
    }
    if(TIDL_removeDCLayersfromPCNet(orgTIDLNetStructure, &gParams))
    {
      //Removed Data Convert Layer, rerunning NC
      printf("Rerunning network compiler\n");
      tidlRunPerfSimTool(&gParams);
    }
  }
  tidlWriteTensorNamesToFile(&gParams, "");
  tidlRunGraphVizTool(&gParams);
  if (gParams.debugTraceLevel > 0)
     tidlRunModelDumpTool(&gParams);

  int errorCount = tidlModelCheck(&gParams, &orgTIDLNetStructure);
  TIDL_freeModelParams(&orgTIDLNetStructure, layerIndex);

  return errorCount;
}
