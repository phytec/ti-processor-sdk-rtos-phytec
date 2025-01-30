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
//#include <io.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <float.h>
#include <cmath>

#include "ti_dl.h"
#include "tidl_import_api.h"
#include "object_detection/protos/pipeline.pb.h"
#include "tidl_import_config.h"

using namespace std;
using namespace object_detection;
using namespace protos;
using ::google::protobuf::Message;
using ::google::protobuf::io::FileInputStream;
using ::google::protobuf::io::FileOutputStream;
using ::google::protobuf::io::ZeroCopyInputStream;
using ::google::protobuf::io::CodedInputStream;
using ::google::protobuf::io::ZeroCopyOutputStream;
using ::google::protobuf::io::CodedOutputStream;

#include "tidl_import_common.h"

extern char outMetaLayerNames[TIDL_MAX_ALG_OUT_BUFS][TIDL_MAX_DATA_NAME];
TIDL_TFSSDConfig_t tidl_TFSSDConfigs[TIDL_MAX_TF_SSD_LAYERS];
TIDL_TFFasterRCNNConfig_t tidl_TFFasterRCNNConfigs[TIDL_MAX_TF_FASTER_RCNN_LAYERS];

int32_t numTFMetaLayers = 0;

void tf_metaArch_faster_rcnn_import(tidl_import_config * params, const FasterRcnn * fasterRCNNDef,uint32_t numMetaLayers)
{
  uint32_t i, j;

  for ( i = 0; i < numMetaLayers; i++ )
  {
    TIDL_TFFasterRCNNConfig_t &tidl_TFFasterRCNNConfig = tidl_TFFasterRCNNConfigs[i];

    tidl_TFFasterRCNNConfig.number_of_stages = fasterRCNNDef->number_of_stages();
    tidl_TFFasterRCNNConfig.num_classes = fasterRCNNDef->num_classes();
    tidl_TFFasterRCNNConfig.firstStageConfig.nms_iou_threshold = fasterRCNNDef->first_stage_nms_iou_threshold();
    tidl_TFFasterRCNNConfig.firstStageConfig.nms_score_threshold = fasterRCNNDef->first_stage_nms_score_threshold();
    tidl_TFFasterRCNNConfig.firstStageConfig.max_proposals = fasterRCNNDef->first_stage_max_proposals();

    if (fasterRCNNDef->has_image_resizer() == true)
    {
      if ( fasterRCNNDef->image_resizer().has_keep_aspect_ratio_resizer())
      {
        if ((params->inWidth[0] == -1) && (params->inHeight[0] == -1))
        {
          //;TODO: This is not correct, need to update this
//          params->inWidth[0] = fasterRCNNDef->image_resizer().keep_aspect_ratio_resizer().min_dimension;
//          params->inHeight[0]  = fasterRCNNDef->image_resizer().keep_aspect_ratio_resizer().max_dimension;
        }
      }
    }

    if (fasterRCNNDef->has_feature_extractor() == true)
    {
    //:TODO: Need to see if we need to update this or not
    }

    if (fasterRCNNDef->has_first_stage_anchor_generator() == true)
    {
      if(fasterRCNNDef->first_stage_anchor_generator().has_grid_anchor_generator() == true)
      {
        tidl_TFFasterRCNNConfig.firstStageConfig.num_aspect_ratios = fasterRCNNDef->first_stage_anchor_generator().grid_anchor_generator().aspect_ratios_size();

        for ( j = 0; j < tidl_TFFasterRCNNConfig.firstStageConfig.num_aspect_ratios; j++)
        {
          tidl_TFFasterRCNNConfig.firstStageConfig.aspect_ratios[j] = fasterRCNNDef->first_stage_anchor_generator().grid_anchor_generator().aspect_ratios(j);
        }

        tidl_TFFasterRCNNConfig.firstStageConfig.num_scales= fasterRCNNDef->first_stage_anchor_generator().grid_anchor_generator().scales_size();

        for ( j = 0; j < tidl_TFFasterRCNNConfig.firstStageConfig.num_scales; j++)
        {
          tidl_TFFasterRCNNConfig.firstStageConfig.scales[j] = fasterRCNNDef->first_stage_anchor_generator().grid_anchor_generator().scales(j);
        }

        tidl_TFFasterRCNNConfig.firstStageConfig.width_offset = fasterRCNNDef->first_stage_anchor_generator().grid_anchor_generator().width_offset();
        tidl_TFFasterRCNNConfig.firstStageConfig.height_offset = fasterRCNNDef->first_stage_anchor_generator().grid_anchor_generator().height_offset();

        tidl_TFFasterRCNNConfig.firstStageConfig.width_stride  = fasterRCNNDef->first_stage_anchor_generator().grid_anchor_generator().width_stride();
        tidl_TFFasterRCNNConfig.firstStageConfig.height_stride = fasterRCNNDef->first_stage_anchor_generator().grid_anchor_generator().height_stride();

        tidl_TFFasterRCNNConfig.firstStageConfig.width  = fasterRCNNDef->first_stage_anchor_generator().grid_anchor_generator().width();
        tidl_TFFasterRCNNConfig.firstStageConfig.height = fasterRCNNDef->first_stage_anchor_generator().grid_anchor_generator().height();
      }

    }

    if (fasterRCNNDef->has_initial_crop_size() == true )
    {
      tidl_TFFasterRCNNConfig.initial_crop_size = fasterRCNNDef->initial_crop_size();
    }
    if (fasterRCNNDef->has_maxpool_kernel_size() == true )
    {
      tidl_TFFasterRCNNConfig.maxpool_kernel_size = fasterRCNNDef->maxpool_kernel_size();
    }
    if (fasterRCNNDef->has_maxpool_stride() == true )
    {
      tidl_TFFasterRCNNConfig.maxpool_stride = fasterRCNNDef->maxpool_stride();
    }

    if (fasterRCNNDef->has_second_stage_post_processing() == true)
    {
       if (fasterRCNNDef->second_stage_post_processing().has_batch_non_max_suppression() == true)
      {
        tidl_TFFasterRCNNConfig.secondStageConfig.nms_iou_threshold             = fasterRCNNDef->second_stage_post_processing().batch_non_max_suppression().iou_threshold();
        tidl_TFFasterRCNNConfig.secondStageConfig.nms_score_threshold         = fasterRCNNDef->second_stage_post_processing().batch_non_max_suppression().score_threshold();
        tidl_TFFasterRCNNConfig.secondStageConfig.max_detections_per_class  = fasterRCNNDef->second_stage_post_processing().batch_non_max_suppression().max_detections_per_class();
        tidl_TFFasterRCNNConfig.secondStageConfig.max_total_detections          = fasterRCNNDef->second_stage_post_processing().batch_non_max_suppression().max_total_detections();
      }
    }


  }

}

void tf_metaArch_import(tidl_import_config * params)
{
  int32_t  i, j;
  TrainEvalPipelineConfig           tfOdpipeLineDef;

  if (strcmp((char*)params->metaLayersNamesList, "") != 0)
  {
    numTFMetaLayers = tidl_getStringsFromList((char *)params->metaLayersNamesList, (char*)outMetaLayerNames, TIDL_MAX_DATA_NAME);
  }
  else
  {
    printf("INFO : No Mata Arch Layer to parse\n");
    return;
  }
  for (i = 0; i < numTFMetaLayers; i++)
  {
    printf("TF Meta PipeLine (Proto) File  : %s  \n", (const char *)outMetaLayerNames[i]);
    bool success = TIDL_readProtoFromTextFile((const char *)outMetaLayerNames[i], &tfOdpipeLineDef);
    if(!success) exit(-1);
    
    TIDL_TFSSDConfig_t &tidl_TFSSDConfig = tidl_TFSSDConfigs[i];


    if (tfOdpipeLineDef.has_model() == true)
    {
      if (tfOdpipeLineDef.model().has_ssd() == true)
      {
        auto &ssdDef = tfOdpipeLineDef.model().ssd();
        tidl_TFSSDConfig.num_classes = ssdDef.num_classes();

        if (ssdDef.encode_background_as_zeros())
        {
          tidl_TFSSDConfig.num_classes++;
          tidl_TFSSDConfig.encode_background_as_zeros = 1;
        }
        else
        {
          tidl_TFSSDConfig.encode_background_as_zeros = 0;
        }
        if (ssdDef.has_image_resizer())
        {
          if (ssdDef.image_resizer().has_fixed_shape_resizer())
          {
            if ((params->inWidth[0] == -1) && (params->inHeight[0] == -1))
            {
              params->inWidth[0] = ssdDef.image_resizer().fixed_shape_resizer().height();
              params->inHeight[0] = ssdDef.image_resizer().fixed_shape_resizer().width();
            }
          }
        }


        if (ssdDef.has_box_coder() == true)
        {
          if (ssdDef.box_coder().has_faster_rcnn_box_coder() == true)
          {
            tidl_TFSSDConfig.y_scale = ssdDef.box_coder().faster_rcnn_box_coder().y_scale();
            tidl_TFSSDConfig.x_scale = ssdDef.box_coder().faster_rcnn_box_coder().x_scale();
            tidl_TFSSDConfig.height_scale = ssdDef.box_coder().faster_rcnn_box_coder().height_scale();
            tidl_TFSSDConfig.width_scale = ssdDef.box_coder().faster_rcnn_box_coder().width_scale();
          }
          else if (ssdDef.box_coder().has_keypoint_box_coder() == true)
          {
            tidl_TFSSDConfig.y_scale = ssdDef.box_coder().keypoint_box_coder().y_scale();
            tidl_TFSSDConfig.x_scale = ssdDef.box_coder().keypoint_box_coder().x_scale();
            tidl_TFSSDConfig.height_scale = ssdDef.box_coder().keypoint_box_coder().height_scale();
            tidl_TFSSDConfig.width_scale = ssdDef.box_coder().keypoint_box_coder().width_scale();
            tidl_TFSSDConfig.num_keypoints = ssdDef.box_coder().keypoint_box_coder().num_keypoints();
          }
          else
          {
            printf(" Unsuported Box Type\n");
            return;
          }
        }
        else
        {
          printf(" Coud not find faster_rcnn_box_coder, Assuming faster_rcnn_box_coder with default Parameters\n");
        }

        if (ssdDef.has_anchor_generator() == true)
        {
          if (ssdDef.anchor_generator().has_ssd_anchor_generator() == true)
          {
            tidl_TFSSDConfig.num_layers = ssdDef.anchor_generator().ssd_anchor_generator().num_layers();
            for (j = 0; j < tidl_TFSSDConfig.num_layers; j++)
            {
              tidl_TFSSDConfig.aspect_ratios[j] = -1;
              tidl_TFSSDConfig.height_stride[j] = -1;
              tidl_TFSSDConfig.width_stride[j] = -1;
              tidl_TFSSDConfig.height_offset[j] = -1;
              tidl_TFSSDConfig.width_offset[j] = -1;
            }

            for (j = 0; j < tidl_TFSSDConfig.num_layers; j++)
            {
              tidl_TFSSDConfig.scales[j] = ssdDef.anchor_generator().ssd_anchor_generator().min_scale() +
                (ssdDef.anchor_generator().ssd_anchor_generator().max_scale()
                  - ssdDef.anchor_generator().ssd_anchor_generator().min_scale())
                * j / (tidl_TFSSDConfig.num_layers - 1);
            }
            for (j = 0; j < ssdDef.anchor_generator().ssd_anchor_generator().scales_size(); j++)
            {
              tidl_TFSSDConfig.scales[j] = ssdDef.anchor_generator().ssd_anchor_generator().scales(j);
            }
            tidl_TFSSDConfig.scales[tidl_TFSSDConfig.num_layers] = 1.0;
            tidl_TFSSDConfig.interpolated_scale_aspect_ratio =
              ssdDef.anchor_generator().ssd_anchor_generator().interpolated_scale_aspect_ratio();

            if (ssdDef.anchor_generator().ssd_anchor_generator().reduce_boxes_in_lowest_layer())
              tidl_TFSSDConfig.reduce_boxes_in_lowest_layer = 1;
            else
              tidl_TFSSDConfig.reduce_boxes_in_lowest_layer = 0;
            tidl_TFSSDConfig.base_anchor_height = ssdDef.anchor_generator().ssd_anchor_generator().base_anchor_height();
            tidl_TFSSDConfig.base_anchor_width = ssdDef.anchor_generator().ssd_anchor_generator().base_anchor_width();
            tidl_TFSSDConfig.num_aspect_ratios = ssdDef.anchor_generator().ssd_anchor_generator().aspect_ratios_size();
            for (j = 0; j < tidl_TFSSDConfig.num_aspect_ratios; j++)
            {
              tidl_TFSSDConfig.aspect_ratios[j] = ssdDef.anchor_generator().ssd_anchor_generator().aspect_ratios(j);
            }

            for (j = 0; j < ssdDef.anchor_generator().ssd_anchor_generator().height_stride_size(); j++)
            {
              printf("Only Default height_stride is suported  \n");
              return;
              tidl_TFSSDConfig.height_stride[j] = ssdDef.anchor_generator().ssd_anchor_generator().height_stride(j);
            }
            for (j = 0; j < ssdDef.anchor_generator().ssd_anchor_generator().width_stride_size(); j++)
            {
              printf("Only Default width_stride is suported  \n");
              return;
              tidl_TFSSDConfig.width_stride[j] = ssdDef.anchor_generator().ssd_anchor_generator().width_stride(j);
            }
            for (j = 0; j < ssdDef.anchor_generator().ssd_anchor_generator().height_offset_size(); j++)
            {
              printf("Only Default height_offset is suported  \n");
              return;
              tidl_TFSSDConfig.height_offset[j] = ssdDef.anchor_generator().ssd_anchor_generator().height_offset(j);
            }
            for (j = 0; j < ssdDef.anchor_generator().ssd_anchor_generator().width_offset_size(); j++)
            {
              printf("Only Default width_offset is suported  \n");
              return;
              tidl_TFSSDConfig.width_offset[j] = ssdDef.anchor_generator().ssd_anchor_generator().width_offset(j);
            }
          }
          else
          {
            printf(" Unsuported anchor_generato Type\n");
            return;
          }
        }
        else
        {
          printf(" Coud not find anchor_generator\n");
          return;
        }

        if (ssdDef.has_post_processing() == true)
        {
          if (ssdDef.post_processing().has_batch_non_max_suppression() == true)
          {
            tidl_TFSSDConfig.score_threshold = ssdDef.post_processing().batch_non_max_suppression().score_threshold();
            tidl_TFSSDConfig.iou_threshold = ssdDef.post_processing().batch_non_max_suppression().iou_threshold();
            tidl_TFSSDConfig.max_detections_per_class = ssdDef.post_processing().batch_non_max_suppression().max_detections_per_class();
            tidl_TFSSDConfig.max_total_detections = ssdDef.post_processing().batch_non_max_suppression().max_total_detections();
            if (ssdDef.post_processing().batch_non_max_suppression().use_static_shapes())
              printf("WARNING : use_static_shapes is Not suported Ignoring\n");
          }
          else
          {
            printf(" Coud not find batch_non_max_suppression Params\n");
            return;
          }
          if ((ssdDef.post_processing().score_converter() != TIDL_scoreConverterSIGMOID) &&
              (ssdDef.post_processing().score_converter() != TIDL_scoreConverterSOFTMAX)
             )
          {
            printf("Only Sigmoid or softmax is suported for score_converter \n");
            return;
          }
          else
          {
            tidl_TFSSDConfig.score_converter = ssdDef.post_processing().score_converter();
          }
          if (ssdDef.post_processing().logit_scale() != 1.0)
          {
            printf("Only suported value for logit_scale is 1.0 \n");
            return;
          }

        }
        else
        {
          printf(" Coud not find has_post_processing\n");
          return;
        }


      }
      else if (tfOdpipeLineDef.model().has_faster_rcnn() == true)
      {
        const FasterRcnn * fasterRcnnDef = &tfOdpipeLineDef.model().faster_rcnn();
        tf_metaArch_faster_rcnn_import(params, fasterRcnnDef,  numTFMetaLayers);
      }

    }
    printf("num_classes : %d\n", tidl_TFSSDConfig.num_classes);
    printf("y_scale : %f\n", tidl_TFSSDConfig.y_scale);
    printf("x_scale : %f\n", tidl_TFSSDConfig.x_scale);
    printf("w_scale : %f\n", tidl_TFSSDConfig.height_scale);
    printf("h_scale : %f\n", tidl_TFSSDConfig.width_scale);
    printf("num_keypoints : %f\n", tidl_TFSSDConfig.num_keypoints);
    printf("score_threshold : %f\n", tidl_TFSSDConfig.score_threshold);
    printf("iou_threshold : %f\n", tidl_TFSSDConfig.iou_threshold);
    printf("max_detections_per_class : %d\n", tidl_TFSSDConfig.max_detections_per_class);
    printf("max_total_detections : %d\n", tidl_TFSSDConfig.max_total_detections);
    printf("%12s, %12s, %12s, %12s, %12s\n", "scales", "height_stride", "width_stride", "height_offset", "width_offset");
    for (j = 0; j < tidl_TFSSDConfig.num_layers; j++)
    {
      printf("%12.7f, %12.7f, %12.7f, %12.7f, %12.7f\n", tidl_TFSSDConfig.scales[j], tidl_TFSSDConfig.height_stride[j], tidl_TFSSDConfig.width_stride[j], tidl_TFSSDConfig.height_offset[j], tidl_TFSSDConfig.width_offset[j]);
    }
    printf("%12s\n", "aspect_ratios");
    for (j = 0; j < tidl_TFSSDConfig.num_aspect_ratios; j++)
    {
      printf("%12.7f\n", tidl_TFSSDConfig.aspect_ratios[j]);
    }
  }

}
