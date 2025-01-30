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


#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/message.h>
#include <google/protobuf/text_format.h>
#if defined(GCC_BUILD)
#include <unistd.h>
#define O_BINARY (0)
#endif
#if defined(MSVC_BUILD)
#include <io.h>
#endif
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <vector>
#include <float.h>
#include <sys/stat.h>

using namespace std;
using ::google::protobuf::Message;
using ::google::protobuf::io::FileInputStream;
using ::google::protobuf::io::FileOutputStream;
using ::google::protobuf::io::ZeroCopyInputStream;
using ::google::protobuf::io::CodedInputStream;
using ::google::protobuf::io::ZeroCopyOutputStream;
using ::google::protobuf::io::CodedOutputStream;

#include "ti_dl.h"
#include "onnx/onnx-ml.proto3.pb.h"
#include "tidl_import_api.h"
#include "tidl_import_quantize.h"
#include "tidl_import_config.h"
#include "tidl_import_common.h"
#include "tidl_import_quantize.h"
#include "perfsim.h"
#include "tidl_import_common_tidlrt_osrt.h"
#include "gc.h"
#include "gc_helper.h"

#define TIDL_MSMC_NUM_PHY_BANKS ( 4U)
#define TIDL_MSMC_PHY_BANK_SIZE (256U)
#define TIDL_MSMC_BANK_PITCH (TIDL_MSMC_NUM_PHY_BANKS * TIDL_MSMC_PHY_BANK_SIZE)
#define TIDL_MSMC_CACHE_LINE_SIZE (128U)
#define ALIGN_SIZE(x,y)       ((((x) + ((y)-1)) / (y)) * (y))
#define TIDL_DC_FOR_PAD_REMOVAL (int32_t(1))
#define TIDL_DC_FOR_PAD_ADDITION (int32_t(2))
#define TIDL_NO_DC (int32_t(3))
#define TIDL_NON_LIN_PARAM_SCALE (int32_t(1<<24))
#define TIDL_PATCH_EMBEDDING_RESHAPES (int32_t(3))
#define TIDL_PATCH_EMBEDDING_TRANSPOSES (int32_t(2))
#define TIDL_APPEND_NAME (int32_t(16))

extern sTIDL_OrgNetwork_t      orgTIDLNetStructure;
extern sTIDL_OrgNetwork_t      tempTIDLNetStructure;
extern sTIDL_Network_t         tIDLNetStructure;
extern sTIDL_multiCoreBatchProcessing multiCoreBatchProcInfo;
extern float *tidlPriorPtrs[TIDL_MAX_TF_SSD_LAYERS];
extern sTIDL_DetectOutputParams_t tidl_OdPostProc[TIDL_MAX_TF_SSD_LAYERS];
extern uint8_t tidl_OdBoxInputNames[TIDL_MAX_TF_SSD_LAYERS][TIDL_MAX_ALG_OUT_BUFS][TIDL_STRING_SIZE];
extern uint8_t tidl_OdClassInputNames[TIDL_MAX_TF_SSD_LAYERS][TIDL_MAX_ALG_OUT_BUFS][TIDL_STRING_SIZE];
extern uint8_t tidl_OdDirInputNames[TIDL_MAX_TF_SSD_LAYERS][TIDL_MAX_ALG_OUT_BUFS][TIDL_STRING_SIZE];
extern uint8_t tidl_OdOutputNames[TIDL_MAX_TF_SSD_LAYERS][TIDL_STRING_SIZE];
extern std::string tidl_OdFramework;
const char * TIDL_LayerString[] =
{
"TIDL_DataLayer",
"TIDL_ConvolutionLayer",
"TIDL_PoolingLayer",
"TIDL_ReLULayer",
"TIDL_PReLULayer",
"TIDL_EltWiseLayer",
"TIDL_InnerProductLayer",
"TIDL_SoftMaxLayer",
"TIDL_BatchNormLayer",
"TIDL_BiasLayer",
"TIDL_ScaleLayer",
"TIDL_Deconv2DLayer",
"TIDL_ConcatLayer",
"TIDL_SplitLayer",
"TIDL_SliceLayer",
"TIDL_CropLayer",
"TIDL_FlattenLayer",
"TIDL_DropOutLayer",
"TIDL_ArgMaxLayer",
"TIDL_DetectionOutputLayer",
"TIDL_ShuffleChannelLayer" ,
"TIDL_ResizeLayer" ,
"TIDL_RoiPoolingLayer",
"TIDL_OdPostProcessingLayer",
"TIDL_DepthToSpaceLayer",
"TIDL_SigmoidLayer",
"TIDL_PadLayer" ,
"TIDL_ColorConversionLayer",
"TIDL_OdOutputReformatLayer",
"TIDL_DataConvertLayer",
"TIDL_CustomLayer          ",
"TIDL_BatchReshapeLayer    ",
"TIDL_ReduceLayer",
"TIDL_ScatterElementsLayer",
"TIDL_SqueezeLayer" ,
"TIDL_TanhLayer",
"TIDL_HardSigmoidLayer",
"TIDL_ELULayer",
"TIDL_ReshapeLayer" ,
"TIDL_ConstDataLayer"   ,
"TIDL_GatherLayer" ,
"TIDL_TransposeLayer" ,
"TIDL_LayerNormLayer",
"TIDL_UnsupportedLayer" ,
"TIDL_PriorBoxLayer" ,
"TIDL_PermuteLayer" ,
"TIDL_ShapeLayer" ,
"TIDL_CLipLayer" ,
"TIDL_MinimumLayer" ,
"TIDL_LeakyReluLayer",
"TIDL_IdentityLayer",
"TIDL_BatchToSpaceLayer",
"TIDL_SpaceToBatchLayer",
"TIDL_PackLayer"        ,
"TIDL_DequantizeLayer"  ,
"TIDL_QuantizeLayer"    ,
"TIDL_SqrtLayer",
"TIDL_ReduceMeanLayer",
"TIDL_PowLayer",
"TIDL_DivLayer",
"TIDL_SubLayer",
"TIDL_PatchMergeLayer",
"TIDL_CastLayer"
};



static int totalMemAllocation = 0;
static int addedLayers = 0;
FILE *fpAlloc = NULL;
void * my_malloc(int size)
{
  void *ptr;
  //if(fpAlloc == NULL) fpAlloc = fopen ("MemAllocation.txt", "w");
  totalMemAllocation += size;
  ptr = malloc(size);
  assert(ptr != NULL);
  if(ptr==NULL)
  {
    printf("MALLOC FAILED TO ALLOCATE BUFFER OF SIZE %d\n",size);
  }

  //fprintf(fpAlloc, "Alloc: Ptr: %0x, Size: %0x\n",ptr,size);
  //fflush(fpAlloc);
  return ptr;
}

void my_free(void *ptr)
{
  //fprintf(fpAlloc, "Free: Ptr: %0x\n",ptr);
  //fflush(fpAlloc);
  free(ptr);
}

bool TIDL_readProtoFromTextFile(const char* fileName, Message* proto)
{
  int32_t           fd;
  bool              success;
  FileInputStream   *input;

  fd = open(fileName, O_RDONLY);
  if (fd == NULL)
  {
    printf("ERROR: Could not open prototext file for reading \n");
    exit(-1);
  }
  input = new FileInputStream(fd);
  success = google::protobuf::TextFormat::Parse(input, proto);
  delete input;
  close(fd);
  if (!success)
  {
    printf("ERROR: google::protobuf::TextFormat::Parse proto file(%s) FAILED !!!\n", fileName);
  }
  return success;
}

#define APP_CNN_INTEROP_CAFFE_READ_BINARY_TOTAL_BYTE_LIMIT  2147483647
#define APP_CNN_INTEROP_CAFFE_READ_BINARY_WARNING_THRESHOLD 1073741824

bool TIDL_readProtoFromBinaryFile(const char* fileName, Message* proto)
{
  int                   fd;
  ZeroCopyInputStream   *rawInput;
  CodedInputStream      *codedInput;
  bool                  success;

  fd = open(fileName, O_BINARY);
  if (fd == NULL)
  {
    printf("ERROR: Could not open caffe model(%s) for reading \n", fileName);
    exit(-1);
  }

  rawInput = new FileInputStream(fd);
  codedInput = new CodedInputStream(rawInput);

  codedInput->SetTotalBytesLimit(
    APP_CNN_INTEROP_CAFFE_READ_BINARY_TOTAL_BYTE_LIMIT
    );

  success = proto->ParseFromCodedStream(codedInput);
  delete codedInput;
  delete rawInput;
  close(fd);
  if (!success)
  {
    printf("ERROR: proto->ParseFromCodedStream proto file(%s)\n", fileName);
  }
  return success;
}
#if 0 // Code to read ResNet_mean.binaryproto"
BlobProto       blob;
TIDL_readProtoFromBinaryFile((const char *)params->inputParamsFile, &blob);
float*blob_data = (float*)my_malloc(sizeof(float)*blob.data_size());

for (i = 0; i < blob.data_size(); i++)
{
  blob_data[i] = blob.data(i);
}
fp1 = fopen((const char *)params->outputNetFile, "wb+");
if (fp1 == NULL)
{
  printf("Could not open %s file for writing \n", (const char *)params->outputNetFile);
}
fwrite(blob_data, 1, sizeof(float)*blob.data_size(), fp1);
if (fp1 != NULL)
{
  fclose(fp1);
}
exit(0);
#endif

int32_t TIDL_getLayerIdx(sTIDL_OrgNetwork_t * pOrgTIDLNetStructure, int32_t numLayer, const char *bufName)
{
  int32_t i, j;
  for (i = (numLayer - 1); i >= 0; i--)
  {
    for (j = 0; j < pOrgTIDLNetStructure->TIDLPCLayers[i].numOutBufs; j++)
    {
      if (strcmp((const char*)bufName, (const char*)pOrgTIDLNetStructure->TIDLPCLayers[i].outDataNames[j]) == 0)
      {
        return i;
      }
    }
  }
  return (-1);
}

void tidl_setLRTBpad(sTIDL_LayerPC_t &TIDLPCLayersOut)
{
  int32_t outWidth = TIDLPCLayersOut.outData[0].dimValues[TIDL_DIM_WIDTH], outHeight = TIDLPCLayersOut.outData[0].dimValues[TIDL_DIM_HEIGHT], inWidth = TIDLPCLayersOut.inData[0].dimValues[TIDL_DIM_WIDTH], inHeight = TIDLPCLayersOut.inData[0].dimValues[TIDL_DIM_HEIGHT];
  int32_t kernelW, kernelH, strideW, strideH, dilationW, dilationH;

  if (TIDLPCLayersOut.layerType == TIDL_ConvolutionLayer)
  {
    kernelW = TIDLPCLayersOut.layerParams.convParams.kernelW;
    kernelH = TIDLPCLayersOut.layerParams.convParams.kernelH;
    strideW = TIDLPCLayersOut.layerParams.convParams.strideW;
    strideH = TIDLPCLayersOut.layerParams.convParams.strideH;
    dilationW = TIDLPCLayersOut.layerParams.convParams.dilationW;
    dilationH = TIDLPCLayersOut.layerParams.convParams.dilationH;
  }
  else if (TIDLPCLayersOut.layerType == TIDL_PoolingLayer)
  {
    kernelW = TIDLPCLayersOut.layerParams.poolParams.kernelW;
    kernelH = TIDLPCLayersOut.layerParams.poolParams.kernelH;
    strideW = TIDLPCLayersOut.layerParams.poolParams.strideW;
    strideH = TIDLPCLayersOut.layerParams.poolParams.strideH;
    dilationW = 1;
    dilationH = 1;
    if (TIDLPCLayersOut.layerParams.poolParams.useCeil == 1 )
    {
      /* Formula to calculate the totalPad assumes floor calculation of outHeight.
        In case it is ceiling, we need to recompute. */
      sTIDL_PoolingParams_t &poolParams = TIDLPCLayersOut.layerParams.poolParams;
      outHeight = floor(((inHeight + poolParams.padH*2.0) - (poolParams.kernelH)) / poolParams.strideH) + 1;
      outWidth  = floor(((inWidth  + poolParams.padW*2.0) - (poolParams.kernelW)) / poolParams.strideW) + 1;
    }
  }
  else
  {
    printf ("Line %d: Layer unsupported with tidl_setLRTBpad!!\n", __LINE__);
    assert(0);
  }
  int32_t effCoeffWidth = ((kernelW - 1) * dilationW) + 1;
  int32_t effCoeffHeight = ((kernelH - 1) * dilationH) + 1;
  int32_t totalPadW = (outWidth - 1)*strideW + effCoeffWidth - inWidth;
  int32_t totalPadH = (outHeight - 1)*strideH + effCoeffHeight - inHeight;
  totalPadH = totalPadH < 0 ? 0 : totalPadH;
  totalPadW = totalPadW < 0 ? 0 : totalPadW;

  if((outWidth > 0) || (outHeight > 0))
  /*
  This function gets called in places(eg. Padreshape()) where tensor dimensions(outH,OutW) are not set,
  This function gets called later in mergeReshape() function under condition that LRTB pads have not been already set,
  hence preventing setting of LRTB pads when output tensor dimensions have not been set.
  */
  {
    if (TIDLPCLayersOut.layerType == TIDL_ConvolutionLayer)
    {
      if(TIDLPCLayersOut.strideOffsetMethod == TIDL_StrideOffsetTopLeft)
      {
        TIDLPCLayersOut.layerParams.convParams.padL = (totalPadW + 1)>>1;
        TIDLPCLayersOut.layerParams.convParams.padR = totalPadW>>1;
        TIDLPCLayersOut.layerParams.convParams.padT = (totalPadH + 1)>>1;
        TIDLPCLayersOut.layerParams.convParams.padB = totalPadH>>1;
      }
      else if(TIDLPCLayersOut.strideOffsetMethod == TIDL_StrideOffsetCenter)
      {
        TIDLPCLayersOut.layerParams.convParams.padL = totalPadW>>1;
        TIDLPCLayersOut.layerParams.convParams.padR = (totalPadW + 1)>>1;
        TIDLPCLayersOut.layerParams.convParams.padT = totalPadH>>1;
        TIDLPCLayersOut.layerParams.convParams.padB = (totalPadH + 1)>>1;
      }
    }
    else if (TIDLPCLayersOut.layerType == TIDL_PoolingLayer)
    {
      if(TIDLPCLayersOut.strideOffsetMethod == TIDL_StrideOffsetTopLeft)
      {
        TIDLPCLayersOut.layerParams.poolParams.padL = (totalPadW + 1)>>1;
        TIDLPCLayersOut.layerParams.poolParams.padR = totalPadW>>1;
        TIDLPCLayersOut.layerParams.poolParams.padT = (totalPadH + 1)>>1;
        TIDLPCLayersOut.layerParams.poolParams.padB = totalPadH>>1;
      }
      else if(TIDLPCLayersOut.strideOffsetMethod == TIDL_StrideOffsetCenter)
      {
        TIDLPCLayersOut.layerParams.poolParams.padL = totalPadW>>1;
        TIDLPCLayersOut.layerParams.poolParams.padR = (totalPadW + 1)>>1;
        TIDLPCLayersOut.layerParams.poolParams.padT = totalPadH>>1;
        TIDLPCLayersOut.layerParams.poolParams.padB = (totalPadH + 1)>>1;
      }
    }
  }
}

const sTIDL_DataParams_t * TIDL_getOutData(const sTIDL_OrgNetwork_t * pOrgTIDLNetStructure,
                                     int32_t dataId)
{
  int32_t i, j;
  for (i = 0; i < pOrgTIDLNetStructure->numLayers; i++)
  {
    for (j = 0; j < pOrgTIDLNetStructure->TIDLPCLayers[i].numOutBufs; j++)
    {
      if (pOrgTIDLNetStructure->TIDLPCLayers[i].outData[j].dataId == dataId)
      {
        return &pOrgTIDLNetStructure->TIDLPCLayers[i].outData[j];
      }
    }
  }
  return (NULL);
}

const sTIDL_DataParams_t * TIDL_getOutDataInNet(const sTIDL_Network_t * tidlNet,
                                     int32_t dataId)
{
  int32_t i, j;
  for (i = 0; i < tidlNet->numLayers; i++)
  {
    for (j = 0; j < tidlNet->TIDLLayers[i].numOutBufs; j++)
    {
      if (tidlNet->TIDLLayers[i].outData.dataId == dataId)
      {
        return &tidlNet->TIDLLayers[i].outData;
      }
    }
  }
  return (NULL);
}

const char* TIDL_getOutDataName(const sTIDL_OrgNetwork_t * pOrgTIDLNetStructure, int32_t dataId)
{
  int32_t i, j;
  for (i = 0; i < pOrgTIDLNetStructure->numLayers; i++)
  {
    for (j = 0; j < pOrgTIDLNetStructure->TIDLPCLayers[i].numOutBufs; j++)
    {
      if (pOrgTIDLNetStructure->TIDLPCLayers[i].outData[j].dataId == dataId)
      {
        return (const char*)pOrgTIDLNetStructure->TIDLPCLayers[i].outDataNames[j];;
      }
    }
  }
  return (NULL);
}

int32_t TIDL_getDataID(sTIDL_DataParams_t *data,
  sTIDL_OrgNetwork_t * pOrgTIDLNetStructure,
  int32_t            numLayer,
  int8_t             *bufName)
{
  int32_t i, j;
  for (i = (numLayer - 1); i >= 0; i--)
  {
    for (j = 0; j < pOrgTIDLNetStructure->TIDLPCLayers[i].numOutBufs; j++)
    {
      if (strcmp((const char*)bufName,
        (const char*)pOrgTIDLNetStructure->TIDLPCLayers[i].outDataNames[j]) == 0)
      {
        *data = pOrgTIDLNetStructure->TIDLPCLayers[i].outData[j];
        return 0;
      }
    }
  }
  return -1;
}

int32_t TIDL_isDataBufUsed(int32_t           dataId,
  sTIDL_Network_t   *pTIDLNetStructure,
  int32_t           numLayer)
{
  int32_t i, j;
  for (i = 0; i < numLayer; i++)
  {
    for (j = 0; j < pTIDLNetStructure->TIDLLayers[i].numInBufs; j++)
    {
      if (pTIDLNetStructure->TIDLLayers[i].inData[j] == dataId)
      {
        return 1;
      }
    }
  }
  return 0;
}

int32_t TIDL_isInputConv2D(sTIDL_OrgNetwork_t   *pOrgTIDLNetStruct,
  int32_t              numLayer,
  const char           *bufName)
{
  int32_t i, j;
  for (i = (numLayer - 1); i >= 0; i--)
  {
    for (j = 0; j < pOrgTIDLNetStruct->TIDLPCLayers[i].numOutBufs; j++)
    {
      if (strcmp((const char*)bufName,
        (const char*)pOrgTIDLNetStruct->TIDLPCLayers[i].outDataNames[j]) == 0)
      {
        if ((pOrgTIDLNetStruct->TIDLPCLayers[i].numOutBufs == 1) &&
          (pOrgTIDLNetStruct->TIDLPCLayers[i].layerType == TIDL_ConvolutionLayer))
        {
          return 1;
        }
        else
        {
          return 0;
        }
      }
    }
  }
  return 0;
}

void TIDL_UpdateInDataBuff(sTIDL_OrgNetwork_t * pOrgTIDLNetStructure,
  uint32_t numLayers, sTIDL_DataParams_t dataBuf)
{
  uint32_t i, j;
  for (i = 0; i < numLayers; i++)
  {
    for (j = 0; (j < pOrgTIDLNetStructure->TIDLPCLayers[i].numInBufs) &&
      (pOrgTIDLNetStructure->TIDLPCLayers[i].numInBufs > 0); j++)
    {
      if (pOrgTIDLNetStructure->TIDLPCLayers[i].inData[j].dataId ==
        dataBuf.dataId)
      {
        pOrgTIDLNetStructure->TIDLPCLayers[i].inData[j] = dataBuf;
      }
    }

  }

  return;
}


void TIDL_UpdateInDataName(sTIDL_OrgNetwork_t * pOrgTIDLNetStructure,
  uint32_t numLayers, char * currName , char * newName)
{
  int32_t i, j;
  for (i = 0; i < numLayers; i++)
  {
    for (j = 0; j < pOrgTIDLNetStructure->TIDLPCLayers[i].numInBufs; j++)
    {
      if (strcmp((const char *)pOrgTIDLNetStructure->TIDLPCLayers[i].inDataNames[j], (const char *)currName) == 0)
      {
        strcpy((char *)pOrgTIDLNetStructure->TIDLPCLayers[i].inDataNames[j], newName);
      }
    }
  }
  return;
}


void TIDL_UpdateInDataNameInLayers(sTIDL_OrgNetwork_t * pOrgTIDLNetStructure,
  uint32_t numLayers, char * currName , char * newName, std::vector<int32_t>& layers)
{
  int32_t j;
  for (int32_t i : layers)
  {
    for (j = 0; j < pOrgTIDLNetStructure->TIDLPCLayers[i].numInBufs; j++)
    {
      if (strcmp((const char *)pOrgTIDLNetStructure->TIDLPCLayers[i].inDataNames[j], (const char *)currName) == 0)
      {
        strcpy((char *)pOrgTIDLNetStructure->TIDLPCLayers[i].inDataNames[j], newName);
      }
    }
  }
  return;
}


bool TIDL_checkIsDirectory(const char *path) {
#ifndef _WIN32
    const char *real = realpath(path, NULL);
    if(!real)
        return false;

    struct stat st;
    int res = stat(real, &st);
    if(res)
        return false;

    bool ret = false;
    if ((st.st_mode & S_IFMT) == S_IFDIR) {
        ret = true;
        free(const_cast<char *>(real));
    }
    return ret;
#else
  return 0;
#endif

}


int32_t TIDL_alignParamsWrite(FILE *fp, sBuffer_t * buf, uint32_t *totalParamSize, uint32_t numBytes)
{

  uint32_t alignSize = (*totalParamSize % ALIGN_NET_DATA);
  int32_t offset = 0;
  uint32_t writeSize = 0 ;

  if(alignSize)
  {
    alignSize =  ALIGN_NET_DATA - alignSize;
    *totalParamSize += alignSize;
  }

  if (alignSize)
  {
    uint8_t * ptr = (uint8_t *)my_malloc(alignSize*sizeof(uint8_t));
    memset(ptr, 0, alignSize*sizeof(uint8_t));

    if (fp) fwrite(ptr, 1, alignSize, fp);
    free(ptr);
  }

  if ( buf->ptr != NULL )
  {
    writeSize = buf->bufSize * numBytes;
    if (fp)
    {
      fwrite(buf->ptr, 1, writeSize, fp);
    }
  }

  offset = *totalParamSize;
  *totalParamSize += writeSize;
  return (offset);
}

void TIDL_setDefaultKernelType(sTIDL_OrgNetwork_t * pOrgTIDLNetStructure, uint32_t numLayers)
{
  int32_t layerIdx;
  for (layerIdx = 0; layerIdx < numLayers; layerIdx++)
  {
    pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].layerKernelType = TIDL_HighThroughputKernel;
  }
}

void TIDL_asymAllocScalesPointers(sTIDL_OrgNetwork_t * pOrgTIDLNetStructure, uint32_t numLayers)
{
  /*Allocates scales and zero points looking at bias in the PC structure*/
  int32_t layerIdx;
  int32_t bufSizeInBytes;
  int32_t numOutputChannels = 0;
  for (layerIdx = 0; layerIdx < numLayers; layerIdx++)
  {
    if(TIDL_doesLayerSupportAsymTensors(&pOrgTIDLNetStructure->TIDLPCLayers[layerIdx]))
    {
      if(TIDL_QuantStyleAsymNP2 == gParams.quantizationStyle &&
          TIDL_doesLayerSupportAsymTensors(&pOrgTIDLNetStructure->TIDLPCLayers[layerIdx]))
      {
        /*Allocate for a worst case scenario:*/
        numOutputChannels = pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].bias.bufSize;
        pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].derivedBias.bufSize = pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].bias.bufSize;
        pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].derivedScales.bufSize = pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].bias.bufSize;
        pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].derivedShifts.bufSize = pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].bias.bufSize;
        pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].derivedBias.ptr   = (uint8_t *)my_malloc( numOutputChannels *  ((pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].weightsElementSizeInBits + 7) / 8) *  sizeof(int32_t));
        pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].derivedScales.ptr = (uint8_t *)my_malloc( numOutputChannels *  ((pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].weightsElementSizeInBits + 7) / 8));
        pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].derivedShifts.ptr = (uint8_t *)my_malloc( numOutputChannels *  ((pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].weightsElementSizeInBits + 7) / 8));
      }
    }
    if( pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].bias.ptr != NULL )
    {
      bufSizeInBytes = pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].bias.bufSize * sizeof(float);
      if(TIDL_QuantStyleAsymNP2 == gParams.quantizationStyle && TIDL_doesLayerSupportAsymTensors(&pOrgTIDLNetStructure->TIDLPCLayers[layerIdx]))
      {
        /*WeightScales & BiasScales have the same buffer size:*/
        pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].weightScales.bufSize = pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].bias.bufSize;
        pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].weightZeroPoints.bufSize = pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].bias.bufSize;
        pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].biasScales.bufSize = pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].bias.bufSize;
        pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].biasZeroPoints.bufSize = pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].bias.bufSize;
        /*Allocate and copy scales:*/
        pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].weightScales.ptr = (uint8_t *)my_malloc( bufSizeInBytes);
        pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].biasScales.ptr = (uint8_t *)my_malloc( bufSizeInBytes);
        //Reduce alloc size for ZP:
        pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].biasZeroPoints.ptr = (uint8_t *)my_malloc( bufSizeInBytes);
        pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].weightZeroPoints.ptr = (uint8_t *)my_malloc( bufSizeInBytes);
      }
    }
  }

}

void TIDL_allocAndCopyModelParams(sTIDL_OrgNetwork_t * orgTIDLNetStructureDst,sTIDL_OrgNetwork_t * orgTIDLNetStructureSrc, uint32_t numLayers)
{
  int32_t layerIdx;
  int32_t bufSizeInBytes;
  int32_t numOutputChannels;
  for (layerIdx = 0; layerIdx < numLayers; layerIdx++)
  {
    if ( orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weights.ptr != NULL )
    {
      orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].weights.bufSize =
        orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weights.bufSize;
      if(orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].layerType == TIDL_CustomLayer)
      {
        /* For custom layer the weight buffer is used to get input parameters along with any other memory
        needed by custom layer. This memory is already in terms of bytes */
        bufSizeInBytes = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weights.bufSize;
      }
      else
      {
        bufSizeInBytes = (gParams.quantizationStyle == TIDL_QuantStyleAsymNP2_TFL)?
                          orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weights.bufSize:
                          orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weights.bufSize* sizeof(float);
      }
      orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].weights.ptr = my_malloc( bufSizeInBytes);

      memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].weights.ptr,
                  orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weights.ptr,
                  bufSizeInBytes);
    }

    if(orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].layerType == TIDL_ConvolutionLayer)
    {
      if((TIDL_QuantStyleAsymNP2 == gParams.quantizationStyle) && TIDL_doesLayerSupportAsymTensors(&orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx]))
      {
        /*Rev 2 specific*/
        int32_t numOutputChannels = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].layerParams.convParams.numOutChannels;
        orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].bias.bufSize = numOutputChannels;
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].derivedBias.bufSize = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].bias.bufSize;
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].derivedScales.bufSize = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].bias.bufSize;
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].derivedShifts.bufSize = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].bias.bufSize;
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].derivedBias.ptr = (uint8_t *)my_malloc( numOutputChannels *  ((orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightsElementSizeInBits + 7) / 8) * sizeof(int32_t));
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].derivedScales.ptr = (uint8_t *)my_malloc( numOutputChannels *  ((orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightsElementSizeInBits + 7) / 8) * sizeof(uint8_t));
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].derivedShifts.ptr = (uint8_t *)my_malloc( numOutputChannels *  ((orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightsElementSizeInBits + 7) / 8) * sizeof(uint8_t));
        //printf("TX from %p to %p of size %d\n",orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].derivedBias.ptr, orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].derivedBias.ptr,numOutputChannels * ((orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightsElementSizeInBits + 7) / 8) * sizeof(int32_t));
        /*Copy*/
        if(orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].derivedBias.ptr != NULL)
        {
          memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].derivedBias.ptr,
                  orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].derivedBias.ptr,
                  numOutputChannels * ((orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightsElementSizeInBits + 7) / 8) * sizeof(int32_t));
        }
        if(orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].derivedScales.ptr != NULL)
        {
          memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].derivedScales.ptr,
                  orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].derivedScales.ptr,
                  numOutputChannels * ((orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightsElementSizeInBits + 7) / 8) * sizeof(uint8_t));
        }
        if(orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].derivedShifts.ptr != NULL)
        {
          memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].derivedShifts.ptr,
                  orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].derivedShifts.ptr,
                  numOutputChannels * ((orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightsElementSizeInBits + 7) / 8) * sizeof(uint8_t));
        }
      }
    }
     else if(orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].layerType == TIDL_InnerProductLayer)
    {
      if(TIDL_QuantStyleAsymNP2 == gParams.quantizationStyle && TIDL_doesLayerSupportAsymTensors(&orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx]))
      {
        /*Rev 2 specific*/
        int32_t numOutputChannels = 1;
        if(orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].layerParams.innerProductParams.constIdx == 0)
        {
          //Along rows
          numOutputChannels = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].layerParams.innerProductParams.numInRows;
        }
        else
        {
          numOutputChannels = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].layerParams.innerProductParams.numOutCols;
        }
        orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].bias.bufSize = numOutputChannels;
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].derivedBias.bufSize = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].bias.bufSize;
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].derivedScales.bufSize = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].bias.bufSize;
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].derivedShifts.bufSize = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].bias.bufSize;
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].derivedBias.ptr = (uint8_t *)my_malloc( numOutputChannels *  ((orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightsElementSizeInBits + 7) / 8) * sizeof(int32_t));
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].derivedScales.ptr = (uint8_t *)my_malloc( numOutputChannels *  ((orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightsElementSizeInBits + 7) / 8) * sizeof(uint8_t));
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].derivedShifts.ptr = (uint8_t *)my_malloc( numOutputChannels *  ((orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightsElementSizeInBits + 7) / 8) * sizeof(uint8_t));
        //printf("TX from %p to %p of size %d\n",orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].derivedBias.ptr, orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].derivedBias.ptr,numOutputChannels * ((orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightsElementSizeInBits + 7) / 8) * sizeof(int32_t));
        /*Copy*/
        if(orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].derivedBias.ptr != NULL)
        {
          memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].derivedBias.ptr,
                  orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].derivedBias.ptr,
                  numOutputChannels * ((orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightsElementSizeInBits + 7) / 8) * sizeof(int32_t));
        }
        if(orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].derivedScales.ptr != NULL)
        {
          memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].derivedScales.ptr,
                  orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].derivedScales.ptr,
                  numOutputChannels * ((orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightsElementSizeInBits + 7) / 8) * sizeof(uint8_t));
        }
        if(orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].derivedShifts.ptr != NULL)
        {
          memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].derivedShifts.ptr,
                  orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].derivedShifts.ptr,
                  numOutputChannels * ((orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightsElementSizeInBits + 7) / 8) * sizeof(uint8_t));
        }
      }
    }

    if ( orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].bias.ptr != NULL )
    {
      orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].bias.bufSize =
         orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].bias.bufSize;
      bufSizeInBytes = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].bias.bufSize * sizeof(float);

      orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].bias.ptr = (uint8_t *)my_malloc( bufSizeInBytes);
      memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].bias.ptr,
                 orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].bias.ptr,
                 bufSizeInBytes);
       if(TIDL_QuantStyleAsymNP2 == gParams.quantizationStyle)
      {
        /*WeightScales & BiasScales have the same buffer size:*/
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].weightScales.bufSize = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].bias.bufSize;
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].weightZeroPoints.bufSize = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].bias.bufSize;
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].biasScales.bufSize = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].bias.bufSize;
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].biasZeroPoints.bufSize = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].bias.bufSize;
        /*Allocate and copy scales:*/
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].weightScales.ptr = (uint8_t *)my_malloc( bufSizeInBytes);
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].biasScales.ptr = (uint8_t *)my_malloc( bufSizeInBytes);
        if(orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightScales.ptr != NULL)
        {
          memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].weightScales.ptr,
                  orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightScales.ptr,
                  bufSizeInBytes);
        }
        if(orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].biasScales.ptr != NULL)
        {
          memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].biasScales.ptr,
                  orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].biasScales.ptr,
                  bufSizeInBytes);
        }

        //Reduce alloc size for ZP:
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].biasZeroPoints.ptr = (uint8_t *)my_malloc( bufSizeInBytes);
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].weightZeroPoints.ptr = (uint8_t *)my_malloc( bufSizeInBytes);
        if(orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].biasZeroPoints.ptr != NULL)
        {
          memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].biasZeroPoints.ptr,
                  orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].biasZeroPoints.ptr,
                  bufSizeInBytes);
        }
        if(orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightZeroPoints.ptr != NULL)
        {
          memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].weightZeroPoints.ptr,
                  orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightZeroPoints.ptr,
                  bufSizeInBytes);
        }
      }
    }

    if ( orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].slope.ptr != NULL )
    {
      orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].slope.bufSize =
         orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].slope.bufSize;
      bufSizeInBytes = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].slope.bufSize * sizeof(float);

      orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].slope.ptr = (uint8_t *)my_malloc( bufSizeInBytes);
      memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].slope.ptr,
                 orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].slope.ptr,
                 bufSizeInBytes);
    }

    if ( orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].priorBox.ptr != NULL )
    {
      orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].priorBox.bufSize =
         orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].priorBox.bufSize;

      bufSizeInBytes = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].priorBox.bufSize * sizeof(float);

      orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].priorBox.ptr = (uint8_t *)my_malloc( bufSizeInBytes);
      memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].priorBox.ptr,
                 orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].priorBox.ptr,
                 bufSizeInBytes);
    }
  }
}


void TIDL_copyModelParams(sTIDL_OrgNetwork_t * orgTIDLNetStructureDst,sTIDL_OrgNetwork_t * orgTIDLNetStructureSrc, uint32_t numLayers)
{
  int32_t layerIdx;
  int32_t bufSizeInBytes;
  int32_t numOutputChannels;

  for (layerIdx = 0; layerIdx < numLayers; layerIdx++)
  {
    if ( orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weights.ptr != NULL )
    {
      orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].weights.bufSize =
        orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weights.bufSize;
      if(orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].layerType == TIDL_CustomLayer)
      {
        bufSizeInBytes = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weights.bufSize;
      }
      else
      {
        bufSizeInBytes = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weights.bufSize * sizeof(float);
      }
      memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].weights.ptr,
                  orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weights.ptr,
                  bufSizeInBytes);
    }

    if ( orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].perChannelWeightScale.ptr != NULL )
    {
      orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].perChannelWeightScale.bufSize =
        orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].perChannelWeightScale.bufSize;
      bufSizeInBytes = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].perChannelWeightScale.bufSize * sizeof(float);
      memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].perChannelWeightScale.ptr,
                  orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].perChannelWeightScale.ptr,
                  bufSizeInBytes);
    }

    if(orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].layerType == TIDL_ConvolutionLayer)
    {
      if(TIDL_QuantStyleAsymNP2 == gParams.quantizationStyle && TIDL_doesLayerSupportAsymTensors(&orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx]))
      {
        int32_t numOutputChannels = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].layerParams.convParams.numOutChannels;
        orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].bias.bufSize = numOutputChannels;
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].derivedBias.bufSize = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].bias.bufSize;
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].derivedScales.bufSize = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].bias.bufSize;
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].derivedShifts.bufSize = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].bias.bufSize;
        /*Copy*/
        if(orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].derivedBias.ptr != NULL)
        {
          memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].derivedBias.ptr,
                 orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].derivedBias.ptr,
                 numOutputChannels *  ((orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightsElementSizeInBits + 7) / 8) * sizeof(int32_t));
        }
        if(orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].derivedScales.ptr != NULL)
        {
          memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].derivedScales.ptr,
                  orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].derivedScales.ptr,
                  numOutputChannels * ((orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightsElementSizeInBits + 7) / 8) * sizeof(uint8_t));
        }
        if(orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].derivedShifts.ptr != NULL)
        {
           memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].derivedShifts.ptr,
                 orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].derivedShifts.ptr,
                 numOutputChannels * ((orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightsElementSizeInBits + 7) / 8) * sizeof(uint8_t));
        }
      }
    }
    else if(orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].layerType == TIDL_InnerProductLayer)
    {
      if(TIDL_QuantStyleAsymNP2 == gParams.quantizationStyle && TIDL_doesLayerSupportAsymTensors(&orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx]))
      {
        int32_t numOutputChannels = 1;
        if(orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].layerParams.innerProductParams.constIdx == 0)
        {
          //Along rows
          numOutputChannels = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].layerParams.innerProductParams.numInRows;
        }
        else
        {
          numOutputChannels = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].layerParams.innerProductParams.numOutCols;
        }
        orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].bias.bufSize = numOutputChannels;
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].derivedBias.bufSize = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].bias.bufSize;
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].derivedScales.bufSize = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].bias.bufSize;
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].derivedShifts.bufSize = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].bias.bufSize;
        /*Copy*/
        if(orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].derivedBias.ptr != NULL)
        {
          memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].derivedBias.ptr,
                 orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].derivedBias.ptr,
                 numOutputChannels *  ((orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightsElementSizeInBits + 7) / 8) * sizeof(int32_t));
        }
        if(orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].derivedScales.ptr != NULL)
        {
          memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].derivedScales.ptr,
                  orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].derivedScales.ptr,
                  numOutputChannels * ((orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightsElementSizeInBits + 7) / 8) * sizeof(uint8_t));
        }
        if(orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].derivedShifts.ptr != NULL)
        {
           memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].derivedShifts.ptr,
                 orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].derivedShifts.ptr,
                 numOutputChannels * ((orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightsElementSizeInBits + 7) / 8) * sizeof(uint8_t));
        }
      }
    }

    if(gParams.quantizationStyle == TIDL_QuantStyleAsymNP2_TFL)
    {
      if ( orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightScales.ptr != NULL )
      {
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].weightScales.bufSize =
          orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightScales.bufSize;
        bufSizeInBytes = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightScales.bufSize * sizeof(float);
        memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].weightScales.ptr,
                    orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightScales.ptr,
                    bufSizeInBytes);
      }
      if ( orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].biasScales.ptr != NULL )
      {
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].biasScales.bufSize =
          orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].biasScales.bufSize;
        bufSizeInBytes = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].biasScales.bufSize * sizeof(float);
        memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].biasScales.ptr,
                    orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].biasScales.ptr,
                    bufSizeInBytes);
      }
      if ( orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightZeroPoints.ptr != NULL )
      {
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].weightZeroPoints.bufSize =
          orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightZeroPoints.bufSize;
        bufSizeInBytes = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightZeroPoints.bufSize * sizeof(int8_t);
        memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].weightZeroPoints.ptr,
                    orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightZeroPoints.ptr,
                    bufSizeInBytes);
      }
      if ( orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].biasZeroPoints.ptr != NULL )
      {
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].biasZeroPoints.bufSize =
          orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].biasZeroPoints.bufSize;
        bufSizeInBytes = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].biasZeroPoints.bufSize * sizeof(float);
        memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].biasZeroPoints.ptr,
                    orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].biasZeroPoints.ptr,
                    bufSizeInBytes);
      }
    }


    if ( orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].bias.ptr != NULL )
    {
      orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].bias.bufSize =
         orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].bias.bufSize;
      bufSizeInBytes = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].bias.bufSize * sizeof(float);
      memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].bias.ptr,
                 orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].bias.ptr,
                 bufSizeInBytes);
      if(TIDL_QuantStyleAsymNP2 == gParams.quantizationStyle && TIDL_doesLayerSupportAsymTensors(&orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx]))
      {
        /*WeightScales & BiasScales have the same buffer size:*/
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].weightScales.bufSize = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].bias.bufSize;
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].weightZeroPoints.bufSize = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].bias.bufSize;
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].biasScales.bufSize = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].bias.bufSize;
        orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].biasZeroPoints.bufSize = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].bias.bufSize;
        /*Allocate and copy scales:*/
        memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].weightScales.ptr,
                 orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightScales.ptr,
                 bufSizeInBytes);
        memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].biasScales.ptr,
                 orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].biasScales.ptr,
                 bufSizeInBytes);
        //Reduce alloc size for ZP:
        memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].biasZeroPoints.ptr,
                 orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].biasZeroPoints.ptr,
                 bufSizeInBytes);
        memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].weightZeroPoints.ptr,
                 orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].weightZeroPoints.ptr,
                 bufSizeInBytes);
      }
    }

    if ( orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].slope.ptr != NULL )
    {
      orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].slope.bufSize =
         orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].slope.bufSize;
      bufSizeInBytes = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].slope.bufSize * sizeof(float);
      memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].slope.ptr,
                 orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].slope.ptr,
                 bufSizeInBytes);
    }

    if ( orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].priorBox.ptr != NULL )
    {
      orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].priorBox.bufSize =
         orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].priorBox.bufSize;

      bufSizeInBytes = orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].priorBox.bufSize * sizeof(float);
      memcpy(orgTIDLNetStructureDst->TIDLPCLayers[layerIdx].priorBox.ptr,
                 orgTIDLNetStructureSrc->TIDLPCLayers[layerIdx].priorBox.ptr,
                 bufSizeInBytes);
    }
  }
}

void TIDL_freeModelParams(sTIDL_OrgNetwork_t * pOrgTIDLNetStructure, uint32_t numLayers)
{
  int32_t layerIdx;
  for (layerIdx = 0; layerIdx < numLayers; layerIdx++)
  {
    if ( pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].weights.ptr != NULL )
    {
      my_free(pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].weights.ptr);
      pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].weights.ptr= NULL;
    }

    if(TIDL_doesLayerSupportAsymTensors(&pOrgTIDLNetStructure->TIDLPCLayers[layerIdx]))
    {
      if(TIDL_QuantStyleAsymNP2 == gParams.quantizationStyle && TIDL_doesLayerSupportAsymTensors(&pOrgTIDLNetStructure->TIDLPCLayers[layerIdx]))
      {
        my_free(pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].derivedBias.ptr);
        pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].derivedBias.ptr = NULL;
        my_free(pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].derivedScales.ptr);
        pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].derivedScales.ptr = NULL;
        my_free(pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].derivedShifts.ptr);
        pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].derivedShifts.ptr = NULL;
      }
    }

    if ( pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].bias.ptr != NULL )
    {
      my_free(pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].bias.ptr);
      pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].bias.ptr= NULL;
      if(TIDL_QuantStyleAsymNP2 == gParams.quantizationStyle && TIDL_doesLayerSupportAsymTensors(&pOrgTIDLNetStructure->TIDLPCLayers[layerIdx]))
      {
        my_free(pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].weightScales.ptr);
        pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].weightScales.ptr = NULL;
        my_free(pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].biasScales.ptr);
        pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].biasScales.ptr = NULL;
        my_free(pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].biasZeroPoints.ptr);
        pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].biasZeroPoints.ptr = NULL;
        my_free(pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].weightZeroPoints.ptr);
        pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].weightZeroPoints.ptr = NULL;
      }
    }

    if ( pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].slope.ptr != NULL )
    {
      my_free(pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].slope.ptr);
      pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].slope.ptr= NULL;
    }

    if ( pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].priorBox.ptr != NULL )
    {
      my_free(pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].priorBox.ptr);
      pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].priorBox.ptr= NULL;
    }
  }
}


int32_t TIDL_getCompilerDataSize(const sPerfSim_t * perfSimInfo, int32_t numLayers)
{
  int32_t size = 0 ;
  if(perfSimInfo)
  {
    size = sizeof(sPerfSim_t ) -
                    sizeof(perfSimInfo->sdataFlowInfo) -
                    sizeof(perfSimInfo->graphCompOut);
  }
  size = ALIGN(size, ALIGN_NET_DATA) ;
  return size ;
}

int32_t TIDL_getNetworkStructDataSize(const sTIDL_Network_t *pNet)
{
  int32_t size = 0 ;
  if(pNet)
    size = ((uint8_t*)(&pNet->TIDLLayers[pNet->numLayers])) - ((uint8_t*)pNet);
  size = ALIGN(size, ALIGN_NET_DATA) ;
  return size ;
}

int32_t TIDL_writeModel(sTIDL_Network_t * tIDLNetStructure,
                                            sTIDL_OrgNetwork_t * orgTIDLNetStructure,
                                            const char * name,
                                            uint32_t numLayers,
                                            tidl_import_config * configParams)
{
  FILE * fp1 = NULL;
  uint32_t i, writeSize, tiLayerIndex;
  uint32_t netSize, compilerSize ;
  int32_t biasParamSize = 2;
  sBuffer_t tempBuf ;
  uint32_t totalParamSize = 0, offset;

  sPerfSim_t *tmpPerfSim = (sPerfSim_t*)malloc(sizeof(sPerfSim_t));

  if (name)
  {
    fp1 = fopen(name, "wb+");
    if (fp1 == NULL)
    {
      printf("ERROR: Could not open %s file for writing \n", (const char *)name);
      exit(-1);
    }
  }
  if(configParams->numParamBits == 32)
  {
    biasParamSize = 4;
  }

  if(gParams.quantizationStyle == TIDL_QuantStyleAsymNP2_TFL)
  {
    biasParamSize = 4;
  }

  //netSize = ((uint8_t*)(&tIDLNetStructure->TIDLLayers[tIDLNetStructure->numLayers])) - ((uint8_t*)tIDLNetStructure);
  netSize =  TIDL_getNetworkStructDataSize(tIDLNetStructure);
  tempBuf.bufSize = netSize ; tempBuf.ptr = (void*)tIDLNetStructure ;
  offset = TIDL_alignParamsWrite(fp1, &tempBuf, &totalParamSize, 1);
  tIDLNetStructure->netBufInfo[TIDL_NET_STRUCTURE_BUF].offset = offset ;
  tIDLNetStructure->netBufInfo[TIDL_NET_STRUCTURE_BUF].size = totalParamSize ;

  compilerSize = TIDL_getCompilerDataSize(tmpPerfSim,tIDLNetStructure->numLayers );
  memset(tmpPerfSim, 0, compilerSize);
  tempBuf.bufSize = compilerSize ; tempBuf.ptr = (void*) tmpPerfSim ;
  offset = TIDL_alignParamsWrite(fp1, &tempBuf, &totalParamSize, 1);
  tIDLNetStructure->netBufInfo[TIDL_NET_COMPILER_BUF].offset = offset ;
  tIDLNetStructure->netBufInfo[TIDL_NET_COMPILER_BUF].size = totalParamSize -
  tIDLNetStructure->netBufInfo[TIDL_NET_COMPILER_BUF].offset ;

  tempBuf.bufSize = 0 ; tempBuf.ptr = NULL ;
  offset = TIDL_alignParamsWrite(fp1, &tempBuf, &totalParamSize, 0);
  tIDLNetStructure->netBufInfo[TIDL_NET_PARAMS_BUF].offset = offset ;

  tiLayerIndex = 0;

  for (i = 0; i < numLayers; i++)
  {
    if ((orgTIDLNetStructure->TIDLPCLayers[i].layerType != TIDL_SplitLayer) &&
      (orgTIDLNetStructure->TIDLPCLayers[i].layerType != TIDL_DropOutLayer) &&
      (orgTIDLNetStructure->TIDLPCLayers[i].layerType != TIDL_PriorBoxLayer) &&
      (orgTIDLNetStructure->TIDLPCLayers[i].layerType != TIDL_UnsupportedLayer))
    {
      if ((orgTIDLNetStructure->TIDLPCLayers[i].layerType == TIDL_ConvolutionLayer) ||
        (orgTIDLNetStructure->TIDLPCLayers[i].layerType == TIDL_Deconv2DLayer))
      {
        tIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.convParams.weights =
          TIDL_alignParamsWrite(fp1, &orgTIDLNetStructure->TIDLPCLayers[i].weights, &totalParamSize, ((orgTIDLNetStructure->TIDLPCLayers[i].weightsElementSizeInBits + 7) / 8));
        if(gParams.quantizationStyle == TIDL_QuantStyleAsymNP2_TFL)
        {
          tIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.convParams.weightScales =
            TIDL_alignParamsWrite(fp1, &orgTIDLNetStructure->TIDLPCLayers[i].weightScales, &totalParamSize, (sizeof(float))); //Scales are float..
          tIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.convParams.weightZeroPoints =
            TIDL_alignParamsWrite(fp1, &orgTIDLNetStructure->TIDLPCLayers[i].weightZeroPoints, &totalParamSize, (1)); //Zero points are int8_t
        }
        else if(gParams.quantizationStyle == TIDL_QuantStyleAsymNP2 && TIDL_doesLayerSupportAsymTensors(&orgTIDLNetStructure->TIDLPCLayers[i]))
        {
          tIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.convParams.weightScales =
            TIDL_alignParamsWrite(fp1, &orgTIDLNetStructure->TIDLPCLayers[i].weightScales, &totalParamSize, (sizeof(float))); //Scales are float..
          tIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.convParams.weightZeroPoints =
            TIDL_alignParamsWrite(fp1, &orgTIDLNetStructure->TIDLPCLayers[i].weightZeroPoints, &totalParamSize, ((orgTIDLNetStructure->TIDLPCLayers[i].weightsElementSizeInBits + 7) / 8));
          tIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.convParams.derivedBias =
            TIDL_alignParamsWrite(fp1, &orgTIDLNetStructure->TIDLPCLayers[i].derivedBias, &totalParamSize, ((orgTIDLNetStructure->TIDLPCLayers[i].weightsElementSizeInBits + 7) / 8) * sizeof(int32_t));
          tIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.convParams.derivedScales =
            TIDL_alignParamsWrite(fp1, &orgTIDLNetStructure->TIDLPCLayers[i].derivedScales, &totalParamSize, ((orgTIDLNetStructure->TIDLPCLayers[i].weightsElementSizeInBits + 7) / 8));
          tIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.convParams.derivedShifts =
            TIDL_alignParamsWrite(fp1, &orgTIDLNetStructure->TIDLPCLayers[i].derivedShifts, &totalParamSize, ((orgTIDLNetStructure->TIDLPCLayers[i].weightsElementSizeInBits + 7) / 8));
        }

        if (orgTIDLNetStructure->TIDLPCLayers[i].layerParams.convParams.enableBias)
        {
          tIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.convParams.bias =
            TIDL_alignParamsWrite(fp1, &orgTIDLNetStructure->TIDLPCLayers[i].bias, &totalParamSize, biasParamSize);
          if(gParams.quantizationStyle == TIDL_QuantStyleAsymNP2_TFL)
          {
            tIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.convParams.biasScales =
              TIDL_alignParamsWrite(fp1, &orgTIDLNetStructure->TIDLPCLayers[i].biasScales, &totalParamSize, (sizeof(float))); //Scales are float..
            tIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.convParams.biasZeroPoints =
              TIDL_alignParamsWrite(fp1, &orgTIDLNetStructure->TIDLPCLayers[i].biasZeroPoints, &totalParamSize, (1)); //Zero points are int8_t
          }
          else if(gParams.quantizationStyle == TIDL_QuantStyleAsymNP2 && TIDL_doesLayerSupportAsymTensors(&orgTIDLNetStructure->TIDLPCLayers[i]))
          {
            tIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.convParams.biasScales =
              TIDL_alignParamsWrite(fp1, &orgTIDLNetStructure->TIDLPCLayers[i].biasScales, &totalParamSize, (sizeof(float))); //Scales are float..
            tIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.convParams.biasZeroPoints =
              TIDL_alignParamsWrite(fp1, &orgTIDLNetStructure->TIDLPCLayers[i].biasZeroPoints, &totalParamSize, ((orgTIDLNetStructure->TIDLPCLayers[i].weightsElementSizeInBits + 7) / 8));
          }
        }

        //Gradually deprecate per channel handling on MMAv2 devices (MMAv1 flow):
        if ((configParams->calibrationOption & TIDL_CalibOptionPerChannelWeightQuantization) ==
                  TIDL_CalibOptionPerChannelWeightQuantization)
        {
          if (orgTIDLNetStructure->TIDLPCLayers[i].layerType == TIDL_ConvolutionLayer)
          {
            tIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.convParams.perChannelWeightScaleOffset =
              TIDL_alignParamsWrite(fp1, &orgTIDLNetStructure->TIDLPCLayers[i].perChannelWeightScale,
                                                &totalParamSize,
                                                sizeof(float));
          }
        }

      }
      else if (orgTIDLNetStructure->TIDLPCLayers[i].layerType == TIDL_ConstDataLayer)
      {
        tIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.constDataParams.offset =
          TIDL_alignParamsWrite(fp1, &orgTIDLNetStructure->TIDLPCLayers[i].weights, &totalParamSize, ((orgTIDLNetStructure->TIDLPCLayers[i].weightsElementSizeInBits + 7) / 8));
      }
      else if (orgTIDLNetStructure->TIDLPCLayers[i].layerType == TIDL_InnerProductLayer)
      {
        if(orgTIDLNetStructure->TIDLPCLayers[i].weights.bufSize != 0)
        {
          tIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.innerProductParams.weights =
          TIDL_alignParamsWrite(fp1, &orgTIDLNetStructure->TIDLPCLayers[i].weights, &totalParamSize, ((orgTIDLNetStructure->TIDLPCLayers[i].weightsElementSizeInBits + 7) / 8));
        }
        else
        {
          tIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.innerProductParams.weights = NULL;
        }

        if(orgTIDLNetStructure->TIDLPCLayers[i].bias.bufSize != 0)
        {
          tIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.innerProductParams.bias =
          TIDL_alignParamsWrite(fp1, &orgTIDLNetStructure->TIDLPCLayers[i].bias, &totalParamSize, biasParamSize);
          if(gParams.quantizationStyle == TIDL_QuantStyleAsymNP2 && TIDL_doesLayerSupportAsymTensors(&orgTIDLNetStructure->TIDLPCLayers[i]))
          {
            tIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.innerProductParams.weightScales =
              TIDL_alignParamsWrite(fp1, &orgTIDLNetStructure->TIDLPCLayers[i].weightScales, &totalParamSize, (sizeof(float))); //Scales are float..
            tIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.innerProductParams.weightZeroPoints =
              TIDL_alignParamsWrite(fp1, &orgTIDLNetStructure->TIDLPCLayers[i].weightZeroPoints, &totalParamSize, ((orgTIDLNetStructure->TIDLPCLayers[i].weightsElementSizeInBits + 7) / 8));
            tIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.innerProductParams.derivedBias =
              TIDL_alignParamsWrite(fp1, &orgTIDLNetStructure->TIDLPCLayers[i].derivedBias, &totalParamSize, ((orgTIDLNetStructure->TIDLPCLayers[i].weightsElementSizeInBits + 7) / 8) * sizeof(int32_t));
            tIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.innerProductParams.derivedScales =
              TIDL_alignParamsWrite(fp1, &orgTIDLNetStructure->TIDLPCLayers[i].derivedScales, &totalParamSize, ((orgTIDLNetStructure->TIDLPCLayers[i].weightsElementSizeInBits + 7) / 8));
            tIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.innerProductParams.derivedShifts =
              TIDL_alignParamsWrite(fp1, &orgTIDLNetStructure->TIDLPCLayers[i].derivedShifts, &totalParamSize, ((orgTIDLNetStructure->TIDLPCLayers[i].weightsElementSizeInBits + 7) / 8));
          }
        }
        else
        {
          tIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.innerProductParams.bias = NULL;
        }
      }
      else if (orgTIDLNetStructure->TIDLPCLayers[i].layerType == TIDL_BatchNormLayer)
      {
        tIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.batchNormParams.weights =
          TIDL_alignParamsWrite(fp1, &orgTIDLNetStructure->TIDLPCLayers[i].weights, &totalParamSize, ((orgTIDLNetStructure->TIDLPCLayers[i].weightsElementSizeInBits + 7) / 8));
        tIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.batchNormParams.bias =
          TIDL_alignParamsWrite(fp1, &orgTIDLNetStructure->TIDLPCLayers[i].bias, &totalParamSize, biasParamSize);

        if (orgTIDLNetStructure->TIDLPCLayers[i].actParams.actType == TIDL_PRelU)
        {
          tIDLNetStructure->TIDLLayers[tiLayerIndex].actParams.slope =
            TIDL_alignParamsWrite(fp1, &orgTIDLNetStructure->TIDLPCLayers[i].slope, &totalParamSize, ((orgTIDLNetStructure->TIDLPCLayers[i].weightsElementSizeInBits + 7) / 8));
        }
      }
      else if (orgTIDLNetStructure->TIDLPCLayers[i].layerType == TIDL_PadLayer)
      {
        int32_t offset, size ;
        offset = TIDL_alignParamsWrite(fp1, &orgTIDLNetStructure->TIDLPCLayers[i].perChannelPadConstTensor,
            &totalParamSize, sizeof(float32_tidl));
        size = totalParamSize - offset ;
        tIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.padLayerParams.perChannelPadConstTensorOffset = offset;
        tIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.padLayerParams.perChannelPadConstTensorSize = size ;
      }
      else if (orgTIDLNetStructure->TIDLPCLayers[i].layerType == TIDL_DetectionOutputLayer)
      {
        tIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.detectOutParams.priorBox =
          TIDL_alignParamsWrite(fp1, &orgTIDLNetStructure->TIDLPCLayers[i].priorBox, &totalParamSize, sizeof(float));
      }
      else if (orgTIDLNetStructure->TIDLPCLayers[i].layerType == TIDL_CustomLayer)
      {
        int32_t offset, size ;
        offset = TIDL_alignParamsWrite(fp1, &orgTIDLNetStructure->TIDLPCLayers[i].weights, &totalParamSize,1);
        size = totalParamSize - offset ;

        tIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.customParams.rsvdPassThrough = offset ;
        tIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.customParams.rsvdPassThroughSize = size ;
      }
      tiLayerIndex++;
    }
  }

  tIDLNetStructure->netBufInfo[TIDL_NET_PARAMS_BUF].size =
    totalParamSize - tIDLNetStructure->netBufInfo[TIDL_NET_PARAMS_BUF].offset ;

  tempBuf.bufSize = sizeof(tmpPerfSim->graphCompOut); tempBuf.ptr = (void*) &tmpPerfSim->graphCompOut ;
  memset(&tmpPerfSim->graphCompOut, 0, sizeof(tmpPerfSim->graphCompOut));
  offset = TIDL_alignParamsWrite(fp1, &tempBuf, &totalParamSize, 1);
  tIDLNetStructure->netBufInfo[TIDL_NET_GC_BUF].offset = offset ;
  tIDLNetStructure->netBufInfo[TIDL_NET_GC_BUF].size = totalParamSize -
  tIDLNetStructure->netBufInfo[TIDL_NET_GC_BUF].offset ;
  free(tmpPerfSim);

  /* Needs to be written before stats collection inference call, else can stay uninitialized in inference pass and unpredictable behaviour */
  tIDLNetStructure->numCores = gParams.numCores; /* numCores is read from import config/device config and populated here */

  if (fp1 != NULL)
  {
    fclose(fp1);
  }
  return 0;
}

void TIDL_UpdateInDataBuffId(sTIDL_OrgNetwork_t * pOrgTIDLNetStructure,
  uint32_t numLayers, sTIDL_DataParams_t dataBuf,  sTIDL_DataParams_t newDataBuf)
{
  uint32_t i, j;
  for (i = 0; i < numLayers; i++)
  {
    for (j = 0; (j < pOrgTIDLNetStructure->TIDLPCLayers[i].numInBufs) &&
      (pOrgTIDLNetStructure->TIDLPCLayers[i].numInBufs > 0); j++)
    {
      if (pOrgTIDLNetStructure->TIDLPCLayers[i].inData[j].dataId ==
        dataBuf.dataId)
      {
        pOrgTIDLNetStructure->TIDLPCLayers[i].inData[j].dataId = newDataBuf.dataId;
      }
    }

  }

  return;
}


void TIDL_UpdateInDataBuffIdInLayers(sTIDL_OrgNetwork_t * pOrgTIDLNetStructure,
  uint32_t numLayers, sTIDL_DataParams_t dataBuf,  sTIDL_DataParams_t newDataBuf,
  std::vector<int32_t>& layers)
{
  int32_t j;
  for (int32_t i : layers)
  {
    for (j = 0; (j < pOrgTIDLNetStructure->TIDLPCLayers[i].numInBufs) &&
      (pOrgTIDLNetStructure->TIDLPCLayers[i].numInBufs > 0); j++)
    {
      if (pOrgTIDLNetStructure->TIDLPCLayers[i].inData[j].dataId ==
        dataBuf.dataId)
      {
        pOrgTIDLNetStructure->TIDLPCLayers[i].inData[j].dataId = newDataBuf.dataId;
      }
    }

  }

  return;
}


void  tidl_updateInDataBatchPadValues(sTIDL_OrgNetwork_t * tIDLNetStructure)
{
  int32_t i, j, k, l;
  int32_t foundInData;
  for (i = 0; i < tIDLNetStructure->numLayers; i++)
  {
    for (j = 0; j < tIDLNetStructure->TIDLPCLayers[i].numInBufs; j++)
    {
      foundInData = 0;
      for (k = 0; ((k < tIDLNetStructure->numLayers) && (foundInData == 0)); k++)
      {
        for (l = 0; ((l < tIDLNetStructure->TIDLPCLayers[k].numOutBufs) && (foundInData == 0)); l++)
        {
          if (tIDLNetStructure->TIDLPCLayers[i].inData[j].dataId == tIDLNetStructure->TIDLPCLayers[k].outData[l].dataId)
          {
            tIDLNetStructure->TIDLPCLayers[i].inData[j].batchPadW = tIDLNetStructure->TIDLPCLayers[k].outData[l].batchPadW;
            tIDLNetStructure->TIDLPCLayers[i].inData[j].batchPadH = tIDLNetStructure->TIDLPCLayers[k].outData[l].batchPadH;
            foundInData = 1;
          }
        }
      }
      if (foundInData == 0)
      {
        printf("Could not find Indata for data ID %d \n", tIDLNetStructure->TIDLPCLayers[i].inData[j].dataId);
      }
    }
  }
}

float TIDL_getOutputFactor(sTIDL_OrgNetwork_t * tIDLNetStructure, int32_t layer, int32_t inId, int32_t outId)
{
  float outFactor = 1.0;
  if ((tIDLNetStructure->TIDLPCLayers[layer].outData[outId].dimValues[TIDL_DIM_WIDTH] != 0) && (tIDLNetStructure->TIDLPCLayers[layer].inData[inId].dimValues[TIDL_DIM_WIDTH] != 0))
  {
    if (tIDLNetStructure->TIDLPCLayers[layer].inData[inId].dimValues[TIDL_DIM_WIDTH] > tIDLNetStructure->TIDLPCLayers[layer].outData[outId].dimValues[TIDL_DIM_WIDTH])
      outFactor = (float)ALIGN(tIDLNetStructure->TIDLPCLayers[layer].inData[inId].dimValues[TIDL_DIM_WIDTH], tIDLNetStructure->TIDLPCLayers[layer].outData[outId].dimValues[TIDL_DIM_WIDTH])/tIDLNetStructure->TIDLPCLayers[layer].outData[outId].dimValues[TIDL_DIM_WIDTH];
    else
      outFactor = (float)tIDLNetStructure->TIDLPCLayers[layer].inData[inId].dimValues[TIDL_DIM_WIDTH]/tIDLNetStructure->TIDLPCLayers[layer].outData[outId].dimValues[TIDL_DIM_WIDTH];
  }
  return outFactor;
}

void  tidl_initAndUpdateBatchPadRequirements(sTIDL_OrgNetwork_t * tIDLNetStructure)
{
  int32_t i, j, k, l;
  int32_t padW, padH, curPadW, curPadH;
  int32_t strideW, strideH;
  int32_t numUpdates;
  /* Below loop initializes output batchPadW for each layer based on it's consumer layer pad requirement */
  for (i = 0; i < tIDLNetStructure->numLayers; i++)
  {
    for (j = 0; j < tIDLNetStructure->TIDLPCLayers[i].numOutBufs; j++)
    {
      padW = 0;
      padH = 0;
      if (TIDL_doesLayerSupportBatchProcessing(&tIDLNetStructure->TIDLPCLayers[i]))
      {
        for (k = i + 1; k < tIDLNetStructure->numLayers; k++)
        {
          for (l = 0; l < tIDLNetStructure->TIDLPCLayers[k].numInBufs; l++)
          {
            curPadW = 0;
            curPadH = 0;
            if (tIDLNetStructure->TIDLPCLayers[i].outData[j].dataId == tIDLNetStructure->TIDLPCLayers[k].inData[l].dataId)
            {
              if ((tIDLNetStructure->TIDLPCLayers[k].layerType == TIDL_ConvolutionLayer) ||
                (tIDLNetStructure->TIDLPCLayers[k].layerType == TIDL_Deconv2DLayer))
              {
                strideW = tIDLNetStructure->TIDLPCLayers[k].layerParams.convParams.strideW;
                strideH = tIDLNetStructure->TIDLPCLayers[k].layerParams.convParams.strideH;
                curPadW = MAX(strideW, tIDLNetStructure->TIDLPCLayers[k].layerParams.convParams.padW);
                curPadH = MAX(strideH, tIDLNetStructure->TIDLPCLayers[k].layerParams.convParams.padH);
              }
              else if (tIDLNetStructure->TIDLPCLayers[k].layerType == TIDL_PoolingLayer)
              {
                strideW = tIDLNetStructure->TIDLPCLayers[k].layerParams.poolParams.strideW;
                strideH = tIDLNetStructure->TIDLPCLayers[k].layerParams.poolParams.strideH;
                if (tIDLNetStructure->TIDLPCLayers[k].layerParams.poolParams.padW)
                {
                  curPadW = tIDLNetStructure->TIDLPCLayers[k].layerParams.poolParams.padW;
                }
                else
                {
                  /* Pooling needs one extra line padding when useCeil is enabled and input width is odd */
                  if((tIDLNetStructure->TIDLPCLayers[k].layerParams.poolParams.useCeil) && (tIDLNetStructure->TIDLPCLayers[k].inData[0].dimValues[TIDL_DIM_WIDTH] & 1))
                  {
                    curPadW = ceil((tIDLNetStructure->TIDLPCLayers[k].layerParams.poolParams.kernelW - 1) / 2.0);
                  }
                  else
                  {
                    curPadW = floor(((tIDLNetStructure->TIDLPCLayers[k].layerParams.poolParams.kernelW - 1) / 2.0));
                  }
                }
                if (tIDLNetStructure->TIDLPCLayers[k].layerParams.poolParams.padH)
                {
                  curPadH = tIDLNetStructure->TIDLPCLayers[k].layerParams.poolParams.padH;
                }
                else
                {
                  /* Pooling needs one extra line padding when useCeil is enabled and input height is odd */
                  if((tIDLNetStructure->TIDLPCLayers[k].layerParams.poolParams.useCeil) && (tIDLNetStructure->TIDLPCLayers[k].inData[0].dimValues[TIDL_DIM_HEIGHT] & 1))
                  {
                    curPadH = ceil((tIDLNetStructure->TIDLPCLayers[k].layerParams.poolParams.kernelH - 1) / 2.0);
                  }
                  else
                  {
                    curPadH = floor(((tIDLNetStructure->TIDLPCLayers[k].layerParams.poolParams.kernelH - 1) / 2.0));
                  }
                }
                curPadW = MAX(strideW, curPadW);
                curPadH = MAX(strideH, curPadH);
              }
              else if (tIDLNetStructure->TIDLPCLayers[k].layerType == TIDL_ResizeLayer)
              {
                /* Resize layer needs minimum pad of 1 */
                curPadW = 1;
                curPadH = 1;
              }
              else if (tIDLNetStructure->TIDLPCLayers[k].layerType == TIDL_CustomLayer)
              {
                curPadW = tIDLNetStructure->TIDLPCLayers[k].layerParams.customParams.padW;
                curPadH = tIDLNetStructure->TIDLPCLayers[k].layerParams.customParams.padH;
              }
              else
              {
                curPadW = 0;
                curPadH = 0;
              }

              if (curPadW > padW) padW = curPadW;
              if (curPadH > padH) padH = curPadH;
            }
          }
        }
      }
      tIDLNetStructure->TIDLPCLayers[i].outData[j].batchPadW = padW;
    }
  }
  /* Copy output batchPadW values to corresponding input databufs */
  tidl_updateInDataBatchPadValues(tIDLNetStructure);

  do
  {
    int strideW, strideH;
    numUpdates = 0;

    /* For loop to check if input batchPadW is lessthan the output batchPadW, then adjust input batchPadW */
    for (i = 0; i < tIDLNetStructure->numLayers; i++)
    {
      for (j = 0; j < tIDLNetStructure->TIDLPCLayers[i].numOutBufs; j++)
      {
        for (k = 0; k < tIDLNetStructure->TIDLPCLayers[i].numInBufs; k++)
        {
          float outFactor = TIDL_getOutputFactor(tIDLNetStructure, i, k, j);
          if ((tIDLNetStructure->TIDLPCLayers[i].inData[k].batchPadW) < (tIDLNetStructure->TIDLPCLayers[i].outData[j].batchPadW * outFactor))
          {
            tIDLNetStructure->TIDLPCLayers[i].inData[k].batchPadW = tIDLNetStructure->TIDLPCLayers[i].outData[j].batchPadW * outFactor;
            numUpdates++;
          }
        }
      }
    }
    /* Copy the updated input batchPad values to outData buffers of its input layers */
    for (i = 0; i < tIDLNetStructure->numLayers; i++)
    {
      for (j = 0; j < tIDLNetStructure->TIDLPCLayers[i].numOutBufs; j++)
      {
        for (k = i + 1; k < tIDLNetStructure->numLayers; k++)
        {
          for (l = 0; l < tIDLNetStructure->TIDLPCLayers[k].numInBufs; l++)
          {
            if (tIDLNetStructure->TIDLPCLayers[i].outData[j].dataId == tIDLNetStructure->TIDLPCLayers[k].inData[l].dataId)
            {
              if (tIDLNetStructure->TIDLPCLayers[k].layerType != TIDL_BatchReshapeLayer)
              {
                tIDLNetStructure->TIDLPCLayers[i].outData[j].batchPadW = MAX(tIDLNetStructure->TIDLPCLayers[i].outData[j].batchPadW, tIDLNetStructure->TIDLPCLayers[k].inData[l].batchPadW);
              }
            }
          }
        }
      }
    }

    /* For loop to check if input batchPadW is morethan the output batchPadW * outFactor, then adjust outData batchPadW */
    for (i = 0; i < tIDLNetStructure->numLayers; i++)
    {
      if (tIDLNetStructure->TIDLPCLayers[i].layerType != TIDL_BatchReshapeLayer)
      {
        for (j = 0; j < tIDLNetStructure->TIDLPCLayers[i].numOutBufs; j++)
        {
          for (k = 0; k < tIDLNetStructure->TIDLPCLayers[i].numInBufs; k++)
          {
            float outFactor = TIDL_getOutputFactor(tIDLNetStructure, i, k, j);
            if ((tIDLNetStructure->TIDLPCLayers[i].inData[k].batchPadW) > (tIDLNetStructure->TIDLPCLayers[i].outData[j].batchPadW* outFactor))
            {
              tIDLNetStructure->TIDLPCLayers[i].outData[j].batchPadW = MAX (tIDLNetStructure->TIDLPCLayers[i].outData[j].batchPadW, (tIDLNetStructure->TIDLPCLayers[i].inData[k].batchPadW/outFactor));
              tidl_updateInDataBatchPadValues(tIDLNetStructure);
            }
          }
        }
      }
    }
  } while (numUpdates > 0);

  /* This below for loop is required to get output dimensions for TIDL_UnStitchLayer, need to revisit and simplify the logic */
  for (i = 0; i < tIDLNetStructure->numLayers; i++)
  {
    for (j = 0; j < tIDLNetStructure->TIDLPCLayers[i].numOutBufs; j++)
    {
      for (k = i + 1; k < tIDLNetStructure->numLayers; k++)
      {
        for (l = 0; l < tIDLNetStructure->TIDLPCLayers[k].numInBufs; l++)
        {
          curPadW = 0;
          curPadH = 0;
          if (tIDLNetStructure->TIDLPCLayers[i].outData[j].dataId == tIDLNetStructure->TIDLPCLayers[k].inData[l].dataId)
          {
            if (tIDLNetStructure->TIDLPCLayers[k].layerType == TIDL_BatchReshapeLayer)
            {
              /* If the consumer layer is UnStitch layer then the current layer output batch pad need to be updated with input_Pad/stride
                 and this will eventually copied to UnStitchLayer input batch Pad */
              if ((tIDLNetStructure->TIDLPCLayers[i].layerType == TIDL_ConvolutionLayer) ||
                (tIDLNetStructure->TIDLPCLayers[i].layerType == TIDL_Deconv2DLayer))
              {
                curPadW = tIDLNetStructure->TIDLPCLayers[i].inData[j].batchPadW/tIDLNetStructure->TIDLPCLayers[i].layerParams.convParams.strideW;
                curPadH = tIDLNetStructure->TIDLPCLayers[i].inData[j].batchPadH/tIDLNetStructure->TIDLPCLayers[i].layerParams.convParams.strideH;
              }
              else if ((tIDLNetStructure->TIDLPCLayers[i].layerType == TIDL_PoolingLayer) &&
                      (tIDLNetStructure->TIDLPCLayers[i].layerParams.poolParams.kernelW > 0) &&
                      (tIDLNetStructure->TIDLPCLayers[i].layerParams.poolParams.kernelH > 0))
              {
                curPadW = tIDLNetStructure->TIDLPCLayers[i].inData[j].batchPadW/tIDLNetStructure->TIDLPCLayers[i].layerParams.poolParams.strideW;
                curPadH = tIDLNetStructure->TIDLPCLayers[i].inData[j].batchPadH/tIDLNetStructure->TIDLPCLayers[i].layerParams.poolParams.strideH;
              }
              else
              {
                curPadW = tIDLNetStructure->TIDLPCLayers[i].inData[j].batchPadW;
                curPadH = tIDLNetStructure->TIDLPCLayers[i].inData[j].batchPadH;
              }
              tIDLNetStructure->TIDLPCLayers[i].outData[j].batchPadW = MAX (curPadW, tIDLNetStructure->TIDLPCLayers[i].outData[j].batchPadW);
              tIDLNetStructure->TIDLPCLayers[i].outData[j].batchPadH = MAX (curPadH, tIDLNetStructure->TIDLPCLayers[i].outData[j].batchPadH);
            }
          }
        }
      }
    }
  }
}
int32_t tidl_getBatchPadReqForThisLayer(sTIDL_OrgNetwork_t * tIDLNetStructure, int32_t i)
{
  int32_t strideW = 1;

  if ((tIDLNetStructure->TIDLPCLayers[i].layerType == TIDL_ConvolutionLayer) ||
    (tIDLNetStructure->TIDLPCLayers[i].layerType == TIDL_Deconv2DLayer))
  {
    strideW = tIDLNetStructure->TIDLPCLayers[i].layerParams.convParams.strideW;
  }
  else if ((tIDLNetStructure->TIDLPCLayers[i].layerType == TIDL_PoolingLayer) &&
          (tIDLNetStructure->TIDLPCLayers[i].layerParams.poolParams.kernelW > 0) &&
          (tIDLNetStructure->TIDLPCLayers[i].layerParams.poolParams.kernelH > 0))
  {
    strideW = tIDLNetStructure->TIDLPCLayers[i].layerParams.poolParams.strideW;
  }
  strideW = MAX(1, strideW);
  return strideW;
}

void  TIDL_fillDataBufPadRequirements(sTIDL_Network_t * tIDLNetStructure)
{
  int32_t i, j, k, l;
  int32_t padW, padH, curPadW, curPadH;
  int32_t foundInData;
  for (i = 0; i < tIDLNetStructure->numLayers; i++)
  {
    for (j = 0; j < tIDLNetStructure->TIDLLayers[i].numOutBufs; j++)
    {
      padW = 0;
      padH = 0;

      for (k = i + 1; k < tIDLNetStructure->numLayers; k++)
      {
        for (l = 0; l < tIDLNetStructure->TIDLLayers[k].numInBufs; l++)
        {
          curPadW = 0;
          curPadH = 0;
          if (tIDLNetStructure->TIDLLayers[i].outData.dataId == tIDLNetStructure->TIDLLayers[k].inData[l])
          {
            if ((tIDLNetStructure->TIDLLayers[k].layerType == TIDL_ConvolutionLayer) ||
              (tIDLNetStructure->TIDLLayers[k].layerType == TIDL_Deconv2DLayer))
            {
              curPadW = tIDLNetStructure->TIDLLayers[k].layerParams.convParams.padW;
              curPadH = tIDLNetStructure->TIDLLayers[k].layerParams.convParams.padH;
            }
            else if (tIDLNetStructure->TIDLLayers[k].layerType == TIDL_PoolingLayer)
            {
              if (tIDLNetStructure->TIDLLayers[k].layerParams.poolParams.padW)
              {
                curPadW = tIDLNetStructure->TIDLLayers[k].layerParams.poolParams.padW;
              }
              else
              {
                /* Pooling needs one extra line padding when useCeil is enabled and input width is odd */
                if((tIDLNetStructure->TIDLLayers[k].layerParams.poolParams.useCeil) && (tIDLNetStructure->TIDLLayers[i].outData.dimValues[TIDL_DIM_WIDTH] & 1))
                {
                  curPadW = ceil((tIDLNetStructure->TIDLLayers[k].layerParams.poolParams.kernelW - 1) / 2.0);
                }
                else
                {
                  curPadW = floor(((tIDLNetStructure->TIDLLayers[k].layerParams.poolParams.kernelW - 1) / 2.0));
                }
              }
              if (tIDLNetStructure->TIDLLayers[k].layerParams.poolParams.padH)
              {
                curPadH = tIDLNetStructure->TIDLLayers[k].layerParams.poolParams.padH;
              }
              else
              {
                /* Pooling needs one extra line padding when useCeil is enabled and input height is odd */
                if((tIDLNetStructure->TIDLLayers[k].layerParams.poolParams.useCeil) && (tIDLNetStructure->TIDLLayers[i].outData.dimValues[TIDL_DIM_HEIGHT] & 1))
                {
                  curPadH = ceil((tIDLNetStructure->TIDLLayers[k].layerParams.poolParams.kernelH - 1) / 2.0);
                }
                else
                {
                  curPadH = floor(((tIDLNetStructure->TIDLLayers[k].layerParams.poolParams.kernelH - 1) / 2.0));
                }
              }
            }
            else if (tIDLNetStructure->TIDLLayers[k].layerType == TIDL_ResizeLayer)
            {
              /* Resize layer needs minimum pad of 1 */
              curPadW = 1;
              curPadH = 1;
            }
            else if (tIDLNetStructure->TIDLLayers[k].layerType == TIDL_CustomLayer)
            {
              curPadW = tIDLNetStructure->TIDLLayers[k].layerParams.customParams.padW;
              curPadH = tIDLNetStructure->TIDLLayers[k].layerParams.customParams.padH;
            }
            else if (tIDLNetStructure->TIDLLayers[k].layerType == TIDL_ColorConversionLayer)
            {
              //:TODO: Not entirely sure about why this is required but
              //:TODO: without this reference is getting
              curPadW = 1;
              curPadH = 1;
            }

            if (curPadW > padW) padW = curPadW;
            if (curPadH > padH) padH = curPadH;
          }
        }
      }
      tIDLNetStructure->TIDLLayers[i].outData.padW = padW;
      tIDLNetStructure->TIDLLayers[i].outData.padH = padH;
      tIDLNetStructure->TIDLLayers[i].outData.pitch[TIDL_LINE_PITCH] = tIDLNetStructure->TIDLLayers[i].outData.dimValues[TIDL_DIM_WIDTH] + tIDLNetStructure->TIDLLayers[i].outData.padW;
      /* Align pitch accross channel so that we dont access same bank */
      tIDLNetStructure->TIDLLayers[i].outData.pitch[TIDL_CHANNEL_PITCH] = (tIDLNetStructure->TIDLLayers[i].outData.dimValues[TIDL_DIM_HEIGHT] + 2 * tIDLNetStructure->TIDLLayers[i].outData.padH + 1) * tIDLNetStructure->TIDLLayers[i].outData.pitch[TIDL_LINE_PITCH];
      tIDLNetStructure->TIDLLayers[i].outData.pitch[TIDL_ROI_PITCH] = tIDLNetStructure->TIDLLayers[i].outData.dimValues[TIDL_DIM_NUMCH] * tIDLNetStructure->TIDLLayers[i].outData.pitch[TIDL_CHANNEL_PITCH];

      //tIDLNetStructure->TIDLLayers[i].outData.padW = 4;
      //tIDLNetStructure->TIDLLayers[i].outData[j].padH = 4;
    }
  }
}



int32_t tidltb_isOutDataBuff(const sTIDL_Network_t *pTIDLNetStructure, int32_t dataId,
  int32_t layersGroupId)
{
  int32_t i, j;
  for (i = 0; i < pTIDLNetStructure->numLayers; i++)
  {
    for (j = 0; j < pTIDLNetStructure->TIDLLayers[i].numInBufs; j++)
    {
      if ((pTIDLNetStructure->TIDLLayers[i].layersGroupId != layersGroupId) &&
        (pTIDLNetStructure->TIDLLayers[i].inData[j] == dataId))
      {
        return 1;
      }
    }
  }
  return 0;
}

int32_t tidltb_isInDataBuff(const sTIDL_Network_t * pTIDLNetStructure, int32_t dataId,
  int32_t layersGroupId)
{
  int32_t i, j;
  for (i = 0; i < pTIDLNetStructure->numLayers; i++)
  {
    for (j = 0; j < pTIDLNetStructure->TIDLLayers[i].numInBufs; j++)
    {
      if ((pTIDLNetStructure->TIDLLayers[i].layersGroupId == layersGroupId) &&
        (pTIDLNetStructure->TIDLLayers[i].inData[j] == dataId))
      {
        return 1;
      }
    }
  }
  return 0;
}
void TIDL_writeNInts(FILE * fp1, int32_t * intData, int32_t n, const char * name)
{
  fprintf(fp1, "%s = ", name);
  for (int32_t i = 0; i < n; i++)
  {
    fprintf(fp1, "%8d ", intData[i]);
  }
  fprintf(fp1, "\n");
}

int32_t tidl_getbufIdxFromDataNamesList(const char * str, char* dataNamesList)
{
  int32_t bufIdx = -1;
  char dataNames[TIDL_MAX_ALG_IN_BUFS][TIDL_MAX_DATA_NAME];
  if (strcmp((char*)dataNamesList, "") != 0)
  {
    /* Break Single line names into pieces */
    int32_t numBufs = tidl_getStringsFromList((char *)dataNamesList, (char*)dataNames, TIDL_MAX_DATA_NAME);
    for (int32_t i = 0; i < numBufs; i++)
    {
      if ((strcmp((const char*)dataNames[i], (const char*)(char*)str) == 0)||
          (strcmp((const char*)strcat((char *)dataNames[i], "_original"), (const char*)(char*)str) == 0))
      {
        bufIdx = i;
        break;
      }
    }
  }
  return bufIdx;
}

int32_t TIDL_isInDataBuff(const sTIDL_Network_t * pTIDLNetStructure,
int32_t dataId, int32_t layersGroupId)
{
  int32_t i,j;
  int32_t status = 0;
  for (i = 0 ; i < pTIDLNetStructure->numLayers; i++)
  {
    for (j = 0; j < pTIDLNetStructure->TIDLLayers[i].numOutBufs; j++)
    {
      if((pTIDLNetStructure->TIDLLayers[i].layersGroupId != layersGroupId) &&
          (pTIDLNetStructure->TIDLLayers[i].outData.dataId == dataId))
      {
        status = 1;
        break;
      }
    }
    if(status == 1)
    {
      break;
    }
  }

  return status;
}


sBufParams_t * TIDL_getInBufParams(const sTIDL_Network_t *net, int32_t prevdataId, int32_t curDataId ,int32_t currLayersGroupId)
{
  sBufParams_t *bufParams = NULL;
  sBufDataBase_t bufDBase;
  sBufDataBase_t *bufDB = &bufDBase;
  sGraphCompilerOutArgs_t *gcOutArgs = NULL;
  sGCCommonDataBase_t *commonDatabase = NULL;

  if ( net->graphCompilerInfoOffset != 0 )
  {
    gcOutArgs  = (sGraphCompilerOutArgs_t *)get_int8_t_pointer((int8_t *)(net), net->graphCompilerInfoOffset);
    commonDatabase = (sGCCommonDataBase_t *)getGCDataBase(gcOutArgs,DATABASE_COMMON);
    bufDB->pBufList = (sBufParams_t*) &commonDatabase->dbPayLoad[commonDatabase->dbParams[DB_COMMON_BUF].startOffset];
    bufDB->count = gcOutArgs->commonDatabaseInfo.bufDB.count;

    for(int32_t i = 0; i < bufDB->count; i++ )
    {
      bufParams = &bufDB->pBufList[i];
      sMetaDataID_t pMetaDataID;
      getMetaDataID(bufParams->dataId, &pMetaDataID);
      int32_t dataLayerId = pMetaDataID.layerId;
      /*bufParams have Data ID of the layer that produces the bufer not the one that consumes so using prevdataId*/
      if(prevdataId == dataLayerId && (TIDL_isInDataBuff(net, curDataId, currLayersGroupId) == 1) && (pMetaDataID.type == BUF_FM_FULL))
      {
        break;
      }
    }
  }
  return bufParams;
}

sBufParams_t * TIDL_getOutBufParams(const sTIDL_Network_t *net, int32_t dataId, int32_t currLayersGroupId)
{
  sBufParams_t *bufParams = NULL;
  sBufDataBase_t bufDBase;
  sBufDataBase_t *bufDB = &bufDBase;
  sGraphCompilerOutArgs_t *gcOutArgs = NULL;
  sGCCommonDataBase_t *commonDatabase = NULL;

  if ( net->graphCompilerInfoOffset != 0 )
  {
    gcOutArgs  = (sGraphCompilerOutArgs_t *)get_int8_t_pointer((int8_t *)(net), net->graphCompilerInfoOffset);
    commonDatabase = (sGCCommonDataBase_t *)getGCDataBase(gcOutArgs,DATABASE_COMMON);
    bufDB->pBufList = (sBufParams_t*) &commonDatabase->dbPayLoad[commonDatabase->dbParams[DB_COMMON_BUF].startOffset];
    bufDB->count = gcOutArgs->commonDatabaseInfo.bufDB.count;

    for(int32_t i = 0; i < bufDB->count; i++ )
    {
      bufParams = &bufDB->pBufList[i];
      sMetaDataID_t pMetaDataID;
      getMetaDataID(bufParams->dataId, &pMetaDataID);
      int32_t dataLayerId = pMetaDataID.layerId;

      if(dataId == dataLayerId && (tidltb_isOutDataBuff(net, dataLayerId, currLayersGroupId) == 1) && (pMetaDataID.type == BUF_FM_FULL))
      {
        break;
      }
    }
  }
  return bufParams;
}

int32_t TIDL_writeInfo(const sTIDL_Network_t * tIDLNetStructure,
                                       const sTIDL_OrgNetwork_t * orgTIDLNetStructure,
                                        const char * name,
                                        uint32_t numLayers,
                                        uint32_t currLayersGroupId,
                                        const sPerfSim_t * perfSimInfo,
                                        const tidl_import_config * configParams)
{
  FILE * fp1 = NULL;
  int32_t i, j, writeSize, tiLayerIndex;
  int32_t numDataBuf = 0;
  int32_t numOutBufs = 0;
  int32_t numInBufs = 0;
  int32_t knownOutBufs = 0;
  char fileName[500];
  char numString[500];
  strcpy(fileName, name);
  sprintf(numString, "%d", currLayersGroupId);
  strcat(fileName, numString);
  strcat(fileName, ".bin");
  sTIDL_IOBufDesc_t  gIOParams;

  if (fileName)
  {
    fp1 = fopen(fileName, "wb+");
    if (fp1 == NULL)
    {
      printf("ERROR: Could not open %s file for writing \n", (const char *)fileName);
      exit(-1);
    }
  }

  //Marking zero to allow easier binary comparison of 2 different execution
  memset(&gIOParams, 0, sizeof(sTIDL_IOBufDesc_t));
  numDataBuf = -1;
  for (i = 0; i < numLayers; i++)
  {
    if (tIDLNetStructure->TIDLLayers[i].layersGroupId != currLayersGroupId)
    {
      for (j = 0; j < tIDLNetStructure->TIDLLayers[i].numOutBufs; j++)
      {
        if (tidltb_isInDataBuff(tIDLNetStructure, tIDLNetStructure->TIDLLayers[i].outData.dataId,
          currLayersGroupId))
        {
          const char * str = TIDL_getOutDataName(orgTIDLNetStructure, tIDLNetStructure->TIDLLayers[i].outData.dataId);
          int32_t idx = tidl_getbufIdxFromDataNamesList(str, (char*)configParams->inDataNamesList);
          if (idx != -1)
            numDataBuf = idx;
          else
            numDataBuf++;
          if(str)
          {
            strcpy((char *)gIOParams.inDataName[numDataBuf], str);
          }

          gIOParams.inWidth[numDataBuf] = tIDLNetStructure->TIDLLayers[i].outData.dimValues[TIDL_DIM_WIDTH];
          gIOParams.inHeight[numDataBuf] = tIDLNetStructure->TIDLLayers[i].outData.dimValues[TIDL_DIM_HEIGHT];
          gIOParams.inNumChannels[numDataBuf] = tIDLNetStructure->TIDLLayers[i].outData.dimValues[TIDL_DIM_NUMCH];
          gIOParams.inDIM2[numDataBuf] = tIDLNetStructure->TIDLLayers[i].outData.dimValues[TIDL_DIM_DIM2];
          gIOParams.inDIM1[numDataBuf] = tIDLNetStructure->TIDLLayers[i].outData.dimValues[TIDL_DIM_DIM1];
          gIOParams.inNumBatches[numDataBuf] = tIDLNetStructure->TIDLLayers[i].outData.dimValues[TIDL_DIM_BATCH];

          gIOParams.inPadL[numDataBuf] = tIDLNetStructure->TIDLLayers[i].outData.padW;
          gIOParams.inPadT[numDataBuf] = tIDLNetStructure->TIDLLayers[i].outData.padH;
          gIOParams.inPadR[numDataBuf] = 0;

          if(configParams->inLayout[numDataBuf] == TIDL_LT_NHWC)
          {
            gIOParams.inChannelPitch[numDataBuf] = gIOParams.inWidth[numDataBuf] * gIOParams.inHeight[numDataBuf];
          }
          else
          {
            gIOParams.inChannelPitch[numDataBuf] = tIDLNetStructure->TIDLLayers[i].outData.pitch[TIDL_CHANNEL_PITCH];
          }
          // inTensorScale is only used in readRAWData() to convert RAW
          // quantized data to float data for STATS_COLLECTION_FLOAT inference.
          // tensorScale in tidlNet have been overwritten to 1.0 in this case,
          // retrieve the org tensorScale from the orgNet
          const sTIDL_DataParams_t *orgOD = TIDL_getOutData(orgTIDLNetStructure,
                            tIDLNetStructure->TIDLLayers[i].outData.dataId);
          if (orgOD != NULL)
          {
            if(gParams.quantizationStyle == TIDL_QuantStyleAsymNP2_TFL)
            {
              /** TIDL_QuantStyleAsymNP2_TFL expects model tensor scale as input tensor scale, so we set this as input tensor scale for TIDL network
               * The same scale value is populated here in IoBUfDesc
               * TIDL scale = 1 / Model scale
               * RT code expects TIDL scale definition to do data conversion, so populate the same in IoBufDesc
               */
              gIOParams.inTensorScale[numDataBuf] = 1.0 / orgOD->tensorScale;
            }
            else
            {
              gIOParams.inTensorScale[numDataBuf] = orgOD->tensorScale;
            }
          }

          int32_t totalHeight = (gIOParams.inChannelPitch[numDataBuf] +
                                   gIOParams.inWidth[numDataBuf]  + gIOParams.inPadL[numDataBuf]  -1)/ (gIOParams.inWidth[numDataBuf] + gIOParams.inPadL[numDataBuf]);
          gIOParams.inPadB[numDataBuf] = totalHeight - gIOParams.inPadT[numDataBuf] - gIOParams.inHeight[numDataBuf];

          gIOParams.inPadCh[numDataBuf] = 0;

          gIOParams.inElementType[numDataBuf] = tIDLNetStructure->TIDLLayers[i].outData.elementType;
          gIOParams.rawDataInElementType[numDataBuf] = configParams->rawDataInElementType[numDataBuf];
          gIOParams.inZeroPoint[numDataBuf] = configParams->inZeroPoint[numDataBuf];
          gIOParams.inLayout[numDataBuf] = configParams->inLayout[numDataBuf];
          gIOParams.inDataId[numDataBuf] = tIDLNetStructure->TIDLLayers[i].outData.dataId;
          gIOParams.inDataFormat[numDataBuf] = configParams->inDataFormat[numDataBuf];
          gIOParams.inResizeType[numDataBuf] = configParams->inResizeType[numDataBuf];
          if (configParams->resizeWidth[numDataBuf] == -1)
          {
            gIOParams.resizeWidth[numDataBuf] = gIOParams.inWidth[numDataBuf];
          }
          else
          {
            gIOParams.resizeWidth[numDataBuf] = configParams->resizeWidth[numDataBuf];
          }
          if (configParams->resizeHeight[numDataBuf] == -1)
          {
            gIOParams.resizeHeight[numDataBuf] = gIOParams.inHeight[numDataBuf];
          }
          else
          {
            gIOParams.resizeHeight[numDataBuf] = configParams->resizeHeight[numDataBuf];
          }
          numInBufs++;
        }
      }
    }
  }
  gIOParams.numInputBuf = numInBufs;
  numDataBuf = -1;
  numOutBufs = 0;
  /*Find out known output buffers to adhere the order provided by the user through the parameter outDataNamesList
    Any unknown output buffers will be placed after known bufferes in random order
  */
  for (i = 0; i < numLayers; i++)
  {
    if (tIDLNetStructure->TIDLLayers[i].layersGroupId == currLayersGroupId)
    {
      for (j = 0; j < tIDLNetStructure->TIDLLayers[i].numOutBufs; j++)
      {
        if (tidltb_isOutDataBuff(tIDLNetStructure, tIDLNetStructure->TIDLLayers[i].outData.dataId,
          currLayersGroupId))
        {
          const char * str = TIDL_getOutDataName(orgTIDLNetStructure, tIDLNetStructure->TIDLLayers[i].outData.dataId);
          int32_t idx = tidl_getbufIdxFromDataNamesList(str, (char*)configParams->outDataNamesList);
          if(idx != -1)
          {
            knownOutBufs++;
          }
        }
      }
    }
  }

  for (i = 0; i < numLayers; i++)
  {
    if (tIDLNetStructure->TIDLLayers[i].layersGroupId == currLayersGroupId)
    {
      for (j = 0; j < tIDLNetStructure->TIDLLayers[i].numOutBufs; j++)
      {
        if (tidltb_isOutDataBuff(tIDLNetStructure, tIDLNetStructure->TIDLLayers[i].outData.dataId,
          currLayersGroupId))
        {
          const char * str = TIDL_getOutDataName(orgTIDLNetStructure, tIDLNetStructure->TIDLLayers[i].outData.dataId);
          int32_t idx = tidl_getbufIdxFromDataNamesList(str, (char*)configParams->outDataNamesList);

          if (idx != -1)
          {
            numDataBuf = idx;
          }
          else
          {
            numDataBuf = knownOutBufs;
            knownOutBufs++;
          }

          if(str)
          {
            strcpy((char *)gIOParams.outDataName[numDataBuf], str);
          }
          gIOParams.outWidth[numDataBuf] = tIDLNetStructure->TIDLLayers[i].outData.dimValues[TIDL_DIM_WIDTH];
          gIOParams.outHeight[numDataBuf] = tIDLNetStructure->TIDLLayers[i].outData.dimValues[TIDL_DIM_HEIGHT];
          gIOParams.outNumChannels[numDataBuf] = tIDLNetStructure->TIDLLayers[i].outData.dimValues[TIDL_DIM_NUMCH];
          gIOParams.outDIM2[numDataBuf] = tIDLNetStructure->TIDLLayers[i].outData.dimValues[TIDL_DIM_DIM2];
          gIOParams.outDIM1[numDataBuf] = tIDLNetStructure->TIDLLayers[i].outData.dimValues[TIDL_DIM_DIM1];
          gIOParams.outNumBatches[numDataBuf] = tIDLNetStructure->TIDLLayers[i].outData.dimValues[TIDL_DIM_BATCH];
          gIOParams.outPadL[numDataBuf] = tIDLNetStructure->TIDLLayers[i].outData.padW;
          gIOParams.outPadT[numDataBuf] = tIDLNetStructure->TIDLLayers[i].outData.padH;
          gIOParams.outPadR[numDataBuf] = 0;
          if(gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_ONNX_RT && gParams.onnxProto != NULL)
          {
            /*Parse and update numOutBufs*/
            onnx::ModelProto model_proto;
            model_proto.ParseFromString(*gParams.onnxProto);
            auto onnxSubGraph = model_proto.graph();
            auto& tensorShape = onnxSubGraph.output(j).type().tensor_type().shape();
            gIOParams.numValidTensorDims[numDataBuf] = tensorShape.dim_size();
          }
          else
          {
            gIOParams.numValidTensorDims[numDataBuf] = TIDL_DIM_MAX;
          }
          if(configParams->outLayout[numDataBuf] == TIDL_LT_NHWC)
          {
            gIOParams.outChannelPitch[numDataBuf] = gIOParams.outWidth[numDataBuf]*gIOParams.outHeight[numDataBuf];
          }
          else
          {
            gIOParams.outChannelPitch[numDataBuf] = tIDLNetStructure->TIDLLayers[i].outData.pitch[TIDL_CHANNEL_PITCH];
          }


          int32_t totalHeight = (gIOParams.outChannelPitch[numDataBuf] +
                                   gIOParams.outWidth[numDataBuf] + gIOParams.outPadL[numDataBuf]  -1)/ (gIOParams.outWidth[numDataBuf] + gIOParams.outPadL[numDataBuf]);
          gIOParams.outPadB[numDataBuf] = totalHeight - gIOParams.outPadT[numDataBuf] - gIOParams.outHeight[numDataBuf];

          gIOParams.outPadCh[numDataBuf] = 0;
          if(perfSimInfo != NULL)
          {
            int32_t totOutMemReq = gIOParams.outChannelPitch[numDataBuf]  *  gIOParams.outNumChannels[numDataBuf] * gIOParams.outDIM1[numDataBuf] * gIOParams.outDIM2[numDataBuf] ;
            int32_t extraMemReq = 0;

            sBufParams_t * bufParams = TIDL_getOutBufParams(tIDLNetStructure, i, currLayersGroupId);
            int32_t elemSize = getByteCnt(bufParams->dataType);
            extraMemReq = ((bufParams->bufSize * elemSize) - totOutMemReq);
            if ( extraMemReq > 0 )
            {
              gIOParams.outPadCh[numDataBuf] = (extraMemReq + (gIOParams.outChannelPitch[numDataBuf] -1 )) /gIOParams.outChannelPitch[numDataBuf];
            }
          }

          if(1) //configParams->outElementType[numDataBuf] == -1)
          {
            gIOParams.outElementType[numDataBuf] = tIDLNetStructure->TIDLLayers[i].outData.elementType;
            if(gParams.quantizationStyle == TIDL_QuantStyleAsymNP2_TFL)
            {
              gIOParams.outTensorScale[numDataBuf] = 1.0 / tIDLNetStructure->TIDLLayers[i].outData.tensorScale;
            }
            else
            {
              gIOParams.outTensorScale[numDataBuf] = tIDLNetStructure->TIDLLayers[i].outData.tensorScale;
            }
          }
          else
          {
            gIOParams.outElementType[numDataBuf] = configParams->outElementType[numDataBuf];
            gIOParams.outTensorScale[numDataBuf] = configParams->outTensorScale[numDataBuf];
          }

          if(gParams.quantizationStyle == TIDL_QuantStyleAsymNP2_TFL)
          {
            gIOParams.outZeroPoint[numDataBuf] = tIDLNetStructure->TIDLLayers[i].outData.tensorZeroPoint; /* tensor zero point is correctly populated as part of this flow */
          }
          else
          {
            gIOParams.outZeroPoint[numDataBuf] = configParams->outZeroPoint[numDataBuf];
          }

          gIOParams.outLayout[numDataBuf] = configParams->outLayout[numDataBuf];


          gIOParams.outDataId[numDataBuf] = tIDLNetStructure->TIDLLayers[i].outData.dataId;
          numOutBufs++;
        }
      }
    }
  }
  gIOParams.numOutputBuf = numOutBufs;

  /* Update IO params for multi batch - do not update in case of import quantization pass, only update for final write after perfsim
     Replicate params for all cores */
  if((configParams->inferenceMode == TIDL_inferenceModeHighThroughput) && (perfSimInfo != NULL)) //  || (configParams->numParamBits == 32))) /* TODO_NG: numParamBits = 32 case needs to be handled for multi core batch processing
  {
    printf("\n\n******************** Updating iobuf desc parameters for multi core batch processing *********************** \n\n");
    int8_t dataNameStr[TIDL_STRING_SIZE];
    for(int numCore = 1; numCore < multiCoreBatchProcInfo.numBatches; numCore++)
    {
      for(int numBuff = 0; numBuff < numOutBufs; numBuff++)
      {
        gIOParams.outElementType[numCore * numOutBufs + numBuff] = gIOParams.outElementType[numBuff];
        gIOParams.outTensorScale[numCore * numOutBufs + numBuff] = gIOParams.outTensorScale[numBuff];
        gIOParams.outZeroPoint[numCore * numOutBufs + numBuff] = gIOParams.outZeroPoint[numBuff];
        gIOParams.outLayout[numCore * numOutBufs + numBuff] = gIOParams.outLayout[numBuff];
        gIOParams.outPadL[numCore * numOutBufs + numBuff] = gIOParams.outPadL[numBuff];
        gIOParams.outPadR[numCore * numOutBufs + numBuff] = gIOParams.outPadR[numBuff];
        gIOParams.outPadT[numCore * numOutBufs + numBuff] = gIOParams.outPadT[numBuff];
        gIOParams.outPadB[numCore * numOutBufs + numBuff] = gIOParams.outPadB[numBuff];
        gIOParams.outPadCh[numCore * numOutBufs + numBuff] = gIOParams.outPadCh[numBuff];
        gIOParams.outChannelPitch[numCore * numOutBufs + numBuff] = gIOParams.outChannelPitch[numBuff];
        gIOParams.outWidth[numCore * numOutBufs + numBuff] = gIOParams.outWidth[numBuff];
        gIOParams.outHeight[numCore * numOutBufs + numBuff] = gIOParams.outHeight[numBuff];
        gIOParams.outNumChannels[numCore * numOutBufs + numBuff] = gIOParams.outNumChannels[numBuff];
        gIOParams.outDIM2[numCore * numOutBufs + numBuff] = gIOParams.outDIM2[numBuff];
        gIOParams.outDIM1[numCore * numOutBufs + numBuff] = gIOParams.outDIM1[numBuff];
        gIOParams.outNumBatches[numCore * numOutBufs + numBuff] = gIOParams.outNumBatches[numBuff];

        /* Update data names for replicated outputs so they can be correctly mapped as distinct RT tensors */
        strcpy((char *)gIOParams.outDataName[numCore * numOutBufs + numBuff], (char *)gIOParams.outDataName[numBuff]);
        sprintf((char *)dataNameStr, "%d", numCore * numOutBufs + numBuff);
        strcat(strcat((char *)gIOParams.outDataName[numCore * numOutBufs + numBuff], "_"), (char *)dataNameStr);
      }

      for(int numBuff = 0; numBuff < numInBufs; numBuff++)
      {
        gIOParams.inElementType[numCore * numInBufs + numBuff] = gIOParams.inElementType[numBuff];
        gIOParams.inTensorScale[numCore * numInBufs + numBuff] = gIOParams.inTensorScale[numBuff];
        gIOParams.inZeroPoint[numCore * numInBufs + numBuff] = gIOParams.inZeroPoint[numBuff];
        gIOParams.inLayout[numCore * numInBufs + numBuff] = gIOParams.inLayout[numBuff];
        gIOParams.inPadL[numCore * numInBufs + numBuff] = gIOParams.inPadL[numBuff];
        gIOParams.inPadR[numCore * numInBufs + numBuff] = gIOParams.inPadR[numBuff];
        gIOParams.inPadT[numCore * numInBufs + numBuff] = gIOParams.inPadT[numBuff];
        gIOParams.inPadB[numCore * numInBufs + numBuff] = gIOParams.inPadB[numBuff];
        gIOParams.inChannelPitch[numCore * numInBufs + numBuff] = gIOParams.inChannelPitch[numBuff];
        gIOParams.inWidth[numCore * numInBufs + numBuff] = gIOParams.inWidth[numBuff];
        gIOParams.inHeight[numCore * numInBufs + numBuff] = gIOParams.inHeight[numBuff];
        gIOParams.inNumChannels[numCore * numInBufs + numBuff] = gIOParams.inNumChannels[numBuff];
        gIOParams.inDIM2[numCore * numInBufs + numBuff] = gIOParams.inDIM2[numBuff];
        gIOParams.inDIM1[numCore * numInBufs + numBuff] = gIOParams.inDIM1[numBuff];
        gIOParams.inNumBatches[numCore * numInBufs + numBuff] = gIOParams.inNumBatches[numBuff];
        gIOParams.inDataFormat[numCore * numInBufs + numBuff] = gIOParams.inDataFormat[numBuff];
        gIOParams.resizeWidth[numCore * numInBufs + numBuff] = gIOParams.resizeWidth[numBuff];
        gIOParams.resizeHeight[numCore * numInBufs + numBuff] = gIOParams.resizeHeight[numBuff];
        gIOParams.inResizeType[numCore * numInBufs + numBuff] = gIOParams.inResizeType[numBuff];

        /* Update data names for replicated outputs so they can be correctly mapped as distinct RT tensors */
        strcpy((char *)gIOParams.inDataName[numCore * numInBufs + numBuff], (char *)gIOParams.inDataName[numBuff]);
        sprintf((char *)dataNameStr, "%d", numCore * numInBufs + numBuff);
        strcat(strcat((char *)gIOParams.inDataName[numCore * numInBufs + numBuff], "_"), (char *)dataNameStr);
      }
    }
    gIOParams.numInputBuf = numInBufs * multiCoreBatchProcInfo.numBatches;
    gIOParams.numOutputBuf = numOutBufs * multiCoreBatchProcInfo.numBatches;
    gIOParams.numVirtualCores = multiCoreBatchProcInfo.numCores;
    gIOParams.numSuperBatches = multiCoreBatchProcInfo.numBatches / multiCoreBatchProcInfo.numCores; /* Number of iterations of batch processing */
    gIOParams.numCores = multiCoreBatchProcInfo.numCores;
  }
  else
  {
    gIOParams.numVirtualCores = 1;
    gIOParams.numSuperBatches = 1;
    gIOParams.numCores = configParams->numCores;
  }

  gIOParams.inferenceMode = configParams->inferenceMode;

  fwrite(&gIOParams, 1, sizeof(sTIDL_IOBufDesc_t), fp1);
  fclose(fp1);
  return 0;
}


int32_t TIDL_tfOutReshapeDataLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  return 0;

}

int32_t TIDL_tfOutReshapeConstDataLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  return 0;
}
int32_t TIDL_tfOutReshapeConvLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  sTIDL_ConvParams_t &convParams = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].layerParams.convParams;

  TIDLPCLayers.outData[0].elementType = tidl_getElementType(1);
  if((gParams.modelType == 3) || (gParams.modelType == 5))
  {
    if(TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH] != convParams.numInChannels)
    {
      convParams.numGroups = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH]/convParams.numInChannels;
      if((convParams.numGroups *convParams.numInChannels) != TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH])
      {
        printf("convParams.numInChannels Is not multiple of convParams.numGroups -  Exiting \n");
        //exit(0);
      }
      convParams.numInChannels = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH];
    }
  }

  if((TIDLPCLayers.actParams.actType == TIDL_RelU6) || (TIDLPCLayers.actParams.actType == TIDL_RelU) ||
    ((TIDLPCLayers.actParams.actType == TIDL_Clip) && (TIDLPCLayers.actParams.clipMin >= 0)))
  {
    TIDLPCLayers.outData[0].elementType = tidl_getElementType(0);
  }
  /* Conversting 2x2 kernelw ith stride 2x2 to 3x3 kernel */
  if((convParams.kernelH == 2) &&
    (convParams.kernelW == 2) &&
    (convParams.strideW == 2) &&
    (convParams.strideH == 2) &&
    (convParams.dilationW == 1) &&
    (convParams.dilationH == 1))
  {
    convParams.kernelW = 3;
    convParams.kernelH = 3;
    convParams.padW = 1;
    convParams.padH = 1;
    if(TIDLPCLayers.weights.ptr != NULL)
    {
      float*  paddedweightsptr;
      float*  weightsptr;
      int32_t paddedsize;
      paddedsize = 9*TIDLPCLayers.weights.bufSize/4;
      paddedweightsptr = (float *)my_malloc(paddedsize*sizeof(float));
      weightsptr =  (float*) TIDLPCLayers.weights.ptr;
      for (int j = 0; j < TIDLPCLayers.weights.bufSize/4; j++)
      {
          paddedweightsptr[j*9+0] = 0;
          paddedweightsptr[j*9+1] = 0;
          paddedweightsptr[j*9+2] = 0;
          paddedweightsptr[j*9+3] = 0;
          paddedweightsptr[j*9+4] = weightsptr[j*4+0];
          paddedweightsptr[j*9+5] = weightsptr[j*4+1];
          paddedweightsptr[j*9+6] = 0;
          paddedweightsptr[j*9+7] = weightsptr[j*4+2];
          paddedweightsptr[j*9+8] = weightsptr[j*4+3];
      }
      my_free(weightsptr);
      TIDLPCLayers.weights.bufSize = paddedsize;
      TIDLPCLayers.weights.ptr = paddedweightsptr;
    }
    if((gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TFLITE) || (gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TENSORFLOW)
     ||(gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TFLITE_RT))
    {
      TIDLPCLayers.strideOffsetMethod = TIDL_StrideOffsetTopLeft;
    }
  }

  TIDLPCLayers.outData[0].numDim = TIDLPCLayers.inData[0].numDim;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_BATCH];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM1];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM2];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] = convParams.numOutChannels;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = ((TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT] + (convParams.padH * 2) -
    ((convParams.kernelH - 1)* convParams.dilationH + 1)) / convParams.strideH) + 1;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = ((TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH] + (convParams.padW * 2) -
    ((convParams.kernelW - 1)* convParams.dilationW + 1)) / convParams.strideW) + 1;

  convParams.numInChannels = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH];

  TIDLPCLayers.numMacs =
    (int64_t)(((int64_t)TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] *TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2] *TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] *
      TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] *
      convParams.kernelW *convParams.kernelH *
      TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH]) / convParams.numGroups);
  /*
  * Setting the pad parameter just in case it is not set or when we have strided case as pad does not correctly calculated for Strided Case
  */
  if(convParams.padL==-1 || convParams.padR==-1 || convParams.padT==-1 || convParams.padB==-1 || convParams.strideW > 1 || convParams.strideH > 1)
  {
    tidl_setLRTBpad(TIDLPCLayers);
  }

  /*Onnx Specific criteria:*/
  if((((convParams.kernelH % convParams.strideH) == 1)  || ((convParams.kernelW % convParams.strideW) == 1)) &&  (TIDLPCLayers.strideOffsetMethod == TIDL_StrideOffsetTopLeft) &&
  ((gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_ONNX) || (gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_ONNX_RT)))
  {
    tidl_setLRTBpad(TIDLPCLayers);
  }


  /*
   * Handle Random Coeff Case
   */
  int32_t dataSize;

  dataSize = convParams.kernelH * convParams.kernelW *
             convParams.numOutChannels *
             TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH] / convParams.numGroups;

  if(TIDLPCLayers.weights.ptr == NULL ||
     TIDLPCLayers.weights.bufSize != dataSize)
  {
    printf("WARNING: Conv Layer %s's coeff cannot be found(or not match) in coef file, "
           "Random coeff will be generated! "
           "Only for evaluation usage! "
           "Results are all random!\n", TIDLPCLayers.name);
    // TVM/Relay import wants to abort on this error ("warning")
    if (gParams.modelType == 4)  exit(-1);

    float*  data;
    data = (float *)my_malloc(dataSize*sizeof(float));

    for (int i = 0; i < dataSize; i++) {
      int32_t val = (rand() & (0X7FFFFFFF));
      data[i] = ((float)((rand() & 1) ? val : -val)) / (0X3FFFFF);
    }

    TIDLPCLayers.weights.ptr = data;
    TIDLPCLayers.weights.bufSize = dataSize;
  }

  /*
   * Handle Random Bias Case
   */
  dataSize = convParams.numOutChannels;

  if(convParams.enableBias &&
    (TIDLPCLayers.bias.ptr == NULL ||
     TIDLPCLayers.bias.bufSize != dataSize))
  {
    printf("WARNING: Conv Layer %s's bias cannot be found(or not match) in coef file, "
           "Random bias will be generated! "
           "Only for evaluation usage! "
           "Results are all random!\n", TIDLPCLayers.name);
    // TVM/Relay import wants to abort on this error ("warning")
    if (gParams.modelType == 4)  exit(-1);

    float*  data;
    data = (float *)my_malloc(dataSize*sizeof(float));

    for (int i = 0; i < dataSize; i++) {
      int32_t val = (rand() & (0X7FFFFFFF));
      data[i] = ((float)((rand() & 1) ? val : -val)) / (0XFFFFFF);
    }

    TIDLPCLayers.bias.ptr = data;
    TIDLPCLayers.bias.bufSize = dataSize;
  }

  return 0;
}

int32_t TIDL_tfOutReshapeResize(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  sTIDL_ConvParams_t &convParams = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].layerParams.convParams;

  TIDLPCLayers.outData[0].elementType = TIDLPCLayers.inData[0].elementType;
  TIDLPCLayers.outData[0].numDim = TIDLPCLayers.inData[0].numDim;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_BATCH];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM1];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM2];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH];
  if ((pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].layerParams.resizeParams.resizeRatio[TIDL_DIM_HEIGHT] < 0) ||
    (pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].layerParams.resizeParams.resizeRatio[TIDL_DIM_WIDTH] < 0))
  {
    pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].layerParams.resizeParams.resizeRatio[TIDL_DIM_HEIGHT] =
      -pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].layerParams.resizeParams.resizeRatio[TIDL_DIM_HEIGHT] / TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT];
    pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].layerParams.resizeParams.resizeRatio[TIDL_DIM_WIDTH] =
      -pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].layerParams.resizeParams.resizeRatio[TIDL_DIM_WIDTH] / TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH];
  }
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT] * pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].layerParams.resizeParams.resizeRatio[TIDL_DIM_HEIGHT];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH] * pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].layerParams.resizeParams.resizeRatio[TIDL_DIM_WIDTH];

  TIDLPCLayers.numMacs =
    (int64_t)((int64_t)TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH]* TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] *TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] *
      TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] * 4);
  return 0;
}


int32_t TIDL_tfOutReshapePoolingLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  sTIDL_PoolingParams_t &poolParams = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].layerParams.poolParams;
  TIDLPCLayers.outData[0].elementType = TIDLPCLayers.inData[0].elementType;
  TIDLPCLayers.outData[0].numDim = TIDLPCLayers.inData[0].numDim;

  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM1];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM2];

  int32_t isGlobalAvgPooling = (poolParams.kernelH  == TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT]) &&
                            (poolParams.kernelW  == TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH]) &&
                            (poolParams.poolingType == TIDL_AveragePooling);

  if(isGlobalAvgPooling)
  {
    poolParams.kernelH = 0;
    poolParams.kernelW = 0;
    poolParams.padW    = 0;
    poolParams.padH    = 0;
    poolParams.padT    = 0;
    poolParams.padB    = 0;
    poolParams.padL    = 0;
    poolParams.padR    = 0;

  }

  if((gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TFLITE) || (gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TFLITE_RT))
  {
    if((poolParams.strideH == 2) && ((TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT] & 1)  == 0))
    {
      TIDLPCLayers.strideOffsetMethod = TIDL_StrideOffsetCenter; /*for tflite padding should be set according to TIDL_StrideOffsetCenter*/
    }
  }

  if (poolParams.kernelH > 0 || poolParams.kernelW > 0)
  {
    poolParams.numChannels = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH];

    TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_BATCH];
    TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH];
    if(poolParams.strideH > 1)
    {
      if(poolParams.useCeil)
      {
        TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = ceil(((TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT] +
          poolParams.padH*2.0) - (poolParams.kernelH)) / poolParams.strideH) + 1;
      }
      else
      {
        TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = floor(((TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT] +
          poolParams.padH*2.0) - (poolParams.kernelH)) / poolParams.strideH) + 1;
      }
    }
    else
    {
      if(poolParams.padH > 0)
      {
        TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT];
      }
      else
      {
       TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT] - (poolParams.kernelH-1);
      }
    }

    if(poolParams.strideW > 1)
    {
      if(poolParams.useCeil)
      {
        TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = ceil(((TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH] +
          poolParams.padW*2.0) - (poolParams.kernelW)) / poolParams.strideW) + 1;
      }
      else
      {
        TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = floor(((TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH] +
          poolParams.padW*2.0) - (poolParams.kernelW)) / poolParams.strideW) + 1;
      }
    }
    else
    {
      if(poolParams.padW > 0)
      {
        TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH];
      }
      else
      {
       TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH] - (poolParams.kernelW-1);
      }
    }
    if(!(isGlobalAvgPooling))
    {
      tidl_setLRTBpad(TIDLPCLayers);
    }
    TIDLPCLayers.numMacs =
      (int64_t)((int64_t)TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] *TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2]* TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] *
        TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] *
        poolParams.kernelW *poolParams.kernelH);
  }
  else
  {
    TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_BATCH];
    TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH];
    TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = 1;
    TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = 1;
    TIDLPCLayers.numMacs =
      (int64_t)((int64_t)TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH]* TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] *TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] *
        TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH]);
  }

  int32_t  outIdx = tidl_getOutLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure->numLayers, TIDLPCLayers.outData[0].dataId);
  sTIDL_LayerPC_t &TIDLPCLayerOut = pOrgTIDLNetStructure->TIDLPCLayers[outIdx];
  if (outIdx == -1)
  {
    return 0;
  }

  if ((TIDLPCLayerOut.layerType == TIDL_InnerProductLayer && TIDLPCLayerOut.numInBufs < 2) && (TIDLPCLayers.outConsumerCnt[0] == 1))
  {
    if ((poolParams.poolingType == TIDL_AveragePooling) &&
        (poolParams.kernelW == 0) &&
        (poolParams.kernelH == 0)
        )
    {
      TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH];
      TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = 1;
      TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
    }
  }

  return 0;
}
int32_t TIDL_tfOutReshapeIdentity(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].elementType = TIDLPCLayers.inData[0].elementType;
  TIDLPCLayers.outData[0].numDim = TIDLPCLayers.inData[0].numDim;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_BATCH];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM1];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM2];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH];
  TIDLPCLayers.numMacs =
    (int64_t)((int64_t)TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH]* TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] *TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] *
      TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH]);

  return 0;
}

int32_t TIDL_tfOutReshapeDataConvert(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].numDim = TIDLPCLayers.inData[0].numDim;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_BATCH];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM1];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM2];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH];

  TIDLPCLayers.numMacs =
    (int64_t)((int64_t)TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] *TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2]* TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] *
      TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH]);

  return 0;
}

int32_t TIDL_tfOutReshapeGatherLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  sTIDL_LayerPC_t &layer = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  int32_t axis = layer.layerParams.gatherParams.axis;

  // re-initialize the numDim of inputs, as they are initialized to 4 by default in the import code
  sTIDL_allowlistingMetaData md = layer.allowlistingMetaData;
  layer.inData[0].numDim = md.varTensorsDims[0].size();
  layer.inData[1].numDim = md.varTensorsDims[1].size();

  layer.outData[0].numDim = layer.inData[0].numDim; // assuming grid is 1D

  for (int32_t i=0;i<TIDL_DIM_MAX;i++)
  {
    layer.outData[0].dimValues[i] = layer.inData[0].dimValues[i];
  }

  layer.outData[0].dimValues[(TIDL_DIM_MAX - layer.inData[0].numDim)+axis] = layer.inData[1].dimValues[TIDL_DIM_WIDTH];
  layer.outData[0].elementType = layer.inData[0].elementType;

  return 0;
}

int32_t TIDL_tfOutReshapeTransposeLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].elementType = TIDLPCLayers.inData[0].elementType;

  TIDLPCLayers.inData[0].numDim = TIDLPCLayers.weights.bufSize;
  TIDLPCLayers.outData[0].numDim = TIDLPCLayers.weights.bufSize;
  int32_t* perm = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].layerParams.transposeParams.perm;
  int32_t weights[TIDL_DIM_MAX], dim=0;

  for(int32_t i = 0; i < TIDLPCLayers.weights.bufSize; i++)
  {
    weights[i] = ((int32_t*)(TIDLPCLayers.weights.ptr))[i];
  }

  if ( TIDLPCLayers.weights.bufSize < TIDL_DIM_MAX)
  {
    int32_t  k = TIDL_DIM_MAX - TIDLPCLayers.weights.bufSize;
    for (int32_t i=0;i<k;i++)
    {
      TIDLPCLayers.outData[0].dimValues[i] = 1;
      perm[i] = dim++;
    }
    for (int32_t i=0;i<TIDLPCLayers.weights.bufSize;i++)
    {
      TIDLPCLayers.outData[0].dimValues[k+i] = TIDLPCLayers.inData[0].dimValues[k + weights[i]];
      perm[k+i] = weights[i] + k;
    }
  }
  else
  {
    for (int32_t i=0;i<TIDL_DIM_MAX;i++)
    {
      TIDLPCLayers.outData[0].dimValues[i] = TIDLPCLayers.inData[0].dimValues[weights[i]];
      perm[i] = weights[i];
    }
  }

  /** Transpose over batch is not supported, we copy the batch as is from inData*/
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_BATCH];

  TIDLPCLayers.numMacs =
    (int64_t)((int64_t)TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] *TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2]* TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] *
      TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH]);
  return 0;
}

int32_t TIDL_tfOutReshapeReshapeLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].elementType = TIDLPCLayers.inData[0].elementType;

  if (TIDLPCLayers.layerPCParams.reshapeParams.isInduced == 1)
  {
    /** Reshape layer introduces by TIDL, should would already be updated, don't update shape*/
    return 0;
  }
  TIDLPCLayers.outData[0].numDim = TIDLPCLayers.inData[0].numDim;
  /*Check if any dimension is -1 or 0. If it is 0, then the corresponding input dimension
  is set to the output dimension. Atmost 1 dimension can be -1, in which case it is calculated
  from other dimensions.
  */
  int32_t minusOnePresent = -1;
  int32_t i, remDim=0, dim=1;
  int64_t weights[TIDL_DIM_MAX];
  if (gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TENSORFLOW || gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TFLITE
      || gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TFLITE_RT)
  {
    for(i = 0; i < TIDLPCLayers.weights.bufSize; i++)
    {
      weights[i] = ((int32_t*)(TIDLPCLayers.weights.ptr))[i];
    }
  }
  else
  {
    for(i = 0; i < TIDLPCLayers.weights.bufSize; i++)
    {
      weights[i] = ((int64_t*)(TIDLPCLayers.weights.ptr))[i];
    }
  }

  int32_t* outData = TIDLPCLayers.outData[0].dimValues;
  int32_t* inData = TIDLPCLayers.inData[0].dimValues;

  if ((TIDLPCLayers.weights.bufSize == TIDL_DIM_MAX && weights[0] != inData[TIDL_DIM_BATCH]) ||
      TIDLPCLayers.weights.bufSize > TIDL_DIM_MAX)
  {
    printf( " Unsupported reshape - %s\n", TIDLPCLayers.name);
    exit(0);
  }

  remDim = TIDL_DIM_MAX - TIDLPCLayers.weights.bufSize;

  outData[TIDL_DIM_BATCH] = inData[TIDL_DIM_BATCH];
  for (int32_t i=1;i<remDim;i++)
  {
    outData[dim++] = 1;
  }
  int32_t startIdx = TIDLPCLayers.weights.bufSize == TIDL_DIM_MAX ? 1:0;

  for (int32_t i=startIdx;i<TIDLPCLayers.weights.bufSize;i++)
  {
    outData[dim++] = weights[i];
  }

  if (gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TENSORFLOW || gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TFLITE
      || gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TFLITE_RT)
  {
    //tflite input will be in NHWC format, change accordingly
    int32_t ch = outData[TIDL_DIM_WIDTH];
    outData[TIDL_DIM_WIDTH] = outData[TIDL_DIM_HEIGHT];
    outData[TIDL_DIM_HEIGHT] = outData[TIDL_DIM_NUMCH];
    outData[TIDL_DIM_NUMCH] = ch;
  }
  uint64_t totalSize = 1;
  for(i = 0; i<TIDL_DIM_MAX; i++)
  {
    totalSize *= TIDLPCLayers.inData[0].dimValues[i];
    if(outData[i]==0) {
      outData[i] = TIDLPCLayers.inData[0].dimValues[i];
    }
    else if(outData[i]==-1) {
      minusOnePresent = i;
    }
  }

  if(minusOnePresent != -1)
  {
  int32_t curSize = 1;
  for(int i=0;i<TIDL_DIM_MAX;i++)
  {
    if(i != minusOnePresent)
    {
      curSize *= outData[i];
    }
  }
  outData[minusOnePresent] = totalSize/curSize;
  }

  int32_t notOne = 0, notOneIndex = 0, idx=0;
  for (int32_t i=1;i<TIDL_DIM_MAX;i++)
  {
    if (TIDLPCLayers.outData[0].dimValues[i] != 1)
    {
      notOne++;
      notOneIndex = i;
    }
  }
  idx = tidl_getOutLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure->numLayers, TIDLPCLayers.outData[0].dataId);
  /*Flatten only if 1D softmax or innerproduct (non act x act)*/
  if ((notOne == 1) && (pOrgTIDLNetStructure->TIDLPCLayers[idx].layerType == TIDL_SoftMaxLayer || ((pOrgTIDLNetStructure->TIDLPCLayers[idx].layerType == TIDL_InnerProductLayer) && (pOrgTIDLNetStructure->TIDLPCLayers[idx].allowlistingMetaData.numVarInputs < 2))))
  {
    //If the input is flattened and the next layer is softmax, shift the flattened dimension to width dimension
    TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = TIDLPCLayers.outData[0].dimValues[notOneIndex];
    TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1]=1;
    TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2]=1;
    TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
    TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = 1;
  }

  TIDLPCLayers.numMacs =
    (int64_t)((int64_t)TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] *TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2]* TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] *
      TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH]);

  return 0;
}

int32_t TIDL_tfOutReshapeEltwise(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];

  TIDLPCLayers.outData[0].numDim = 0;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] = 0;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] = 0;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2] = 0;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] = 0;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = 0;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = 0;

  int32_t i;
  for (i = 0; i < TIDLPCLayers.numInBufs; i++)
  {
    TIDLPCLayers.outData[0].numDim                      = std::max(TIDLPCLayers.outData[0].numDim,                      TIDLPCLayers.inData[i].numDim);
    TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH]   = std::max(TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH],   TIDLPCLayers.inData[i].dimValues[TIDL_DIM_BATCH]);
    TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1]    = std::max(TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1],    TIDLPCLayers.inData[i].dimValues[TIDL_DIM_DIM1]);
    TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2]    = std::max(TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2],    TIDLPCLayers.inData[i].dimValues[TIDL_DIM_DIM2]);
    TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH]   = std::max(TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH],   TIDLPCLayers.inData[i].dimValues[TIDL_DIM_NUMCH]);
    TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT]  = std::max(TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT],  TIDLPCLayers.inData[i].dimValues[TIDL_DIM_HEIGHT]);
    TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH]   = std::max(TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH],   TIDLPCLayers.inData[i].dimValues[TIDL_DIM_WIDTH]);
  }

  TIDLPCLayers.numMacs =
    (int64_t)((int64_t)TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] *TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2]* TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] *
      TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH]);

  TIDLPCLayers.outData[0].elementType = tidl_getElementType(0);
  for (int32_t j = 0; j < TIDLPCLayers.numInBufs; j++)
  {
    if ((TIDLPCLayers.inData[j].elementType == TIDL_SignedChar) ||
        (TIDLPCLayers.inData[j].elementType == TIDL_SignedShort))
    {
      TIDLPCLayers.outData[0].elementType = tidl_getElementType(1);
    }
  }
  if((TIDLPCLayers.actParams.actType == TIDL_RelU6) || (TIDLPCLayers.actParams.actType == TIDL_RelU))
  {
    TIDLPCLayers.outData[0].elementType = tidl_getElementType(0);
  }

  return 0;
}

int32_t TIDL_tfOutReshapePRelu(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].elementType = tidl_getElementType(1);
  TIDLPCLayers.layerParams.batchNormParams.numChannels = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH];
  TIDLPCLayers.outData[0].numDim = TIDLPCLayers.inData[0].numDim;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_BATCH];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM1];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM2];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH];
  TIDLPCLayers.numMacs =
    (int64_t)((int64_t)TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH]* TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] *TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] *
      TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH]);

  /*
  * Handle Random Coeff case
  */
  int32_t dataSize = TIDLPCLayers.layerParams.batchNormParams.numChannels;

  if(TIDLPCLayers.bias.ptr == NULL ||
     TIDLPCLayers.bias.bufSize !=  dataSize ||
     TIDLPCLayers.weights.ptr == NULL ||
     TIDLPCLayers.weights.bufSize != dataSize ||
     TIDLPCLayers.slope.ptr == NULL ||
     TIDLPCLayers.slope.bufSize != dataSize)
  {
    printf("WARNING: PReLU Layer %s's bias cannot be found(or not match) in coef file, "
           "Random bias will be generated! "
           "Only for evaluation usage! "
           "Results are all random!\n", TIDLPCLayers.name);

    float * weights = (float*)my_malloc(dataSize*sizeof(float));
    float * bias = (float*)my_malloc(dataSize*sizeof(float));
    float * slope = (float*)my_malloc(dataSize*sizeof(float));
    for (int j = 0; j < dataSize; j++)
    {
      weights[j] = 1;
      bias[j]  = 0;
      slope[j] = 1;
    }

    TIDLPCLayers.bias.ptr = bias;
    TIDLPCLayers.bias.bufSize = dataSize;
    TIDLPCLayers.weights.ptr = weights;
    TIDLPCLayers.weights.bufSize = dataSize;
    TIDLPCLayers.slope.ptr = slope;
    TIDLPCLayers.slope.bufSize = dataSize;
  }
  return 0;
}

int32_t TIDL_tfOutReshapeBN(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  TIDL_tfOutReshapeIdentity(pOrgTIDLNetStructure, layerIndex);
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].elementType = tidl_getElementType(1);
  TIDLPCLayers.layerParams.batchNormParams.numChannels = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH];

  if((TIDLPCLayers.actParams.actType == TIDL_RelU6) || (TIDLPCLayers.actParams.actType == TIDL_RelU)||
    (TIDLPCLayers.actParams.actType == TIDL_Sigmoid) ||
    ((TIDLPCLayers.actParams.actType == TIDL_Clip) && (TIDLPCLayers.actParams.clipMin >= 0)))
  {
    TIDLPCLayers.outData[0].elementType = tidl_getElementType(0);
  }

  /*
  * Handle Random Coeff case
  */
  int32_t dataSize = TIDLPCLayers.layerParams.batchNormParams.numChannels;

  if(TIDLPCLayers.bias.ptr == NULL ||
     TIDLPCLayers.bias.bufSize !=  dataSize ||
     TIDLPCLayers.weights.ptr == NULL ||
     TIDLPCLayers.weights.bufSize != dataSize)
  {
    printf("WARNING: Batch Norm Layer %s's coeff cannot be found(or not match) in coef file, "
           "Random bias will be generated! "
           "Only for evaluation usage! "
           "Results are all random!\n", TIDLPCLayers.name);

    float * weights = (float*)my_malloc(dataSize*sizeof(float));
    float * bias = (float*)my_malloc(dataSize*sizeof(float));
    for (int j = 0; j < dataSize; j++)
    {
      weights[j] = 1;
      bias[j]  = 0;
    }

    TIDLPCLayers.bias.ptr = bias;
    TIDLPCLayers.bias.bufSize = dataSize;
    TIDLPCLayers.weights.ptr = weights;
    TIDLPCLayers.weights.bufSize = dataSize;
  }

  return 0;
}


int32_t TIDL_tfOutReshapeRelu(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  TIDL_tfOutReshapeIdentity(pOrgTIDLNetStructure, layerIndex);
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].elementType = tidl_getElementType(0);
  return 0;
}


int32_t TIDL_tfOutReshapeTanh(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  TIDL_tfOutReshapeIdentity(pOrgTIDLNetStructure, layerIndex);
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].elementType = TIDLPCLayers.inData->elementType;
  return 0;
}

int32_t TIDL_tfOutReshapeHardSigmoid(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  TIDL_tfOutReshapeIdentity(pOrgTIDLNetStructure, layerIndex);
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].elementType = tidl_getElementType(0);
  return 0;
}

int32_t TIDL_tfOutReshapeELU(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  TIDL_tfOutReshapeIdentity(pOrgTIDLNetStructure, layerIndex);
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].elementType = TIDLPCLayers.inData->elementType;
  return 0;
}

int32_t TIDL_tfOutReshapeSigmoid(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  TIDL_tfOutReshapeIdentity(pOrgTIDLNetStructure, layerIndex);
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].elementType = tidl_getElementType(0);
  return 0;
}

int32_t TIDL_tfOutReshapeSqueeze(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i;
  int32_t num_dim_4_squeeze = 0;
  int32_t ii = 0;

  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].elementType = TIDLPCLayers.inData[0].elementType;
  /*In squeeze one dimension gets removed*/

  for(i = 0; i < TIDL_DIM_MAX; i++)
  {
    if(TIDLPCLayers.layerPCParams.squeezeParams.axis[i]!= 0)
    {
      num_dim_4_squeeze++;
    }
  }

  TIDLPCLayers.outData[0].numDim = TIDLPCLayers.inData[0].numDim - num_dim_4_squeeze;

  for(i = TIDL_DIM_MAX - 1; i > TIDL_DIM_BATCH; i--)
  {
    if(TIDLPCLayers.layerPCParams.squeezeParams.axis[i] == 0)
    {
      TIDLPCLayers.outData[0].dimValues[TIDL_DIM_MAX - 1 - ii] = TIDLPCLayers.inData[0].dimValues[i];
      ii++;
    }
  }

  for(; ii < TIDL_DIM_MAX-1; ii++)
  {
    TIDLPCLayers.outData[0].dimValues[TIDL_DIM_MAX - 1 - ii] = 1;
  }

  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_BATCH];

  return 0;
}

int32_t TIDL_tfOutReshapeClip(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  TIDL_tfOutReshapeIdentity(pOrgTIDLNetStructure, layerIndex);
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  if(TIDLPCLayers.actParams.clipMin >= 0)
  {
    TIDLPCLayers.outData[0].elementType = tidl_getElementType(0);
  }
  else
  {
    TIDLPCLayers.outData[0].elementType = tidl_getElementType(1);
  }
  return 0;
}

bool TIDL_isOutTensorFlat(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  int32_t nonFlatDims = 0;
  int32_t i;
  bool status = false;
  for(i = 0; i < TIDL_DIM_MAX; i++)
  {
    if(TIDLPCLayers.outData[0].dimValues[i] != 1 && (i != TIDL_DIM_BATCH))
    {
      nonFlatDims++;
    }
  }
  if(nonFlatDims == 1)
  {
    status = true;
  }
  return status;
}

int32_t TIDL_tfOutReshapeSoftmax(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t outIdx = 0, isOutLayer = 0;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].elementType = TIDLPCLayers.inData[0].elementType;
  if(TIDLPCLayers.inData[0].elementType < TIDL_UnsignedWord)
  {
    /*8/16-bit*/
    if(TIDLPCLayers.inData[0].elementType < TIDL_UnsignedShort)
    {
      /*8-bit*/
      /*Always have unsigned output with ZP=0*/
      TIDLPCLayers.outData[0].elementType = TIDL_UnsignedChar;
    }
    else
    {
      /*16-bit*/
      /*Always have unsigned output with ZP=0*/
      TIDLPCLayers.outData[0].elementType = TIDL_UnsignedShort;
    }
  }
  TIDLPCLayers.outData[0].numDim = TIDLPCLayers.inData[0].numDim;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_BATCH];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM1];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM2];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH];

  /**
   * Change the outElement Type of softmax to float based on two conditions,
   * 1. Input to the softmax is flattened and
   * 2. Softmax is the last layer in the network
  */
  outIdx = tidl_getOutLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure->numLayers, TIDLPCLayers.outData[0].dataId);
  isOutLayer = pOrgTIDLNetStructure->TIDLPCLayers[outIdx].layerType == TIDL_DataLayer ? 1 : 0;

  if(TIDL_isOutTensorFlat(pOrgTIDLNetStructure, layerIndex) && isOutLayer == 1)
  {
    TIDLPCLayers.outData[0].elementType = TIDL_SinglePrecFloat;
  }

  TIDLPCLayers.numMacs =
    (int64_t)((int64_t)TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] *TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2]* TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] *
      TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH]);

  return 0;
}

int32_t TIDL_tfOutReshapeIPLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  sTIDL_InnerProductParams_t &innerProductParams = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].layerParams.innerProductParams;

  innerProductParams.numInRows = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT];

  if(innerProductParams.numOutCols == -1 || innerProductParams.numOutCols == 0)
  {
    innerProductParams.numOutCols = TIDLPCLayers.inData[1].dimValues[TIDL_DIM_WIDTH];
  }

  if(innerProductParams.numInCols == -1 || innerProductParams.numInCols == 0)
  {
    innerProductParams.numInCols = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH];
  }

  TIDLPCLayers.outData[0].elementType = tidl_getElementType(1);
  if((TIDLPCLayers.actParams.actType == TIDL_RelU6) || (TIDLPCLayers.actParams.actType == TIDL_RelU)||
    ((TIDLPCLayers.actParams.actType == TIDL_Clip) && (TIDLPCLayers.actParams.clipMin >= 0)))
  {
    TIDLPCLayers.outData[0].elementType = tidl_getElementType(0);
  }

  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH]  = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_BATCH];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH]  = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = innerProductParams.numInRows; // out rows is equal to in rows
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH]  = innerProductParams.numOutCols;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1]   =  TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM1]; // out rows is equal to in rows
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2]   =  TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM2];

  innerProductParams.numBatches = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH]; // all channel and batches are excuted in single loop in kernel with one pitch, which might break sometime

  TIDLPCLayers.numMacs =
    (int64_t)((int64_t) innerProductParams.numBatches * innerProductParams.numInCols * innerProductParams.numInRows * innerProductParams.numOutCols);

  /*
  * Handle Random Coeff Case
  */
  int32_t dataSize;

  dataSize = innerProductParams.numBatches * innerProductParams.numInRows * innerProductParams.numOutCols;

  if((TIDLPCLayers.bias.ptr == NULL ||
    TIDLPCLayers.bias.bufSize != dataSize) && (TIDLPCLayers.layerParams.innerProductParams.isBias) && (TIDLPCLayers.layerParams.innerProductParams.constIdx != 1))
  {
    printf("WARNING: Inner Product Layer bias %s's coeff cannot be found(or not match) in coef file, "
          "Random coeff will be generated! "
          "Only for evaluation usage! "
          "Results are all random!\n",TIDLPCLayers.name);
    float  * bias = (float *)my_malloc(dataSize*sizeof(float));

    for (int idx = 0; idx < dataSize; idx++)
    {
      bias[idx] = 0;
    }

    TIDLPCLayers.bias.ptr = bias;
    TIDLPCLayers.bias.bufSize = dataSize;
  }

  return 0;
}

int32_t TIDL_handleAxis(sTIDL_LayerPC_t &TIDLPCLayers, int32_t axis)
{
  /* This function updates the axis to fit for TIDL_DIM_MAX dimensions, default axis could be for 2D/3D data*/

  /* Using varTensorsDims[0].size() for numDim as numDim parameter in inData/outData is not being preserved when set */
  sTIDL_allowlistingMetaData& md = TIDLPCLayers.allowlistingMetaData;
  int32_t numDim = 0, remDim = 0;
  std::vector<int32_t> temp;

  if (md.varTensorsDims.size() != 0)
  {
    numDim = md.varTensorsDims[0].size();
  }
  else
  {
    md.varTensorsDims.push_back(temp);
  }

  if (numDim != TIDL_DIM_MAX) // this condition ensures we don't update axis repeatedly
  {
    if (numDim != 0)
    {
      remDim = (TIDL_DIM_MAX - numDim);
    }
    else
    {
      /* If numDim is not set, shape inference is not done on the model, we assume 4 dimensions for the tensor by default */
      remDim = (TIDL_DIM_MAX - 4);
      /* push indata dimvalues so that we read correct numDim the next time this function is called. */
      for (int32_t i=TIDL_DIM_DIM2;i<TIDL_DIM_MAX;i++)
      {
        //md.varTensorsDims[0].insert(md.varTensorsDims[0].begin(), 1);
        //md.varTensorsDims[0].insert(md.varTensorsDims[0].end(), TIDLPCLayers.inData[0].dimValues[i]);
        md.varTensorsDims[0].push_back(TIDLPCLayers.inData[0].dimValues[i]);
      }
    }
    if (axis < 0)
    {
      axis += numDim;
    }

    axis += remDim;

    /* Update md.varTensorsDims[0].size() to 6 so that the axis update is not done repeatedly - this function is called multiple times */
    for (int32_t i=0;i< remDim;i++)
    {
      md.varTensorsDims[0].insert(md.varTensorsDims[0].begin(), 1);
    }
  }

  if (axis > TIDL_DIM_WIDTH || axis <TIDL_DIM_BATCH)
  {
    printf("Unsupported axis for concat - %d\n",axis);
  }
  return axis;
}

void TIDL_zeroPadKernelToIncreaseSize(sBuffer_t &buf, int32_t iw, int32_t ih, int32_t ow, int32_t oh)
{
  int32_t num_kernels = buf.bufSize / (iw * ih);
  int32_t i, j, k;
  float*  src = (float*)buf.ptr;
  float*  dst = (float*)my_malloc(num_kernels *ow *oh * sizeof(float));
  memset(dst, 0, num_kernels *ow *oh * sizeof(float));

  for(k =0; k < num_kernels; k++)
  {
    for(j = 0; j < ih; j++)
    {
      for(i = 0; i < iw; i++)
      {
        dst[k * oh * ow + j * ow + i] = dst[k * ih * iw + j * iw + i];
      }
    }
  }
  buf.ptr = dst;
  my_free(src);
}

int32_t TIDL_tfOutReshapeDeConvLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  sTIDL_ConvParams_t &convParams = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].layerParams.convParams;

  /* Currently validated with ONNX */
  TIDLPCLayers.outData[0].elementType = tidl_getElementType(1);
  if((TIDLPCLayers.actParams.actType == TIDL_RelU6) || (TIDLPCLayers.actParams.actType == TIDL_RelU)||
    ((TIDLPCLayers.actParams.actType == TIDL_Clip) && (TIDLPCLayers.actParams.clipMin >= 0)))
  {
    TIDLPCLayers.outData[0].elementType = tidl_getElementType(0);
  }
  TIDLPCLayers.outData[0].numDim = TIDLPCLayers.inData[0].numDim;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_BATCH];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM1];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM2];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] = convParams.numOutChannels;

  /* stride[i] * (input_size[i] - 1) + output_padding[i] + kernel_shape[i] - pads[start_i] - pads[end_i]*/
  if((TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] < 0) || (TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] < 0))
  {
    TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = -TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT];
    TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = -TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH];
  }
  else
  {
    TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] =
      (convParams.strideH * (TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT] - 1) + convParams.kernelH - (convParams.padH * 2));
    TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] =
      (convParams.strideW * (TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH] - 1) + convParams.kernelW - (convParams.padW * 2));
  }
  convParams.numInChannels = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH];

  TIDLPCLayers.numMacs =
    (int64_t)(((int64_t)TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH]* TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] *TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] *
      TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] *
      convParams.kernelW *convParams.kernelH *
      TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH]) / convParams.numGroups);


  /* Conversting 3x3 kernel with stride 2x2 to 4x4 kernel */
  if((convParams.kernelH == 3) &&
    (convParams.kernelW == 3) &&
    (convParams.strideW == 2) &&
    (convParams.strideH == 2) &&
    (convParams.dilationW == 1) &&
    (convParams.dilationH == 1))
  {
    TIDL_zeroPadKernelToIncreaseSize(TIDLPCLayers.weights, 3,3,4,4);
  }

  /*
   * Handle Random Coeff Case
   */
  int32_t dataSize;

  dataSize = convParams.kernelH * convParams.kernelW *
             convParams.numOutChannels *
             TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH] / convParams.numGroups;

  if(TIDLPCLayers.weights.ptr == NULL ||
     TIDLPCLayers.weights.bufSize != dataSize)
  {
    printf("WARNING: Deconv Layer %s's coeff cannot be found(or not match) in coef file, "
           "Random coeff will be generated! "
           "Only for evaluation usage! "
           "Results are all random!\n", TIDLPCLayers.name);

    float*  data;
    data = (float *)my_malloc(dataSize*sizeof(float));

    for (int i = 0; i < dataSize; i++) {
      int32_t val = (rand() & (0X7FFFFFFF));
      data[i] = ((float)((rand() & 1) ? val : -val)) / (0X3FFFFF);
    }

    TIDLPCLayers.weights.ptr = data;
    TIDLPCLayers.weights.bufSize = dataSize;
  }
  if((gParams.modelType == 0) || (gParams.modelType == 2))
  {
    if(TIDLPCLayers.weightsReordered == 0)
    {
      TIDL_caffeReorderDeconvFilters(TIDLPCLayers.weights, convParams.numOutChannels, convParams.numInChannels, convParams.numGroups, convParams.kernelW*convParams.kernelH);
      TIDLPCLayers.weightsReordered = 1;
    }
  }

  /*
   * Handle Random Bias Case
   */
  dataSize = convParams.numOutChannels;

  if(convParams.enableBias &&
    (TIDLPCLayers.bias.ptr == NULL ||
     TIDLPCLayers.bias.bufSize != dataSize))
  {
    printf("WARNING: Deconv Layer %s's bias cannot be found(or not match) in coef file, "
           "Random bias will be generated! "
           "Only for evaluation usage! "
           "Results are all random!\n", TIDLPCLayers.name);

    float*  data;
    data = (float *)my_malloc(dataSize*sizeof(float));

    for (int i = 0; i < dataSize; i++) {
      int32_t val = (rand() & (0X7FFFFFFF));
      data[i] = ((float)((rand() & 1) ? val : -val)) / (0X3FFFFF);
    }

    TIDLPCLayers.bias.ptr = data;
    TIDLPCLayers.bias.bufSize = dataSize;
  }
  return 0;
}

int32_t TIDL_tfOutReshapeConcatLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  int32_t j;

  int32_t totDim = 0, axisId;

  // Special code for Caffe Concat import
  if(gParams.modelType == 0)
  {
    bool widthWise = true;
    for (j = 0; j < TIDLPCLayers.numInBufs; j++)
    {
      int32_t  idx_previous = tidl_getInLayer(*pOrgTIDLNetStructure, layerIndex, TIDLPCLayers.inData[j].dataId);
      if (idx_previous == -1)
      {
        printf("ERROR: TIDL_tfOutReshapeConcatLayer ConcatLayer find prev layer failed!\n");
        return -1;
      }
      sTIDL_LayerPC_t &previous = pOrgTIDLNetStructure->TIDLPCLayers[idx_previous];
      if(previous.layerType != TIDL_FlattenLayer)
      {
        widthWise = false;
      }
    }

    if(widthWise)
    {
      TIDLPCLayers.layerParams.concatParams.axis = TIDL_DIM_WIDTH;
    }

  }
  /** Axis for tensorflow, tflite models is already adjusted to TIDL_DIM_MAX during operator map function*/
  if (gParams.modelType != TIDL_IMPORT_MODEL_FORMAT_TFLITE_RT && gParams.modelType != TIDL_IMPORT_MODEL_FORMAT_TFLITE && gParams.modelType != TIDL_IMPORT_MODEL_FORMAT_TENSORFLOW)
  {
    axisId = TIDL_handleAxis(TIDLPCLayers, TIDLPCLayers.layerParams.concatParams.axis);
    TIDLPCLayers.layerParams.concatParams.axis = axisId;
  }
  else
  {
    axisId = TIDLPCLayers.layerParams.concatParams.axis;
  }

  TIDLPCLayers.outData[0].elementType = tidl_getElementType(0);
  TIDLPCLayers.outData[0].numDim = TIDLPCLayers.inData[0].numDim;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_BATCH];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM1];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM2];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH];

  for (j = 0; j < TIDLPCLayers.numInBufs; j++)
  {
    if ((TIDLPCLayers.inData[j].elementType == TIDL_SignedChar) ||
        (TIDLPCLayers.inData[j].elementType == TIDL_SignedShort))
    {
      TIDLPCLayers.outData[0].elementType = tidl_getElementType(1);
    }
    totDim += TIDLPCLayers.inData[j].dimValues[axisId];
  }
  TIDLPCLayers.outData[0].dimValues[axisId] = totDim;

  TIDLPCLayers.numMacs =
    (int64_t)((int64_t)TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH]* TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] *TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] *
      TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH]);
  return 0;
}
int32_t TIDL_tfOutReshapeSliceLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  int32_t j;
  TIDLPCLayers.outData[0].numDim = TIDLPCLayers.inData[0].numDim;

  int32_t totDim, axisId;

  axisId = TIDLPCLayers.layerParams.sliceParams.axis;
  totDim = TIDLPCLayers.inData[0].dimValues[axisId];

  /*Onnx puts int64b max in case of max dim. Int64 is captured as int32 in slicePoints. Hence setting the right valid value in that scenario*/

  for (j = 0; j < (TIDLPCLayers.numOutBufs+1); j++)
  {
    if(TIDLPCLayers.layerParams.sliceParams.slicePoints[j] == 0x7fffffff)
    {
      TIDLPCLayers.layerParams.sliceParams.slicePoints[j] = totDim;
    }
  }

  if ( (TIDLPCLayers.layerParams.sliceParams.slicePoints[0] >= 0 ) &&
        (TIDLPCLayers.layerParams.sliceParams.slicePoints[1] < 0 ))
  {
  /* TFLite gives slicePoint as negative which indicates that end point of slice layer is (image dimension - slice point),
  to distinguish this condition check if the slicePoint 0 is a positive value and slicePoint 1 is a negative value*/
    if ( (TIDLPCLayers.layerParams.sliceParams.slicePoints[0] >= 0 ) && (TIDLPCLayers.layerParams.sliceParams.slicePoints[1] < 0 ))
    {
      TIDLPCLayers.layerParams.sliceParams.slicePoints[1] += TIDLPCLayers.inData[0].dimValues[axisId];
    }
  }
  /*
    Case: Slicepoints [-x : ]
  */
  else if (
            (TIDLPCLayers.layerParams.sliceParams.slicePoints[0] < 0) &&
            (TIDLPCLayers.layerParams.sliceParams.slicePoints[1] == totDim)
          )
  {
    TIDLPCLayers.layerParams.sliceParams.slicePoints[0] += totDim;
  }
  else
  {
    int32_t setSlicePoints = 0;
    for (j = 0; j < (TIDLPCLayers.numOutBufs+1); j++)
    {
      if(TIDLPCLayers.layerParams.sliceParams.slicePoints[j] == -1)
      {
        setSlicePoints = 1;
      }

    }
    if(setSlicePoints)
    {
      for (j = 0; j < TIDLPCLayers.numOutBufs; j++)
      {
        if (TIDLPCLayers.layerParams.sliceParams.slicePoints[j] == -1)
        {
          TIDLPCLayers.layerParams.sliceParams.slicePoints[j] = j * (totDim / TIDLPCLayers.numOutBufs);
        }
      }
      TIDLPCLayers.layerParams.sliceParams.slicePoints[TIDLPCLayers.numOutBufs] = totDim;
    }
  }



  for (j = 0; j < TIDLPCLayers.numOutBufs; j++)
  {
    TIDLPCLayers.outData[j].elementType = TIDLPCLayers.inData[0].elementType;
    TIDLPCLayers.outData[j].dimValues[TIDL_DIM_BATCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_BATCH];
    TIDLPCLayers.outData[j].dimValues[TIDL_DIM_DIM1] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM1];
    TIDLPCLayers.outData[j].dimValues[TIDL_DIM_DIM2] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM2];
    TIDLPCLayers.outData[j].dimValues[TIDL_DIM_HEIGHT] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT];
    TIDLPCLayers.outData[j].dimValues[TIDL_DIM_WIDTH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH];
    TIDLPCLayers.outData[j].dimValues[TIDL_DIM_NUMCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH];

  }

  for (j = 0; j < TIDLPCLayers.numOutBufs; j++)
  {
    TIDLPCLayers.outData[j].dimValues[axisId] =
    min(TIDLPCLayers.layerParams.sliceParams.slicePoints[j+1] - TIDLPCLayers.layerParams.sliceParams.slicePoints[j],TIDLPCLayers.outData[j].dimValues[axisId]);
    if (TIDLPCLayers.layerParams.sliceParams.stride != 1)
    {
      TIDLPCLayers.outData[j].dimValues[axisId] /=  TIDLPCLayers.layerParams.sliceParams.stride;

    }
  }

  TIDLPCLayers.numMacs = 0;

  return 0;
}

int32_t TIDL_tfOutReshapeCropLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].layerParams.cropParams.numChannels = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].inData[0].dimValues[TIDL_DIM_NUMCH];

  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outData[0].numDim = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].inData[0].numDim;
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_BATCH] = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].inData[0].dimValues[TIDL_DIM_BATCH];
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_DIM1] = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].inData[0].dimValues[TIDL_DIM_DIM1];
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_DIM2] = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].inData[0].dimValues[TIDL_DIM_DIM2];
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH] = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].inData[0].dimValues[TIDL_DIM_NUMCH];
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outData[0].elementType = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].inData[0].elementType;

  /* Second Input has cropped output sizes */
  if ( pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].numInBufs == 2 )
  {
    pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].inData[1].dimValues[TIDL_DIM_HEIGHT];
    pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].inData[1].dimValues[TIDL_DIM_WIDTH];
    pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].numInBufs = 1;

  }

  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].numMacs = 0;

  return 0;
}
int32_t TIDL_tfOutReshapeFlattenLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].elementType = TIDLPCLayers.inData[0].elementType;
  TIDLPCLayers.outData[0].numDim = TIDLPCLayers.inData[0].numDim;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_BATCH];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM1];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM2];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = 1;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH] *
    TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT] *
    TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH];
  TIDLPCLayers.numMacs =
    (int64_t)((int64_t)TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] *TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2]* TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] *
      TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH]);
  return 0;
}

int32_t TIDL_tfOutReshapeDeptoSpace(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].elementType = TIDLPCLayers.inData[0].elementType;
  TIDLPCLayers.outData[0].numDim = TIDLPCLayers.inData[0].numDim;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_BATCH];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM1];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM2];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH]/
  (TIDLPCLayers.layerParams.depthToSpaceParams.blockSize*TIDLPCLayers.layerParams.depthToSpaceParams.blockSize);
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT] * TIDLPCLayers.layerParams.depthToSpaceParams.blockSize;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH] * TIDLPCLayers.layerParams.depthToSpaceParams.blockSize;
  TIDLPCLayers.numMacs =
    (int64_t)((int64_t)TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] *TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2]* TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] *
      TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH]);
  return 0;
}
int32_t TIDL_tfOutReshapeBatchtoSpace(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  int32_t blockH = TIDLPCLayers.layerPCParams.batchToSpaceParams.blockHeight;
  int32_t blockW = TIDLPCLayers.layerPCParams.batchToSpaceParams.blockWidth;
  int32_t cropT =  TIDLPCLayers.layerPCParams.batchToSpaceParams.cropT;
  int32_t cropB =  TIDLPCLayers.layerPCParams.batchToSpaceParams.cropB;
  int32_t cropL =  TIDLPCLayers.layerPCParams.batchToSpaceParams.cropL;
  int32_t cropR =  TIDLPCLayers.layerPCParams.batchToSpaceParams.cropR;
  TIDLPCLayers.outData[0].elementType = TIDLPCLayers.inData[0].elementType;
  TIDLPCLayers.outData[0].numDim = TIDLPCLayers.inData[0].numDim;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] = 1;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM1];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM2];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT] * blockH - cropT - cropB;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH] * blockW - cropL - cropR;
  TIDLPCLayers.numMacs =
    (int64_t)((int64_t)TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] *TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2]* TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] *
      TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH]);
  return 0;
}

int32_t TIDL_tfOutReshapeSpacetoBatch(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  int32_t blockH = TIDLPCLayers.layerPCParams.spaceToBatchParams.blockHeight;
  int32_t blockW = TIDLPCLayers.layerPCParams.spaceToBatchParams.blockWidth;
  int32_t padT =  TIDLPCLayers.layerPCParams.spaceToBatchParams.padT;
  int32_t padB =  TIDLPCLayers.layerPCParams.spaceToBatchParams.padB;
  int32_t padL =  TIDLPCLayers.layerPCParams.spaceToBatchParams.padL;
  int32_t padR =  TIDLPCLayers.layerPCParams.spaceToBatchParams.padR;
  TIDLPCLayers.outData[0].elementType = TIDLPCLayers.inData[0].elementType;
  TIDLPCLayers.outData[0].numDim = TIDLPCLayers.inData[0].numDim;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_BATCH] * blockH * blockW;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM1];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM2];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = (TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT] + padT + padB) / blockH;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = (TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH] + padL + padR) / blockW;
  TIDLPCLayers.numMacs =
    (int64_t)((int64_t)TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH]* TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] *TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] *
      TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH]);
  return 0;
}

int32_t TIDL_tfOutReshapeBatchReshape(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t orgWidth, orgHeight;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  int32_t blockW = TIDLPCLayers.inData[0].numBatchW;
  int32_t blockH = TIDLPCLayers.inData[0].numBatchH;
  TIDLPCLayers.outData[0].elementType = TIDLPCLayers.inData[0].elementType;
  TIDLPCLayers.outData[0].numDim = TIDLPCLayers.inData[0].numDim;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_BATCH];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM1];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM2];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH];
  if ( TIDLPCLayers.inData[0].dimValues[TIDL_DIM_BATCH] > 1)  // StitchLayer
  {
    TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] = 1;
    TIDLPCLayers.outData[0].numBatchW = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_BATCH];
    TIDLPCLayers.outData[0].numBatchH = 1;
    blockW = TIDLPCLayers.outData[0].numBatchW;
    blockH = TIDLPCLayers.outData[0].numBatchH;
    TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT]*blockH + TIDLPCLayers.outData[0].batchPadH*(blockH-1);
    TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH]*blockW + TIDLPCLayers.outData[0].batchPadW*(blockW-1);
  }
  else
  {
    orgHeight = (TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT] - (TIDLPCLayers.inData[0].batchPadH*(blockH-1))) / blockH;
    orgWidth = (TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH] - (TIDLPCLayers.inData[0].batchPadW*(blockW-1))) / blockW;
    TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = orgHeight*blockH + TIDLPCLayers.outData[0].batchPadH*(blockH-1);
    TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = orgWidth*blockW + TIDLPCLayers.outData[0].batchPadW*(blockW-1);
    if ( TIDLPCLayers.outData[0].batchPadW == 0) // UnStitchLayer
    {
      TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_BATCH] * blockW * blockH;
      TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = orgHeight;
      TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = orgWidth;
      TIDLPCLayers.outData[0].numBatchW = 1;
      TIDLPCLayers.outData[0].numBatchH = 1;
    }
  }
  TIDLPCLayers.numMacs =
    (int64_t)((int64_t)TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] *TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2]* TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] *
      TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH]);
  return 0;
}

int32_t TIDL_tfOutReshapeColorConversionLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].elementType = TIDLPCLayers.inData[0].elementType;
  TIDLPCLayers.outData[0].numDim = TIDLPCLayers.inData[0].numDim;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_BATCH];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM1];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM2];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH] * 2;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT] * 2;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH];
  TIDLPCLayers.numMacs =
    (int64_t)((int64_t)TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] *TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2]* TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] *
      TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH]);
  return 0;
}

int32_t TIDL_tfOutReshapeArgmaxLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].elementType = tidl_getElementType(0);
  TIDLPCLayers.outData[0].numDim = TIDLPCLayers.inData[0].numDim;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_BATCH];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM1];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM2];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH];
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].layerParams.argMaxParams.numChannels = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH];

  TIDLPCLayers.numMacs =
    (int64_t)((int64_t)TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH]* TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] *TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] *
      TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH]);
  return 0;
}
int32_t TIDL_tfOutReshapePadLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  TIDLPCLayers.outData[0].elementType = TIDLPCLayers.inData[0].elementType;
  TIDLPCLayers.outData[0].numDim = TIDLPCLayers.inData[0].numDim;

  int32_t padT, padB, padL, padR;
  padT = TIDLPCLayers.layerParams.padLayerParams.padT;
  padB = TIDLPCLayers.layerParams.padLayerParams.padB;
  padL = TIDLPCLayers.layerParams.padLayerParams.padL;
  padR = TIDLPCLayers.layerParams.padLayerParams.padR;

  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_BATCH];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM1];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM2];
   // + TIDLPCLayers.layerParams.padLayerParams.padTensor[0] + TIDLPCLayers.layerParams.padLayerParams.padTensor[1];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT] + (padT + padB);
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH] + (padL + padR);

  TIDLPCLayers.numMacs = 0;
  return 0;
}

int32_t TIDL_tfOutReshapeDetOutLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t keep_top_k;
  int32_t numOutDataPerObject;

  keep_top_k = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].layerParams.detectOutParams.keepTopK;

  numOutDataPerObject = ((sizeof(TIDL_ODLayerObjInfo) - sizeof(TIDL_ODLayerKeyPoint) * TIDL_OD_MAX_KEY_POINTS) + (sizeof(TIDL_ODLayerKeyPoint)-sizeof(float32_tidl)) * pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].layerParams.detectOutParams.numKeypoints)/sizeof(float32_tidl);
  if (pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].layerParams.detectOutParams.subCodeType == TIDL_ObjectPose)
  {
  numOutDataPerObject += sizeof(TIDL_ODLayerObjectPose)/sizeof(float32_tidl);
  }


  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outData[0].numDim       = 1;
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_BATCH] = 1;
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_DIM1] = 1;
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_DIM2] = 1;
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] = 1;
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] = (sizeof(TIDL_ODLayerHeaderInfo)/sizeof(float32_tidl)) + keep_top_k*numOutDataPerObject;
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outData[0].elementType = TIDL_SinglePrecFloat;


  return 0;
}

int32_t TIDL_tfOutReshapeRoiPoolingLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  printf("ERROR: TIDL_tfOutReshapeRoiPoolingLayer is not supported for current version.\n");
  return -1;
}

int32_t TIDL_tfOutReshapeOdPostProcessingLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  printf("ERROR: TIDL_tfOutReshapeOdPostProcessingLayer is not supported for current version.\n");
  return -1;
}

int32_t TIDL_tfOutReshapeOdOutputReformatLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  return 0;
}

int32_t TIDL_tfOutReshapeReduceMaxLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  sTIDL_ReduceParams_t* reduceParams = &pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].layerParams.reduceParams;
  int32_t i;

  TIDLPCLayers.outData[0].elementType = TIDLPCLayers.inData[0].elementType;

  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_BATCH];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM1];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM2];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH];

  if(reduceParams->keepDims == 1)
  {
    TIDLPCLayers.outData[0].numDim = TIDLPCLayers.inData[0].numDim;
    TIDLPCLayers.outData[0].dimValues[reduceParams->axis] = 1;
  }
  else
  {
    TIDLPCLayers.outData[0].numDim = TIDLPCLayers.inData[0].numDim - 1;
    for(i = reduceParams->axis; i < TIDL_DIM_MAX - 1; i++)
    {
      TIDLPCLayers.outData[0].dimValues[i] = TIDLPCLayers.outData[0].dimValues[i+1];
    }

    /**bringing back 4 dimension if a dimension is reduced. If this contraint is removed then below code can go away**/
    if(TIDLPCLayers.outData[0].numDim < TIDL_DIM_MAX)
    {
      for(i = TIDL_DIM_MAX-1; i >= ( TIDL_DIM_MAX- TIDLPCLayers.outData[0].numDim); i--)
      {
        TIDLPCLayers.outData[0].dimValues[i] = TIDLPCLayers.outData[0].dimValues[i - (TIDL_DIM_MAX-TIDLPCLayers.outData[0].numDim)];
      }

      for(;i >= 0; i--)
      {
        TIDLPCLayers.outData[0].dimValues[i] = 1;
      }

      TIDLPCLayers.outData[0].numDim = TIDL_DIM_MAX;
    }
    /** -- **/
  }

  TIDLPCLayers.numMacs =
    (int64_t)((int64_t)TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] *TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2]* TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] *
      TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH]);

  return 0;
}

int32_t TIDL_tfOutReshapeScatterElementsLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex)
{
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex];
  int32_t dataTensorId = 0;
  int32_t i,j,k;
  int32_t minDim;

  int32_t axis = TIDLPCLayers.layerParams.scatterElementsParams.axis;
  int32_t isScatterNd = (axis == -1)?1:0;

  if(isScatterNd)
  {
    int32_t dataDim = TIDLPCLayers.allowlistingMetaData.varTensorsDims[0].size();
    int32_t indSize = TIDLPCLayers.allowlistingMetaData.varTensorsDims[1].size();
    int32_t indDim = TIDLPCLayers.allowlistingMetaData.varTensorsDims[1][indSize-1];
    if (dataDim == 0)
    {
      if (indDim == 4) {TIDLPCLayers.inData[0].numDim = 4;}
      else if (indDim == 2) {TIDLPCLayers.inData[0].numDim = 3;}
      else if (indDim == 1) {TIDLPCLayers.inData[0].numDim = 2;}
    }
    else
    {
      TIDLPCLayers.inData[0].numDim = dataDim;
    }
    TIDLPCLayers.outData[0].numDim = TIDLPCLayers.inData[0].numDim;
  }

  TIDLPCLayers.outData[0].elementType = tidl_getElementType(1);

  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_BATCH];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM1];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM2];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT];
  TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH];

  return 0;
}

int32_t tidl_linkInputTensors(sTIDL_OrgNetwork_t  *pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i0, i1, i2;
  for (i0 = 0; i0 < pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].numInBufs; i0++)
  {
    for (i1 = layerIndex - 1; i1 >= 0; i1--)
    {
      for (i2 = 0; i2 < pOrgTIDLNetStructure->TIDLPCLayers[i1].numOutBufs; i2++)
      {
        if (pOrgTIDLNetStructure->TIDLPCLayers[i1].outConsumerLinked[i2] < pOrgTIDLNetStructure->TIDLPCLayers[i1].outConsumerCnt[i2])
        {
          if (strcmp((const char *)pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].inDataNames[i0], (const char *)pOrgTIDLNetStructure->TIDLPCLayers[i1].outDataNames[i2]) == 0)
          {
            pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].inData[i0].dataId = pOrgTIDLNetStructure->TIDLPCLayers[i1].outData[i2].dataId;
            pOrgTIDLNetStructure->TIDLPCLayers[i1].outConsumerLinked[i2]++;
          }
        }
      }
    }
  }
  return 0;
}


int32_t tidl_linkOutputTensors(sTIDL_OrgNetwork_t  *pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i0, i1, i2;
  for (i0 = 0; i0 < pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].numOutBufs; i0++)
  {
    for (i1 = layerIndex - 1; i1 >= 0; i1--)
    {
      for (i2 = 0; i2 < pOrgTIDLNetStructure->TIDLPCLayers[i1].numInBufs; i2++)
      {
        if (pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outConsumerLinked[i0] < pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outConsumerCnt[i0])
        {
          if (strcmp((const char *)pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outDataNames[i0], (const char *)pOrgTIDLNetStructure->TIDLPCLayers[i1].inDataNames[i2]) == 0)
          {
            pOrgTIDLNetStructure->TIDLPCLayers[i1].inData[i2].dataId = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outData[i0].dataId;
            pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outConsumerLinked[i0]++;
            if(pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].layerType == TIDL_OdOutputReformatLayer)
            {
              pOrgTIDLNetStructure->TIDLPCLayers[i1].inData[i2] = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outData[0];
            }
          }
        }
      }
    }
  }
  return 0;
}
int32_t tidl_getConsumerCount(sTIDL_OrgNetwork_t  *pOrgTIDLNetStructure, int32_t layerIndex, const char * name)
{
  int32_t i0, i1, i2;
  int32_t outConsumerLinked = 0;
  for (i1 = layerIndex - 1; i1 >= 0; i1--)
  {
    for (i2 = 0; i2 < pOrgTIDLNetStructure->TIDLPCLayers[i1].numInBufs; i2++)
    {
      if (strcmp((const char *)pOrgTIDLNetStructure->TIDLPCLayers[i1].inDataNames[i2], (const char *)name) == 0)
      {
        outConsumerLinked++;
      }
    }
  }
  return outConsumerLinked;
}

int32_t tidl_isAllInsAvailable(sTIDL_LayerPC_t  *orgLayer, sTIDL_OrgNetwork_t  *ptempTIDLNetStructure, int32_t layerIndex)
{
  int32_t i0, i1, i2;
  int32_t status = 0;
  int32_t availableIns = 0;
  for (i0 = 0; i0 < orgLayer->numInBufs; i0++)
  {
    for (i1 = 0; i1 < layerIndex; i1++)
    {
      for (i2 = 0; i2 < ptempTIDLNetStructure->TIDLPCLayers[i1].numOutBufs; i2++)
      {
        if (strcmp((const char *)ptempTIDLNetStructure->TIDLPCLayers[i1].outDataNames[i2], (const char *)orgLayer->inDataNames[i0]) == 0)
        {
          //printf("Ins %s vs %s\n", (const char *)ptempTIDLNetStructure->TIDLPCLayers[i1].outDataNames[i2], (const char *)orgLayer->inDataNames[i0]);
          availableIns++;
        }
      }
    }
  }
  /* Is shall be orgLayer->numInBufs <= availableIns, temprary fix to get caffe import working
     TODO : need rever back after migatration caffe to new import framework */

  if ((orgLayer->numInBufs <= availableIns) || (orgLayer->numInBufs == -1))
  {
    status = 1;
  }
  return(status);
}

int32_t tidl_sortLayersInProcOrder(sTIDL_OrgNetwork_t  *pOrgTIDLNetStructure, sTIDL_OrgNetwork_t  *ptempTIDLNetStructure, int32_t layerIndex)
{
  int32_t i0, i1, i2;
  int32_t newNetIdx = 0;
  int32_t lastNetIdx = 0;
  int32_t *isAddedToList = (int32_t *)my_malloc(layerIndex*sizeof(int32_t));
  memset(isAddedToList, 0, sizeof(int32_t)*layerIndex);
  while (newNetIdx < layerIndex)
  {
    lastNetIdx = newNetIdx;
    for (i0 = 0; i0 < layerIndex; i0++)
    {
      if (isAddedToList[i0] == 0)
      {
        if (tidl_isAllInsAvailable(&pOrgTIDLNetStructure->TIDLPCLayers[i0], ptempTIDLNetStructure, newNetIdx))
        {
          ptempTIDLNetStructure->TIDLPCLayers[newNetIdx] = pOrgTIDLNetStructure->TIDLPCLayers[i0];
          newNetIdx++;
          isAddedToList[i0] = 1;
        }
        else
        {
          ;
        }
      }
    }
    if((newNetIdx == lastNetIdx) && (newNetIdx < layerIndex))
    {
      for (i0 = 0; i0 < layerIndex; i0++)
      {
        if (isAddedToList[i0] == 0)
        {
          // print the first layer missing inputs and cannot be topo-sorted
          printf("Error: Layer %d, %s:%s is missing inputs in the network and cannot be topologically sorted\n",
                 i0,
                 pOrgTIDLNetStructure->TIDLPCLayers[i0].name,
                 pOrgTIDLNetStructure->TIDLPCLayers[i0].outDataNames[0]);
          for (i1=0; i1<pOrgTIDLNetStructure->TIDLPCLayers[i0].numInBufs; i1++)
            printf("  Input %d: %s, dataId=%d\n", i1,
                   pOrgTIDLNetStructure->TIDLPCLayers[i0].inDataNames[i1],
                   pOrgTIDLNetStructure->TIDLPCLayers[i0].inData[i1].dataId);
          break;
        }
      }
      if ((gParams.modelType == 4))  exit(-1);
      break;
    }
  }
  my_free(isAddedToList);
  ptempTIDLNetStructure->numLayers = newNetIdx;
  *pOrgTIDLNetStructure = *ptempTIDLNetStructure;
  return 0;
}

void tidl_replaceInTensorName(sTIDL_OrgNetwork_t  *pOrgTIDLNetStructure, int32_t layerIndex, char * orgTensorName, char * newTensorName)
{
  int32_t i0, i1, i2;
  int32_t newNetIdx = 0;

  for (i0 = 0; i0 < layerIndex; i0++)
  {
    for (i1 = 0; i1 < pOrgTIDLNetStructure->TIDLPCLayers[i0].numInBufs; i1++)
    {
      if(strcmp((const char *)pOrgTIDLNetStructure->TIDLPCLayers[i0].inDataNames[i1],(const char *)orgTensorName) == 0)
      {
        strcpy((char *)pOrgTIDLNetStructure->TIDLPCLayers[i0].inDataNames[i1], (const char *)newTensorName);
      }
    }
  }
}

int32_t tidl_removeMergedLayersFromNet(sTIDL_OrgNetwork_t  *pOrgTIDLNetStructure, sTIDL_OrgNetwork_t  *ptempTIDLNetStructure)
{
  int32_t i0, i1, i2;
  int32_t newNetIdx = 0;

  int32_t layerIndex = pOrgTIDLNetStructure->numLayers;
  memset((void *)ptempTIDLNetStructure, 0, sizeof(orgTIDLNetStructure));
  for (i0 = 0; i0 < layerIndex; i0++)
  {
    if ((pOrgTIDLNetStructure->TIDLPCLayers[i0].numInBufs != -1) ||
      (pOrgTIDLNetStructure->TIDLPCLayers[i0].numOutBufs != -1))
    {
      ptempTIDLNetStructure->TIDLPCLayers[newNetIdx] = pOrgTIDLNetStructure->TIDLPCLayers[i0];
      newNetIdx++;
    }
  }
  ptempTIDLNetStructure->numLayers = newNetIdx;
  *pOrgTIDLNetStructure = *ptempTIDLNetStructure;

  tidl_sortDataIds(&orgTIDLNetStructure);
  return 0;
}
int32_t tidl_upateAInDataId(sTIDL_OrgNetwork_t  *pOrgTIDLNetStructure, int32_t layerIndex, int32_t oldId, int32_t currId)
{
  int32_t i0, i1, i2, i3, i4;
  for (i3 = 0; i3 < layerIndex; i3++)
  {
    for (i4 = 0; i4 < pOrgTIDLNetStructure->TIDLPCLayers[i3].numInBufs; i4++)
    {
      if (pOrgTIDLNetStructure->TIDLPCLayers[i3].inData[i4].dataId == oldId)
      {
        pOrgTIDLNetStructure->TIDLPCLayers[i3].inData[i4].dataId = currId;
      }
    }
  }
  return 0;
}

int32_t tidl_sortDataIds(sTIDL_OrgNetwork_t *pOrgTIDLNetStructure)
{
  int32_t i0, i1, i2, i3, i4;
  int32_t maxDataId = 0;
  int32_t currId = 0;
  int32_t oldId = 0;
  int32_t layerIndex = pOrgTIDLNetStructure->numLayers;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    for (i2 = 0; i2 < pOrgTIDLNetStructure->TIDLPCLayers[i1].numOutBufs; i2++)
    {
      maxDataId = pOrgTIDLNetStructure->TIDLPCLayers[i1].outData[i2].dataId >= maxDataId ? pOrgTIDLNetStructure->TIDLPCLayers[i1].outData[i2].dataId : maxDataId;
    }
  }
  maxDataId = maxDataId + 1;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    for (i2 = 0; i2 < pOrgTIDLNetStructure->TIDLPCLayers[i1].numOutBufs; i2++)
    {
      pOrgTIDLNetStructure->TIDLPCLayers[i1].outData[i2].dataId += maxDataId;
    }
    for (i2 = 0; i2 < pOrgTIDLNetStructure->TIDLPCLayers[i1].numInBufs; i2++)
    {
      pOrgTIDLNetStructure->TIDLPCLayers[i1].inData[i2].dataId += maxDataId;
    }
  }

  for (i1 = 0; i1 < layerIndex; i1++)
  {
    for (i2 = 0; i2 < pOrgTIDLNetStructure->TIDLPCLayers[i1].numOutBufs; i2++)
    {
      oldId = pOrgTIDLNetStructure->TIDLPCLayers[i1].outData[i2].dataId;
      //currId = i1;
      pOrgTIDLNetStructure->TIDLPCLayers[i1].outData[i2].dataId = currId;
      tidl_upateAInDataId(pOrgTIDLNetStructure, layerIndex, oldId, currId);
      currId++;
    }
  }
  return 0;
}

int32_t tidl_makeDataIdLayerIdSame(sTIDL_OrgNetwork_t  *pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i0, i1, i2, i3, i4;
  int32_t maxDataId = 0;
  int32_t currId = 0;
  int32_t oldId = 0;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    for (i2 = 0; i2 < pOrgTIDLNetStructure->TIDLPCLayers[i1].numOutBufs; i2++)
    {
      maxDataId = pOrgTIDLNetStructure->TIDLPCLayers[i1].outData[i2].dataId >= maxDataId ? pOrgTIDLNetStructure->TIDLPCLayers[i1].outData[i2].dataId : maxDataId;
    }
  }
  maxDataId = maxDataId + 1;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    for (i2 = 0; i2 < pOrgTIDLNetStructure->TIDLPCLayers[i1].numOutBufs; i2++)
    {
      pOrgTIDLNetStructure->TIDLPCLayers[i1].outData[i2].dataId += maxDataId;
    }
    for (i2 = 0; i2 < pOrgTIDLNetStructure->TIDLPCLayers[i1].numInBufs; i2++)
    {
      pOrgTIDLNetStructure->TIDLPCLayers[i1].inData[i2].dataId += maxDataId;
    }
  }
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    for (i2 = 0; i2 < pOrgTIDLNetStructure->TIDLPCLayers[i1].numOutBufs; i2++)
    {
      oldId = pOrgTIDLNetStructure->TIDLPCLayers[i1].outData[i2].dataId;
      currId = i1;
      pOrgTIDLNetStructure->TIDLPCLayers[i1].outData[i2].dataId = currId;
      tidl_upateAInDataId(pOrgTIDLNetStructure, layerIndex, oldId, currId);
    }
  }
  return 0;
}

int32_t tidl_updateOutDataShape(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t startIdx, int32_t layerIndex, sTIDL_tfOutReshapeMap_t * sTIDL_tfOutReshapeTable)
{
  int32_t i1, i2, i3, i4;
  int32_t status = 0;

  for (i1 = startIdx; i1 < layerIndex; i1++)
  {
    if(pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType != TIDL_DataLayer)
    {
      pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].numBatchH = pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0].numBatchH;
      pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].numBatchW = pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0].numBatchW;
    }
    status = sTIDL_tfOutReshapeTable[pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType].tidl_tfOutReshape(&pOrgTIDLNetStructure, i1);
    /* The below code within the "if (status != -1)"" condition has potential bugs due to which some parametrs in the first layer parameters of the cut out n/w are not coming out correct. To be fixed*/
    if (status != -1)
    {
      for (i2 = 0; i2 < pOrgTIDLNetStructure.TIDLPCLayers[i1].numOutBufs; i2++)
      {
        for (i3 = 0; i3 < layerIndex; i3++)
        {
          for (i4 = 0; i4 < pOrgTIDLNetStructure.TIDLPCLayers[i3].numInBufs; i4++)
          {
            if (pOrgTIDLNetStructure.TIDLPCLayers[i3].inData[i4].dataId == pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[i2].dataId)
            {
              pOrgTIDLNetStructure.TIDLPCLayers[i3].inData[i4] = pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[i2];
            }
          }
        }
      }
    }
  }
  return status;
}
int32_t tidl_getInLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex, int32_t dataId)
{
  int32_t i1, i2;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    for (i2 = 0; i2 < pOrgTIDLNetStructure.TIDLPCLayers[i1].numOutBufs; i2++)
    {
      if (pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[i2].dataId == dataId)
      {
        return (i1);
      }
    }
  }
  return (-1);
}

int32_t tidl_getOutLayer(sTIDL_OrgNetwork_t  *pOrgTIDLNetStructure, int32_t layerIndex, int32_t dataId)
{
  int32_t i1, i2;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    for (i2 = 0; i2 < pOrgTIDLNetStructure->TIDLPCLayers[i1].numInBufs; i2++)
    {
      if (pOrgTIDLNetStructure->TIDLPCLayers[i1].inData[i2].dataId == dataId)
      {
        return (i1);
      }
    }
  }
  return (-1);
}


int32_t tidl_getOutLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex, int32_t dataId)
{
  int32_t i1, i2;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    for (i2 = 0; i2 < pOrgTIDLNetStructure.TIDLPCLayers[i1].numInBufs; i2++)
    {
      if (pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[i2].dataId == dataId)
      {
        return (i1);
      }
    }
  }
  return (-1);
}

int32_t tidl_getInLayer(sTIDL_Network_t  *tidlNet, int32_t layerIndex, int32_t dataId)
{
  int32_t i1, i2;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    for (i2 = 0; i2 < tidlNet->TIDLLayers[i1].numOutBufs; i2++)
    {
      if (tidlNet->TIDLLayers[i1].outData.dataId == dataId)
      {
        return (i1);
      }
    }
  }
  return (-1);
}
int32_t tidl_getOutLayer(sTIDL_Network_t  *tidlNet, int32_t layerIndex, int32_t dataId)
{
  int32_t i1, i2;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    for (i2 = 0; i2 < tidlNet->TIDLLayers[i1].numInBufs; i2++)
    {
      if (tidlNet->TIDLLayers[i1].inData[i2] == dataId)
      {
        return (i1);
      }
    }
  }
  return (-1);
}

std::vector<int32_t> tidl_getOutLayers(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex, int32_t dataId)
{
  std::vector<int32_t> out_layers;
  int32_t i1, i2;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    for (i2 = 0; i2 < pOrgTIDLNetStructure.TIDLPCLayers[i1].numInBufs; i2++)
    {
      if (pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[i2].dataId == dataId)
      {
        out_layers.push_back(i1);
        break;
      }
    }
  }
  return out_layers;
}
int32_t tidl_mergeFalttenLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i1, i2, i3, i4;
  int32_t status = 0;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_FlattenLayer)
    {
      int32_t  idx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0].dataId);
      if (idx == -1)
      {
        return -1;
      }
      sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure.TIDLPCLayers[idx];
      if ((TIDLPCLayers.layerType == TIDL_InnerProductLayer) &&
        (TIDLPCLayers.outConsumerCnt[0] == 1))
      {
        TIDLPCLayers.numMacs += pOrgTIDLNetStructure.TIDLPCLayers[i1].numMacs;
        TIDLPCLayers.outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0];
        strcpy((char *)TIDLPCLayers.outDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i1].outDataNames[0]);
        TIDLPCLayers.outConsumerCnt[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].outConsumerCnt[0];
        pOrgTIDLNetStructure.TIDLPCLayers[i1].numInBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[i1].numOutBufs = -1;
      }
    }
  }

  return 0;
}


int32_t tidl_mergeBiasLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i1, i2, i3, i4;
  int32_t status = 0;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_BiasLayer)
    {
      int32_t  idx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0].dataId);
      if (idx == -1)
      {
        return -1;
      }
      sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure.TIDLPCLayers[idx];
      if ((TIDLPCLayers.layerType == TIDL_ConvolutionLayer) &&
        (TIDLPCLayers.outConsumerCnt[0] == 1))
      {
        TIDLPCLayers.numMacs += pOrgTIDLNetStructure.TIDLPCLayers[i1].numMacs;
        TIDLPCLayers.outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0];
        strcpy((char *)TIDLPCLayers.outDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i1].outDataNames[0]);
        TIDLPCLayers.outConsumerCnt[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].outConsumerCnt[0];
        pOrgTIDLNetStructure.TIDLPCLayers[i1].numInBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[i1].numOutBufs = -1;
        if (TIDLPCLayers.layerParams.convParams.enableBias == 0)
        {
          TIDLPCLayers.layerParams.convParams.enableBias = 1;
          TIDLPCLayers.bias = pOrgTIDLNetStructure.TIDLPCLayers[i1].bias;
        }
        else
        {
          float * src = (float *)pOrgTIDLNetStructure.TIDLPCLayers[i1].bias.ptr;
          float * dst = (float *)TIDLPCLayers.bias.ptr;
          for (i2 = 0; i2 < TIDLPCLayers.bias.bufSize; i2++)
          {
            dst[i2] += src[i2];
          }
        }
      }
      else if((TIDLPCLayers.layerType == TIDL_InnerProductLayer) &&
        (TIDLPCLayers.outConsumerCnt[0] == 1))
      {
        TIDLPCLayers.numMacs += pOrgTIDLNetStructure.TIDLPCLayers[i1].numMacs;
        TIDLPCLayers.outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0];
        strcpy((char *)TIDLPCLayers.outDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i1].outDataNames[0]);
        TIDLPCLayers.outConsumerCnt[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].outConsumerCnt[0];
        pOrgTIDLNetStructure.TIDLPCLayers[i1].numInBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[i1].numOutBufs = -1;
        float * src = (float *)pOrgTIDLNetStructure.TIDLPCLayers[i1].bias.ptr;
        float * dst = (float *)TIDLPCLayers.bias.ptr;
        for (i2 = 0; i2 < TIDLPCLayers.bias.bufSize; i2++)
        {
          dst[i2] += src[i2];
        }
      }
    }
  }

  return 0;
}

int32_t tidl_mergePadLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i1, i2, i3, i4;
  int32_t status = 0;
  int32_t padW, padH;
  int32_t padL = 0, padT = 0;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_PadLayer)
    {
      int32_t  inIdx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0].dataId);
      if (inIdx == -1)
      {
        return -1;
      }
      int32_t  outIdx = tidl_getOutLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].dataId);
      if (outIdx == -1)
      {
        return -1;
      }

      sTIDL_LayerPC_t &TIDLPCLayersIn = pOrgTIDLNetStructure.TIDLPCLayers[inIdx];
      sTIDL_LayerPC_t &TIDLPCLayersOut = pOrgTIDLNetStructure.TIDLPCLayers[outIdx];

      int32_t padT, padB, padL, padR;
      padT = pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.padLayerParams.padT;
      padB = pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.padLayerParams.padB;
      padL = pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.padLayerParams.padL;
      padR = pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.padLayerParams.padR;

      padW = padR < padL ? padL : padR;
      padH = padB < padT ? padT : padB;

      if ((gParams.modelType == 1) || (gParams.modelType == 3) || (gParams.modelType == 4) || (gParams.modelType == 5))
      {
        TIDLPCLayersOut.strideOffsetMethod = TIDL_StrideOffsetCenter;
      }

      if ((TIDLPCLayersOut.layerType == TIDL_ConvolutionLayer) &&
        (pOrgTIDLNetStructure.TIDLPCLayers[i1].outConsumerCnt[0] == 1) &&
        /*(TIDLPCLayersIn.outConsumerCnt[0] == 1) &&*/
        ((TIDLPCLayersOut.layerParams.convParams.kernelW/2) == padW) &&
        ((TIDLPCLayersOut.layerParams.convParams.kernelH/2) == padH))
      {
        TIDLPCLayersOut.numMacs += pOrgTIDLNetStructure.TIDLPCLayers[i1].numMacs;

        TIDLPCLayersOut.layerParams.convParams.padW = padW;
        TIDLPCLayersOut.layerParams.convParams.padH = padH;
        // TIDLPCLayersOut.layerParams.convParams.padT = padT;
        // TIDLPCLayersOut.layerParams.convParams.padB = padB;
        // TIDLPCLayersOut.layerParams.convParams.padL = padL;
        // TIDLPCLayersOut.layerParams.convParams.padR = padR;
        if (((gParams.modelType == 1 || gParams.modelType == 3 || gParams.modelType == 4 || gParams.modelType == 5)) && (padL == padW) && (padT == padH))
        {
          TIDLPCLayersOut.strideOffsetMethod = TIDL_StrideOffsetTopLeft;
        }
        tidl_setLRTBpad(TIDLPCLayersOut);
          //TIDLPCLayersIn.outData[0]        = pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0];
        TIDLPCLayersOut.inData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0];
        strcpy((char *)TIDLPCLayersOut.inDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i1].inDataNames[0]);
        pOrgTIDLNetStructure.TIDLPCLayers[i1].numInBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[i1].numOutBufs = -1;
      }
      else if ((TIDLPCLayersOut.layerType == TIDL_PoolingLayer) &&
        (pOrgTIDLNetStructure.TIDLPCLayers[i1].outConsumerCnt[0] == 1))
      {
        TIDLPCLayersIn.numMacs += pOrgTIDLNetStructure.TIDLPCLayers[i1].numMacs;
        //Does this need to update pad T,B,L,R
        if (TIDLPCLayersOut.layerParams.poolParams.poolingType == TIDL_AveragePooling)
        {
          TIDLPCLayersOut.layerParams.poolParams.padW += padW;
          TIDLPCLayersOut.layerParams.poolParams.padH += padH;

          TIDLPCLayersOut.inData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0];
          strcpy((char *)TIDLPCLayersOut.inDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i1].inDataNames[0]);
          pOrgTIDLNetStructure.TIDLPCLayers[i1].numInBufs = -1;
          pOrgTIDLNetStructure.TIDLPCLayers[i1].numOutBufs = -1;
        }
        else if (TIDLPCLayersOut.layerParams.poolParams.poolingType == TIDL_MaxPooling &&
              ((TIDLPCLayersOut.layerParams.convParams.kernelW/2) == padW) &&
              ((TIDLPCLayersOut.layerParams.convParams.kernelH/2) == padH))
        {
          TIDLPCLayersOut.layerParams.convParams.padW = padW;
          TIDLPCLayersOut.layerParams.convParams.padH = padH;

          TIDLPCLayersOut.inData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0];
          strcpy((char *)TIDLPCLayersOut.inDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i1].inDataNames[0]);
          pOrgTIDLNetStructure.TIDLPCLayers[i1].numInBufs = -1;
          pOrgTIDLNetStructure.TIDLPCLayers[i1].numOutBufs = -1;
        }
        if ( gParams.modelType == 2 )
        {
          printf("Warning : Merging Pad layer with Average Pooling layer. This is expected to work but  this flow is functionally not validated with ONNX model format. \n");
        }
      }
      else
      {
        printf("WARNING : Pad layer won't be merged in the succeeding layer, it will be treated as a stand alone layer\n");
      }
    }
  }

  return 0;
}

void tidl_getInverse3X3Matrix(float32_tidl mat[3][3], float32_tidl matInv[3][3])
{
  float32_tidl determinant = 0;
  int32_t i, j;
  for(i = 0; i < 3; i++)
  {
    determinant = determinant + (mat[0][i] * (mat[1][(i+1)%3] * mat[2][(i+2)%3] - mat[1][(i+2)%3] * mat[2][(i+1)%3]));
  }
	for(i = 0; i < 3; i++)
  {
		for(j = 0; j < 3; j++)
    {
      matInv[i][j] = ((mat[(j+1)%3][(i+1)%3] * mat[(j+2)%3][(i+2)%3]) - (mat[(j+1)%3][(i+2)%3] * mat[(j+2)%3][(i+1)%3]))/ determinant;
    }
	}
}

void tidl_addPadValuesForYUVtoRGBConversion(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIdx, int32_t inDataFormat)
{
  int32_t i1,i2,i3,i4;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure.TIDLPCLayers[layerIdx];
  int32_t numInCh = 3;
  float matrixYUVtoRGB[3][3];
  float matrixYUVtoRGBInv[3][3];
  /* Calculate YUV to RGB folding matrices */
  int32_t ITUR_BT_601_CY = 1220542;
  int32_t ITUR_BT_601_CUB = 2116026;
  int32_t ITUR_BT_601_CUG = -409993;
  int32_t ITUR_BT_601_CVG = -852492;
  int32_t ITUR_BT_601_CVR = 1673527;
  int32_t ITUR_BT_601_SHIFT = 20;
  int32_t offset = (1 << (ITUR_BT_601_SHIFT - 1));
  int32_t uv_mean = 128;
  int32_t divFact = (1 << ITUR_BT_601_SHIFT);

  float rComp[3] = {ITUR_BT_601_CY, 0, ITUR_BT_601_CVR};
  float gComp[3] = {ITUR_BT_601_CY, ITUR_BT_601_CUG, ITUR_BT_601_CVG};
  float bComp[3] = {ITUR_BT_601_CY, ITUR_BT_601_CUB, 0};
  float biasComp[3] = {offset-(ITUR_BT_601_CVR*uv_mean)-(16*ITUR_BT_601_CY),
                       offset-(ITUR_BT_601_CVG*uv_mean)-(ITUR_BT_601_CUG*uv_mean)-(16*ITUR_BT_601_CY),
                       offset-(ITUR_BT_601_CUB*uv_mean)-(16*ITUR_BT_601_CY)};

  for(i1=0; i1<numInCh; i1++)
  {
    rComp[i1] = rComp[i1]/divFact;
    gComp[i1] = gComp[i1]/divFact;
    bComp[i1] = bComp[i1]/divFact;
    biasComp[i1] = biasComp[i1]/divFact;
  }

  if(inDataFormat == TIDL_inDataFormatRGBPlanar)
  {
    for(int i = 0; i < 3; i++)
    {
      matrixYUVtoRGB[0][i] = rComp[i];
      matrixYUVtoRGB[1][i] = gComp[i];
      matrixYUVtoRGB[2][i] = bComp[i];
    }
  }
  if(inDataFormat == TIDL_inDataFormatBGRPlanar)
  {
    for(int i = 0; i < 3; i++)
    {
      matrixYUVtoRGB[0][i] = bComp[i];
      matrixYUVtoRGB[1][i] = gComp[i];
      matrixYUVtoRGB[2][i] = rComp[i];
    }
  }
  /* Calculate inv(YUV to RGB folding matrix) */
  tidl_getInverse3X3Matrix(matrixYUVtoRGB, matrixYUVtoRGBInv);

  /* Populate the values to be padded */
  int32_t dataSize = numInCh; /* number of channels */

  TIDLPCLayers.layerParams.padLayerParams.padType = TIDL_PadPerChannel;
  if(TIDLPCLayers.perChannelPadConstTensor.ptr == NULL) /* this is not an existing pad layer, but newly added one for YUV to RGB conversion padding */
  {
    TIDLPCLayers.perChannelPadConstTensor.ptr = my_malloc(dataSize*sizeof(float));
    TIDLPCLayers.perChannelPadConstTensor.bufSize = dataSize;
    float *  perChannelPadConstTensor  = (float *)TIDLPCLayers.perChannelPadConstTensor.ptr;

    /* Need to set  Wyuv2rgb_3x3 * Y_3x1 + Byuv2rgb_3x1 = 0 where Y_3x1 is the padded region
     * Y_3x1 = inv(Wyuv2rgb_3x3) * (- Byuv2rgb_3x1) ---- value to be padded
     */
    for (int32_t j = 0; j < dataSize; j++)
    {
      perChannelPadConstTensor[j] = 0;
      for(int32_t k = 0; k < 3; k++)
      {
        perChannelPadConstTensor[j] += matrixYUVtoRGBInv[j][k] * biasComp[k] * (-1);
      }
    }
  }
  else
  {
    /* this is the case with existing pad layer which was added for batchnorm padding, so in this case,
     * add the unified pad value for YUV to RGB conversion and batchnorm
     *
     * Need to set Wyuv2rgb_3x3 * [diag(Wbn_3x1) * Y_3x1 + Bbn_3x1] + Byuv2rgb_3x1 = 0  where Y_3x1 is the value to be padded
     * Y_3x1 = inv(diag(Wbn_3x1)) * [inv(Wyuv2rgb_3x3) * (- Byuv2rgb_3x1) - Bbn_3x1]
     * Y_3x1[i] = 1/inScale[i] * [inv(Wyuv2rgb_3x3) * (- Byuv2rgb_3x1)][i] - (- inScale[i] * inMean[i])
     * Y_3x1[i] = {[inv(Wyuv2rgb_3x3) * (- Byuv2rgb_3x1)][i] / inScale[i]} + inMean[i]
     */
    float *  perChannelPadConstTensor  = (float *)TIDLPCLayers.perChannelPadConstTensor.ptr;
    for (int32_t j = 0; j < dataSize; j++)
    {
      perChannelPadConstTensor[j] = 0;
      for(int32_t k = 0; k < 3; k++)
      {
        perChannelPadConstTensor[j] += matrixYUVtoRGBInv[j][k] * biasComp[k] * (-1);
      }
      /* inScale and inMean can be directly used here since yuv to rgb conversion occurs only for input layer, where inMean and inScale for batchnorm are known */
      perChannelPadConstTensor[j] = perChannelPadConstTensor[j]/gParams.inScale[j] + gParams.inMean[j];
    }
  }
}

int32_t tidl_mergeYuv444InConvLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIdx, int32_t inDataFormat)
{
  int32_t i1,i2,i3,i4;
  sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure.TIDLPCLayers[layerIdx];
  int32_t numInCh  = TIDLPCLayers.layerParams.convParams.numInChannels;

  /* Add bias if not already enabled in convolution */
  if(TIDLPCLayers.layerParams.convParams.enableBias == 0)
  {
    TIDLPCLayers.layerParams.convParams.enableBias = 1;
    TIDLPCLayers.bias.ptr = my_malloc(sizeof(float)*TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH]);
    TIDLPCLayers.bias.bufSize = TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH];
    float * dst = (float *)TIDLPCLayers.bias.ptr;
    for(i2=0; i2<TIDLPCLayers.bias.bufSize; i2++)
    {
      dst[i2] = 0;
    }
  }

  /* Calculate YUV to RGB folding matrices */
  int32_t ITUR_BT_601_CY = 1220542;
  int32_t ITUR_BT_601_CUB = 2116026;
  int32_t ITUR_BT_601_CUG = -409993;
  int32_t ITUR_BT_601_CVG = -852492;
  int32_t ITUR_BT_601_CVR = 1673527;
  int32_t ITUR_BT_601_SHIFT = 20;
  int32_t offset = (1 << (ITUR_BT_601_SHIFT - 1));
  int32_t uv_mean = 128;
  int32_t divFact = (1 << ITUR_BT_601_SHIFT);

  float rComp[3] = {ITUR_BT_601_CY, 0, ITUR_BT_601_CVR};
  float gComp[3] = {ITUR_BT_601_CY, ITUR_BT_601_CUG, ITUR_BT_601_CVG};
  float bComp[3] = {ITUR_BT_601_CY, ITUR_BT_601_CUB, 0};
  float biasComp[3] = {offset-(ITUR_BT_601_CVR*uv_mean)-(16*ITUR_BT_601_CY),
                       offset-(ITUR_BT_601_CVG*uv_mean)-(ITUR_BT_601_CUG*uv_mean)-(16*ITUR_BT_601_CY),
                       offset-(ITUR_BT_601_CUB*uv_mean)-(16*ITUR_BT_601_CY)};

  for(i1=0; i1<numInCh; i1++)
  {
    rComp[i1] = rComp[i1]/divFact;
    gComp[i1] = gComp[i1]/divFact;
    bComp[i1] = bComp[i1]/divFact;
    biasComp[i1] = biasComp[i1]/divFact;
  }

  /* Calculate loop parameters for YUV to RGB folding */
  float * weights = (float *)TIDLPCLayers.weights.ptr;
  float * bias = (float *)TIDLPCLayers.bias.ptr;
  int32_t numOutCh = TIDLPCLayers.bias.bufSize;
  int32_t weightsSize = ((TIDLPCLayers.weights.bufSize / TIDLPCLayers.bias.bufSize))/ numInCh;

  for(i2=0; i2<numOutCh; i2++)
  {
    float rgbBias = 0;
    float *origWeights = (float *)my_malloc(sizeof(float)*numInCh*weightsSize);
    float wtMul1, wtMul2, wtMul3;
    float biasMul1, biasMul2, biasMul3;
    biasMul2 = biasComp[1];
    if(inDataFormat == TIDL_inDataFormatRGBPlanar)
    {
      biasMul1 = biasComp[0];
      biasMul3 = biasComp[2];
    }
    else if(inDataFormat == TIDL_inDataFormatBGRPlanar)
    {
      biasMul1 = biasComp[2];
      biasMul3 = biasComp[0];
    }
    else
    {
      printf("Unsupported data format for YUV to RGB conversion\n");
      return -1;
    }

    /* Copy original weights */
    for(i3=0; i3<numInCh; i3++)
    {
      for(i4=0; i4<weightsSize; i4++)
      {
        origWeights[i3*weightsSize + i4] = weights[i2*numInCh*weightsSize + i3*weightsSize + i4];
      }
    }

    /* Wrgb_folded_1x3 = (Wrgb_1x3 * Wyuv2rgb_3x3) and Bfolded_1x1 = (Wrgb_1x3 * Byuv2rgb_3x1) */
    for(i3=0; i3<numInCh; i3++)
    {
      wtMul2 = gComp[i3];
      if(inDataFormat == TIDL_inDataFormatRGBPlanar)
      {
        wtMul1 = rComp[i3];
        wtMul3 = bComp[i3];
      }
      else if(inDataFormat == TIDL_inDataFormatBGRPlanar)
      {
        wtMul1 = bComp[i3];
        wtMul3 = rComp[i3];
      }
      else
      {
        printf("Unsupported data format for YUV to RGB conversion\n");
        return -1;
      }

      for(i4=0; i4<weightsSize; i4++)
      {
        /* Calculate the new weight */
        weights[i2*numInCh*weightsSize + i3*weightsSize + i4] = (wtMul1 * origWeights[i4]) +
                                                                (wtMul2 * origWeights[weightsSize + i4]) +
                                                                (wtMul3 * origWeights[2*weightsSize + i4]);
        /* Bias needs to be calculated only once for a triplet */
        if(i3 == 0)
        {
          rgbBias += (origWeights[i4] * biasMul1) + (origWeights[weightsSize + i4] * biasMul2) + (origWeights[2*weightsSize + i4] * biasMul3);
        }
      }
    }

    my_free(origWeights);
    bias[i2] = rgbBias + bias[i2];
  }
  return 0;
}

/*this function checks if a layer can be split across multi-core based on it's height*/
int32_t TIDL_canSplitHeightForKernelSize(sTIDL_LayerPC_t& layer, int32_t numCores)
{
  int32_t noSplit = 0;
  int32_t kernelH = 0, splitHeight = 0, transferLines = 0, strideH = 0;

  if(layer.layerType == TIDL_PoolingLayer)
  {
    kernelH = layer.layerParams.poolParams.kernelH;
    strideH = layer.layerParams.poolParams.strideH;
  }
  else if(layer.layerType == TIDL_ConvolutionLayer)
  {
    kernelH = ( (layer.layerParams.convParams.kernelH - 1)*(layer.layerParams.convParams.dilationH) ) + 1;
    strideH = layer.layerParams.convParams.strideH;
  }

  splitHeight = (layer.inData[0].dimValues[TIDL_DIM_HEIGHT]) / numCores; /*even if there is uneven split there will be atleast one core which will have this height*/
  transferLines = (kernelH - 1)/2;

  if(splitHeight < transferLines) /*if our transferLines is more than the height for a core then we do not split that layer*/
  {
    noSplit = 1;
  }

  return noSplit;
}

int32_t  tidl_doesLayerSupportSpatialSplit(sTIDL_OrgNetwork_t& pOrgTIDLNetStructure, int32_t lyrIdx, int32_t numCores)
{
  int32_t split = 1;

  sTIDL_LayerPC_t& layer = pOrgTIDLNetStructure.TIDLPCLayers[lyrIdx];

  if((layer.layerType == TIDL_PoolingLayer && layer.layerParams.poolParams.kernelH == 0 && layer.layerParams.poolParams.kernelW == 0) || /*global pooling*/
    layer.layerType == TIDL_FlattenLayer ||
    layer.layerType == TIDL_SoftMaxLayer ||
    layer.layerType == TIDL_InnerProductLayer ||
    layer.layerType == TIDL_DetectionOutputLayer ||
    layer.layerType == TIDL_ReduceLayer ||
    layer.layerType == TIDL_GatherLayer ||
    layer.layerType == TIDL_ReshapeLayer ||
    layer.layerType == TIDL_SqueezeLayer ||
    layer.layerType == TIDL_ScatterElementsLayer ||
    layer.layerType == TIDL_DataConvertLayer ||
    layer.layerType == TIDL_PadLayer ||
    layer.layerType == TIDL_CropLayer ||
    layer.layerType == TIDL_Deconv2DLayer||  
    TIDL_canSplitHeightForKernelSize(layer, numCores) ||
    (layer.inData[0].dimValues[TIDL_DIM_HEIGHT] < numCores && layer.inData[0].dimValues[TIDL_DIM_HEIGHT] > 0)  ||
    (layer.outData[0].dimValues[TIDL_DIM_HEIGHT] < numCores && layer.outData[0].dimValues[TIDL_DIM_HEIGHT] > 0) ||
    (layer.layerType == TIDL_DataLayer || layer.layerType == TIDL_ConstDataLayer))
  {
    split = 0;
  }
  if(layer.layerType == TIDL_DataConvertLayer) /* Data convert layer with layout change not supported on muliple cores */
  {
    if(layer.layerParams.dataConvertParams.layout != layer.layerParams.dataConvertParams.outLayout)
    {
      split = 0;
    }
    //:TODO: Temporary change to mark first and last data convert layer as split
    if(layer.outData[0].elementType == TIDL_SinglePrecFloat || layer.inData[0].elementType == TIDL_SinglePrecFloat )
    {
      split = 1;
    }
  }

  return split;
}

int32_t tidl_isLayerMatMul(sTIDL_LayerPC_t& layer)
{
  int32_t isMatMul = 0;

  if(layer.layerType == TIDL_InnerProductLayer && layer.numInBufs == 2) /*if layer is inner product with two inputs then it is MatMul layer*/
  {
    isMatMul = 1;
  }

  return isMatMul;
}

int32_t tidl_doesNetworkHasMatmulLayer(sTIDL_OrgNetwork_t& pOrgTIDLNetStructure)
{
  int32_t matMul = 0;

  for(int i = 0; i < pOrgTIDLNetStructure.numLayers; i++)
  {
    sTIDL_LayerPC_t& layer = pOrgTIDLNetStructure.TIDLPCLayers[i];

    if(tidl_isLayerMatMul(layer))
    {
      matMul = 1;
      break;
    }
  }

  return matMul;
}



/**
 *******************************************************************************
 *  @func         doesLayerRequireScratchMemory
 *  @brief        This function tells if the layer requires Scratch Memory
 *                Eventually this information is desired to be obtained from USER
 *                Currently it is done for specific layer only which require scratch memory
 *                and not for all layers to not unnecesarily fill BufferDatabase.
 *
 *  @param [in]   lyrIdx    : Layer Index
 *  @param [in]   pOrgTIDLNetStructure : PC Net structure
 *  @return       0/1 indicating if the layer requires scratch memory
 *  @remarks
 *******************************************************************************
 */

int32_t doesLayerRequireScratchMemory(sTIDL_OrgNetwork_t& pOrgTIDLNetStructure, int32_t lyrIdx)
{
  int32_t scratchMemRequired = 0;
  sTIDL_LayerPC_t& layer = pOrgTIDLNetStructure.TIDLPCLayers[lyrIdx];
  if(layer.layerType == TIDL_InnerProductLayer ||
    layer.layerType == TIDL_GatherLayer ||
    layer.layerType == TIDL_ScatterElementsLayer ||
    layer.layerType == TIDL_CustomLayer ||
    layer.layerType == TIDL_SoftMaxLayer ||
    layer.layerType == TIDL_LayerNormLayer)
    {
      scratchMemRequired = 1;
    }
  return scratchMemRequired;
}

/* This function checks if a particular layer is supported across multiple cores and updates the
corresponding state in layer properties. This updation is done for all layers in the network */
void tidl_updateMultiCoreState(sTIDL_OrgNetwork_t& pOrgTIDLNetStructure, tidl_import_config * params)
{

  int32_t numCores = params->numCores;

  for(int i = 0; i < pOrgTIDLNetStructure.numLayers; i++)
  {
    sTIDL_LayerPC_t& layer = pOrgTIDLNetStructure.TIDLPCLayers[i];
    if(tidl_doesLayerSupportSpatialSplit(pOrgTIDLNetStructure, i, numCores) == 1)
    {
      layer.multiCoreMode = TIDL_MULTI_CORE;
    }
    else
    {
      layer.multiCoreMode = TIDL_NOT_MULTI_CORE;
    }

    /* Force layers specified in singleCoreLayersNames user option to single core mode */
    if (strcmp((char *)params->singleCoreLayersNames, "") != 0)
    {
      char singleCoreLayers[TIDL_MAX_ALG_IN_BUFS][TIDL_MAX_DATA_NAME];
      int32_t numSingleCoreLayer = tidl_getStringsFromList((char *)params->singleCoreLayersNames, (char *)singleCoreLayers, TIDL_MAX_DATA_NAME);
      int32_t j = 0;

      for (j = 0; j < numSingleCoreLayer; j++)
      {
        if(strcmp((char * )layer.outDataNames[0], singleCoreLayers[j]) == 0)
        {
          layer.multiCoreMode = TIDL_NOT_MULTI_CORE;
        }
      }
    }
  }
}

void tidl_updateScratchMemoryRequirement(sTIDL_OrgNetwork_t& pOrgTIDLNetStructure)
{
  for(int i = 0; i < pOrgTIDLNetStructure.numLayers; i++)
  {
    sTIDL_LayerPC_t& layer = pOrgTIDLNetStructure.TIDLPCLayers[i];
    if(doesLayerRequireScratchMemory(pOrgTIDLNetStructure, i))
    {
      layer.scratchMemRequired = 1;
    }
    else
    {
      layer.scratchMemRequired = 0;
    }
  }
  return;
}

int32_t tidl_addCropLayerForMultiCore(sTIDL_OrgNetwork_t& pOrgTIDLNetStructure, int32_t layerIndex, int32_t * dataIndex, int32_t quantizationStyle, int32_t numCores)
{
  int8_t cropLayerNumber[TIDL_STRING_SIZE];
  int32_t count = 0;
  for(int i = 0; i < pOrgTIDLNetStructure.numLayers; i++)
  {
    sTIDL_LayerPC_t& layer = pOrgTIDLNetStructure.TIDLPCLayers[i];
    for(int inLyrIdx = 0; inLyrIdx < layer.numInBufs; inLyrIdx++)
    {
      int32_t prevLayerIdx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, layer.inData[inLyrIdx].dataId);

      if(! ((pOrgTIDLNetStructure.TIDLPCLayers[prevLayerIdx].layerType == TIDL_DataLayer) && (pOrgTIDLNetStructure.TIDLPCLayers[prevLayerIdx].numInBufs == -1)))
      {
        /* Do not add crop layer for input data layer */
        /*if we have layers that can not be split spacially one after the other we only add crop layer once*/
        if( pOrgTIDLNetStructure.TIDLPCLayers[i].multiCoreMode  ^ pOrgTIDLNetStructure.TIDLPCLayers[prevLayerIdx].multiCoreMode )
        {
          sTIDL_LayerPC_t& cropLayer = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex];

          cropLayer.layerType = TIDL_CropLayer;
          cropLayer.multiCoreMode = TIDL_MULTI_CORE;
          cropLayer.numInBufs = 1;
          cropLayer.numOutBufs = 1;
          cropLayer.outData[0].dataId = (*dataIndex)++;
          cropLayer.outConsumerCnt[0] = 1;
          cropLayer.outConsumerLinked[0] = 1;
          cropLayer.actParams.actType = TIDL_NoAct;
          cropLayer.outData[0].elementType = pOrgTIDLNetStructure.TIDLPCLayers[prevLayerIdx].outData[0].elementType;
          cropLayer.outData[0].batchPadW = pOrgTIDLNetStructure.TIDLPCLayers[prevLayerIdx].outData[0].batchPadW;
          cropLayer.outData[0].batchPadH = pOrgTIDLNetStructure.TIDLPCLayers[prevLayerIdx].outData[0].batchPadH;
          cropLayer.outData[0].numBatchW = pOrgTIDLNetStructure.TIDLPCLayers[prevLayerIdx].outData[0].numBatchW;
          cropLayer.outData[0].numBatchH = pOrgTIDLNetStructure.TIDLPCLayers[prevLayerIdx].outData[0].numBatchH;

          if(TIDL_QuantStyleAsymNP2_TFL == quantizationStyle)
            {
              cropLayer.outData[0].tensorScale = pOrgTIDLNetStructure.TIDLPCLayers[prevLayerIdx].outData[0].tensorScale;
              cropLayer.outData[0].tensorZeroPoint = pOrgTIDLNetStructure.TIDLPCLayers[prevLayerIdx].outData[0].tensorZeroPoint;
              cropLayer.actParams = pOrgTIDLNetStructure.TIDLPCLayers[prevLayerIdx].actParams;
            }

          for(int dim = 0; dim < TIDL_DIM_MAX; dim++) /*input and output dimensions are the same as the prev layer output dimension*/
          {
            cropLayer.inData[0].dimValues[dim] = cropLayer.outData[0].dimValues[dim] = pOrgTIDLNetStructure.TIDLPCLayers[prevLayerIdx].outData[0].dimValues[dim];
          }

          /*adding layer params*/
          if(pOrgTIDLNetStructure.TIDLPCLayers[i].multiCoreMode && !pOrgTIDLNetStructure.TIDLPCLayers[prevLayerIdx].multiCoreMode) //Split Mode
            cropLayer.layerParams.cropParams.multiCoreMode = TIDL_MULTI_CORE_CROP_SPLIT;
          else if(!pOrgTIDLNetStructure.TIDLPCLayers[i].multiCoreMode && pOrgTIDLNetStructure.TIDLPCLayers[prevLayerIdx].multiCoreMode) //Join Mode
            cropLayer.layerParams.cropParams.multiCoreMode = TIDL_MULTI_CORE_CROP_JOIN;

          /* Do no add crop layer to split, each core will directly be reading data from full buffer */
          if ( cropLayer.layerParams.cropParams.multiCoreMode != TIDL_MULTI_CORE_CROP_SPLIT)
          {
            cropLayer.layerParams.cropParams.numChannels = 0;
            cropLayer.layerParams.cropParams.offsetH = 0;
            cropLayer.layerParams.cropParams.offsetW = 0;

            cropLayer.inData[0].dataId = pOrgTIDLNetStructure.TIDLPCLayers[prevLayerIdx].outData[0].dataId;
            layer.inData[inLyrIdx].dataId = cropLayer.outData[0].dataId;

            strcpy((char*)cropLayer.inDataNames[0], (char*)pOrgTIDLNetStructure.TIDLPCLayers[prevLayerIdx].outDataNames[0]);
            /* Copy input name to output name */
            strcpy((char*)cropLayer.outDataNames[0], (char*)pOrgTIDLNetStructure.TIDLPCLayers[prevLayerIdx].outDataNames[0]);
            /* Update output name to be unique for each of the added crop layers 
                (else in case of multiple crop layers added as output of single layer, same name would result in issues in sorting layers in proc order) */
            // sprintf((char *)cropLayerNumber, "%d", count);
            // strcat(strcat((char*)cropLayer.outDataNames[0], "_crop_layer_"), (char *)cropLayerNumber);
            // count++;

            strcat((char*)cropLayer.outDataNames[0], "_crop_layer");
            /* TODO : In case a single layer results in multiple output crop layers, these crop layers can be combined together for optimization */

            strcpy((char*)layer.inDataNames[inLyrIdx], (char*)cropLayer.outDataNames[0]);

            layerIndex++;
            TIDL_UpdateInDataBuff(&pOrgTIDLNetStructure, layerIndex, cropLayer.outData[0]);
            TIDL_UpdateInDataBuff(&pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[prevLayerIdx].outData[0]);
          }
        }
      }
    }
  }
  pOrgTIDLNetStructure.numLayers = layerIndex;

  return 0;
}


int32_t tidl_addLayersForUVConversion(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t *dataIndex, tidl_import_config *params)
{
  int32_t inputLayer[TIDL_NUM_IN_BUFS];
  int32_t outLayerIdInputLayer[TIDL_NUM_IN_BUFS][TIDL_NUM_OUT_BUFS];
  int32_t numOutLayers[TIDL_NUM_IN_BUFS];
  int32_t uvLayerList[TIDL_NUM_IN_BUFS];
  int32_t newConcatLayerList[TIDL_NUM_IN_BUFS];
  int32_t numInLayers = 0, i, j;

  memset(&inputLayer[0], 0, sizeof(int32_t)*TIDL_NUM_IN_BUFS);
  memset(&numOutLayers[0], 0, sizeof(int32_t)*TIDL_NUM_IN_BUFS);
  memset(&uvLayerList[0], 0, sizeof(int32_t)*TIDL_NUM_IN_BUFS);
  memset(&newConcatLayerList[0], 0, sizeof(int32_t)*TIDL_NUM_IN_BUFS);
  memset(&outLayerIdInputLayer[0][0], 0, sizeof(int32_t)*TIDL_NUM_IN_BUFS*TIDL_NUM_OUT_BUFS);

  /* Find all input layers that have YUV420 format and their output */
  for(i=0; i<pOrgTIDLNetStructure.numLayers; i++)
  {
    if((pOrgTIDLNetStructure.TIDLPCLayers[i].numInBufs == -1) && (TIDL_inYuvFormatYuv420_NV12 == params->inYuvFormat[numInLayers]))
    {
      inputLayer[numInLayers] = i;
      for(j=0; j<pOrgTIDLNetStructure.TIDLPCLayers[i].numOutBufs; j++)
      {
        outLayerIdInputLayer[i][j] = tidl_getOutLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, pOrgTIDLNetStructure.TIDLPCLayers[i].outData[j].dataId);
      }
      numOutLayers[numInLayers] = j;
      numInLayers++;
    }
  }

  /* Add new data layer for UV semi planar data */
  tidl_addInDataLayerForUVData(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, dataIndex, params, &inputLayer[0], numInLayers);

  /* Add layer to convert UV420SP To UV Planar */
  tidl_addUVConversionLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, dataIndex, &uvLayerList[0]);

  /* Add concat layers using original input and the newly added layer */
  tidl_addConcatLayerForYUVMerge(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, dataIndex, &inputLayer[0], &outLayerIdInputLayer[0], &uvLayerList[0], &newConcatLayerList[0], &numOutLayers[0], numInLayers);

  /* In case of YUV input, normalization needs to be added as newly added concat layer's output */
  if(params->inDataNorm[0])
  {
    tidl_addNormLayerToConcat(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, dataIndex, params, &newConcatLayerList[0], numInLayers);
  }

  return 0;
}

int32_t tidl_mergeYUVtoRGBConversion(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t *dataIndex, tidl_import_config *params)
{
  int32_t convLayerList[TIDL_NUM_MAX_LAYERS];
  int32_t padLayerList[TIDL_NUM_MAX_LAYERS];
  int32_t i;
  int32_t numYuvMergeConvLayers = 0, numYuvMergePadLayers = 0;

  memset(&convLayerList[0], 0, sizeof(int32_t)*TIDL_NUM_MAX_LAYERS);
  memset(&padLayerList[0], 0, sizeof(int32_t)*TIDL_NUM_MAX_LAYERS);

  /* Find all the layers where the YUV to RGB conversion can be absorbed in convolution.
   * In case there are layers where layer following the concat is a non convolution layer
   * add a new 1x1 convolution layer to the network and add it to convLayerList.
   * Also add pad layer for (YUV to RGB conversion) or (YUV to RGB conversion + batchnorm) where needed
   * Pad values for the pad layer are not assigned in this function, they are assigned in tidl_addPadValuesForYUVtoRGBConversion
   * Add existing or newly added pad layers which as consumers of concat layer to padLayerList
   */
  tidl_addPadLayerAndConv1x1ForYUVtoRGB(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, dataIndex, params,
                                    &convLayerList[0], &padLayerList[0], &numYuvMergeConvLayers, &numYuvMergePadLayers);

  /* Make sure all convolutions in convLayerList have three input channels, else throw an error */
  for(i=0; i<numYuvMergeConvLayers; i++)
  {
    sTIDL_LayerPC_t &TIDLPCLayersLocal = pOrgTIDLNetStructure.TIDLPCLayers[convLayerList[i]];
    int32_t numInCh  = TIDLPCLayersLocal.layerParams.convParams.numInChannels;

    /* YUV to RGB folding is supported only when number of input channels is 3 */
    if(3 != numInCh)
    {
      printf("Number of input channels should be 3 for YUV to RGB conversion, current layer has %d channels\n", numInCh);
      printf("This model will not work\n");
    }
  }

  /* All layers are guaranteed to be convolution as per the above logic, hence merge directly */
  for(i=0; i<numYuvMergeConvLayers; i++)
  {
    tidl_mergeYuv444InConvLayer(pOrgTIDLNetStructure, convLayerList[i], params->inDataFormat[0]);
  }
  /* Add per channel values to be padded for pad layers */
  for(i = 0; i < numYuvMergePadLayers; i++)
  {
    tidl_addPadValuesForYUVtoRGBConversion(pOrgTIDLNetStructure, padLayerList[i], params->inDataFormat[0]);
  }

  return 0;
}

void tidl_mergeBNInConvLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, sTIDL_LayerPC_t &TIDLPCLayers, int32_t i1, int32_t chIdx)
{
  int32_t i2, i3;

  TIDLPCLayers.numMacs += pOrgTIDLNetStructure.TIDLPCLayers[i1].numMacs;
  TIDLPCLayers.actParams.actType = pOrgTIDLNetStructure.TIDLPCLayers[i1].actParams.actType;
  if (TIDLPCLayers.layerParams.convParams.enableBias == 0)
  {
    TIDLPCLayers.layerParams.convParams.enableBias = 1;
    TIDLPCLayers.bias.ptr = my_malloc(sizeof(float)*TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH]);
    TIDLPCLayers.bias.bufSize = TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH];
    float * dst = (float *)TIDLPCLayers.bias.ptr;
    for (i2 = 0; i2 < TIDLPCLayers.bias.bufSize; i2++)
    {
      dst[i2] = 0;
    }
  }
  /* Merge BN scale and Bias to Conv2d */
  float * weights = (float *)TIDLPCLayers.weights.ptr;
  float * bias = (float *)TIDLPCLayers.bias.ptr;

  float * scale = (float *)pOrgTIDLNetStructure.TIDLPCLayers[i1].weights.ptr;
  float * bias2 = (float *)pOrgTIDLNetStructure.TIDLPCLayers[i1].bias.ptr;
  int32_t weightsSize = (TIDLPCLayers.weights.bufSize / TIDLPCLayers.bias.bufSize);
  for (i2 = 0; i2 < TIDLPCLayers.bias.bufSize; i2++)
  {
    for (i3 = 0; i3 < weightsSize; i3++)
    {
      weights[i2*weightsSize + i3] *= scale[chIdx+i2];
    }
    bias[i2] = bias[i2] * scale[i2] + bias2[chIdx+i2];
  }
}


int32_t tidl_canMergeBNwithAllInLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, sTIDL_LayerPC_t &TIDLPCLayers, int32_t layerIndex)
{
  int32_t i2, i3;
  for (i2 = 0; i2 < TIDLPCLayers.numInBufs; i2++)
  {
    int32_t idx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, TIDLPCLayers.inData[i2].dataId);
    if (idx == -1)
    {
      return false;
    }
    sTIDL_LayerPC_t &inTIDLPCLayers = pOrgTIDLNetStructure.TIDLPCLayers[idx];
    if (((inTIDLPCLayers.layerType == TIDL_ConvolutionLayer) || (inTIDLPCLayers.layerType == TIDL_Deconv2DLayer)) &&
        (inTIDLPCLayers.outConsumerCnt[0] == 1) && (inTIDLPCLayers.actParams.actType == TIDL_NoAct))
    {
      continue;
    }
    else
    {
      return false;
    }
  }
  return true;
}
int32_t tidl_mergeBNLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i1, i2, i3, i4;
  int32_t status = 0;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_BatchNormLayer)
    {
      int32_t merged = 0;
      int32_t idx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0].dataId);
      if (idx == -1)
      {
        return -1;
      }
      sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure.TIDLPCLayers[idx];
      idx = tidl_getOutLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].dataId);
      if (idx == -1)
      {
        return -1;
      }
      sTIDL_LayerPC_t &outTIDLPCLayers = pOrgTIDLNetStructure.TIDLPCLayers[idx];

       if ((outTIDLPCLayers.layerType == TIDL_BatchNormLayer) &&
        (pOrgTIDLNetStructure.TIDLPCLayers[i1].outConsumerCnt[0] == 1) &&
        (pOrgTIDLNetStructure.TIDLPCLayers[i1].actParams.actType == TIDL_NoAct))
      {
        outTIDLPCLayers.numMacs += pOrgTIDLNetStructure.TIDLPCLayers[i1].numMacs;
        outTIDLPCLayers.inData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0];
        strcpy((char *)outTIDLPCLayers.inDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i1].inDataNames[0]);
        pOrgTIDLNetStructure.TIDLPCLayers[i1].numInBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[i1].numOutBufs = -1;
        /* Merge BN scale and Bias */
        float * weights = (float *)outTIDLPCLayers.weights.ptr;
        float * bias = (float *)outTIDLPCLayers.bias.ptr;

        float * weights0 = (float *)pOrgTIDLNetStructure.TIDLPCLayers[i1].weights.ptr;
        float * bias0 = (float *)pOrgTIDLNetStructure.TIDLPCLayers[i1].bias.ptr;
        /*
           T1 = T0*W0 + B0
           T2 = T1*W + B
           T2 = (T0*W0 + B0)*W + B
           T2 = (T0*W0*W + B0*W + B)
        */
        for (i2 = 0; i2 < outTIDLPCLayers.bias.bufSize; i2++)
        {
          bias[i2] = bias0[i2] * weights[i2] + bias[i2];
          weights[i2] *= weights0[i2];
        }
        my_free(weights0);
        my_free(bias0);
      }
      else if (((TIDLPCLayers.layerType == TIDL_ConvolutionLayer) || (TIDLPCLayers.layerType == TIDL_Deconv2DLayer)) &&
        (TIDLPCLayers.outConsumerCnt[0] == 1) && (TIDLPCLayers.actParams.actType == TIDL_NoAct))
      {
        tidl_mergeBNInConvLayer(pOrgTIDLNetStructure, TIDLPCLayers, i1, 0);
        merged = 1;
      }
      else if ((TIDLPCLayers.layerType == TIDL_ConcatLayer) && (TIDLPCLayers.outConsumerCnt[0] == 1) &&
      (TIDLPCLayers.actParams.actType == TIDL_NoAct))
      {
        if(tidl_canMergeBNwithAllInLayer(pOrgTIDLNetStructure, TIDLPCLayers, layerIndex))
        {
          int32_t currChIdx = 0;
          for (i2 = 0; i2 < TIDLPCLayers.numInBufs; i2++)
          {
            int32_t idx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, TIDLPCLayers.inData[i2].dataId);
            if (idx == -1)
            {
              return -1;
            }
            sTIDL_LayerPC_t &inTIDLPCLayers = pOrgTIDLNetStructure.TIDLPCLayers[idx];
            tidl_mergeBNInConvLayer(pOrgTIDLNetStructure, inTIDLPCLayers, i1, currChIdx);
            if(TIDLPCLayers.layerParams.concatParams.axis == TIDL_DIM_NUMCH)
            {
              currChIdx += inTIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH];
            }
            /* update out name here also update all the consumers in names.
            This is need for reference matching */
            char newNaame[TIDL_STRING_SIZE];
            strcpy(newNaame, (char *)inTIDLPCLayers.outDataNames[0]);
            strcat(newNaame, "_");
            strcat(newNaame, (char *)pOrgTIDLNetStructure.TIDLPCLayers[i1].outDataNames[0]);
            tidl_replaceInTensorName(&orgTIDLNetStructure, layerIndex, (char*)inTIDLPCLayers.outDataNames[0], (char*)newNaame);
            strcpy((char *)inTIDLPCLayers.outDataNames[0], newNaame);
          }
          merged = 1;
        }
      }
      if(merged == 1)
      {
        float * scale = (float *)pOrgTIDLNetStructure.TIDLPCLayers[i1].weights.ptr;
        float * bias2 = (float *)pOrgTIDLNetStructure.TIDLPCLayers[i1].bias.ptr;
        my_free(scale);
        my_free(bias2);

        TIDLPCLayers.outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0];
        strcpy((char *)TIDLPCLayers.outDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i1].outDataNames[0]);
        TIDLPCLayers.outConsumerCnt[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].outConsumerCnt[0];
        pOrgTIDLNetStructure.TIDLPCLayers[i1].numInBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[i1].numOutBufs = -1;
      }
    }
  }

  return 0;
}

int32_t tidl_getDimPadLayerWithStrideOffset(int32_t * padT, int32_t * padB, int32_t * padL, int32_t * padR,
                       const sTIDL_ConvParams_t * convParams,
                       int32_t convOutHeight, int32_t convOutWidth,
                       int32_t convInHeight, int32_t convInWidth, int32_t * strideOffsetMethod)
{
  int32_t coeffsWidth = convParams->kernelW;
  int32_t coeffsHeight = convParams->kernelH;
  int32_t dilationWidth = convParams->dilationW;
  int32_t dilationHeight = convParams->dilationH;
  int32_t strideWidth = convParams->strideW;
  int32_t strideHeight = convParams->strideH;

  int32_t effectiveKw = ((coeffsWidth - 1)*dilationWidth) + 1;
  int32_t effectiveKh = ((coeffsHeight - 1)*dilationHeight) + 1;
  int32_t filter_left_offset =
    ((convOutWidth - 1) * strideWidth + effectiveKw - convInWidth) / 2;
  int32_t filter_top_offset =
    ((convOutHeight - 1) * strideHeight + effectiveKh - convInHeight) / 2;

  int32_t padShiftH = (effectiveKh / 2) - filter_top_offset;
  int32_t padShiftW = (effectiveKw / 2) - filter_left_offset;

  /*TIDL-2789: padShift cannot be negative*/
  padShiftH = padShiftH < 0 ? 0 : padShiftH;
  padShiftW = padShiftW < 0 ? 0 : padShiftW;

  if(padShiftH == 0 && padShiftW == 0)
  {
    *strideOffsetMethod = TIDL_StrideOffsetTopLeft;
  }
  else
  {
    if((gParams.modelType == 1) || (gParams.modelType == 3) || (gParams.modelType == 4) || (gParams.modelType == 5) || (gParams.modelType == 6))
    {
      if(*strideOffsetMethod != TIDL_StrideOffsetTopLeft)
      {
        /* strideOffset for tf/tflite is TopLeft and not Center in the case with VALID convolution and padL == padW and padT == padH */
        *padT = *padT - padShiftH;
        *padL = *padL - padShiftW;
        *strideOffsetMethod = TIDL_StrideOffsetTopLeft;
      }
    }
    else
    {
      *padB = *padB - padShiftH;
      *padR = *padR - padShiftW;
    }
  }
  return 0;
}

// Convert BiasLayer to BatchNormLayer.
//   This function should be called after tidl_mergeBiasLayer() which merges
//   BiasAdd into Conv2d or InnerProduct.
int32_t tidl_convertBiasToBNLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t numLayers)
{
  for(int i = 0; i < numLayers; i++)
  {
    if(pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_BiasLayer)
    {
      sTIDL_LayerPC_t &TIDLPCLayer = pOrgTIDLNetStructure.TIDLPCLayers[i];
      TIDLPCLayer.layerType = TIDL_BatchNormLayer;
      int32_t dataSize = TIDLPCLayer.outData[0].dimValues[TIDL_DIM_NUMCH];
      if(dataSize != TIDLPCLayer.bias.bufSize)
      {
        printf("Error in converting Bias to BN: params size is not right.\n");
        return -1;
      }
      TIDLPCLayer.weights.ptr = my_malloc(dataSize*sizeof(float));
      TIDLPCLayer.weights.bufSize = dataSize;
      float * scalePtr = (float*)TIDLPCLayer.weights.ptr;
      for (int j = 0; j < dataSize; j++)
      {
        // make weights to be 1 and keep bias as is to implement bias add in BN
        scalePtr[j] = 1.0;
      }
    }
  }

  return 0;
}

int32_t tidl_mergePreBNLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i1, i2, i3, i4;
  int32_t status = 0;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    if ((pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_BatchNormLayer) &&
       (pOrgTIDLNetStructure.TIDLPCLayers[i1].outConsumerCnt[0] == 1) &&
       (pOrgTIDLNetStructure.TIDLPCLayers[i1].actParams.actType == TIDL_NoAct))
    {
      int32_t mergeBN = 1;
      int32_t  idx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0].dataId);
      if (idx == -1)
      {
        return -1;
      }
      sTIDL_LayerPC_t &TIDLPCLayersIn = pOrgTIDLNetStructure.TIDLPCLayers[idx];

      idx = tidl_getOutLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].dataId);
      if (idx == -1)
      {
        return -1;
      }
      sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure.TIDLPCLayers[idx];
      if ((TIDLPCLayers.layerType == TIDL_ConvolutionLayer) && (mergeBN == 1))
      {
        if (TIDLPCLayers.layerParams.convParams.enableBias == 0)
        {
          TIDLPCLayers.layerParams.convParams.enableBias = 1;
          TIDLPCLayers.bias.ptr = my_malloc(sizeof(float)*TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH]);
          TIDLPCLayers.bias.bufSize = TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH];
          float * dst = (float *)TIDLPCLayers.bias.ptr;
          for (i2 = 0; i2 < TIDLPCLayers.bias.bufSize; i2++)
          {
            dst[i2] = 0;
          }
        }
        /* Merge BN scale and Bias to Conv2d */
        float * weights = (float *)TIDLPCLayers.weights.ptr;
        float * bias = (float *)TIDLPCLayers.bias.ptr;

        float * scale = (float *)pOrgTIDLNetStructure.TIDLPCLayers[i1].weights.ptr;
        float * bias2 = (float *)pOrgTIDLNetStructure.TIDLPCLayers[i1].bias.ptr;

        int32_t numInCh  = pOrgTIDLNetStructure.TIDLPCLayers[i1].weights.bufSize;
        int32_t numOutCh = TIDLPCLayers.bias.bufSize;
        int32_t weightsSize = ((TIDLPCLayers.weights.bufSize / TIDLPCLayers.bias.bufSize))/ numInCh;

        /* Merge BN parameters into convolution parameters */
        for (i2 = 0; i2 < numOutCh; i2++)
        {
          float bnBias = 0;
          for (i3 = 0; i3 < numInCh; i3++)
          {
            for (i4 = 0; i4 < weightsSize; i4++)
            {
              bnBias += bias2[i3] * weights[i2*numInCh*weightsSize + i3*weightsSize + i4];
              weights[i2*numInCh*weightsSize + i3*weightsSize + i4] *= scale[i3];
            }
          }
          bias[i2] = bnBias + bias[i2];
        }
        int32_t padH = TIDLPCLayers.layerParams.convParams.padH;
        int32_t padW = TIDLPCLayers.layerParams.convParams.padW;

        if((((TIDLPCLayersIn.layerType == TIDL_DataLayer) && (TIDLPCLayersIn.numOutBufs > 0)) && (gParams.inDataPadInTIDL[0] == 0))
             || ((TIDLPCLayersIn.layerType != TIDL_DataLayer) && (gParams.foldPreBnConv2D == 1))
             || (padH == 0 && padW == 0))
        {
          /* if input to BN is input data layer and inDataPadInTIDL = 0, no padding to be done, just remove the BN layer */
          TIDLPCLayers.inData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0];
          strcpy((char *)TIDLPCLayers.inDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i1].inDataNames[0]);
          pOrgTIDLNetStructure.TIDLPCLayers[i1].numInBufs = -1;
          pOrgTIDLNetStructure.TIDLPCLayers[i1].numOutBufs = -1;
        }
        else
        {
          int32_t padT = padH, padB = padH;
          int32_t padL = padW, padR = padW;
          tidl_getDimPadLayerWithStrideOffset(&padT, &padB, &padL, &padR,
                          &(TIDLPCLayers.layerParams.convParams),
                          TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT],TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH],
                          TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT], TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH],
                          &(TIDLPCLayers.strideOffsetMethod));
          /* Convert BN layer to Pad Layer in all cases except input data layer with inDataPadInTIDL = 0 */
          pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType = TIDL_PadLayer;
          pOrgTIDLNetStructure.TIDLPCLayers[i1].actParams.actType = TIDL_NoAct;
          pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.padLayerParams.padT = padT;
          pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.padLayerParams.padB = padB;
          pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.padLayerParams.padL = padL;
          pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.padLayerParams.padR = padR;
          TIDLPCLayers.layerParams.convParams.padH = 0;
          TIDLPCLayers.layerParams.convParams.padW = 0;
          pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].dimValues[TIDL_DIM_HEIGHT] = pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].dimValues[TIDL_DIM_HEIGHT] + padT + padB; /* dim of pad layer */
          pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].dimValues[TIDL_DIM_WIDTH] = pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].dimValues[TIDL_DIM_WIDTH] + padL + padR;
          pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].elementType = pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0].elementType;
          TIDLPCLayers.inData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0];
          tidl_setLRTBpad(TIDLPCLayers);
          /* Populate the values to be padded */
          int32_t dataSize = pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].dimValues[TIDL_DIM_NUMCH]; /* number of channels */

          pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.padLayerParams.padType = TIDL_PadPerChannel;

          pOrgTIDLNetStructure.TIDLPCLayers[i1].perChannelPadConstTensor.ptr = my_malloc(dataSize*sizeof(float));
          pOrgTIDLNetStructure.TIDLPCLayers[i1].perChannelPadConstTensor.bufSize = dataSize;
          float *  perChannelPadConstTensor  = (float *)pOrgTIDLNetStructure.TIDLPCLayers[i1].perChannelPadConstTensor.ptr;
          for (int32_t j = 0; j < dataSize; j++)
          {
            perChannelPadConstTensor[j] = -1.0 * bias2[j] / scale[j];  /* mean of the BN layer */
          }
        }
        my_free(scale);
        scale = NULL;
        pOrgTIDLNetStructure.TIDLPCLayers[i1].weights.ptr = NULL;
        my_free(bias2);
        bias2 = NULL;
        pOrgTIDLNetStructure.TIDLPCLayers[i1].bias.ptr = NULL;
      }
    }
  }

  return 0;
}

int32_t tidl_mergeFlattenLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i1, i2, i3, i4;
  int32_t status = 0;
  int32_t merged;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_FlattenLayer)
    {
      merged = 1;
      for (i2 = 0; i2 < TIDL_DIM_MAX-1; i2++)
      {
        if ((pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0].dimValues[i2] != 1) ||
          (pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].dimValues[i2] != 1))
        {
          merged = 0;
          break;
        }
      }
      int32_t  inIdx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0].dataId);
      if (inIdx != -1)
      {
        sTIDL_LayerPC_t &TIDLPCLayersIn = pOrgTIDLNetStructure.TIDLPCLayers[inIdx];

        if ((TIDLPCLayersIn.layerType == TIDL_PoolingLayer) &&
            (TIDLPCLayersIn.layerParams.poolParams.poolingType == TIDL_AveragePooling) &&
            (TIDLPCLayersIn.outConsumerCnt[0] == 1) &&
            (TIDLPCLayersIn.layerParams.poolParams.kernelW == 0) &&
            (TIDLPCLayersIn.layerParams.poolParams.kernelH == 0))
        {
            merged = 1;
        }
        if (TIDLPCLayersIn.layerType == TIDL_ReshapeLayer)
        {
          merged = 1;
        }
      }

      if (merged == 1)
      {
        int32_t  idx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0].dataId);
        if (idx == -1)
        {
          return -1;
        }
        sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure.TIDLPCLayers[idx];
        TIDLPCLayers.numMacs += pOrgTIDLNetStructure.TIDLPCLayers[i1].numMacs;
        // Preserve input DataLayer's tensorScale
        if (TIDLPCLayers.layerType == TIDL_DataLayer)
          pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].tensorScale =
                                   TIDLPCLayers.outData[0].tensorScale;
        TIDLPCLayers.outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0];
        strcpy((char *)TIDLPCLayers.outDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i1].outDataNames[0]);
        TIDLPCLayers.outConsumerCnt[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].outConsumerCnt[0];
        pOrgTIDLNetStructure.TIDLPCLayers[i1].numInBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[i1].numOutBufs = -1;
      }
    }
  }

  return 0;
}
int32_t tidl_mergeReluLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i1, i2, i3, i4;
  int32_t status = 0;
  int32_t merged;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_ReLULayer)
    {
      merged = 0;
      int32_t  idx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0].dataId);
      if (idx == -1)
      {
        return -1;
      }
      sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure.TIDLPCLayers[idx];
      if (((TIDLPCLayers.layerType == TIDL_ConvolutionLayer) || (TIDLPCLayers.layerType == TIDL_Deconv2DLayer) ) &&
        (TIDLPCLayers.outConsumerCnt[0] == 1) && ((pOrgTIDLNetStructure.TIDLPCLayers[i1].actParams.actType == TIDL_RelU) ||
        (pOrgTIDLNetStructure.TIDLPCLayers[i1].actParams.actType == TIDL_RelU6) ))
      {
        merged = 1;
      }
      if ((TIDLPCLayers.layerType == TIDL_EltWiseLayer) &&
        (TIDLPCLayers.outConsumerCnt[0] == 1) )
      {
        merged = 1;
      }
      if ((TIDLPCLayers.layerType == TIDL_BatchNormLayer) &&
        (TIDLPCLayers.outConsumerCnt[0] == 1))
      {
        merged = 1;
      }
      if ((TIDLPCLayers.layerType == TIDL_InnerProductLayer) &&
        (TIDLPCLayers.outConsumerCnt[0] == 1))
      {
        merged = 1;
      }
      if (merged == 1)
      {
        TIDLPCLayers.actParams = pOrgTIDLNetStructure.TIDLPCLayers[i1].actParams;
        TIDLPCLayers.numMacs += pOrgTIDLNetStructure.TIDLPCLayers[i1].numMacs;
        TIDLPCLayers.outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0];
        strcpy((char *)TIDLPCLayers.outDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i1].outDataNames[0]);
        TIDLPCLayers.actParams.actType = pOrgTIDLNetStructure.TIDLPCLayers[i1].actParams.actType;
        TIDLPCLayers.outConsumerCnt[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].outConsumerCnt[0];
        pOrgTIDLNetStructure.TIDLPCLayers[i1].numInBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[i1].numOutBufs = -1;
      }
      else
      {
        // TODO: Confirm?
        //pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType = TIDL_BatchNormLayer;
        //TIDLPCLayers.actParams.actType = TIDL_RelU;
      }
    }
  }

  return 0;
}

#define NUM_LAYERNORM_SUBLAYERS (7U)
/*
  Validate if starting from layer at nodeIdx, if there is a GELU layer
*/
int32_t tidl_validateLayerNormLayer(sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t layerIndex, int32_t nodeIdx, int32_t *layerNormIdx)
{
  int32_t layerNormLayerTypes[NUM_LAYERNORM_SUBLAYERS] = {TIDL_ReduceMeanLayer, TIDL_SubLayer, TIDL_PowLayer, TIDL_ReduceMeanLayer, TIDL_BatchNormLayer, TIDL_SqrtLayer, TIDL_DivLayer};
  layerNormIdx[0] = nodeIdx;  // set the first node
  // check for outputs
  for (int i = 0; i < (NUM_LAYERNORM_SUBLAYERS - 1); i++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[layerNormIdx[i]].numOutBufs > 1) // not gelu, more than one output
    {
      return -1;
    }
    else
    {
      bool nextLayerFound = false;
      int32_t outIdx = -1;
      //Loop over all consumers of the ith layer and check if they are part of the layer template:
      for(int32_t j = 0; j < pOrgTIDLNetStructure.TIDLPCLayers[layerNormIdx[i]].outConsumerCnt[0]; j++)
      {
        // get consumer for i-th layer's data
        outIdx = tidl_getOutLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[layerNormIdx[i]].outData[j].dataId);
        if (outIdx == -1){  // invalid, no consumer
          continue;
        }
        else
        {
          //MOV ://layerNormIdx[i+1] = outIdx; // next layer in gelu is output of current layer
          if (pOrgTIDLNetStructure.TIDLPCLayers[outIdx].layerType != layerNormLayerTypes[i+1])
          {
            /*Mismatch in layer type*/
            continue;
          }
          else
          {
            nextLayerFound = true;
            break;
          }
        }
      }
      if(nextLayerFound)
      {
        layerNormIdx[i+1] = outIdx; /*Update outIdx*/
      }
      else
      {
        return -1;
      }
    }
  }
  /*Add SUB, Div consumer checks*/

  return 0;
}

/*
  Validate if starting from layer at nodeIdx, if there is a GELU layer
*/
int32_t tidl_validateGeluLayer (sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t layerIndex, int32_t nodeIdx, int32_t *geluLayerIdx)
{
  int32_t geluLayerTypes[4] = {TIDL_BatchNormLayer, TIDL_IdentityLayer, TIDL_BatchNormLayer, TIDL_EltWiseLayer};
  geluLayerIdx[0] = nodeIdx;  // set the first node
  // check for outputs
  for (int i = 0; i < 3; i++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[i]].numOutBufs > 1) // not gelu, more than one output
    {
      return -1;
    }
    else
    {
      // get consumer for i-th layer's data
      int32_t outIdx = tidl_getOutLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[i]].outData[0].dataId);

      if (outIdx == -1){  // invalid, no consumer
        return -1;
      }
      else
      {
        geluLayerIdx[i+1] = outIdx; // next layer in gelu is output of current layer
        if (pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[i+1]].layerType != geluLayerTypes[i+1])  // mismatch in layer type
        {
          return -1;
        }
      }
    }
  }

  // last layer has not been cheked in previous loop
  if(pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[3]].numOutBufs > 1)
  {
    return -1;
  }

  // check if input to first node is same as one input to mul
  // producer of first node's input i.e., previous add
  int32_t geluInIdx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[0]].inData[0].dataId);
  if (geluInIdx == -1)  // invalid, no producer
  {
    return -1;
  }
  else
  {
    sTIDL_LayerPC_t *geluInputProducer = &pOrgTIDLNetStructure.TIDLPCLayers[geluInIdx];
    sTIDL_LayerPC_t *geluMulLayer = &pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[3]];
    // check if any input in mul is same as first output from previous add
    if(
        geluInputProducer->outData[0].dataId != geluMulLayer->inData[0].dataId &&   // not 1st input
        geluInputProducer->outData[0].dataId != geluMulLayer->inData[1].dataId      // neither 2nd input
      )
    {
      return -1;
    }
  }

  return 0;
}


/*
  Merge all GELU components to single GELU layer
*/
int32_t tidl_mergeLayerNormLayer (sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i;
  int32_t status = 0;
  int32_t merged;
  int32_t layerNormLayerIdx[NUM_LAYERNORM_SUBLAYERS] = {-1};
  for(i = 0; i < NUM_LAYERNORM_SUBLAYERS; i++)
  {
    layerNormLayerIdx[i] = -1;
  }

  for (i = 0; i < layerIndex; i++)
  {
    if( (pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_ReduceMeanLayer) &&
        (tidl_validateLayerNormLayer(pOrgTIDLNetStructure, layerIndex, i, layerNormLayerIdx) == 0)
      )
    {
      /*Layernorm merge steps:
        1. Fuse all layernorm layers into the reduceMean node
        2. Adjust the consumer layers properties for this particular fusion
        3. Delete irrelevant layers
        */
      //int32_t layerNormLayerTypes[NUM_LAYERNORM_SUBLAYERS] = {TIDL_ReduceMeanLayer, TIDL_SubLayer, TIDL_PowLayer, TIDL_ReduceMeanLayer, TIDL_BatchNormLayer, TIDL_SqrtLayer, TIDL_DivLayer};
      int32_t layerNormOutput = tidl_getOutLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[layerNormLayerIdx[NUM_LAYERNORM_SUBLAYERS - 1]].outData[0].dataId);
      if (layerNormOutput == -1)
      {
        return -1;
      }
      else
      {
        pOrgTIDLNetStructure.TIDLPCLayers[i].layerType = TIDL_LayerNormLayer;
        sTIDL_LayerNormParams_t &layerNormParams = pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.layerNormParams;
        layerNormParams.axis = TIDL_DIM_WIDTH;
        /*Get epsilon value from BN:*/
        float32_tidl epsilon = 0;
        /*Update epsilon from constituent add layer:*/
        epsilon = *((float32_tidl*)pOrgTIDLNetStructure.TIDLPCLayers[layerNormLayerIdx[4]].bias.ptr);
        layerNormParams.epsilon = epsilon;
        /*TODO: Update LayerNorm MACS:*/
        pOrgTIDLNetStructure.TIDLPCLayers[i].numMacs = 0U;
        /*Update output:*/
        /* Output of first node = output of last node*/
        pOrgTIDLNetStructure.TIDLPCLayers[layerNormLayerIdx[0]].outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[layerNormLayerIdx[NUM_LAYERNORM_SUBLAYERS - 1]].outData[0];
        strcpy((char*)pOrgTIDLNetStructure.TIDLPCLayers[layerNormLayerIdx[0]].outDataNames[0], (char*)pOrgTIDLNetStructure.TIDLPCLayers[layerNormLayerIdx[NUM_LAYERNORM_SUBLAYERS - 1]].outDataNames[0]);
        /*Discard other layers:*/
        for(int32_t j = 1; j < NUM_LAYERNORM_SUBLAYERS; j++)
        {
          pOrgTIDLNetStructure.TIDLPCLayers[layerNormLayerIdx[j]].numInBufs  = -1;
          pOrgTIDLNetStructure.TIDLPCLayers[layerNormLayerIdx[j]].numOutBufs = -1;
        }
        /*Update # consumers for input of layernorm:*/
        int32_t layerNormInIdx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[layerNormLayerIdx[0]].inData[0].dataId);
        if (layerNormInIdx == -1)  // invalid, no producer
        {
          return -1;
        }
        else
        {
          pOrgTIDLNetStructure.TIDLPCLayers[layerNormInIdx].outConsumerCnt[0]--;
          pOrgTIDLNetStructure.TIDLPCLayers[layerNormInIdx].outConsumerLinked[0]--;
        }
      }
      #if 0
      // merge
      pOrgTIDLNetStructure.TIDLPCLayers[i].numMacs += pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[1]].numMacs;
      pOrgTIDLNetStructure.TIDLPCLayers[i].numMacs += pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[2]].numMacs;
      pOrgTIDLNetStructure.TIDLPCLayers[i].numMacs += pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[3]].numMacs;

      pOrgTIDLNetStructure.TIDLPCLayers[i].actParams.actType = TIDL_GELU;

      // output from gelu
      int32_t geluOutIdx = tidl_getOutLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[3]].outData[0].dataId);

      // no consumer from gelu
      if (geluOutIdx == -1)
      {
        return -1;
      }
      else
      {
        // in gelu block,make
        // output of first node = output of last node
        pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[0]].outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[3]].outData[0];
        strcpy((char*)pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[0]].outDataNames[0], (char*)pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[3]].outDataNames[0]);

        // discard other 3 layers
        pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[1]].numInBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[1]].numOutBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[2]].numInBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[2]].numOutBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[3]].numInBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[3]].numOutBufs = -1;
      }
    #endif
    }
  }


  return 0;
}

/*
  Merge all GELU components to single GELU layer
*/
int32_t tidl_mergeGeluLayer (sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i;
  int32_t status = 0;
  int32_t merged;
  int32_t geluLayerIdx[4] = {-1, -1, -1, -1};
  for (i = 0; i < layerIndex; i++)
  {
    if( // check if gelu exists
        (pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_BatchNormLayer)           &&
        (tidl_validateGeluLayer(pOrgTIDLNetStructure, layerIndex, i, geluLayerIdx) == 0)
      )
    {
      // merge
      pOrgTIDLNetStructure.TIDLPCLayers[i].numMacs += pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[1]].numMacs;
      pOrgTIDLNetStructure.TIDLPCLayers[i].numMacs += pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[2]].numMacs;
      pOrgTIDLNetStructure.TIDLPCLayers[i].numMacs += pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[3]].numMacs;

      pOrgTIDLNetStructure.TIDLPCLayers[i].actParams.actType = TIDL_GELU;

      // output from gelu
      int32_t geluOutIdx = tidl_getOutLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[3]].outData[0].dataId);

      // no consumer from gelu
      if (geluOutIdx == -1)
      {
        return -1;
      }
      else
      {
        // in gelu block,make
        // output of first node = output of last node
        pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[0]].outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[3]].outData[0];
        strcpy((char*)pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[0]].outDataNames[0], (char*)pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[3]].outDataNames[0]);

        // discard other 3 layers
        pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[1]].numInBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[1]].numOutBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[2]].numInBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[2]].numOutBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[3]].numInBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[geluLayerIdx[3]].numOutBufs = -1;
      }


    }
  }


  return 0;
}


/*
  Returns 1 if Tranpose is happening only in last 2 dims
  else returns 0
*/
int32_t isTransposeLast2Dims (sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIdx)
{
  int i;
  sTIDL_TransposeParams_t &tpParams = pOrgTIDLNetStructure.TIDLPCLayers[layerIdx].layerParams.transposeParams;
  if (
      // check if last 2 are swapped
      (tpParams.perm[TIDL_DIM_MAX-1] == (TIDL_DIM_MAX-2)) &&
      (tpParams.perm[TIDL_DIM_MAX-2] == TIDL_DIM_MAX-1)
    )
  {
    return 1;
  }

  return 0;
}

/*
  Search if InnerProduct layer has B side input coming from a transpose
  Then absorb the transpose in the InnerProduct layer if suitable
*/
int32_t  tidl_InnerProductBTranspose (sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t numLayers){
  int i;
  for (i = 0; i < numLayers; i++){
    // check for InnerProduct layer
    if (pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_InnerProductLayer)
    {
      // get B side input producer
      sTIDL_LayerPC_t& ipLayer = pOrgTIDLNetStructure.TIDLPCLayers[i];
      int32_t bSideProducerIdx = tidl_getInLayer(pOrgTIDLNetStructure, numLayers, ipLayer.inData[1].dataId);

      // check for valid transpose
      if (
          (pOrgTIDLNetStructure.TIDLPCLayers[bSideProducerIdx].layerType == TIDL_TransposeLayer) &&
          isTransposeLast2Dims(pOrgTIDLNetStructure, bSideProducerIdx)
        )
      {
        // set b side transpose in innerproduce
        ipLayer.layerParams.innerProductParams.inputBTranspose = 1;

        // copy the input
        // input of transpose is B-side input of innerproduct
        ipLayer.inData[1] = pOrgTIDLNetStructure.TIDLPCLayers[bSideProducerIdx].inData[0];
        strcpy((char*)ipLayer.inDataNames[1], (char*)pOrgTIDLNetStructure.TIDLPCLayers[bSideProducerIdx].inDataNames[0]);

        // set transpose for removing
        pOrgTIDLNetStructure.TIDLPCLayers[bSideProducerIdx].numInBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[bSideProducerIdx].numOutBufs = -1;
      }

    }
  }

  return 1;
}



int32_t tidl_mergeMinimumLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i1, i2, i3, i4;
  int32_t status = 0;
  int32_t merged;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    if ((pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_MinimumLayer) &&
        (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerPCParams.minimumParams.min == 6))
    {
      merged = 0;
      int32_t  idx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0].dataId);
      if (idx == -1)
      {
        return -1;
      }
      sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure.TIDLPCLayers[idx];
      if ((TIDLPCLayers.actParams.actType == TIDL_RelU) && (TIDLPCLayers.outConsumerCnt[0] == 1))
      {
        merged = 1;
      }
      if (merged == 1)
      {
        TIDLPCLayers.actParams.actType = TIDL_RelU6;
        TIDLPCLayers.numMacs += pOrgTIDLNetStructure.TIDLPCLayers[i1].numMacs;
        TIDLPCLayers.outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0];
        strcpy((char *)TIDLPCLayers.outDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i1].outDataNames[0]);
        TIDLPCLayers.outConsumerCnt[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].outConsumerCnt[0];
        pOrgTIDLNetStructure.TIDLPCLayers[i1].numInBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[i1].numOutBufs = -1;
      }
    }
  }

  return 0;
}


void tidl_poolPadParams(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i1;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_PoolingLayer)
    {
      sTIDL_PoolingParams_t &poolParams = pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.poolParams;
      sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure.TIDLPCLayers[i1];
      if((gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TFLITE) ||
        (gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TFLITE_RT))
      {
        if((poolParams.strideH == 2) && ((TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT] & 1)  == 0))
        {
          if(poolParams.padH > 0)
          {
            poolParams.padH -= 1;
          }
        }
        if((poolParams.strideW == 2) && ((TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH] & 1)  == 0))
        {
          if(poolParams.padW > 0)
          {
            poolParams.padW -= 1;
          }
        }
      }
      if((gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_ONNX) ||
        (gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_ONNX_RT))
      {
        if(TIDLPCLayers.strideOffsetMethod == TIDL_StrideOffsetCenter) /* padL < padR and padT < padB*/
        {
          poolParams.padH -= 1;
          poolParams.padW -= 1;
          poolParams.padT -= 1;
          poolParams.padL -= 1;
        }
        TIDLPCLayers.strideOffsetMethod = TIDL_StrideOffsetTopLeft; /* Reset stride offset method to make sure pad values don't keep getting reduced if this
        function is called multiple times */
      }

    }
  }
}


int32_t tidl_mergeClipLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i1, i2, i3, i4;
  int32_t status = 0;
  int32_t merged;
  int32_t numClipMerged = 0;
  do
  {
    numClipMerged = 0;
    for (i1 = 0; i1 < layerIndex; i1++)
    {
      if ((pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_ClipLayer) && (pOrgTIDLNetStructure.TIDLPCLayers[i1].numInBufs != -1))
      {
        merged = 0;
        int32_t  idx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0].dataId);
        if (idx == -1)
        {
          return -1;
        }
        sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure.TIDLPCLayers[idx];
        if ((TIDLPCLayers.layerType == TIDL_ConvolutionLayer) &&
          (TIDLPCLayers.outConsumerCnt[0] == 1) && (pOrgTIDLNetStructure.TIDLPCLayers[i1].actParams.actType == TIDL_Clip))
        {
          merged = 1;
        }
        if ((TIDLPCLayers.layerType == TIDL_EltWiseLayer) &&
          (TIDLPCLayers.outConsumerCnt[0] == 1) && (pOrgTIDLNetStructure.TIDLPCLayers[i1].actParams.actType == TIDL_Clip))
        {
          merged = 1;
        }
        if ((TIDLPCLayers.layerType == TIDL_ConcatLayer) &&
          (TIDLPCLayers.outConsumerCnt[0] == 1) && (pOrgTIDLNetStructure.TIDLPCLayers[i1].actParams.actType == TIDL_Clip))
        {
          merged = 1;
        }
        if ((TIDLPCLayers.layerType == TIDL_BatchNormLayer) &&
          (TIDLPCLayers.outConsumerCnt[0] == 1))
        {
          merged = 1;
        }
        if ((TIDLPCLayers.layerType == TIDL_ClipLayer) &&
          (TIDLPCLayers.outConsumerCnt[0] == 1))
        {
          merged = 1;
        }
        if ((TIDLPCLayers.layerType == TIDL_ReLULayer) &&
           (TIDLPCLayers.outConsumerCnt[0] == 1))
        {
          TIDLPCLayers.layerType = TIDL_ClipLayer;
          merged = 1;
        }
        if ((TIDLPCLayers.layerType == TIDL_InnerProductLayer) &&
          (TIDLPCLayers.outConsumerCnt[0] == 1) && (pOrgTIDLNetStructure.TIDLPCLayers[i1].actParams.actType == TIDL_Clip))
        {
          merged = 1;
        }
        if (merged == 1)
        {
          numClipMerged++;
          float32_tidl     clipMin = -FLT_MAX;
          float32_tidl     clipMax =  FLT_MAX;
          if (TIDLPCLayers.actParams.actType == TIDL_RelU)
          {
            clipMin = 0;
          }
          else if (TIDLPCLayers.actParams.actType == TIDL_RelU6)
          {
            clipMin = 0;
            clipMax = 6.0;
          }
          else if(TIDLPCLayers.actParams.actType == TIDL_Clip)
          {
            clipMin = TIDLPCLayers.actParams.clipMin;
            clipMax = TIDLPCLayers.actParams.clipMax;
          }

          TIDLPCLayers.actParams = pOrgTIDLNetStructure.TIDLPCLayers[i1].actParams;
          TIDLPCLayers.actParams.clipMin = clipMin > TIDLPCLayers.actParams.clipMin ? clipMin : TIDLPCLayers.actParams.clipMin;
          TIDLPCLayers.actParams.clipMax = clipMax < TIDLPCLayers.actParams.clipMax ? clipMax : TIDLPCLayers.actParams.clipMax;
          
          TIDLPCLayers.numMacs += pOrgTIDLNetStructure.TIDLPCLayers[i1].numMacs;
          TIDLPCLayers.outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0];
          /*Update the element type of the merged layer based on clip values*/
          if(TIDLPCLayers.actParams.clipMin >= 0)
          {
            TIDLPCLayers.outData[0].elementType = tidl_getElementType(0);
          }
          else
          {
            TIDLPCLayers.outData[0].elementType = tidl_getElementType(1);
          }
          strcpy((char *)TIDLPCLayers.outDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i1].outDataNames[0]);
          TIDLPCLayers.outConsumerCnt[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].outConsumerCnt[0];
          pOrgTIDLNetStructure.TIDLPCLayers[i1].numInBufs = -1;
          pOrgTIDLNetStructure.TIDLPCLayers[i1].numOutBufs = -1;
        }
        else
        {
          //pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType = TIDL_BatchNormLayer;
          //TIDLPCLayers.actParams.actType = TIDL_RelU;
        }
      }
    }
  } while (numClipMerged);

  return 0;
}

int32_t tidl_mergeInitializerDQLayers(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i1, i2, i3, i4;
  int32_t status = 0;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    sTIDL_LayerPC_t &i1Layer = pOrgTIDLNetStructure.TIDLPCLayers[i1];
    if(i1Layer.layerType == TIDL_DequantizeLayer && i1Layer.allowlistingMetaData.numVarInputs == 0)
    {
      //Identify Consumer layer:
      int32_t outDataId = i1Layer.outData[0].dataId;
      int32_t downIdx = tidl_getOutLayer(pOrgTIDLNetStructure, layerIndex,
                                          outDataId);
      sTIDL_LayerPC_t &consumerLayer = pOrgTIDLNetStructure.TIDLPCLayers[downIdx];

      //Discard node:
      i1Layer.numInBufs = -1;
      i1Layer.numOutBufs = -1;

    }
  }
  return 0;
}

int32_t tidl_mergeQDQLayers(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex, bool mergeQuant)
{
  /*This function performs 2x functions:
    1. Remove DQ->Layer->Q Patterns & update the tensor stats
    2. Add dataconvert for the following scenarios:
        a. IN->Q (Input = Float, Quantized output = INT8)
        b. DQ->OUT (INT8 -> Float)*/
  int32_t i1, i2, i3, i4;
  int32_t status = 0;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    sTIDL_LayerPC_t &i1Layer = pOrgTIDLNetStructure.TIDLPCLayers[i1];
    if((i1Layer.layerType == TIDL_DequantizeLayer) && !mergeQuant)
    {
      //Add dataconvert layer here if output & needs it:

      /*Merge Upwards*/
      int32_t idx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex,
                                    i1Layer.inData[0].dataId);
      if (idx == -1 || i1Layer.allowlistingMetaData.numVarInputs == 0)
      {
        printf("Unable to merge Dequantize upwards - DQ without initializer?\n");
        //Remove layer altogether:
        i1Layer.numInBufs = -1;
        i1Layer.numOutBufs = -1;
      }
      sTIDL_LayerPC_t &upLayer = pOrgTIDLNetStructure.TIDLPCLayers[idx];
      /*Copy over pitch and dim values to layer to be replaced:*/
      i1Layer.outData[0].dimValues[TIDL_DIM_BATCH] = upLayer.outData[0].dimValues[TIDL_DIM_BATCH];
      i1Layer.outData[0].dimValues[TIDL_DIM_DIM1] = upLayer.outData[0].dimValues[TIDL_DIM_DIM1];
      i1Layer.outData[0].dimValues[TIDL_DIM_DIM2] = upLayer.outData[0].dimValues[TIDL_DIM_DIM2];
      i1Layer.outData[0].dimValues[TIDL_DIM_NUMCH] = upLayer.outData[0].dimValues[TIDL_DIM_NUMCH];
      i1Layer.outData[0].dimValues[TIDL_DIM_HEIGHT] = upLayer.outData[0].dimValues[TIDL_DIM_HEIGHT];
      i1Layer.outData[0].dimValues[TIDL_DIM_WIDTH] = upLayer.outData[0].dimValues[TIDL_DIM_WIDTH];
      upLayer.outData[0] = i1Layer.outData[0];
      strcpy((char *)upLayer.outDataNames[0],
                (char *)i1Layer.outDataNames[0]);
      i1Layer.outConsumerCnt[0] = 0;
      i1Layer.numInBufs = -1;
      i1Layer.numOutBufs = -1;
    }
    else if(i1Layer.layerType == TIDL_QuantizeLayer && mergeQuant)
    {
      //Add DC / Update tensorScale and tensorZP:
      /*Update tensorMin and tensorMax:*/

      /*Merge Updwards:*/
      int32_t idx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex,
                                    i1Layer.inData[0].dataId);
      if (idx == -1)
      {
        printf("Unable to merge Quantize upwards\n");
        return -1;
      }
      sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure.TIDLPCLayers[idx];
      if ((TIDLPCLayers.outConsumerCnt[0] == 1))
      {
        /*Copy over pitch and dim values to layer to be replaced:*/
        i1Layer.outData[0].dimValues[TIDL_DIM_BATCH] = TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH];
        i1Layer.outData[0].dimValues[TIDL_DIM_DIM1] = TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1];
        i1Layer.outData[0].dimValues[TIDL_DIM_DIM2] = TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2];
        i1Layer.outData[0].dimValues[TIDL_DIM_NUMCH] = TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH];
        i1Layer.outData[0].dimValues[TIDL_DIM_HEIGHT] = TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT];
        i1Layer.outData[0].dimValues[TIDL_DIM_WIDTH] = TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH];
        TIDLPCLayers.outData[0] = i1Layer.outData[0];
        strcpy((char *)TIDLPCLayers.outDataNames[0],
                (char *)i1Layer.outDataNames[0]);
        TIDLPCLayers.outConsumerCnt[0] = i1Layer.outConsumerCnt[0];
        i1Layer.numInBufs = -1;
        i1Layer.numOutBufs = -1;
        TIDLPCLayers.actParams.actType = TIDL_Clip;
        //TIDLPCLayers.outData[0].elementType = TIDL_SignedChar; /*QDQ -> Force to signed datatype?*/
        float min, max;
        if(i1Layer.layerPCParams.quantizeParams.elementType == TIDL_SignedChar)
        {
          max = (127.0  - i1Layer.layerPCParams.quantizeParams.zeroPoint) * i1Layer.layerPCParams.quantizeParams.scale;
          min = (-128.0  - i1Layer.layerPCParams.quantizeParams.zeroPoint) * i1Layer.layerPCParams.quantizeParams.scale;
        }
        else
        {
          //Unsigned 8-bit:
          max = (255.0  - i1Layer.layerPCParams.quantizeParams.zeroPoint) * i1Layer.layerPCParams.quantizeParams.scale;
          min = (0.0  - i1Layer.layerPCParams.quantizeParams.zeroPoint) * i1Layer.layerPCParams.quantizeParams.scale;
        }
        /*Update scale, zero point, min & max*/
        TIDLPCLayers.outData[0].maxValue = max;
        TIDLPCLayers.outData[0].minValue = min;
        TIDLPCLayers.outData[0].maxTensorValue = max;
        TIDLPCLayers.outData[0].minTensorValue = min;
        TIDLPCLayers.actParams.clipMax = max;
        TIDLPCLayers.actParams.clipMin = min;
      }
    }
  }
  return 0;
}

int32_t tidl_mergeIdentitytLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i1, i2, i3, i4;
  int32_t status = 0;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    sTIDL_LayerPC_t &i1Layer = pOrgTIDLNetStructure.TIDLPCLayers[i1];
    if ((i1Layer.layerType == TIDL_DropOutLayer) ||
        (i1Layer.layerType == TIDL_DequantizeLayer) ||
        (i1Layer.layerType == TIDL_QuantizeLayer)  ||
        (i1Layer.layerType == TIDL_CastLayer) ||
        (i1Layer.layerType == TIDL_IdentityLayer))
    {
      int32_t idx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex,
                                    i1Layer.inData[0].dataId);
      if (idx == -1)
      {
        return -1;
      }
      sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure.TIDLPCLayers[idx];
      if ((TIDLPCLayers.outConsumerCnt[0] == 1))
      {
        TIDLPCLayers.numMacs += i1Layer.numMacs;
        TIDLPCLayers.outData[0] = i1Layer.outData[0];
        strcpy((char *)TIDLPCLayers.outDataNames[0],
               (char *)i1Layer.outDataNames[0]);
        TIDLPCLayers.outConsumerCnt[0] = i1Layer.outConsumerCnt[0];
        i1Layer.numInBufs = -1;
        i1Layer.numOutBufs = -1;

        // TVM/Relay import maps "cast" op (to uint8) to Identity Layer
        // with TIDL_Clip ActType, which is intended for previous layer
        if (TIDLPCLayers.actParams.actType == TIDL_NoAct)
          TIDLPCLayers.actParams = i1Layer.actParams;
      }
      else if (i1Layer.outConsumerCnt[0] == 1)
      {
        // if cannot merge upwards, then merge downwards
        int32_t outDataId = i1Layer.outData[0].dataId;
        int32_t downIdx = tidl_getOutLayer(pOrgTIDLNetStructure, layerIndex,
                                          outDataId);
        if (downIdx == -1)
        {
          return -1;
        }
        sTIDL_LayerPC_t &downLayer = pOrgTIDLNetStructure.TIDLPCLayers[downIdx];
        for (i2 = 0; i2 < downLayer.numInBufs; i2++)
        {
          if (downLayer.inData[i2].dataId == outDataId)
          {
            downLayer.inData[i2] = i1Layer.inData[0];
            strcpy((char *)downLayer.inDataNames[i2],
                   (char *)i1Layer.inDataNames[0]);
          }
        }
        i1Layer.outConsumerCnt[0] = 0;
        i1Layer.numInBufs = -1;
        i1Layer.numOutBufs = -1;
      }
    }
  }

  return 0;
}


int32_t tidl_merge1x1MaxPoolingLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i1, i2, i3, i4;
  int32_t status = 0;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_PoolingLayer &&
        pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.poolParams.poolingType == TIDL_MaxPooling &&
        pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.poolParams.kernelH == 1 &&
        pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.poolParams.kernelW == 1 &&
        pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.poolParams.strideH == 1 &&
        pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.poolParams.strideW == 1)
    {
      int32_t  idx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0].dataId);
      if (idx == -1)
      {
        return -1;
      }
      sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure.TIDLPCLayers[idx];
      if ((TIDLPCLayers.outConsumerCnt[0] == 1))
      {
        TIDLPCLayers.numMacs += pOrgTIDLNetStructure.TIDLPCLayers[i1].numMacs;
        TIDLPCLayers.outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0];
        strcpy((char *)TIDLPCLayers.outDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i1].outDataNames[0]);
        TIDLPCLayers.outConsumerCnt[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].outConsumerCnt[0];
        pOrgTIDLNetStructure.TIDLPCLayers[i1].numInBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[i1].numOutBufs = -1;
      }
    }
  }

  return 0;
}

int32_t tidl_merge1x1ResizeLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i1, i2, i3, i4;
  int32_t status = 0;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_ResizeLayer &&
        pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.resizeParams.resizeRatio[TIDL_DIM_WIDTH] == 1 &&
        pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.resizeParams.resizeRatio[TIDL_DIM_HEIGHT] == 1)
    {
      int32_t  idx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0].dataId);
      if (idx == -1)
      {
        return -1;
      }
      sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure.TIDLPCLayers[idx];
      int32_t  outIdx = tidl_getOutLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].dataId);
      if (outIdx == -1)
      {
        return -1;
      }
      sTIDL_LayerPC_t &TIDLPCLayersOut = pOrgTIDLNetStructure.TIDLPCLayers[outIdx];
      if((TIDLPCLayers.layerType != TIDL_DataLayer) || (TIDLPCLayersOut.layerType != TIDL_DataLayer))  /* Do not remove resize layer if it is the only layer in the network */
      {
        if ((TIDLPCLayers.outConsumerCnt[0] == 1))
        {
          TIDLPCLayers.numMacs += pOrgTIDLNetStructure.TIDLPCLayers[i1].numMacs;
          TIDLPCLayers.outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0];
          strcpy((char *)TIDLPCLayers.outDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i1].outDataNames[0]);
          TIDLPCLayers.outConsumerCnt[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].outConsumerCnt[0];
          pOrgTIDLNetStructure.TIDLPCLayers[i1].numInBufs = -1;
          pOrgTIDLNetStructure.TIDLPCLayers[i1].numOutBufs = -1;
        }
      }
    }
  }

  return 0;
}

int32_t tidl_mergeDetectionoutLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i1, i2, i3, i4;
  int32_t status = 0;
  int32_t SINGLE_INPUT = 0;
  int32_t SINGLE_OUTPUT = 0;

  // Remove Permute First
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_PermuteLayer)
    {
      int32_t  idx_permute = i1;
      sTIDL_LayerPC_t& permute = pOrgTIDLNetStructure.TIDLPCLayers[idx_permute];

      // previous -> Permute
      int32_t  idx_previous = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, permute.inData[SINGLE_INPUT].dataId);
      if (idx_previous == -1)
      {
        printf("ERROR: tidl_mergeDetectionoutLayer PermuteLayer find prev layer failed!\n");
        return -1;
      }
      sTIDL_LayerPC_t &previous = pOrgTIDLNetStructure.TIDLPCLayers[idx_previous];

      // Permute -> Flatten
      int32_t  idx_flatten = tidl_getOutLayer(pOrgTIDLNetStructure, layerIndex, permute.outData[SINGLE_INPUT].dataId);
      if (idx_flatten == -1 || pOrgTIDLNetStructure.TIDLPCLayers[idx_flatten].layerType != TIDL_FlattenLayer)
      {
        printf("ERROR: tidl_mergeDetectionoutLayer PermuteLayer find next layer failed! or Permute + Flatten combo not found!\n");
        return -1;
      }
      sTIDL_LayerPC_t &flatten = pOrgTIDLNetStructure.TIDLPCLayers[idx_flatten];

      // Flatten -> Concat
      int32_t  idx_concat = tidl_getOutLayer(pOrgTIDLNetStructure, layerIndex, flatten.outData[SINGLE_OUTPUT].dataId);
      if (idx_concat == -1 || pOrgTIDLNetStructure.TIDLPCLayers[idx_concat].layerType != TIDL_ConcatLayer)
      {
        printf("ERROR: tidl_mergeDetectionoutLayer Flatten find next layer failed! or Permute + Flatten + Concat combo not found!\n");
        return -1;
      }
      sTIDL_LayerPC_t &concat = pOrgTIDLNetStructure.TIDLPCLayers[idx_concat];

      previous.outData[SINGLE_OUTPUT] = permute.outData[SINGLE_OUTPUT];
      strcpy((char *)previous.outDataNames[0], (char *)permute.outDataNames[0]);
      previous.outConsumerCnt[0] = permute.outConsumerCnt[0];
      permute.numInBufs = -1;
      permute.numOutBufs = -1;
    }
  }

  // Remove Softmax/Reshape
  for(i1 = 0; i1 < layerIndex; i1++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_SoftMaxLayer)
    {
      int32_t idx_softmax = i1;
      sTIDL_LayerPC_t &softmax = pOrgTIDLNetStructure.TIDLPCLayers[idx_softmax];
      int32_t  idx_detectionOut = tidl_getOutLayer(pOrgTIDLNetStructure, layerIndex, softmax.outData[SINGLE_OUTPUT].dataId);
      if (idx_detectionOut == -1)
      {
        printf("ERROR: tidl_mergeDetectionoutLayer Softmax find next layer failed!\n");
        return -1;
      }
      if(pOrgTIDLNetStructure.TIDLPCLayers[idx_detectionOut].layerType != TIDL_DetectionOutputLayer)
      {
        // Check next softmax.
        continue;
      }

      sTIDL_LayerPC_t &detectionOut = pOrgTIDLNetStructure.TIDLPCLayers[idx_detectionOut];
      int32_t  idx_reshape = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, softmax.inData[SINGLE_INPUT].dataId);
      if (idx_reshape == -1 || pOrgTIDLNetStructure.TIDLPCLayers[idx_reshape].layerType != TIDL_ReshapeLayer)
      {
        printf("ERROR: tidl_mergeDetectionoutLayer Softmax find prev layer failed! or Reshape + Softmax + DetectionOut combo not found!\n");
        return -1;
      }

      sTIDL_LayerPC_t &reshape = pOrgTIDLNetStructure.TIDLPCLayers[idx_reshape];
      int32_t  idx_concat = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, reshape.inData[SINGLE_INPUT].dataId);
      if (idx_concat == -1 || pOrgTIDLNetStructure.TIDLPCLayers[idx_concat].layerType != TIDL_ConcatLayer)
      {
        printf("ERROR: tidl_mergeDetectionoutLayer Reshape find prev layer failed! or Concat + Reshape + Softmax + DetectionOut combo not found!\n");
        return -1;
      }

      sTIDL_LayerPC_t &concat = pOrgTIDLNetStructure.TIDLPCLayers[idx_concat];
      reshape.numInBufs = -1;
      reshape.numOutBufs = -1;
      softmax.numInBufs = -1;
      softmax.numOutBufs = -1;

      concat.outData[SINGLE_OUTPUT].dataId = softmax.outData[SINGLE_OUTPUT].dataId;
      strcpy((char*)concat.outDataNames[SINGLE_OUTPUT], (char*)softmax.outDataNames[SINGLE_OUTPUT]);

    }
  }

  int32_t numDetectionLayer = 0;
  int32_t numPriorBoxLayerForDeletion = 0;
  int32_t numConcatLayerForDeletion = 0;
  int32_t detectionLayerIdx[MAX_NUM_DETECTION_LAYER];
  /*Maximum number of prior box layers accounting maximum MAX_NUM_DETECTION_LAYER number of detection layers*/
  int32_t priorBoxLayerIds[TIDL_NUM_IN_BUFS*MAX_NUM_DETECTION_LAYER];
  int32_t concatLayerIds[TIDL_NUM_IN_BUFS*MAX_NUM_DETECTION_LAYER];

  for (i1 = 0; i1 < layerIndex; i1++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_DetectionOutputLayer)
    {
      detectionLayerIdx[numDetectionLayer] = i1;
      numDetectionLayer++;
    }
  }

  // Remove PriorBox Layer
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_ConcatLayer)
    {
      int32_t idx_concat = i1;
      sTIDL_LayerPC_t &concat = pOrgTIDLNetStructure.TIDLPCLayers[idx_concat];

      for(int32_t detCnt = 0; detCnt <  numDetectionLayer; detCnt++)
      {
        int32_t idx_detectionOut = detectionLayerIdx[detCnt];
        int32_t ipCnt;
        for(ipCnt = 0; ipCnt < pOrgTIDLNetStructure.TIDLPCLayers[idx_detectionOut].numInBufs; ipCnt++)
        {
          if(pOrgTIDLNetStructure.TIDLPCLayers[idx_detectionOut].inData[ipCnt].dataId == concat.outData[SINGLE_OUTPUT].dataId)
          {
            break;
          }
        }

        if(ipCnt == pOrgTIDLNetStructure.TIDLPCLayers[idx_detectionOut].numInBufs) continue;

        // check the priorbox
        int32_t  idx_priorBox = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, concat.inData[0].dataId);
        if (idx_priorBox == -1)
        {
          printf("ERROR: tidl_mergeDetectionoutLayer Concat find prev layer failed!\n");
          return -1;
        }
        if(pOrgTIDLNetStructure.TIDLPCLayers[idx_priorBox].layerType != TIDL_PriorBoxLayer)
        {
          continue;
        }


        // Priorbox Path confirmed
        sTIDL_LayerPC_t &detectionOut = pOrgTIDLNetStructure.TIDLPCLayers[idx_detectionOut];
        detectionOut.numInBufs = 2;

        /* calculate the size of all the priorboxes for concatination */
        int32_t total_priorBoxSize = 1; // To store number of heads

        // parse each priorbox
        for(i2 = 0; i2 < concat.numInBufs; i2++)
        {
          int32_t  idx_priorBox = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, concat.inData[i2].dataId);
          if (idx_priorBox == -1 || pOrgTIDLNetStructure.TIDLPCLayers[idx_priorBox].layerType != TIDL_PriorBoxLayer)
          {
            printf("ERROR: tidl_mergeDetectionoutLayer Concat find prev layer failed! or Priorbox + Concat + DetectionOut combo not found!\n");
            return -1;
          }
          sTIDL_LayerPC_t &priorbox = pOrgTIDLNetStructure.TIDLPCLayers[idx_priorBox];

          total_priorBoxSize += priorbox.layerParams.detectOutParams.priorBoxSize;
        }

        sTIDL_AnchorBoxParams_t* anchorBox;
        anchorBox = (sTIDL_AnchorBoxParams_t*)my_malloc(concat.numInBufs*sizeof(sTIDL_AnchorBoxParams_t));
        memset(anchorBox, 0, concat.numInBufs*sizeof(sTIDL_AnchorBoxParams_t));

        detectionOut.layerParams.detectOutParams.numHeads = concat.numInBufs;
        detectionOut.layerParams.detectOutParams.numSubHeads = 1;

        // parse each priorbox
        for(i2 = 0; i2 < concat.numInBufs; i2++)
        {
          int32_t  idx_priorBox = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, concat.inData[i2].dataId);
          if (idx_priorBox == -1 || pOrgTIDLNetStructure.TIDLPCLayers[idx_priorBox].layerType != TIDL_PriorBoxLayer)
          {
            printf("ERROR: tidl_mergeDetectionoutLayer Concat find prev layer failed! or Priorbox + Concat + DetectionOut combo not found!\n");
            return -1;
          }
          sTIDL_LayerPC_t &priorbox = pOrgTIDLNetStructure.TIDLPCLayers[idx_priorBox];
          priorBoxLayerIds[numPriorBoxLayerForDeletion] = idx_priorBox;
          numPriorBoxLayerForDeletion++;
          /*
          * Parsing PriorBox Layer params before remove this layer
          *
          */
          int32_t img_height = priorbox.inData[1].dimValues[TIDL_DIM_HEIGHT];
          int32_t img_width  = priorbox.inData[1].dimValues[TIDL_DIM_WIDTH];
          int32_t layer_height = priorbox.inData[0].dimValues[TIDL_DIM_HEIGHT];
          int32_t layer_width  = priorbox.inData[0].dimValues[TIDL_DIM_WIDTH];
          float step_h = ((float)1.0f)/layer_height; // step width and height are always in normalized domain
          float step_w = ((float)1.0f)/layer_width; // step width and height are always in normalized domain

          detectionOut.layerParams.detectOutParams.imHeight = img_height;
          detectionOut.layerParams.detectOutParams.imWidth = img_width;

          anchorBox[i2] = *(sTIDL_AnchorBoxParams_t*)priorbox.priorBox.ptr;

          if(anchorBox[i2].headWidth == -1) anchorBox[i2].headWidth = layer_width;
          if(anchorBox[i2].headHeight == -1) anchorBox[i2].headHeight = layer_height;
          if(anchorBox[i2].strideW == -1) anchorBox[i2].strideW = step_w;
          if(anchorBox[i2].strideH == -1) anchorBox[i2].strideH = step_h;
        }

        detectionOut.priorBox.ptr = anchorBox;
        detectionOut.priorBox.bufSize = concat.numInBufs*sizeof(sTIDL_AnchorBoxParams_t)/sizeof(float);

        // Remove Concat Layer
        concatLayerIds[numConcatLayerForDeletion] = i1;
        numConcatLayerForDeletion++;
      }
    }
  }

  for(int32_t i0 = 0; i0 < numPriorBoxLayerForDeletion; i0++)
  {
    int32_t idx_priorBox = priorBoxLayerIds[i0];
    sTIDL_LayerPC_t &priorbox = pOrgTIDLNetStructure.TIDLPCLayers[idx_priorBox];
    if((priorbox.numInBufs != -1) || (priorbox.numInBufs != -1))
    {
      free(priorbox.priorBox.ptr);
      priorbox.priorBox.ptr = NULL;
      priorbox.priorBox.bufSize = 0;

      /*
      * Remove PriorBox Layer & Previous layer output
      *
      */

      for(i4 = 0; i4 < priorbox.numInBufs; i4++)
      {
        int32_t  idx_previous = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, priorbox.inData[i4].dataId);
        if (idx_previous == -1)
        {
          printf("ERROR: tidl_mergeDetectionoutLayer PriorBox find prev layer failed!\n");
          return -1;
        }
        sTIDL_LayerPC_t &previous = pOrgTIDLNetStructure.TIDLPCLayers[idx_previous];
        for(i3 = 0; i3 < previous.numOutBufs; i3++)
        {
          if(previous.outData[i3].dataId == priorbox.inData[i4].dataId)
          {
            break;
          }
        }
        int32_t dataIdx_priorbox = i3;
        // Remove Previous layer's output
        previous.outConsumerCnt[dataIdx_priorbox]--;
        previous.outConsumerLinked[dataIdx_priorbox]--;
      }
      // Remove PriorBox Layer
      priorbox.numInBufs = -1;
      priorbox.numOutBufs = -1;
    }
  }

  for(int32_t i0 = 0; i0 < numConcatLayerForDeletion; i0++)
  {
    int32_t idx_concat = concatLayerIds[i0];
    sTIDL_LayerPC_t &concat = pOrgTIDLNetStructure.TIDLPCLayers[idx_concat];
    concat.numInBufs = -1;
    concat.numOutBufs = -1;
  }

  return 0;
}
int32_t tidl_removeConcatFlattenLayerInDetNet(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i1, i2, i3, i4;
  int32_t status = 0;
  int32_t SINGLE_INPUT = 0;
  int32_t SINGLE_OUTPUT = 0;
  sTIDL_DataParams_t inDataBkup[TIDL_NUM_IN_BUFS][TIDL_NUM_IN_BUFS];
  int8_t  inDataNamesBkup[TIDL_NUM_IN_BUFS][TIDL_STRING_SIZE];

  int32_t numFlattenLayerForDeletion = 0;
  int32_t numConcatLayerForDeletion = 0;
  int32_t flattenLayerIds[TIDL_NUM_IN_BUFS*MAX_NUM_DETECTION_LAYER];
  int32_t concatLayerIds[TIDL_NUM_IN_BUFS*MAX_NUM_DETECTION_LAYER];

  /*Remove flatten and concat layer just before detection layer*/
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_DetectionOutputLayer)
    {
      sTIDL_LayerPC_t &det_layer = pOrgTIDLNetStructure.TIDLPCLayers[i1];
      int32_t det_layer_prev_num_ips = det_layer.numInBufs;
      int32_t i3, i4;
      i4 = 0;
      for(i2 = 0; i2 < det_layer_prev_num_ips; i2++)
      {
        // check the detection layer's all inputs
        int32_t  idx_concat = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, det_layer.inData[i2].dataId);

        if (idx_concat == -1)
        {
          printf("ERROR: Concat -> Detection Layer finding failed!\n");
          return -1;
        }

        if (pOrgTIDLNetStructure.TIDLPCLayers[idx_concat].layerType != TIDL_ConcatLayer)
        {
          continue; // Check for next input (next i2) of detection layer
        }

        /*Concat --> Detection Layer is found*/
        sTIDL_LayerPC_t &concat_layer = pOrgTIDLNetStructure.TIDLPCLayers[idx_concat];

        for (i3 = 0; i3 < concat_layer.numInBufs; i3++)
        {

          int32_t  idx_flatten = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, concat_layer.inData[i3].dataId);

          if (idx_flatten == -1)
          {
            printf("ERROR: Flatten -> Concat Layer finding failed!\n");
            return -1;
          }

          if (pOrgTIDLNetStructure.TIDLPCLayers[idx_flatten].layerType != TIDL_FlattenLayer)
          {
            continue; // Check for next input (next i3) concat layer
          }

          sTIDL_LayerPC_t &flatten_layer = pOrgTIDLNetStructure.TIDLPCLayers[idx_flatten];

          int32_t  idx_conv = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, flatten_layer.inData[SINGLE_INPUT].dataId);

          if (idx_conv == -1)
          {
            printf("ERROR: Flatten -> Concat Layer finding failed!\n");
            return -1;
          }

          sTIDL_LayerPC_t &conv_layer = pOrgTIDLNetStructure.TIDLPCLayers[idx_conv];

          if(i4 < TIDL_NUM_IN_BUFS)
          {
            memcpy((int32_t*)(&inDataBkup[i4][0]), (int32_t*)&conv_layer.outData[SINGLE_OUTPUT], sizeof(sTIDL_DataParams_t));
            strcpy((char*)inDataNamesBkup[i4], (char*)conv_layer.outDataNames[SINGLE_OUTPUT]);
          }
          i4++;
          flattenLayerIds[numFlattenLayerForDeletion] = idx_flatten;
          numFlattenLayerForDeletion++;
        }
        concatLayerIds[numConcatLayerForDeletion] = idx_concat;
        numConcatLayerForDeletion++;
      }

      if((i4 < TIDL_NUM_IN_BUFS) && (i4 !=0))
      {
        det_layer.numInBufs = i4;

        for(i3 = 0; i3 < i4; i3++)
        {
          strcpy((char*)det_layer.inDataNames[i3], (char*)inDataNamesBkup[i3]);
          memcpy((int32_t*)&det_layer.inData[i3], (int32_t*)&inDataBkup[i3][0], sizeof(sTIDL_DataParams_t));
        }
      }
    }
  }

  for(int32_t i = 0; i < numFlattenLayerForDeletion; i++)
  {
    sTIDL_LayerPC_t &flatten_layer = pOrgTIDLNetStructure.TIDLPCLayers[flattenLayerIds[i]];
    flatten_layer.numInBufs = -1;
    flatten_layer.numOutBufs = -1;
  }

  for(int32_t i = 0; i < numConcatLayerForDeletion; i++)
  {
    sTIDL_LayerPC_t &concat_layer = pOrgTIDLNetStructure.TIDLPCLayers[concatLayerIds[i]];
    concat_layer.numInBufs = -1;
    concat_layer.numOutBufs = -1;
  }

  return 0;
}

int32_t tidl_removeDivideByOneLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex)
{
  for(int i1 = 0; i1 < layerIndex; i1++)
  {
    if(
        (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_BatchNormLayer) &&
        (pOrgTIDLNetStructure.TIDLPCLayers[i1].actParams.actType != TIDL_GELU)  &&
        (pOrgTIDLNetStructure.TIDLPCLayers[i1].actParams.actType != TIDL_Tanh) &&
        (pOrgTIDLNetStructure.TIDLPCLayers[i1].actParams.actType != TIDL_ELU) &&
        (pOrgTIDLNetStructure.TIDLPCLayers[i1].actParams.actType != TIDL_HardSigmoid)
      )
    {
      sTIDL_LayerPC_t &layerBN = pOrgTIDLNetStructure.TIDLPCLayers[i1];
      float32_tidl * weightPtr = (float *)layerBN.weights.ptr;
      int32_t weightSize = layerBN.weights.bufSize;
      float32_tidl * biasPtr = (float *)layerBN.bias.ptr;
      int32_t biasSize = layerBN.bias.bufSize;
      bool divRemove = true;

      for(int i2 = 0; i2 < weightSize; i2++)
      {
        if(weightPtr[i2] != 1)
        {
          divRemove = false;
          break;
        }
      }
      if(divRemove)
      {
        for(int i3 = 0; i3 < biasSize; i3++)
        {
          if(biasPtr[i3] != 0)
          {
            divRemove = false;
            break;
          }
        }
      }
      if(divRemove)
      {
        int32_t inIdx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, layerBN.inData[0].dataId);
        sTIDL_LayerPC_t &inLayer = pOrgTIDLNetStructure.TIDLPCLayers[inIdx];
        int32_t outIdx = tidl_getOutLayer(pOrgTIDLNetStructure, layerIndex, layerBN.outData[0].dataId);
        sTIDL_LayerPC_t &outLayer = pOrgTIDLNetStructure.TIDLPCLayers[outIdx];
        outLayer.inData[0] = layerBN.inData[0];
        strcpy((char *)outLayer.inDataNames[0], (char *)layerBN.inDataNames[0]);
        layerBN.numInBufs = -1;
        layerBN.numOutBufs = -1;
      }
    }
  }
  return 0;
}

int32_t tidl_AddTfODOutputLayers(sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t layerIndex, int32_t * dataIndex, int32_t metaArchType)
{
  bool isObjectDetectionNetwork = false;
  int32_t numDetLayer = 0;
  int32_t detLayerIdx[MAX_NUM_DETECTION_LAYER];
  int32_t currNumOutputDataLayers;

  for (int i1 = 0; i1 < layerIndex; i1++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_DetectionOutputLayer)
    {
      isObjectDetectionNetwork = true;
      detLayerIdx[numDetLayer] = i1;
      numDetLayer++;
    }
  }
  if(isObjectDetectionNetwork)
  {
    if(metaArchType == TIDL_metaArchTFSSD || metaArchType == TIDL_metaArchTfliteRtSsdDetectionLayer)
    {
      currNumOutputDataLayers = 0;
      for(int i4 = 0; i4 < numDetLayer; i4++)
      {
        for(int i1 = 0; i1 < 4; i1++)
        {
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType  = TIDL_DataLayer;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs  = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = -1;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dataId = 0;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = 0;
          strcpy((char*)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].name, (char *)tidl_OdOutputNames[currNumOutputDataLayers + i1]);
          strcpy((char*)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0], (char*)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].name);
          strcpy((char*)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char*)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].name);
          tidl_linkOutputTensors(&pOrgTIDLNetStructure, layerIndex);
          layerIndex++;
        }
        pOrgTIDLNetStructure.numLayers = layerIndex;

        int32_t i1  = detLayerIdx[i4];
        sTIDL_LayerPC_t &det_layer = pOrgTIDLNetStructure.TIDLPCLayers[i1];
        sTIDL_DetectOutputParams_t * detOutParams = &det_layer.layerParams.detectOutParams;
        for(int i2 = 0; i2 < 4; i2++)
        {
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType  = TIDL_OdOutputReformatLayer;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs  = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weightsElementSizeInBits = NUM_WHGT_BITS;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = 0;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].actParams.actType = TIDL_NoAct;

          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0] = det_layer.outData[0];
          strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0], (char *)det_layer.outDataNames[0]);
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0];
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dataId = (*dataIndex)++;
          strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char *)tidl_OdOutputNames[currNumOutputDataLayers + i2]);
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.odOutputReformatLayerParams.inWidthOdNetwork = tidl_OdPostProc[0].imWidth;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.odOutputReformatLayerParams.inHeightOdNetwork = tidl_OdPostProc[0].imHeight;
          if(i2 == 0)  // detection_boxes
          {
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.odOutputReformatLayerParams.layerType = TIDL_DetectionBoxesTflite;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_BATCH] = 1;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] = detOutParams->keepTopK; // confirm if keepTopK or topK
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] = 4;
          }
          else if(i2 == 1)  //detection_classes
          {
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.odOutputReformatLayerParams.layerType = TIDL_DetectionClassesTflite;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_BATCH] = 1;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] = 1;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] = detOutParams->keepTopK;
          }
          else if (i2 == 2)  //detection_scores
          {
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.odOutputReformatLayerParams.layerType = TIDL_DetectionScoresTflite;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_BATCH] = 1;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] = 1;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] = detOutParams->keepTopK;
          }
          else if(i2 == 3)  //num_detections
          {
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.odOutputReformatLayerParams.layerType = TIDL_NumDetectionsTflite;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_BATCH] = 1;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] = 1;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] = 1;
          }
          tidl_linkOutputTensors(&pOrgTIDLNetStructure, layerIndex);
          layerIndex++;
        }
        currNumOutputDataLayers += 4;
      }
      pOrgTIDLNetStructure.numLayers = layerIndex;
      return 0;
    }
    else if(metaArchType == TIDL_metaArchTIDLRetinaNet )
    {
      currNumOutputDataLayers = 0;
      for(int i4 = 0; i4 < numDetLayer; i4++)
      {
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType  = TIDL_DataLayer;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs  = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dataId = 0;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = 0;
        strcpy((char*)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].name, (char *)tidl_OdOutputNames[currNumOutputDataLayers + i4]);
        strcpy((char*)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0], (char*)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].name);
        strcpy((char*)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char*)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].name);
        tidl_linkOutputTensors(&pOrgTIDLNetStructure, layerIndex);
        layerIndex++;

        pOrgTIDLNetStructure.numLayers = layerIndex;
        int32_t i1  = detLayerIdx[i4];
        sTIDL_LayerPC_t &det_layer = pOrgTIDLNetStructure.TIDLPCLayers[i1];
        sTIDL_DetectOutputParams_t * detOutParams = &det_layer.layerParams.detectOutParams;
        for(int i2 = 0; i2 < 1; i2++)
        {
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType  = TIDL_OdOutputReformatLayer;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs  = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weightsElementSizeInBits = NUM_WHGT_BITS;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = 0;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].actParams.actType = TIDL_NoAct;

          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0] = det_layer.outData[0];
          strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0], (char *)det_layer.outDataNames[0]);
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0];
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dataId = (*dataIndex)++;
          strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char *)tidl_OdOutputNames[currNumOutputDataLayers + i2]);
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.odOutputReformatLayerParams.inWidthOdNetwork = tidl_OdPostProc[0].imWidth;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.odOutputReformatLayerParams.inHeightOdNetwork = tidl_OdPostProc[0].imHeight;
          if(i2 == 0)  // detection_boxes + scores + labels
          {
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.odOutputReformatLayerParams.layerType = TIDL_DetectionBoxesScoresLabelsTfliteEfficientdet;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_BATCH] = 1;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] = detOutParams->keepTopK; // confirm if keepTopK or topK
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] = 7;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].elementType = TIDL_SinglePrecFloat;
          }
          tidl_linkOutputTensors(&pOrgTIDLNetStructure, layerIndex);
          layerIndex++;
        }
        currNumOutputDataLayers += 1;
      }
      pOrgTIDLNetStructure.numLayers = layerIndex;
      return 0;
    }
  }
  return 0;
}


int32_t tidl_AddOnnxODOutputLayers(sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t layerIndex, int32_t * dataIndex)
{
  int32_t numDetLayer=0;
  bool isObjectDetectionNetwork = false;
  int32_t detLayerIdx[MAX_NUM_DETECTION_LAYER];

  for (int i1 = 0; i1 < layerIndex; i1++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_DetectionOutputLayer)
    {
      isObjectDetectionNetwork = true;
      detLayerIdx[numDetLayer] = i1;
      numDetLayer++;
    }
  }

  if(isObjectDetectionNetwork)
  {
    int32_t currNumOutputDataLayers = 0;
    for(int i0 = 0; i0 < numDetLayer; i0++)
    {
      if(tidl_OdFramework.compare("MMDetection") == 0)
      {
        // if framework is MMDetection
        for(int i1 = 0; i1 < 2; i1++)
        {
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType  = TIDL_DataLayer;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs  = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = -1;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dataId = 0;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = 0;
          strcpy((char*)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].name, (char *)tidl_OdOutputNames[currNumOutputDataLayers + i1]);
          strcpy((char*)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0], (char*)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].name);
          strcpy((char*)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char*)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].name);
          tidl_linkOutputTensors(&pOrgTIDLNetStructure, layerIndex);
          layerIndex++;
        }
        pOrgTIDLNetStructure.numLayers = layerIndex;

        int i1  = detLayerIdx[i0];
        sTIDL_LayerPC_t &det_layer = pOrgTIDLNetStructure.TIDLPCLayers[i1];
        sTIDL_DetectOutputParams_t * detOutParams = &det_layer.layerParams.detectOutParams;
        for(int i2 = 0; i2 < 2; i2++)
        {
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType  = TIDL_OdOutputReformatLayer;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs  = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weightsElementSizeInBits = NUM_WHGT_BITS;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = 0;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].actParams.actType = TIDL_NoAct;

          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0] = det_layer.outData[0];
          strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0], (char *)det_layer.outDataNames[0]);
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0];
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dataId = (*dataIndex)++;
          strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char *)tidl_OdOutputNames[currNumOutputDataLayers + i2]);
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.odOutputReformatLayerParams.inWidthOdNetwork = tidl_OdPostProc[0].imWidth;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.odOutputReformatLayerParams.inHeightOdNetwork = tidl_OdPostProc[0].imHeight;
          if(i2 == 0)  // detection_boxes + scores
          {
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.odOutputReformatLayerParams.layerType = TIDL_DetectionBoxesAndScoresMmdetection;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_BATCH] = 1;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] = detOutParams->keepTopK; // confirm if keepTopK or topK
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] = 5;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].elementType = TIDL_SinglePrecFloat;
          }
          else if(i2 == 1)  //detection_classes
          {
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.odOutputReformatLayerParams.layerType = TIDL_DetectionClassesMmdetection;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_BATCH] = 1;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] = 1;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] = detOutParams->keepTopK;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].elementType = TIDL_SinglePrecFloat;
          }
          tidl_linkOutputTensors(&pOrgTIDLNetStructure, layerIndex);
          layerIndex++;
        }
        currNumOutputDataLayers += 2;
      }
      else if((gParams.metaArchType == TIDL_metaArchTIDLSSD))
      {
        for(int i1 = 0; i1 < 3; i1++)
        {
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType  = TIDL_DataLayer;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs  = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = -1;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dataId = 0;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = 0;
          strcpy((char*)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].name, (char *)tidl_OdOutputNames[currNumOutputDataLayers + i1]);
          strcpy((char*)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0], (char*)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].name);
          strcpy((char*)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char*)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].name);
          tidl_linkOutputTensors(&pOrgTIDLNetStructure, layerIndex);
          layerIndex++;
        }
        pOrgTIDLNetStructure.numLayers = layerIndex;

        int32_t i1  = detLayerIdx[i0];
        sTIDL_LayerPC_t &det_layer = pOrgTIDLNetStructure.TIDLPCLayers[i1];
        sTIDL_DetectOutputParams_t * detOutParams = &det_layer.layerParams.detectOutParams;
        for(int i2 = 0; i2 < 3; i2++)
        {
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType  = TIDL_OdOutputReformatLayer;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs  = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weightsElementSizeInBits = NUM_WHGT_BITS;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = 0;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].actParams.actType = TIDL_NoAct;

          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0] = det_layer.outData[0];
          strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0], (char *)det_layer.outDataNames[0]);
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0];
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dataId = (*dataIndex)++;
          strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char *)tidl_OdOutputNames[currNumOutputDataLayers + i2]);
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.odOutputReformatLayerParams.inWidthOdNetwork = tidl_OdPostProc[0].imWidth;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.odOutputReformatLayerParams.inHeightOdNetwork = tidl_OdPostProc[0].imHeight;
          if(i2 == 0)  // detection_boxes
          {
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.odOutputReformatLayerParams.layerType = TIDL_DetectionBoxesSsd;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_BATCH] = 1;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] = detOutParams->keepTopK; // confirm if keepTopK or topK
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] = 4;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].elementType = TIDL_SinglePrecFloat; //TODO: this will differ for different OD networks
          }
          else if(i2 == 1)  //detection_classes
          {
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.odOutputReformatLayerParams.layerType = TIDL_DetectionClassesSsd;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_BATCH] = 1;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] = 1;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] = detOutParams->keepTopK;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].elementType = TIDL_SinglePrecFloat;  //Should this be int64?
          }
          else if (i2 == 2)  //detection_scores
          {
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.odOutputReformatLayerParams.layerType = TIDL_DetectionScoresSsd;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_BATCH] = 1;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] = 1;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] = detOutParams->keepTopK;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].elementType = TIDL_SinglePrecFloat;
          }
          tidl_linkOutputTensors(&pOrgTIDLNetStructure, layerIndex);
          layerIndex++;
        }
        currNumOutputDataLayers += 3;
      }
      else if((gParams.metaArchType == TIDL_metaArchTIDLYoloV5) ||
              (gParams.metaArchType == TIDL_metaArchTIDLYoloV8) ||
              (gParams.metaArchType == TIDL_metaArchTIDLCenterPoint))
      {
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType  = TIDL_DataLayer;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs  = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dataId = 0;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = 0;
        strcpy((char*)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].name, (char *)tidl_OdOutputNames[currNumOutputDataLayers]);
        strcpy((char*)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0], (char*)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].name);
        strcpy((char*)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char*)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].name);
        tidl_linkOutputTensors(&pOrgTIDLNetStructure, layerIndex);
        layerIndex++;

        pOrgTIDLNetStructure.numLayers = layerIndex;

        int32_t i1  = detLayerIdx[i0];
        sTIDL_LayerPC_t &det_layer = pOrgTIDLNetStructure.TIDLPCLayers[i1];
        sTIDL_DetectOutputParams_t * detOutParams = &det_layer.layerParams.detectOutParams;
        for(int i2 = 0; i2 < 1; i2++)
        {
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType  = TIDL_OdOutputReformatLayer;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs  = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weightsElementSizeInBits = NUM_WHGT_BITS;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = 0;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].actParams.actType = TIDL_NoAct;

          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0] = det_layer.outData[0];
          strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0], (char *)det_layer.outDataNames[0]);
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0];
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dataId = (*dataIndex)++;
          strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char *)tidl_OdOutputNames[currNumOutputDataLayers + i2]);
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.odOutputReformatLayerParams.inWidthOdNetwork = tidl_OdPostProc[0].imWidth;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.odOutputReformatLayerParams.inHeightOdNetwork = tidl_OdPostProc[0].imHeight;
          if(i2 == 0)  // detection_boxes + scores + classes
          {
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.odOutputReformatLayerParams.layerType = TIDL_DetectionBoxesScoresLabelsYoloV5;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_BATCH] = 1;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] = detOutParams->keepTopK;
            //Each ketupoint is defined by two or three parameters based on keypointConfidence
            //6D pose of an object is defined by 9 parameters. (Six for rotation and three for translation)
            if((gParams.metaArchType == TIDL_metaArchTIDLYoloV5) ||
               (gParams.metaArchType == TIDL_metaArchTIDLYoloV8)
              )
            {
              pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] = 6 + detOutParams->numKeypoints*(2+detOutParams->keypointConfidence);
            }
            else
            {
              pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] = detOutParams->numKeypoints*(2+detOutParams->keypointConfidence);
            }

            if (detOutParams->subCodeType == TIDL_ObjectPose)
            {
              pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] += sizeof(TIDL_ODLayerObjectPose)/sizeof(float32_tidl);
            }
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.detectOutParams.numKeypoints = detOutParams->numKeypoints;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.detectOutParams.keypointConfidence = detOutParams->keypointConfidence;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.detectOutParams.subCodeType = detOutParams->subCodeType;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].elementType = TIDL_SinglePrecFloat;
          }
          tidl_linkOutputTensors(&pOrgTIDLNetStructure, layerIndex);
          layerIndex++;
        }
        currNumOutputDataLayers += 1;
      }
      else if(gParams.metaArchType == TIDL_metaArchTIDL3DOD)
      {
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType  = TIDL_DataLayer;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs  = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dataId = 0;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = 0;
        strcpy((char*)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].name, (char *)tidl_OdOutputNames[currNumOutputDataLayers]);
        strcpy((char*)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0], (char*)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].name);
        strcpy((char*)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char*)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].name);
        tidl_linkOutputTensors(&pOrgTIDLNetStructure, layerIndex);
        layerIndex++;

        pOrgTIDLNetStructure.numLayers = layerIndex;

        int32_t i1  = detLayerIdx[i0];
        sTIDL_LayerPC_t &det_layer = pOrgTIDLNetStructure.TIDLPCLayers[i1];
        sTIDL_DetectOutputParams_t * detOutParams = &det_layer.layerParams.detectOutParams;
        for(int i2 = 0; i2 < 1; i2++)
        {
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType  = TIDL_OdOutputReformatLayer;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs  = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weightsElementSizeInBits = NUM_WHGT_BITS;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = 0;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].actParams.actType = TIDL_NoAct;

          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0] = det_layer.outData[0];
          strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0], (char *)det_layer.outDataNames[0]);
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0];
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dataId = (*dataIndex)++;
          strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char *)tidl_OdOutputNames[currNumOutputDataLayers + i2]);
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.odOutputReformatLayerParams.inWidthOdNetwork = tidl_OdPostProc[0].imWidth;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.odOutputReformatLayerParams.inHeightOdNetwork = tidl_OdPostProc[0].imHeight;
          if(i2 == 0)  // output
          {
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.odOutputReformatLayerParams.layerType = TIDL_Detection3dOd;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_BATCH] = 1;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] = detOutParams->keepTopK;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] = 9;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].elementType = TIDL_SinglePrecFloat;
          }
          tidl_linkOutputTensors(&pOrgTIDLNetStructure, layerIndex);
          layerIndex++;
        }
        currNumOutputDataLayers += 1;
      }
    }
  }
  pOrgTIDLNetStructure.numLayers = layerIndex;
  return 0;
}

int32_t tidl_mergeSplitLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i1, i2, i3, i4;

  for (i1 = 0; i1 < layerIndex; i1++)
  {
    sTIDL_LayerPC_t &currentLayer = pOrgTIDLNetStructure.TIDLPCLayers[i1];

    if (currentLayer.layerType == TIDL_SplitLayer)
    {
      // Get previous layer index
      int32_t  idx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, currentLayer.inData[0].dataId);
      if (idx == -1)
      {
        ;// Treat this error as debug info, since the second split layer will not get previous layer index.
      }
      sTIDL_LayerPC_t &previousLayer = pOrgTIDLNetStructure.TIDLPCLayers[idx];

      // Match Split input with Previous Layer's output number
      for(i2 = 0; i2 < previousLayer.numOutBufs; i2++)
      {
        if(currentLayer.inData[0].dataId == previousLayer.outData[i2].dataId)
          break;
      }
      previousLayer.outConsumerCnt[i2]--;

      for(i4 = 0; i4 < currentLayer.numOutBufs; i4++)
      {
        // Get next layer index
        int32_t  idx_next = tidl_getOutLayer(pOrgTIDLNetStructure, layerIndex, currentLayer.outData[i4].dataId);
        if (idx_next == -1)
        {
          // Treat this error as debug info, since the second split layer will not get previous layer index.
        }
        sTIDL_LayerPC_t &nextLayer = pOrgTIDLNetStructure.TIDLPCLayers[idx_next];
        for(i3 = 0; i3 < nextLayer.numInBufs; i3++)
        {
          if(currentLayer.outData[i4].dataId == nextLayer.inData[i3].dataId)
            break;
        }

        // Split Layer only have single output, but previous layer has multiple output, need to parse in order
        previousLayer.numMacs += currentLayer.numMacs;
        nextLayer.inData[i3] = previousLayer.outData[i2];
        previousLayer.outConsumerCnt[i2]++;
        strcpy((char*)nextLayer.inDataNames[i3], (char*)previousLayer.outDataNames[i2]);
      }

      currentLayer.numInBufs = -1;
      currentLayer.numOutBufs = -1;


    }
  }

  return 0;
}


int32_t tidl_duplicateSliceLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t& layerIndex)
{
  int32_t i1, i2, i3, i4;
  int32_t status = 0;

  for (i1 = 0; i1 < layerIndex; i1++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_SliceLayer && pOrgTIDLNetStructure.TIDLPCLayers[i1].numOutBufs > 1)
    {
      int32_t idx_slice_origin = i1;
      sTIDL_LayerPC_t &slice_origin = pOrgTIDLNetStructure.TIDLPCLayers[idx_slice_origin];

      int32_t  idx_previous = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, slice_origin.inData[0].dataId);
      if (idx_previous == -1)
      {
        // Treat this error as debug info, since the second split layer will not get previous layer index.
        printf("ERROR: tidl_duplicateSliceLayer slice layer search for previous layer failed!\n");
        return -1;
      }
      sTIDL_LayerPC_t &previous = pOrgTIDLNetStructure.TIDLPCLayers[idx_previous];

      for(i2 = 0; i2 < previous.numOutBufs; i2++)
      {
        if(previous.outData[i2].dataId == slice_origin.inData[0].dataId)
          break;
      }
      previous.outConsumerCnt[i2]--;
      previous.outConsumerCnt[i2] += slice_origin.numOutBufs;

      for(i2 = 0; i2 < slice_origin.numOutBufs; i2++)
      {
        sTIDL_LayerPC_t &slice_new = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex];
        slice_new.inData[0] = slice_origin.inData[0];
        strcpy((char*)slice_new.inDataNames[0], (char*)slice_origin.inDataNames[0]);
        slice_new.layerParams.sliceParams.slicePoints[0] = slice_origin.layerParams.sliceParams.slicePoints[i2];
        slice_new.layerParams.sliceParams.slicePoints[1] = slice_origin.layerParams.sliceParams.slicePoints[i2+1];
        slice_new.layerParams.sliceParams.axis = slice_origin.layerParams.sliceParams.axis;
        slice_new.layerParams.sliceParams.stride = slice_origin.layerParams.sliceParams.stride;
        slice_new.layerType = slice_origin.layerType;
        sprintf((char*)slice_new.name, "%s_TIDL_%d", (char*)slice_origin.name, i2);
        slice_new.numInBufs = 1;
        slice_new.numOutBufs = 1;
        slice_new.outConsumerCnt[0] = slice_origin.outConsumerCnt[i2];
        slice_new.outConsumerLinked[0] = slice_origin.outConsumerLinked[i2];
        slice_new.outData[0] = slice_origin.outData[i2];
        strcpy((char*)slice_new.outDataNames[0], (char*)slice_origin.outDataNames[i2]);
        /* Copy activation paramms from the original layer*/
        slice_new.actParams = slice_origin.actParams;

        layerIndex++;
      }

      slice_origin.numInBufs = -1;
      slice_origin.numOutBufs = -1;
    }
  }

  return 0;
}

int32_t tidl_configureReshapeLayer (sTIDL_LayerPC_t *layer, void *shape)
{
  int32_t elementSize = 0, status = 0;

  if (gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TENSORFLOW || gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TFLITE
      || gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TFLITE_RT)
  {
    elementSize = sizeof(int32_t);
  }
  else
  {
    elementSize = sizeof(int64_t);
  }

  layer->weights.ptr = malloc (elementSize * TIDL_DIM_MAX);
  layer->weights.bufSize = TIDL_DIM_MAX;
  memcpy(layer->weights.ptr, shape, elementSize * TIDL_DIM_MAX);

  return status;
}

int32_t tidl_configureTransposeLayer (sTIDL_LayerPC_t *layer, int32_t *permute)
{
  int32_t status = 0;

  layer->weights.ptr = malloc(sizeof(int32_t) * TIDL_DIM_MAX);
  layer->weights.bufSize = TIDL_DIM_MAX;
  memcpy(layer->weights.ptr, permute, sizeof(int32_t)* TIDL_DIM_MAX);

  return status;
}

int32_t tidl_broadcastWeights (sTIDL_LayerPC_t& constLayer, int32_t broadcastSize)
{
  int32_t status = 0;
  float *src_ptr = NULL, *dst_ptr = NULL;
  int32_t offset = 0;

  if (broadcastSize % constLayer.weights.bufSize == 0)
  {
    int32_t icnt = broadcastSize / constLayer.weights.bufSize;
    dst_ptr = (float*) malloc (sizeof (float) * broadcastSize);
    src_ptr = (float*)constLayer.weights.ptr;

    for (int32_t i = 0; i < constLayer.weights.bufSize; i++)
    {
      for (int32_t j = 0; j < icnt; j++)
      {
        dst_ptr[offset++] = src_ptr[i];
      }
    }
    free (src_ptr);
    constLayer.weights.ptr = dst_ptr;
    constLayer.weights.bufSize = broadcastSize;
  }


  return status;
}
int32_t tidl_createConstDataLayer (sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t* dataIndex, float* data, int32_t dataSize, int32_t layerIndex)
{
  int32_t status = 0;

  sTIDL_LayerPC_t& constLayer = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex];
  constLayer.layerType = TIDL_ConstDataLayer;
  constLayer.numInBufs  = -1;
  constLayer.numOutBufs = 1;
  constLayer.outData[0].dataId = (*dataIndex)++;
  constLayer.outConsumerCnt[0] = 1;
  constLayer.outConsumerLinked[0] = 0;
  pOrgTIDLNetStructure.numLayers = layerIndex + 1;

  float* constPtr = (float*)malloc(dataSize * sizeof (float));

  memcpy (constPtr, data, dataSize * sizeof (float));

  constLayer.weights.ptr = constPtr;
  constLayer.weights.bufSize = dataSize;

  char append[TIDL_APPEND_NAME];
  sprintf(append, "_%d", addedLayers++);
  strcpy((char*)constLayer.outDataNames[0], append);

  constLayer.outData[0].elementType = tidl_getElementType(1);

  /**Free the source pointer as the data is now copied to const pointer*/
  free (data);

  return layerIndex;
}

int32_t tidl_replacePatchEmbeddingBlock(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t* dataIndex, sTIDL_ConvParams_t* params, float* ptr, float* bias, int32_t inputIdx)
{
  int32_t status = 0;
  int32_t i1=0, i2=0, index = inputIdx;

  int8_t tempName[TIDL_STRING_SIZE];
  sTIDL_LayerPC_t* reshape[TIDL_PATCH_EMBEDDING_RESHAPES];
  sTIDL_LayerPC_t* transpose[TIDL_PATCH_EMBEDDING_TRANSPOSES];

  /**
   * Add the required layers
  */

  sTIDL_LayerPC_t& input    = pOrgTIDLNetStructure.TIDLPCLayers[inputIdx];

  TIDL_addLayer(pOrgTIDLNetStructure, index, TIDL_ReshapeLayer, dataIndex, pOrgTIDLNetStructure.numLayers, ADD_LAYER_AFTER);
  reshape[0] = &pOrgTIDLNetStructure.TIDLPCLayers[pOrgTIDLNetStructure.numLayers - 1];
  /**
   * Update the layerParams of the added layers
  */

  /* Configure reshape 1 */

  int64_t shape1[TIDL_DIM_MAX] = {0};
  shape1[0] = input.outData[0].dimValues[TIDL_DIM_BATCH];
  shape1[1] = input.outData[0].dimValues[TIDL_DIM_NUMCH];
  shape1[2] = input.outData[0].dimValues[TIDL_DIM_HEIGHT]/params->kernelH;
  shape1[3] = params->kernelH;
  shape1[4] = input.outData[0].dimValues[TIDL_DIM_WIDTH]/params->kernelW;
  shape1[5] = params->kernelW;

  tidl_configureReshapeLayer (reshape[0], (void*)shape1);

  strcpy((char*)tempName, (char*)reshape[0]->outDataNames[0]);
  tidl_sortLayersInProcOrder(&pOrgTIDLNetStructure, &tempTIDLNetStructure, pOrgTIDLNetStructure.numLayers);

  index = TIDL_getLayerIdx(&pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, (char*)tempName);

  TIDL_addLayer(pOrgTIDLNetStructure, index, TIDL_TransposeLayer, dataIndex, pOrgTIDLNetStructure.numLayers, ADD_LAYER_AFTER);
  transpose[0] = &pOrgTIDLNetStructure.TIDLPCLayers[pOrgTIDLNetStructure.numLayers - 1];

  /**
   * Configure transpose 1
  */
  int32_t perm1[] = {0,1,2,4,3,5};

  tidl_configureTransposeLayer (transpose[0], perm1);

  strcpy((char*)tempName, (char*)transpose[0]->outDataNames[0]);
  tidl_sortLayersInProcOrder(&pOrgTIDLNetStructure, &tempTIDLNetStructure, pOrgTIDLNetStructure.numLayers);

  index = TIDL_getLayerIdx(&pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, (char*)tempName);

  TIDL_addLayer(pOrgTIDLNetStructure, index, TIDL_ReshapeLayer, dataIndex, pOrgTIDLNetStructure.numLayers, ADD_LAYER_AFTER);
  reshape[1] = &pOrgTIDLNetStructure.TIDLPCLayers[pOrgTIDLNetStructure.numLayers - 1];

  /**
   * Configure reshape 2
  */
  int64_t shape2[TIDL_DIM_MAX] = {0};
  shape2[0] = shape1[TIDL_DIM_BATCH];
  shape2[1] = 1;
  shape2[2] = 1;
  shape2[3] = shape1[TIDL_DIM_DIM1];
  shape2[4] = shape1[TIDL_DIM_DIM2] * shape1[TIDL_DIM_HEIGHT];
  shape2[5] = shape1[TIDL_DIM_NUMCH] * shape1[TIDL_DIM_WIDTH];

  tidl_configureReshapeLayer (reshape[1], (void*)shape2);

  strcpy((char*)tempName, (char*)reshape[1]->outDataNames[0]);
  tidl_sortLayersInProcOrder(&pOrgTIDLNetStructure, &tempTIDLNetStructure, pOrgTIDLNetStructure.numLayers);

  index = TIDL_getLayerIdx(&pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, (char*)tempName);

  TIDL_addLayer(pOrgTIDLNetStructure, index, TIDL_TransposeLayer, dataIndex, pOrgTIDLNetStructure.numLayers, ADD_LAYER_AFTER);
  transpose[1] = &pOrgTIDLNetStructure.TIDLPCLayers[pOrgTIDLNetStructure.numLayers - 1];

  /**
   * Configure transpose 2
  */
  int32_t perm2[] = {0,1,2,4,3,5};

  tidl_configureTransposeLayer (transpose[1], perm1);

  strcpy((char*)tempName, (char*)transpose[1]->outDataNames[0]);
  tidl_sortLayersInProcOrder(&pOrgTIDLNetStructure, &tempTIDLNetStructure, pOrgTIDLNetStructure.numLayers);

  index = TIDL_getLayerIdx(&pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, (char*)tempName);

  TIDL_addLayer(pOrgTIDLNetStructure, index, TIDL_ReshapeLayer, dataIndex, pOrgTIDLNetStructure.numLayers, ADD_LAYER_AFTER);
  reshape[2] = &pOrgTIDLNetStructure.TIDLPCLayers[pOrgTIDLNetStructure.numLayers - 1];

  /**
   * Configure reshape 3
  */
  int64_t shape3[TIDL_DIM_MAX] = {0};
  shape3[0] = shape2[TIDL_DIM_BATCH];
  shape3[1] = shape2[TIDL_DIM_DIM1];
  shape3[2] = shape2[TIDL_DIM_DIM2];
  shape3[3] = 1;
  shape3[4] = shape2[TIDL_DIM_HEIGHT];
  shape3[5] = shape2[TIDL_DIM_WIDTH] * shape2[TIDL_DIM_NUMCH];

  tidl_configureReshapeLayer (reshape[2], (void*)shape3);

  strcpy((char*)tempName, (char*)reshape[2]->outDataNames[0]);
  tidl_sortLayersInProcOrder(&pOrgTIDLNetStructure, &tempTIDLNetStructure, pOrgTIDLNetStructure.numLayers);

  index = TIDL_getLayerIdx(&pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, (char*)tempName);

  TIDL_addLayer(pOrgTIDLNetStructure, index, TIDL_InnerProductLayer, dataIndex, pOrgTIDLNetStructure.numLayers, ADD_LAYER_AFTER);
  sTIDL_LayerPC_t& ip       = pOrgTIDLNetStructure.TIDLPCLayers[pOrgTIDLNetStructure.numLayers-1];

  /**
   * Setup Inner Product Layer
  */
  ip.numInBufs = 2;
  ip.layerParams.innerProductParams.numInRows  = params->numOutChannels;
  ip.layerParams.innerProductParams.numOutCols = shape3[TIDL_DIM_HEIGHT];
  ip.layerParams.innerProductParams.numInCols  = shape3[TIDL_DIM_WIDTH];
  ip.layerParams.innerProductParams.isBias     = 0;
  ip.layerParams.innerProductParams.inputBTranspose = 1;
  ip.layerParams.innerProductParams.constIdx = 0; /*"A" is the constant buffer*/


  int32_t bufSize = params->numOutChannels * params->numInChannels * params->kernelH * params->kernelW;
  int32_t biasSize = params->numOutChannels;

  int32_t constIdx = tidl_createConstDataLayer (pOrgTIDLNetStructure, dataIndex, ptr, bufSize, pOrgTIDLNetStructure.numLayers);
  sTIDL_LayerPC_t& constLayer = pOrgTIDLNetStructure.TIDLPCLayers[constIdx];

  constLayer.outData[0].dimValues[TIDL_DIM_BATCH]  = 1;
  constLayer.outData[0].dimValues[TIDL_DIM_DIM1]   = 1;
  constLayer.outData[0].dimValues[TIDL_DIM_DIM2]   = 1;
  constLayer.outData[0].dimValues[TIDL_DIM_NUMCH]  = 1;
  constLayer.outData[0].dimValues[TIDL_DIM_HEIGHT] = params->numOutChannels;
  constLayer.outData[0].dimValues[TIDL_DIM_WIDTH]  = params->numInChannels * params->kernelH * params->kernelW;

  ip.inData[1] = constLayer.outData[0];
  strcpy((char*)ip.inDataNames[1], (char*)constLayer.outDataNames[0]);

  sTIDL_DataParams_t temp;
  /**
   * Swap both inputs of innerproduct
   * Input A of the innerproduct needs to be transposed, but we support transpose op for ip only for input B
   * Hence swap both inputs
  */
  temp         = ip.inData[0];
  ip.inData[0] = ip.inData[1];
  ip.inData[1] = temp;

  strcpy((char*)tempName, (char*)ip.inDataNames[0]);
  strcpy((char*)ip.inDataNames[0], (char*)ip.inDataNames[1]);
  strcpy((char*)ip.inDataNames[1], (char*)tempName);

  strcpy((char*)tempName, (char*)ip.outDataNames[0]);
  tidl_sortLayersInProcOrder(&pOrgTIDLNetStructure, &tempTIDLNetStructure, pOrgTIDLNetStructure.numLayers);

  index = TIDL_getLayerIdx(&pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, (char*)tempName);

  TIDL_addLayer(pOrgTIDLNetStructure, index, TIDL_EltWiseLayer, dataIndex, pOrgTIDLNetStructure.numLayers, ADD_LAYER_AFTER);
  sTIDL_LayerPC_t& biasAdd       = pOrgTIDLNetStructure.TIDLPCLayers[pOrgTIDLNetStructure.numLayers-1];

  /**
   * Setup Eltwise Layer
  */
  biasAdd.numInBufs = 2;
  biasAdd.layerParams.eltWiseParams.eltWiseType  = TIDL_EltWiseSum;
  biasAdd.allowlistingMetaData.numConstInputs = 1;
  std::vector<int32_t> constDims;
  constDims.push_back(biasSize);
  biasAdd.allowlistingMetaData.constTensorsDims.push_back(constDims);

  constIdx = tidl_createConstDataLayer (pOrgTIDLNetStructure, dataIndex, bias, biasSize, pOrgTIDLNetStructure.numLayers);
  sTIDL_LayerPC_t& biasAddConst = pOrgTIDLNetStructure.TIDLPCLayers[constIdx];

  biasAddConst.outData[0].dimValues[TIDL_DIM_BATCH]  = 1;
  biasAddConst.outData[0].dimValues[TIDL_DIM_DIM1]   = 1;
  biasAddConst.outData[0].dimValues[TIDL_DIM_DIM2]   = 1;
  biasAddConst.outData[0].dimValues[TIDL_DIM_NUMCH]  = 1;
  biasAddConst.outData[0].dimValues[TIDL_DIM_HEIGHT] = params->numOutChannels;
  biasAddConst.outData[0].dimValues[TIDL_DIM_WIDTH]  =  shape3[TIDL_DIM_HEIGHT];

  biasAdd.inData[1] = biasAddConst.outData[0];
  strcpy((char*)biasAdd.inDataNames[1], (char*)biasAddConst.outDataNames[0]);
  tidl_broadcastWeights (biasAddConst, params->numOutChannels * shape3[TIDL_DIM_HEIGHT]);
  
  return status;
}

int32_t tidl_checkMatMulTranspose (sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t ipIdx, int32_t *perm, int32_t& oneConst, int32_t& twoConst)
{
  int32_t status = 0;
  int32_t inOne = tidl_getInLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, pOrgTIDLNetStructure.TIDLPCLayers[ipIdx].inData[0].dataId);
  int32_t inTwo = tidl_getInLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, pOrgTIDLNetStructure.TIDLPCLayers[ipIdx].inData[1].dataId);

  oneConst = pOrgTIDLNetStructure.TIDLPCLayers[inOne].layerType == TIDL_ConstDataLayer;
  twoConst = pOrgTIDLNetStructure.TIDLPCLayers[inTwo].layerType == TIDL_ConstDataLayer;

  // if (oneConst || twoConst)
  // {

    if (perm[0] == 0 &&
        perm[1] == 1 &&
        perm[2] == 2 &&
        perm[3] == 3 &&
        perm[4] == 5 &&
        perm[5] == 4)
      {
        status = 1;
      }
//  }
  return status;
}

int32_t tidl_transposeWeights (float* src_ptr, float* dst_ptr, int32_t channels, int32_t height, int32_t width)
{
  int32_t i1, i2, i3;
  int32_t status = 0;

  for (i1 = 0; i1 < channels; i1++)
  {
    for (i2 = 0; i2 < height; i2++)
    {
      for (i3 = 0; i3 < width; i3++)
      {
        dst_ptr[i1 * height * width + i3 * height + i2] = src_ptr[i1 * height * width + i2 * width + i3];
      }
    }
  }
  return status;
}

int32_t tidl_fuseTransposeMatMul(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t& layerIndex, int32_t* dataIndex)
{
  /**
   * This function fuses any transpose from the output side of the matmul by absorbing the transpose into the inputs of matmul
  */
  int32_t i1, i2, i3, i4;
  int32_t status = 0;

  for (i1 = 0; i1 < pOrgTIDLNetStructure.numLayers; i1++)
  {
    if ((pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.innerProductParams.constIdx != 1) && pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_InnerProductLayer)
    {
      int32_t outIdx = tidl_getOutLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].dataId);

      /** Skip reshape layers if any inbetween matmul & transpose layers*/
      while (pOrgTIDLNetStructure.TIDLPCLayers[outIdx].layerType == TIDL_ReshapeLayer)
      {
        outIdx = tidl_getOutLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, pOrgTIDLNetStructure.TIDLPCLayers[outIdx].outData[0].dataId);
      }
      /** Look for the transpose layer*/
      if (pOrgTIDLNetStructure.TIDLPCLayers[outIdx].layerType == TIDL_TransposeLayer)
      {
        int32_t *perm = pOrgTIDLNetStructure.TIDLPCLayers[outIdx].layerParams.transposeParams.perm;
        int32_t oneConst, twoConst;
        int32_t inOne = tidl_getInLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0].dataId);
        int32_t inTwo = tidl_getInLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[1].dataId);
        /** Check if we can fuse the transpose into matmul*/
        if (tidl_checkMatMulTranspose (pOrgTIDLNetStructure, i1, perm, oneConst, twoConst) == 1 )
        {
          /** Different handling based on how many inputs of the matmul are const inputs,
           * Note that transpose cannot be fused if both the inputs are variable
           * A = B x C => At = Ct X Bt, atleast one of them should be const data, as we don't have support for transpose for both inputs in hardware
          */
          if (oneConst && twoConst)
          {
            /** Both the inputs are const layers, transpose weights of A input & set the transpose flag for the B input*/
            status = 1;
            /**
             * Transpose incoming inputA
            */
            float* dst_ptr = (float*)malloc (pOrgTIDLNetStructure.TIDLPCLayers[inTwo].weights.bufSize * sizeof (float));
            tidl_transposeWeights((float*)pOrgTIDLNetStructure.TIDLPCLayers[inTwo].weights.ptr,
                                  dst_ptr,
                                  pOrgTIDLNetStructure.TIDLPCLayers[inTwo].outData[0].dimValues[TIDL_DIM_NUMCH],
                                  pOrgTIDLNetStructure.TIDLPCLayers[inTwo].outData[0].dimValues[TIDL_DIM_HEIGHT],
                                  pOrgTIDLNetStructure.TIDLPCLayers[inTwo].outData[0].dimValues[TIDL_DIM_WIDTH]);

            int32_t t = pOrgTIDLNetStructure.TIDLPCLayers[inTwo].outData[0].dimValues[TIDL_DIM_HEIGHT];
            pOrgTIDLNetStructure.TIDLPCLayers[inTwo].outData[0].dimValues[TIDL_DIM_HEIGHT] = pOrgTIDLNetStructure.TIDLPCLayers[inTwo].outData[0].dimValues[TIDL_DIM_WIDTH];
            pOrgTIDLNetStructure.TIDLPCLayers[inTwo].outData[0].dimValues[TIDL_DIM_WIDTH] = t;

            /**
             * Transpose incoming inputB
            */
            pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.innerProductParams.inputBTranspose = 1;
          }
          else if (oneConst && pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.innerProductParams.inputBTranspose == 1)
          {
            /**
             * Swap inputs & apply transpose to const input
            */
            status = 1;
          }
          else if (twoConst)
          {
            /**
             * Swap inputs and apply transpose to const input & set inputBTranspose to 1
            */
            status = 1;

            if (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.innerProductParams.inputBTranspose == 0)
            {
              /**
               * Transpose incoming inputA
              */
              float* dst_ptr = (float*)malloc (pOrgTIDLNetStructure.TIDLPCLayers[inTwo].weights.bufSize * sizeof (float));
              tidl_transposeWeights((float*)pOrgTIDLNetStructure.TIDLPCLayers[inTwo].weights.ptr,
                                    dst_ptr,
                                    pOrgTIDLNetStructure.TIDLPCLayers[inTwo].outData[0].dimValues[TIDL_DIM_NUMCH],
                                    pOrgTIDLNetStructure.TIDLPCLayers[inTwo].outData[0].dimValues[TIDL_DIM_HEIGHT],
                                    pOrgTIDLNetStructure.TIDLPCLayers[inTwo].outData[0].dimValues[TIDL_DIM_WIDTH]);

              int32_t t = pOrgTIDLNetStructure.TIDLPCLayers[inTwo].outData[0].dimValues[TIDL_DIM_HEIGHT];
              pOrgTIDLNetStructure.TIDLPCLayers[inTwo].outData[0].dimValues[TIDL_DIM_HEIGHT] = pOrgTIDLNetStructure.TIDLPCLayers[inTwo].outData[0].dimValues[TIDL_DIM_WIDTH];
              pOrgTIDLNetStructure.TIDLPCLayers[inTwo].outData[0].dimValues[TIDL_DIM_WIDTH] = t;

              free (pOrgTIDLNetStructure.TIDLPCLayers[inTwo].weights.ptr);
              pOrgTIDLNetStructure.TIDLPCLayers[inTwo].weights.ptr = dst_ptr;
            }

            /**
             * Transpose incoming inputB
            */
            pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.innerProductParams.inputBTranspose = 1;
          }

          else if ((oneConst == 0 || twoConst == 0) && pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.innerProductParams.inputBTranspose == 1)
          {

            /** If the innerproduct has both the variable inputs, check if the B transpose flag is set
             * If set, keep it set for the incoming A input
            */
            status = 1;

          }

          if (status == 1)
          {
            sTIDL_DataParams_t temp;
            /**
             * Swap both inputs of innerproduct
             * Input A of the innerproduct needs to be transposed, but we support transpose op for ip only for input B
             * Hence swap both inputs
            */
            temp         = pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0];
            pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[1];
            pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[1] = temp;

            int8_t tempName[512];

            strcpy((char*)tempName, (char*)pOrgTIDLNetStructure.TIDLPCLayers[i1].inDataNames[0]);
            strcpy((char*)pOrgTIDLNetStructure.TIDLPCLayers[i1].inDataNames[0], (char*)pOrgTIDLNetStructure.TIDLPCLayers[i1].inDataNames[1]);
            strcpy((char*)pOrgTIDLNetStructure.TIDLPCLayers[i1].inDataNames[1], (char*)tempName);

            if (pOrgTIDLNetStructure.TIDLPCLayers[i1].bias.ptr != NULL &&
                pOrgTIDLNetStructure.TIDLPCLayers[i1].bias.bufSize == pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.innerProductParams.numInRows * pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.innerProductParams.numOutCols)
            {
              /**
               * Transpose bias
              */
              float* tp_bias = (float*)malloc (pOrgTIDLNetStructure.TIDLPCLayers[i1].bias.bufSize * sizeof (float));

              tidl_transposeWeights((float*)pOrgTIDLNetStructure.TIDLPCLayers[i1].bias.ptr,
                                    tp_bias,
                                    1,
                                    pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.innerProductParams.numInRows,
                                    pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.innerProductParams.numOutCols);
              free (pOrgTIDLNetStructure.TIDLPCLayers[i1].bias.ptr);
              pOrgTIDLNetStructure.TIDLPCLayers[i1].bias.ptr = tp_bias;
            }

            int32_t inOne = tidl_getInLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0].dataId);
            int32_t inTwo = tidl_getInLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[1].dataId);

            pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.innerProductParams.numInRows  = pOrgTIDLNetStructure.TIDLPCLayers[inOne].outData[0].dimValues[TIDL_DIM_HEIGHT];
            if (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.innerProductParams.inputBTranspose == 1)
            {
              pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.innerProductParams.numOutCols = pOrgTIDLNetStructure.TIDLPCLayers[inTwo].outData[0].dimValues[TIDL_DIM_HEIGHT];
            }
            else
            {
              pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.innerProductParams.numOutCols = pOrgTIDLNetStructure.TIDLPCLayers[inTwo].outData[0].dimValues[TIDL_DIM_WIDTH];
            }
            pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.innerProductParams.numInCols = pOrgTIDLNetStructure.TIDLPCLayers[inOne].outData[0].dimValues[TIDL_DIM_WIDTH];

            /**
             * Remove the Reshape & Transpose layers as the Transpose is now fused into MatMul
            */
            int32_t prevIdx, outIdx = tidl_getOutLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].dataId);
            while (pOrgTIDLNetStructure.TIDLPCLayers[outIdx].layerType == TIDL_ReshapeLayer)
            {
              prevIdx = outIdx;
              outIdx = tidl_getOutLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, pOrgTIDLNetStructure.TIDLPCLayers[prevIdx].outData[0].dataId);
              pOrgTIDLNetStructure.TIDLPCLayers[prevIdx].numInBufs  = -1;
              pOrgTIDLNetStructure.TIDLPCLayers[prevIdx].numOutBufs = -1;
            }
            if (pOrgTIDLNetStructure.TIDLPCLayers[outIdx].layerType == TIDL_TransposeLayer)
            {
              prevIdx = outIdx;
              outIdx = tidl_getOutLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, pOrgTIDLNetStructure.TIDLPCLayers[outIdx].outData[0].dataId);
              pOrgTIDLNetStructure.TIDLPCLayers[prevIdx].numInBufs  = -1;
              pOrgTIDLNetStructure.TIDLPCLayers[prevIdx].numOutBufs = -1;
              for (int32_t m = 0; m < pOrgTIDLNetStructure.TIDLPCLayers[outIdx].numInBufs; m++)
              {
                if (pOrgTIDLNetStructure.TIDLPCLayers[outIdx].inData[m].dataId  == pOrgTIDLNetStructure.TIDLPCLayers[prevIdx].outData[0].dataId)
                {
                  pOrgTIDLNetStructure.TIDLPCLayers[outIdx].inData[m].dataId = pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].dataId;
                  strcpy((char*)pOrgTIDLNetStructure.TIDLPCLayers[outIdx].inDataNames[m], (char*)pOrgTIDLNetStructure.TIDLPCLayers[i1].outDataNames[0]);
                  /** Copy out shape of Transpose to out shape of Innerproduct*/
                  memcpy (pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].dimValues, pOrgTIDLNetStructure.TIDLPCLayers[prevIdx].outData[0].dimValues, sizeof(int32_t)* TIDL_DIM_MAX);
                }
              }

              tidl_removeMergedLayersFromNet(&pOrgTIDLNetStructure, &tempTIDLNetStructure);
            }
          }
        }
      }
    }
  }
  return status;
}

int32_t tidl_optimizePatchEmbedding(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t& layerIndex, int32_t* dataIndex)
{
  int32_t i1, i2, i3, i4;
  int32_t status = 0;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    if (tidl_identifyPatchEmbeddingBlock(pOrgTIDLNetStructure, i1) == 1)
    {
      /**
       * We replace the patch embedding block with following,
       * Two transposes (with appropriate reshapes) & an inner product
      */

    /**
      * 1. Remove the patch embedding block
    */
      int32_t convIdx = i1;

      sTIDL_ConvParams_t* params = (sTIDL_ConvParams_t*)malloc(sizeof(sTIDL_ConvParams_t));
      memcpy(params, &pOrgTIDLNetStructure.TIDLPCLayers[convIdx].layerParams.convParams, sizeof(sTIDL_ConvParams_t));

      float* ptr = (float*)malloc(pOrgTIDLNetStructure.TIDLPCLayers[convIdx].weights.bufSize * sizeof (float));
      float* bias = (float*)malloc(pOrgTIDLNetStructure.TIDLPCLayers[convIdx].bias.bufSize * sizeof(float));

      memcpy (ptr, pOrgTIDLNetStructure.TIDLPCLayers[convIdx].weights.ptr, pOrgTIDLNetStructure.TIDLPCLayers[convIdx].weights.bufSize * sizeof(float));
      memcpy (bias, pOrgTIDLNetStructure.TIDLPCLayers[convIdx].bias.ptr, pOrgTIDLNetStructure.TIDLPCLayers[convIdx].bias.bufSize * sizeof(float));

      /** Free the weights & bias pointers of convolution*/

      if (pOrgTIDLNetStructure.TIDLPCLayers[convIdx].weights.ptr != NULL)
      {
        free (pOrgTIDLNetStructure.TIDLPCLayers[convIdx].weights.ptr);
        pOrgTIDLNetStructure.TIDLPCLayers[convIdx].weights.ptr = NULL;
        pOrgTIDLNetStructure.TIDLPCLayers[convIdx].weights.bufSize = 0;
      }

      if (pOrgTIDLNetStructure.TIDLPCLayers[convIdx].bias.ptr != NULL)
      {
        free (pOrgTIDLNetStructure.TIDLPCLayers[convIdx].bias.ptr);
        pOrgTIDLNetStructure.TIDLPCLayers[convIdx].bias.ptr = NULL;
        pOrgTIDLNetStructure.TIDLPCLayers[convIdx].bias.bufSize = 0;
      }
      int32_t inLayerIdx = tidl_getInLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, pOrgTIDLNetStructure.TIDLPCLayers[convIdx].inData[0].dataId);
      int32_t outLayerIdx = tidl_getOutLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, pOrgTIDLNetStructure.TIDLPCLayers[convIdx].outData[0].dataId);

      /**Replace the layer with a reshape layer, which is responsible for reshaping the patch embedding output shape to the shape required in onnx model
       * Mostly our optimization output shape may not be same as the shape onnx is expecting , hence this reshape is required to bring back the required shape
      */

      pOrgTIDLNetStructure.TIDLPCLayers[convIdx].layerType = TIDL_ReshapeLayer;
      pOrgTIDLNetStructure.TIDLPCLayers[convIdx].layerPCParams.reshapeParams.isInduced = 1;

      /**
       * 2. Add the optimization layers
       *  Reshape -> Transpose -> Reshape -> Transpose -> Reshape -> Innerproduct
      */
      tidl_replacePatchEmbeddingBlock(pOrgTIDLNetStructure, dataIndex, params, ptr, bias, inLayerIdx);

    }
  }

  return status;
}

int32_t tidl_splitResizeLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t& layerIndex, int32_t& dataIndex)
{
  int32_t i1, i2, i3, i4;
  int32_t status = 0;

  for (i1 = 0; i1 < layerIndex; i1++)
  {
    if ((pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_ResizeLayer) &&
        (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.resizeParams.resizeRatio[TIDL_DIM_WIDTH] > 4) &&
         (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.resizeParams.resizeRatio[TIDL_DIM_HEIGHT] > 4))

    {
      sTIDL_LayerPC_t &curr = pOrgTIDLNetStructure.TIDLPCLayers[i1];

      int32_t  idx_previous = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, curr.inData[0].dataId);
      if (idx_previous == -1)
      {
        // Treat this error as debug info, since the second split layer will not get previous layer index.
        printf("ERROR: tidl_splitResizeLayer layer search for previous layer failed!\n");
        return -1;
      }
      sTIDL_LayerPC_t *previous = &pOrgTIDLNetStructure.TIDLPCLayers[idx_previous];
      i2 = 0;
      while ((curr.layerParams.resizeParams.resizeRatio[TIDL_DIM_WIDTH] > 2) ||
             (curr.layerParams.resizeParams.resizeRatio[TIDL_DIM_HEIGHT] > 2))
      {
        float crw = curr.layerParams.resizeParams.resizeRatio[TIDL_DIM_WIDTH];
        float crh = curr.layerParams.resizeParams.resizeRatio[TIDL_DIM_HEIGHT];
        float nrw, nrh;
        sTIDL_LayerPC_t *resize_new = &pOrgTIDLNetStructure.TIDLPCLayers[layerIndex];
        resize_new->inData[0] = previous->outData[0];
        strcpy((char*)resize_new->inDataNames[0], (char*)previous->outDataNames[0]);
        resize_new->layerType = curr.layerType;
        sprintf((char*)resize_new->name, "%s_TIDL_%d", (char*)curr.name, i2);
        resize_new->layerParams.resizeParams = curr.layerParams.resizeParams;
        resize_new->numInBufs = 1;
        resize_new->numOutBufs = 1;
        resize_new->outConsumerCnt[0] = 1;
        resize_new->outConsumerLinked[0] = 1;
        resize_new->outData[0] = resize_new->inData[0];
        resize_new->outData[0].dataId = dataIndex++;
        sprintf((char *)resize_new->outDataNames[0], "%s_TIDL_%d", curr.outDataNames[0], i2);
        /* Copy activation parameters from original layer*/
        resize_new->actParams = curr.actParams;
        if((crw > 4) &&(crh > 4))
        {
          nrw = 4.0;
          nrh = 4.0;
        }
        else if((crw > 2) &&(crh > 2))
        {
          nrw = 2.0;
          nrh = 2.0;
        }
        else if(crw > 2)
        {
          nrw = 2.0;
          nrh = 1.0;
        }
        else if(crh > 2)
        {
          nrw = 1.0;
          nrh = 2.0;
        }
        else
        {
          printf("ERROR: tidl_splitResizeLayer Resize Layer split is entering infinite loop. This is not expected!\n");
          exit(-1);
        }
        crw /= nrw;
        crh /= nrh;

        resize_new->layerParams.resizeParams.resizeRatio[TIDL_DIM_WIDTH] = nrw;
        resize_new->layerParams.resizeParams.resizeRatio[TIDL_DIM_HEIGHT] = nrh;

        resize_new->outData[0].dimValues[TIDL_DIM_WIDTH]  *= nrw;
        resize_new->outData[0].dimValues[TIDL_DIM_HEIGHT] *= nrh;

        curr.layerParams.resizeParams.resizeRatio[TIDL_DIM_WIDTH] = crw;
        curr.layerParams.resizeParams.resizeRatio[TIDL_DIM_HEIGHT] = crh;
        strcpy((char*)curr.inDataNames[0], (char*)resize_new->outDataNames[0]);
        curr.inData[0] = resize_new->outData[0];
        previous = resize_new;
        i2++;
        layerIndex++;
        if ((crw == 4 && crh == 4) || (crw == 2 && crh == 2))
        {
          break;
        }
      }
    }
  }

  return 0;
}


int32_t tidl_mergeDepthToSpaceLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i1, i2, i3, i4;
  int32_t status = 0;

  for (i1 = 0; i1 < layerIndex; i1++)
  {
    if ((pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_DepthToSpaceLayer) &&
        ((pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.depthToSpaceParams.blockSize == 2) ||
        (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.depthToSpaceParams.blockSize  == 4) ||
        (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.depthToSpaceParams.blockSize  == 8)))
    {
      int32_t  idx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0].dataId);
      if (idx == -1)
      {
        continue;
      }
      sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure.TIDLPCLayers[idx];
      if ((TIDLPCLayers.layerType == TIDL_ConvolutionLayer) &&
          (TIDLPCLayers.outConsumerCnt[0] == 1) &&
          (TIDLPCLayers.layerParams.convParams.numGroups == 1) &&
          (TIDLPCLayers.layerParams.convParams.kernelH == 1) &&
          (TIDLPCLayers.layerParams.convParams.kernelW == 1) &&
          (TIDLPCLayers.layerParams.convParams.strideH == 1) &&
          (TIDLPCLayers.layerParams.convParams.strideW == 1) )
      {
        TIDLPCLayers.numMacs += pOrgTIDLNetStructure.TIDLPCLayers[i1].numMacs;
        TIDLPCLayers.outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0];
        strcpy((char*)TIDLPCLayers.outDataNames[0], (char*)pOrgTIDLNetStructure.TIDLPCLayers[i1].outDataNames[0]);
        TIDLPCLayers.outConsumerCnt[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].outConsumerCnt[0];
        TIDLPCLayers.layerParams.convParams.enableDepthToSpace = 1;
        {
          int32_t upscaleFactor = TIDLPCLayers.layerParams.convParams.numOutChannels / TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH];
          upscaleFactor =  sqrt(upscaleFactor);
          TIDLPCLayers.layerParams.convParams.upscaleFactor = upscaleFactor;
        }
        pOrgTIDLNetStructure.TIDLPCLayers[i1].numInBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[i1].numOutBufs = -1;
      }
    }
  }

  return 0;
}

int32_t tidl_mergeBatchToSpaceInDWConvLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i1, i2, i3, i4;
  int32_t status = 0;

  for (i1 = 0; i1 < layerIndex; i1++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_SpaceToBatchLayer)
    {
      sTIDL_LayerPC_t &TIDLSpaceToBatchLayer = pOrgTIDLNetStructure.TIDLPCLayers[i1];
      int32_t  idx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, TIDLSpaceToBatchLayer.inData[0].dataId);
      if (idx == -1)
      {
        continue;
      }
      sTIDL_LayerPC_t &TIDLSpaceToBatchInLayer = pOrgTIDLNetStructure.TIDLPCLayers[idx];
      int32_t DWConvIdx = tidl_getOutLayer(pOrgTIDLNetStructure, layerIndex, TIDLSpaceToBatchLayer.outData[0].dataId);
      sTIDL_LayerPC_t &TIDLDWConvLayer = pOrgTIDLNetStructure.TIDLPCLayers[DWConvIdx];
      int32_t batchToSpaceIdx = tidl_getOutLayer(pOrgTIDLNetStructure, layerIndex, TIDLDWConvLayer.outData[0].dataId);
      sTIDL_LayerPC_t &TIDLBatchToSpaceLayer = pOrgTIDLNetStructure.TIDLPCLayers[batchToSpaceIdx];

      int32_t spaceToBatchBlockH = TIDLSpaceToBatchLayer.layerPCParams.spaceToBatchParams.blockHeight;
      int32_t spaceToBatchBlockW = TIDLSpaceToBatchLayer.layerPCParams.spaceToBatchParams.blockWidth;
      int32_t spaceToBatchPadT = TIDLSpaceToBatchLayer.layerPCParams.spaceToBatchParams.padT;
      int32_t spaceToBatchPadB = TIDLSpaceToBatchLayer.layerPCParams.spaceToBatchParams.padB;
      int32_t spaceToBatchPadL = TIDLSpaceToBatchLayer.layerPCParams.spaceToBatchParams.padL;
      int32_t spaceToBatchPadR = TIDLSpaceToBatchLayer.layerPCParams.spaceToBatchParams.padR;

      int32_t padH = (spaceToBatchPadT < spaceToBatchPadB) ? spaceToBatchPadT : spaceToBatchPadB;
      int32_t padW = (spaceToBatchPadL < spaceToBatchPadR) ? spaceToBatchPadL : spaceToBatchPadR;
      TIDLDWConvLayer.layerParams.convParams.padH = TIDLDWConvLayer.layerParams.convParams.padH + padH;
      TIDLDWConvLayer.layerParams.convParams.padW = TIDLDWConvLayer.layerParams.convParams.padW + padW;
      TIDLDWConvLayer.layerParams.convParams.dilationH = spaceToBatchBlockH;
      TIDLDWConvLayer.layerParams.convParams.dilationW = spaceToBatchBlockW;

      TIDLDWConvLayer.inData[0] = TIDLSpaceToBatchInLayer.outData[0];
      strcpy((char *)TIDLDWConvLayer.inDataNames[0], (char *)TIDLSpaceToBatchInLayer.outDataNames[0]);

      TIDLDWConvLayer.outData[0] = TIDLBatchToSpaceLayer.outData[0];
      strcpy((char *)TIDLDWConvLayer.outDataNames[0], (char *)TIDLBatchToSpaceLayer.outDataNames[0]);
      TIDLDWConvLayer.outConsumerCnt[0] = TIDLBatchToSpaceLayer.outConsumerCnt[0];

      TIDLSpaceToBatchLayer.numInBufs = -1;
      TIDLSpaceToBatchLayer.numOutBufs = -1;
      TIDLBatchToSpaceLayer.numInBufs = -1;
      TIDLBatchToSpaceLayer.numOutBufs = -1;
    }
  }
  return 0;
}

int32_t tidl_mergePoolingLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i1, i2, i3, i4;
  int32_t status = 0;
  int32_t merged;

  for (i1 = 0; i1 < layerIndex; i1++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_PoolingLayer &&
        pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.poolParams.kernelH != 0 &&
        pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.poolParams.kernelW != 0)
    {
      merged = 0;
      int32_t  idx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0].dataId);
      if (idx == -1)
      {
        continue;
      }
      sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure.TIDLPCLayers[idx];
      if ((TIDLPCLayers.layerType == TIDL_ConvolutionLayer) &&
        (TIDLPCLayers.outConsumerCnt[0] == 1) &&
        (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.poolParams.kernelH == 2) &&
        (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.poolParams.kernelW == 2))
      {
        merged = 1;
      }

      if (merged == 1)
      {
        int32_t  idx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0].dataId);
        if (idx == -1)
        {
          return -1;
        }
        sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure.TIDLPCLayers[idx];
        TIDLPCLayers.numMacs += pOrgTIDLNetStructure.TIDLPCLayers[i1].numMacs;
        TIDLPCLayers.outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0];
        strcpy((char*)TIDLPCLayers.outDataNames[0], (char*)pOrgTIDLNetStructure.TIDLPCLayers[i1].outDataNames[0]);
        TIDLPCLayers.outConsumerCnt[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].outConsumerCnt[0];
        TIDLPCLayers.layerParams.convParams.poolParams = pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.poolParams;
        TIDLPCLayers.layerParams.convParams.enablePooling = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i1].numInBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[i1].numOutBufs = -1;
      }
    }
  }

  return 0;
}

int32_t tidl_mergeReshapeLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex, sTIDL_tfOutReshapeMap_t * sTIDL_tfOutReshapeTable)
{
  int32_t i1, i2, i3, i4;
  int32_t status = 0;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_ReshapeLayer)
    {
      int32_t  idx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0].dataId);
      if (idx == -1)
      {
        return -1;
      }
      sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure.TIDLPCLayers[idx];
      if (TIDLPCLayers.layerType == TIDL_ScatterElementsLayer && TIDLPCLayers.layerParams.scatterElementsParams.axis != -1)
      {
        TIDLPCLayers.numMacs += pOrgTIDLNetStructure.TIDLPCLayers[i1].numMacs;
        TIDLPCLayers.outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0];
        strcpy((char *)TIDLPCLayers.outDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i1].outDataNames[0]);
        TIDLPCLayers.outConsumerCnt[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].outConsumerCnt[0];
        pOrgTIDLNetStructure.TIDLPCLayers[i1].numInBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[i1].numOutBufs = -1;

        TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] = ((int64_t*)pOrgTIDLNetStructure.TIDLPCLayers[i1].weights.ptr)[1];
        TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = ((int64_t*)pOrgTIDLNetStructure.TIDLPCLayers[i1].weights.ptr)[2];
        TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = ((int64_t*)pOrgTIDLNetStructure.TIDLPCLayers[i1].weights.ptr)[3];

        sTIDL_LayerPC_t *TIDLPCLayersOut;
        int32_t  outIdx = tidl_getOutLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].dataId);
        if (outIdx != -1)
        {
          TIDLPCLayersOut = &pOrgTIDLNetStructure.TIDLPCLayers[outIdx];
          TIDLPCLayersOut->inData[0] = TIDLPCLayers.outData[0];
          tidl_updateOutDataShape(pOrgTIDLNetStructure, outIdx, layerIndex, sTIDL_tfOutReshapeTable);
        }
      }
    }
  }

  return 0;
}

// Convert ReshapeLayer to FlattenLayer (only for Relay import/tflite runtime):
//   This function should be called after tidl_mergeReshapeLayer() which merges ReshapeLayer
//   into PoolingLayer or InnerProductLayer.
int32_t tidl_convertReshapeToFlatten(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t numLayers)
{
  for (int i1 = 0; i1 < numLayers; i1++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_ReshapeLayer)
    {
      pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType = TIDL_FlattenLayer;
      TIDL_tfOutReshapeFlattenLayer(&pOrgTIDLNetStructure, i1);
      int32_t outIdx = tidl_getOutLayer(pOrgTIDLNetStructure, numLayers, pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].dataId);
      if (outIdx != -1)
      {
        pOrgTIDLNetStructure.TIDLPCLayers[outIdx].inData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0];
      }
    }
  }

  return 0;
}

int32_t tidl_convertIpLayerInputShape(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i1, i2, i3, i4;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_InnerProductLayer)
    {
      sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure.TIDLPCLayers[i1];
      int32_t  inIdx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0].dataId);
      if (inIdx == -1)
      {
        return -1;
      }
      sTIDL_LayerPC_t &TIDLPCLayersIn = pOrgTIDLNetStructure.TIDLPCLayers[inIdx];

      if ((TIDLPCLayersIn.layerType == TIDL_PoolingLayer) &&
          (TIDLPCLayersIn.layerParams.poolParams.poolingType == TIDL_AveragePooling) &&
          (TIDLPCLayersIn.layerParams.poolParams.kernelW == 0) &&
          (TIDLPCLayersIn.layerParams.poolParams.kernelH == 0) &&
          (TIDLPCLayersIn.outConsumerCnt[0] == 1))
      {
        TIDLPCLayersIn.layerParams.poolParams.kernelW = 0;
        TIDLPCLayersIn.layerParams.poolParams.kernelH = 0;

        TIDLPCLayersIn.outData[0].dimValues[TIDL_DIM_WIDTH] = TIDLPCLayersIn.outData[0].dimValues[TIDL_DIM_NUMCH] * TIDLPCLayersIn.outData[0].dimValues[TIDL_DIM_HEIGHT] * TIDLPCLayersIn.outData[0].dimValues[TIDL_DIM_WIDTH];
        TIDLPCLayersIn.outData[0].dimValues[TIDL_DIM_HEIGHT] = 1;
        TIDLPCLayersIn.outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
        TIDLPCLayersIn.outData[0].dimValues[TIDL_DIM_DIM1] = 1;
        TIDLPCLayersIn.outData[0].dimValues[TIDL_DIM_DIM2] = 1;

        TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH] * TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT] * TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH];
        TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT]    = 1;
        TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH]    = 1;
        TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM1]    = 1;
        TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM2]    = 1;
      }
    }
  }

  return 0;
}


int32_t tidl_convertSoftMaxLayerInputShape(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i1, i2, i3, i4;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_SoftMaxLayer &&
        TIDL_isOutTensorFlat(&pOrgTIDLNetStructure, i1))
    {
      sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure.TIDLPCLayers[i1];
      int32_t  inIdx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0].dataId);
      if (inIdx == -1)
      {
        return -1;
      }
      sTIDL_LayerPC_t &TIDLPCLayersIn = pOrgTIDLNetStructure.TIDLPCLayers[inIdx];

      if ((TIDLPCLayersIn.layerType == TIDL_PoolingLayer) &&
          (TIDLPCLayersIn.layerParams.poolParams.poolingType == TIDL_AveragePooling) &&
          (TIDLPCLayersIn.outConsumerCnt[0] == 1))
      {
        TIDLPCLayersIn.layerParams.poolParams.kernelW = 0;
        TIDLPCLayersIn.layerParams.poolParams.kernelH = 0;

        TIDLPCLayersIn.outData[0].dimValues[TIDL_DIM_WIDTH] = TIDLPCLayersIn.outData[0].dimValues[TIDL_DIM_NUMCH] * TIDLPCLayersIn.outData[0].dimValues[TIDL_DIM_HEIGHT] * TIDLPCLayersIn.outData[0].dimValues[TIDL_DIM_WIDTH];
        TIDLPCLayersIn.outData[0].dimValues[TIDL_DIM_HEIGHT] = 1;
        TIDLPCLayersIn.outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
        TIDLPCLayersIn.outData[0].dimValues[TIDL_DIM_DIM2] = 1;
        TIDLPCLayersIn.outData[0].dimValues[TIDL_DIM_DIM1] = 1;

        TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH] * TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT] * TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH];
        TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT]    = 1;
        TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH]    = 1;
        TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM2]    = 1;
        TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM1]    = 1;
        TIDL_tfOutReshapeSoftmax(&pOrgTIDLNetStructure, i1);
      }
      else
      {
        if ((TIDLPCLayersIn.outData[0].dimValues[TIDL_DIM_NUMCH] != 1) || (TIDLPCLayersIn.outData[0].dimValues[TIDL_DIM_HEIGHT] != 1))
        {
          printf("In put of TIDL_SoftMaxLayer layer needs to be Faltten. Please add Flatten layer to import this mdoels. Except SSD. \n");
          //exit(0);
        }
      }
    }
  }

  return 0;
}


int32_t tidl_convertConv2DToIpLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex, sTIDL_tfOutReshapeMap_t * sTIDL_tfOutReshapeTable)
{
  int32_t i1, i2, i3, i4;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_ConvolutionLayer)
    {
      sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure.TIDLPCLayers[i1];
      sTIDL_ConvParams_t &convParams = pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.convParams;
      if ((convParams.kernelW == 1) && (convParams.kernelH == 1) && (TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT] == 1) && (TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH] == 1))
      {
        int32_t  inIdx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0].dataId);
        if (inIdx == -1)
        {
          return -1;
        }
        sTIDL_LayerPC_t &TIDLPCLayersIn = pOrgTIDLNetStructure.TIDLPCLayers[inIdx];

        if ((TIDLPCLayersIn.layerType == TIDL_PoolingLayer) && (TIDLPCLayersIn.layerParams.poolParams.poolingType == TIDL_AveragePooling) && (TIDLPCLayersIn.outConsumerCnt[0] == 1))
        {
          sTIDL_LayerPC_t *TIDLPCLayersOut = NULL;
          int32_t  outIdx = tidl_getOutLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].dataId);
          if (outIdx != -1)
          {
            TIDLPCLayersOut = &pOrgTIDLNetStructure.TIDLPCLayers[outIdx];
          }
          if ((outIdx == -1) ||
            (TIDLPCLayersOut->layerType == TIDL_InnerProductLayer) ||
            (TIDLPCLayersOut->layerType == TIDL_DataLayer) ||
            (TIDLPCLayersOut->layerType == TIDL_SoftMaxLayer) ||
            (TIDLPCLayersOut->layerType == TIDL_FlattenLayer) ||
            (TIDLPCLayersOut->layerType == TIDL_ReshapeLayer))
          {
            TIDLPCLayersIn.layerParams.poolParams.kernelW = 0;
            TIDLPCLayersIn.layerParams.poolParams.kernelH = 0;

            sTIDL_LayerPC_t TIDLPCLayerstemp = TIDLPCLayers;
            TIDLPCLayers.layerType = TIDL_InnerProductLayer;
            TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH] * TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT] * TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH];
            TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT] = 1;
            TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH] = 1;
            TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM2] = 1;
            TIDLPCLayers.inData[0].dimValues[TIDL_DIM_DIM1] = 1;
            TIDLPCLayersIn.outData[0] = TIDLPCLayers.inData[0];

            TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH];
            TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = 1;
            TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
            TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2] = 1;
            TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] = 1;
            if (outIdx != -1)
            {
              TIDLPCLayersOut->inData[0] = TIDLPCLayers.outData[0];
              tidl_updateOutDataShape(pOrgTIDLNetStructure, outIdx,
                                      layerIndex, sTIDL_tfOutReshapeTable);
            }

            TIDLPCLayers.layerParams.innerProductParams.numInRows = 1;
            TIDLPCLayers.layerParams.innerProductParams.numInCols = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH];
            TIDLPCLayers.layerParams.innerProductParams.numBatches = 1;
            TIDLPCLayers.layerParams.innerProductParams.numOutCols = TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH];
            TIDLPCLayers.layerParams.innerProductParams.inputATranspose = 0;
            TIDLPCLayers.layerParams.innerProductParams.inputBTranspose = 1;
            TIDLPCLayers.layerParams.innerProductParams.isBias = 1;
            TIDLPCLayers.layerParams.innerProductParams.constIdx = -1;

            if(TIDLPCLayerstemp.bias.ptr == NULL) {
                TIDLPCLayers.bias.ptr = calloc(TIDLPCLayers.layerParams.innerProductParams.numOutCols, sizeof(float));
                TIDLPCLayers.bias.bufSize = TIDLPCLayers.layerParams.innerProductParams.numOutCols;
            }
          }
        }
      }
    }
  }
  return 0;
}

int32_t tidl_convertSqueezeToFlattenLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i1, i2, i3, i4;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_SqueezeLayer)
    {
      sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure.TIDLPCLayers[i1];
      if( (TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH] == 1) &&
          (TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT] == 1) &&
          (TIDLPCLayers.layerPCParams.squeezeParams.axis[TIDL_DIM_WIDTH]  == 1) &&
          (TIDLPCLayers.layerPCParams.squeezeParams.axis[TIDL_DIM_HEIGHT] == 1)
      )
      {
        TIDLPCLayers.layerType = TIDL_FlattenLayer;
        TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_BATCH];
        TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM1] = 1;
        TIDLPCLayers.outData[0].dimValues[TIDL_DIM_DIM2] = 1;
        TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
        TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] = 1;
        TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH] = TIDLPCLayers.inData[0].dimValues[TIDL_DIM_NUMCH] *
          TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT] *
          TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH];
        TIDLPCLayers.numMacs =
          (int64_t)((int64_t)TIDLPCLayers.outData[0].dimValues[TIDL_DIM_BATCH] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH] *
            TIDLPCLayers.outData[0].dimValues[TIDL_DIM_HEIGHT] * TIDLPCLayers.outData[0].dimValues[TIDL_DIM_WIDTH]);
        int32_t outIdx = tidl_getOutLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].dataId);
        if (outIdx != -1)
        {
          pOrgTIDLNetStructure.TIDLPCLayers[outIdx].inData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0];
        }
      }
    }
  }
  return 0;
}

int32_t tidl_convertPoolToIdentityLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i1, i2, i3, i4;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_PoolingLayer)
    {
      sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure.TIDLPCLayers[i1];
      sTIDL_PoolingParams_t *poolParams = &TIDLPCLayers.layerParams.poolParams;
      if( (TIDLPCLayers.inData[0].dimValues[TIDL_DIM_WIDTH] == 1) &&
          (TIDLPCLayers.inData[0].dimValues[TIDL_DIM_HEIGHT] == 1)  &&
          (poolParams->kernelW == 0) && (poolParams->kernelH == 0)
      )
      {
        TIDLPCLayers.layerType = TIDL_IdentityLayer;
      }
    }
  }
  return 0;
}


int32_t tidl_convertRelUToBNLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i1, i2, i3, i4;
  for (i1 = 0; i1 < layerIndex; i1++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_ReLULayer ||
        pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_PReLULayer ||
        pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_SigmoidLayer ||
        pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_TanhLayer ||
        pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_HardSigmoidLayer ||
        pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_ELULayer ||
        pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_ClipLayer ||
        pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_LeakyReluLayer)

    {
      sTIDL_LayerPC_t &TIDLPCLayers = pOrgTIDLNetStructure.TIDLPCLayers[i1];

      TIDLPCLayers.actParams = pOrgTIDLNetStructure.TIDLPCLayers[i1].actParams;
      int32_t dataSize = TIDLPCLayers.outData[0].dimValues[TIDL_DIM_NUMCH];

      TIDLPCLayers.weights.ptr = my_malloc(dataSize*sizeof(float));
      TIDLPCLayers.weights.bufSize = dataSize;
      TIDLPCLayers.bias.ptr = my_malloc(dataSize*sizeof(float));
      TIDLPCLayers.bias.bufSize = dataSize;
      float * scalePtr = (float*)TIDLPCLayers.weights.ptr;
      float * biasPtr  = (float*)TIDLPCLayers.bias.ptr;
      float *dataSlope = NULL;

      if(pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_LeakyReluLayer)
      {
        dataSlope = (float *)my_malloc(dataSize*sizeof(float));
        TIDLPCLayers.slope.ptr = dataSlope;
        TIDLPCLayers.slope.bufSize = dataSize;
        TIDLPCLayers.actParams.actType = TIDL_PRelU;
      }

      if(pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_HardSigmoidLayer)
      {
        TIDLPCLayers.layerParams.batchNormParams.inDataQ = (int32_t)(TIDLPCLayers.layerPCParams.hardSigmoidParams.alpha*(TIDL_NON_LIN_PARAM_SCALE));
        TIDLPCLayers.layerParams.batchNormParams.weightsQ = (int32_t)(TIDLPCLayers.layerPCParams.hardSigmoidParams.beta*(TIDL_NON_LIN_PARAM_SCALE));
        TIDLPCLayers.actParams.actType = TIDL_HardSigmoid;
      }
      else if(pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_ELULayer)
      {
        TIDLPCLayers.layerParams.batchNormParams.inDataQ = (int32_t)(TIDLPCLayers.layerPCParams.eluParams.alpha*(TIDL_NON_LIN_PARAM_SCALE));
        TIDLPCLayers.actParams.actType = TIDL_ELU;
      }
      else if(pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_SigmoidLayer)
      {
        TIDLPCLayers.actParams.actType = TIDL_Sigmoid;
      }
      else if(pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_TanhLayer)
      {
        TIDLPCLayers.actParams.actType = TIDL_Tanh;
      }

      for (i2 = 0; i2 < dataSize; i2++)
      {
        scalePtr[i2] = 1;
        biasPtr[i2]  = 0;
        if(pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_LeakyReluLayer)
        {
          dataSlope[i2] = TIDLPCLayers.layerPCParams.leakyReluParams.alpha;
        }
      }
      pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType = TIDL_BatchNormLayer;
    }
  }
  return 0;
}

int32_t tidl_copyPCNetToDeviceNet(sTIDL_OrgNetwork_t  * pOrgTIDLNetStructure,
                                                                  sTIDL_Network_t  * pTIDLNetStructure,
                                                                  tidl_import_config * configParams,
                                                                  int32_t numLayers)
{
  int32_t i, j;
  int64_t                    totalMacs = 0;
  int32_t tiLayerIndex = 0;

  pTIDLNetStructure->dataElementSize = (configParams->numFeatureBits + 7)>>3; /* Convert to number of bytes */
  pTIDLNetStructure->biasElementSize = (configParams->numParamBits <= 16) ? 2 : 4;
  pTIDLNetStructure->weightsElementSize = ((configParams->numParamBits - 1) / 8 + 1);
  pTIDLNetStructure->slopeElementSize = pTIDLNetStructure->weightsElementSize;
  pTIDLNetStructure->interElementSize = 4;
  pTIDLNetStructure->quantizationStyle = configParams->quantizationStyle;
  pTIDLNetStructure->calibrationOption  = configParams->calibrationOption;
  pTIDLNetStructure->calibrationParams.activationRangeMethod = configParams->activationRangeMethod;
  pTIDLNetStructure->calibrationParams.weightRangeMethod     = configParams->weightRangeMethod;
  pTIDLNetStructure->calibrationParams.percentileActRangeShrink = configParams->percentileActRangeShrink;
  pTIDLNetStructure->calibrationParams.percentileWtRangeShrink = configParams->percentileWtRangeShrink;
  pTIDLNetStructure->calibrationParams.biasCalibrationFactor = configParams->biasCalibrationFactor;
  pTIDLNetStructure->calibrationParams.biasCalibrationIterations = configParams->biasCalibrationIterations;
  pTIDLNetStructure->netVersion = TIDL_NET_VERSION;

  for (i = 0; i < numLayers; i++)
  {
    if (pOrgTIDLNetStructure->TIDLPCLayers[i].layerType != TIDL_UnsupportedLayer)
    {
      pTIDLNetStructure->TIDLLayers[tiLayerIndex].layerType = pOrgTIDLNetStructure->TIDLPCLayers[i].layerType;
      pTIDLNetStructure->TIDLLayers[tiLayerIndex].layerKernelType = pOrgTIDLNetStructure->TIDLPCLayers[i].layerKernelType;
      pTIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams = pOrgTIDLNetStructure->TIDLPCLayers[i].layerParams;
      pTIDLNetStructure->TIDLLayers[tiLayerIndex].actParams  = pOrgTIDLNetStructure->TIDLPCLayers[i].actParams;
      pTIDLNetStructure->TIDLLayers[tiLayerIndex].numInBufs = pOrgTIDLNetStructure->TIDLPCLayers[i].numInBufs;
      pTIDLNetStructure->TIDLLayers[tiLayerIndex].numOutBufs = pOrgTIDLNetStructure->TIDLPCLayers[i].numOutBufs;
      pTIDLNetStructure->TIDLLayers[tiLayerIndex].weightsElementSizeInBits = pOrgTIDLNetStructure->TIDLPCLayers[i].weightsElementSizeInBits;
      pTIDLNetStructure->TIDLLayers[tiLayerIndex].strideOffsetMethod = pOrgTIDLNetStructure->TIDLPCLayers[i].strideOffsetMethod;
      pTIDLNetStructure->TIDLLayers[tiLayerIndex].multiCoreMode = pOrgTIDLNetStructure->TIDLPCLayers[i].multiCoreMode;
      pTIDLNetStructure->TIDLLayers[tiLayerIndex].scratchMemRequired = pOrgTIDLNetStructure->TIDLPCLayers[i].scratchMemRequired;
      if(pTIDLNetStructure->TIDLLayers[tiLayerIndex].layerType == TIDL_ConvolutionLayer)
      {
        if(TIDL_HighPrecisionKernel == pOrgTIDLNetStructure->TIDLPCLayers[i].layerKernelType)
        {
          /*Set PSAT Values for high precision convolution consumption*/
          pTIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.convParams.minPSAT = pOrgTIDLNetStructure->TIDLPCLayers[i].minPSAT;
          pTIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.convParams.maxPSAT = pOrgTIDLNetStructure->TIDLPCLayers[i].maxPSAT;
        }
        else
        {
          /*Undefined/Unused quantities for high throughput kernels:*/
          pTIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.convParams.minPSAT = -1;
          pTIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.convParams.maxPSAT = -1;
        }
      }

      if (pTIDLNetStructure->TIDLLayers[tiLayerIndex].layerType == TIDL_DataLayer)
      {
        pTIDLNetStructure->TIDLLayers[tiLayerIndex].layersGroupId = 0;
        if(pTIDLNetStructure->TIDLLayers[tiLayerIndex].numOutBufs == -1) {
          pTIDLNetStructure->TIDLLayers[tiLayerIndex].outData.dataId = 0;
        }
      }
      else
      {
        pTIDLNetStructure->TIDLLayers[tiLayerIndex].coreID = 1;
        pTIDLNetStructure->TIDLLayers[tiLayerIndex].layersGroupId = 1;
      }

      for (j = 0; j < pOrgTIDLNetStructure->TIDLPCLayers[i].numInBufs; j++)
      {
        pTIDLNetStructure->TIDLLayers[tiLayerIndex].inData[j] = pOrgTIDLNetStructure->TIDLPCLayers[i].inData[j].dataId;
      }

      for (j = 0; j < pOrgTIDLNetStructure->TIDLPCLayers[i].numOutBufs; j++)
      {
        pTIDLNetStructure->TIDLLayers[tiLayerIndex].outData = pOrgTIDLNetStructure->TIDLPCLayers[i].outData[j];

        /* Make sure all the quantization scales are set to 1.0 for float mode */
        if ( configParams->numParamBits == 32 )
        {
          pTIDLNetStructure->TIDLLayers[tiLayerIndex].actParams.slopeScale = 1.0;
          pTIDLNetStructure->TIDLLayers[tiLayerIndex].outData.tensorScale = 1.0;
          if ((pOrgTIDLNetStructure->TIDLPCLayers[i].layerType == TIDL_ConvolutionLayer) ||
          (pOrgTIDLNetStructure->TIDLPCLayers[i].layerType == TIDL_Deconv2DLayer))
          {
            pTIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.convParams.weightScale = 1.0;
            pTIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.convParams.biasScale     = 1.0;
          }
          else if (pOrgTIDLNetStructure->TIDLPCLayers[i].layerType == TIDL_InnerProductLayer)
          {
            pTIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.innerProductParams.weightScale = 1.0;
            pTIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.innerProductParams.biasScale= 1.0;
          }
          else if (pOrgTIDLNetStructure->TIDLPCLayers[i].layerType == TIDL_BatchNormLayer)
          {
            pTIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.batchNormParams.weightScale = 1.0;
            pTIDLNetStructure->TIDLLayers[tiLayerIndex].layerParams.batchNormParams.biasScale= 1.0;

          }

        }
      }
      totalMacs += pOrgTIDLNetStructure->TIDLPCLayers[i].numMacs;
      tiLayerIndex++;
    }
  }

  char filenameStr[1000];
  sprintf(filenameStr, "%s_netLog.txt", configParams->outputNetFile);
  FILE * nfp = fopen(filenameStr, "w+");
  if(nfp)
  {
    fprintf(nfp,"Num of Layer Detected : %3d \n", numLayers);
    fprintf(nfp,"--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    fprintf(nfp,"%5s|%-30s|%-50s|%-6s|%-6s|%-6s|%-32s|%-10s|%-36s|%-36s|%-11s|\n", "Num", "TIDL Layer Name", "Out Data Name", "Group", "#Ins", "#Outs", "Inbuf Ids", "Outbuf Id", "In NCHW", "Out NCHW", "MACS");
    fprintf(nfp,"--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    int32_t layerIndexCount = 0;
    for (i = 0; i < numLayers; i++)
    {
      if (pOrgTIDLNetStructure->TIDLPCLayers[i].layerType != TIDL_UnsupportedLayer)
      {
        fprintf(nfp,"%5d|%-30s|", layerIndexCount++, TIDL_LayerString[pOrgTIDLNetStructure->TIDLPCLayers[i].layerType], pOrgTIDLNetStructure->TIDLPCLayers[i].outDataNames[0]);
        if (strlen((const char *)pOrgTIDLNetStructure->TIDLPCLayers[i].outDataNames[0]) > 50)
        {
          fprintf(nfp,"%-50s|", &pOrgTIDLNetStructure->TIDLPCLayers[i].outDataNames[0][strlen((const char *)pOrgTIDLNetStructure->TIDLPCLayers[i].outDataNames[0]) - 50]);
        }
        else
        {
          fprintf(nfp,"%-50s|", pOrgTIDLNetStructure->TIDLPCLayers[i].outDataNames[0]);
        }

        fprintf(nfp,"%6d|%6d|%6d|", pTIDLNetStructure->TIDLLayers[tiLayerIndex].layersGroupId, pOrgTIDLNetStructure->TIDLPCLayers[i].numInBufs, pOrgTIDLNetStructure->TIDLPCLayers[i].numOutBufs);

        for (j = 0; j < pOrgTIDLNetStructure->TIDLPCLayers[i].numInBufs; j++)
        {
          fprintf(nfp,"%3d ", pOrgTIDLNetStructure->TIDLPCLayers[i].inData[j].dataId);
        }
        for (j = (pOrgTIDLNetStructure->TIDLPCLayers[i].numInBufs > 0 ? pOrgTIDLNetStructure->TIDLPCLayers[i].numInBufs : 0); j < 8; j++)
        {
          fprintf(nfp,"  x ");
        }
        fprintf(nfp,"|");
        fprintf(nfp,"%3d ", pOrgTIDLNetStructure->TIDLPCLayers[i].outData[0].dataId);
        fprintf(nfp,"      |");
        for (j = 0; j < TIDL_DIM_MAX; j++)
        {
          fprintf(nfp,"%8d ", pOrgTIDLNetStructure->TIDLPCLayers[i].inData[0].dimValues[j]);
        }
        fprintf(nfp,"|");

        for (j = 0; j < TIDL_DIM_MAX; j++)
        {
          fprintf(nfp,"%8d ", pOrgTIDLNetStructure->TIDLPCLayers[i].outData[0].dimValues[j]);
        }
        fprintf(nfp,"|");
        fprintf(nfp,"%10lld |", pOrgTIDLNetStructure->TIDLPCLayers[i].numMacs);
        fprintf(nfp,"\n");
      }
    }
    fprintf(nfp,"--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    fprintf(nfp,"Total Giga Macs : %4.4f\n", ((float)totalMacs / 1000000000));
    fprintf(nfp,"--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    fclose(nfp);
  }
  if(configParams->debugTraceLevel > 0)
  {
    nfp = fopen(filenameStr, "r");
    if(nfp)
    {
      while(fgets(filenameStr, sizeof(filenameStr),nfp))
      {
        printf("%s", filenameStr);
      }
    }
  }
  return tiLayerIndex;
}
int32_t tidl_addOutDataLayer(sTIDL_Network_t  * tIDLNetStructure, int32_t tiLayerIndex)
{
  int32_t i, j, addOneLayer = 0;

  tIDLNetStructure->TIDLLayers[tiLayerIndex].layerType = TIDL_DataLayer;
  tIDLNetStructure->TIDLLayers[tiLayerIndex].layersGroupId = 0;
  tIDLNetStructure->TIDLLayers[tiLayerIndex].outData.dataId = 0;
  tIDLNetStructure->TIDLLayers[tiLayerIndex].numInBufs = 0;
  tIDLNetStructure->TIDLLayers[tiLayerIndex].numOutBufs = -1;
  tIDLNetStructure->TIDLLayers[tiLayerIndex].coreID = 255;

  for (i = 0; i < tiLayerIndex; i++)
  {
    if (tIDLNetStructure->TIDLLayers[i].layerType != TIDL_DataLayer)
    {
      for (j = 0; j < tIDLNetStructure->TIDLLayers[i].numOutBufs; j++)
      {
        if (!TIDL_isDataBufUsed(tIDLNetStructure->TIDLLayers[i].outData.dataId,
                                            tIDLNetStructure,
                                            tiLayerIndex))
        {
          tIDLNetStructure->TIDLLayers[tiLayerIndex].inData[tIDLNetStructure->TIDLLayers[tiLayerIndex].numInBufs] = tIDLNetStructure->TIDLLayers[i].outData.dataId;
          tIDLNetStructure->TIDLLayers[tiLayerIndex].numInBufs++;
          addOneLayer = 1;
        }
      }
    }
  }
  tIDLNetStructure->numLayers = tiLayerIndex + addOneLayer;
  return 0;
}

// Split single outDataLayer with multi-data to multiple outDataLayers
// each with single data.  Otherwise, perfsim assumes all data co-existing,
// which caused perfsim to crash for mxnet_fcn_resnet101_voc network.
int32_t tidl_splitOutDataLayer(sTIDL_OrgNetwork_t &orgTIDLNetStructure,
                                 int32_t &layerIndex)
{
  int32_t i, j;

  layerIndex = orgTIDLNetStructure.numLayers;
  for (i = 0; i < orgTIDLNetStructure.numLayers; i++)
  {
    sTIDL_LayerPC_t& layer_i = orgTIDLNetStructure.TIDLPCLayers[i];
    if (layer_i.layerType != TIDL_DataLayer || layer_i.numOutBufs != -1)
      continue;
    // Note j start from 1, only rewrite from second data as new outDataLayers
    for (j = 1; j < layer_i.numInBufs; j++)
    {
      sTIDL_LayerPC_t& layer_n = orgTIDLNetStructure.TIDLPCLayers[layerIndex++];
      memset(&layer_n, 0, sizeof(sTIDL_LayerPC_t));
      layer_n.layerType  = TIDL_DataLayer;
      layer_n.numInBufs = 1;
      layer_n.numOutBufs = -1;
      layer_n.inData[0].dataId = layer_i.inData[j].dataId;
      memcpy(layer_n.inDataNames[0], layer_i.inDataNames[j], TIDL_STRING_SIZE);
      memcpy(layer_n.outDataNames[0],layer_n.inDataNames[0], TIDL_STRING_SIZE);
      layer_n.outConsumerCnt[0] = 1;
      layer_n.outConsumerLinked[0] = 1;
    }
    layer_i.numInBufs = 1;
  }
  orgTIDLNetStructure.numLayers = layerIndex;
  return 0;
}

template <class Tin>
int32_t tidl_convertNHWCtoNCHW(Tin* dst, Tin* src, int32_t n, int32_t c, int32_t h, int32_t w)
{
    int32_t i0 , i1, i2, i3;
    float out;
    for (i0 = 0; i0 < n; i0++){
        for (i1 = 0; i1 < c; i1++)
        {
            for (i2 = 0; i2 < h; i2++)
            {
                for (i3 = 0; i3 < w; i3++)
                {
                    out = src[i0 * c * h * w + i1 + i2*w*c + i3*c];
                    dst[ i0 * c * h * w + i1 * h * w + i2 * w + i3] = (Tin)out;
                }
            }
        }
    }
    return 0;
}

bool TIDL_isProducerInitializerDequantize(sTIDL_OrgNetwork_t&  pOrgTIDLNetStructure, int32_t outDataId)
{
  int32_t consumerLayer = tidl_getInLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, outDataId);
  sTIDL_LayerPC_t& layer = pOrgTIDLNetStructure.TIDLPCLayers[consumerLayer];
  if(layer.layerType == TIDL_DequantizeLayer && layer.allowlistingMetaData.numVarInputs == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

int32_t tidl_addConstDataLayers (sTIDL_OrgNetwork_t&  pOrgTIDLNetStructure, int32_t layerIndex, int32_t * dataIndex, tidl_import_config* configParams)
{
  for (int32_t i=0;i<layerIndex;i++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_EltWiseLayer ||
        pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_ConcatLayer  ||
        pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_GatherLayer  ||
        pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_InnerProductLayer)
    {
      sTIDL_LayerPC_t& layer = pOrgTIDLNetStructure.TIDLPCLayers[i];

      sTIDL_allowlistingMetaData md = layer.allowlistingMetaData;
      if (md.numConstInputs == 0 || (layer.layerType == TIDL_InnerProductLayer && layer.numInBufs < 2))
      {
        continue;
      }

      if (layer.weights.ptr != NULL)
      {
        sTIDL_allowlistingMetaData md = layer.allowlistingMetaData;
        sTIDL_LayerPC_t& constLayer = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex];
        constLayer.layerType = TIDL_ConstDataLayer;
        constLayer.numInBufs  = -1;
        constLayer.numOutBufs = 1;
        constLayer.outData[0].dataId = (*dataIndex)++;
        constLayer.outConsumerCnt[0] = 1;
        constLayer.outConsumerLinked[0] = 0;

        int32_t constSize = 1, varSize = 1;
        for (int32_t k =0;k<md.constTensorsDims[0].size();k++)
        {
          constSize *= md.constTensorsDims[0][k];
        }
        for (int32_t k = 0;k<md.varTensorsDims[0].size();k++)
        {
          varSize *= md.varTensorsDims[0][k];
        }
        int32_t broadcastFactor = varSize/constSize;
        int32_t offset = 0, i1=0;

        constLayer.weights.ptr = (float *)my_malloc(layer.weights.bufSize*sizeof(float));
        memcpy (constLayer.weights.ptr, layer.weights.ptr, layer.weights.bufSize*sizeof(float));

        constLayer.weights.bufSize = layer.weights.bufSize;

        /*Free the layer's weights pointer as they are now copied to const layer weights:*/
        if(layer.weights.ptr)
        {
          free(layer.weights.ptr);
          layer.weights.ptr = NULL;
          layer.weights.bufSize = 0;
        }

        std::vector<int32_t> Dims;
        Dims = md.constTensorsDims[0];

        if (Dims.size() < TIDL_DIM_MAX)
        {
          int32_t remDim = TIDL_DIM_MAX - Dims.size(), i1=0, dim=0;
          for (i1=0;i1<remDim;i1++)
          {
            constLayer.outData[0].dimValues[dim++] = 1;
          }
          for (i1=0;i1<Dims.size();i1++)
          {
            constLayer.outData[0].dimValues[dim++] = Dims[i1];
          }
        }
        else if (Dims.size() == TIDL_DIM_MAX)
        {
          for (int32_t i1=0;i1<TIDL_DIM_MAX;i1++)
          {
            constLayer.outData[0].dimValues[i1] = Dims[i1];
          }
        }
        else
        {
          printf ("Unsupported number of dimensions for ConstLayer\n");
        }

        if (configParams->modelType == TIDL_IMPORT_MODEL_FORMAT_TFLITE || configParams->modelType == TIDL_IMPORT_MODEL_FORMAT_TFLITE_RT)
        {
          float* dst = (float *)my_malloc(constLayer.weights.bufSize*sizeof(float));
          if(TIDL_QuantStyleAsymNP2_TFL == configParams->quantizationStyle)
          {
            tidl_convertNHWCtoNCHW((uint8_t*)dst, (uint8_t*)constLayer.weights.ptr, constLayer.outData[0].dimValues[TIDL_DIM_BATCH], constLayer.outData[0].dimValues[TIDL_DIM_NUMCH], constLayer.outData[0].dimValues[TIDL_DIM_HEIGHT],
              constLayer.outData[0].dimValues[TIDL_DIM_WIDTH]);
          }
          else
          {
            tidl_convertNHWCtoNCHW((float*)dst, (float*)constLayer.weights.ptr, constLayer.outData[0].dimValues[TIDL_DIM_BATCH], constLayer.outData[0].dimValues[TIDL_DIM_NUMCH], constLayer.outData[0].dimValues[TIDL_DIM_HEIGHT],
              constLayer.outData[0].dimValues[TIDL_DIM_WIDTH]);
          }
          constLayer.weights.ptr = dst;
        }

        constLayer.outData[0].elementType = tidl_getElementType(1);

        if(pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_GatherLayer)
        {
          constLayer.outData[0].elementType = TIDL_SignedWord;
        }

        if(TIDL_QuantStyleAsymNP2_TFL == configParams->quantizationStyle)
        {
          constLayer.outData[0].tensorScale = *((float*)layer.weightScales.ptr);
          constLayer.outData[0].tensorZeroPoint = *((int*)layer.weightZeroPoints.ptr);
        }

        for (int32_t j=0;j<layer.numInBufs;j++)
        {
          if (layer.inData[j].dataId == -1 || TIDL_isProducerInitializerDequantize(pOrgTIDLNetStructure, layer.inData[j].dataId))
          {
            char append[10];
            sprintf(append, "_%d", layerIndex);
            strcat((char*)layer.inDataNames[j], append);
            strcpy((char*)constLayer.outDataNames[0], (char*)layer.inDataNames[j]);
            tidl_linkOutputTensors(&pOrgTIDLNetStructure, layerIndex);
            break;
          }
        }
        layerIndex++;
      }
    }
    else if (pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_ScatterElementsLayer)
    {
        sTIDL_LayerPC_t& scatterLayer = pOrgTIDLNetStructure.TIDLPCLayers[i];
        sTIDL_allowlistingMetaData md = scatterLayer.allowlistingMetaData;
        int32_t numConst = md.numConstInputs, input=0;
        void* constPtr = NULL;
        for (int32_t i=0;i<numConst;i++)
        {
          sTIDL_LayerPC_t& constLayer = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex];
          constLayer.layerType = TIDL_ConstDataLayer;
          constLayer.numInBufs  = -1;
          constLayer.numOutBufs = 1;
          constLayer.outData[0].dataId = (*dataIndex)++;
          constLayer.outConsumerCnt[0] = 1;
          constLayer.outConsumerLinked[0] = 0;
          sBuffer_t constData;
          if (i == 0) constData = scatterLayer.weights;
          else constData = scatterLayer.bias;
          for (int32_t j=0;j<md.numInputs;j++)
          {
            if (strcmp((const char*)scatterLayer.inDataNames[j], md.constTensorNames[i].c_str()) == 0)
            {
              input = j;
              break;
            }
          }
          if (input == 1)
          {
            //indices - 64-bit
            constLayer.weights.ptr = (int32_t *)my_malloc(constData.bufSize*sizeof(int32_t));
            constLayer.weights.bufSize = constData.bufSize;
            constLayer.outData[0].elementType = TIDL_SignedWord;
            std::copy((long long int*)constData.ptr, ((long long int*)constData.ptr + constData.bufSize), (int32_t*)constLayer.weights.ptr);
          }
          else
          {
            //inData, updates
            constLayer.weights.ptr = (float *)my_malloc(constData.bufSize*sizeof(float));
            memcpy (constLayer.weights.ptr, constData.ptr, constData.bufSize*sizeof(float));
            constLayer.weights.bufSize = constData.bufSize;
            constLayer.outData[0].elementType = tidl_getElementType(1);
          }
          int32_t numDim = md.constTensorsDims[i].size();
          int32_t remDim = 4-numDim;
          for (int k=0;k<remDim;k++)
            constLayer.outData[0].dimValues[k] = 1;
          for (int k=0;k<numDim;k++)
          {
            constLayer.outData[0].dimValues[k+remDim] = md.constTensorsDims[i][k];
          }
          strcpy((char*)constLayer.outDataNames[0], (char*)scatterLayer.inDataNames[input]);
          tidl_linkOutputTensors(&pOrgTIDLNetStructure, layerIndex);
          layerIndex++;
        }
    }
  }
  pOrgTIDLNetStructure.numLayers = layerIndex;
  return 0;
}

int32_t tidl_addInDataLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex, int32_t * dataIndex)
{
  int32_t i, j;
  int32_t idx;

  for (i = 0; i < layerIndex; i++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i].layerType != TIDL_DataLayer)
    {
      for (j = 0; j < pOrgTIDLNetStructure.TIDLPCLayers[i].numInBufs; j++)
      {

        idx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i].inData[j].dataId);
        if (idx == -1)
        {
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType = TIDL_DataLayer;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs  = -1;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = 1;
          strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].inDataNames[j]);
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dataId = (*dataIndex)++;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = \
            tidl_getConsumerCount(&pOrgTIDLNetStructure, layerIndex, (char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0]);
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = 0;
          tidl_linkOutputTensors(&pOrgTIDLNetStructure, layerIndex);
          layerIndex++;
        }
      }
    }
  }
  pOrgTIDLNetStructure.numLayers = layerIndex;
  return 0;
}

int32_t tidl_addNormLayerToInData(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex, int32_t * dataIndex, tidl_import_config * params)
{
  int32_t i, j;
  int32_t idx;
  int32_t normParamIdx = 0;
  for (i = 0; i < layerIndex; i++)
  {
    if ((pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_DataLayer) && (pOrgTIDLNetStructure.TIDLPCLayers[i].numOutBufs > 0))
    {
//      if (pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_NUMCH] != 1024)//:TODO: Temp change for RCNN
      {
        int32_t dataSize;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType = TIDL_BatchNormLayer;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = pOrgTIDLNetStructure.TIDLPCLayers[i].numOutBufs;
        pOrgTIDLNetStructure.TIDLPCLayers[i].numOutBufs = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weightsElementSizeInBits = NUM_WHGT_BITS;

        strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0]);
        strcat((char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0], "_original");

        strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0]);

        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0];
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = pOrgTIDLNetStructure.TIDLPCLayers[i].outConsumerCnt[0];
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = pOrgTIDLNetStructure.TIDLPCLayers[i].outConsumerLinked[0];

        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0];

        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].elementType = tidl_getElementType(1);
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].actParams.actType = TIDL_NoAct;

        pOrgTIDLNetStructure.TIDLPCLayers[i].outConsumerCnt[0] = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outConsumerLinked[0] = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dataId = (*dataIndex)++;

        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0];

        dataSize = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH];

        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weights.ptr = my_malloc(dataSize*sizeof(float));
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weights.bufSize = dataSize;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].bias.ptr = my_malloc(dataSize*sizeof(float));
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].bias.bufSize = dataSize;
        float *  bias     = (float *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].bias.ptr;
        float *  weights  = (float *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weights.ptr;
        for (j = 0; j < dataSize; j++)
        {
          if (normParamIdx >= TIDL_MAX_ALG_IN_BUFS*TIDL_IN_NUF_MAX_CH)
          {
            printf(" normParamIdx >= TIDL_MAX_ALG_IN_BUFS*TIDL_IN_NUF_MAX_CH \n ");
            exit(0);
          }
          weights[j] = params->inScale[normParamIdx];
          bias[j]    = - (params->inMean[normParamIdx]* weights[j]);
          normParamIdx++;
        }
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numMacs =
          (int64_t)((int64_t)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_BATCH] * pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH] *
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] * pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] * 4);

        TIDL_UpdateInDataBuff(&pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0]);

        layerIndex++;
      }
    }
  }
  pOrgTIDLNetStructure.numLayers = layerIndex;
  return 0;
}

static int32_t tidl_isPadChangePossible(const sTIDL_LayerPC_t * pLayer)

{
  int retVal = 0;

  if (pLayer->layerType == TIDL_ArgMaxLayer)
  {
    retVal = 1;
  }
  else if (pLayer->layerType == TIDL_SoftMaxLayer)
  {
    retVal = 1;
  }
  else if (pLayer->layerType == TIDL_DetectionOutputLayer)
  {
    retVal = 1;
  }
  else if ((pLayer->layerType == TIDL_OdOutputReformatLayer) ||
          (pLayer->layerType == TIDL_OdPostProcessingLayer) )
  {
    retVal = 1;
  }

  return(retVal);
}


static int32_t tidl_doesOutputNeedsDataConvert(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure,
                                                       const tidl_import_config * params,
                                                       int32_t bufIdx,
                                                       int32_t currLayerIdx,
                                                       int32_t numLayers)
{
  int32_t needsDataConvert = 1;

  int32_t producerLayerIdx = tidl_getInLayer(pOrgTIDLNetStructure,
                                             numLayers,
                                             pOrgTIDLNetStructure.TIDLPCLayers[currLayerIdx].inData[0].dataId);

  /* Data Convert layer will not be required if the following conditions are met
  1) Output Layout is same as TIDL layout which is NCHW or Num Channels == 1 and hence layout conversion is not required
  2) There is not extra padding generated by TIDL so no padding removal is required. This is possible
     for layers which can change padding
  3) Output element type is same as TIDL output element type
  4) Output tensor scale is same as producers tensor scale and there is no zero point in the output buffer
  */
  if ( ( params->outLayout[bufIdx] == TIDL_LT_NCHW) ||
    (pOrgTIDLNetStructure.TIDLPCLayers[producerLayerIdx].outData[0].dimValues[TIDL_DIM_NUMCH] == 1 ))
  {
    if ( tidl_isPadChangePossible(&pOrgTIDLNetStructure.TIDLPCLayers[producerLayerIdx]) == 1)
    {
      if (params->outElementType[bufIdx] == pOrgTIDLNetStructure.TIDLPCLayers[producerLayerIdx].outData[0].elementType)
      {
        if ((params->outTensorScale[bufIdx] == 1.0) &&
            (params->outZeroPoint[bufIdx]  == 0.0 ) )
        {
          needsDataConvert  = 0;
        }
        else if(pOrgTIDLNetStructure.TIDLPCLayers[producerLayerIdx].outData[0].elementType == TIDL_SinglePrecFloat )
        {
          /*Float to float convert is not supported*/
          needsDataConvert = 0;
        }
      }
    }
  }

  /* Argmax layer either needs uint8 to int32 or uint8 to int64. both are not optimal now.
     if both input type== output type then conversion is not needed So
     When we have optimal implementation for uint8 to int32 and  uint8 to int64, then this condition can be removed
  */
  if((pOrgTIDLNetStructure.TIDLPCLayers[producerLayerIdx].layerType == TIDL_ArgMaxLayer) &&
      (gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_ONNX_RT))
  {
    needsDataConvert  = 0;
  }

  return needsDataConvert;

}

int32_t tidl_isInDataConvertTypeSupported(int32_t inElementType, int32_t outElementType, int32_t inZeroPoint, float inQuantFactor)
{
  int32_t ret = 1;
  //:TODO: Check for more un-supported combos
  if (( ( tidl_getElementSizeInBits(inElementType) == 32 ) ||
        ( tidl_getElementSizeInBits(inElementType) == 64 ) ) &&
        (inElementType != TIDL_SinglePrecFloat) )
  {
    ret = 0;
  }

  return ret;
}

int32_t tidl_isOutDataConvertTypeSupported(int32_t inElementType, int32_t outElementType, int32_t outZeroPoint, float outQuantFactor)
{
 int32_t ret = 1;

 //:TODO: Check for more un-supported combos
 if ( (( tidl_getElementSizeInBits(outElementType) == 32 ) ||
        ( tidl_getElementSizeInBits(outElementType) == 64 )) && (outElementType != TIDL_SinglePrecFloat))
  {
    if ((outQuantFactor != 1.0) || (outZeroPoint != 0 ) )
    {
      ret = 0;
    }
  }

#if 0
  if ((outElementType != TIDL_SignedWord) && (outElementType != TIDL_SignedWord) && (outElementType <= TIDL_SinglePrecFloat))
  {
    if(outElementType == TIDL_SinglePrecFloat)
    {
        ret = 1;
    }
    else if((outElementType == TIDL_UnsignedChar)  && (inElementType == outElementType))
    {
        ret = 1;
    }
  }
#endif
  return ret;
}


int32_t tidl_addDataConvertLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex, int32_t * dataIndex, tidl_import_config * params)
{
  int32_t i, j;
  int32_t idx;
  int32_t inParamIdx = 0;
  int32_t outParamIdx = 0;

  for (i = 0; i < layerIndex; i++)
  {
    if ((pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_DataLayer) &&
        (pOrgTIDLNetStructure.TIDLPCLayers[i].numOutBufs == 1) &&
         (gParams.addDataConvertToNet & ADD_DC_LAYER_AT_INPUT))
    {
      int32_t dataSize, outElementType;
      if((params->inElementType[inParamIdx] == TIDL_SinglePrecFloat) ||
        (params->inElementType[inParamIdx] == TIDL_SignedChar) ||
        (params->inElementType[inParamIdx] == TIDL_SignedShort))
      {
        outElementType = tidl_getElementType(1);
      }
      else
      {
        if(params->inZeroPoint[inParamIdx] == 0)
        {
          outElementType = tidl_getElementType(0);
        }
        else
        {
          outElementType = tidl_getElementType(1);
        }
      }

      int32_t outLayerdx = tidl_getOutLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dataId);
      if (outLayerdx == -1)
        return -1;
      sTIDL_LayerPC_t &inputDataConsumerLayer = pOrgTIDLNetStructure.TIDLPCLayers[outLayerdx];
      if(((inputDataConsumerLayer.layerType == TIDL_ScatterElementsLayer || inputDataConsumerLayer.layerType == TIDL_GatherLayer)
          && (params->inElementType[inParamIdx] == TIDL_SignedWord) && (params->numParamBits != 32)))
      {
        /** For scatter layer, the int32 indices should be passed as int32 itself to the network in all cases except float mode(numParamBits = 32)
         * For float mode, indices need to be converted from int32 to float32 */
        outElementType = TIDL_SignedWord;
      }

      if(tidl_isInDataConvertTypeSupported(params->inElementType[inParamIdx], outElementType, params->inZeroPoint[inParamIdx], params->inQuantFactor[inParamIdx]))
      {
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType  = TIDL_DataConvertLayer;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs  = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weightsElementSizeInBits = NUM_WHGT_BITS;

        strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0]);
        strcat((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], "_netFormat");

        strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0]);

        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0];
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = pOrgTIDLNetStructure.TIDLPCLayers[i].outConsumerCnt[0];
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = pOrgTIDLNetStructure.TIDLPCLayers[i].outConsumerLinked[0];

        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].elementType = outElementType;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].actParams.actType = TIDL_NoAct;

        pOrgTIDLNetStructure.TIDLPCLayers[i].outConsumerCnt[0] = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outConsumerLinked[0] = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dataId = (*dataIndex)++;
        if(!gParams.preQuantizedModel)/*Not QDQ*/
        {
          pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].tensorScale = params->inQuantFactor[inParamIdx];
        }

        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0];
        if(gParams.preQuantizedModel)
        {
          pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].elementType = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0].elementType = params->inElementType[inParamIdx];
          //Updates:
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0].tensorZeroPoint = 0;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].tensorZeroPoint = 0;
        }
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.dataConvertParams.layout    = params->inLayout[inParamIdx];
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.dataConvertParams.inZeroPoint = params->inZeroPoint[inParamIdx];
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.dataConvertParams.type = TIDL_DC_TYPE_INPUT;

        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numMacs =
          (int64_t)((int64_t)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_BATCH] * pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH] *
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] * pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] * 4);

        TIDL_UpdateInDataBuff(&pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0]);
        TIDL_UpdateInDataName(&pOrgTIDLNetStructure, layerIndex,
        (char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0],
        (char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0]);
        layerIndex++;
      }
      else
      {
        params->inLayout[inParamIdx]    = 0;
        params->inZeroPoint[inParamIdx] = 0;
        printf("Warning : Requested Input Data Convert Layer is not Added to the network, It is currently not Optimal\n");
      }
      inParamIdx++;
    }

    else if ((pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_DataLayer) &&
             (pOrgTIDLNetStructure.TIDLPCLayers[i].numInBufs > 0) &&
             (gParams.addDataConvertToNet & ADD_DC_LAYER_AT_OUTPUT))
    {
      int32_t doesOutpuNeedDataConvert;

      /* After many topo sorts, there is no guarantee that output data layers
         are in the same order as they are originally created.  For TVM Relay
         input, use the output index encoded in the name, e.g. tidl_0_o1 */
      if (gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TVM_RELAY)
      {
        char *o_name = (char*) pOrgTIDLNetStructure.TIDLPCLayers[i].inDataNames[0];
        int pos = strlen(o_name);
        while (o_name[pos-1] != 'o') pos--;
        outParamIdx = atoi(&o_name[pos]);
      }

      doesOutpuNeedDataConvert = tidl_doesOutputNeedsDataConvert(pOrgTIDLNetStructure,
                                                                 params,
                                                                 outParamIdx,
                                                                 i,
                                                                 layerIndex);
      /* Add data convert layer only if required */
      int32_t inIdx;
      inIdx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dataId);
      if(doesOutpuNeedDataConvert &&
        tidl_isOutDataConvertTypeSupported(pOrgTIDLNetStructure.TIDLPCLayers[inIdx].outData[0].elementType, params->outElementType[outParamIdx],
                                            params->outZeroPoint[outParamIdx], params->outTensorScale[outParamIdx]))
      {

        for (j = 0; j < pOrgTIDLNetStructure.TIDLPCLayers[i].numInBufs ; j++)
        {
          int32_t dataSize;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType  = TIDL_DataConvertLayer;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs  = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weightsElementSizeInBits = NUM_WHGT_BITS;

          inIdx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i].inData[j].dataId);
          strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[inIdx].outDataNames[0]);
          strcat((char *)pOrgTIDLNetStructure.TIDLPCLayers[inIdx].outDataNames[0], "_netFormat");
          strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[inIdx].outDataNames[0]);

          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0] = pOrgTIDLNetStructure.TIDLPCLayers[inIdx].outData[0];
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0]    = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = 1;

          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0];

          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].elementType = params->outElementType[outParamIdx];
          /* Default value of tensor scale is zero, if the user given value is anything other than default then it indicates user wants
          to specify output tensor scale. */
          if ( params->outTensorScale[outParamIdx] != 0.0 )
          {
            if(1)/*Not QDQ:*/
            {
              pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].tensorScale = params->outTensorScale[outParamIdx];
            }
          }
          else
          {
            /* this is irrelevant as this will be re-computed as part of output scale computation */
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].tensorScale = 1.0;
          }
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].actParams.actType = TIDL_NoAct;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.dataConvertParams.layout    = params->outLayout[outParamIdx];
          if(1)
          {
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.dataConvertParams.outZeroPoint = params->outZeroPoint[outParamIdx];
          }
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.dataConvertParams.type = TIDL_DC_TYPE_OUTPUT;


          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dataId = (*dataIndex)++;

          pOrgTIDLNetStructure.TIDLPCLayers[i].inData[j] = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0];
          strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[i].inDataNames[j], (char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0]);

          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numMacs =
            (int64_t)((int64_t)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_BATCH] * pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH] *
              pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] * pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] * 4);
          layerIndex++;
        }
      }
      else
      {
        params->outElementType[outParamIdx] = -1;
        params->outLayout[outParamIdx]      = 0;
        params->outZeroPoint[outParamIdx]   = 0;
        printf("Warning : Requested Output Data Convert Layer is not Added to the network, It is currently not Optimal\n");
      }
      outParamIdx++;
    }
  }
  pOrgTIDLNetStructure.numLayers = layerIndex;
  return 0;
}

static void tidl_printOpsList_dup(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure)
{
  printf("printing Current net\n");

  for (int i = 0; i < pOrgTIDLNetStructure.numLayers; i++)
  {
    printf("%5d|%-30s|%-50s|%-50s|%-50s|\n", i,
    TIDL_LayerString[pOrgTIDLNetStructure.TIDLPCLayers[i].layerType],
    pOrgTIDLNetStructure.TIDLPCLayers[i].name,
    pOrgTIDLNetStructure.TIDLPCLayers[i].inDataNames[0],
    pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0]);
  }

}

static int32_t tidl_checkForDCLayerForOTF(int32_t inputLayerType, int32_t outputLayerType, unordered_set<int32_t>& LayerList)
{
  if((LayerList.find(inputLayerType) != LayerList.end()) && (LayerList.find(outputLayerType) == LayerList.end()))
  {
    return TIDL_DC_FOR_PAD_REMOVAL; //Remove Pad
  }
  if((LayerList.find(inputLayerType) == LayerList.end()) && (LayerList.find(outputLayerType) != LayerList.end()))
  {
    return TIDL_DC_FOR_PAD_ADDITION; //Add Pad
  }
  return TIDL_NO_DC;
}

static int32_t tidl_checkLayerForOTFSupport(int32_t inputLayerType)
{
  unordered_set<int32_t> unsupportedLayersForOTF = { TIDL_Deconv2DLayer};
  return (unsupportedLayersForOTF.find(inputLayerType) != unsupportedLayersForOTF.end());
}

int32_t TIDL_addLayer(sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t layerIdx, int32_t newLayerType, int32_t* dataIndex, int32_t numLayers, int32_t opType)
{
  /*Returns number of layers Added*/
  int32_t layersAdded = 0;
  sTIDL_LayerPC_t& layer = pOrgTIDLNetStructure.TIDLPCLayers[layerIdx];

  if (opType == ADD_LAYER_BEFORE)
  {
    /* This block adds a newLayerType layer before layer with index 'layerIdx' , outdatashape is not updated, update if required, call topologicalsort after addition*/

    for (int32_t j=0;j<layer.numInBufs;j++)
    {
      int32_t inIdx = tidl_getInLayer(pOrgTIDLNetStructure, numLayers, pOrgTIDLNetStructure.TIDLPCLayers[layerIdx].inData[j].dataId);
      sTIDL_LayerPC_t& inLayer = pOrgTIDLNetStructure.TIDLPCLayers[inIdx];
      sTIDL_LayerPC_t& newLayer = pOrgTIDLNetStructure.TIDLPCLayers[numLayers];
      newLayer.layerType  = newLayerType;
      newLayer.numInBufs  = 1;
      newLayer.numOutBufs = 1;
      newLayer.weightsElementSizeInBits = NUM_WHGT_BITS;
      char append[TIDL_APPEND_NAME];
      sprintf(append, "_%d", addedLayers++);
      strcpy((char *)newLayer.outDataNames[0], (char *)inLayer.outDataNames[0]);
      strcat((char *)inLayer.outDataNames[j], append);
      strcpy((char *)newLayer.inDataNames[0], (char *)inLayer.outDataNames[0]);

      newLayer.inData[0] = inLayer.outData[0];
      newLayer.outConsumerCnt[0]    = 1;
      newLayer.outConsumerLinked[0] = 1;

      newLayer.outData[0] = newLayer.inData[0];

      newLayer.outData[0].elementType = inLayer.outData[0].elementType;

      newLayer.outData[0].dataId = (*dataIndex)++;

      layer.inData[j] = newLayer.outData[0];
      strcpy((char *)layer.inDataNames[0], (char *)newLayer.outDataNames[0]);

      newLayer.numMacs =
      (int64_t)((int64_t)newLayer.outData[0].dimValues[TIDL_DIM_BATCH] * newLayer.outData[0].dimValues[TIDL_DIM_DIM1] *newLayer.outData[0].dimValues[TIDL_DIM_DIM2]* newLayer.outData[0].dimValues[TIDL_DIM_NUMCH] *
        newLayer.outData[0].dimValues[TIDL_DIM_HEIGHT] * newLayer.outData[0].dimValues[TIDL_DIM_WIDTH]);
      TIDL_UpdateInDataBuff(&pOrgTIDLNetStructure, numLayers, newLayer.outData[0]);

      numLayers++;
      layersAdded++;
    }
  }
  else if (opType == ADD_LAYER_AFTER)
  {
  /* This block adds a newLayerType layer after layer with index 'layerIdx' , outdatashape is not updated, update if required, call topologicalsort after addition*/

    for (int32_t j=0;j<layer.numOutBufs;j++)
    {
      sTIDL_LayerPC_t& newLayer = pOrgTIDLNetStructure.TIDLPCLayers[numLayers];

      newLayer.layerType = newLayerType;
      newLayer.numInBufs = 1;
      newLayer.numOutBufs = 1;
      newLayer.weightsElementSizeInBits = NUM_WHGT_BITS;
      char append[TIDL_APPEND_NAME];
      sprintf(append, "_%d", addedLayers++);
      strcpy((char*)newLayer.outDataNames[0], (char*)layer.outDataNames[j]);
      strcpy((char*)newLayer.inDataNames[0], (char*)layer.outDataNames[j]);
      strcat((char*)newLayer.outDataNames[0], append);

      newLayer.outData[0] = layer.outData[j];
      newLayer.outConsumerCnt[0] = layer.outConsumerCnt[j];
      newLayer.outConsumerLinked[0] = layer.outConsumerLinked[j];

      newLayer.outData[0].elementType = layer.outData[j].elementType;
      newLayer.actParams.actType = TIDL_NoAct;

      layer.outConsumerCnt[0] = 1;
      layer.outConsumerLinked[0] = 1;
      layer.outData[j].dataId = (*dataIndex)++;

      newLayer.inData[0] = layer.outData[j];

        newLayer.numMacs =
        (int64_t)((int64_t)newLayer.outData[0].dimValues[TIDL_DIM_BATCH] * newLayer.outData[0].dimValues[TIDL_DIM_DIM1] *newLayer.outData[0].dimValues[TIDL_DIM_DIM2]* newLayer.outData[0].dimValues[TIDL_DIM_NUMCH] *
          newLayer.outData[0].dimValues[TIDL_DIM_HEIGHT] * newLayer.outData[0].dimValues[TIDL_DIM_WIDTH]);

      TIDL_UpdateInDataBuff(&pOrgTIDLNetStructure, numLayers, newLayer.outData[0]);
      TIDL_UpdateInDataName(&pOrgTIDLNetStructure, numLayers,
      (char *)newLayer.inDataNames[0],
      (char *)newLayer.outDataNames[0]);

      numLayers++;
      layersAdded++;
    }
  }
  pOrgTIDLNetStructure.numLayers = numLayers;

  return layersAdded;
}

int32_t TIDL_find_index(int32_t *perm, int32_t val, int32_t size)
{
  int32_t index = -1;
  for (int32_t i=0;i<size;i++)
  {
    if (perm[i] == val)
    {
      index = i;
      break;
    }
  }
  return index;
}

int32_t TIDL_removeTransposeFromNet(sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t numLayers)
{
  int32_t layerRemoved = 0;
  for (int32_t i=0;i<numLayers;i++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_TransposeLayer)
    {
      /** After dividing the transpose into two, the second transpose can be nop in some cases, check and remove if that is the case */
      int32_t removeTranspose = 1;
      for (int32_t j=0;j<TIDL_DIM_MAX;j++)
      {
        if(pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[j] != pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[j])
        {
          removeTranspose = 0;
        }
      }

      if (removeTranspose == 1)
      {

        // int32_t outLayerIdx = tidl_getOutLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dataId);

        std::vector<int32_t> out_layers =  tidl_getOutLayers(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dataId);

        for ( int32_t outLayerIdx: out_layers)
        {
          for(int j=0;j<pOrgTIDLNetStructure.TIDLPCLayers[outLayerIdx].numInBufs;j++)
          {
            if(pOrgTIDLNetStructure.TIDLPCLayers[outLayerIdx].inData[j].dataId == pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dataId)
            {
              strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[outLayerIdx].inDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].inDataNames[0]);
              pOrgTIDLNetStructure.TIDLPCLayers[outLayerIdx].inData[j].dataId = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dataId;
            }
          }

          // int32_t outLayerIdx = tidl_getOutLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dataId);

        }

        layerRemoved = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].numInBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].numOutBufs = -1;
      }
    }
  }
  return layerRemoved;
}

int32_t TIDL_convertTransposetoReshape(sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t numLayers)
{
  for (int32_t i=0;i<numLayers;i++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_TransposeLayer)
    {
      if (pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_WIDTH] == pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_WIDTH])
      {
        int32_t notOnes=0, Dims=TIDL_DIM_MAX-1;

        for (int32_t j=0;j<Dims;j++)
        {
          if (pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[j] != 1)
          {
            notOnes++;
          }
        }
        if (notOnes == 1)
        {
          /** If only one dimension is other than one, transpose is redundant, change it to Reshape*/
          pOrgTIDLNetStructure.TIDLPCLayers[i].layerType = TIDL_ReshapeLayer;
        }
      }
    }
  }
  return 0;
}

int32_t factorDimensions(int32_t& d1, int32_t& d2, int32_t totDim)
{
  d1 = 1;
  d2 = totDim;
  for (int32_t i=1;i<=sqrt(totDim);i++)
  {
    if (totDim % i == 0)
    {
      d1 = i;
      d2 = totDim/i;
    }
  }
  return 0;
}

int32_t TIDL_convertTransposeToDataConvert(sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t* dataIndex, int32_t numLayers)
{
  int32_t layerAdded = 0;
  for (int32_t i=0;i<numLayers;i++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_TransposeLayer)
    {
      if (pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.transposeParams.isDataConvertNeeded == 1)
      {
        int32_t* cur_perm = (int32_t*)malloc(sizeof(int32_t)*TIDL_DIM_MAX);
        memcpy(cur_perm, pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.transposeParams.perm, sizeof(int32_t)*TIDL_DIM_MAX);

        pOrgTIDLNetStructure.TIDLPCLayers[i].layerType = TIDL_DataConvertLayer;
        pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.dataConvertParams.type = TIDL_DC_TYPE_INTERMEDIATE;
        pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.dataConvertParams.inZeroPoint = 0;
        pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.dataConvertParams.outZeroPoint = 0;
        /*This value is set again during import_quantize,
          setting it to -1 to distinguish this from quantize dataconvert in tflite asymmetric quantization*/
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].roundBits = -1;

        #if 0

        int32_t lastDim = cur_perm[TIDL_DIM_WIDTH], A=1, B=1, A_count = lastDim, C, H, W;
        int32_t B_count = TIDL_DIM_MAX - A_count - 1;

        for (int32_t j=1;j<=lastDim;j++)
        {
          A *= pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[j];
        }
        for (int32_t j=lastDim+1;j<TIDL_DIM_MAX;j++)
        {
          B *= pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[j];
        }

        TIDL_addLayer (pOrgTIDLNetStructure, i, TIDL_ReshapeLayer, dataIndex, pOrgTIDLNetStructure.numLayers, ADD_LAYER_BEFORE);
        sTIDL_LayerPC_t& beforeReshape = pOrgTIDLNetStructure.TIDLPCLayers[pOrgTIDLNetStructure.numLayers-1];
        memcpy(beforeReshape.inData[0].dimValues, pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues, sizeof(int32_t) * TIDL_DIM_MAX);

        TIDL_addLayer  (pOrgTIDLNetStructure, i, TIDL_ReshapeLayer, dataIndex, pOrgTIDLNetStructure.numLayers, ADD_LAYER_AFTER);
        sTIDL_LayerPC_t& afterReshape = pOrgTIDLNetStructure.TIDLPCLayers[pOrgTIDLNetStructure.numLayers-1];
        memcpy(afterReshape.outData[0].dimValues, pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues, sizeof(int32_t) * TIDL_DIM_MAX);

        if (A >= B)
        {
          /** Using HWC->CHW dataconvert*/
          pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.dataConvertParams.layout = TIDL_LT_NHWC;
          pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.dataConvertParams.outLayout = TIDL_LT_NCHW;

          C=B, H=1,W=1;
          factorDimensions(H,W,A);
          /*if (A_count == 1)
          {
            H = 1;
            W = A;
          }
          else
          {
            factorDimensions(H,W,A);
          }*/
        }
        else
        {
          /** Using CHW->HWC dataconvert*/
          pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.dataConvertParams.layout = TIDL_LT_NCHW;
          pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.dataConvertParams.outLayout = TIDL_LT_NHWC;

          C=A, H=1, W=1;
          factorDimensions(H,W,B);
          /*
          if (B_count == 1)
          {
            H = 1;
            W = B;
          }
          else
          {
            factorDimensions(H,W,B);
          }
          */
        }

        beforeReshape.outData[0].dimValues[TIDL_DIM_BATCH]  = 1;
        beforeReshape.outData[0].dimValues[TIDL_DIM_DIM1]   = 1;
        beforeReshape.outData[0].dimValues[TIDL_DIM_DIM2]   = 1;
        beforeReshape.outData[0].dimValues[TIDL_DIM_NUMCH]  = C;
        beforeReshape.outData[0].dimValues[TIDL_DIM_HEIGHT] = H;
        beforeReshape.outData[0].dimValues[TIDL_DIM_WIDTH]  = W;

        memcpy(pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues, beforeReshape.outData[0].dimValues, sizeof(int32_t) * TIDL_DIM_MAX);
        memcpy(pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues, beforeReshape.outData[0].dimValues, sizeof(int32_t) * TIDL_DIM_MAX);
        memcpy(afterReshape.inData[0].dimValues, beforeReshape.outData[0].dimValues, sizeof(int32_t) * TIDL_DIM_MAX);
       #else
        if (cur_perm[TIDL_DIM_WIDTH] == TIDL_DIM_NUMCH)
        {
          pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.dataConvertParams.layout = TIDL_LT_NCHW;
          pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.dataConvertParams.outLayout = TIDL_LT_NHWC;
          TIDL_addLayer(pOrgTIDLNetStructure, i, TIDL_ReshapeLayer, dataIndex, pOrgTIDLNetStructure.numLayers, ADD_LAYER_AFTER);
          for (int32_t k=0;k<TIDL_DIM_MAX;k++)
          {
            //pOrgTIDLNetStructure.TIDLPCLayers[numLayers].inData[0].dimValues[k]  = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[k];
            pOrgTIDLNetStructure.TIDLPCLayers[pOrgTIDLNetStructure.numLayers-1].outData[0].dimValues[k] = pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[k];
            pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[k]         = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[k];
          }

        }
        else if (cur_perm[TIDL_DIM_WIDTH] == TIDL_DIM_HEIGHT)
        {
          pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.dataConvertParams.layout = TIDL_LT_NHWC;
          pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.dataConvertParams.outLayout = TIDL_LT_NCHW;
          TIDL_addLayer(pOrgTIDLNetStructure, i, TIDL_ReshapeLayer, dataIndex, pOrgTIDLNetStructure.numLayers, ADD_LAYER_BEFORE);

          for (int32_t k=0;k<TIDL_DIM_MAX;k++)
          {
            pOrgTIDLNetStructure.TIDLPCLayers[pOrgTIDLNetStructure.numLayers-1].outData[0].dimValues[k] = pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[k];
            pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[k]                                 = pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[k];
          }

        }
        else if (cur_perm[TIDL_DIM_WIDTH] == TIDL_DIM_BATCH)
        {
          pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.dataConvertParams.layout = TIDL_LT_NCHW;
          pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.dataConvertParams.outLayout = TIDL_LT_NHWC;
          TIDL_addLayer(pOrgTIDLNetStructure, i, TIDL_ReshapeLayer, dataIndex, pOrgTIDLNetStructure.numLayers, ADD_LAYER_BEFORE);
          TIDL_addLayer(pOrgTIDLNetStructure, i, TIDL_ReshapeLayer, dataIndex, pOrgTIDLNetStructure.numLayers, ADD_LAYER_AFTER);
          sTIDL_LayerPC_t& beforeReshape = pOrgTIDLNetStructure.TIDLPCLayers[pOrgTIDLNetStructure.numLayers-2];
          sTIDL_LayerPC_t& afterReshape = pOrgTIDLNetStructure.TIDLPCLayers[pOrgTIDLNetStructure.numLayers-1];

          beforeReshape.outData[0].dimValues[TIDL_DIM_BATCH] = 1;
          beforeReshape.outData[0].dimValues[TIDL_DIM_DIM1] = 1;
          beforeReshape.outData[0].dimValues[TIDL_DIM_DIM2] = 1;
          beforeReshape.outData[0].dimValues[TIDL_DIM_NUMCH] = beforeReshape.inData[0].dimValues[TIDL_DIM_BATCH];
          beforeReshape.outData[0].dimValues[TIDL_DIM_HEIGHT] = beforeReshape.inData[0].dimValues[TIDL_DIM_NUMCH] * beforeReshape.inData[0].dimValues[TIDL_DIM_HEIGHT];
          beforeReshape.outData[0].dimValues[TIDL_DIM_WIDTH] = beforeReshape.inData[0].dimValues[TIDL_DIM_WIDTH];

          memcpy(pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues, beforeReshape.outData[0].dimValues, sizeof(int32_t)*4);
          memcpy(pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues, beforeReshape.outData[0].dimValues, sizeof(int32_t)*4);
          memcpy(afterReshape.inData[0].dimValues, beforeReshape.outData[0].dimValues, sizeof(int32_t)*4);

          afterReshape.outData[0].dimValues[TIDL_DIM_BATCH] = beforeReshape.inData[0].dimValues[TIDL_DIM_NUMCH];
          afterReshape.outData[0].dimValues[TIDL_DIM_DIM1] = 1;
          afterReshape.outData[0].dimValues[TIDL_DIM_DIM2] = 1;
          afterReshape.outData[0].dimValues[TIDL_DIM_NUMCH] = beforeReshape.inData[0].dimValues[TIDL_DIM_HEIGHT];
          afterReshape.outData[0].dimValues[TIDL_DIM_HEIGHT] = beforeReshape.inData[0].dimValues[TIDL_DIM_WIDTH];
          afterReshape.outData[0].dimValues[TIDL_DIM_WIDTH] = beforeReshape.inData[0].dimValues[TIDL_DIM_BATCH];

        }
        #endif
        layerAdded = 1;
      }
      else if (pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.transposeParams.isDataConvertNeeded == 0)
      {

      }
    }
  }
  return layerAdded;
}

int32_t tidl_addTransposeLayer(sTIDL_OrgNetwork_t &orgTIDLNetStructure, int32_t i, int32_t perm[], int32_t *dataIndex, int32_t addDirection )
{
  int32_t j;
  TIDL_addLayer(orgTIDLNetStructure, i, TIDL_TransposeLayer, dataIndex, orgTIDLNetStructure.numLayers, addDirection);
  tidl_configureTransposeLayer (&orgTIDLNetStructure.TIDLPCLayers[orgTIDLNetStructure.numLayers - 1], perm);
  return 0;
}

int32_t tidl_optimizeSoftmaxAxis(sTIDL_OrgNetwork_t &orgTIDLNetStructure, int32_t* dataIndex, int32_t numLayers)
{
  int32_t i;
  for (i=0;i<numLayers;i++)
  {
    if (orgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_SoftMaxLayer)
    {
        if(!TIDL_isOutTensorFlat(&orgTIDLNetStructure, i))
      {
        if(orgTIDLNetStructure.TIDLPCLayers[i].layerParams.softMaxParams.axis == TIDL_DIM_WIDTH)
        {
          /*Switch to a more SIMD friendly axis by introducing transpose:*/
          int32_t perm1[TIDL_DIM_MAX] = {0,1,2,3,5,4};
          tidl_addTransposeLayer(orgTIDLNetStructure, i, perm1, dataIndex, ADD_LAYER_BEFORE);
          /**Update the output of input transpose & input of softmax*/
          TIDL_tfOutReshapeTransposeLayer(&orgTIDLNetStructure, orgTIDLNetStructure.numLayers - 1);

          memcpy (orgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues,
                  orgTIDLNetStructure.TIDLPCLayers[orgTIDLNetStructure.numLayers - 1].outData[0].dimValues, sizeof(int32_t) * TIDL_DIM_MAX);

          /**Set the outtranspose flag of softmax layer*/
          orgTIDLNetStructure.TIDLPCLayers[i].layerParams.softMaxParams.outTranspose = 1;

        }
        /*Update softmax's axis:*/
        orgTIDLNetStructure.TIDLPCLayers[i].layerParams.softMaxParams.axis = TIDL_DIM_HEIGHT;
      }
    }
}
  return 0;
}

int32_t tidl_checkGlobalAveragePoolonNonImageData (sTIDL_LayerPC_t& layer)
{
  int32_t status = 0;

  /** This function checks if the global average pool is on non-image data*/

  if (layer.layerType == TIDL_PoolingLayer &&
      layer.layerParams.poolParams.poolingType == TIDL_AveragePooling &&
      layer.layerParams.poolParams.kernelH == 0 && layer.layerParams.poolParams.kernelW == 0 &&
      layer.layerParams.poolParams.avgDims != 2)
  {
    status = 1;
  }

  return status;
}

int32_t tidl_optimizeGlobalAveragePoolingLayers (sTIDL_OrgNetwork_t &orgTIDLNetStructure, int32_t* dataIndex, int32_t numLayers)
{
  int32_t status = 0;
  int32_t i1 = 0, channelDim;

  for (i1 = 0; i1 < orgTIDLNetStructure.numLayers; i1++)
  {
    sTIDL_LayerPC_t& layer = orgTIDLNetStructure.TIDLPCLayers[i1];
    if (tidl_checkGlobalAveragePoolonNonImageData (layer) == 1)
    {
      /** Global average pooling on non image data*/

      if (layer.layerParams.poolParams.avgDims == 1)
      {
        /** NxCxD1 -> Global average pool on only D1 dimension*/

        TIDL_addLayer (orgTIDLNetStructure, i1, TIDL_ReshapeLayer, dataIndex, orgTIDLNetStructure.numLayers, ADD_LAYER_BEFORE);
        TIDL_addLayer (orgTIDLNetStructure, i1, TIDL_ReshapeLayer, dataIndex, orgTIDLNetStructure.numLayers, ADD_LAYER_AFTER);

        sTIDL_LayerPC_t& beforeReshape = orgTIDLNetStructure.TIDLPCLayers[orgTIDLNetStructure.numLayers-2];
        sTIDL_LayerPC_t& afterReshape  = orgTIDLNetStructure.TIDLPCLayers[orgTIDLNetStructure.numLayers-1];

        memcpy (beforeReshape.outData[0].dimValues, beforeReshape.inData[0].dimValues, sizeof (int32_t) * TIDL_DIM_MAX);
        memcpy (afterReshape.outData[0].dimValues, afterReshape.inData[0].dimValues, sizeof (int32_t) * TIDL_DIM_MAX);

        beforeReshape.outData[0].dimValues[TIDL_DIM_DIM2]  = beforeReshape.inData[0].dimValues[TIDL_DIM_NUMCH];
        beforeReshape.outData[0].dimValues[TIDL_DIM_NUMCH]  = beforeReshape.inData[0].dimValues[TIDL_DIM_HEIGHT];
        beforeReshape.outData[0].dimValues[TIDL_DIM_HEIGHT] = 1;

        memcpy (layer.inData[0].dimValues, beforeReshape.outData[0].dimValues, sizeof (int32_t) * TIDL_DIM_MAX);

        TIDL_tfOutReshapePoolingLayer (&orgTIDLNetStructure, i1);

        afterReshape.outData[0].dimValues[TIDL_DIM_NUMCH] = layer.outData[0].dimValues[TIDL_DIM_DIM2];
        afterReshape.outData[0].dimValues[TIDL_DIM_HEIGHT] = layer.outData[0].dimValues[TIDL_DIM_NUMCH];

        afterReshape.layerPCParams.reshapeParams.isInduced = 1;
        beforeReshape.layerPCParams.reshapeParams.isInduced = 1;

      }

      else if (layer.layerParams.poolParams.avgDims > 2)
      {
        /** Need to handle this case*/
      }
    }
  }
  return status;
}

int32_t tidl_mergeNoOpReshapeLayers (sTIDL_OrgNetwork_t &orgTIDLNetStructure, int32_t numLayers)
{
  int32_t i1 = 0, i2 = 0, i3 = 0, removeLayer = 1, outIdx = 0, inIdx = 0;
  int32_t status = 0;

  for (i1 = 0; i1 < numLayers; i1++)
  {
    sTIDL_LayerPC_t& layer = orgTIDLNetStructure.TIDLPCLayers[i1];

    removeLayer = 1;

    if (layer.layerType == TIDL_ReshapeLayer)
    {
      /** Check if the in & out shapes of reshape layer are same
       * Remove if that's the case, such cases can come due different optimizations across optimizeNet
      */

      for (i2 = 0; i2 < TIDL_DIM_MAX; i2++)
      {
        if (layer.inData[0].dimValues[i2] != layer.outData[0].dimValues[i2])
        {
          removeLayer = 0;
        }
      }

      if (removeLayer)
      {
        layer.numInBufs = -1;
        layer.numOutBufs = -1;

        inIdx = tidl_getInLayer(orgTIDLNetStructure, numLayers, layer.inData[0].dataId);
        outIdx = tidl_getOutLayer(orgTIDLNetStructure, numLayers, layer.outData[0].dataId);

        if (outIdx != -1 && inIdx != -1)
        {
          sTIDL_LayerPC_t *TIDLPCLayersOut, *TIDLPCLayersIn;
          TIDLPCLayersIn = &orgTIDLNetStructure.TIDLPCLayers[inIdx];
          TIDLPCLayersOut = &orgTIDLNetStructure.TIDLPCLayers[outIdx];

          for (i3 = 0; i3 < TIDLPCLayersOut->numInBufs; i3++)
          {
            if (TIDLPCLayersOut->inData[i3].dataId == layer.outData[0].dataId)
            {
              TIDLPCLayersOut->inData[i3].dataId = TIDLPCLayersIn->outData[0].dataId;
              strcpy((char *)TIDLPCLayersOut->inDataNames[i3], (char *)TIDLPCLayersIn->outDataNames[0]);
            }
          }
        }
      }
    }
  }
  return status;
}

int32_t tidl_optimizeSqueezeLayers (sTIDL_OrgNetwork_t &orgTIDLNetStructure, int32_t* dataIndex, int32_t numLayers)
{
  int32_t status = 0;
  int32_t i1 = 0, i2 = 0;

  for (i1 = 0; i1 < orgTIDLNetStructure.numLayers; i1++)
  {
    if (orgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_SqueezeLayer)
    {
      sTIDL_LayerPC_t& squeezeLyr = orgTIDLNetStructure.TIDLPCLayers[i1];

      if (squeezeLyr.layerPCParams.squeezeParams.axis[TIDL_DIM_HEIGHT] == 1 || squeezeLyr.layerPCParams.squeezeParams.axis[TIDL_DIM_WIDTH] == 1)
      {
        /** Convert the layer to reshape layer as pad requirements needs to be handled correctly*/
        squeezeLyr.layerType = TIDL_ReshapeLayer;
        squeezeLyr.layerPCParams.reshapeParams.isInduced = 1;
        status = 1;
      }

      else
      {
        /**We can remove the squeeze layer, higher dimension squeeze is a no op */
        int32_t outLayerIdx = tidl_getOutLayer(orgTIDLNetStructure, orgTIDLNetStructure.numLayers, squeezeLyr.outData[0].dataId);

        for (int j = 0;j < orgTIDLNetStructure.TIDLPCLayers[outLayerIdx].numInBufs; j++)
        {
          if(orgTIDLNetStructure.TIDLPCLayers[outLayerIdx].inData[j].dataId == squeezeLyr.outData[0].dataId)
          {
            strcpy((char *)orgTIDLNetStructure.TIDLPCLayers[outLayerIdx].inDataNames[j], (char *)squeezeLyr.inDataNames[0]);
            orgTIDLNetStructure.TIDLPCLayers[outLayerIdx].inData[j].dataId = squeezeLyr.inData[0].dataId;
          }
        }

        status = 1;
        squeezeLyr.numInBufs = -1;
        squeezeLyr.numOutBufs = -1;
      }
    }
  }
  return status;
}

int32_t TIDL_handlePadforTransposeLayers (sTIDL_OrgNetwork_t &orgTIDLNetStructure, int32_t* dataIndex, int32_t numLayers)
{
  int32_t status = 0;
  int32_t i1 = 0, i2 = 0;

  for (i1 = 0; i1 < numLayers; i1++)
  {
    if (orgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_TransposeLayer)
    {
      /** Add Dataconvert layer after transpose to remove/add the pad before & after the transpose */
      TIDL_addLayer(orgTIDLNetStructure, i1, TIDL_DataConvertLayer, dataIndex, orgTIDLNetStructure.numLayers, ADD_LAYER_BEFORE);
      TIDL_addLayer(orgTIDLNetStructure, i1, TIDL_DataConvertLayer, dataIndex, orgTIDLNetStructure.numLayers, ADD_LAYER_AFTER);

      sTIDL_LayerPC_t& beforeDataConvert = orgTIDLNetStructure.TIDLPCLayers[orgTIDLNetStructure.numLayers - 2];
      sTIDL_LayerPC_t& afterDataConvert  = orgTIDLNetStructure.TIDLPCLayers[orgTIDLNetStructure.numLayers - 1];

      beforeDataConvert.layerParams.dataConvertParams.type = TIDL_DC_TYPE_INTERMEDIATE;
      beforeDataConvert.layerParams.dataConvertParams.inZeroPoint = 0;
      beforeDataConvert.layerParams.dataConvertParams.outZeroPoint = 0;

      afterDataConvert.layerParams.dataConvertParams.type = TIDL_DC_TYPE_INTERMEDIATE;
      afterDataConvert.layerParams.dataConvertParams.inZeroPoint = 0;
      afterDataConvert.layerParams.dataConvertParams.outZeroPoint = 0;

      /*This value is set again during import_quantize,
        setting it to -1 to distinguish this from quantize dataconvert in tflite asymmetric quantization*/
      beforeDataConvert.outData[0].roundBits = -1;
      afterDataConvert.outData[0].roundBits  = -1;

            status = 1;
    }
  }
  return status;
}

int32_t tidl_optimizeTransposeLayers(sTIDL_OrgNetwork_t &orgTIDLNetStructure, int32_t* dataIndex, int32_t numLayers)
{
  /** 
   * Transpose layers cannot handle pad, this function removes the pad in the input of transpose & also adds pad in the output of transpose 
   * with the help of data convert layers
  */
  int32_t isPadAdded = TIDL_handlePadforTransposeLayers(orgTIDLNetStructure, dataIndex, orgTIDLNetStructure.numLayers);
  if (isPadAdded == 1)
  {
   tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);    
  }

  /**
   * This function splits a transpose into two if the last dimension (Ex: 'W' in NCHW) of the input is changing its position because of
   * tranpose 
  */
  int32_t isTransposeAdded = TIDL_handleTransposeLayers(orgTIDLNetStructure, dataIndex, orgTIDLNetStructure.numLayers);
  if (isTransposeAdded == 1)
  {
   tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);
  }

  /**
   * This function converts the transpose into dataconvert (uses the help of (NCHW <-> NHWC) layout change dataconvert)
  */
  int32_t isLayerAdded = TIDL_convertTransposeToDataConvert(orgTIDLNetStructure, dataIndex, orgTIDLNetStructure.numLayers);
  if (isLayerAdded == 1)
  {
   tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);
  }

  /**
   * This function removes any duplicate transpose if formed due to above optimizations
  */
  int32_t removeTranspose = TIDL_removeTransposeFromNet(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);
  if (removeTranspose == 1)
  {
    tidl_removeMergedLayersFromNet(&orgTIDLNetStructure, &tempTIDLNetStructure);
    tidl_sortLayersInProcOrder(&orgTIDLNetStructure, &tempTIDLNetStructure, orgTIDLNetStructure.numLayers);
  }

  /**
   * This function converts a transpose into reshape if there is no data movement in the transpose (Ex: 1x100 -> 100x1)
  */
  TIDL_convertTransposetoReshape(orgTIDLNetStructure, orgTIDLNetStructure.numLayers);

  return 0;
}

int32_t TIDL_handleTransposeLayers(sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t* dataIndex, int32_t numLayers)
{
  int32_t layerAdded  = 0;
  for (int32_t i=0;i<numLayers;i++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_TransposeLayer)
    {
      int32_t *perm = pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.transposeParams.perm;

      if (perm[TIDL_DIM_WIDTH] != TIDL_DIM_WIDTH)
      {
        int32_t temp;
        TIDL_addLayer(pOrgTIDLNetStructure, i, TIDL_TransposeLayer, dataIndex, pOrgTIDLNetStructure.numLayers, ADD_LAYER_BEFORE);
        sTIDL_LayerPC_t& newLayer = pOrgTIDLNetStructure.TIDLPCLayers[pOrgTIDLNetStructure.numLayers - 1];
        sTIDL_LayerPC_t& curLayer = pOrgTIDLNetStructure.TIDLPCLayers[i];

        int32_t* new_perm = (int32_t*)malloc(sizeof(int32_t)*TIDL_DIM_MAX);
        int32_t* cur_perm = (int32_t*)malloc(sizeof(int32_t)*TIDL_DIM_MAX);
        for (int32_t j=0;j<TIDL_DIM_MAX;j++)
        {
          new_perm[j] = j;
        }
        #if 1
        if (perm[TIDL_DIM_WIDTH] == TIDL_DIM_NUMCH)
        {
          new_perm[TIDL_DIM_NUMCH]  = TIDL_DIM_HEIGHT;
          new_perm[TIDL_DIM_HEIGHT] = TIDL_DIM_WIDTH;
          new_perm[TIDL_DIM_WIDTH]  = TIDL_DIM_NUMCH;
        }
        else if (perm[TIDL_DIM_WIDTH] == TIDL_DIM_HEIGHT)
        {
          new_perm[TIDL_DIM_NUMCH]  = TIDL_DIM_WIDTH;
          new_perm[TIDL_DIM_HEIGHT] = TIDL_DIM_NUMCH;
          new_perm[TIDL_DIM_WIDTH]  = TIDL_DIM_HEIGHT;
        }
        else if (perm[TIDL_DIM_WIDTH] == TIDL_DIM_BATCH)
        {
          new_perm[TIDL_DIM_BATCH]  = TIDL_DIM_NUMCH;
          new_perm[TIDL_DIM_NUMCH]  = TIDL_DIM_HEIGHT;
          new_perm[TIDL_DIM_HEIGHT] = TIDL_DIM_WIDTH;
          new_perm[TIDL_DIM_WIDTH]  = TIDL_DIM_BATCH;
        }
        #else
        int32_t lastDim = perm[TIDL_DIM_WIDTH], dim=1;

        for (int32_t i = lastDim+1; i<TIDL_DIM_MAX; i++)
        {
          new_perm[dim++] = i;
        }
        for (int32_t i=TIDL_DIM_BATCH+1;i<=lastDim;i++)
        {
          new_perm[dim++] = i;
        }
        /** Transpose over batch not supported*/
        new_perm[0] = 0;
        #endif
        memcpy(newLayer.layerParams.transposeParams.perm, new_perm, sizeof(int32_t)*TIDL_DIM_MAX);
        newLayer.layerParams.transposeParams.isDataConvertNeeded = 1;

        for (int32_t j=0;j<TIDL_DIM_MAX;j++)
        {
          newLayer.outData[0].dimValues[j] = newLayer.inData[0].dimValues[new_perm[j]];
          curLayer.inData[0].dimValues[j] = newLayer.outData[0].dimValues[j];
          cur_perm[j] = TIDL_find_index(new_perm, perm[j], TIDL_DIM_MAX);
          if (cur_perm[j] == -1)
          {
            printf("Axis search failed in transpose perm\n");
          }
        }
        layerAdded = 1;
        memcpy(curLayer.layerParams.transposeParams.perm, cur_perm, sizeof(int32_t)*TIDL_DIM_MAX);
        curLayer.layerParams.transposeParams.isDataConvertNeeded = 0;

      }
    }
  }
  return layerAdded;
}

int32_t tidl_addDataConvertLayerForOTFsupport(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex, int32_t * dataIndex, tidl_import_config * params, unordered_set<int32_t>& LayerList)
{
  int32_t i, j;
  int32_t idx;
  int32_t inParamIdx = 0;
  int32_t outParamIdx = 0;
  int32_t inputAdd = 1;

  for (i = 0; i < layerIndex; i++)
  {
    int32_t outLayerdx = tidl_getOutLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dataId);
    if (outLayerdx == -1) {
      pOrgTIDLNetStructure.numLayers = layerIndex;
      return -1;
    }
    int32_t inIdx;
    inIdx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dataId);
    int32_t layerUnsupported = (LayerList.find(pOrgTIDLNetStructure.TIDLPCLayers[i].layerType) != LayerList.end());
    int32_t layerTypeAdded2 = tidl_checkForDCLayerForOTF(pOrgTIDLNetStructure.TIDLPCLayers[inIdx].layerType, pOrgTIDLNetStructure.TIDLPCLayers[i].layerType, LayerList);
    int32_t layerTypeAdded1 = tidl_checkForDCLayerForOTF(pOrgTIDLNetStructure.TIDLPCLayers[i].layerType, pOrgTIDLNetStructure.TIDLPCLayers[outLayerdx].layerType, LayerList);

    if(layerUnsupported)
    {
      if(pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_ReshapeLayer)
      {
        /* Add Data Convert Layers before and after reshape layer to remove and add pad if required,
           these layers are later removed at the end of import process if not required */
        layerTypeAdded1 = TIDL_DC_FOR_PAD_REMOVAL;
        layerTypeAdded2 = TIDL_DC_FOR_PAD_ADDITION;
      }
      if ((pOrgTIDLNetStructure.TIDLPCLayers[i].numOutBufs == 1) && (layerTypeAdded1==TIDL_DC_FOR_PAD_REMOVAL))
      {
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType  = TIDL_DataConvertLayer;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs  = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weightsElementSizeInBits = NUM_WHGT_BITS;

        strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0]);
        strcat((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], "_netFormat");

        strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0]);

        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0];
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = pOrgTIDLNetStructure.TIDLPCLayers[i].outConsumerCnt[0];
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = pOrgTIDLNetStructure.TIDLPCLayers[i].outConsumerLinked[0];

        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].elementType = pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].elementType;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].actParams.actType = TIDL_NoAct;

        pOrgTIDLNetStructure.TIDLPCLayers[i].outConsumerCnt[0] = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outConsumerLinked[0] = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dataId = (*dataIndex)++;

        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0];

        /*This value is set again during import_quantize,
          setting it to -1 to distinguish this from quantize dataconvert in tflite asymmetric quantization*/
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].roundBits = -1;

        if ((gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TENSORFLOW || gParams.modelType ==  TIDL_IMPORT_MODEL_FORMAT_TFLITE || gParams.modelType ==  TIDL_IMPORT_MODEL_FORMAT_TFLITE_RT) &&
            (pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_ReshapeLayer))
        {
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.dataConvertParams.layout    = TIDL_LT_NHWC;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.dataConvertParams.outLayout    = TIDL_LT_NCHW;
        }
        else
        {
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.dataConvertParams.layout    = params->inLayout[0];
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.dataConvertParams.outLayout    = params->inLayout[0];
        }

        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.dataConvertParams.inZeroPoint = params->inZeroPoint[0];
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.dataConvertParams.type = TIDL_DC_TYPE_INTERMEDIATE;

        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numMacs =
          (int64_t)((int64_t)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_BATCH] * pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH] *
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] * pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] * 4);

        TIDL_UpdateInDataBuff(&pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0]);
        TIDL_UpdateInDataName(&pOrgTIDLNetStructure, layerIndex,
        (char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0],
        (char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0]);
        layerIndex++;
        inParamIdx++;
      }

      if ((pOrgTIDLNetStructure.TIDLPCLayers[i].numInBufs > 0) && (layerTypeAdded2==TIDL_DC_FOR_PAD_ADDITION))
      {

        /* After many topo sorts, there is no guarantee that output data layers
          are in the same order as they are originally created.  For TVM Relay
          input, use the output index encoded in the name, e.g. tidl_0_o1 */
        if (gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TVM_RELAY)
        {
          char *o_name = (char*) pOrgTIDLNetStructure.TIDLPCLayers[i].inDataNames[0];
          int pos = strlen(o_name);
          while (o_name[pos-1] != 'o') pos--;
          outParamIdx = atoi(&o_name[pos]);
        }

        for (j = 0; j < pOrgTIDLNetStructure.TIDLPCLayers[i].numInBufs ; j++)
        {
          int32_t dataSize;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType  = TIDL_DataConvertLayer;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs  = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weightsElementSizeInBits = NUM_WHGT_BITS;

          strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[inIdx].outDataNames[0]);
          strcat((char *)pOrgTIDLNetStructure.TIDLPCLayers[inIdx].outDataNames[0], "_netFormat");
          strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[inIdx].outDataNames[0]);

          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0] = pOrgTIDLNetStructure.TIDLPCLayers[inIdx].outData[0];
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0]    = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = 1;

          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0];

          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].elementType = pOrgTIDLNetStructure.TIDLPCLayers[inIdx].outData[0].elementType;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].padH = pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.convParams.padH;
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].padW = pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.convParams.padW;
          /* Default value of tensor scale is zero, if the user given value is anything other than default then it indicates user wants
          to specify output tensor scale. */

          if ( params->outTensorScale[0] != 0.0 )
          {
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].tensorScale = params->outTensorScale[0];
          }
          else
          {
            /* this is irrelevant as this will be re-computed as part of output scale computation */
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].tensorScale = 1.0;
          }

          /*This value is set again during import_quantize,
            setting it to -1 to distinguish this from quantize dataconvert*/
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].roundBits = -1;

          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].actParams.actType = TIDL_NoAct;
          if ((gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TENSORFLOW || gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TFLITE || gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_TFLITE_RT) &&
              (pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_ReshapeLayer))
          {
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.dataConvertParams.layout    = TIDL_LT_NCHW;
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.dataConvertParams.outLayout    = TIDL_LT_NHWC;
          }
          else
          {
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.dataConvertParams.layout    = params->outLayout[0];
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.dataConvertParams.outLayout    = params->outLayout[0];
          }
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.dataConvertParams.outZeroPoint = params->outZeroPoint[0];
          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.dataConvertParams.type = TIDL_DC_TYPE_INTERMEDIATE;


          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dataId = (*dataIndex)++;

          pOrgTIDLNetStructure.TIDLPCLayers[i].inData[j] = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0];
          strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[i].inDataNames[j], (char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0]);

          pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numMacs =
            (int64_t)((int64_t)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_BATCH] * pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH] *
              pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] * pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] * 4);
          TIDL_UpdateInDataBuff(&pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0]);
          layerIndex++;
        }
        outParamIdx++;
      }
    }
  }
  pOrgTIDLNetStructure.numLayers = layerIndex;
  return 0;
}

int32_t tidl_optimizePatchMergingBlock (sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t* dataIndex, int32_t& layerIndex)
{
  int32_t i1, i2, i3, i4;
  int32_t status = 0, optimized = 0;

  for (i1 = 0; i1 < layerIndex; i1++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_PatchMergeLayer)
    {
      optimized = 1;
      pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType = TIDL_TransposeLayer;
      TIDL_addLayer (pOrgTIDLNetStructure, i1, TIDL_ReshapeLayer, dataIndex, pOrgTIDLNetStructure.numLayers, ADD_LAYER_BEFORE);
      TIDL_addLayer (pOrgTIDLNetStructure, i1, TIDL_ReshapeLayer, dataIndex, pOrgTIDLNetStructure.numLayers, ADD_LAYER_AFTER);
      sTIDL_LayerPC_t& beforeReshape = pOrgTIDLNetStructure.TIDLPCLayers[pOrgTIDLNetStructure.numLayers-2];
      sTIDL_LayerPC_t& afterReshape  = pOrgTIDLNetStructure.TIDLPCLayers[pOrgTIDLNetStructure.numLayers-1];
      beforeReshape.layerPCParams.reshapeParams.isInduced = 1;
      afterReshape.layerPCParams.reshapeParams.isInduced = 1;
      beforeReshape.outData[0].dimValues[0] = 1;
      beforeReshape.outData[0].dimValues[1] = 1;
      beforeReshape.outData[0].dimValues[2] = beforeReshape.inData[0].dimValues[TIDL_DIM_NUMCH]/2;
      beforeReshape.outData[0].dimValues[3] = 2;
      beforeReshape.outData[0].dimValues[4] = beforeReshape.inData[0].dimValues[TIDL_DIM_HEIGHT];
      beforeReshape.outData[0].dimValues[5] = beforeReshape.inData[0].dimValues[TIDL_DIM_WIDTH];
      memcpy(pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0].dimValues, beforeReshape.outData[0].dimValues, sizeof(int32_t)* TIDL_DIM_MAX);
      memcpy(afterReshape.outData[0].dimValues, pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].dimValues, sizeof(int32_t)* TIDL_DIM_MAX);
      memcpy(pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].dimValues, pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0].dimValues, sizeof(int32_t)* TIDL_DIM_MAX);

      int32_t perm[] = {0,1,2,4,3,5};
      pOrgTIDLNetStructure.TIDLPCLayers[i1].weights.ptr = malloc(sizeof(int32_t) * TIDL_DIM_MAX);
      pOrgTIDLNetStructure.TIDLPCLayers[i1].weights.bufSize = TIDL_DIM_MAX;
      memcpy(pOrgTIDLNetStructure.TIDLPCLayers[i1].weights.ptr, perm, sizeof(int32_t)* TIDL_DIM_MAX);

      TIDL_tfOutReshapeTransposeLayer(&pOrgTIDLNetStructure, i1);
      memcpy(afterReshape.inData[0].dimValues, pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].dimValues, sizeof(int32_t)* TIDL_DIM_MAX);
    }
  }
  if (optimized == 1)
  {
    tidl_sortLayersInProcOrder(&pOrgTIDLNetStructure, &tempTIDLNetStructure, pOrgTIDLNetStructure.numLayers);
  }
  return 0;
}

int32_t tidl_checkSliceForPatchMerge (sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t sIdx, int32_t eIdx)
{
  int32_t status = 1, i1, i2, layerCheck, strideCheck, axisCheck, startCheck;
  int32_t starts0[10] = {0}, starts1[10] = {0}, s0=0, s1=0;
  for (i1 = sIdx + 1; i1 < eIdx ; i1++)
  {
    layerCheck = 0;strideCheck = 0;axisCheck = 0;startCheck = 0;
    sTIDL_LayerPC_t& layer = pOrgTIDLNetStructure.TIDLPCLayers[i1];
    layerCheck  = layer.layerType == TIDL_SliceLayer ? 1:0;
    strideCheck = layer.layerParams.sliceParams.stride == 2 ? 1:0;
    axisCheck   = (layer.layerParams.sliceParams.axis == TIDL_DIM_NUMCH || layer.layerParams.sliceParams.axis == TIDL_DIM_HEIGHT ) ? 1:0;
    startCheck  = (layer.layerParams.sliceParams.slicePoints[0] == 0 || layer.layerParams.sliceParams.slicePoints[0] == 1) ? 1:0;

    if (layerCheck && strideCheck && axisCheck && startCheck)
    {
      if (layer.layerParams.sliceParams.axis == TIDL_DIM_NUMCH)
      {
        int32_t inLyrIdx = tidl_getInLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, layer.inData[0].dataId);
        if (pOrgTIDLNetStructure.TIDLPCLayers[inLyrIdx].layerType != TIDL_ReshapeLayer)
        {
          status = -1;
        }
      }
      else if (layer.layerParams.sliceParams.axis == TIDL_DIM_HEIGHT)
      {
        int32_t inLyrIdx = tidl_getInLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, layer.inData[0].dataId);
        sTIDL_LayerPC_t& inLayer = pOrgTIDLNetStructure.TIDLPCLayers[inLyrIdx];
        if (inLayer.layerType != TIDL_SliceLayer ||
            inLayer.layerParams.sliceParams.axis != TIDL_DIM_NUMCH)
        {
          status = -1;
        }

        if (inLayer.layerParams.sliceParams.slicePoints[0] == 0)
        {
          starts0[s0++] = i1;
        }
        else
        {
          starts1[s1++] = i1;
        }
      }
    }
    else
    {
      status = -1;
    }
  }
  if (s0 != 2 || s1 != 2)
  {
    status = -1;
  }
  else
  {
      if ( (pOrgTIDLNetStructure.TIDLPCLayers[starts0[0]].layerParams.sliceParams.slicePoints[0] ==
          pOrgTIDLNetStructure.TIDLPCLayers[starts0[1]].layerParams.sliceParams.slicePoints[0]) ||
            (pOrgTIDLNetStructure.TIDLPCLayers[starts1[0]].layerParams.sliceParams.slicePoints[0] ==
          pOrgTIDLNetStructure.TIDLPCLayers[starts1[1]].layerParams.sliceParams.slicePoints[0])
          )

      {
        status = -1;
      }
  }
  return status;
}

int32_t tidl_identifyPatchMergingBlock (sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t* dataIndex, int32_t& layerIndex, int32_t& sIdx)
{
  int32_t i1, i2, i3, i4;
  int32_t eIdx = -1;

  if (pOrgTIDLNetStructure.TIDLPCLayers[sIdx].layerType == TIDL_ReshapeLayer)
  {
    int32_t outLayerIdx = tidl_getOutLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, pOrgTIDLNetStructure.TIDLPCLayers[sIdx].outData[0].dataId);
    if (pOrgTIDLNetStructure.TIDLPCLayers[outLayerIdx].layerType == TIDL_SliceLayer)
    {
      /** Check for patch merging block*/
      int32_t nextLayerIdx = outLayerIdx;
      while (pOrgTIDLNetStructure.TIDLPCLayers[nextLayerIdx].layerType == TIDL_SliceLayer)
      {
        nextLayerIdx = tidl_getOutLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, pOrgTIDLNetStructure.TIDLPCLayers[nextLayerIdx].outData[0].dataId);
      }
      if (pOrgTIDLNetStructure.TIDLPCLayers[nextLayerIdx].layerType == TIDL_ConcatLayer &&
          (nextLayerIdx - outLayerIdx) == 8)
      {
        if (tidl_checkSliceForPatchMerge (pOrgTIDLNetStructure, sIdx, nextLayerIdx) == 1)
        {
          eIdx = nextLayerIdx;
        }
      }
    }
  }
  return eIdx;
}

int32_t tidl_fusePatchMergingBlock (sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t* dataIndex, int32_t& layerIndex)
{
  int32_t sIdx=0, eIdx=0, i1=0;
  for (int32_t i=0;i<layerIndex;i++)
  {
    sIdx = i;
    eIdx = tidl_identifyPatchMergingBlock (pOrgTIDLNetStructure, dataIndex, layerIndex, sIdx);
    if (eIdx != -1)
    {
      #if 1
      memcpy(pOrgTIDLNetStructure.TIDLPCLayers[sIdx+1].outData[0].dimValues, pOrgTIDLNetStructure.TIDLPCLayers[eIdx].outData[0].dimValues, sizeof(int32_t) * TIDL_DIM_MAX);
      #else
      pOrgTIDLNetStructure.TIDLPCLayers[sIdx].outData[0].dimValues[0] = 1;
      pOrgTIDLNetStructure.TIDLPCLayers[sIdx].outData[0].dimValues[1] = 1;
      pOrgTIDLNetStructure.TIDLPCLayers[sIdx].outData[0].dimValues[2] = 1;
      pOrgTIDLNetStructure.TIDLPCLayers[sIdx].outData[0].dimValues[3] = 7;
      pOrgTIDLNetStructure.TIDLPCLayers[sIdx].outData[0].dimValues[4] = 7;
      pOrgTIDLNetStructure.TIDLPCLayers[sIdx].outData[0].dimValues[5] = 1536;
      #endif
      pOrgTIDLNetStructure.TIDLPCLayers[sIdx+1].outData[0].dataId = pOrgTIDLNetStructure.TIDLPCLayers[eIdx].outData[0].dataId;
      strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[sIdx+1].outDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[eIdx].outDataNames[0]);
      /** Remove all the layers except reshape*/
      for (int32_t i1=sIdx+2;i1<=eIdx;i1++)
      {
        pOrgTIDLNetStructure.TIDLPCLayers[i1].numInBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[i1].numOutBufs = -1;
      }
      tidl_removeMergedLayersFromNet(&pOrgTIDLNetStructure, &tempTIDLNetStructure);

      pOrgTIDLNetStructure.TIDLPCLayers[sIdx+1].layerType = TIDL_PatchMergeLayer;
      pOrgTIDLNetStructure.TIDLPCLayers[sIdx+1].layerParams.transposeParams.isDataConvertNeeded = 0;
      pOrgTIDLNetStructure.TIDLPCLayers[sIdx+1].outData[0].elementType = pOrgTIDLNetStructure.TIDLPCLayers[sIdx].outData[0].elementType;
    }
  }
  tidl_optimizePatchMergingBlock (pOrgTIDLNetStructure, dataIndex, pOrgTIDLNetStructure.numLayers);
  return 0;
}

int32_t tidl_reduceSliceDims(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t* dataIndex, int32_t& layerIndex)
{
  int32_t i1, i2, i3, i4;
  int32_t status = 0, layersAdded = 0;

  for (i1 = 0; i1 < layerIndex; i1++)
  {
    if (pOrgTIDLNetStructure.TIDLPCLayers[i1].layerType == TIDL_SliceLayer &&
        pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.sliceParams.axis < TIDL_DIM_NUMCH)
    {
      status = 1;
      int32_t totDim = 1;
      int32_t axisId = pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.sliceParams.axis;
      int32_t origDim = pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0].dimValues[axisId];
      for (int32_t i2 = TIDL_DIM_NUMCH; i2>= TIDL_DIM_BATCH; i2--)
      {
        totDim *= pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0].dimValues[i2];
      }
      TIDL_addLayer (pOrgTIDLNetStructure, i1, TIDL_ReshapeLayer, dataIndex, pOrgTIDLNetStructure.numLayers, ADD_LAYER_BEFORE);
      sTIDL_LayerPC_t& Reshape = pOrgTIDLNetStructure.TIDLPCLayers[pOrgTIDLNetStructure.numLayers-1];
      Reshape.layerPCParams.reshapeParams.isInduced = 1;
      Reshape.outData[0].dimValues[TIDL_DIM_BATCH] = 1;
      Reshape.outData[0].dimValues[TIDL_DIM_DIM1]  = 1;
      Reshape.outData[0].dimValues[TIDL_DIM_DIM2]  = 1;
      Reshape.outData[0].dimValues[TIDL_DIM_NUMCH] = totDim;
      Reshape.outData[0].dimValues[TIDL_DIM_HEIGHT] = pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0].dimValues[TIDL_DIM_HEIGHT];
      Reshape.outData[0].dimValues[TIDL_DIM_HEIGHT] = pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[0].dimValues[TIDL_DIM_HEIGHT];

      int32_t outLayerIdx = tidl_getOutLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, Reshape.outData[0].dataId);

      {
        for(int j=0;j<pOrgTIDLNetStructure.TIDLPCLayers[outLayerIdx].numInBufs;j++)
        {
          if(pOrgTIDLNetStructure.TIDLPCLayers[outLayerIdx].inData[j].dataId == Reshape.outData[0].dataId)
          {
            memcpy(pOrgTIDLNetStructure.TIDLPCLayers[outLayerIdx].inData[j].dimValues, Reshape.outData[0].dimValues,  sizeof(int32_t)*TIDL_DIM_MAX);
          }
        }
      }

      layersAdded = TIDL_addLayer (pOrgTIDLNetStructure, i1, TIDL_ReshapeLayer, dataIndex, pOrgTIDLNetStructure.numLayers, ADD_LAYER_AFTER);
      for (i3 = 0;i3 < layersAdded; i3++)
      {
        sTIDL_LayerPC_t& Reshape = pOrgTIDLNetStructure.TIDLPCLayers[pOrgTIDLNetStructure.numLayers-1-i3];
        Reshape.layerPCParams.reshapeParams.isInduced = 1;
        memcpy(Reshape.outData[0].dimValues, pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[i3].dimValues,  sizeof(int32_t)*TIDL_DIM_MAX);
      }

      for(i3 = 0; i3 < pOrgTIDLNetStructure.TIDLPCLayers[i1].numOutBufs; i3++)
      {
        pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.sliceParams.slicePoints[i3] *= (totDim/origDim);
      }
      pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.sliceParams.slicePoints[pOrgTIDLNetStructure.TIDLPCLayers[i1].numOutBufs] = totDim;
      pOrgTIDLNetStructure.TIDLPCLayers[i1].layerParams.sliceParams.axis = TIDL_DIM_NUMCH;
    }
  }
  return status;
}

static int32_t tidl_checkForValidPad(sTIDL_LayerPC_t &processingLayer)
{
  int32_t output = 0;
  if(processingLayer.layerType == TIDL_ConvolutionLayer)
  {
    if(processingLayer.layerParams.convParams.kernelW>1 && processingLayer.layerParams.convParams.kernelH>1 && processingLayer.layerParams.convParams.padH==0 && processingLayer.layerParams.convParams.padW==0 && processingLayer.layerParams.convParams.strideH==1 && processingLayer.layerParams.convParams.strideW==1)
    {
      output = 1;
    }
  }

  if(processingLayer.layerType == TIDL_PoolingLayer)
  {
    if(processingLayer.layerParams.poolParams.kernelW>1 && processingLayer.layerParams.poolParams.kernelH>1 && processingLayer.layerParams.poolParams.padH==0 && processingLayer.layerParams.poolParams.padW==0 && processingLayer.layerParams.poolParams.strideH==1 && processingLayer.layerParams.poolParams.strideW==1)
    {
      output = 1;
    }
  }
  return output;
}

int32_t tidl_addCropLayerForValidPadSupport(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex, int32_t * dataIndex, tidl_import_config * params)
{
  int32_t i, j;
  int32_t idx;
  int32_t inParamIdx = 0;
  int32_t outParamIdx = 0;
  int32_t inputAdd = 1;

  for (i = 0; i < layerIndex; i++)
  {
    int32_t outLayerdx = tidl_getOutLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dataId);
    if (outLayerdx == -1) {
      pOrgTIDLNetStructure.numLayers = layerIndex;
      return -1;
    }
    int32_t inIdx;
    inIdx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dataId);
    int32_t layerUnsupported = tidl_checkForValidPad(pOrgTIDLNetStructure.TIDLPCLayers[i]);

    if(layerUnsupported)
    {
      if ((pOrgTIDLNetStructure.TIDLPCLayers[i].numOutBufs > 0) && pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_ConvolutionLayer)
      {
        /*Modify the convolution layer, such that it performs "SAME" convolution*/
        int32_t Fc = pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.convParams.kernelW;
        int32_t Fr = pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.convParams.kernelH;
        int32_t dilationH = pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.convParams.dilationH;
        int32_t dilationW = pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.convParams.dilationW;
        int32_t eFc = (dilationW * (Fc - 1)) + 1;
        int32_t eFr = (dilationH * (Fr - 1)) + 1;
        int32_t localPadW = (eFc - 1)/2;
        int32_t localPadH = (eFr - 1)/2;
        int32_t numOutChannels = pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.convParams.numOutChannels;
        pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.convParams.padW = localPadW;
        pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.convParams.padH = localPadH;
        pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.convParams.padT = localPadH;
        pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.convParams.padB = localPadH;
        pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.convParams.padL = localPadW;
        pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.convParams.padR = localPadW;
        /*Update output height and width to be the same as input height & width: */
        int32_t cropHeight = pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_HEIGHT];
        int32_t cropWidth = pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_WIDTH];
        int32_t inHeight  = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_HEIGHT];
        int32_t inWidth = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_WIDTH];
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_HEIGHT] = inHeight;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_WIDTH] = inWidth;
        /************************************************************************/
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType  = TIDL_CropLayer;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs  = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weightsElementSizeInBits = NUM_WHGT_BITS;

        strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0]);
        strcat((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], "_cropLayer");

        strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0]);

        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0];
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = pOrgTIDLNetStructure.TIDLPCLayers[i].outConsumerCnt[0];
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = pOrgTIDLNetStructure.TIDLPCLayers[i].outConsumerLinked[0];

        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].elementType = pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].elementType;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].actParams.actType = TIDL_NoAct;

        pOrgTIDLNetStructure.TIDLPCLayers[i].outConsumerCnt[0] = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outConsumerLinked[0] = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dataId = (*dataIndex)++;

        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0];
        /*Adjust input dimensions of crop:*/
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] = cropHeight;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] = cropWidth;
        /*Set crop params:*/
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.cropParams.numChannels = numOutChannels;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.cropParams.offsetW = localPadW;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.cropParams.offsetH = localPadH;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.cropParams.multiCoreMode = TIDL_NOT_MULTI_CORE;
        /* Copy Activation params*/
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].actParams = pOrgTIDLNetStructure.TIDLPCLayers[i].actParams;
        /******************/

        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numMacs =
          (int64_t)((int64_t)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_BATCH] * pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH] *
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] * pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] * 4);

        TIDL_UpdateInDataBuff(&pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0]);
        TIDL_UpdateInDataName(&pOrgTIDLNetStructure, layerIndex,
        (char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0],
        (char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0]);
        layerIndex++;
        inParamIdx++;
      }
      else if ((pOrgTIDLNetStructure.TIDLPCLayers[i].numOutBufs > 0) && pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_PoolingLayer)
      {
        /*Modify the Pooling layer, such that it performs "SAME" Pooling*/
        int32_t Fc = pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.poolParams.kernelW;
        int32_t Fr = pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.poolParams.kernelH;
        int32_t localPadW = (Fc - 1)/2;
        int32_t localPadH = (Fr - 1)/2;
        int32_t numOutChannels = pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.poolParams.numChannels;
        pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.poolParams.padW = localPadW;
        pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.poolParams.padH = localPadH;
        //Review:
        pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.poolParams.padT = localPadH;
        pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.poolParams.padB = localPadH;
        pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.poolParams.padL = localPadW;
        pOrgTIDLNetStructure.TIDLPCLayers[i].layerParams.poolParams.padR = localPadW;
        /*Update output height and width to be the same as input height & width: */
        int32_t cropHeight = pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_HEIGHT];
        int32_t cropWidth = pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_WIDTH];
        int32_t inHeight  = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_HEIGHT];
        int32_t inWidth = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_WIDTH];
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_HEIGHT] = inHeight;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_WIDTH] = inWidth;
        /************************************************************************/
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType  = TIDL_CropLayer;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs  = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weightsElementSizeInBits = NUM_WHGT_BITS;

        strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0]);
        strcat((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], "_cropLayer");

        strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0]);

        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0];
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = pOrgTIDLNetStructure.TIDLPCLayers[i].outConsumerCnt[0];
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = pOrgTIDLNetStructure.TIDLPCLayers[i].outConsumerLinked[0];

        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].elementType = pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].elementType;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].actParams.actType = TIDL_NoAct;

        pOrgTIDLNetStructure.TIDLPCLayers[i].outConsumerCnt[0] = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outConsumerLinked[0] = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dataId = (*dataIndex)++;

        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0];
        /*Adjust input dimensions of crop:*/
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] = cropHeight;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] = cropWidth;
        /*Set crop params:*/
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.cropParams.numChannels = numOutChannels;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.cropParams.offsetW = localPadW;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.cropParams.offsetH = localPadH;
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.cropParams.multiCoreMode = TIDL_NOT_MULTI_CORE;
        /******************/

        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numMacs =
          (int64_t)((int64_t)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_BATCH] * pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH] *
            pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] * pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] * 4);

        TIDL_UpdateInDataBuff(&pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0]);
        TIDL_UpdateInDataName(&pOrgTIDLNetStructure, layerIndex,
        (char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0],
        (char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0]);
        layerIndex++;
        inParamIdx++;
      }
    }
  }
  pOrgTIDLNetStructure.numLayers = layerIndex;
  return 0;
}

bool tidl_findReshapeinNetwork(sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t numLayers)
{
  for(int i=0;i<numLayers;i++)
  {
    if(pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_ReshapeLayer)
    {
      return 1;
    }
  }
  return 0;
}

int32_t tidl_addInDataLayerForUVData(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure,
                                     int32_t layerIndex,
                                     int32_t *dataIndex,
                                     tidl_import_config *params,
                                     int32_t *inputList,
                                     int32_t numInLayers)
{
  int32_t i;
  char dataLayerName[10];

  for(i=0; i<numInLayers; i++)
  {
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType = TIDL_DataLayer;
    snprintf(dataLayerName, 2, "%d", i);
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs  = -1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = 1;
    strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char *)"Input_UVSemiPlanar_");
    strcat((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char *)dataLayerName);
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dataId = (*dataIndex)++;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = 0;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = 0;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_BATCH] = pOrgTIDLNetStructure.TIDLPCLayers[inputList[i]].outData[0].dimValues[TIDL_DIM_BATCH];
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_DIM1] = pOrgTIDLNetStructure.TIDLPCLayers[inputList[i]].outData[0].dimValues[TIDL_DIM_DIM1];
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_DIM2] = pOrgTIDLNetStructure.TIDLPCLayers[inputList[i]].outData[0].dimValues[TIDL_DIM_DIM2];
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH] = pOrgTIDLNetStructure.TIDLPCLayers[inputList[i]].outData[0].dimValues[TIDL_DIM_NUMCH];
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] = pOrgTIDLNetStructure.TIDLPCLayers[inputList[i]].outData[0].dimValues[TIDL_DIM_HEIGHT]/2;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] = pOrgTIDLNetStructure.TIDLPCLayers[inputList[i]].outData[0].dimValues[TIDL_DIM_WIDTH];
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].maxTensorValue = 255;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].minTensorValue = 0;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].tensorScale = params->inQuantFactor[i];
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].elementType = params->inElementType[i];
    layerIndex++;
  }

  pOrgTIDLNetStructure.numLayers = layerIndex;
  return 0;
}

int32_t tidl_addUVConversionLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure,
                                  int32_t layerIndex,
                                  int32_t *dataIndex,
                                  int32_t *uvLayerList)
{
  int32_t i, inputLayerIndex;
  char dataLayerName[10];

  for(i=0; i<pOrgTIDLNetStructure.numLayers; i++)
  {
    inputLayerIndex = 0;
    if((pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_DataLayer) &&
       (pOrgTIDLNetStructure.TIDLPCLayers[i].numInBufs == -1) &&
       (pOrgTIDLNetStructure.TIDLPCLayers[i].outConsumerCnt[0] == 0))
    {
      pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType = TIDL_ColorConversionLayer;
      snprintf(dataLayerName, 2, "%d", inputLayerIndex);
      pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs  = 1;
      pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = pOrgTIDLNetStructure.TIDLPCLayers[i].numOutBufs;
      pOrgTIDLNetStructure.TIDLPCLayers[i].numOutBufs = 1;

      strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char *)"Convert_UV420SP_To_Planar");
      strcat((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char *)dataLayerName);
      strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0]);

      pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0];
      pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = pOrgTIDLNetStructure.TIDLPCLayers[i].outConsumerCnt[0];
      pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = pOrgTIDLNetStructure.TIDLPCLayers[i].outConsumerLinked[0];
      pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].elementType = tidl_getElementType(0);
      pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].actParams.actType = TIDL_NoAct;
      pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dataId = (*dataIndex)++;
      pOrgTIDLNetStructure.TIDLPCLayers[i].outConsumerCnt[0] = 1;
      pOrgTIDLNetStructure.TIDLPCLayers[i].outConsumerLinked[0] = 1;

      TIDL_tfOutReshapeColorConversionLayer(&pOrgTIDLNetStructure, layerIndex);
      uvLayerList[inputLayerIndex] = layerIndex;
      layerIndex++;
      inputLayerIndex++;
    }
  }

  pOrgTIDLNetStructure.numLayers = layerIndex;
  return 0;
}

int32_t tidl_addConcatLayerForYUVMerge(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure,
                                       int32_t layerIndex,
                                       int32_t *dataIndex,
                                       int32_t *inputList,
                                       int32_t (*outForInputList)[TIDL_NUM_OUT_BUFS],
                                       int32_t *uvLayerList,
                                       int32_t *newConcatLayerList,
                                       int32_t *numOutLayers,
                                       int32_t numInLayers)
{
  int32_t i, j;
  char dataLayerName[10];

  for(i=0; i<numInLayers; i++)
  {
    orgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType = TIDL_ConcatLayer;
    orgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.concatParams.axis = TIDL_DIM_NUMCH;
    snprintf(dataLayerName, 2, "%d", i);
    orgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs  = 2;
    orgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = orgTIDLNetStructure.TIDLPCLayers[inputList[i]].numOutBufs;
    orgTIDLNetStructure.TIDLPCLayers[inputList[i]].numOutBufs = 1;
    strcpy((char *)orgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char *)"Concat_Y_UV_");
    strcat((char *)orgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char *)dataLayerName);
    strcpy((char *)orgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0], (char *)orgTIDLNetStructure.TIDLPCLayers[inputList[i]].outDataNames[0]);
    strcpy((char *)orgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[1], (char *)orgTIDLNetStructure.TIDLPCLayers[uvLayerList[i]].outDataNames[0]);

    orgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0] = orgTIDLNetStructure.TIDLPCLayers[inputList[i]].outData[0];
    orgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[1] = orgTIDLNetStructure.TIDLPCLayers[uvLayerList[i]].outData[0];
    orgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = orgTIDLNetStructure.TIDLPCLayers[inputList[i]].outConsumerCnt[0];
    orgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = orgTIDLNetStructure.TIDLPCLayers[inputList[i]].outConsumerLinked[0];
    orgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].elementType = tidl_getElementType(0);
    orgTIDLNetStructure.TIDLPCLayers[layerIndex].actParams.actType = TIDL_NoAct;
    orgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dataId = (*dataIndex)++;
    for(j=0; j<numOutLayers[i]; j++)
    {
      orgTIDLNetStructure.TIDLPCLayers[outForInputList[i][j]].inData[0] = orgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0];
      strcpy((char *)orgTIDLNetStructure.TIDLPCLayers[outForInputList[i][j]].inDataNames[0], (char *)orgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0]);
    }
    orgTIDLNetStructure.TIDLPCLayers[inputList[i]].outConsumerCnt[0] = 1;
    orgTIDLNetStructure.TIDLPCLayers[inputList[i]].outConsumerLinked[0] = 1;
    orgTIDLNetStructure.TIDLPCLayers[uvLayerList[i]].outConsumerCnt[0] = 1;
    orgTIDLNetStructure.TIDLPCLayers[uvLayerList[i]].outConsumerLinked[0] = 1;
    TIDL_tfOutReshapeConcatLayer(&orgTIDLNetStructure, layerIndex);
    newConcatLayerList[i] = layerIndex;
    layerIndex++;
  }

  orgTIDLNetStructure.numLayers = layerIndex;
  return 0;
}

int32_t tidl_addNormLayerToConcat(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure,
                                  int32_t layerIndex,
                                  int32_t *dataIndex,
                                  tidl_import_config *params,
                                  int32_t *concatLayerList,
                                  int32_t numConcatLayers)
{
  int32_t normParamIdx = 0, i, j;

  for(i=0; i<numConcatLayers; i++)
  {
    int32_t dataSize;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType = TIDL_BatchNormLayer;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = pOrgTIDLNetStructure.TIDLPCLayers[concatLayerList[i]].numOutBufs;
    pOrgTIDLNetStructure.TIDLPCLayers[concatLayerList[i]].numOutBufs = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weightsElementSizeInBits = NUM_WHGT_BITS;

    strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[concatLayerList[i]].outDataNames[0]);
    strcat((char *)pOrgTIDLNetStructure.TIDLPCLayers[concatLayerList[i]].outDataNames[0], "_original");
    strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[concatLayerList[i]].outDataNames[0]);

    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[concatLayerList[i]].outData[0];
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = pOrgTIDLNetStructure.TIDLPCLayers[concatLayerList[i]].outConsumerCnt[0];
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = pOrgTIDLNetStructure.TIDLPCLayers[concatLayerList[i]].outConsumerLinked[0];
    pOrgTIDLNetStructure.TIDLPCLayers[concatLayerList[i]].outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0];
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].elementType = tidl_getElementType(1);
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].actParams.actType = TIDL_NoAct;

    pOrgTIDLNetStructure.TIDLPCLayers[concatLayerList[i]].outConsumerCnt[0] = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[concatLayerList[i]].outConsumerLinked[0] = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[concatLayerList[i]].outData[0].dataId = (*dataIndex)++;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0] = pOrgTIDLNetStructure.TIDLPCLayers[concatLayerList[i]].outData[0];

    dataSize = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH];

    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weights.ptr = my_malloc(dataSize*sizeof(float));
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weights.bufSize = dataSize;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].bias.ptr = my_malloc(dataSize*sizeof(float));
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].bias.bufSize = dataSize;
    float *  bias     = (float *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].bias.ptr;
    float *  weights  = (float *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weights.ptr;
    for (j = 0; j < dataSize; j++)
    {
      if (normParamIdx >= TIDL_MAX_ALG_IN_BUFS*TIDL_IN_NUF_MAX_CH)
      {
        printf(" normParamIdx >= TIDL_MAX_ALG_IN_BUFS*TIDL_IN_NUF_MAX_CH \n ");
        exit(0);
      }
      weights[j] = params->inScale[normParamIdx];
      bias[j]    = - (params->inMean[normParamIdx]* weights[j]);
      normParamIdx++;
    }
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numMacs =
      (int64_t)((int64_t)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_BATCH] * pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH] *
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] * pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] * 4);

    TIDL_UpdateInDataBuff(&pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0]);

    layerIndex++;
  }

  pOrgTIDLNetStructure.numLayers = layerIndex;
  return 0;
}

int32_t tidl_addConvLayerAfterConcat(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure,
                                     int32_t layerIndex,
                                     int32_t *dataIndex,
                                     tidl_import_config *params,
                                     int32_t *concatLayerList,
                                     int32_t *newConvList,
                                     int32_t numConcatLayers)
{
  int32_t i, j, concatLayerId;
  int32_t weightsSize, biasSize;
  for (i = 0; i < numConcatLayers; i++)
  {
    concatLayerId = concatLayerList[i];
    if(3 != pOrgTIDLNetStructure.TIDLPCLayers[concatLayerId].outData[0].dimValues[TIDL_DIM_NUMCH])
    {
      printf("YUV Conversion: Cannot add convolution layer after concat when number of channels is %d\n", pOrgTIDLNetStructure.TIDLPCLayers[concatLayerId].outData[0].dimValues[TIDL_DIM_NUMCH]);
    }

    sTIDL_ConvParams_t &convParams = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.convParams;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType = TIDL_ConvolutionLayer;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = pOrgTIDLNetStructure.TIDLPCLayers[concatLayerId].numOutBufs;
    pOrgTIDLNetStructure.TIDLPCLayers[concatLayerId].numOutBufs = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weightsElementSizeInBits = NUM_WHGT_BITS;

    strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[concatLayerId].outDataNames[0]);
    strcat((char *)pOrgTIDLNetStructure.TIDLPCLayers[concatLayerId].outDataNames[0], "_original");
    strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[concatLayerId].outDataNames[0]);

    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[concatLayerId].outData[0];
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = pOrgTIDLNetStructure.TIDLPCLayers[concatLayerId].outConsumerCnt[0];
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = pOrgTIDLNetStructure.TIDLPCLayers[concatLayerId].outConsumerLinked[0];

    pOrgTIDLNetStructure.TIDLPCLayers[concatLayerId].outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0];

    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].elementType = tidl_getElementType(0);
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].actParams.actType = TIDL_NoAct;

    pOrgTIDLNetStructure.TIDLPCLayers[concatLayerId].outConsumerCnt[0] = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[concatLayerId].outConsumerLinked[0] = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[concatLayerId].outData[0].dataId = (*dataIndex)++;

    convParams.enableBias     = 1;
    convParams.enablePooling  = 0;
    convParams.numOutChannels = 3;
    convParams.numInChannels  = 3;
    convParams.kernelH        = 1;
    convParams.kernelW        = 1;
    convParams.numGroups = 1;
    convParams.dilationW = 1;
    convParams.dilationH = 1;
    convParams.strideW   = 1;
    convParams.strideH   = 1;
    convParams.padW      = 0;
    convParams.padH      = 0;

    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0] = pOrgTIDLNetStructure.TIDLPCLayers[concatLayerId].outData[0];
    weightsSize = convParams.numOutChannels * convParams.numInChannels * convParams.kernelH * convParams.kernelW; /* NoxNixFrxFc */
    biasSize = convParams.numOutChannels;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weights.ptr = my_malloc(weightsSize*sizeof(float));
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weights.bufSize = weightsSize;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].bias.ptr = my_malloc(biasSize*sizeof(float));
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].bias.bufSize = biasSize;
    float *  bias     = (float *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].bias.ptr;
    float *  weights  = (float *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weights.ptr;
    for (j = 0; j < weightsSize; j++)
    {
      weights[j] = 1.0/3.0;
    }
    for (j = 0; j < biasSize; j++)
    {
      bias[j]    = 0;
    }

    if((params->modelType == 2) || (params->modelType == 0) || (params->modelType == 6) )
    {
      pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].strideOffsetMethod = TIDL_StrideOffsetTopLeft;
    }

    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numMacs =
      (int64_t)(((int64_t)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_BATCH] * pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH] *
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] * pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] *
        convParams.kernelW *convParams.kernelH *
        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0].dimValues[TIDL_DIM_NUMCH]) / convParams.numGroups);
    TIDL_UpdateInDataBuff(&pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0]);

    newConvList[i] = layerIndex;
    layerIndex++;
  }
  pOrgTIDLNetStructure.numLayers = layerIndex;
  return 0;
}

int32_t tidl_addPadLayerAndConv1x1ForYUVtoRGB(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure,
                                              int32_t layerIndex,
                                              int32_t *dataIndex,
                                              tidl_import_config *params,
                                              int32_t * convLayerList,
                                              int32_t * padLayerList,
                                              int32_t * numYuvMergeConvLayers,
                                              int32_t * numYuvMergePadLayers)
{
  int32_t layerType[2], layerId[2];
  int32_t nonMergeConcatList[TIDL_NUM_MAX_LAYERS];
  int32_t nonMergeConcatLayerNum = 0, concatOutId, padOutId;
  int32_t i, j, k, l, i1;
  int32_t count = 0;
  bool canMergeInConv;
  int8_t padLayerNumber[TIDL_STRING_SIZE];
  int32_t padW, padH;
  int32_t numLayers = layerIndex;

  /* Loop over all the layers and identify the relevant concat layers present in the network */
  for(i=0; i<pOrgTIDLNetStructure.numLayers; i++)
  {
    /* Layer type should be concat and there should be two input buffers to this particular concat */
    if((pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_ConcatLayer) &&
       (pOrgTIDLNetStructure.TIDLPCLayers[i].numInBufs == 2))
    {
      layerId[0] = tidl_getInLayer(pOrgTIDLNetStructure,
                                   pOrgTIDLNetStructure.numLayers,
                                   pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dataId);
      layerId[1] = tidl_getInLayer(pOrgTIDLNetStructure,
                                   pOrgTIDLNetStructure.numLayers,
                                   pOrgTIDLNetStructure.TIDLPCLayers[i].inData[1].dataId);
      layerType[0] = pOrgTIDLNetStructure.TIDLPCLayers[layerId[0]].layerType;
      layerType[1] = pOrgTIDLNetStructure.TIDLPCLayers[layerId[1]].layerType;

      /* One of the input should be data layer and other should be uv420 conversion layer */
      if(((layerType[0] == TIDL_DataLayer) && (layerType[1] == TIDL_ColorConversionLayer)) ||
          ((layerType[1] == TIDL_DataLayer) && (layerType[0] == TIDL_ColorConversionLayer)))
      {
        canMergeInConv = true;

        /* Look at all the outputs of this concat and identify if all following layers are convolution
         * If any of the consumers is a pad layer, then check if consumer of pad layer is convolution  */
        for(j=0; j<pOrgTIDLNetStructure.TIDLPCLayers[i].numOutBufs; j++)
        {
          for(k = 0; k < pOrgTIDLNetStructure.numLayers; k++)
          {
            for(l = 0; l < pOrgTIDLNetStructure.TIDLPCLayers[k].numInBufs; k++)
            {
              if(pOrgTIDLNetStructure.TIDLPCLayers[i].outData[j].dataId == pOrgTIDLNetStructure.TIDLPCLayers[k].inData[l].dataId)
              {
                if(pOrgTIDLNetStructure.TIDLPCLayers[k].layerType == TIDL_PadLayer)
                {
                  padOutId = tidl_getOutLayer(pOrgTIDLNetStructure,
                                           pOrgTIDLNetStructure.numLayers,
                                           pOrgTIDLNetStructure.TIDLPCLayers[k].outData[0].dataId);
                  if(pOrgTIDLNetStructure.TIDLPCLayers[padOutId].layerType != TIDL_ConvolutionLayer)
                  {
                    canMergeInConv = false;
                    break;
                  }
                }
                else if(pOrgTIDLNetStructure.TIDLPCLayers[k].layerType != TIDL_ConvolutionLayer)
                {
                  canMergeInConv = false;
                  break;
                }
              }
            }
            if(canMergeInConv == false) break;
          }
          if(canMergeInConv == false) break;
        }

        /* If all following layers are convolution, add them to the convLayerList list.
         * Else add it to nonMergeConcatList
         * Add pad layer to network where needed, and add its index to padLayerList.
         */
        if(true == canMergeInConv)
        {
          count = 0;
          for(j=0; j<pOrgTIDLNetStructure.TIDLPCLayers[i].numOutBufs; j++)
          {
            for(k = 0; k < pOrgTIDLNetStructure.numLayers; k++)
            {
              for(l = 0; l < pOrgTIDLNetStructure.TIDLPCLayers[k].numInBufs; l++)
              {
                if(pOrgTIDLNetStructure.TIDLPCLayers[i].outData[j].dataId == pOrgTIDLNetStructure.TIDLPCLayers[k].inData[l].dataId)
                {
                  if(pOrgTIDLNetStructure.TIDLPCLayers[k].layerType == TIDL_PadLayer)
                  {
                    padLayerList[*numYuvMergePadLayers] = k;
                    (*numYuvMergePadLayers)++;
                    /* this is pad layer for batchnorm, modify the pad values for yuv to rgb padding in addition to batchnorm padding */
                    padOutId = tidl_getOutLayer(pOrgTIDLNetStructure,
                                            pOrgTIDLNetStructure.numLayers,
                                            pOrgTIDLNetStructure.TIDLPCLayers[k].outData[0].dataId);
                    convLayerList[*numYuvMergeConvLayers] = padOutId;
                    (*numYuvMergeConvLayers)++;
                  }
                  else if(pOrgTIDLNetStructure.TIDLPCLayers[k].layerType == TIDL_ConvolutionLayer)
                  {
                    convLayerList[*numYuvMergeConvLayers] = k;
                    (*numYuvMergeConvLayers)++;
                    /* Add pad layer before convolution layer for per channel yuv to rgb padding */
                    padW = pOrgTIDLNetStructure.TIDLPCLayers[k].layerParams.convParams.padW;
                    padH = pOrgTIDLNetStructure.TIDLPCLayers[k].layerParams.convParams.padH;
                    if((padH != 0) || (padW != 0)) /* no pad layer should be added if padH = padW = 0 */
                    {
                      if((gParams.inDataNorm[0] == 1) && (gParams.foldPreBnConv2D != 2))
                      {
                        /* If BN is present, and fold != 2 means no padding for batchnorm, so don't add padding for yuv - no padding at all */
                      }
                      else
                      {
                        /* Add pad layer before convolution */
                        padLayerList[*numYuvMergePadLayers] = numLayers;
                        (*numYuvMergePadLayers)++;
                        int32_t padT = padH, padB = padH;
                        int32_t padL = padW, padR = padW;
                        tidl_getDimPadLayerWithStrideOffset(&padT, &padB, &padL, &padR,
                                                            &(pOrgTIDLNetStructure.TIDLPCLayers[k].layerParams.convParams),
                                                            pOrgTIDLNetStructure.TIDLPCLayers[k].outData[0].dimValues[TIDL_DIM_HEIGHT],pOrgTIDLNetStructure.TIDLPCLayers[k].outData[0].dimValues[TIDL_DIM_WIDTH],
                                                            pOrgTIDLNetStructure.TIDLPCLayers[k].inData[0].dimValues[TIDL_DIM_HEIGHT], pOrgTIDLNetStructure.TIDLPCLayers[k].inData[0].dimValues[TIDL_DIM_WIDTH],
                                                            &(pOrgTIDLNetStructure.TIDLPCLayers[k].strideOffsetMethod));
                        pOrgTIDLNetStructure.TIDLPCLayers[k].layerParams.convParams.padW = 0;
                        pOrgTIDLNetStructure.TIDLPCLayers[k].layerParams.convParams.padH = 0;
                        pOrgTIDLNetStructure.TIDLPCLayers[numLayers].layerParams.padLayerParams.padType = TIDL_PadZero;

                        pOrgTIDLNetStructure.TIDLPCLayers[numLayers].layerType = TIDL_PadLayer;
                        pOrgTIDLNetStructure.TIDLPCLayers[numLayers].numInBufs = 1;
                        pOrgTIDLNetStructure.TIDLPCLayers[numLayers].numOutBufs = 1;
                        pOrgTIDLNetStructure.TIDLPCLayers[numLayers].weightsElementSizeInBits = NUM_WHGT_BITS;
                        strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[numLayers].outDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0]);
                        sprintf((char *)padLayerNumber, "%d", count);
                        strcat(strcat((char *)pOrgTIDLNetStructure.TIDLPCLayers[numLayers].outDataNames[0], "_pad_"), (char *)padLayerNumber);
                        count++;
                        strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[numLayers].inDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0]);
                        strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[k].inDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[numLayers].outDataNames[0]);
                        pOrgTIDLNetStructure.TIDLPCLayers[numLayers].outConsumerCnt[0] = 1;
                        pOrgTIDLNetStructure.TIDLPCLayers[numLayers].outConsumerLinked[0] = 1;
                        pOrgTIDLNetStructure.TIDLPCLayers[numLayers].actParams.actType = TIDL_NoAct;
                        pOrgTIDLNetStructure.TIDLPCLayers[numLayers].inData[0] = pOrgTIDLNetStructure.TIDLPCLayers[k].inData[l];
                        pOrgTIDLNetStructure.TIDLPCLayers[numLayers].outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[numLayers].inData[0];
                        pOrgTIDLNetStructure.TIDLPCLayers[numLayers].outData[0].dataId = (*dataIndex)++;
                        pOrgTIDLNetStructure.TIDLPCLayers[numLayers].outData[0].dimValues[TIDL_DIM_HEIGHT] = pOrgTIDLNetStructure.TIDLPCLayers[numLayers].outData[0].dimValues[TIDL_DIM_HEIGHT] + padT + padB;
                        pOrgTIDLNetStructure.TIDLPCLayers[numLayers].outData[0].dimValues[TIDL_DIM_WIDTH] = pOrgTIDLNetStructure.TIDLPCLayers[numLayers].outData[0].dimValues[TIDL_DIM_WIDTH] + padL + padR;
                        pOrgTIDLNetStructure.TIDLPCLayers[k].inData[l] = pOrgTIDLNetStructure.TIDLPCLayers[numLayers].outData[0];
                        pOrgTIDLNetStructure.TIDLPCLayers[numLayers].layerParams.padLayerParams.padT = padT;
                        pOrgTIDLNetStructure.TIDLPCLayers[numLayers].layerParams.padLayerParams.padB = padB;
                        pOrgTIDLNetStructure.TIDLPCLayers[numLayers].layerParams.padLayerParams.padL = padL;
                        pOrgTIDLNetStructure.TIDLPCLayers[numLayers].layerParams.padLayerParams.padR = padR;
                        pOrgTIDLNetStructure.TIDLPCLayers[numLayers].numMacs = 0;
                        numLayers++;
                      }
                    }
                  }
                }
              }
            }
          }
        }
        else
        {
          /* Add concat layer to nonMergeConcatList */
          nonMergeConcatList[nonMergeConcatLayerNum] = i;
          nonMergeConcatLayerNum++;
        }
      }
    }
  }

  /* Check if any new 1x1 convolutions are needed for YUV to RGB merge */
  if(nonMergeConcatLayerNum > 0)
  {
    int32_t newConvList[TIDL_NUM_MAX_LAYERS];
    memset(&newConvList[0], 0, sizeof(int32_t)*TIDL_NUM_MAX_LAYERS);

    /* Get the list of new 1x1 convolution layers */
    tidl_addConvLayerAfterConcat(pOrgTIDLNetStructure,
                                 numLayers,
                                 dataIndex,
                                 params,
                                 &nonMergeConcatList[0],
                                 &newConvList[0],
                                 nonMergeConcatLayerNum);

    /* Add all new convolutions to convLayerList */
    for(i1=0; i1<nonMergeConcatLayerNum; i1++)
    {
      convLayerList[(*numYuvMergeConvLayers)+i1] = newConvList[i1];
      (*numYuvMergeConvLayers)++;
    }
  }
  else
  {
    pOrgTIDLNetStructure.numLayers = numLayers; /* this is taken care of in tidl_addConvLayerAfterConcat, so need to do this here if tidl_addConvLayerAfterConcat is not called */
  }

  return 0;
}

int32_t tidl_addPadLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex, int32_t * dataIndex, tidl_import_config * params)
{
  int32_t i, j, k, l, x, m, n;
  int32_t padW, padH;
  int32_t idx;
  int32_t normParamIdx = 0;
  float32_tidl * scale, * bias;
  for (i = 0; i < layerIndex; i++)
  {
    int32_t count = 0;
    int8_t padLayerNumber[TIDL_STRING_SIZE];

    if ((((pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_DataLayer) && (pOrgTIDLNetStructure.TIDLPCLayers[i].numOutBufs > 0)) /* input data layer */
          && (gParams.inDataPadInTIDL[0] == 0)) ||
          (pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_PadLayer))
    {
      /* For input data layer, if inDataPadInTIDL = 0, no padding to be done */
      /* No padding if current layer is Pad layer to prevent recursive addition of pad layers */
    }
    else
    {
      padH = 0;
      padW = 0;
      /**** TODO : Currently have added separate pad layer for each consumer layer, can think of way to add common pad layer with max padding of consumers ***/

      /*** TODO : Pad information for one of the consuming layer can be updated to match the out shape : issue arises when we do max padding,
        but required individual padding for one of the consumers is less than the determined max pad, will need to update inDataParams correctly ****/

      for (j = 0; j < pOrgTIDLNetStructure.TIDLPCLayers[i].numOutBufs; j++)
      {
        for(k = 0; k < layerIndex; k++)
        {
          for (l = 0; l < pOrgTIDLNetStructure.TIDLPCLayers[k].numInBufs; l++)
          {
            if (pOrgTIDLNetStructure.TIDLPCLayers[i].outData[j].dataId == pOrgTIDLNetStructure.TIDLPCLayers[k].inData[l].dataId)
            {
              if (pOrgTIDLNetStructure.TIDLPCLayers[k].layerType == TIDL_PoolingLayer)
              {
                if((!((pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_DataLayer) && (pOrgTIDLNetStructure.TIDLPCLayers[i].numOutBufs > 0))))/* &&
                         ((pOrgTIDLNetStructure.TIDLPCLayers[k].layerParams.poolParams.poolingType == TIDL_AveragePooling) ||
                          (TIDL_getDatElementSign(pOrgTIDLNetStructure.TIDLPCLayers[i].outData[j].elementType) == 0)))*/
                {
                  /****** Above commented condition to be uncommented when we support pad before signed max pooling *******/
                  /* no padding required for average pooling if it is not a consumer of input data layer  */
                  /* padding required for average pooling only if it is consumer of input and inDataPadInTIDL = 1 */
                  /* No padding needed for internal unsigned max pooling, but will be padded irrespective of sign if it is consumer of input and inDataPadInTIDL = 1 */
                }
                else
                {
                  padW = pOrgTIDLNetStructure.TIDLPCLayers[k].layerParams.poolParams.padW;
                  padH = pOrgTIDLNetStructure.TIDLPCLayers[k].layerParams.poolParams.padH;


                  if((padH != 0) || (padW != 0)) /* no pad layer should be added if padH = padW = 0 */
                  {
                    pOrgTIDLNetStructure.TIDLPCLayers[k].layerParams.poolParams.padW = 0;
                    pOrgTIDLNetStructure.TIDLPCLayers[k].layerParams.poolParams.padH = 0;
                    pOrgTIDLNetStructure.TIDLPCLayers[k].layerParams.poolParams.padT = 0;
                    pOrgTIDLNetStructure.TIDLPCLayers[k].layerParams.poolParams.padB = 0;
                    pOrgTIDLNetStructure.TIDLPCLayers[k].layerParams.poolParams.padL = 0;
                    pOrgTIDLNetStructure.TIDLPCLayers[k].layerParams.poolParams.padR = 0;

                    /*** TODO : Take care of non zero padding for max pooling - modify below commented code for that case ***/
                    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.padLayerParams.padType = TIDL_PadZero;
                    /*if( pOrgTIDLNetStructure.TIDLPCLayers[k].layerParams.poolParams.poolingType == TIDL_MaxPooling)
                    {
                      int32_t dataSize;
                      dataSize = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH];
                      if(pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.padLayerParams.padType == TIDL_PadPerChannel)
                      {
                        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].perChannelPadConstTensor.ptr = my_malloc(dataSize*sizeof(float));
                        pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].perChannelPadConstTensor.bufSize = dataSize;
                        float *  perChannelPadConstTensor  = (float *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].perChannelPadConstTensor.ptr;
                        for (j = 0; j < dataSize; j++)
                        {
                          perChannelPadConstTensor[j] = -1.0 * bias[j] / scale[j];
                          normParamIdx++;
                        }
                      }
                    } */
                    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType = TIDL_PadLayer;
                    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs = 1;
                    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = 1;
                    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weightsElementSizeInBits = NUM_WHGT_BITS;
                    strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0]);
                    sprintf((char *)padLayerNumber, "%d", count);
                    strcat(strcat((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], "_pad_"), (char *)padLayerNumber);
                    count++;
                    strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0]);
                    strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[k].inDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0]);
                    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = 1;
                    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = 1;
                    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].actParams.actType = TIDL_NoAct;
                    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0] = pOrgTIDLNetStructure.TIDLPCLayers[k].inData[l];
                    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0];
                    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dataId = (*dataIndex)++;
                    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] + 2 * padH;
                    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] + 2 * padW;
                    pOrgTIDLNetStructure.TIDLPCLayers[k].inData[l] = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0];
                    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.padLayerParams.padT = padH;
                    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.padLayerParams.padB = padH;
                    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.padLayerParams.padL = padW;
                    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.padLayerParams.padR = padW;
                    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numMacs = 0;
                    layerIndex++;
                  }
                }
              }

              else if (((pOrgTIDLNetStructure.TIDLPCLayers[k].layerType == TIDL_ConvolutionLayer) ||
                     (pOrgTIDLNetStructure.TIDLPCLayers[k].layerType == TIDL_Deconv2DLayer)) &&
                     ((pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_DataLayer) && (pOrgTIDLNetStructure.TIDLPCLayers[i].numOutBufs > 0)))
                     /* add pad before convolution only if it is a consumer of input data layer and inDataPadInTIDL == 1 */
              {
                padW = pOrgTIDLNetStructure.TIDLPCLayers[k].layerParams.convParams.padW;
                padH = pOrgTIDLNetStructure.TIDLPCLayers[k].layerParams.convParams.padH;
                if((padH != 0) || (padW != 0)) /* no pad layer should be added if padH = padW = 0 */
                {
                  int32_t padT = padH, padB = padH;
                  int32_t padL = padW, padR = padW;
                  tidl_getDimPadLayerWithStrideOffset(&padT, &padB, &padL, &padR,
                                                      &(pOrgTIDLNetStructure.TIDLPCLayers[k].layerParams.convParams),
                                                      pOrgTIDLNetStructure.TIDLPCLayers[k].outData[0].dimValues[TIDL_DIM_HEIGHT],pOrgTIDLNetStructure.TIDLPCLayers[k].outData[0].dimValues[TIDL_DIM_WIDTH],
                                                      pOrgTIDLNetStructure.TIDLPCLayers[k].inData[0].dimValues[TIDL_DIM_HEIGHT], pOrgTIDLNetStructure.TIDLPCLayers[k].inData[0].dimValues[TIDL_DIM_WIDTH],
                                                      &(pOrgTIDLNetStructure.TIDLPCLayers[k].strideOffsetMethod));
                  pOrgTIDLNetStructure.TIDLPCLayers[k].layerParams.convParams.padW = 0;
                  pOrgTIDLNetStructure.TIDLPCLayers[k].layerParams.convParams.padH = 0;
                  pOrgTIDLNetStructure.TIDLPCLayers[k].layerParams.convParams.padL = 0;
                  pOrgTIDLNetStructure.TIDLPCLayers[k].layerParams.convParams.padR = 0;
                  pOrgTIDLNetStructure.TIDLPCLayers[k].layerParams.convParams.padT = 0;
                  pOrgTIDLNetStructure.TIDLPCLayers[k].layerParams.convParams.padB = 0;
                  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.padLayerParams.padType = TIDL_PadZero;

                  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType = TIDL_PadLayer;
                  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs = 1;
                  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = 1;
                  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weightsElementSizeInBits = NUM_WHGT_BITS;
                  strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0]);
                  sprintf((char *)padLayerNumber, "%d", count);
                  strcat(strcat((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], "_pad_"), (char *)padLayerNumber);
                  count++;
                  strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0]);
                  strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[k].inDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0]);
                  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = 1;
                  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = 1;
                  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].actParams.actType = TIDL_NoAct;
                  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0] = pOrgTIDLNetStructure.TIDLPCLayers[k].inData[l];
                  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inData[0];
                  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dataId = (*dataIndex)++;
                  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] + padT + padB;
                  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] + padL + padR;
                  pOrgTIDLNetStructure.TIDLPCLayers[k].inData[l] = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0];
                  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.padLayerParams.padT = padT;
                  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.padLayerParams.padB = padB;
                  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.padLayerParams.padL = padL;
                  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.padLayerParams.padR = padR;
                  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numMacs = 0;
                  layerIndex++;
                }
              }
            }
          }
        }
      }
    }
  }
  pOrgTIDLNetStructure.numLayers = layerIndex;

  return 0;
}

extern TIDL_TFSSDConfig_t tidl_TFSSDConfigs[TIDL_MAX_TF_SSD_LAYERS];
extern TIDL_TFFasterRCNNConfig_t tidl_TFFasterRCNNConfigs[TIDL_MAX_TF_FASTER_RCNN_LAYERS];

extern int32_t numTFMetaLayers;

int32_t tidl_addFasterRCNNLayersToNet(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t * numLayers, int32_t * dataIndex, tidl_import_config * params, int32_t detLayerId)
{

    uint32_t boxFlattenLayerId;
    uint32_t classFlattenLayerId;
    uint32_t featureLayerIdx;
    uint32_t secondStageLayerIdx;
    int32_t roiPoolingLayerIdx =0;
    int32_t detOutLayerIdx;
    int32_t secondStagePostProcessingLayerIdx;
    int32_t dataIdIdx =0;
    int32_t i, j;
    int32_t numHeads = 1;
    int32_t layerIndex;
    float widthStride;
    float heightStride;
    int32_t numOutputPerDataObject;
    sTIDL_DetectOutputParams_t * odPostProcessingParams;
    sTIDL_DetectOutputParams_t * doParams;
    if (numTFMetaLayers < 1)
    {
      printf("Could not find Meta Arch confg file \n");
      return -1;
    }
    TIDL_TFFasterRCNNConfig_t &tidl_TFFasterRCNNConfig = tidl_TFFasterRCNNConfigs[0];

    layerIndex = *numLayers;


    /* Add detection output layer, data id for this layer is assigned later which will be connected to ROI pooling layer */
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType = TIDL_DetectionOutputLayer;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs = 2;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weightsElementSizeInBits = NUM_WHGT_BITS;
    strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char*)&tidl_OdOutputNames[detLayerId][0]);
    strcat((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], "_det");
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = 0;

    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.detectOutParams.numKeypoints = 0;

    numOutputPerDataObject = 7;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].numDim       = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_BATCH] = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_DIM1] = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_DIM2] = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] = (sizeof(TIDL_ODLayerHeaderInfo)/sizeof(float32_tidl)) + tidl_TFFasterRCNNConfig.firstStageConfig.max_proposals * numOutputPerDataObject;/* Reusing same structure as ssd*/
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].elementType = TIDL_SinglePrecFloat;
    detOutLayerIdx = layerIndex;
    layerIndex++;

    doParams = &pOrgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].layerParams.detectOutParams;
    numHeads = 1;

    doParams->processingType = TIDL_processingTypeDetectionOutput;
    doParams->numClasses    = 2;
    doParams->topK              = tidl_TFFasterRCNNConfig.firstStageConfig.max_proposals;//:TODO: Check this
    doParams->keepTopK          = tidl_TFFasterRCNNConfig.firstStageConfig.max_proposals;//:TODO: Check this
    doParams->nmsThreshold      = tidl_TFFasterRCNNConfig.firstStageConfig.nms_iou_threshold;
    doParams->confThreshold     = tidl_TFFasterRCNNConfig.firstStageConfig.nms_score_threshold;
    doParams->backgroundLabelId = 0;
    doParams->codeType          = 0;
    doParams->varianceEncoded   = 0;
    doParams->eta               = 0;
    doParams->topKAllClasses    = -1;
    doParams->numKeypoints      = 0;
    doParams->shareLocation     = 1;
    doParams->imWidth  = params->inWidth[0];
    doParams->imHeight = params->inHeight[0];
    doParams->metaArchType = TIDL_metaArchTFFasterRcnn;
    doParams->numHeads     = numHeads;
    doParams->numSubHeads  = 1;

    orgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].priorBox.ptr    = (void*)malloc(sizeof(sTIDL_AnchorBoxParams_t)*numHeads);
    orgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].priorBox.bufSize = (sizeof(sTIDL_AnchorBoxParams_t)*numHeads)/sizeof(float);
    sTIDL_AnchorBoxParams_t *anchorBoxParams = (sTIDL_AnchorBoxParams_t*) orgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].priorBox.ptr;

    /* Find the data layer which matches the output data list as mentioned in config file */
    for (i = 0; i < layerIndex; i++)
    {
      if ((pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_DataLayer) && (pOrgTIDLNetStructure.TIDLPCLayers[i].numInBufs ==1)
         && (strcmp((const char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0],(const char *)outDataNames[detLayerId]) == 0))
      {
        int32_t dataSize;
        /* Rename this data layer to Flatten Layer */
        pOrgTIDLNetStructure.TIDLPCLayers[i].layerType  = TIDL_FlattenLayer;
        pOrgTIDLNetStructure.TIDLPCLayers[i].numInBufs  = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].numOutBufs = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0];
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dataId = (*dataIndex)++;/* assign a new output data id which will be connected to detection output layer*/
        strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].inDataNames[0]);
        strcat((char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0], "_flatten");


        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_BATCH] = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_DIM1] = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_DIM2] = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_HEIGHT] = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_WIDTH] = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_BATCH] *
                                                                       pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_NUMCH] *
                                                                       pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_HEIGHT] *
                                                                       pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_WIDTH];
        anchorBoxParams[0].headWidth  = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_WIDTH];
        anchorBoxParams[0].headHeight = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_HEIGHT];
        boxFlattenLayerId = i;
      }
    }

    for (i = 0; i < layerIndex; i++)
    {
      /* Store the feature layer index, to be used later */
      if (strcmp((const char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0], (const char *)"FirstStageFeatureExtractor/resnet_v1_50/resnet_v1_50/block3/unit_6/bottleneck_v1/Relu") == 0)
      {
        featureLayerIdx = i;
      }
      if (strcmp((const char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0], (const char *)"MaxPool2D/MaxPool") == 0)
      {
        secondStageLayerIdx = i;
      }
      if ((pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_DataLayer) && (pOrgTIDLNetStructure.TIDLPCLayers[i].numInBufs == 1)
        && (strcmp((const char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0], (const char *)outDataNames[1]) == 0))
      {
        int32_t dataSize;
        pOrgTIDLNetStructure.TIDLPCLayers[i].layerType = TIDL_FlattenLayer;
        pOrgTIDLNetStructure.TIDLPCLayers[i].numInBufs = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].numOutBufs = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0];
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dataId = (*dataIndex)++;
        strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].inDataNames[0]);
        strcat((char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0], "_flatten");

        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_BATCH] = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_DIM1] = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_DIM2] = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_HEIGHT] = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_WIDTH] = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_BATCH] *
          pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_NUMCH] *
          pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_HEIGHT] *
          pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_HEIGHT];
        classFlattenLayerId = i;
      }
    }

    pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dataId = (*dataIndex)++;

    strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].inDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[boxFlattenLayerId].outDataNames[0]);
    strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].inDataNames[1], (char *)pOrgTIDLNetStructure.TIDLPCLayers[classFlattenLayerId].outDataNames[0]);

    pOrgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].inData[0] = pOrgTIDLNetStructure.TIDLPCLayers[boxFlattenLayerId].outData[0];
    pOrgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].inData[1] = pOrgTIDLNetStructure.TIDLPCLayers[classFlattenLayerId].outData[0];
    pOrgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].outData[0].dataId = (*dataIndex)++;

    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType = TIDL_RoiPoolingLayer;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs = 2;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weightsElementSizeInBits = NUM_WHGT_BITS;
    strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], "tf_faster_rcnn_roi_pooling_layer");
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = 0;

    strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[featureLayerIdx].outDataNames[0]);
    strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[1], (char *)pOrgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].outDataNames[0]);

    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].numDim       = 1;//doubt
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_BATCH] = tidl_TFFasterRCNNConfig.firstStageConfig.max_proposals;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH] = (tidl_TFFasterRCNNConfig.initial_crop_size /  tidl_TFFasterRCNNConfig.maxpool_kernel_size);
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] = (tidl_TFFasterRCNNConfig.initial_crop_size /  tidl_TFFasterRCNNConfig.maxpool_kernel_size);
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] = pOrgTIDLNetStructure.TIDLPCLayers[featureLayerIdx].outData[0].dimValues[TIDL_DIM_NUMCH];
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].elementType = TIDL_SinglePrecFloat;
    roiPoolingLayerIdx = layerIndex;
    layerIndex++;

    pOrgTIDLNetStructure.TIDLPCLayers[roiPoolingLayerIdx].inData[0] = pOrgTIDLNetStructure.TIDLPCLayers[featureLayerIdx].outData[0];
    pOrgTIDLNetStructure.TIDLPCLayers[roiPoolingLayerIdx].inData[1] = pOrgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].outData[0];

    pOrgTIDLNetStructure.TIDLPCLayers[roiPoolingLayerIdx].outData[0]= pOrgTIDLNetStructure.TIDLPCLayers[secondStageLayerIdx].outData[0];
    pOrgTIDLNetStructure.TIDLPCLayers[roiPoolingLayerIdx].outConsumerCnt[0] = 2;
    pOrgTIDLNetStructure.TIDLPCLayers[roiPoolingLayerIdx].outConsumerLinked[0] = 2;
    strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[roiPoolingLayerIdx].outDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[secondStageLayerIdx].outDataNames[0]);

    /* Remove one of the layer which was created because of second part of the network */
    pOrgTIDLNetStructure.TIDLPCLayers[secondStageLayerIdx].numInBufs = -1;
    pOrgTIDLNetStructure.TIDLPCLayers[secondStageLayerIdx].numOutBufs = -1;

    /* Add second stage post processing layer, data id for this layer is assigned later which will be connected to ROI pooling layer */
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType = TIDL_OdPostProcessingLayer;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs = 3;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weightsElementSizeInBits = NUM_WHGT_BITS;
    strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], "tf_faster_rcnn_post_processing_layer");
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = 0;

    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dataId = (*dataIndex)++;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].numDim       = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_BATCH] = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_DIM1] = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_DIM2] = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] = 4 * tidl_TFFasterRCNNConfig.secondStageConfig.max_detections_per_class *
                                                                                                                            tidl_TFFasterRCNNConfig.secondStageConfig.max_total_detections;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].elementType = TIDL_SinglePrecFloat;
    secondStagePostProcessingLayerIdx = layerIndex;
    layerIndex++;

    odPostProcessingParams =
       &pOrgTIDLNetStructure.TIDLPCLayers[secondStagePostProcessingLayerIdx].layerParams.detectOutParams;
    odPostProcessingParams->processingType = TIDL_processingTypeOdPostProcessing;
    odPostProcessingParams->numClasses    = tidl_TFFasterRCNNConfig.num_classes;
    odPostProcessingParams->topK              = tidl_TFFasterRCNNConfig.secondStageConfig.max_detections_per_class;//:TODO: Check this
    odPostProcessingParams->keepTopK          = tidl_TFFasterRCNNConfig.secondStageConfig.max_total_detections;//:TODO: Check this
    odPostProcessingParams->nmsThreshold      = tidl_TFFasterRCNNConfig.secondStageConfig.nms_iou_threshold;
    odPostProcessingParams->confThreshold     = tidl_TFFasterRCNNConfig.secondStageConfig.nms_score_threshold;
    odPostProcessingParams->backgroundLabelId = 0;
    odPostProcessingParams->codeType          = 0;
    odPostProcessingParams->varianceEncoded   = 0;
    odPostProcessingParams->eta               = 0;
    odPostProcessingParams->topKAllClasses    = -1;
    odPostProcessingParams->numKeypoints      = 0;
    odPostProcessingParams->shareLocation     = 1;
    odPostProcessingParams->imWidth  = params->inWidth[0];
    odPostProcessingParams->imHeight = params->inHeight[0];
    odPostProcessingParams->metaArchType = TIDL_metaArchTFFasterRcnn;
    odPostProcessingParams->numHeads     = 1;
    odPostProcessingParams->numSubHeads  = 1;

    for (i = 0; i < layerIndex; i++)
    {
      if ((pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_DataLayer) && (pOrgTIDLNetStructure.TIDLPCLayers[i].numOutBufs == -1) &&
        (pOrgTIDLNetStructure.TIDLPCLayers[i].numInBufs == 1))
      {
        int32_t dataSize;
        pOrgTIDLNetStructure.TIDLPCLayers[i].numInBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].numOutBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[secondStagePostProcessingLayerIdx].inData[dataIdIdx] = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0];

        strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[secondStagePostProcessingLayerIdx].inDataNames[dataIdIdx], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].inDataNames[0]);

        dataIdIdx++;
      }
    }
    /* Connect detection output layer output to second stage post processing layer*/
    pOrgTIDLNetStructure.TIDLPCLayers[secondStagePostProcessingLayerIdx].inData[dataIdIdx] = pOrgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].outData[0];
    strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[secondStagePostProcessingLayerIdx].inDataNames[dataIdIdx], (char *)pOrgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].outDataNames[0]);

    sTIDL_RoiPoolingLayerParams_t * roiPoolingParams = &pOrgTIDLNetStructure.TIDLPCLayers[roiPoolingLayerIdx].layerParams.roiPoolingParams;
    roiPoolingParams->poolingType = TIDL_RoiPoolingFasterRCNN;
    roiPoolingParams->imWidth  =params->inWidth[0];
    roiPoolingParams->imHeight = params->inHeight[0];

    float scales[TIDL_MAX_AR_PER_HEAD];
    float aspect_ratios[TIDL_MAX_AR_PER_HEAD];
    int num_ars, min_im_shape;
    float ratio_sqrts;

    for (j = 0; j < tidl_TFFasterRCNNConfig.firstStageConfig.num_aspect_ratios; j++)
    {
      aspect_ratios[j] = tidl_TFFasterRCNNConfig.firstStageConfig.aspect_ratios[j];
    }

    for (j = 0; j < tidl_TFFasterRCNNConfig.firstStageConfig.num_scales; j++)
    {
      scales[j] = tidl_TFFasterRCNNConfig.firstStageConfig.scales[j];
    }

    num_ars = tidl_TFFasterRCNNConfig.firstStageConfig.num_aspect_ratios * tidl_TFFasterRCNNConfig.firstStageConfig.num_scales;

    widthStride =  tidl_TFFasterRCNNConfig.firstStageConfig.width_stride;
    heightStride = tidl_TFFasterRCNNConfig.firstStageConfig.height_stride;

    tidl_TFFasterRCNNConfig.firstStageConfig.width_stride =  widthStride;
    tidl_TFFasterRCNNConfig.firstStageConfig.height_stride = heightStride;

    anchorBoxParams[0].numAnchors = num_ars;
    anchorBoxParams[0].numKeyPoints =0;
    anchorBoxParams[0].strideW = tidl_TFFasterRCNNConfig.firstStageConfig.width_stride;
    anchorBoxParams[0].strideH = tidl_TFFasterRCNNConfig.firstStageConfig.height_stride;
    anchorBoxParams[0].offsetW =tidl_TFFasterRCNNConfig.firstStageConfig.width_offset;
    anchorBoxParams[0].offsetH = tidl_TFFasterRCNNConfig.firstStageConfig.height_offset;

    anchorBoxParams[0].boxScales[0] = 10.0;//tidl_TFSSDConfig.y_scale;
    anchorBoxParams[0].boxScales[1] = 10.0;//tidl_TFSSDConfig.x_scale;
    anchorBoxParams[0].boxScales[2] = 5.0;//tidl_TFSSDConfig.height_scale;
    anchorBoxParams[0].boxScales[3] = 5.0;//tidl_TFSSDConfig.width_scale;

    /* Need to fill
    anchorBoxParams[i].kpScales[0] = 0;
    */
    for ( j = 0; j < tidl_TFFasterRCNNConfig.firstStageConfig.num_aspect_ratios ; j++)
    {
      ratio_sqrts = sqrt(aspect_ratios[j]);
      for (i = 0; i < tidl_TFFasterRCNNConfig.firstStageConfig.num_scales; i++)
      {
        anchorBoxParams[0].boxHeight[i + (j * tidl_TFFasterRCNNConfig.firstStageConfig.num_scales)] = (scales[i] / ratio_sqrts) * tidl_TFFasterRCNNConfig.firstStageConfig.height;
        anchorBoxParams[0].boxWidth[i + (j * tidl_TFFasterRCNNConfig.firstStageConfig.num_scales)]  = scales[i] * ratio_sqrts * tidl_TFFasterRCNNConfig.firstStageConfig.width;
      }
    }
    *numLayers = layerIndex;

  return 0;
}

int32_t tidl_addTidlSSDPostProc(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t * numLayers, int32_t * dataIndex, tidl_import_config * params, int32_t detLayerId)
{

  int32_t i, j, k;
  int32_t idx;
  int32_t normParamIdx = 0;
  int32_t numHeads = 6;
  int32_t hidx = 0;
  int32_t keep_top_k = 40;
  int32_t numOutDataPerObject;
  int32_t boxCatLayerIdx = 0;
  int32_t classCatLayerIdx = 0;
  int32_t detOutLayerIdx = 0;
  int32_t dataOutLayerIdx = 0;
  int32_t layerIndex = *numLayers;

  numHeads = tidl_OdPostProc[detLayerId].numHeads;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType  = TIDL_ConcatLayer;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs  = numHeads;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = 1;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weightsElementSizeInBits = NUM_WHGT_BITS;
  strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], "tidl_ssd_box_conat_layer");
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = 1;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = 1;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.concatParams.axis = TIDL_DIM_WIDTH;
  boxCatLayerIdx = layerIndex;
  layerIndex++;

  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType = TIDL_ConcatLayer;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs = numHeads;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = 1;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weightsElementSizeInBits = NUM_WHGT_BITS;
  strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], "tidl_ssd_class_conat_layer");
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = 1;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = 1;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.concatParams.axis = TIDL_DIM_WIDTH;
  classCatLayerIdx = layerIndex;
  layerIndex++;

  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType = TIDL_DetectionOutputLayer;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs = 2;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = 1;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weightsElementSizeInBits = NUM_WHGT_BITS;
  strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char*)&tidl_OdOutputNames[detLayerId][0]);
  strcat((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], "_det"); /* Add "_det" to make sure name of detection layer is different from name of output reformat layer */
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = 1;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = 0;

  strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex - 2].outDataNames[0]);
  strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[1], (char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex - 1].outDataNames[0]);

   sTIDL_DetectOutputParams_t * doParams =
    &pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.detectOutParams;
  *doParams =  tidl_OdPostProc[detLayerId];
  doParams->numHeads     = numHeads;

  numOutDataPerObject = ((sizeof(TIDL_ODLayerObjInfo) - sizeof(TIDL_ODLayerKeyPoint) * TIDL_OD_MAX_KEY_POINTS)
                            + sizeof(TIDL_ODLayerKeyPoint) * pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.detectOutParams.numKeypoints)/sizeof(float32_tidl);

  if (pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.detectOutParams.subCodeType == TIDL_ObjectPose)
  {
    numOutDataPerObject += sizeof(TIDL_ODLayerObjectPose)/sizeof(float32_tidl);
  }

  keep_top_k = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.detectOutParams.keepTopK;

  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].numDim       = 1;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_BATCH] = 1;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_DIM1] = 1;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_DIM2] = 1;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] = 1;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] = (sizeof(TIDL_ODLayerHeaderInfo)/sizeof(float32_tidl)) + keep_top_k*numOutDataPerObject;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].elementType = TIDL_SinglePrecFloat;
  detOutLayerIdx = layerIndex;
  layerIndex++;

  if(gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_ONNX)
  {
    /** ONNX runtime assumes no output data layer is added to the network before tidl_AddOnnxODOutputLayers function is called
     * It reads the output names from meta arch file, and adds corresponding output data layers and output reformat layers
     * So perform this output data layer addition only for ONNX models and make sure the output data layer is not added for runtimes */

    /* TODO : Add data layer protected for runtimes for other meta arch types as well */
    orgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType = TIDL_DataLayer;
    orgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs = 1;
    orgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = -1;
    strcpy((char*)orgTIDLNetStructure.TIDLPCLayers[layerIndex].name, (char*)&tidl_OdOutputNames[detLayerId][0]);
    strcpy((char *)orgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0], (char*)&tidl_OdOutputNames[detLayerId][0]);
    strcpy((char *)orgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char*)&tidl_OdOutputNames[detLayerId][0]);
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].numDim       = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_BATCH] = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_DIM1] = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_DIM2] = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] = (sizeof(TIDL_ODLayerHeaderInfo)/sizeof(float32_tidl)) + keep_top_k*numOutDataPerObject;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].elementType = TIDL_SinglePrecFloat;
    strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex - 1].outDataNames[0]);
    dataOutLayerIdx = layerIndex;
    layerIndex++;
  }

  orgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].priorBox.ptr    = (void*)tidlPriorPtrs[detLayerId];
  orgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].priorBox.bufSize = doParams->priorBoxSize;
  sTIDL_AnchorBoxParams_t *anchorBoxParams = (sTIDL_AnchorBoxParams_t*) orgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].priorBox.ptr;
  for (j = 0; j < numHeads; j++)
  {
    for (i = 0; i < layerIndex; i++)
    {
      if ((pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_DataLayer) && (pOrgTIDLNetStructure.TIDLPCLayers[i].numInBufs ==1)
          && (strcmp((const char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0],(const char *)tidl_OdBoxInputNames[detLayerId][hidx]) == 0))
      {
        int32_t dataSize;
        pOrgTIDLNetStructure.TIDLPCLayers[i].layerType  = TIDL_FlattenLayer;
        pOrgTIDLNetStructure.TIDLPCLayers[i].numInBufs  = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].numOutBufs = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0];
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dataId = (*dataIndex)++;
        strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].inDataNames[0]);
        strcat((char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0], "_flatten");

        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_BATCH] = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_DIM1] = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_DIM2] = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_HEIGHT] = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_WIDTH] = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_BATCH] *
                                                                        pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_NUMCH] *
                                                                        pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_HEIGHT] *
                                                                        pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_WIDTH];
        pOrgTIDLNetStructure.TIDLPCLayers[boxCatLayerIdx].inData[hidx] = pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0];
        strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[boxCatLayerIdx].inDataNames[hidx], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0]);

        anchorBoxParams[hidx].headWidth  = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_WIDTH];
        anchorBoxParams[hidx].headHeight = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_HEIGHT];

        if((tidl_OdPostProc[0].metaArchType == TIDL_metaArchTIDLRetinaNet) ||
          (tidl_OdPostProc[0].metaArchType == TIDL_metaArchTIDLYoloV8)
        )
        {
          anchorBoxParams[hidx].strideW = (int32_t)(tidl_OdPostProc[0].imWidth  / anchorBoxParams[hidx].headWidth);
          anchorBoxParams[hidx].strideH = (int32_t)(tidl_OdPostProc[0].imHeight / anchorBoxParams[hidx].headHeight);
          for(k = 0; k < anchorBoxParams[hidx].numAnchors; k++)
          {
            anchorBoxParams[hidx].boxWidth[k]  *= anchorBoxParams[hidx].strideW;
            anchorBoxParams[hidx].boxHeight[k] *= anchorBoxParams[hidx].strideH;
          }

          anchorBoxParams[hidx].strideW = (1.0f  / ((float)anchorBoxParams[hidx].headWidth));
          anchorBoxParams[hidx].strideH = (1.0f  / ((float)anchorBoxParams[hidx].headHeight));

        }

        hidx++;
      }
    }
  }
  if (numHeads != hidx)
  {
      printf("Could not find all the inputs of tidl_ssd_detection_output_layer in the base network \n");
      exit(-1);
  }
  hidx = 0;
  for (j = 0; j < numHeads; j++)
  {
    for (i = 0; i < layerIndex; i++)
    {
      if ((pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_DataLayer) && (pOrgTIDLNetStructure.TIDLPCLayers[i].numInBufs == 1)
        && (strcmp((const char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0], (const char *)tidl_OdClassInputNames[detLayerId][hidx]) == 0))
      {
        int32_t dataSize;
        pOrgTIDLNetStructure.TIDLPCLayers[i].layerType = TIDL_FlattenLayer;
        pOrgTIDLNetStructure.TIDLPCLayers[i].numInBufs = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].numOutBufs = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0];
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dataId = (*dataIndex)++;
        strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].inDataNames[0]);
        strcat((char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0], "_flatten");

        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_BATCH] = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_DIM1] = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_DIM2] = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_HEIGHT] = 1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_WIDTH] = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_BATCH] *
          pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_NUMCH] *
          pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_HEIGHT] *
          pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_WIDTH];
        pOrgTIDLNetStructure.TIDLPCLayers[classCatLayerIdx].inData[hidx] = pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0];
        strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[classCatLayerIdx].inDataNames[hidx], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0]);
        hidx++;
      }
    }
  }
  if (numHeads != hidx)
  {
      printf("Could not find all the inputs of tidl_ssd_detection_output_layer in the base network \n");
      exit(-1);
  }
  pOrgTIDLNetStructure.TIDLPCLayers[boxCatLayerIdx].outData[0]    = pOrgTIDLNetStructure.TIDLPCLayers[boxCatLayerIdx].inData[0];
  pOrgTIDLNetStructure.TIDLPCLayers[classCatLayerIdx].outData[0]  = pOrgTIDLNetStructure.TIDLPCLayers[classCatLayerIdx].inData[0];
  for (i = 1; i < numHeads; i++)
  {
    pOrgTIDLNetStructure.TIDLPCLayers[boxCatLayerIdx].outData[0].dimValues[TIDL_DIM_WIDTH]   += pOrgTIDLNetStructure.TIDLPCLayers[boxCatLayerIdx].inData[i].dimValues[TIDL_DIM_WIDTH];
    pOrgTIDLNetStructure.TIDLPCLayers[classCatLayerIdx].outData[0].dimValues[TIDL_DIM_WIDTH] += pOrgTIDLNetStructure.TIDLPCLayers[classCatLayerIdx].inData[i].dimValues[TIDL_DIM_WIDTH];
  }
  pOrgTIDLNetStructure.TIDLPCLayers[boxCatLayerIdx].outData[0].dataId = (*dataIndex)++;
  pOrgTIDLNetStructure.TIDLPCLayers[classCatLayerIdx].outData[0].dataId = (*dataIndex)++;

  pOrgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].inData[0] = pOrgTIDLNetStructure.TIDLPCLayers[boxCatLayerIdx].outData[0];
  pOrgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].inData[1] = pOrgTIDLNetStructure.TIDLPCLayers[classCatLayerIdx].outData[0];
  pOrgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].outData[0].dataId = (*dataIndex)++;

  if(gParams.modelType == TIDL_IMPORT_MODEL_FORMAT_ONNX)
  {
    pOrgTIDLNetStructure.TIDLPCLayers[dataOutLayerIdx].inData[0] = pOrgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].outData[0];
    pOrgTIDLNetStructure.TIDLPCLayers[dataOutLayerIdx].outData[0].dataId = (*dataIndex)++;
  }

  *numLayers = layerIndex;
  return 0;
}


int32_t tidl_addTidlYoloPostProc(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t * numLayers, int32_t * dataIndex, tidl_import_config * params, int32_t detLayerId)
{

  int32_t i, j;
  int32_t idx;
  int32_t normParamIdx = 0;
  int32_t numHeads = 6;
  int32_t hidx = 0;
  int32_t keep_top_k = 40;
  int32_t numOutDataPerObject;
  int32_t boxCatLayerIdx = 0;
  int32_t classCatLayerIdx = 0;
  int32_t detOutLayerIdx = 0;
  int32_t layerIndex = *numLayers;

  numHeads = tidl_OdPostProc[detLayerId].numHeads;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType = TIDL_DetectionOutputLayer;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs = numHeads;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = 1;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weightsElementSizeInBits = NUM_WHGT_BITS;
  strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char*)&tidl_OdOutputNames[detLayerId][0]);
  strcat((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], "_det"); /* Add "_det" to make sure name of detection layer is different from name of output reformat layer */
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = 1;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = 0;


   sTIDL_DetectOutputParams_t * doParams =
    &pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.detectOutParams;
  *doParams =  tidl_OdPostProc[detLayerId];
   // Each keypoint is defined by (x,y,conf) at output
  numOutDataPerObject = ((sizeof(TIDL_ODLayerObjInfo) - sizeof(TIDL_ODLayerKeyPoint) * TIDL_OD_MAX_KEY_POINTS)
                            + sizeof(TIDL_ODLayerKeyPoint) * pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.detectOutParams.numKeypoints)/sizeof(float32_tidl);
  if (pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.detectOutParams.subCodeType == TIDL_ObjectPose)
  {
    numOutDataPerObject += sizeof(TIDL_ODLayerObjectPose)/sizeof(float32_tidl);
  }
  keep_top_k = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.detectOutParams.keepTopK;

  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].numDim       = 1;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_BATCH] = 1;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_DIM1] = 1;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_DIM2] = 1;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] = 1;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] = (sizeof(TIDL_ODLayerHeaderInfo)/sizeof(float32_tidl)) + keep_top_k*numOutDataPerObject;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].elementType = TIDL_SinglePrecFloat;
  detOutLayerIdx = layerIndex;
  layerIndex++;

  orgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].priorBox.ptr    = (void*)tidlPriorPtrs[detLayerId];
  orgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].priorBox.bufSize = doParams->priorBoxSize;
  sTIDL_AnchorBoxParams_t *anchorBoxParams = (sTIDL_AnchorBoxParams_t*) orgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].priorBox.ptr;

  for (j = 0; j < numHeads; j++)
  {
    for (i = 0; i < layerIndex; i++)
    {
      if ((pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_DataLayer) && (pOrgTIDLNetStructure.TIDLPCLayers[i].numInBufs == 1)
        && (strcmp((const char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0], (const char *)tidl_OdBoxInputNames[detLayerId][hidx]) == 0))
      {
        int32_t dataSize;
        pOrgTIDLNetStructure.TIDLPCLayers[i].layerType = TIDL_FlattenLayer;
        pOrgTIDLNetStructure.TIDLPCLayers[i].numInBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].numOutBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].inData[hidx] = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0];
        strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].inDataNames[hidx], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].inDataNames[0]);

        anchorBoxParams[hidx].headWidth = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_WIDTH];
        anchorBoxParams[hidx].headHeight = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_HEIGHT];
        hidx++;
      }
    }
  }
  if (numHeads != hidx)
  {
      printf("Could not find all the inputs of tidl_yol_od_output_layer in the base network \n");
      exit(-1);
  }

  pOrgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].outData[0].dataId = (*dataIndex)++;
  *numLayers = layerIndex;
  return 0;
}


int32_t tidl_addTidl3DODPostProc(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t * numLayers, int32_t * dataIndex, tidl_import_config * params, int32_t detLayerId)
{

  int32_t i, j;
  int32_t idx;
  int32_t normParamIdx = 0;
  int32_t numHeads,numAnchors;
  int32_t hidx = 0;
  int32_t inBufidx = 0;
  int32_t keep_top_k = tidl_OdPostProc[0].keepTopK;
  int32_t numOutDataPerObject;
  int32_t boxCatLayerIdx = 0;
  int32_t classCatLayerIdx = 0;
  int32_t detOutLayerIdx = 0;
  int32_t layerIndex = *numLayers;

  numHeads = tidl_OdPostProc[detLayerId].numHeads; // equal to number of box_input_size()
  numAnchors = tidl_OdPostProc[detLayerId].numClasses; // equal to num param layers
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType = TIDL_DetectionOutputLayer;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs = 3; // 3 because of box, class, and dir input
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = 1;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weightsElementSizeInBits = NUM_WHGT_BITS;
  strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], (char*)&tidl_OdOutputNames[detLayerId][0]);
  strcat((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], "_det"); /* Add "_det" to make sure name of detection layer is different from name of output reformat layer */
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = 1;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = 0;

   sTIDL_DetectOutputParams_t * doParams =
    &pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.detectOutParams;
  *doParams =  tidl_OdPostProc[detLayerId];

  /*10 prameter for 3D object. objId, x, y, z, l, w, h, angle, cls, score */
  numOutDataPerObject = sizeof(TIDL_3DODLayerObjInfo)/sizeof(float32_tidl);
  keep_top_k = pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.detectOutParams.keepTopK;

  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].numDim       = 1;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_BATCH] = 1;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_DIM1] = 1;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_DIM2] = 1;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] = 1;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] = keep_top_k*numOutDataPerObject;
  pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].elementType = TIDL_SinglePrecFloat;
  detOutLayerIdx = layerIndex;
  layerIndex++;

  orgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].priorBox.ptr    = (void*)tidlPriorPtrs[detLayerId];
  orgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].priorBox.bufSize = doParams->priorBoxSize;
  sTIDL_AnchorBoxParams_t *anchorBoxParams = (sTIDL_AnchorBoxParams_t*) orgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].priorBox.ptr;

  for (j = 0; j < numHeads; j++)
  {
    for (i = 0; i < layerIndex; i++)
    {
      if ((pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_DataLayer) && (pOrgTIDLNetStructure.TIDLPCLayers[i].numInBufs == 1)
        && (strcmp((const char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0], (const char *)tidl_OdBoxInputNames[detLayerId][hidx]) == 0))
      {
        pOrgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].inData[inBufidx] = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0];
        strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].inDataNames[inBufidx], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].inDataNames[0]);

        anchorBoxParams[hidx].headWidth = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_WIDTH];
        anchorBoxParams[hidx].headHeight = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_HEIGHT];
        hidx++;
        inBufidx++;
        /** removing the data layer */
        pOrgTIDLNetStructure.TIDLPCLayers[i].layerType = TIDL_FlattenLayer;
        pOrgTIDLNetStructure.TIDLPCLayers[i].numInBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].numOutBufs = -1;
        break;
      }
    }
  }

  for(; hidx < numAnchors; hidx++)
  {
    anchorBoxParams[hidx].headWidth  = anchorBoxParams[0].headWidth;
    anchorBoxParams[hidx].headHeight = anchorBoxParams[0].headHeight;
  }

  hidx = 0;
  for (j = 0; j < numHeads; j++)
  {
    for (i = 0; i < layerIndex; i++)
    {
      if ((pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_DataLayer) && (pOrgTIDLNetStructure.TIDLPCLayers[i].numInBufs == 1)
        && (strcmp((const char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0], (const char *)tidl_OdClassInputNames[detLayerId][hidx]) == 0))
      {
        pOrgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].inData[inBufidx] = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0];
        strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].inDataNames[inBufidx], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].inDataNames[0]);
        hidx++;
        inBufidx++;
        /** removing the data layer */
        pOrgTIDLNetStructure.TIDLPCLayers[i].layerType = TIDL_FlattenLayer;
        pOrgTIDLNetStructure.TIDLPCLayers[i].numInBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].numOutBufs = -1;
        break;
      }
    }
  }

  hidx = 0;
  for (j = 0; j < numHeads; j++)
  {
    for (i = 0; i < layerIndex; i++)
    {
      if ((pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_DataLayer) && (pOrgTIDLNetStructure.TIDLPCLayers[i].numInBufs == 1)
        && (strcmp((const char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0], (const char *)tidl_OdDirInputNames[detLayerId][hidx]) == 0))
      {
        pOrgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].inData[inBufidx] = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0];
        strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].inDataNames[inBufidx], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].inDataNames[0]);
        hidx++;
        inBufidx++;
        /** removing the data layer */
        pOrgTIDLNetStructure.TIDLPCLayers[i].layerType = TIDL_FlattenLayer;
        pOrgTIDLNetStructure.TIDLPCLayers[i].numInBufs = -1;
        pOrgTIDLNetStructure.TIDLPCLayers[i].numOutBufs = -1;
        break;
      }
    }
  }

  if (numHeads*3 != inBufidx) // 3 because of box, class and dir inputs
  {
      printf("Could not find all the inputs of tidl_3d_od_output_layer in the base network \n");
      exit(-1);
  }

  pOrgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].outData[0].dataId = (*dataIndex)++;
  *numLayers = layerIndex;
  return 0;
 }


int32_t tidl_addMetaArchLayersToNet(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex, int32_t * dataIndex, tidl_import_config * params, int32_t detLayerId)
{
  int32_t i, j;
  int32_t idx;
  int32_t normParamIdx = 0;
  int32_t numHeads = 6;
  int32_t hidx = 0;
  int32_t keep_top_k = 40;
  int32_t numOutDataPerObject;
  int32_t boxCatLayerIdx = 0;
  int32_t classCatLayerIdx = 0;
  int32_t detOutLayerIdx = 0;
  int32_t anchorIdx = 0;


  if (params->metaArchType == TIDL_metaArchTFSSD || params->metaArchType == TIDL_metaArchTfliteRtSsdDetectionLayer)
  {
    if (numTFMetaLayers < 1)
    {
      printf("Could not find Meta Arch confg file \n");
      return -1;
    }
    TIDL_TFSSDConfig_t &tidl_TFSSDConfig = tidl_TFSSDConfigs[detLayerId];
    numHeads = tidl_TFSSDConfig.num_layers;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType  = TIDL_ConcatLayer;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs  = numHeads;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weightsElementSizeInBits = NUM_WHGT_BITS;
    strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], "tf_ssd_box_conat_layer");
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.concatParams.axis = TIDL_DIM_WIDTH;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].actParams.actType = TIDL_NoAct;
    boxCatLayerIdx = layerIndex;
    layerIndex++;

    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType = TIDL_ConcatLayer;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs = numHeads;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weightsElementSizeInBits = NUM_WHGT_BITS;
    strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], "tf_ssd_class_conat_layer");
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.concatParams.axis = TIDL_DIM_WIDTH;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].actParams.actType = TIDL_NoAct;

    classCatLayerIdx = layerIndex;
    layerIndex++;

    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerType = TIDL_DetectionOutputLayer;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numInBufs = 2;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].numOutBufs = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].weightsElementSizeInBits = NUM_WHGT_BITS;
    strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outDataNames[0], "tf_ssd_detection_output_layer");
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerCnt[0] = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outConsumerLinked[0] = 0;

    strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex - 2].outDataNames[0]);
    strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].inDataNames[1], (char *)pOrgTIDLNetStructure.TIDLPCLayers[layerIndex - 1].outDataNames[0]);

    sTIDL_DetectOutputParams_t * doParams =
      &pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.detectOutParams;

    doParams->numClasses        = tidl_TFSSDConfig.num_classes;
    doParams->topK              = tidl_TFSSDConfig.max_detections_per_class;
    doParams->keepTopK          = tidl_TFSSDConfig.max_total_detections;
    doParams->nmsThreshold      = tidl_TFSSDConfig.iou_threshold;
    doParams->confThreshold     = tidl_TFSSDConfig.score_threshold;
    doParams->backgroundLabelId = tidl_TFSSDConfig.encode_background_as_zeros ? 0 : -1;
    doParams->codeType          = 2;
    doParams->varianceEncoded   = 0;
    doParams->eta               = 1.0;
    doParams->topKAllClasses    = -1;
    doParams->numKeypoints      = 0;
    doParams->shareLocation     = 1;
    doParams->imWidth  = params->inWidth[0];
    doParams->imHeight = params->inHeight[0];
    doParams->metaArchType = TIDL_metaArchTFSSD;
    doParams->numHeads     = numHeads;
    doParams->numSubHeads  = 1;
    doParams->scoreConverter = tidl_TFSSDConfig.score_converter;
    doParams->dataLayout = 0;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.detectOutParams.numKeypoints = 0;

    numOutDataPerObject = ((sizeof(TIDL_ODLayerObjInfo) - sizeof(TIDL_ODLayerKeyPoint) * TIDL_OD_MAX_KEY_POINTS)
                            + sizeof(TIDL_ODLayerKeyPoint) * pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.detectOutParams.numKeypoints)/sizeof(float32_tidl);

    if (pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].layerParams.detectOutParams.subCodeType == TIDL_ObjectPose)
    {
    numOutDataPerObject += sizeof(TIDL_ODLayerObjectPose)/sizeof(float32_tidl);
    }

    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].numDim       = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_BATCH] = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_DIM1] = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_DIM2] = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_HEIGHT] = 1;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].dimValues[TIDL_DIM_WIDTH] = (sizeof(TIDL_ODLayerHeaderInfo)/sizeof(float32_tidl)) + doParams->keepTopK *numOutDataPerObject;
    pOrgTIDLNetStructure.TIDLPCLayers[layerIndex].outData[0].elementType = TIDL_SinglePrecFloat;
    detOutLayerIdx = layerIndex;
    layerIndex++;



    orgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].priorBox.ptr    = (void*)malloc(sizeof(sTIDL_AnchorBoxParams_t)*numHeads);
    orgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].priorBox.bufSize = (sizeof(sTIDL_AnchorBoxParams_t)*numHeads)/sizeof(float);
    sTIDL_AnchorBoxParams_t *anchorBoxParams = (sTIDL_AnchorBoxParams_t*) orgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].priorBox.ptr;
    float * anchorBoxes = tidl_TFSSDConfig.anchorInputsFromModel;
    for (j = 0; j < numHeads; j++)
    {
      for (i = 0; i < layerIndex; i++)
      {
        if ((pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_DataLayer) && (pOrgTIDLNetStructure.TIDLPCLayers[i].numInBufs ==1)
          && (strcmp((const char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0],(const char *)outDataNames[2*hidx]) == 0))
        {
          int32_t dataSize;
          pOrgTIDLNetStructure.TIDLPCLayers[i].layerType  = TIDL_FlattenLayer;
          pOrgTIDLNetStructure.TIDLPCLayers[i].actParams.actType = TIDL_NoAct;

          pOrgTIDLNetStructure.TIDLPCLayers[i].numInBufs  = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[i].numOutBufs = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0];
          pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dataId = (*dataIndex)++;
          strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].inDataNames[0]);
          strcat((char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0], "_flatten");

          pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_BATCH] = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_DIM1] = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_DIM2] = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_HEIGHT] = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_WIDTH] = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_BATCH] *
                                                                        pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_NUMCH] *
                                                                        pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_HEIGHT] *
                                                                        pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_WIDTH];
          pOrgTIDLNetStructure.TIDLPCLayers[boxCatLayerIdx].inData[hidx] = pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0];
          strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[boxCatLayerIdx].inDataNames[hidx], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0]);
          anchorBoxParams[hidx].headWidth  = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_WIDTH];
          anchorBoxParams[hidx].headHeight = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_HEIGHT];
          hidx++;
        }
      }
    }
    if (numHeads != hidx)
    {
        printf("Could not find all the inputs of tf_ssd_detection_output_layer in the base network \n");
        exit(-1);
    }
    hidx = 0;
    for (j = 0; j < numHeads; j++)
    {
      for (i = 0; i < layerIndex; i++)
      {
        if ((pOrgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_DataLayer) && (pOrgTIDLNetStructure.TIDLPCLayers[i].numInBufs == 1)
          && (strcmp((const char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0], (const char *)outDataNames[2*hidx+1]) == 0))
        {
          int32_t dataSize;
          pOrgTIDLNetStructure.TIDLPCLayers[i].layerType = TIDL_FlattenLayer;
          pOrgTIDLNetStructure.TIDLPCLayers[i].actParams.actType = TIDL_NoAct;
          pOrgTIDLNetStructure.TIDLPCLayers[i].numInBufs = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[i].numOutBufs = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0] = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0];
          pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dataId = (*dataIndex)++;
          strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].inDataNames[0]);
          strcat((char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0], "_flatten");

          pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_BATCH] = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_DIM1] = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_DIM2] = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_NUMCH] = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_HEIGHT] = 1;
          pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_WIDTH] = pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_BATCH] *
            pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_NUMCH] *
            pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_HEIGHT] *
            pOrgTIDLNetStructure.TIDLPCLayers[i].inData[0].dimValues[TIDL_DIM_HEIGHT];
          pOrgTIDLNetStructure.TIDLPCLayers[classCatLayerIdx].inData[hidx] = pOrgTIDLNetStructure.TIDLPCLayers[i].outData[0];
          strcpy((char *)pOrgTIDLNetStructure.TIDLPCLayers[classCatLayerIdx].inDataNames[hidx], (char *)pOrgTIDLNetStructure.TIDLPCLayers[i].outDataNames[0]);
          hidx++;
        }
      }
    }
    if (numHeads != hidx)
    {
        printf("Could not find all the inputs of tf_ssd_detection_output_layer in the base network \n");
        exit(-1);
    }

    pOrgTIDLNetStructure.TIDLPCLayers[boxCatLayerIdx].outData[0]    = pOrgTIDLNetStructure.TIDLPCLayers[boxCatLayerIdx].inData[0];
    pOrgTIDLNetStructure.TIDLPCLayers[classCatLayerIdx].outData[0]  = pOrgTIDLNetStructure.TIDLPCLayers[classCatLayerIdx].inData[0];
    for (i = 1; i < numHeads; i++)
    {
      pOrgTIDLNetStructure.TIDLPCLayers[boxCatLayerIdx].outData[0].dimValues[TIDL_DIM_WIDTH]   += pOrgTIDLNetStructure.TIDLPCLayers[boxCatLayerIdx].inData[i].dimValues[TIDL_DIM_WIDTH];
      pOrgTIDLNetStructure.TIDLPCLayers[classCatLayerIdx].outData[0].dimValues[TIDL_DIM_WIDTH] += pOrgTIDLNetStructure.TIDLPCLayers[classCatLayerIdx].inData[i].dimValues[TIDL_DIM_WIDTH];
    }
    pOrgTIDLNetStructure.TIDLPCLayers[boxCatLayerIdx].outData[0].dataId = (*dataIndex)++;
    pOrgTIDLNetStructure.TIDLPCLayers[classCatLayerIdx].outData[0].dataId = (*dataIndex)++;

    pOrgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].inData[0] = pOrgTIDLNetStructure.TIDLPCLayers[boxCatLayerIdx].outData[0];
    pOrgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].inData[1] = pOrgTIDLNetStructure.TIDLPCLayers[classCatLayerIdx].outData[0];
    pOrgTIDLNetStructure.TIDLPCLayers[detOutLayerIdx].outData[0].dataId = (*dataIndex)++;


    float scales[TIDL_MAX_AR_PER_HEAD];
    float aspect_ratios[TIDL_MAX_AR_PER_HEAD];
    int num_ars, min_im_shape;
    float ratio_sqrts;

    min_im_shape = doParams->imHeight < doParams->imWidth ? doParams->imHeight : doParams->imWidth;
    tidl_TFSSDConfig.base_anchor_height = (min_im_shape / doParams->imHeight) * tidl_TFSSDConfig.base_anchor_height;
    tidl_TFSSDConfig.base_anchor_width = (min_im_shape / doParams->imWidth) * tidl_TFSSDConfig.base_anchor_width;

    for (i = 0; i < numHeads; i++)
    {
      if ((i == 0) && (tidl_TFSSDConfig.reduce_boxes_in_lowest_layer))
      {
        num_ars = 3;
        scales[0] = 0.1;
        aspect_ratios[0] = 1.0;
        scales[1] = tidl_TFSSDConfig.scales[i];
        aspect_ratios[1] = 2.0;
        scales[2] = tidl_TFSSDConfig.scales[i];
        aspect_ratios[2] = 0.5;
      }
      else
      {
        for (j = 0; j < tidl_TFSSDConfig.num_aspect_ratios; j++)
        {
          scales[j] = tidl_TFSSDConfig.scales[i];
          aspect_ratios[j] = tidl_TFSSDConfig.aspect_ratios[j];
        }
        num_ars = tidl_TFSSDConfig.num_aspect_ratios;
        if (tidl_TFSSDConfig.interpolated_scale_aspect_ratio > 0.0)
        {
          scales[num_ars] = sqrt(tidl_TFSSDConfig.scales[i] * tidl_TFSSDConfig.scales[i + 1]);
          aspect_ratios[num_ars] = tidl_TFSSDConfig.interpolated_scale_aspect_ratio;
          num_ars += 1;
        }
      }
      tidl_TFSSDConfig.width_stride[i] = 1.0 / anchorBoxParams[i].headWidth;
      tidl_TFSSDConfig.height_stride[i] = 1.0 / anchorBoxParams[i].headHeight;
      tidl_TFSSDConfig.width_offset[i] = 0.5 * tidl_TFSSDConfig.width_stride[i];
      tidl_TFSSDConfig.height_offset[i] = 0.5 * tidl_TFSSDConfig.height_stride[i];
      if(gParams.metaArchType == TIDL_metaArchTfliteRtSsdDetectionLayer)
      {
        anchorBoxParams[i].numAnchors = tidl_TFSSDConfig.num_ars[i]; /* Populated in TIDL_mapDetectionLayerParamsToSsdConfig function for tflite runtime */
      }
      else{
        anchorBoxParams[i].numAnchors = num_ars;
      }
      anchorBoxParams[i].numKeyPoints = tidl_TFSSDConfig.num_keypoints;
      anchorBoxParams[i].strideW = tidl_TFSSDConfig.width_stride[i]  ;
      anchorBoxParams[i].strideH = tidl_TFSSDConfig.height_stride[i] ;
      anchorBoxParams[i].offsetW = tidl_TFSSDConfig.width_offset[i]/tidl_TFSSDConfig.width_stride[i] ;
      anchorBoxParams[i].offsetH = tidl_TFSSDConfig.height_offset[i]/tidl_TFSSDConfig.height_stride[i] ;

      anchorBoxParams[i].boxScales[0] = 1.0/tidl_TFSSDConfig.y_scale;
      anchorBoxParams[i].boxScales[1] = 1.0/tidl_TFSSDConfig.x_scale;
      anchorBoxParams[i].boxScales[2] = 1.0/tidl_TFSSDConfig.height_scale;
      anchorBoxParams[i].boxScales[3] = 1.0/tidl_TFSSDConfig.width_scale;

      /* Need to fill
      anchorBoxParams[i].kpScales[0] = 0;
      */
      if(gParams.metaArchType == TIDL_metaArchTfliteRtSsdDetectionLayer)
      {
        /* anchorBoxes are populated in TIDL_mapDetectionLayerParamsToSsdConfig function for tflite runtime */
        for (j = 0; j < anchorBoxParams[i].numAnchors; j++)
        {
          anchorBoxParams[i].boxHeight[j] = anchorBoxes[anchorIdx + 4 * j + 2];
          anchorBoxParams[i].boxWidth[j]  = anchorBoxes[anchorIdx + 4 * j + 3];
        }
        anchorIdx += (anchorBoxParams[i].headWidth * anchorBoxParams[i].headHeight * 4 * anchorBoxParams[i].numAnchors);
      }
      else
      {
        for (j = 0; j < num_ars; j++)
        {
          ratio_sqrts = sqrt(aspect_ratios[j]);
          anchorBoxParams[i].boxHeight[j] = scales[j] / ratio_sqrts * tidl_TFSSDConfig.base_anchor_height;
          anchorBoxParams[i].boxWidth[j]  = scales[j] * ratio_sqrts * tidl_TFSSDConfig.base_anchor_width;
        }
      }
    }
  }
  else if ( params->metaArchType == TIDL_metaArchTFFasterRcnn )
  {
    tidl_addFasterRCNNLayersToNet(pOrgTIDLNetStructure, &layerIndex, dataIndex, params, detLayerId);
  }
  else if (( params->metaArchType == TIDL_metaArchTIDLSSD ) || (params->metaArchType == TIDL_metaArchTIDLRetinaNet) ||
           (params->metaArchType == TIDL_metaArchTIDLCenterPoint) || (params->metaArchType == TIDL_metaArchTIDLYoloV8)
          )
  {
    tidl_addTidlSSDPostProc(pOrgTIDLNetStructure, &layerIndex, dataIndex, params, detLayerId);
  }
  else if ( params->metaArchType == TIDL_metaArchTIDLYolo || params->metaArchType == TIDL_metaArchTIDLYoloV5)
  {
    tidl_addTidlYoloPostProc(pOrgTIDLNetStructure, &layerIndex, dataIndex, params, detLayerId);
  }
  else if ( params->metaArchType == TIDL_metaArchTIDL3DOD )
  {
    tidl_addTidl3DODPostProc(pOrgTIDLNetStructure, &layerIndex, dataIndex, params, detLayerId);
  }

  pOrgTIDLNetStructure.numLayers = layerIndex;
  return 0;
}

int32_t tidl_addStitchLayerForBatchProcessing(sTIDL_OrgNetwork_t  *pOrgTIDLNetStructure, int32_t * dataIndex,
int32_t layerIndex, std::vector<int32_t>& out_layers, int32_t inLayerdx, int32_t *layerID)
{
  char dataLayerName[10];
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].layerType = TIDL_BatchReshapeLayer;
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].numInBufs  = 1;
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].numOutBufs = 1;
  snprintf(dataLayerName, 8, "%d", (*layerID)++);
  strcpy((char *)pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outDataNames[0], (char *)"batchReshape_");
  strcat((char *)pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outDataNames[0], (char *)dataLayerName);
  strcpy((char *)pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].inDataNames[0], (char *)pOrgTIDLNetStructure->TIDLPCLayers[inLayerdx].outDataNames[0]);

  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].inData[0].dataId = pOrgTIDLNetStructure->TIDLPCLayers[inLayerdx].outData[0].dataId;
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outConsumerCnt[0] = out_layers.size();
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outConsumerLinked[0] = out_layers.size();
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outData[0].elementType = pOrgTIDLNetStructure->TIDLPCLayers[inLayerdx].outData[0].elementType;
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].actParams.actType = TIDL_NoAct;
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outData[0].dataId = (*dataIndex)++;
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outData[0].batchPadW = 1;

  pOrgTIDLNetStructure->TIDLPCLayers[inLayerdx].outConsumerCnt[0]    -= (out_layers.size() - 1);
  pOrgTIDLNetStructure->TIDLPCLayers[inLayerdx].outConsumerLinked[0] -= (out_layers.size() - 1);

  TIDL_UpdateInDataNameInLayers(pOrgTIDLNetStructure, layerIndex, (char *)(char *)pOrgTIDLNetStructure->TIDLPCLayers[inLayerdx].outDataNames[0],
  (char *)pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outDataNames[0], out_layers);
  TIDL_UpdateInDataBuffIdInLayers(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure->TIDLPCLayers[inLayerdx].outData[0],
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outData[0], out_layers);

  layerIndex++;
  pOrgTIDLNetStructure->numLayers = layerIndex;
  return 0;
}

int32_t tidl_addUnStitchLayerForBatchProcessing(sTIDL_OrgNetwork_t  *pOrgTIDLNetStructure, int32_t * dataIndex,
        int32_t layerIndex, int32_t i1, std::vector<int32_t>& out_layers, int32_t *layerID)
{
  char dataLayerName[10];
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].layerType = TIDL_BatchReshapeLayer;
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].numInBufs  = 1; //pOrgTIDLNetStructure->TIDLPCLayers[i1].numOutBufs;
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].numOutBufs = 1;

  snprintf(dataLayerName, 8, "%d", (*layerID)++);
  /* renaming layer to propagate the output name to last layer */
  strcpy((char *)pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outDataNames[0], (char *)pOrgTIDLNetStructure->TIDLPCLayers[i1].outDataNames[0]);
  strcat((char *)pOrgTIDLNetStructure->TIDLPCLayers[i1].outDataNames[0], "_batchFormat");
  strcpy((char *)pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].inDataNames[0], (char *)pOrgTIDLNetStructure->TIDLPCLayers[i1].outDataNames[0]);

  for (int32_t i2 : out_layers)
  {
    strcpy((char *)pOrgTIDLNetStructure->TIDLPCLayers[i2].inDataNames[0], (char *)pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outDataNames[0]);
  }

  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].inData[0].dataId = pOrgTIDLNetStructure->TIDLPCLayers[i1].outData[0].dataId;
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outConsumerCnt[0] = out_layers.size();
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outConsumerLinked[0] = out_layers.size();
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outData[0].elementType = pOrgTIDLNetStructure->TIDLPCLayers[i1].outData[0].elementType;
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].actParams.actType = TIDL_NoAct;
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outData[0].dataId = (*dataIndex)++;
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outData[0].batchPadW = 1;

  pOrgTIDLNetStructure->TIDLPCLayers[i1].outConsumerCnt[0]    -= (out_layers.size() - 1);
  pOrgTIDLNetStructure->TIDLPCLayers[i1].outConsumerLinked[0] -= (out_layers.size() - 1);

  TIDL_UpdateInDataNameInLayers(pOrgTIDLNetStructure, layerIndex, (char *)(char *)pOrgTIDLNetStructure->TIDLPCLayers[i1].outDataNames[0],
  (char *)pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outDataNames[0], out_layers);
  TIDL_UpdateInDataBuffIdInLayers(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure->TIDLPCLayers[i1].outData[0],
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outData[0], out_layers);

  layerIndex++;
  pOrgTIDLNetStructure->numLayers = layerIndex;
  return 0;
}

int32_t tidl_addPadInsertionForBatchProcessing(sTIDL_OrgNetwork_t  *pOrgTIDLNetStructure, int32_t * dataIndex,
        int32_t layerIndex, int32_t i1, int32_t outLayerdx, int32_t *layerID)
{
  char dataLayerName[10];
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].layerType = TIDL_BatchReshapeLayer;
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].numInBufs  = 1; //pOrgTIDLNetStructure->TIDLPCLayers[i1].numOutBufs;
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].numOutBufs = 1;

  snprintf(dataLayerName, 8, "%d", (*layerID)++);
  strcpy((char *)pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outDataNames[0], (char *)"batchReshape_");
  strcat((char *)pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outDataNames[0], (char *)dataLayerName);
  strcpy((char *)pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].inDataNames[0], (char *)pOrgTIDLNetStructure->TIDLPCLayers[i1].outDataNames[0]);

  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].inData[0].dataId = pOrgTIDLNetStructure->TIDLPCLayers[i1].outData[0].dataId;
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outConsumerCnt[0] = pOrgTIDLNetStructure->TIDLPCLayers[i1].outConsumerCnt[0];
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outConsumerLinked[0] = pOrgTIDLNetStructure->TIDLPCLayers[i1].outConsumerLinked[0];
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outData[0].elementType = pOrgTIDLNetStructure->TIDLPCLayers[i1].outData[0].elementType;
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].actParams.actType = TIDL_NoAct;
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outData[0].dataId = (*dataIndex)++;
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outData[0].batchPadW = 1;

  TIDL_UpdateInDataName(pOrgTIDLNetStructure, layerIndex, (char *)(char *)pOrgTIDLNetStructure->TIDLPCLayers[i1].outDataNames[0],
  (char *)pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outDataNames[0]);
  TIDL_UpdateInDataBuffId(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure->TIDLPCLayers[i1].outData[0],
  pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outData[0]);
  pOrgTIDLNetStructure->TIDLPCLayers[outLayerdx].outConsumerCnt[0] = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outConsumerCnt[0];
  pOrgTIDLNetStructure->TIDLPCLayers[outLayerdx].outConsumerLinked[0] = pOrgTIDLNetStructure->TIDLPCLayers[layerIndex].outConsumerLinked[0];
  layerIndex++;
  pOrgTIDLNetStructure->numLayers = layerIndex;
  return 0;
}


/* BatchReshape layers need to be added in between layers that support
*    batch stitching (L_s) and those do not support batch stitching (L_nos)
*  Example: (output consumed by multiple layers)
*  L_nos --+-- L_s            L_nos --+-- BatchReshape(Stitching) --+-- L_s
*          +-- L_s                    |                             +-- L_s
*          +-- L_nos    =>            +-- L_nos
*          +-- L_nos                  +-- L_nos
*
*  L_s   --+-- L_s            L_s   --+-- L_s
*          +-- L_s                    +-- L_s
*          +-- L_nos    =>            +-- BatchReshape(Unstitching) --+-- L_nos
*          +-- L_nos                                                  +-- L_nos
*/
int32_t tidl_addBatchReshapeLayerForBatchProcessing(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t * dataIndex, int32_t layerIndex, int32_t *layerID, int32_t *batchPadTotal)
{
  int32_t i1, i2, i4;
  char dataLayerName[10];
  int32_t status = 0;
  int32_t lyrAdded = 0;
  for (i1 = 0; i1 < pOrgTIDLNetStructure.numLayers; i1++)
  {
    if (TIDL_doesLayerSupportBatchProcessing(&pOrgTIDLNetStructure.TIDLPCLayers[i1]))
    {
      for (i2 = 0; i2 < pOrgTIDLNetStructure.TIDLPCLayers[i1].numInBufs; i2++)
      {
        int32_t inLayerdx = tidl_getInLayer(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[i2].dataId);
        if (inLayerdx == -1)
          return -1;
        sTIDL_LayerPC_t &TIDLPCInLayers = pOrgTIDLNetStructure.TIDLPCLayers[inLayerdx];
        if ((TIDLPCInLayers.layerType != TIDL_BatchReshapeLayer) &&
          (TIDL_doesLayerSupportBatchProcessing(&TIDLPCInLayers) == 0))
        {
          std::vector<int32_t> out_layers = tidl_getOutLayers(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[i2].dataId);
          std::vector<int32_t> out_layers_support_stitching;
          for (int32_t i3 : out_layers)
          {
            if (TIDL_doesLayerSupportBatchProcessing(&pOrgTIDLNetStructure.TIDLPCLayers[i3]) == 1)
            {
              out_layers_support_stitching.push_back(i3);
            }
          }
          tidl_addStitchLayerForBatchProcessing(&pOrgTIDLNetStructure, dataIndex, layerIndex, out_layers_support_stitching, inLayerdx, layerID);
          lyrAdded = 1;
          break;
        }
        else
        {
          pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[i2].batchPadW = pOrgTIDLNetStructure.TIDLPCLayers[inLayerdx].outData[0].batchPadW;
          if (pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[i2].batchPadW >= TIDL_BATCHPADMAXVALUE)
          {
            tidl_addPadInsertionForBatchProcessing(&pOrgTIDLNetStructure, dataIndex, layerIndex, inLayerdx, i1, layerID);
            lyrAdded = 1;
            break;
          }
          else
          {
            int32_t outFactor = 1;
            if ((pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].dimValues[TIDL_DIM_WIDTH] != 0) && (pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[i2].dimValues[TIDL_DIM_WIDTH] != 0))
            {
              if ( pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[i2].dimValues[TIDL_DIM_WIDTH] > pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].dimValues[TIDL_DIM_WIDTH])
                outFactor = pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[i2].dimValues[TIDL_DIM_WIDTH]/pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].dimValues[TIDL_DIM_WIDTH];
              else
                outFactor = pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].dimValues[TIDL_DIM_WIDTH]/pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[i2].dimValues[TIDL_DIM_WIDTH];
            }
            pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].batchPadW = MAX(pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[0].batchPadW, pOrgTIDLNetStructure.TIDLPCLayers[i1].inData[i2].batchPadW * outFactor);
          }
        }
      }
      if (lyrAdded)
        break;

      for (i2 = 0; i2 < pOrgTIDLNetStructure.TIDLPCLayers[i1].numOutBufs; i2++)
      {
        std::vector<int32_t> out_layers = tidl_getOutLayers(pOrgTIDLNetStructure, layerIndex, pOrgTIDLNetStructure.TIDLPCLayers[i1].outData[i2].dataId);
        if (out_layers.size() == 0)
          return -1;
        std::vector<int32_t> out_layers_donot_support_stitching;
        for (int32_t i3 : out_layers)
        {
          sTIDL_LayerPC_t &TIDLPCOutLayers = pOrgTIDLNetStructure.TIDLPCLayers[i3];
          if ((TIDLPCOutLayers.layerType != TIDL_BatchReshapeLayer) &&
              (TIDL_doesLayerSupportBatchProcessing(&TIDLPCOutLayers) == 0))
          {
            out_layers_donot_support_stitching.push_back(i3);
          }
        }

        if (out_layers_donot_support_stitching.size() > 0)
        {
          tidl_addUnStitchLayerForBatchProcessing(&pOrgTIDLNetStructure, dataIndex, layerIndex, i1, out_layers_donot_support_stitching, layerID);
          lyrAdded = 1;
          break;
        }
      }
      if (lyrAdded)
        break;
    }
  }
  return lyrAdded;
}

/**/
bool tidl_canOverwriteElemType(sTIDL_LayerPC_t* layer, int32_t overWriteType)
{
  bool status = true;
  /*For ONNX-RT*/
  if(gParams.modelType != TIDL_IMPORT_MODEL_FORMAT_ONNX_RT)
  {
    status = true;
  }
  else
  {
    if(layer->layerType == TIDL_DataLayer)
    {
      if(layer->outData[0].elementType < TIDL_SinglePrecFloat)
      {
        if ((layer->numOutBufs == 1) &&
            (gParams.addDataConvertToNet & ADD_DC_LAYER_AT_INPUT))
         {
            /*Input data layers with data convert*/
            status = false;
         }
         else if ((layer->numInBufs > 0) &&
                  (gParams.addDataConvertToNet & ADD_DC_LAYER_AT_OUTPUT))
         {
            /*Output data layers with data convert*/
            status = false;
         }
         else
         {
            /*No dataconvert implies datatype should switch*/
            status = true;
         }
      }
    }
    else
    {
      status = true;
    }
  }
  return status;
}
/** Modifies element type based on specified tensor bits while maintaining sign */
int32_t tidl_changeElemType(int32_t elemTypeOld, int32_t numParamBits)
{
  int32_t sign = TIDL_getDatElementSign(elemTypeOld);
  int32_t elemTypeNew = elemTypeOld;
  if(numParamBits == 32)
  {
    elemTypeNew = TIDL_SinglePrecFloat;
  }
  else if(numParamBits == 16)
  {
    if(sign)
    {
      elemTypeNew = TIDL_SignedShort;
    }
    else
    {
      elemTypeNew = TIDL_UnsignedShort;
    }
  }
  else if(numParamBits == 8)
  {
    if(sign)
    {
      elemTypeNew = TIDL_SignedChar;
    }
    else
    {
      elemTypeNew = TIDL_UnsignedChar;
    }
  }
  return elemTypeNew;
}

void tidl_convertElementTypeGivenParambits(sTIDL_OrgNetwork_t  * pOrgTIDLNetStructure, int32_t numLayers, int32_t numParamBits)
{
  int layerIdx;
  int idx;
  for ( layerIdx = 0; layerIdx < numLayers; layerIdx++)
  {
    /*Gating the DataLayer check to be contingent on the model being either ONNX_RT or ONNX*/
    if (tidl_canOverwriteElemType(&pOrgTIDLNetStructure->TIDLPCLayers[layerIdx], numParamBits))
    {
      for ( idx = 0; idx < pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].numInBufs; idx++)
      {
        pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].inData[idx].elementType = tidl_changeElemType(pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].inData[idx].elementType, numParamBits);
      }

      for ( idx = 0; idx < pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].numOutBufs; idx++)
      {
        pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].outData[idx].elementType = tidl_changeElemType(pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].outData[idx].elementType, numParamBits);
      }
    }
  }
}

void tidl_updateWeightElemSize(sTIDL_OrgNetwork_t  * pOrgTIDLNetStructure, tidl_import_config * params, int32_t numLayers)
{
  int layerIdx;
  for ( layerIdx = 0; layerIdx < numLayers; layerIdx++)
  {
    pOrgTIDLNetStructure->TIDLPCLayers[layerIdx].weightsElementSizeInBits = params->numParamBits;
  }
}

int32_t tidl_fillInDataLayerShape(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, tidl_import_config * params, int32_t layerIndex)
{
  int32_t i, j, inDataIdx;
  int overWritefirstNode = 1;
  if ((params->inWidth[0] == -1) || (params->inHeight[0] == -1) || (params->inNumChannels[0] == -1) )
  {
    overWritefirstNode = 0;
  }
  inDataIdx = 0;
  for (i = 0; i < layerIndex; i++)
  {
    if ((orgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_DataLayer) && (orgTIDLNetStructure.TIDLPCLayers[i].numOutBufs > 0))
    {
      //orgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_BATCH] = 1;
      if (overWritefirstNode)
      {
        orgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_BATCH] = params->numBatches[inDataIdx];
        orgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_DIM1] = params->inDIM1[inDataIdx];
        orgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_DIM2] = params->inDIM2[inDataIdx];
        orgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_NUMCH] = params->inNumChannels[inDataIdx];
        orgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_HEIGHT] = params->inHeight[inDataIdx];
        orgTIDLNetStructure.TIDLPCLayers[i].outData[0].dimValues[TIDL_DIM_WIDTH] = params->inWidth[inDataIdx];
      }
       orgTIDLNetStructure.TIDLPCLayers[i].outData[0].elementType = params->inElementType[inDataIdx];
      orgTIDLNetStructure.TIDLPCLayers[i].outData[0].maxTensorValue = 255;
      orgTIDLNetStructure.TIDLPCLayers[i].outData[0].minTensorValue = 0;
      orgTIDLNetStructure.TIDLPCLayers[i].outData[0].tensorScale = params->inQuantFactor[inDataIdx];
      orgTIDLNetStructure.TIDLPCLayers[i].outData[0].numBatchH = 1;
      orgTIDLNetStructure.TIDLPCLayers[i].outData[0].numBatchW = 1;
      //orgTIDLNetStructure.TIDLPCLayers[i].outData[0].tensorZeroPoint = params->inZeroPoint[inDataIdx];

      inDataIdx++;
    }
  }
  return 0;
}



void TIDL_transpose2DMatrix(float * dstPtr, float * srcPtr, int32_t kw, int32_t kh)
{
  int32_t i,j;
  /* Horizontal Fliep */
  for(j = 0; j < kh; j++)
  {
    for(i = 0; i < kw; i++)
    {
      dstPtr[j*kw + i] = srcPtr[(j*kw-1)-i];
    }
  }
  memcpy(srcPtr,dstPtr,kw*kh*sizeof(float));
  /* Vertical Flipe */
  for(j = 0; j < kh; j++)
  {
    for(i = 0; i < kw; i++)
    {
      dstPtr[j*kw + i] = srcPtr[(kh-j-1)*kw + i];
    }
  }

}

void TIDL_convertDeconv2DtoConv(sTIDL_OrgNetwork_t   &pOrgTIDLNetStructure, int32_t layerIndex)
{
  int32_t i,j,k;
  for (i = 0; i < layerIndex; i++)
  {
    if(orgTIDLNetStructure.TIDLPCLayers[i].layerType == TIDL_Deconv2DLayer)
    {
     if((orgTIDLNetStructure.TIDLPCLayers[i].layerParams.convParams.strideH == 1) &&
        (orgTIDLNetStructure.TIDLPCLayers[i].layerParams.convParams.strideW == 1))
        {
          orgTIDLNetStructure.TIDLPCLayers[i].layerType = TIDL_ConvolutionLayer;
          int32_t kw, kh, ni, no;
          kw = orgTIDLNetStructure.TIDLPCLayers[i].layerParams.convParams.kernelW;
          kh = orgTIDLNetStructure.TIDLPCLayers[i].layerParams.convParams.kernelH;
          ni = orgTIDLNetStructure.TIDLPCLayers[i].layerParams.convParams.numInChannels;
          no = orgTIDLNetStructure.TIDLPCLayers[i].layerParams.convParams.numOutChannels;

          ni = ni / orgTIDLNetStructure.TIDLPCLayers[i].layerParams.convParams.numGroups;

          float * basePtr = (float *)orgTIDLNetStructure.TIDLPCLayers[i].weights.ptr;
          float * tempPtr = (float *)my_malloc(kw*kh*sizeof(float));
          for(j = 0; j < no; j++)
          {
            for(k = 0; k < ni; k++)
            {
                float * srcPtr = &basePtr[j*ni*kw*kh + k*kw*kh];
                memcpy(tempPtr,srcPtr,kw*kh*sizeof(float));
                TIDL_transpose2DMatrix(srcPtr, tempPtr, kw, kh);
            }
          }
          free(tempPtr);
        }
    }
  }
}



int32_t TIDL_isInputLayer(sTIDL_OrgNetwork_t * pOrgTIDLNetStructure, int32_t numLayer, const char *bufName, int32_t layerType)
{
  int32_t i, j;
  for (i = (numLayer - 1); i >= 0; i--)
  {
    for (j = 0; j < pOrgTIDLNetStructure->TIDLPCLayers[i].numOutBufs; j++)
    {
      if (strcmp((const char*)bufName, (const char*)pOrgTIDLNetStructure->TIDLPCLayers[i].outDataNames[j]) == 0)
      {
        if ((pOrgTIDLNetStructure->TIDLPCLayers[i].numOutBufs == 1) && (pOrgTIDLNetStructure->TIDLPCLayers[i].layerType == layerType))
        {
          return 1;
        }
        else
        {
          return 0;
        }
      }
    }
  }
  return 0;
}


int32_t tf_getLayreTypeMapIdx(char* layerName, TIDL_TFLayerMapping_t* TIDL_TFLayerMap, int32_t tblSize)
{
  int32_t idx;
  for (idx = 0; idx < tblSize; idx++)
  {
    if (strcmp(layerName, TIDL_TFLayerMap[idx].layerName) == 0)
    {
      return (idx);
    }
  }
  return -1;
}

int32_t tf_isLayerType(char* layerName, int32_t  startLayer, sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, TIDL_TFLayerMapping_t* TIDL_TFLayerMap, int32_t tblSize)
{
  int32_t i, numOps;
  int32_t mapIdx = tf_getLayreTypeMapIdx(layerName, TIDL_TFLayerMap, tblSize);
  if (mapIdx != -1)
  {
    char layerOpsString[300] = "";
    numOps = TIDL_TFLayerMap[mapIdx].NumOps;
    int32_t nextlayerIdx = startLayer;
    for (i = 0; i < numOps; i++)
    {
      strcat(layerOpsString, TIDL_LayerString[pOrgTIDLNetStructure.TIDLPCLayers[nextlayerIdx].layerType]);
      if ((pOrgTIDLNetStructure.TIDLPCLayers[nextlayerIdx].numOutBufs == 1) && (pOrgTIDLNetStructure.TIDLPCLayers[nextlayerIdx].outConsumerCnt[0] == 1))
      {
        nextlayerIdx = tidl_getOutLayer(pOrgTIDLNetStructure, pOrgTIDLNetStructure.numLayers, pOrgTIDLNetStructure.TIDLPCLayers[nextlayerIdx].outData[0].dataId);
        if(nextlayerIdx == -1)
        {
          break;
        }
      }
      else
      {
        break;
      }
    }
    if (strcmp(layerOpsString, TIDL_TFLayerMap[mapIdx].layerOpsString) == 0)
    {
      return (1);
    }
  }
  return (0);
}
int32_t tidl_getStringsFromList(char *list, char * names, int strLen)
{
  int32_t numStrings = 0;
  char *ptr = list;
  while (ptr[0] != '\0')
  {
    if (ptr[0] == ',') ptr[0] = ' ';
    ptr++;
  }
  ptr = list;
  while (ptr[0] != '\0' )
  {
    if((ptr[0] == ' ') || (ptr[0] == ',') || (ptr[0] == '\t'))
    {
      ptr++;
    }
    else
    {
      sscanf(ptr, "%s", &names[strLen*numStrings]);
      ptr += strlen((char*)(&names[strLen*numStrings]));
      numStrings++;
    }
  }
  return numStrings;
}

int32_t tidl_getElementType(int32_t sign)
{
  if(gParams.quantizationStyle == TIDL_QuantStyleAsymNP2_TFL){
    sign = 1;
  }
  if (gParams.numFeatureBits <= 8)
  {
    if (sign)
    {
      return(TIDL_SignedChar);
    }
    else
    {
      return(TIDL_UnsignedChar);
    }
  }
  else if (gParams.numFeatureBits <= 16)
  {
    if (sign)
    {
      return(TIDL_SignedShort);
    }
    else
    {
      return(TIDL_UnsignedShort);
    }
  }
  else //32
  {
      return(TIDL_SinglePrecFloat);
  }
}

int32_t tidl_getElementSizeInBits(int32_t elementType)
{
  int32_t size = 1;
  if ((elementType == TIDL_SignedChar) || (elementType == TIDL_UnsignedChar))
  {
    size = 1;
  }
  else if ((elementType == TIDL_SignedShort) || (elementType == TIDL_UnsignedShort))
  {
    size = 2;
  }
  else if ((elementType == TIDL_SinglePrecFloat) || (elementType == TIDL_UnsignedWord) || (elementType == TIDL_SignedWord))
  {
    size = 4;
  }
  else if ((elementType == TIDL_UnsignedDoubleWord) || (elementType == TIDL_SignedDoubleWord))
  {
    size = 8;
  }
  else
  {
    size = 1;
  }
  return (size * 8);
}


int32_t TIDL_getDatElementSign(int32_t elementType)
{
  int32_t sign = 1;
  if ((elementType == TIDL_SignedChar) ||
      (elementType == TIDL_SignedShort) ||
      (elementType == TIDL_SignedWord) ||
      (elementType == TIDL_SinglePrecFloat) ||
      (elementType == TIDL_SignedDoubleWord))
  {
    sign = 1;
  }
  else
  {
    sign = 0;
  }
  return sign;
}

int32_t TIDL_getConfigParamOutIndexFromLayerName(const char * layerName)
{
  int32_t index = -1;
  int32_t i;
  for (i = 0; i < TIDL_MAX_ALG_OUT_BUFS; i++)
  {
    if(strcmp(layerName, (const char*)outDataNames[i]) == 0)
    {
      index = i;
      break;
    }
  }

  return index;
}
