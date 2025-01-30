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

#ifndef TIDL_IMPORT_COMMONH_
#define TIDL_IMPORT_COMMONH_ 1

#include "perfsim.h"
#include <google/protobuf/message.h>
using ::google::protobuf::Message;

#define ALIGN_NET_DATA (64)
#define TF_PAD_SAME (0)
#define TF_PAD_VALID (1)
#define TIDL_BATCHPADMAXVALUE (8)
#define MAX_NUM_DETECTION_LAYER (TIDL_MAX_TF_SSD_LAYERS)

bool TIDL_readProtoFromTextFile(const char* fileName, Message* proto);

bool TIDL_readProtoFromBinaryFile(const char* fileName, Message* proto);

int32_t TIDL_getDataID(sTIDL_DataParams_t *data,
sTIDL_OrgNetwork_t * pOrgTIDLNetStructure,
int32_t            numLayer,
int8_t             *bufName);

int32_t TIDL_isDataBufUsed(int32_t           dataId,
sTIDL_Network_t   *pTIDLNetStructure,
int32_t           numLayer);
int32_t TIDL_isInputConv2D(sTIDL_OrgNetwork_t   *pOrgTIDLNetStruct,
int32_t              numLayer,
const char           *bufName);
void TIDL_UpdateInDataBuff(sTIDL_OrgNetwork_t * pOrgTIDLNetStructure,
uint32_t numLayers, sTIDL_DataParams_t dataBuf);

void TIDL_findRange(float * data, int32_t dataSize, float * minOut, float * maxOut, float scale);
int32_t TIDL_alignParamsWrite(FILE *fp, sBuffer_t * buf, uint32_t *totalParamSize, uint32_t numBytes);

void TIDL_asymAllocScalesPointers(sTIDL_OrgNetwork_t * pOrgTIDLNetStructure, uint32_t numLayers);
void TIDL_setDefaultKernelType(sTIDL_OrgNetwork_t * pOrgTIDLNetStructure, uint32_t numLayers);
void TIDL_allocAndCopyModelParams(sTIDL_OrgNetwork_t * orgTIDLNetStructureDst,sTIDL_OrgNetwork_t * orgTIDLNetStructureSrc, uint32_t numLayers);
void TIDL_copyModelParams(sTIDL_OrgNetwork_t * orgTIDLNetStructureDst,sTIDL_OrgNetwork_t * orgTIDLNetStructureSrc, uint32_t numLayers);
void TIDL_freeModelParams(sTIDL_OrgNetwork_t * pOrgTIDLNetStructure, uint32_t numLayers);

int32_t TIDL_writeModel(sTIDL_Network_t * tIDLNetStructure,
                                            sTIDL_OrgNetwork_t * orgTIDLNetStructure,
                                            const char * name,
                                            uint32_t numLayers,
                                            tidl_import_config * configParams);

void  TIDL_fillDataBufPadRequirements(sTIDL_Network_t * tIDLNetStructure);
void * my_malloc(int size);
void my_free(void *ptr);
int32_t TIDL_writeInfo(const sTIDL_Network_t * tIDLNetStructure,
                                        const sTIDL_OrgNetwork_t * orgTIDLNetStructure,
                                        const char * name,
                                        uint32_t numLayers,
                                        uint32_t currLayersGroupId,
                                        const sPerfSim_t * perfSimInfo,
                                        const tidl_import_config * configParams);

#define NUM_WHGT_BITS   (gParams.numParamBits)
#define NUM_BIAS_BITS   (8)
#define PRINT_TENSOR_MINMAX (0)
#define LAYER_INFO_FILENAME "layer_info.txt"

#define TIDL_ALLOWLISTING_LAYER_TYPE_UNSUPPORTED (-2)
#define TIDL_ALLOWLISTING_LAYER_CHECK_FAILED (-1)
#define TIDL_ALLOWLISTING_LAYER_CHECK_PASSED (0)

int32_t TIDL_getLayerIdx(sTIDL_OrgNetwork_t * pOrgTIDLNetStructure, int32_t numLayer, const char *bufName);
typedef struct {
  int32_t layerType;
  int32_t(*tidl_tfOutReshape)(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure,
    int32_t              layerIndex);
}sTIDL_tfOutReshapeMap_t;
extern sTIDL_tfOutReshapeMap_t sTIDL_tfOutReshapeTable[];

typedef struct {
  char           *layerName;
  char           *layerOpsString;
  uint32_t       NumOps;
} TIDL_TFLayerMapping_t;

#define TIDL_MAX_HEADS_TF_SSD   (16)
#define TIDL_MAX_AR_PER_HEAD    (16)

#define MAX_NUM_DETECTION_LAYER (4)

#define TIDL_MAX_TF_SSD_LAYERS  (MAX_NUM_DETECTION_LAYER)
#define TIDL_MAX_TF_FASTER_RCNN_LAYERS  (MAX_NUM_DETECTION_LAYER)

typedef struct {
  int32_t  num_classes;
  int32_t  num_keypoints;
  int32_t  num_aspect_ratios;
  int32_t  num_layers;
  int32_t  max_detections_per_class;
  int32_t  max_total_detections;
  int32_t  score_converter;
  int32_t  encode_background_as_zeros;
  float    y_scale;
  float    x_scale;
  float    height_scale;
  float    width_scale;
  float    scales[TIDL_MAX_HEADS_TF_SSD + 1];
  float    aspect_ratios[TIDL_MAX_AR_PER_HEAD];
  float    interpolated_scale_aspect_ratio;
  float    reduce_boxes_in_lowest_layer;
  float    base_anchor_height;
  float    base_anchor_width;
  float    height_stride[TIDL_MAX_HEADS_TF_SSD];
  float    width_stride[TIDL_MAX_HEADS_TF_SSD];
  float    height_offset[TIDL_MAX_HEADS_TF_SSD];
  float    width_offset[TIDL_MAX_HEADS_TF_SSD];
  float    score_threshold;
  float    iou_threshold;
  float *   anchorInputsFromModel;
  std::vector<int> num_ars;
  std::vector<string> box_input;
  std::vector<string> class_input;
} TIDL_TFSSDConfig_t;


typedef struct {
  //:TODO: Rename macros to RCNN
  int32_t num_scales;
  int32_t num_aspect_ratios;
  int32_t max_proposals;
  float    scales[TIDL_MAX_HEADS_TF_SSD + 1];
  float    aspect_ratios[TIDL_MAX_AR_PER_HEAD];
  float    height_stride;/*Anchor stride in height dimension in pixels */
  float    width_stride;/*Anchor stride in width dimension in pixels.*/
  float    height_offset;/*Anchor height offset in pixels.*/
  float    width_offset;/*Anchor width offset in pixels */
  float    width;/*Anchor width in pixels */
  float    height;/*Anchor height in pixels */
  float    nms_score_threshold;
  float    nms_iou_threshold;
} TIDL_TFFasterRCNNFirstStageConfig_t;

typedef struct {
  int32_t max_total_detections;
  int32_t max_detections_per_class;
  float    nms_score_threshold;
  float    nms_iou_threshold;
} TIDL_TFFasterRCNNSecondStageConfig_t;


typedef struct {
  int32_t  number_of_stages;
  int32_t  num_classes;
  int32_t initial_crop_size;
  int32_t maxpool_kernel_size;
  int32_t maxpool_stride;
  TIDL_TFFasterRCNNFirstStageConfig_t firstStageConfig;
  TIDL_TFFasterRCNNSecondStageConfig_t secondStageConfig;
} TIDL_TFFasterRCNNConfig_t;

typedef enum {
  ADD_LAYER_BEFORE = 0,
  ADD_LAYER_AFTER
} TIDL_addLayer_opType_t;

typedef enum {
    BASE_OP_SUB = 0,
    BASE_OP_ADD,
    BASE_OP_DIV,
    BASE_OP_MUL
} TIDL_base_operator_t;

int32_t TIDL_tfOutReshapeDataLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapeConvLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapePoolingLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapeIdentity(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapeEltwise(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapeBN(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapeRelu(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapePRelu(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapeSigmoid(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapeTanh(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapeHardSigmoid(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapeELU(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapeSoftmax(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapeIPLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapeDeConvLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapeConcatLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapeSliceLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapeCropLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapeFlattenLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapeArgmaxLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapePadLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapeDetOutLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapeRoiPoolingLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapeOdPostProcessingLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_optimizeSqueezeLayers(sTIDL_OrgNetwork_t &orgTIDLNetStructure, int32_t* dataIndex, int32_t numLayers);
int32_t tidl_optimizeTransposeLayers(sTIDL_OrgNetwork_t &orgTIDLNetStructure, int32_t* dataIndex, int32_t numLayers);
int32_t tidl_optimizeGlobalAveragePoolingLayers (sTIDL_OrgNetwork_t &orgTIDLNetStructure, int32_t* dataIndex, int32_t numLayers);
int32_t tidl_optimizeSoftmaxAxis(sTIDL_OrgNetwork_t &orgTIDLNetStructure, int32_t* dataIndex, int32_t numLayers);
int32_t TIDL_tfOutReshapeOdOutputReformatLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapeReduceMaxLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapeScatterElementsLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_AddTfODOutputLayers(sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t layerIndex, int32_t * dataIndex, int32_t metaArchTtype);
int32_t tidl_AddOnnxODOutputLayers(sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t layerIndex, int32_t * dataIndex);
int32_t TIDL_tfOutReshapeDataConvert(sTIDL_OrgNetwork_t *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapeReshapeLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapeGatherLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapeConstDataLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapeTransposeLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapeSqueeze(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);

int32_t TIDL_getTfOpParamMapId(const char  * name);
int32_t tidl_linkInputTensors(sTIDL_OrgNetwork_t  *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_linkOutputTensors(sTIDL_OrgNetwork_t  *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_isAllInsAvailable(sTIDL_LayerPC_t  *orgLayer, sTIDL_OrgNetwork_t  *ptempTIDLNetStructure, int32_t layerIndex);
int32_t tidl_sortLayersInProcOrder(sTIDL_OrgNetwork_t  *pOrgTIDLNetStructure, sTIDL_OrgNetwork_t  *ptempTIDLNetStructure, int32_t layerIndex);
int32_t tidl_removeMergedLayersFromNet(sTIDL_OrgNetwork_t  *pOrgTIDLNetStructure, sTIDL_OrgNetwork_t  *ptempTIDLNetStructure);
int32_t tidl_upateAInDataId(sTIDL_OrgNetwork_t  *pOrgTIDLNetStructure, int32_t layerIndex, int32_t oldId, int32_t currId);
int32_t tidl_sortDataIds(sTIDL_OrgNetwork_t  *pOrgTIDLNetStructure);
int32_t tidl_makeDataIdLayerIdSame(sTIDL_OrgNetwork_t  *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_updateOutDataShape(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t startIdx, int32_t layerIndex, sTIDL_tfOutReshapeMap_t * sTIDL_tfOutReshapeTable);
int32_t tidl_getInLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex, int32_t dataId);
int32_t tidl_getOutLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex, int32_t dataId);
int32_t tidl_getOutLayer(sTIDL_OrgNetwork_t  *pOrgTIDLNetStructure, int32_t layerIndex, int32_t dataId);
int32_t tidl_getInLayer(sTIDL_Network_t  *pOrgTIDLNetStructure, int32_t layerIndex, int32_t dataId);
int32_t tidl_getOutLayer(sTIDL_Network_t  *pOrgTIDLNetStructure, int32_t layerIndex, int32_t dataId);
int32_t tidl_mergeFalttenLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_mergeBiasLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_mergePadLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_mergeBNLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_mergeYuv444InConvLayer(sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t layerIndex, int32_t inDataFormat);
int32_t tidl_mergeReluLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_mergeGeluLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_mergeLayerNormLayer (sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_convertConv2DToIpLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex, sTIDL_tfOutReshapeMap_t * sTIDL_tfOutReshapeTable);
int32_t tidl_copyPCNetToDeviceNet(sTIDL_OrgNetwork_t  * pOrgTIDLNetStructure,
                                                                  sTIDL_Network_t  * pTIDLNetStructure,
                                                                  tidl_import_config * configParams,
                                                                  int32_t numLayers);
int32_t tidl_addOutDataLayer(sTIDL_Network_t  * tIDLNetStructure, int32_t tiLayerIndex);
int32_t tidl_splitOutDataLayer(sTIDL_OrgNetwork_t &orgTIDLNetStructure,
                                 int32_t &layerIndex);
void tidl_updateWeightElemSize(sTIDL_OrgNetwork_t  * pOrgTIDLNetStructure, tidl_import_config * params, int32_t numLayers);
void tidl_convertElementTypeGivenParambits(sTIDL_OrgNetwork_t  * pOrgTIDLNetStructure, int32_t numLayers, int32_t numParamBits);
int32_t tidl_fillInDataLayerShape(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, tidl_import_config * params, int32_t tiLayerIndex);
void TIDL_convertDeconv2DtoConv(sTIDL_OrgNetwork_t   &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_addInDataLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex, int32_t * dataIndex);
int32_t tidl_addConstDataLayers(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex, int32_t * dataIndex, tidl_import_config *params);
int32_t tidl_createConstDataLayer (sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t* dataIndex, float* data, int32_t dataSize, int32_t layerIndex);
int32_t tidl_doesLayerSupportSpatialSplit(sTIDL_OrgNetwork_t& pOrgTIDLNetStructure, int32_t lyrIdx, int32_t numCores);
void tidl_updateScratchMemoryRequirement(sTIDL_OrgNetwork_t& pOrgTIDLNetStructure);
void tidl_updateMultiCoreState(sTIDL_OrgNetwork_t& pOrgTIDLNetStructure, tidl_import_config * params);
int32_t tidl_doesNetworkHasMatmulLayer(sTIDL_OrgNetwork_t& pOrgTIDLNetStructure);
int32_t tidl_addCropLayerForMultiCore(sTIDL_OrgNetwork_t& pOrgTIDLNetStructure, int32_t layerIndex, int32_t * dataIndex, int32_t quantizationStyle, int32_t numCores);
template <class Tin>
int32_t tidl_convertNHWCtoNCHW(Tin* dst, Tin* src, int32_t n, int32_t c, int32_t h, int32_t w);
int32_t tidl_mergeIdentitytLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_mergeQDQLayers(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex, bool mergeQuant);
int32_t tidl_mergeInitializerDQLayers(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_merge1x1MaxPoolingLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_mergePoolingLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_mergeDepthToSpaceLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_mergeBatchToSpaceInDWConvLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_optimizePatchMergingBlock (sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t* dataIndex, int32_t& layerIndex);
int32_t tidl_fusePatchMergingBlock (sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t* dataIndex, int32_t& layerIndex);
int32_t tidl_reduceSliceDims(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t* dataIndex, int32_t& layerIndex);
int32_t tidl_duplicateSliceLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t& layerIndex);
int32_t tidl_mergeDetectionoutLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_removeConcatFlattenLayerInDetNet(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_removeConcatReshapeLogisticLayerInDetNet(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_removeDivideByOneLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_mergeSplitLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_mergeReshapeLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex, sTIDL_tfOutReshapeMap_t * sTIDL_tfOutReshapeTable);
int32_t tidl_mergeNoOpReshapeLayers (sTIDL_OrgNetwork_t &orgTIDLNetStructure, int32_t numLayers);

int32_t TIDL_tfOutReshapeResize(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_addNormLayerToInData(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex, int32_t * dataIndex, tidl_import_config * params);
int32_t tidl_addPadLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex, int32_t * dataIndex, tidl_import_config * params);
int32_t TIDL_addLayer(sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t layerIdx, int32_t newLayerType, int32_t* dataIndex, int32_t numLayers, int32_t opType);
int32_t tidl_convertIpLayerInputShape(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_convertSqueezeToFlattenLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_convertPoolToIdentityLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_convertSoftMaxLayerInputShape(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tf_getLayreTypeMapIdx(char* layerName, TIDL_TFLayerMapping_t* TIDL_TFLayerMap, int32_t tblSize);
int32_t tf_isLayerType(char* layerName, int32_t  startLayer, sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, TIDL_TFLayerMapping_t* TIDL_TFLayerMap, int32_t tblSize);
int32_t tidl_convertRelUToBNLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_getStringsFromList(char *list, char * names, int strLen);
int32_t tidl_mergePreBNLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_getDimPadLayerWithStrideOffset(int32_t * padT, int32_t * padB, int32_t * padL, int32_t * padR,
                       const sTIDL_ConvParams_t * convParams,
                       int32_t convOutHeight, int32_t convOutWidth,
                       int32_t convInHeight, int32_t convInWidth, int32_t * strideOffsetMethod);
int32_t tidl_mergeFlattenLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_getElementType(int32_t sign);
int32_t tidl_getElementSizeInBits(int32_t elementType);
int32_t TIDL_getDatElementSign(int32_t elementType);
const char* TIDL_getOutDataName(const sTIDL_OrgNetwork_t * pOrgTIDLNetStructure, int32_t dataId);
const sTIDL_DataParams_t * TIDL_getOutDataInNet(const sTIDL_Network_t * tidlNet,
                                     int32_t dataId);
const sTIDL_DataParams_t *TIDL_getOutData(const sTIDL_OrgNetwork_t *pOrgTIDLNetStructure,
                                    int32_t dataId);
void getAbsPath(char *path, char *absPath);
bool TIDL_doesLayerSupportAsymTensors(sTIDL_LayerPC_t* layerType);
int32_t getNumberOfLinesIntheFile(char *fileName);
int32_t updatePadAndWriteModel(sTIDL_OrgNetwork_t *pTidlOrigNet,
                               sTIDL_Network_t *pTidlNet,
                               tidl_import_config *configParams);
void TIDL_copyTensorStats(sTIDL_OrgNetwork_t *pOrgTIDLNetStructureDst,
                          sTIDL_OrgNetwork_t *pOrgTIDLNetStructureSrc,
                          int32_t skip_input_datalayers);
int tidlRunQuantStatsTool(sTIDL_OrgNetwork_t *pOrgTIDLNetStructure,
                          sTIDL_Network_t *pTIDLNetStructure,
                          tidl_import_config *params,
                          uint32_t numLayers);
void getDirFromPath(char *path);
char *getFileNameFromPath(char *path);
int32_t tidl_addMetaArchLayersToNet(sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t layerIndex, int32_t *dataIndex, tidl_import_config *params, int32_t detLayerId);
int32_t tidl_addInDataLayerForUVData(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure,
                                     int32_t layerIndex,
                                     int32_t *dataIndex,
                                     tidl_import_config *params,
                                     int32_t *inputList,
                                     int32_t numInLayers);
int32_t tidl_addUVConversionLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure,
                                  int32_t layerIndex,
                                  int32_t *dataIndex,
                                  int32_t *uvLayerList);
int32_t tidl_addConcatLayerForYUVMerge(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure,
                                       int32_t layerIndex,
                                       int32_t *dataIndex,
                                       int32_t *inputList,
                                       int32_t (*outForInputList)[TIDL_NUM_OUT_BUFS],
                                       int32_t *uvLayerList,
                                       int32_t *newConcatLayerList,
                                       int32_t *numOutLayers,
                                       int32_t numInLayers);
int32_t tidl_addNormLayerToConcat(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure,
                                  int32_t layerIndex,
                                  int32_t *dataIndex,
                                  tidl_import_config *params,
                                  int32_t *concatLayerList,
                                  int32_t numConcatLayers);
int32_t tidl_addConvLayerAfterConcat(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure,
                                     int32_t layerIndex,
                                     int32_t *dataIndex,
                                     tidl_import_config *params,
                                     int32_t *concatLayerList,
                                     int32_t *newConvList,
                                     int32_t numConcatLayers);
int32_t tidl_fillRgbToYuvConvLayerList(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure,
                                       int32_t layerIndex,
                                       int32_t *dataIndex,
                                       tidl_import_config *params,
                                       int32_t *convLayerList);
int32_t tidl_addLayersForUVConversion(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t *dataIndex, tidl_import_config *params);
int32_t tidl_mergeYUVtoRGBConversion(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t *dataIndex, tidl_import_config *params);
int32_t tidl_addPadLayerAndConv1x1ForYUVtoRGB(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure,
                                              int32_t layerIndex,
                                              int32_t *dataIndex,
                                              tidl_import_config *params,
                                              int32_t *convLayerList,
                                              int32_t * padLayerList,
                                              int32_t * numYuvMergeConvLayers,
                                              int32_t * numYuvMergePadLayers);
void tidl_getInverse3X3Matrix(float32_tidl mat[3][3], float32_tidl matInv[3][3]);
int32_t tidl_addBatchReshapeLayerForBatchProcessing(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t * dataIndex, int32_t layerIndex, int32_t *layerID, int32_t *roiPadTotal);
int32_t TIDL_doesLayerSupportBatchProcessing(sTIDL_LayerPC_t * layer);
void tidl_initAndUpdateBatchPadRequirements(sTIDL_OrgNetwork_t * tIDLNetStructure);

extern uint32_t TIDL_kernelReshape(float * param, uint32_t w, uint32_t h, uint32_t ci, uint32_t co);
extern uint32_t TIDL_kernelReshapeHWOI(float * param, uint32_t w, uint32_t h, uint32_t ci, uint32_t co);
extern uint32_t TIDL_kernelReshapeIOHW(float * param, uint32_t w, uint32_t h, uint32_t ci, uint32_t co);
extern int32_t tidl_FindFlattenLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex);
extern int32_t tidl_FindHardSigmoidLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex);
extern int32_t tidl_mergePackToNNResizeLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_mergeClipLayer(sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapeClip(sTIDL_OrgNetwork_t *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_mergeMinimumLayer(sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t layerIndex);
void tidl_replaceInTensorName(sTIDL_OrgNetwork_t  *pOrgTIDLNetStructure, int32_t layerIndex, char * orgTensorName, char * newTensorName);
int32_t tidltb_isInDataBuff(const sTIDL_Network_t * pTIDLNetStructure, int32_t dataId, int32_t layersGroupId);
int32_t tidltb_isOutDataBuff(const sTIDL_Network_t *pTIDLNetStructure, int32_t dataId, int32_t layersGroupId);
void TIDL_caffeReorderDeconvFilters(sBuffer_t &buf, int32_t gno, int32_t gni, int32_t gr, int32_t k);
int32_t TIDL_tfOutReshapeDeptoSpace(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapeBatchtoSpace(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapeSpacetoBatch(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapeColorConversionLayer(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);
int32_t TIDL_tfOutReshapeBatchReshape(sTIDL_OrgNetwork_t   *pOrgTIDLNetStructure, int32_t layerIndex);

#define TIDL_MAX_DATA_NAME (500)

extern char inDataNames[TIDL_MAX_ALG_IN_BUFS][TIDL_MAX_DATA_NAME];
extern char outDataNames[TIDL_MAX_ALG_OUT_BUFS][TIDL_MAX_DATA_NAME];
extern int32_t numNetInData;
extern int32_t numNetOutData;


extern sTIDL_tfOutReshapeMap_t sTIDL_OutReshapeTable[];
int32_t tidl_FindOnnxShuffleLayer(sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_FindMatMulBias(sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_fuseLayerNormBetaGamma(sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t layerIndex, int32_t* dataIndex);
int32_t tidl_identifyPatchEmbeddingBlock(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_FindFlattenLayer(sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_FindHardSigmoidLayer(sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_mergePackToNNResizeLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_FindOnnxShuffleLayer(sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t layerIndex);
int32_t tidl_splitResizeLayer(sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t &layerIndex, int32_t& dataIndex);
int32_t tidl_merge1x1ResizeLayer(sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t layerIndex);

extern void TIDL_tfBNToScaleBias(float* scale, float* bias, uint32_t numCh, float* mean, float* var, float* gamma, float* beta, float eps);
extern int32_t tidl_convertBiasToBNLayer(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t numLayers);
extern int32_t tidl_convertReshapeToFlatten(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t numLayers);

int32_t tidl_addDataConvertLayer(sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t layerIndex, int32_t *dataIndex, tidl_import_config *params);
int32_t TIDL_handlePadforTransposeLayers (sTIDL_OrgNetwork_t &orgTIDLNetStructure, int32_t* dataIndex, int32_t numLayers);
int32_t TIDL_handleTransposeLayers(sTIDL_OrgNetwork_t &orgTIDLNetStructure, int32_t* dataIndex, int32_t numLayers);
int32_t TIDL_convertTransposetoReshape(sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t numLayers);
int32_t TIDL_find_index(int32_t *perm, int32_t val, int32_t size);
int32_t TIDL_convertTransposeToDataConvert(sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t* dataIndex, int32_t numLayers);
int32_t TIDL_removeTransposeFromNet(sTIDL_OrgNetwork_t &orgTIDLNetStructure, int32_t numLayers);
int32_t tidl_addDataConvertLayerForOTFsupport(sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t layerIndex, int32_t *dataIndex, tidl_import_config *params, unordered_set<int32_t>& LayerList);
int32_t tidl_addCropLayerForValidPadSupport(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex, int32_t * dataIndex, tidl_import_config * params);
int32_t TIDL_getConfigParamOutIndexFromLayerName(const char * layerName);
bool TIDL_checkIsDirectory(const char *path);
bool tidl_findReshapeinNetwork(sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t numLayers);
int32_t mergereshapeLayers(sTIDL_OrgNetwork_t& orgTIDLNetStructure);
void tidl_checkPadForDCLayers(sTIDL_Network_t  *tidlNet, sTIDL_OrgNetwork_t &pOrgTIDLNetStructure, int32_t reshapeIdx, int32_t* removeDC1, int32_t* removeDC2);
int32_t TIDL_getCompilerDataSize(const sPerfSim_t * perfSimInfo, int32_t numLayers);
int32_t TIDL_getNetworkStructDataSize(const sTIDL_Network_t *pNet);
void tidl_poolPadParams(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t layerIndex);

sBufParams_t * TIDL_getInBufParams(const sTIDL_Network_t *net,int32_t prevDataId, int32_t curdataId, int32_t currLayersGroupId);
sBufParams_t * TIDL_getOutBufParams(const sTIDL_Network_t *net, int32_t dataId, int32_t currLayersGroupId);
int TIDL_importBitDepthProtoTxt(sTIDL_OrgNetwork_t *pOrgTIDLNetStructure, 
                                        tidl_import_config *configParams);
int32_t  tidl_InnerProductBTranspose (sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t numLayers);

int32_t tidl_optimizePatchEmbedding(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t& layerIndex, int32_t* dataIndex);
int32_t tidl_fuseTransposeMatMul(sTIDL_OrgNetwork_t  &pOrgTIDLNetStructure, int32_t& layerIndex, int32_t* dataIndex);

#endif /*TIDL_IMPORT_COMMONH_ */
