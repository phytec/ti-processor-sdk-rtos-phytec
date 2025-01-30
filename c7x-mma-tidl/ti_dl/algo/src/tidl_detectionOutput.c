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

/**
----------------------------------------------------------------------------
@file    tidl_detectionOutput.c
@brief   This file defines private functions for detectionOutput layer.
@version 0.1 (Feb 2018) : Initial version [EPR]
----------------------------------------------------------------------------
*/
#include <typeinfo>
#include "tidl_alg_int.h"
#include "tidl_commonUtils.h"
#include "tidl_detectionOutput.h"
#include "tidl_detectionOutput_int.h"
#include "math.h"
#include "stdlib.h"
#include "tidl_types.h"
using namespace c7x;


#include "tidl_odFindValidLocAndScore_ixX_oxX_priv.h"

#ifndef HOST_EMULATION
#define TIDL_DET_LAYER_PROFILE
#endif

//#define TIDL_OD_L1_DEBUG
//#define TIDL_OD_L2_DEBUG

template void TIDL_sparseLocDataFetch<int8_t>(sTIDL_DetectOutputParams_t *params,
                                              sTIDL_ALgDetectOutputParams_t *algDetLyrParams,
                                              sTIDL_AnchorBoxParams_t *anchorBox,
                                              int32_t curClass,
                                              int32_t countK);

template void TIDL_sparseLocDataFetch<int16_t>(sTIDL_DetectOutputParams_t *params,
                                               sTIDL_ALgDetectOutputParams_t *algDetLyrParams,
                                               sTIDL_AnchorBoxParams_t *anchorBox,
                                               int32_t curClass,
                                               int32_t countK);

template void TIDL_sparseLocDataFetch<float>(sTIDL_DetectOutputParams_t *params,
                                             sTIDL_ALgDetectOutputParams_t *algDetLyrParams,
                                             sTIDL_AnchorBoxParams_t *anchorBox,
                                             int32_t curClass,
                                             int32_t countK);

template int32_t TIDL_objOuputPreperation<int8_t>(sTIDL_DetectOutputParams_t *params,
                                                  sTIDL_ALgDetectOutputParams_t *algDetLyrParams,
                                                  float *priorData,
                                                  float *objData,
                                                  int32_t keepKCnt,
                                                  int32_t numDet,
                                                  int32_t cls);

template int32_t TIDL_objOuputPreperation<int16_t>(sTIDL_DetectOutputParams_t *params,
                                                   sTIDL_ALgDetectOutputParams_t *algDetLyrParams,
                                                   float *priorData,
                                                   float *objData,
                                                   int32_t keepKCnt,
                                                   int32_t numDet,
                                                   int32_t cls);

template int32_t TIDL_objOuputPreperation<float>(sTIDL_DetectOutputParams_t *params,
                                                 sTIDL_ALgDetectOutputParams_t *algDetLyrParams,
                                                 float *priorData,
                                                 float *objData,
                                                 int32_t keepKCnt,
                                                 int32_t numDet,
                                                 int32_t cls);

/**
 ----------------------------------------------------------------------------
 @ingroup TIDL_detectionOutput
 @fn      TIDL_detectionOutputProcess
 @brief   detectionOutput Layer

 @param   intAlgHandle : This structure is the main handle.
 @param   algLayer : Ptr to alg layer parameter used in detectionOutput layer
 @param   tidlLayer: Ptr to tidl layer parameter used in detectionOutput layer
 @param   inPtrs: Ptrs to input buffers to be processed
 @param   outPtrs: Ptrs to output buffers to be processed
 @param   sysMems:Ptr to memory releated buffers used in detectionOutput layer
 @remarks    None
 ----------------------------------------------------------------------------
*/
int32_t TIDL_detectionOutputProcess(
    TIDL_Handle intAlgHandle,
    sTIDL_AlgLayer_t *algLayer,
    sTIDL_Layer_t *tidlLayer,
    void *inPtrs[],
    void *outPtrs[],
    sTIDL_sysMemHandle_t sysMems[TIDL_SYSMEM_MAX])
{

  int32_t status = TIDL_SUCCESS;
  int32_t flowCtrl = intAlgHandle->createParams->flowCtrl;

  sTIDL_DetectOutputParams_t *params = &tidlLayer->layerParams.detectOutParams;
  sTIDL_ALgDetectOutputParams_t *algDetLyrParams = &algLayer->layerParams.detectionOutputParams;

  /* Get output data  pointer*/
  int8_t(*outPtr)[] = (int8_t(*)[])(outPtrs[0]);
  int32_t dataOffset = tidlLayer->outData.padH * tidlLayer->outData.pitch[TIDL_LINE_PITCH] + tidlLayer->outData.padW;
  TIDL_ODLayerHeaderInfo *outputData = (TIDL_ODLayerHeaderInfo *)((((float32_tidl *)(*outPtr)) + dataOffset));
  //int32_t_tidl* odObjectType = (((int32_t_tidl *)(*outPtr)) + dataOffset + 4);
  outputData->numDetObjects = 0;
  if (params->numKeypoints > 0)
  {
    TIDL_SetObjDetectionFormat(&(outputData->odObjectType), TIDL_DetectKeyPoints);
  }
  if (params->subCodeType == TIDL_ObjectPose)
  {
    TIDL_SetObjDetectionFormat(&(outputData->odObjectType), TIDL_DetectObjectPose);
  }
  if(params->metaArchType != TIDL_metaArchTIDL3DOD)
  {
    outputData->objInfoSize = ((sizeof(TIDL_ODLayerObjInfo) - sizeof(TIDL_ODLayerKeyPoint) * TIDL_OD_MAX_KEY_POINTS)
                                + sizeof(TIDL_ODLayerKeyPoint) * params->numKeypoints);
    if (params->subCodeType == TIDL_ObjectPose)
    {
    outputData->objInfoSize += sizeof(TIDL_ODLayerObjectPose);
    }
    TIDL_SetObjDetectionFormat( &(outputData->odObjectType), TIDL_Detect2DBox);
  }
  else
  {
    outputData->objInfoSize = 10 * sizeof(float32_tidl);
    TIDL_SetObjDetectionFormat(&(outputData->odObjectType), TIDL_Detect3DBox);
  }
  outputData->odNumKeyPoints = params->numKeypoints;
  outputData->objInfoOffset = sizeof(TIDL_ODLayerHeaderInfo);  //size of the structure

  algDetLyrParams->numOutElementPer_2dBox = (sizeof(TIDL_ODLayerObjInfo) - sizeof(TIDL_ODLayerKeyPoint) * TIDL_OD_MAX_KEY_POINTS)/sizeof(float32_tidl);
  algDetLyrParams->numOutElementPer_3dBox = sizeof(TIDL_3DODLayerObjInfo)/sizeof(float32_tidl);
  float32_tidl *outObjData = (float32_tidl *)outputData + ((int32_t)(outputData->objInfoOffset / sizeof(float32_tidl)));
  int32_t simdWidth = TIDL_SIMD_WIDTH>>2;
  int32_t simdShift = SIMD_SHIFT_FROM_WIDTH<(TIDL_SIMD_WIDTH>>2)>::value;

  //if((flowCtrl & TIDL_FLOW_CTRL_REF_STAT) != TIDL_FLOW_CTRL_REF_STAT)
  {
    float32_tidl *priorData;
    int32_t numDet, keepKCnt;

#ifdef TIDL_DET_LAYER_PROFILE
    long long acc0 = 0, acc1 = 0, acc2 = 0, acc3 = 0, acc4 = 0;
    long long t0, t1;
#endif

#ifdef TIDL_DET_LAYER_PROFILE
    t0 = __TSC;
#endif
    priorData = (float32_tidl *)algDetLyrParams->priorBoxPtr;

    /* Consolidate all the location and confidnece head pointers, along with their pitch*/
    TIDL_updateLocConfHeadPtrs(intAlgHandle->createParams->net->TIDLLayers, algLayer, inPtrs);

    /*
    Points to Note. There are two places where decision of DDR flow is taken
    1. Early decision, When topM or topMAllClasses is less than zero. This is singel pass but from start itself it is DDR flow
    2. Secondly late decision, If valid points becomes more than the calculated topM
       value then another pass is performed with all topM related pointers being in DDR and topM value being the worst possible
       value. That happens in the function "TIDL_findValidLocAndScore"
    */

    int32_t availableL2Size = sysMems[TIDL_SYSMEM_L2_SCRATCH].size;

    /*Data space needed is (10C - 10)*topM for modules outside class loop. hence deriving topM based on avaibility of L2 space*/
    if (params->scoreConverter == TIDL_scoreConverterSIGMOID)
    {
      /*Data space needed is (10C - 10)*topM for modules outside class loop. hence deriving topM based on avaibility of L2 space*/
      algDetLyrParams->topM = ((availableL2Size)
                              / (TIDL_SIGMOID_SCRATCH_BYTES_PER_CLASS * (params->numClasses - algDetLyrParams->isBckClsAvailable)));
    }
    else
    {
      /*Data space needed is (12C - 12)*topM for modules outside class loop. hence deriving topM based on avaibility of L2 space*/
      algDetLyrParams->topM = ((availableL2Size)
                              / (TIDL_SOFTMAX_SCRATCH_BYTES_PER_CLASS * (params->numClasses - algDetLyrParams->isBckClsAvailable)));
    }

    algDetLyrParams->topM = (algDetLyrParams->topM >> simdShift) << simdShift; // making multiple of 16 for ci restriction

    // ci code has restriction to have topMAllClasses value 16 less than acctual value
    algDetLyrParams->topMAllClasses = algDetLyrParams->topM * (params->numClasses - algDetLyrParams->isBckClsAvailable) - simdWidth;

    if((algDetLyrParams->topM <= 0) || (algDetLyrParams->topMAllClasses <= 0))
    {
      // early decision of DDR flow. Thre could be late decision when valid locations become more than this topM
      // Eventually memory will not get allocated in L2, and so DDR memories will be selected inside the function "TIDL_allocInternalMemBuffers"

      algDetLyrParams->topM = algDetLyrParams->topMDdr / (params->numClasses - algDetLyrParams->isBckClsAvailable);

      algDetLyrParams->topM = (algDetLyrParams->topM >> simdShift) << simdShift; // making multiple of 16 for ci restriction

      // ci code has restriction to have topMAllClasses value 16 less than acctual value
      algDetLyrParams->topMAllClasses = algDetLyrParams->topM * (params->numClasses - algDetLyrParams->isBckClsAvailable) - simdWidth;

    }

    keepKCnt = 0;
#ifdef TIDL_DET_LAYER_PROFILE
    t1 = __TSC;
    tidl_printf(0, "TIDL_updateLocConfHeadPtrs() cycle is %lld \n", (t1 - t0));
    t0 = __TSC;
#endif

    /*Allocate internal memory for different scratch data buffers*/
    status = TIDL_allocInternalMemBuffers(params, algDetLyrParams, priorData, sysMems);

#ifdef TIDL_DET_LAYER_PROFILE
    t1 = __TSC;
    tidl_printf(0, "TIDL_allocInternalMemBuffers() cycle is %lld \n", (t1 - t0));
    t0 = __TSC;
#endif

    /*Find locations where scores are higher than the user provided threshold. These are valid locations for which accurate score and decoding of boxes will be done in later part of code.
    If for any class score is higher than user provided threshold then this location is to be considered for further processing, otherwise it will be discarded.
    In softmax this is achieved by calculating the upper bound of score for an object for each location by observing the score across all the classes.
    In sigmoid acctual score are compared instead of upper bound in softmax.
  */

    TIDL_odFindValidLocAndScore_ixX_oxX_PrivArgs *pKerPrivArgs =
        (TIDL_odFindValidLocAndScore_ixX_oxX_PrivArgs *)algLayer->kernelHandle[0][0];

    if(status == TIDL_SUCCESS)
    {
      status = TIDL_findValidLocAndScore(pKerPrivArgs, params, algDetLyrParams, priorData, flowCtrl);
    }

#ifdef TIDL_DET_LAYER_PROFILE
    t1 = __TSC;
    tidl_printf(0, "TIDL_validPointScoreCalc() cycle is %lld \n", (t1 - t0));
    t0 = __TSC;
#endif

    /*Every data of all modules in this class loop will work from L2
    All valid objects are kept in L2.
   */

    /* One approach could be that class by class topK slection and bounding box decoding
     is done. Another approach could be to do topK and box decoding for all
     classes and keep the data in L2.
     First Approach advantage is that L2 requirement will be less and disadvantge is box decoding
     might get repeated for points where two class bounding boxes are lying exactly at the same place.

     In second approach L2 requirement is high, but has potential to avoid same bounding box decoding for
     multiple classes.

     For 100 class, and topK=100 total ~16KB L2 space is needed assuming 4 float parameter for one box.
     And that too if all the classes has valid 100 elements then 16KB will be filled.

     However taking first approach for now for simplicity with knowledge that for some valid points
     box decoding may happen multiple times for different classes. And this will be helpful to extend
     the support of models where location parameters are not shared.
  */

    if(params->topKAllClasses != -1)
    {
      /*In this flow select topKAllClasses number of detections across all classes before applying NMS*/

      /*Find topK scores among all valid scores*/
      TIDL_topKAllClassesSelection(params, algDetLyrParams);

    }

    for (int32_t cls = 0; cls < params->numClasses; cls++)
    {

      if ((cls == params->backgroundLabelId) || (status != TIDL_SUCCESS))
      {
        continue;
      }

#ifdef TIDL_DET_LAYER_PROFILE
      t0 = __TSC;
#endif

      /*Find topK scores among all valid scores*/
      int32_t countK = TIDL_topKSelection(params, algDetLyrParams, cls);

#ifdef TIDL_OD_L1_DEBUG
      tidl_printf(0, "For class = %d, countK = %d\n", cls, countK);
#endif

#ifdef TIDL_DET_LAYER_PROFILE
      t1 = __TSC;
      acc0 += (t1 - t0);
      t0 = __TSC;
#endif

      /*For valid locations, fetch the sparse location head data*/
      TIDL_sparseLocDataFetchiX(params, algDetLyrParams, (sTIDL_AnchorBoxParams_t *)priorData, cls, countK);

#ifdef TIDL_DET_LAYER_PROFILE
      t1 = __TSC;
      acc1 += (t1 - t0);
      t0 = __TSC;
#endif

      /*From previously fetched sparse location data, decode the box and keypoints parameters*/
      TIDL_boxParamsDecoding(params, algDetLyrParams, priorData, countK);

#ifdef TIDL_DET_LAYER_PROFILE
      t1 = __TSC;
      acc2 += (t1 - t0);
      t0 = __TSC;
#endif

      /*Non maxima supression of the valid boxes*/
      numDet = TIDL_applyNMSFast(params, algDetLyrParams, countK);

#ifdef TIDL_OD_L1_DEBUG
      tidl_printf(0, "For class = %d, numDet = %d\n", cls, numDet);
#endif

#ifdef TIDL_DET_LAYER_PROFILE
      t1 = __TSC;
      acc3 += (t1 - t0);
      t0 = __TSC;
#endif

      TIDL_objOuputPreperationiX(params, algDetLyrParams, priorData, outObjData, keepKCnt, numDet, cls);

      keepKCnt = keepKCnt + numDet;

#ifdef TIDL_DET_LAYER_PROFILE
      t1 = __TSC;
      acc4 += (t1 - t0);
#endif
    }

    outputData->numDetObjects = keepKCnt > params->keepTopK ? params->keepTopK : keepKCnt;

#ifdef TIDL_DET_LAYER_PROFILE
    tidl_printf(0, "TIDL_topKSelection() cycle is %lld \n", acc0);
    tidl_printf(0, "TIDL_sparseLocDataFetch() cycle is %lld \n", acc1);
    tidl_printf(0, "TIDL_boxParamsDecoding() cycle is %lld \n", acc2);
    tidl_printf(0, "TIDL_applyNMSFast() cycle is %lld \n", acc3);
    tidl_printf(0, "TIDL_objOuputPreperation() cycle is %lld \n", acc4);
#endif
  }
  //else
  //{
  //outputData[0] = 0;
  //}

  if ((flowCtrl & TIDL_FLOW_CTRL_REF_STAT) == TIDL_FLOW_CTRL_REF_STAT)
  {
    if (TIDL_getDatElementSign(intAlgHandle->createParams->net->TIDLLayers[algLayer->layerIdx].outData.elementType) == 1)
    {
      TIDL_UpdateScaleFactors(intAlgHandle, algLayer->layerIdx, 1, -1, 1);
    }
    else
    {
      TIDL_UpdateScaleFactors(intAlgHandle, algLayer->layerIdx, 1, 0, 1);
    }
  }
  return status;
}

/**
 * @brief Function to update the locations and confedences based on input tensor scales
 *
 * @param TIDLLayers : Pointer to the common layer parameters
 * @param algLayer : Pointer to the layer specific parameters
 * @param inPtrs : Pointer to input data memory
 * @param priorData : Pointer to prior data buffer
 */
void TIDL_updateLocConfHeadPtrs(sTIDL_Layer_t *TIDLLayers, sTIDL_AlgLayer_t *algLayer, void *inPtrs[])
{
  sTIDL_Layer_t *tidlLayer = &TIDLLayers[algLayer->layerIdx];
  sTIDL_DetectOutputParams_t *params = &tidlLayer->layerParams.detectOutParams;
  sTIDL_ALgDetectOutputParams_t *algDetLyrParams = &algLayer->layerParams.detectionOutputParams;
  int32_t dataOffset;
  sTIDL_DataParams_t *inLocDataParams;
  sTIDL_DataParams_t *inConfDataParams;
  sTIDL_DataParams_t *inExtraDataParams;

  int32_t locHeadOffset   = 0;
  int32_t confHeadOffset  = 0;
  int32_t extraHeadOffset = 0;

  /*This information can come from import tool, to make code free of meta architecture type.*/
  if (params->metaArchType == TIDL_metaArchTIDLYolo)
  {
    locHeadOffset = 0;
    confHeadOffset = 0;
  }
  else
  {
    locHeadOffset   = 0;
    confHeadOffset  = params->numHeads;
    if(params->metaArchType == TIDL_metaArchTIDL3DOD)
    {
      extraHeadOffset = 2*params->numHeads;
    }
  }

  for (int32_t j = 0; j < params->numHeads; j++)
  {

    /*get location data pointer, scale and other parameters*/
    int8_t(*inLocPtr)[] = (int8_t(*)[])(inPtrs[j + locHeadOffset]);
    /* Except of top and bottom padding all the data is processed*/
    dataOffset = algDetLyrParams->locDataOffset[j];
    int8_t *inLocData = (int8_t *)(*inLocPtr + dataOffset);
    algDetLyrParams->inLocDataList[j] = (void *)inLocData;

    /* Get conf data  pointer and scale*/
    int8_t(*inConfPtr)[] = (int8_t(*)[])(inPtrs[j + confHeadOffset]);

    /* Except of top and bottom padding all the data is processed*/
    dataOffset = algDetLyrParams->confDataOffset[j];
    float32_tidl *inConfData = (float32_tidl *)(((int8_t *)(*inConfPtr)) + dataOffset);
    algDetLyrParams->inConfDataList[j] = (void *)inConfData;

    /* Get extra data  pointer and scale*/
    int8_t(*inExtraPtr)[] = (int8_t(*)[])(inPtrs[j + extraHeadOffset]);

    dataOffset = algDetLyrParams->extraDataOffset[j];
    float32_tidl *inExtraData = (float32_tidl *)(((int8_t *)(*inExtraPtr)) + dataOffset);
    algDetLyrParams->inExtraDataList[j] = (void *)inExtraData;

    inLocDataParams = &TIDLLayers[algLayer->inLayerIdx[j + locHeadOffset]].outData;
    inConfDataParams = &TIDLLayers[algLayer->inLayerIdx[j + confHeadOffset]].outData;
    inExtraDataParams = &TIDLLayers[algLayer->inLayerIdx[j + extraHeadOffset]].outData;
    int32_t numAnchor = algDetLyrParams->numAnchors[j];

    /* In typical 2d-od each head will have different spatial resolution to detect different
       scale of object. And for each head one prior box information (with different aspect ratio)
       is provided through prototext file. Range of aspect ratio for spefic scale object are
       tragetted in one head. And this all are independent of number of classes.

       However in 3d-od it is possible for given head ( or given spatial resolution) multiple scale is
       targetted. And typically total Nc (Number of classes) number of scales are targetted for given
       spatial resolution. Hence Nc number of prior box parameter is provided through prototext file for
       each head. So for 3 class scenario this can be there will be 3 scales prior boxes are taken from
       one head(or spatial resolution)

       To take care of this special scenario new concept of numSubHead is introduced. nunSubHeads means
       how many 'prior_bax_params' are present for given head ( or given spatial resolution).

       In 3d od with 3 classes of points pillar, number of subhead is 3 and numHeads is 1. In other scenario
       numSubHeads will be 1 hence it doesnt affect the typical functionality.

                                              (Conv)
                                                |
                                                | (head) In 3dOd for each head 2 anchors can be be there, one at 0 degree another at 90 degree
                                                v
                                            ----------
                                            |   |    |
                                            |s1 |s2  |s3
                                            v   v    v
                                            (Sub Heads)

      As shown above one convolution layer output(one head) is fed to detect three different scale (s1,s2,s3), and each scale will have 2 anchors one at
      zero dgree and another at 90 degree. Above shown picture is virtually converted into 3 heads problem to minimize the changes in the
      detection layer code
    */
    /* Except of top and bottom padding all the data is processed*/

    inLocDataParams = &TIDLLayers[algLayer->inLayerIdx[j + locHeadOffset]].outData;
    inConfDataParams = &TIDLLayers[algLayer->inLayerIdx[j + confHeadOffset]].outData;
    inExtraDataParams = &TIDLLayers[algLayer->inLayerIdx[j + extraHeadOffset]].outData;

    for(int32_t k = 0; k < params->numSubHeads; k++)
    {
      /* Except of top and bottom padding all the data is processed*/
      dataOffset = algDetLyrParams->locDataOffset[j*params->numSubHeads + k];
      int8_t *inLocData = (int8_t *)(*inLocPtr + dataOffset);

      dataOffset = algDetLyrParams->confDataOffset[j*params->numSubHeads + k];
      int8_t *inConfData = (int8_t *)(((int8_t *)(*inConfPtr)) + dataOffset);

      dataOffset = algDetLyrParams->extraDataOffset[j*params->numSubHeads + k];
      int8_t *inExtraData = (int8_t *)(((int8_t *)(*inExtraPtr)) + dataOffset);

      algDetLyrParams->inLocDataList[j*params->numSubHeads + k] = (void *)(inLocData +
      k*algDetLyrParams->locHeadPitchList[j][TIDL_CHANNEL_PITCH]*
      numAnchor*algDetLyrParams->boxParamsSize*algDetLyrParams->elmSize);

      /* Except of top and bottom padding all the data is processed*/
      algDetLyrParams->inConfDataList[j*params->numSubHeads + k] = (void *)(inConfData +
      k*algDetLyrParams->confHeadPitchList[j][TIDL_CHANNEL_PITCH]*
      numAnchor*params->numClasses*algDetLyrParams->elmSize);

      /* Except of top and bottom padding all the data is processed*/
      algDetLyrParams->inExtraDataList[j*params->numSubHeads + k] = (void *)(inExtraData +
      k*algDetLyrParams->extraHeadPitchList[j][TIDL_CHANNEL_PITCH]*
      numAnchor*algDetLyrParams->extraParamSize*algDetLyrParams->elmSize);

      if ((inLocDataParams->elementType == TIDL_SinglePrecFloat) || (inLocDataParams->tensorScale == 0))
      {
        algDetLyrParams->inLocdataQList[j*params->numSubHeads + k] = 1.0f;
      }
      else
      {
        algDetLyrParams->inLocdataQList[j*params->numSubHeads + k] = (1.0f / ((float)inLocDataParams->tensorScale));
      }

      if ((inConfDataParams->elementType == TIDL_SinglePrecFloat) || (inConfDataParams->tensorScale == 0))
      {
        algDetLyrParams->inConfdataQList[j*params->numSubHeads + k] = 1.0f;
      }
      else
      {
        algDetLyrParams->inConfdataQList[j*params->numSubHeads + k] = (1.0f / ((float)inConfDataParams->tensorScale));
      }

      if ((inExtraDataParams->elementType == TIDL_SinglePrecFloat) || (inExtraDataParams->tensorScale == 0))
      {
        algDetLyrParams->inExtradataQList[j*params->numSubHeads + k] = 1.0f;
      }
      else
      {
        algDetLyrParams->inExtradataQList[j*params->numSubHeads + k] = (1.0f / ((float)inExtraDataParams->tensorScale));
      }
    }
  }
}

/**
 * @brief  This function is to allocate the internal memory buffers
 *
 * @param params : Parameters of the detectOut layer
 * @param algDetLyrParams : parameter detectOut algorithm
 * @param priorData : pointer to prior data buffer
 * @param sysMems : sysMem handle
 */
int32_t TIDL_allocInternalMemBuffers(sTIDL_DetectOutputParams_t *params, sTIDL_ALgDetectOutputParams_t *algDetLyrParams,
                                  float *priorData, sTIDL_sysMemHandle_t sysMems[TIDL_SYSMEM_MAX])
{

  int32_t topMScoreSize;
  int32_t topK = params->topK;
  int32_t numClasses = params->numClasses;
  int32_t topM = algDetLyrParams->topM;

  int32_t l1Offset = sysMems[TIDL_SYSMEM_L1_SCRATCH].offset;
  int32_t l2Offset = sysMems[TIDL_SYSMEM_L2_SCRATCH].offset;
  int32_t l3Offset = sysMems[TIDL_SYSMEM_L3_SCRATCH].offset;
  int32_t l4Offset = sysMems[TIDL_SYSMEM_DDR_SCRATCH].offset;

  int32_t status = TIDL_SUCCESS;

  sTIDL_AnchorBoxParams_t *anchorBox = (sTIDL_AnchorBoxParams_t *)priorData;

  int32_t maxConfPlaneSize = 0;
  int32_t curConfPts;

  for (int32_t j = 0; j < params->numHeads; j++)
  {
    curConfPts = anchorBox[j].headHeight * algDetLyrParams->confHeadPitchList[j][TIDL_LINE_PITCH];
    if (maxConfPlaneSize < curConfPts)
    {
      maxConfPlaneSize = curConfPts;
    }
  }

  /*Advancing the DDR pointer for already allocated scratch pointer at init*/
  TIDL_getMemoryChunkFromSysmem(sysMems, algDetLyrParams->scratchDDRConsumed, 128U, TIDL_SYSMEM_DDR_SCRATCH, IALG_SCRATCH);

  maxConfPlaneSize = TIDL_ALIGN_CEIL(maxConfPlaneSize, 64);

  /****************** L1D memory allocation ************************************/
  /* Class loop related data structure allocation. Currently it is going to be topK*24 + 8C +
     (maxConfPlaneSize + 63)>>3 bytes needed for this portion of modules*/
  /*****************************************************************************/
  algDetLyrParams->topKBbox = (BBox *)TIDL_getMemoryChunkFromSysmem(sysMems, params->topK * sizeof(BBox), 128U, TIDL_SYSMEM_L1_SCRATCH, IALG_SCRATCH);

  if (algDetLyrParams->topKBbox == NULL)
  {
    algDetLyrParams->topKBbox = (BBox *)TIDL_getMemoryChunkFromSysmem(sysMems, params->topK * sizeof(BBox), 128U, TIDL_SYSMEM_DDR_SCRATCH, IALG_SCRATCH);
    if(algDetLyrParams->topKBbox == NULL)
    {
      tidl_printf(0, "Not enough memory to allocate mmaHandleArgsMem for topKBbox\n");
      status = TIDL_ERR_FAILURE;
      return status;
    }
  }

  algDetLyrParams->topKLoc = (float *)algDetLyrParams->topKBbox;

  algDetLyrParams->topKScore = (uint16_t *)TIDL_getMemoryChunkFromSysmem(sysMems, topK * sizeof(uint16_t), 128U, TIDL_SYSMEM_L1_SCRATCH, IALG_SCRATCH);
  if (algDetLyrParams->topKScore == NULL)
  {
    algDetLyrParams->topKScore = (uint16_t *)TIDL_getMemoryChunkFromSysmem(sysMems, topK * sizeof(uint16_t), 128U, TIDL_SYSMEM_DDR_SCRATCH, IALG_SCRATCH);
    if(algDetLyrParams->topKScore == NULL)
    {
      tidl_printf(0, "Not enough memory to allocate mmaHandleArgsMem for topKScore\n");
      status = TIDL_ERR_FAILURE;
      return status;
    }
  }

  algDetLyrParams->topKIndices = (int32_t *)TIDL_getMemoryChunkFromSysmem(sysMems, topK * sizeof(int32_t), 128U, TIDL_SYSMEM_L1_SCRATCH, IALG_SCRATCH);
  if (algDetLyrParams->topKIndices == NULL)
  {
    algDetLyrParams->topKIndices = (int32_t *)TIDL_getMemoryChunkFromSysmem(sysMems, topK * sizeof(int32_t), 128U, TIDL_SYSMEM_DDR_SCRATCH, IALG_SCRATCH);
    if((algDetLyrParams->topKIndices == NULL))
    {
      tidl_printf(0, "Not enough memory to allocate mmaHandleArgsMem \n");
      status = TIDL_ERR_FAILURE;
      return status;
    }
  }

  algDetLyrParams->nmsKeptIndices = (int32_t *)TIDL_getMemoryChunkFromSysmem(sysMems, topK * sizeof(int32_t), 128U, TIDL_SYSMEM_L1_SCRATCH, IALG_SCRATCH);
  if (algDetLyrParams->nmsKeptIndices == NULL)
  {
    algDetLyrParams->nmsKeptIndices = (int32_t *)TIDL_getMemoryChunkFromSysmem(sysMems, topK * sizeof(int32_t), 128U, TIDL_SYSMEM_DDR_SCRATCH, IALG_SCRATCH);
    if(algDetLyrParams->nmsKeptIndices == NULL)
    {
      tidl_printf(0, "Not enough memory to allocate mmaHandleArgsMem for nmsKeptIndices\n");
      status = TIDL_ERR_FAILURE;
      return status;
    }
  }

  algDetLyrParams->countMList = (int32_t *)TIDL_getMemoryChunkFromSysmem(sysMems, sizeof(int32_t) * numClasses, 128U, TIDL_SYSMEM_L1_SCRATCH, IALG_SCRATCH);
  if (algDetLyrParams->countMList == NULL)
  {
    algDetLyrParams->countMList = (int32_t *)TIDL_getMemoryChunkFromSysmem(sysMems, sizeof(int32_t) * numClasses, 128U, TIDL_SYSMEM_DDR_SCRATCH, IALG_SCRATCH);
    if(algDetLyrParams->countMList == NULL)
    {
      tidl_printf(0, "Not enough memory to allocate mmaHandleArgsMem for countMList\n");
      status = TIDL_ERR_FAILURE;
      return status;
    }
  }

  algDetLyrParams->countMListAcc = (int32_t *)TIDL_getMemoryChunkFromSysmem(sysMems, sizeof(int32_t) * numClasses, 128U, TIDL_SYSMEM_L1_SCRATCH, IALG_SCRATCH);
  if (algDetLyrParams->countMListAcc == NULL)
  {
    algDetLyrParams->countMListAcc = (int32_t *)TIDL_getMemoryChunkFromSysmem(sysMems, sizeof(int32_t) * numClasses, 128U, TIDL_SYSMEM_DDR_SCRATCH, IALG_SCRATCH);
    if(algDetLyrParams->countMListAcc == NULL)
    {
      tidl_printf(0, "Not enough memory to allocate mmaHandleArgsMem for countMListAcc\n");
      status = TIDL_ERR_FAILURE;
      return status;
    }
  }

  if (params->scoreConverter == TIDL_scoreConverterSOFTMAX)
  {
    algDetLyrParams->tempScore = (float *)TIDL_getMemoryChunkFromSysmem(sysMems, numClasses * sizeof(float), 128U, TIDL_SYSMEM_L1_SCRATCH, IALG_SCRATCH);

    if (algDetLyrParams->tempScore == NULL)
    {
      algDetLyrParams->tempScore = (float *)TIDL_getMemoryChunkFromSysmem(sysMems, numClasses * sizeof(float), 128U, TIDL_SYSMEM_DDR_SCRATCH, IALG_SCRATCH);
      if(algDetLyrParams->tempScore == NULL)
      {
        tidl_printf(0, "Not enough memory to allocate mmaHandleArgsMem for tempScore\n");
        status = TIDL_ERR_FAILURE;
        return status;
      }
    }
  }
  else
  {
    algDetLyrParams->tempScore = NULL;
  }

  /****************** L2D memory allocation ************************************/
  /* Sigmoid L2 memory need --> (4*(C - 1)<topMindices> + 4*(C - 1)<topMindicesSorted> + 2*(C - 1)<topMScore> )*topM = 10(C-1)topM*/
  /* Softmax L2 memory need --> (4*(C - 1)<topMindices> + 4*(C - 1)<topMindicesSorted> + 2*(C - 1)<topMScore> + 2*(C - 1)<topMScoreSorted> )*topM = 12(C-1)topM*/
  /*****************************************************************************/
  algDetLyrParams->topMIndices = (int32_t *)TIDL_getMemoryChunkFromSysmem(sysMems, topM * sizeof(int32_t) * (numClasses - algDetLyrParams->isBckClsAvailable), 128U, TIDL_SYSMEM_L2_SCRATCH, IALG_SCRATCH);
  if (algDetLyrParams->topMIndices == NULL)
  {
    algDetLyrParams->topMIndices = algDetLyrParams->topMIndicesDdr;
    if (algDetLyrParams->topMIndices == NULL)
    {
      tidl_printf(0, "Not enough memory to allocate mmaHandleArgsMem for topMIndices\n");
      status = TIDL_ERR_FAILURE;
      return status;
    }
  }

  algDetLyrParams->topMIndicesSorted = (int32_t *)TIDL_getMemoryChunkFromSysmem(sysMems, topM * sizeof(int32_t) * (numClasses - algDetLyrParams->isBckClsAvailable), 128U, TIDL_SYSMEM_L2_SCRATCH, IALG_SCRATCH);
  if (algDetLyrParams->topMIndicesSorted == NULL)
  {
    algDetLyrParams->topMIndicesSorted = algDetLyrParams->topMIndicesSortedDdr;
    if (algDetLyrParams->topMIndicesSorted == NULL)
    {
      tidl_printf(0, "Not enough memory to allocate mmaHandleArgsMem for topMIndicesSorted\n");
      status = TIDL_ERR_FAILURE;
      return status;
    }
  }

  /*In softmax all the classes are assumed to be valid temporarily, hence allocating the space for storing all the indices*/
  /*Also scores are seperated for each class with distinct boundary, hence allocating topMIndices, and topMScore for numClasses*topM*/
  topMScoreSize = topM * (numClasses - algDetLyrParams->isBckClsAvailable) * sizeof(int16_t);

  algDetLyrParams->topMScore = (uint16_t *)TIDL_getMemoryChunkFromSysmem(sysMems, topMScoreSize, 128U, TIDL_SYSMEM_L2_SCRATCH, IALG_SCRATCH);
  if (algDetLyrParams->topMScore == NULL)
  {
    algDetLyrParams->topMScore = algDetLyrParams->topMScoreDdr;
    if (algDetLyrParams->topMScore == NULL)
    {
      tidl_printf(0, "Not enough memory to allocate mmaHandleArgsMem for topMScore\n");
      status = TIDL_ERR_FAILURE;
      return status;
    }
  }
  if (params->scoreConverter == TIDL_scoreConverterSOFTMAX)
  {
    algDetLyrParams->topMScoreSorted = (uint16_t *)TIDL_getMemoryChunkFromSysmem(sysMems, topMScoreSize, 128U, TIDL_SYSMEM_L2_SCRATCH, IALG_SCRATCH);
    if (algDetLyrParams->topMScoreSorted == NULL)
    {
      algDetLyrParams->topMScoreSorted = algDetLyrParams->topMScoreSortedDdr;
      if (algDetLyrParams->topMScoreSorted == NULL)
      {
        tidl_printf(0, "Not enough memory to allocate mmaHandleArgsMem for topMScoreSorted\n");
        status = TIDL_ERR_FAILURE;
        return status;
      }
    }
  }

  /*L3 scratch memory allocation*/
  /*Each element is trated as int, so total bytes = maxConfPlaneSize*4, andone bit for each byte. SO total bytes  = maxConfPlaneSize*4/8*/
  algDetLyrParams->pred = (long long *)TIDL_getMemoryChunkFromSysmem(sysMems, (maxConfPlaneSize + 63) >> 1, 128U, TIDL_SYSMEM_L3_SCRATCH, IALG_SCRATCH);
  if (algDetLyrParams->pred == NULL)
  {
    algDetLyrParams->pred = (long long *)TIDL_getMemoryChunkFromSysmem(sysMems, (maxConfPlaneSize + 63) >> 1, 128U, TIDL_SYSMEM_DDR_SCRATCH, IALG_SCRATCH);

    if(algDetLyrParams->pred == NULL)
    {
      tidl_printf(0, "Not enough memory to allocate mmaHandleArgsMem for pred\n");
      status = TIDL_ERR_FAILURE;
      return status;
    }
  }

  if (params->scoreConverter == TIDL_scoreConverterSOFTMAX)
  {
    algDetLyrParams->featMaxMinVal = (float *)TIDL_getMemoryChunkFromSysmem(sysMems, maxConfPlaneSize * 2 * algDetLyrParams->elmSize, 128U, TIDL_SYSMEM_L3_SCRATCH, IALG_SCRATCH);
    if (algDetLyrParams->featMaxMinVal == NULL)
    {
      algDetLyrParams->featMaxMinVal = (float *)TIDL_getMemoryChunkFromSysmem(sysMems, maxConfPlaneSize * 2 * algDetLyrParams->elmSize, 128U, TIDL_SYSMEM_DDR_SCRATCH, IALG_SCRATCH);

      if(algDetLyrParams->featMaxMinVal == NULL){
        tidl_printf(0, "Not enough memory to allocate mmaHandleArgsMem for featMaxMinVal\n");
        status = TIDL_ERR_FAILURE;
        return status;
      }
	}
  }

  sysMems[TIDL_SYSMEM_L1_SCRATCH].offset = l1Offset;
  sysMems[TIDL_SYSMEM_L2_SCRATCH].offset = l2Offset;
  sysMems[TIDL_SYSMEM_L3_SCRATCH].offset = l3Offset;
  sysMems[TIDL_SYSMEM_DDR_SCRATCH].offset= l4Offset;

  return status;
}

/**
 * @brief  This function is to select the top k values based on score
 *
 * @param params : Parameters of the detectOut layer
 * @param algDetLyrParams : parameter detectOut algorithm
 * @param cls : class lable for top k selection
 * @return int32_t : returns top k values
 */
int32_t TIDL_topKSelection(sTIDL_DetectOutputParams_t *params, sTIDL_ALgDetectOutputParams_t *algDetLyrParams, int32_t cls)
{

  int32_t kCount = 0;
  int16_t maxScore;
  int32_t maxScoreIdx = 0;
  uint16_t *curClsScore = &algDetLyrParams->topMScoreSorted[algDetLyrParams->countMListAcc[cls]];
  int32_t *curClsIndex = &algDetLyrParams->topMIndicesSorted[algDetLyrParams->countMListAcc[cls]];
  uint16_t scoreTh = params->confThreshold * SSD_CONF_DATA_Q;
  int32_t topKCnt;

  if (algDetLyrParams->countMList[cls] > params->topK)
  {
    topKCnt = params->topK;
  }
  else
  {
    topKCnt = algDetLyrParams->countMList[cls];
  }

  for (int32_t i = 0; i < topKCnt; i++)
  {
    maxScore = 0;
    for (int32_t j = 0; j < algDetLyrParams->countMList[cls]; j++)
    {
      if (curClsScore[j] > maxScore)
      {
        maxScore = curClsScore[j];
        maxScoreIdx = j;
      }
    }

    if (maxScore >= scoreTh)
    {
      algDetLyrParams->topKScore[i] = maxScore;
      algDetLyrParams->topKIndices[i] = curClsIndex[maxScoreIdx];
      kCount++;
    }
    else
    {
      tidl_printf(0, "maxScore is not higher than threshold = %f, %f \n", maxScore, scoreTh);
    }

    curClsScore[maxScoreIdx] = 0;
  }

  if (topKCnt != kCount)
  {
    tidl_printf(0, "topKCnt and kCount doesnt match and those are %d, %d, for class %d\n", topKCnt, kCount, cls);
  }

  return kCount;
}

/**
 * @brief  This function is to select the topkAllClasses across all classes values based on score
 *
 * @param params : Parameters of the detectOut layer
 * @param algDetLyrParams : parameter detectOut algorithm
 * @param cls : class lable for top k selection
 * @return int32_t : returns top k values
 */
void TIDL_topKAllClassesSelection(sTIDL_DetectOutputParams_t *params, sTIDL_ALgDetectOutputParams_t *algDetLyrParams)
{

  uint16_t minScore;
  int32_t minScoreIdx = 0;
  uint16_t *curClsScore;
  uint16_t scoreTh = params->confThreshold * SSD_CONF_DATA_Q;
  int32_t topKAllClassesCnt = 0;
  int32_t cls, clsMin;
  int32_t i;

  for(cls = 0; cls < params->numClasses; cls++)
  {
    topKAllClassesCnt += algDetLyrParams->countMList[cls];
  }

  if (topKAllClassesCnt > params->topKAllClasses)
  {
    for (i = 0; i < (topKAllClassesCnt - params->topKAllClasses); i++)
    {
      minScore = 0xFFFF;

      for(cls = 0; cls < params->numClasses; cls++)
      {
        curClsScore = &algDetLyrParams->topMScoreSorted[algDetLyrParams->countMListAcc[cls]];
        for (int32_t j = 0; j < algDetLyrParams->countMList[cls]; j++)
        {
          if (curClsScore[j] < minScore)
          {
            minScore = curClsScore[j];
            minScoreIdx = j;
            clsMin = cls;
          }
        }
      }

      if (minScore >= scoreTh)
      {
        curClsScore = &algDetLyrParams->topMScoreSorted[algDetLyrParams->countMListAcc[clsMin]];
        curClsScore[minScoreIdx] = 0xFFFF;
      }
      else
      {
        /*Control should  not have come here. As all the scores greater than or equal to scoreTh reaches here*/
        tidl_printf(0, "minScore is not higher than threshold = %f, %f \n", minScore, scoreTh);
      }
    }
    for(cls = 0; cls < params->numClasses; cls++)
    {
      curClsScore = &algDetLyrParams->topMScoreSorted[algDetLyrParams->countMListAcc[cls]];
      for (int32_t j = 0; j < algDetLyrParams->countMList[cls]; j++)
      {
        if (curClsScore[j] == 0xFFFF)
        {
          curClsScore[j] = 0;
        }
      }
    }
  }
}

/**
 * @brief  This function is to get valid locations
 *
 * @param params : Parameters of the detectOut layer
 * @param algDetLyrParams : parameter detectOut algorithm
 * @param anchorBox : parameters of the anchorBox
 * @param curClass : class label for top k selection
 * @param countK : number of K values
 * @return None
 */
void TIDL_sparseLocDataFetchiX(sTIDL_DetectOutputParams_t *params, sTIDL_ALgDetectOutputParams_t *algDetLyrParams,
                               sTIDL_AnchorBoxParams_t *anchorBox, int32_t curClass, int32_t countK)
{
  if (algDetLyrParams->elementType == TIDL_SignedChar)
  {
    TIDL_sparseLocDataFetch<int8_t>(params, algDetLyrParams, anchorBox, curClass, countK);
  }
  else if (algDetLyrParams->elementType == TIDL_SignedShort)
  {
    TIDL_sparseLocDataFetch<int16_t>(params, algDetLyrParams, anchorBox, curClass, countK);
  }
  else
  {
    TIDL_sparseLocDataFetch<float>(params, algDetLyrParams, anchorBox, curClass, countK);
  }
}

/**
 * @brief  This function is to get valid locations
 *
 * @param params : Parameters of the detectOut layer
 * @param algDetLyrParams : parameter detectOut algorithm
 * @param anchorBox : parameters of the anchorBox
 * @param curClass : class label for top k selection
 * @param countK : number of K values
 * @return None
 */
template <typename Tloc>
void TIDL_sparseLocDataFetch(sTIDL_DetectOutputParams_t *params, sTIDL_ALgDetectOutputParams_t *algDetLyrParams,
                             sTIDL_AnchorBoxParams_t *anchorBox, int32_t curClass, int32_t countK)
{
  Tloc *restrict inPtr;
  float *restrict outBboxPtr = (float *)algDetLyrParams->topKLoc;
  int64_t curIdx;
  int16_t col, row, anchor, head;
  int32_t linePitch, chPitch;
  int32_t chNo;
  int32_t numInCodeElements; //coded elements for given OD architecture
  float onebyqFact;
  uint16_t loc;

  int32_t bbStride;
  int32_t anchorStirde;


  if(params->metaArchType == TIDL_metaArchTIDL3DOD)
  {
    /*In 3D OD bounding bix parameters are x y z l w h angle.*/
    numInCodeElements = algDetLyrParams->boxParamsSize;
  }
  else if (params->metaArchType != TIDL_metaArchTIDLYolo)
  {
    numInCodeElements = 4 + algDetLyrParams->numInElementPerKeypoint * params->numKeypoints;
  }
  else
  {
    numInCodeElements = 5 + params->numClasses
                          +  algDetLyrParams->numInElementPerKeypoint * params->numKeypoints
                          +  (algDetLyrParams->numSubCodeElements); // in YOLO it is not code size, just reusing the variable to find pitch of one acnhor data to another.
  }

  for (int32_t i = 0; i < countK; i++)
  {
    curIdx = algDetLyrParams->topKIndices[i];
    loc = (curIdx >> 0) & 0xffff;
    anchor = (curIdx >> 24) & 0xf;
    head = (curIdx >> 28) & 0xf;
    chNo = anchor;

    if(params->dataLayout == 0)
    {
      bbStride = 1;
      anchorStirde = numInCodeElements;
    }
    else /* (params->dataLayout == 1) */
    {
      bbStride = algDetLyrParams->numAnchors[head];
      anchorStirde = 1;
    }

    row = loc * algDetLyrParams->confHeadInvPitchList[head][TIDL_LINE_PITCH];
    col = loc - row * algDetLyrParams->confHeadPitchList[head][TIDL_LINE_PITCH];

    onebyqFact = algDetLyrParams->inLocdataQList[head];

    linePitch = algDetLyrParams->locHeadPitchList[head][TIDL_LINE_PITCH];
    chPitch = algDetLyrParams->locHeadPitchList[head][TIDL_CHANNEL_PITCH];
    inPtr = (Tloc *)(algDetLyrParams->inLocDataList[head]);

    if(params->metaArchType == TIDL_metaArchTIDLYoloV8)
    {
      int32_t j = 0, k = 0;
      float reg_prm[16];
      float wt_exp_sum;
      float exp_sum;
      float exp_prm;

      for(j = 0; j < 4; j++)
      {
        wt_exp_sum = 0.0f;
        exp_sum = 0.0f;
        for(k = 0; k < 16; k++)
        {
          exp_prm = exp((float)(inPtr[((chNo*anchorStirde + (j*16 + k))*bbStride) * chPitch + row * linePitch + col] * onebyqFact));
          reg_prm[k] = exp_prm;
          exp_sum += exp_prm;
        }

        for(k = 0; k < 16; k++)
        {
          wt_exp_sum += (k*(reg_prm[k]));
        }
        outBboxPtr[i * 4 + j] = wt_exp_sum/exp_sum;
      }
    }
    else if(params->metaArchType != TIDL_metaArchTIDL3DOD)
    {
      outBboxPtr[i * 4 + 0] = inPtr[(chNo*anchorStirde + 0*bbStride) * chPitch + row * linePitch + col] * onebyqFact;
      outBboxPtr[i * 4 + 1] = inPtr[(chNo*anchorStirde + 1*bbStride) * chPitch + row * linePitch + col] * onebyqFact;
      outBboxPtr[i * 4 + 2] = inPtr[(chNo*anchorStirde + 2*bbStride) * chPitch + row * linePitch + col] * onebyqFact;
      outBboxPtr[i * 4 + 3] = inPtr[(chNo*anchorStirde + 3*bbStride) * chPitch + row * linePitch + col] * onebyqFact;
    }
    else
    {
      /* In 3d od box parameters are x y z w l h angle.
         For NMS only x y and l w is required.
      */
      outBboxPtr[i * 4 + 0] = inPtr[(chNo*anchorStirde + 0*bbStride) * chPitch + row * linePitch + col] * onebyqFact;
      outBboxPtr[i * 4 + 1] = inPtr[(chNo*anchorStirde + 1*bbStride) * chPitch + row * linePitch + col] * onebyqFact;
      outBboxPtr[i * 4 + 2] = inPtr[(chNo*anchorStirde + 3*bbStride) * chPitch + row * linePitch + col] * onebyqFact;
      outBboxPtr[i * 4 + 3] = inPtr[(chNo*anchorStirde + 4*bbStride) * chPitch + row * linePitch + col] * onebyqFact;
    }
  }
}
/**
 * @brief  This function is decode Box parameters
 *
 * @param params : Parameters of the detectOut layer
 * @param algDetLyrParams : parameter detectOut algorithm
 * @param priorData : pointer to prior data buffer
 * @param countK : number of K values for selection
 * @return None
 */
int32_t TIDL_boxParamsDecoding(sTIDL_DetectOutputParams_t *params, sTIDL_ALgDetectOutputParams_t *algDetLyrParams,
                               float *priorData, int32_t countK)
{

  float32_tidl boxWidth;
  float32_tidl boxHeight;
  float32_tidl *pVariance;
  float32_tidl stepH, stepW, offsetW, offsetH;
  int32_t codeType = params->codeType;

  sTIDL_AnchorBoxParams_t *anchorBox = (sTIDL_AnchorBoxParams_t *)priorData;
  int32_t curIdx;
  BBox *curBox;
  float *curLoc;
  int32_t cxId, cyId, wId, hId;

  /*This code can go inside import tool.*/
  if ((params->metaArchType == TIDL_metaArchTFSSD) || (codeType == 5))
  {
    cxId = 1;
    cyId = 0;
    wId = 3;
    hId = 2;
  }
  else
  {
    cxId = 0;
    cyId = 1;
    wId = 2;
    hId = 3;
  }

  /* This loop decodes predictions into bboxes for all heads */
  for (int32_t cnt = 0; cnt < countK; cnt++)
  {
    curIdx = algDetLyrParams->topKIndices[cnt];
    curLoc = (float *)algDetLyrParams->topKLoc;
    curBox = &algDetLyrParams->topKBbox[cnt];

    int32_t loc = (curIdx >> 0) & 0xFFFF;
    int32_t anchor = (curIdx >> 24) & 0xF;
    int32_t head = (curIdx >> 28) & 0xF;
    int32_t curY = loc * algDetLyrParams->confHeadInvPitchList[head][TIDL_LINE_PITCH];
    int32_t curX = loc - curY * algDetLyrParams->confHeadPitchList[head][TIDL_LINE_PITCH];

    stepW     = anchorBox[head].strideW;
    stepH     = anchorBox[head].strideH;
    offsetW   = anchorBox[head].offsetW;
    offsetH   = anchorBox[head].offsetH;
    pVariance = anchorBox[head].boxScales;

    boxWidth  = anchorBox[head].boxWidth[anchor]; // x direction
    boxHeight = anchorBox[head].boxHeight[anchor]; // y direction
    /* compute  prior box data */
    float32_tidl centerX = ((float32_tidl)curX + offsetW) * stepW;
    float32_tidl centerY = ((float32_tidl)curY + offsetH) * stepH;

    float32_tidl dXmin, dYmin, dXmax, dYmax;
    float32_tidl dCx, dCy, dW, dH;

    if ((codeType == 2) || (codeType == 5))
    {
      dCx = (pVariance[cxId] * curLoc[4 * cnt + cxId] * boxWidth) + centerX;
      dCy = (pVariance[cyId] * curLoc[4 * cnt + cyId] * boxHeight) + centerY;
      dW = 0.5f * (exp_taylor((pVariance[wId] * curLoc[4 * cnt + wId])) * boxWidth);
      dH = 0.5f * (exp_taylor((pVariance[hId] * curLoc[4 * cnt + hId])) * boxHeight);

      dXmin = dCx - dW;
      dYmin = dCy - dH;
      dXmax = dCx + dW;
      dYmax = dCy + dH;
    }
    else if (codeType == 1)
    {
      dXmin = (centerX - 0.5f * boxWidth) + (pVariance[cxId] * curLoc[4 * cnt + cxId]);
      dYmin = (centerY - 0.5f * boxHeight) + (pVariance[cyId] * curLoc[4 * cnt + cyId]);
      dXmax = (centerX + 0.5f * boxWidth) + (pVariance[wId] * curLoc[4 * cnt + wId]);
      dYmax = (centerY + 0.5f * boxHeight) + (pVariance[hId] * curLoc[4 * cnt + hId]);
    }
    else if (codeType == 9) // yolov8
    {
      dXmin = centerX - curLoc[4 * cnt + cxId]*stepW;
      dYmin = centerY - curLoc[4 * cnt + cyId]*stepH;
      dXmax = centerX + curLoc[4 * cnt + wId]*stepW;
      dYmax = centerY + curLoc[4 * cnt + hId]*stepH;
    }
    else if (codeType == 3) /*PriorBoxParameter_CodeType_CORNER_SIZE */
    {
      dXmin = (centerX - 0.5f * boxWidth) + (pVariance[cxId] * curLoc[4 * cnt + cxId] * boxWidth);
      dYmin = (centerY - 0.5f * boxHeight) + (pVariance[cyId] * curLoc[4 * cnt + cyId] * boxHeight);
      dXmax = (centerX + 0.5f * boxWidth) + (pVariance[wId] * curLoc[4 * cnt + wId] * boxWidth);
      dYmax = (centerY + 0.5f * boxHeight) + (pVariance[hId] * curLoc[4 * cnt + hId] * boxHeight);
    }
    else if (codeType == 4) //yolov3
    {
      float dx = 1.0f / (exp_taylor(-curLoc[4 * cnt + cxId]) + 1);
      float dy = 1.0f / (exp_taylor(-curLoc[4 * cnt + cyId]) + 1);

      dCx = (dx + curX) / anchorBox[head].headWidth;
      dCy = (dy + curY) / anchorBox[head].headHeight;
      dH = 0.5 * (float)(exp_taylor(curLoc[4 * cnt + hId])) * boxHeight;
      dW = 0.5 * (float)(exp_taylor(curLoc[4 * cnt + wId])) * boxWidth;

      dXmin = dCx - dW;
      dYmin = dCy - dH;
      dXmax = dCx + dW;
      dYmax = dCy + dH;
    }
    else if (codeType == 6) //yolov5
    {
      float dx = 1.0f / (exp_taylor(-curLoc[4 * cnt + cxId]) + 1);
      float dy = 1.0f / (exp_taylor(-curLoc[4 * cnt + cyId]) + 1);
      float dH = 1.0f / (exp_taylor(-curLoc[4 * cnt + hId]) + 1);
      float dW = 1.0f / (exp_taylor(-curLoc[4 * cnt + wId]) + 1);

      dCx = (2.0*dx - 0.5 + curX) / anchorBox[head].headWidth;
      dCy = (2.0*dy - 0.5 + curY) / anchorBox[head].headHeight;
      dH = (2*dH*dH) * boxHeight;
      dW = (2*dW*dW) * boxWidth;

      dXmin = dCx - dW;
      dYmin = dCy - dH;
      dXmax = dCx + dW;
      dYmax = dCy + dH;
    }
    else if (codeType == 7) //yolox
    {
      float dx = curLoc[4 * cnt + cxId];
      float dy = curLoc[4 * cnt + cyId];

      dCx = (dx + curX) / anchorBox[head].headWidth;
      dCy = (dy + curY) / anchorBox[head].headHeight;
      dH = 0.5 * (float)(exp_taylor(curLoc[4 * cnt + hId])) * boxHeight;
      dW = 0.5 * (float)(exp_taylor(curLoc[4 * cnt + wId])) * boxWidth;

      dXmin = dCx - dW;
      dYmin = dCy - dH;
      dXmax = dCx + dW;
      dYmax = dCy + dH;
    }
    else if (codeType == 8) // 3d od
    {
      float dx = curLoc[4 * cnt + 0];
      float dy = curLoc[4 * cnt + 1];

      float aH,aW;

      aW  = boxWidth;
      aH  = boxHeight;

      float diagonal = aH*aH + aW*aW;

      diagonal = diagonal * __recip_sqrt(diagonal);

      dCx = dx*diagonal + centerX;
      dCy = dy*diagonal + centerY;

      dW  = exp_taylor(curLoc[4 * cnt + 2])*aW;
      dH  = exp_taylor(curLoc[4 * cnt + 3])*aH;

      dXmin = dCx - dW/2;
      dYmin = dCy - dH/2;
      dXmax = dCx + dW/2;
      dYmax = dCy + dH/2;
    }
    else
    {
      tidl_printf(0,"Not supported codeType \n");
    }


    curBox->xmin = dXmin;
    curBox->ymin = dYmin;
    curBox->xmax = dXmax;
    curBox->ymax = dYmax;
  }

  return 0;
}

/**
 * @brief  This function is to prepare objection dimensions for each class
 *
 * @param params : Parameters of the detectOut layer
 * @param algDetLyrParams : parameter detectOut algorithm
 * @param priorData : pointer to prior data buffer
 * @param objData : pointer to objectdetect buffer
 * @param keepKCnt : number of K values to keep for selection
 * @param numDet : no of detected objects
 * @param cls : class lable for top k selection
 */
void TIDL_objOuputPreperationiX(sTIDL_DetectOutputParams_t *params, sTIDL_ALgDetectOutputParams_t *algDetLyrParams,
                                float *priorData, float *objData, int32_t keepKCnt, int32_t numDet, int32_t cls)
{
  if (algDetLyrParams->elementType == TIDL_SignedChar)
  {
    TIDL_objOuputPreperation<int8_t>(params, algDetLyrParams, priorData, objData, keepKCnt, numDet, cls);
  }
  else if (algDetLyrParams->elementType == TIDL_SignedShort)
  {
    TIDL_objOuputPreperation<int16_t>(params, algDetLyrParams, priorData, objData, keepKCnt, numDet, cls);
  }
  else
  {
    TIDL_objOuputPreperation<float>(params, algDetLyrParams, priorData, objData, keepKCnt, numDet, cls);
  }
}

/**
 * @brief  This function is to prepare objection dimensions for each class
 *
 * @param params : Parameters of the detectOut layer
 * @param algDetLyrParams : parameter detectOut algorithm
 * @param priorData : pointer to prior data buffer
 * @param objData : pointer to objectdetect buffer
 * @param keepKCnt : number of K values to keep for selection
 * @param numDet : no of detected objects
 * @param cls : class lable for top k selection
 * @return int32_t : no of detected objects
 */
template <typename Tloc>
int32_t TIDL_objOuputPreperation(sTIDL_DetectOutputParams_t *params, sTIDL_ALgDetectOutputParams_t *algDetLyrParams,
                                 float *priorData, float *objData, int32_t keepKCnt, int32_t numDet, int32_t cls)
{
  int32_t numOutDataPerObject;
  int32_t objSrcIdx, objDstIdx;
  sTIDL_AnchorBoxParams_t *anchorBox = (sTIDL_AnchorBoxParams_t *)priorData;
  int32_t replaceLowestScoreBox = 1;
  float32_tidl currBoxScore;

  if (params->metaArchType != TIDL_metaArchTIDL3DOD)
  {
    numOutDataPerObject = algDetLyrParams->numOutElementPer_2dBox
                            + algDetLyrParams->numOutElementPerKeypoint * params->numKeypoints
                            + algDetLyrParams->numSubCodeElements;  // Each keypoint is defined by (x,y,conf) at output
  }
  else
  {
    numOutDataPerObject = algDetLyrParams->numOutElementPer_3dBox; // no key points for 3d od. k,cls,score,X,Y,Z,L,W,H,angle
  }

  for (int32_t k = 0; k < numDet; k++)
  {
    objSrcIdx = algDetLyrParams->nmsKeptIndices[k];
    currBoxScore = (((float32_tidl)algDetLyrParams->topKScore[objSrcIdx]) / (float32_tidl)SSD_CONF_DATA_Q);

    if ((k + keepKCnt) < params->keepTopK)
    {
      objDstIdx = k + keepKCnt;
    }
    else
    {
      /* When the number of detections is greater than keepTopK, for every detection seen after keepTopK, we check the box with minimum score among
      the existing keepTopK boxes, and replace the box with the lowest score(minScore) with the new box if the new box's score is greater than
      minScore */
      float minScore = FLT_MAX;
      for (int32_t l = 0; l < params->keepTopK; l++)
      {
        if (objData[((numOutDataPerObject * l) + 2)] < minScore)
        {
          minScore = objData[((numOutDataPerObject * l) + 2)];
          objDstIdx = l;
        }
      }
      if(currBoxScore < minScore) replaceLowestScoreBox = 0; //Do not replace box with minScore if (currBoxScore < minScore)
    }

    if(replaceLowestScoreBox)
    {
      objData[((numOutDataPerObject * objDstIdx) + 0)] = (float32_tidl)(keepKCnt + k);
      objData[((numOutDataPerObject * objDstIdx) + 1)] = (float32_tidl)cls;
      objData[((numOutDataPerObject * objDstIdx) + 2)] = (((float32_tidl)algDetLyrParams->topKScore[objSrcIdx]) / (float32_tidl)SSD_CONF_DATA_Q);

      if(params->metaArchType != TIDL_metaArchTIDL3DOD)
      {
        objData[((numOutDataPerObject * objDstIdx) + 3)] = algDetLyrParams->topKBbox[objSrcIdx].xmin;
        objData[((numOutDataPerObject * objDstIdx) + 4)] = algDetLyrParams->topKBbox[objSrcIdx].ymin;
        objData[((numOutDataPerObject * objDstIdx) + 5)] = algDetLyrParams->topKBbox[objSrcIdx].xmax;
        objData[((numOutDataPerObject * objDstIdx) + 6)] = algDetLyrParams->topKBbox[objSrcIdx].ymax;
      }
      else
      {
        objData[((numOutDataPerObject * objDstIdx) + 3)] = (algDetLyrParams->topKBbox[objSrcIdx].xmin +
                                                          algDetLyrParams->topKBbox[objSrcIdx].xmax)/2;

        objData[((numOutDataPerObject * objDstIdx) + 4)] = (algDetLyrParams->topKBbox[objSrcIdx].ymin +
                                                            algDetLyrParams->topKBbox[objSrcIdx].ymax)/2;

        objData[((numOutDataPerObject * objDstIdx) + 6)] = (algDetLyrParams->topKBbox[objSrcIdx].xmax -
                                                          algDetLyrParams->topKBbox[objSrcIdx].xmin);

        objData[((numOutDataPerObject * objDstIdx) + 7)] = (algDetLyrParams->topKBbox[objSrcIdx].ymax -
                                                            algDetLyrParams->topKBbox[objSrcIdx].ymin);
      {

        int32_t objPkdIndx;
        int32_t loc, curY, curX, anchor, head;
        int32_t linePitch, chPitch;
        int32_t chNo;
        float onebyqFactLoc;
        float onebyqFactDir;

        objPkdIndx = algDetLyrParams->topKIndices[objSrcIdx];

        loc = (objPkdIndx >> 0) & 0xffff;
        cls = (objPkdIndx >> 16) & 0xff;
        anchor = (objPkdIndx >> 24) & 0xf;
        head = (objPkdIndx >> 28) & 0xf;
        chNo = anchor;
        curY = loc / algDetLyrParams->locHeadPitchList[head][TIDL_LINE_PITCH];
        curX = loc - curY * algDetLyrParams->locHeadPitchList[head][TIDL_LINE_PITCH];
        onebyqFactLoc = algDetLyrParams->inLocdataQList[head];
        onebyqFactDir = algDetLyrParams->inExtradataQList[head];

        Tloc *locPtr = (Tloc *)(algDetLyrParams->inLocDataList[head]);
        Tloc *dirPtr = (Tloc *)(algDetLyrParams->inExtraDataList[head]);

        linePitch = algDetLyrParams->locHeadPitchList[head][TIDL_LINE_PITCH];
        chPitch = algDetLyrParams->locHeadPitchList[head][TIDL_CHANNEL_PITCH];

        int32_t anchorStirde = algDetLyrParams->boxParamsSize;
        int32_t bbStride = 1;

        float zt      = locPtr[(chNo*anchorStirde + 2*bbStride) * chPitch + curY * linePitch + curX] * onebyqFactLoc;
        float ht      = locPtr[(chNo*anchorStirde + 5*bbStride) * chPitch + curY * linePitch + curX] * onebyqFactLoc;

        /*decode the Z location*/
        float za = anchorBox[head].offsetL;
        float ha = anchorBox[head].boxLength[anchor];
        float hg = exp_taylor(ht)*ha;
              za = za + ha / 2;
        float zg = zt*ha + za;
              zg-= hg/2;

        objData[((numOutDataPerObject * objDstIdx) + 5)] = zg;

        /*decode the height*/
        objData[((numOutDataPerObject * objDstIdx) + 8)] = hg;

        /*angle calculation*/
          float angle  = locPtr[(chNo*anchorStirde + 6*bbStride) * chPitch + curY * linePitch + curX] * onebyqFactLoc;
          float dir    = dirPtr[(chNo*anchorBox[head].numAnchors + 0) * chPitch + curY * linePitch + curX] * onebyqFactDir;
          int32_t j;
          int32_t max_dir = 0;
          float pi = 3.1415926f;

          for (j = 1; j < anchorBox[head].numAnchors; j++)
          {
            if(((float)dirPtr[(chNo*anchorBox[head].numAnchors + j) * chPitch + curY * linePitch + curX]) * onebyqFactDir > dir)
            {
              max_dir = j;
            }
          }

          angle = angle + anchorBox[head].boxAngle[anchor];
          angle = angle - anchorBox[head].offsetDir;
          angle = angle - ((int32_t)(angle/pi + 0))*pi;
          angle = angle + anchorBox[head].offsetDir;
          angle = angle + pi*max_dir;

          objData[((numOutDataPerObject * objDstIdx) + 9)] = angle;

        }
      }
      if (params->numKeypoints > 0 || params->subCodeType==TIDL_ObjectPose)
      {
        int32_t objPkdIndx;
        float kpX;
        float kpY;
        float tX, tY, tZ, r11, r21, r31, rc1_inv, r12, r22, r32, rc2_inv, rc1_rc2;
        float kp_conf;
        int32_t loc, curY, curX, anchor, head;
        int32_t linePitch, chPitch;
        int32_t chNo;
        int32_t numInCodeElements;
        if (params->metaArchType == TIDL_metaArchTIDLYolo)
        {
            numInCodeElements = 5 + params->numClasses
                                  + algDetLyrParams->numInElementPerKeypoint * params->numKeypoints
                                  + algDetLyrParams->numSubCodeElements;
        }
        else
        {
            numInCodeElements = 4 + algDetLyrParams->numInElementPerKeypoint * params->numKeypoints;
        }
        float onebyqFact;
        float32_tidl centerX, centerY;
        float32_tidl boxWidth;
        float32_tidl boxHeight;
        float32_tidl *pVariance;
        float32_tidl stepH, stepW, offsetW, offsetH, dCx, dCy, dkp_conf;

        if (params->subCodeType == TIDL_ObjectPose)
        {
          objPkdIndx = algDetLyrParams->topKIndices[objSrcIdx];

          loc = (objPkdIndx >> 0) & 0xffff;
          cls = (objPkdIndx >> 16) & 0xff;
          anchor = (objPkdIndx >> 24) & 0xf;
          head = (objPkdIndx >> 28) & 0xf;
          chNo = anchor;
          chNo *= numInCodeElements;
          curY = loc / algDetLyrParams->locHeadPitchList[head][TIDL_LINE_PITCH];
          curX = loc - curY * algDetLyrParams->locHeadPitchList[head][TIDL_LINE_PITCH];
          int32_t outDataObjectPoseStartIndex = algDetLyrParams->numOutElementPer_2dBox
                                                 + params->numKeypoints * algDetLyrParams->numOutElementPerKeypoint;  // Starting Index of objectpose for an object in objData
          onebyqFact = algDetLyrParams->inLocdataQList[head];

          linePitch = algDetLyrParams->locHeadPitchList[head][TIDL_LINE_PITCH];
          chPitch = algDetLyrParams->locHeadPitchList[head][TIDL_CHANNEL_PITCH];
          Tloc *inPtr = (Tloc *)(algDetLyrParams->inLocDataList[head]);
          //translation Postprocessing
          tX = inPtr[(chNo + numInCodeElements - algDetLyrParams->numSubCodeElements + 6) * chPitch + curY * linePitch + curX] * onebyqFact;
          tY = inPtr[(chNo + numInCodeElements - algDetLyrParams->numSubCodeElements + 7) * chPitch + curY * linePitch + curX] * onebyqFact;
          tZ = inPtr[(chNo + numInCodeElements - algDetLyrParams->numSubCodeElements + 8) * chPitch + curY * linePitch + curX] * onebyqFact;
          tZ = exp_taylor(tZ) * 100.0;
          tX = (tX + curX) * params->imWidth / anchorBox[head].headWidth;
          tY = (tY + curY) *params->imHeight/ anchorBox[head].headHeight;
          tX = (tX - params->subCodeParams.cameraParams.pX) *tZ / params->subCodeParams.cameraParams.fX;
          tY = (tY - params->subCodeParams.cameraParams.pY) *tZ / params->subCodeParams.cameraParams.fY;
          //rotation Postprocessing
          r11 = inPtr[(chNo + numInCodeElements - algDetLyrParams->numSubCodeElements + 0) * chPitch + curY * linePitch + curX] * onebyqFact;
          r21 = inPtr[(chNo + numInCodeElements - algDetLyrParams->numSubCodeElements + 1) * chPitch + curY * linePitch + curX] * onebyqFact;
          r31 = inPtr[(chNo + numInCodeElements - algDetLyrParams->numSubCodeElements + 2) * chPitch + curY * linePitch + curX] * onebyqFact;
          r12 = inPtr[(chNo + numInCodeElements - algDetLyrParams->numSubCodeElements + 3) * chPitch + curY * linePitch + curX] * onebyqFact;
          r22 = inPtr[(chNo + numInCodeElements - algDetLyrParams->numSubCodeElements + 4) * chPitch + curY * linePitch + curX] * onebyqFact;
          r32 = inPtr[(chNo + numInCodeElements - algDetLyrParams->numSubCodeElements + 5) * chPitch + curY * linePitch + curX] * onebyqFact;
          rc1_inv = __recip_sqrt(r11*r11 + r21*r21 + r31*r31);
          r11 = r11*rc1_inv;
          r21 = r21*rc1_inv;
          r31 = r31*rc1_inv;
          rc1_rc2 = r11*r12 + r21*r22 + r31*r32;
          r12 = r12 - rc1_rc2*r11;
          r22 = r22 - rc1_rc2*r21;
          r32 = r32 - rc1_rc2*r31;
          rc2_inv = __recip_sqrt(r12*r12 + r22*r22 + r32*r32);
          r12 = r12*rc2_inv;
          r22 = r22*rc2_inv;
          r32 = r32*rc2_inv;
          objData[((numOutDataPerObject * objDstIdx) + outDataObjectPoseStartIndex + 0)] = r11;
          objData[((numOutDataPerObject * objDstIdx) + outDataObjectPoseStartIndex + 1)] = r21;
          objData[((numOutDataPerObject * objDstIdx) + outDataObjectPoseStartIndex + 2)] = r31;
          objData[((numOutDataPerObject * objDstIdx) + outDataObjectPoseStartIndex + 3)] = r12;
          objData[((numOutDataPerObject * objDstIdx) + outDataObjectPoseStartIndex + 4)] = r22;
          objData[((numOutDataPerObject * objDstIdx) + outDataObjectPoseStartIndex + 5)] = r32;
          objData[((numOutDataPerObject * objDstIdx) + outDataObjectPoseStartIndex + 6)] = tX;
          objData[((numOutDataPerObject * objDstIdx) + outDataObjectPoseStartIndex + 7)] = tY;
          objData[((numOutDataPerObject * objDstIdx) + outDataObjectPoseStartIndex + 8)] = tZ;
        }

        for (int32_t j = 0; j < params->numKeypoints; j++)
        {
          objPkdIndx = algDetLyrParams->topKIndices[objSrcIdx];

          loc = (objPkdIndx >> 0) & 0xffff;
          cls = (objPkdIndx >> 16) & 0xff;
          anchor = (objPkdIndx >> 24) & 0xf;
          head = (objPkdIndx >> 28) & 0xf;
          chNo = anchor;
          chNo *= numInCodeElements;
          curY = loc / algDetLyrParams->locHeadPitchList[head][TIDL_LINE_PITCH];
          curX = loc - curY * algDetLyrParams->locHeadPitchList[head][TIDL_LINE_PITCH];

          onebyqFact = algDetLyrParams->inLocdataQList[head];

          linePitch = algDetLyrParams->locHeadPitchList[head][TIDL_LINE_PITCH];
          chPitch = algDetLyrParams->locHeadPitchList[head][TIDL_CHANNEL_PITCH];
          Tloc *inPtr = (Tloc *)(algDetLyrParams->inLocDataList[head]);

          if (params->metaArchType == TIDL_metaArchTIDLYolo)
          {
              kpX = inPtr[(chNo + 5 + params->numClasses + algDetLyrParams->numInElementPerKeypoint * j + 0) * chPitch + curY * linePitch + curX] * onebyqFact;
              kpY = inPtr[(chNo + 5 + params->numClasses + algDetLyrParams->numInElementPerKeypoint * j + 1) * chPitch + curY * linePitch + curX] * onebyqFact;
              if (params->keypointConfidence ==1)
              {
              kp_conf = inPtr[(chNo + 5 + params->numClasses + algDetLyrParams->numInElementPerKeypoint * j + 2) * chPitch + curY * linePitch + curX] * onebyqFact;
              }
          }
          else
          {
              kpX = inPtr[(chNo + 4 + algDetLyrParams->numInElementPerKeypoint * j + 0) * chPitch + curY * linePitch + curX] * onebyqFact;
              kpY = inPtr[(chNo + 4 + algDetLyrParams->numInElementPerKeypoint * j + 1) * chPitch + curY * linePitch + curX] * onebyqFact;
              if (params->keypointConfidence ==1)
              {
              kp_conf = inPtr[(chNo + 4 + algDetLyrParams->numInElementPerKeypoint * j + 2) * chPitch + curY * linePitch + curX] * onebyqFact;
              }
          }

          /*Decoding of keypoints*/
          if (params->metaArchType == TIDL_metaArchTIDLYolo)
          {
            dCx = (2.0*kpX - 0.5 + curX) / anchorBox[head].headWidth;
            dCy = (2.0*kpY - 0.5 + curY) / anchorBox[head].headHeight;
          }
          else
          {
              stepW = anchorBox[head].strideW;
              stepH = anchorBox[head].strideH;
              offsetW = anchorBox[head].offsetW;
              offsetH = anchorBox[head].offsetH;
              pVariance = anchorBox[head].kpScales;
              boxWidth = anchorBox[head].boxWidth[anchor];
              boxHeight = anchorBox[head].boxHeight[anchor];

              centerX = ((float32_tidl)curX + offsetW) * stepW;
              centerY = ((float32_tidl)curY + offsetH) * stepH;
              dCx = (pVariance[2 * j] * kpX * boxWidth) + centerX;
              dCy = (pVariance[2 * j + 1] * kpY * boxHeight) + centerY;
          }
          objData[((numOutDataPerObject * objDstIdx) + algDetLyrParams->numOutElementPer_2dBox + algDetLyrParams->numOutElementPerKeypoint * j + 0)] = dCx;
          objData[((numOutDataPerObject * objDstIdx) + algDetLyrParams->numOutElementPer_2dBox + algDetLyrParams->numOutElementPerKeypoint * j + 1)] = dCy;
          if (params->keypointConfidence ==1)
          {
            dkp_conf = 1.0f / (exp_taylor(-kp_conf) + 1);
            objData[((numOutDataPerObject * objDstIdx) + algDetLyrParams->numOutElementPer_2dBox + algDetLyrParams->numOutElementPerKeypoint * j + 2)] = dkp_conf;
          }
        }
      }
    }
  }

  return numDet;
}

/**
 * @brief To find the overlap of two boxes using jaccard overlap
 *
 * @param bbox1 : Box1 dimensions for overlap
 * @param bbox2 : Box2 dimensions for overlap
 * @return float32_tidl : overlap percentage
 */
float32_tidl TIDL_jaccardOverlap(
    const BBox *bbox1,
    const BBox *bbox2)
{
  BBox intersectBbox;
  float32_tidl bbox1Size, bbox2Size, overlap;
  float32_tidl intersectW, intersectH, intersectSize;

  if ((bbox2->xmin > bbox1->xmax) || (bbox2->xmax < bbox1->xmin) ||
      (bbox2->ymin > bbox1->ymax) || (bbox2->ymax < bbox1->ymin))
  {
    /* Return [0, 0, 0, 0] if there is no intersection */
    intersectBbox.xmin = 0;
    intersectBbox.ymin = 0;
    intersectBbox.xmax = 0;
    intersectBbox.ymax = 0;
  }
  else
  {
    intersectBbox.xmin = bbox1->xmin > bbox2->xmin ? bbox1->xmin : bbox2->xmin;
    intersectBbox.ymin = bbox1->ymin > bbox2->ymin ? bbox1->ymin : bbox2->ymin;
    intersectBbox.xmax = bbox1->xmax < bbox2->xmax ? bbox1->xmax : bbox2->xmax;
    intersectBbox.ymax = bbox1->ymax < bbox2->ymax ? bbox1->ymax : bbox2->ymax;
  }
  intersectW = intersectBbox.xmax - intersectBbox.xmin;
  intersectH = intersectBbox.ymax - intersectBbox.ymin;
  if ((intersectW > (float32_tidl)0) && (intersectH > (float32_tidl)0))
  {
    intersectSize = intersectW * intersectH;
    bbox1Size = (bbox1->xmax - bbox1->xmin) * (bbox1->ymax - bbox1->ymin);
    bbox2Size = (bbox2->xmax - bbox2->xmin) * (bbox2->ymax - bbox2->ymin);
    overlap = intersectSize / ((bbox1Size + bbox2Size) - intersectSize);
  }
  else
  {
    overlap = (float32_tidl)0;
  }
  return overlap;
}

/*
  bboxes       --> list of bounding boxes
  scores       --> list of scores
  inCount      --> total number of valid entries in scores and bboxes
  nmsThreshold --> NMS threhsold coming from model parameters
  eta          --> coming from model. Mostly not used
  outIndices   --> list of indices of survived boxes
*/

/**
 * @brief  This function is to select the top k values based on score
 *
 * @param params : Parameters of the detectOut layer
 * @param algDetLyrParams : parameter detectOut algorithm
 * @param countK : count for top k selection
 */
int32_t TIDL_applyNMSFast(sTIDL_DetectOutputParams_t *params, sTIDL_ALgDetectOutputParams_t *algDetLyrParams, int32_t countK)
{
  int8_t keep;
  int32_t i, k;
  int32_t outCount = 0;
  float32_tidl overlap, adaptiveThreshold;

  /* Do nms */
  adaptiveThreshold = params->nmsThreshold;

  for (i = 0; i < countK; ++i)
  {
    /* Get the index from score which is in lower 16-bit */
    keep = 1;
    for (k = 0; k < outCount; ++k)
    {
      if (keep)
      {
        overlap = TIDL_jaccardOverlap(&algDetLyrParams->topKBbox[i], &algDetLyrParams->topKBbox[algDetLyrParams->nmsKeptIndices[k]]);

        if (overlap <= adaptiveThreshold)
        {
          keep = 1;
        }
        else
        {
          keep = 0;
        }
      }
      else
      {
        break;
      }
    }

    if (keep)
    {
      algDetLyrParams->nmsKeptIndices[outCount] = i;

#ifdef TIDL_OD_L2_DEBUG
      tidl_printf(0, "Kept object[%d] is %4.3f, %4.3f, %4.3f, %4.3f \n",
                  i,
                  algDetLyrParams->topKBbox[i].xmin,
                  algDetLyrParams->topKBbox[i].ymin,
                  algDetLyrParams->topKBbox[i].xmax,
                  algDetLyrParams->topKBbox[i].ymax);
#endif

      outCount++;
    }
    else
    {
      algDetLyrParams->topKScore[i] = 0;
    }

    if (((keep == 1) && (params->eta < (float32_tidl)1.0)) && (adaptiveThreshold > (float32_tidl)0.5))
    {
      adaptiveThreshold *= params->eta;
    }
  }

  return outCount;
}

/**
 * @brief  This function is to find valid locations and scores
 *
 * @param pKerPrivArgs : private agrs for detection params
 * @param params : Parameters of the detectOut layer
 * @param algDetLyrParams : parameter detectOut algorithm
 * @param priorData : pointer to prior data buffer
 * @param flowCtrl : indicates the tidl flow
 */
int32_t TIDL_findValidLocAndScore(void *pKerPrivArgs,
                                  sTIDL_DetectOutputParams_t *params,
                                  sTIDL_ALgDetectOutputParams_t *algDetLyrParams,
                                  float *priorData,
                                  int32_t flowCtrl)
{
  int32_t status = TIDL_SUCCESS;
  int32_t countM;
  int32_t simdWidth = TIDL_SIMD_WIDTH>>2;
  int32_t simdShift = SIMD_SHIFT_FROM_WIDTH<(TIDL_SIMD_WIDTH>>2)>::value;

  TIDL_odFindValidLocAndScore_ixX_oxX_PrivArgs *pKerPrivArgsL =
      (TIDL_odFindValidLocAndScore_ixX_oxX_PrivArgs *)pKerPrivArgs;

  if ((flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == TIDL_FLOW_CTRL_REF_ONLY)
  {
    if (algDetLyrParams->elementType == TIDL_SignedChar)
    {
      countM =
          TIDL_findValidLocation_cn<int8_t>(params, algDetLyrParams, priorData);

      if (countM <= algDetLyrParams->topMAllClasses)
      {
        TIDL_sparseDetScoreCalc_cn<int8_t>(params, algDetLyrParams);
      }
    }
    else if (algDetLyrParams->elementType == TIDL_SignedShort)
    {
      countM =
          TIDL_findValidLocation_cn<int16_t>(params, algDetLyrParams, priorData);

      if (countM <= algDetLyrParams->topMAllClasses)
      {
        TIDL_sparseDetScoreCalc_cn<int16_t>(params, algDetLyrParams);
      }
    }
    else if (algDetLyrParams->elementType == TIDL_SinglePrecFloat)
    {
      countM =
          TIDL_findValidLocation_cn<float>(params, algDetLyrParams, priorData);

      if (countM <= algDetLyrParams->topMAllClasses)
      {
        TIDL_sparseDetScoreCalc_cn<float>(params, algDetLyrParams);
      }
    }
    else
    {
      status = TIDL_ERR_NOT_IMPLEMENTED;
      tidl_printf(0, "Not supported for this data type \n");
    }
  }
  else
  {
    countM = pKerPrivArgsL->execute((void *)pKerPrivArgsL, NULL, NULL, NULL, NULL);
  }

#ifdef TIDL_OD_L1_DEBUG
  tidl_printf(0, "Maximum COunt M  is %d \n", countM);
#endif

  /*Total detection is clipped to topMAllClasses, hence if countM >= algDetLyrParams->topMAllClasses then DDR flow has to be chosen.
    However countM > topMAllClasses will never happen technically. Equal itself is indication of crossing the limit.
  */
  if (countM >= algDetLyrParams->topMAllClasses)
  {
    /*Late decision of DDR flow*/

    tidl_printf(0, "Valid points has excceded from assumption, DDR flow will be executed \n");
    algDetLyrParams->topMScore = algDetLyrParams->topMScoreDdr;
    algDetLyrParams->topMScoreSorted = algDetLyrParams->topMScoreSortedDdr;
    algDetLyrParams->topMIndices = algDetLyrParams->topMIndicesDdr;
    algDetLyrParams->topMIndicesSorted = algDetLyrParams->topMIndicesSortedDdr;

    algDetLyrParams->topM = algDetLyrParams->topMDdr / (params->numClasses - algDetLyrParams->isBckClsAvailable);
    algDetLyrParams->topM = (algDetLyrParams->topM >> simdShift) << simdShift; // making multiple of 16 as part for ci restriction

    // ci code has restriction to have topMAllClasses value 16 less than acctual value
    algDetLyrParams->topMAllClasses = algDetLyrParams->topM * (params->numClasses - algDetLyrParams->isBckClsAvailable) - simdWidth;

    if ((flowCtrl & TIDL_FLOW_CTRL_REF_ONLY) == TIDL_FLOW_CTRL_REF_ONLY)
    {
      if (algDetLyrParams->elementType == TIDL_SignedChar)
      {
        countM =
            TIDL_findValidLocation_cn<int8_t>(params, algDetLyrParams, priorData);

        if (countM <= algDetLyrParams->topMAllClasses)
        {
          TIDL_sparseDetScoreCalc_cn<int8_t>(params, algDetLyrParams);
        }
      }
      else if (algDetLyrParams->elementType == TIDL_SignedShort)
      {
        countM =
            TIDL_findValidLocation_cn<int16_t>(params, algDetLyrParams, priorData);

        if (countM <= algDetLyrParams->topMAllClasses)
        {
          TIDL_sparseDetScoreCalc_cn<int16_t>(params, algDetLyrParams);
        }
      }
      else if (algDetLyrParams->elementType == TIDL_SinglePrecFloat)
      {
        countM =
            TIDL_findValidLocation_cn<float>(params, algDetLyrParams, priorData);

        if (countM <= algDetLyrParams->topMAllClasses)
        {
          TIDL_sparseDetScoreCalc_cn<float>(params, algDetLyrParams);
        }
      }
      else
      {
        status = TIDL_ERR_NOT_IMPLEMENTED;
        tidl_printf(0, "Not supported for this data type \n");
      }
    }
    else
    {
      pKerPrivArgsL->execute((void *)pKerPrivArgsL, NULL, NULL, NULL, NULL);
    }
  }
  return status;
}

/**
 * @brief Function is to collect location and confedence scores
 *
 * @param TIDLLayers : Pointer to the common layer parameters
 * @param algLayer : Pointer to the layer specific parameters
 * @param inPtrs : Pointer to input data memory
 * @param priorData : Pointer to prior data buffer
 */
void TIDL_collectLocConfHeadInfo(sTIDL_Layer_t * TIDLLayers, sTIDL_AlgLayer_t * algLayer, void * inPtrs[], void* priorData)
{
  sTIDL_DataParams_t       *inLocDataParams;
  sTIDL_DataParams_t       *inConfDataParams;
  sTIDL_DataParams_t       *inExtraDataParams;
  sTIDL_AnchorBoxParams_t  *anchorBox = (sTIDL_AnchorBoxParams_t*)priorData;
  sTIDL_Layer_t * tidlLayer = &TIDLLayers[algLayer->layerIdx];
  sTIDL_DetectOutputParams_t * params     = &tidlLayer->layerParams.detectOutParams;
  sTIDL_ALgDetectOutputParams_t * algDetLyrParams = &algLayer->layerParams.detectionOutputParams;
  if (params->subCodeType == TIDL_ObjectPose)
  {
    algDetLyrParams->numSubCodeElements = sizeof(TIDL_ODLayerObjectPose)/sizeof(float32_tidl);
  }
  else
  {
   algDetLyrParams->numSubCodeElements = 0;
  }
  algDetLyrParams->numInElementPerKeypoint = ((sizeof(TIDL_ODLayerKeyPoint)/sizeof(float32_tidl)) - 1 + params->keypointConfidence);
  algDetLyrParams->numOutElementPerKeypoint = sizeof(TIDL_ODLayerKeyPoint)/sizeof(float32_tidl);

  int32_t      numPriors;
  int32_t      num_keypoint;
  int32_t      elementType;
  int32_t      locHeadOffset   = 0;
  int32_t      confHeadOffset  = 0;
  int32_t      extraHeadOffset = 0;

  /*This information can come from import tool, to make code free of meta architecture type.*/
  if(params->metaArchType == TIDL_metaArchTIDLYolo)
  {
    locHeadOffset  = 0;
    confHeadOffset = 0;
  }
  else
  {
    locHeadOffset  = 0;
    confHeadOffset = params->numHeads;
    if(params->metaArchType == TIDL_metaArchTIDL3DOD)
    {
      extraHeadOffset= 2*params->numHeads;
    }
  }

  for(int32_t j = 0; j < params->numHeads; j++)
  {
    inLocDataParams     = &TIDLLayers[algLayer->inLayerIdx[j + locHeadOffset]].outData;
    inConfDataParams    = &TIDLLayers[algLayer->inLayerIdx[j + confHeadOffset]].outData;
    inExtraDataParams   = &TIDLLayers[algLayer->inLayerIdx[j + extraHeadOffset]].outData;

    numPriors           = anchorBox[j].numAnchors;
    num_keypoint        = anchorBox[j].numKeyPoints;
    algDetLyrParams->numAnchors[j] = numPriors;

    if(params->metaArchType == TIDL_metaArchTIDL3DOD)
    {
      algDetLyrParams->boxParamsSize = 7 + 2*num_keypoint;
      algDetLyrParams->extraParamSize = 2;
    }
    else
    {
      algDetLyrParams->boxParamsSize = 4 + algDetLyrParams->numInElementPerKeypoint * num_keypoint
                                         + algDetLyrParams->numSubCodeElements;
      algDetLyrParams->extraParamSize = 0;
    }

    /*This Error check can go inside import tool*/
    if(params->metaArchType != TIDL_metaArchTIDLYolo)
    {
      if(inLocDataParams->dimValues[TIDL_DIM_NUMCH] != numPriors * algDetLyrParams->boxParamsSize * params->numSubHeads)
      {
        tidl_printf(0,"location head dimension is not correct for head = %d \n", j);
      }

      if(inConfDataParams->dimValues[TIDL_DIM_NUMCH] != numPriors * params->numClasses * params->numSubHeads)
      {
        tidl_printf(0, "Confidence head dimension is not correct for head = %d \n", j);
      }
      if(params->metaArchType == TIDL_metaArchTIDL3DOD)
      {
        if(inExtraDataParams->dimValues[TIDL_DIM_NUMCH] != numPriors * 2 * params->numSubHeads)
        {
          tidl_printf(0, "Extra Feat head dimension is not correct for head = %d \n", j);
        }
      }
    }

    for(int32_t k = 0; k < params->numSubHeads; k++)
    {
      /*get location data pointer, scale and other parameters*/
      uint32_t inPitch       = (uint32_t)inLocDataParams->pitch[TIDL_LINE_PITCH];
      algDetLyrParams->locDataOffset[j*params->numSubHeads + k]       =
            (inLocDataParams->padH*inPitch + inConfDataParams->padW)*TIDL_getDatElementSize(inLocDataParams->elementType);

      elementType                             = inLocDataParams->elementType; /*All input element types are expected to be same*/

      algDetLyrParams->locHeadPitchList[j*params->numSubHeads + k][TIDL_ROI_PITCH] = inLocDataParams->pitch[TIDL_ROI_PITCH];
      algDetLyrParams->locHeadPitchList[j*params->numSubHeads + k][TIDL_CHANNEL_PITCH] = inLocDataParams->pitch[TIDL_CHANNEL_PITCH];
      algDetLyrParams->locHeadPitchList[j*params->numSubHeads + k][TIDL_LINE_PITCH] = inLocDataParams->pitch[TIDL_LINE_PITCH];

      /* Get conf data  pointer and scale*/
      inPitch                   = (uint32_t)inConfDataParams->pitch[TIDL_LINE_PITCH];
      /* Except of top and bottom padding all the data is processed*/
      algDetLyrParams->confDataOffset[j*params->numSubHeads + k]  =
        (inConfDataParams->padH*inPitch + inConfDataParams->padW)*TIDL_getDatElementSize(inConfDataParams->elementType);

      /*This information can come from import tool, to make code free of meta architecture type.*/
      /*For yolo there is no seperate head for location and confidence, instead in same single head
        first 4 channels are for location parameter, and rest channel data is for objectness threshold
        and confdence score.
      */
      if(params->metaArchType == TIDL_metaArchTIDLYolo)
      {
        algDetLyrParams->confDataOffset[j*params->numSubHeads + k] +=
          ((inConfDataParams->pitch[TIDL_CHANNEL_PITCH]*4)*TIDL_getDatElementSize(inConfDataParams->elementType));
      }

      algDetLyrParams->confHeadPitchList[j*params->numSubHeads + k][TIDL_ROI_PITCH]   = inConfDataParams->pitch[TIDL_ROI_PITCH];
      algDetLyrParams->confHeadPitchList[j*params->numSubHeads + k][TIDL_CHANNEL_PITCH]   = inConfDataParams->pitch[TIDL_CHANNEL_PITCH];
      algDetLyrParams->confHeadPitchList[j*params->numSubHeads + k][TIDL_LINE_PITCH]   = inConfDataParams->pitch[TIDL_LINE_PITCH];

      algDetLyrParams->confHeadInvPitchList[j*params->numSubHeads + k][TIDL_ROI_PITCH]   = (1.0f/inConfDataParams->pitch[TIDL_ROI_PITCH]);
      algDetLyrParams->confHeadInvPitchList[j*params->numSubHeads + k][TIDL_CHANNEL_PITCH]   = (1.0f/inConfDataParams->pitch[TIDL_CHANNEL_PITCH]);
      algDetLyrParams->confHeadInvPitchList[j*params->numSubHeads + k][TIDL_LINE_PITCH]   = (1.0f/inConfDataParams->pitch[TIDL_LINE_PITCH]);

      /*get extra data pointer, scale and other parameters*/
      inPitch       = (uint32_t)inExtraDataParams->pitch[TIDL_LINE_PITCH];
      algDetLyrParams->extraDataOffset[j*params->numSubHeads + k]     =
        (inExtraDataParams->padH*inPitch + inExtraDataParams->padW)*TIDL_getDatElementSize(inExtraDataParams->elementType);
      elementType                             = inExtraDataParams->elementType; /*All input element types are expected to be same*/

      algDetLyrParams->extraHeadPitchList[j*params->numSubHeads + k][TIDL_ROI_PITCH] = inExtraDataParams->pitch[TIDL_ROI_PITCH];
      algDetLyrParams->extraHeadPitchList[j*params->numSubHeads + k][TIDL_CHANNEL_PITCH] = inExtraDataParams->pitch[TIDL_CHANNEL_PITCH];
      algDetLyrParams->extraHeadPitchList[j*params->numSubHeads + k][TIDL_LINE_PITCH] = inExtraDataParams->pitch[TIDL_LINE_PITCH];

    }

  }

  algDetLyrParams->elementType = elementType;
  algDetLyrParams->elmSize     = TIDL_getDatElementSize(elementType);

  if((params->backgroundLabelId >= 0) && (params->backgroundLabelId < params->numClasses))
  {
    algDetLyrParams->isBckClsAvailable = 1;
  }
  else
  {
    algDetLyrParams->isBckClsAvailable = 0;
  }
}
