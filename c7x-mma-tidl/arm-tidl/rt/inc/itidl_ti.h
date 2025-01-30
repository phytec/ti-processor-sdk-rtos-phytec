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
@file      itidl_ti.h
brief      This file defines the ivision interface for TI deep learning library. \n
           This interface is targeted for users integrating with middleware framework like openVX.

@version 0.1 Oct 2016 : Initial Code
@version 0.5 Jun 2018 : Cleaned up
@version 1.0 Sep 2019 : Updated
*/

/** @ingroup    iTI_DL */
/*@{*/
#ifndef ITIDL_H_
#define ITIDL_H_

#define USE_IVISION (1)

#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <float.h>

#ifdef  TIDL_IMPORT_TOOL
#define USE_IVISION (0)
#endif

#if USE_IVISION
#include <ivision.h>



/**
* @brief The unique const function table for the TI deep learning algorithm.
*/
extern const IVISION_Fxns TIDL_VISION_FXNS;


#endif
#ifdef __cplusplus
extern "C" {
#endif

#define TIDL_NUM_MAX_LAYERS     ((int32_t) 1536)
#define TIDL_NUM_IN_BUFS        ((int32_t) 24)
#define TIDL_NUM_OUT_BUFS       ((int32_t) 16)
#define TIDL_STRING_SIZE        ((int32_t) 512)
#define TIDL_MAX_DATA_BUFS      ((int32_t) 1536)
#define TIDL_MAX_ALG_IN_BUFS    ((int32_t) 32)
#define TIDL_MAX_ALG_OUT_BUFS   ((int32_t) 128)
#define TIDL_MAX_NUM_GROUPS     ((int32_t) 64)
#define TIDL_MAX_PRIORITY_LEVEL    ((int32_t) 8)
#define TIDL_MAX_OBJECTS_PER_LEVEL ((int32_t) 16)

#if defined (SOC_J784S4)
  #define TIDL_MAX_NUM_CORES      (4)
#endif

#if defined (SOC_J722S)
  #define TIDL_MAX_NUM_CORES      (2)
#endif

#if defined (SOC_AM62A) || defined (SOC_J721E) || defined (SOC_J721S2)
  #define TIDL_MAX_NUM_CORES      (1)
#endif

/* Typedef for 32 bit float */
typedef float float32_tidl;
/* Typedef for 64 bit float */
typedef double float64_tidl;

/**
 *  \brief Call back function pointer to lock interrupts
 *
 *  \return oldIntState : returns interrupt state
 */
typedef int32_t (*TIDL_Lock_t) (void) ;
/**
 *  \brief Call back function pointer to unlock interrupts
 *
 *  \param  oldIntState : previous interrupt state
 *
 *  \return None
 */
typedef void (*TIDL_Unlock_t) (int32_t);

/**
 *  \anchor eTIDL_LayerType
 *  \name   TIDL Layer Type
 *
 *  This group defines the different types of layers used in deep learning algorithms
 *
 *  @{
 */
#define TIDL_DataLayer                 ((int32_t) 0)
#define TIDL_ConvolutionLayer          ((int32_t) 1)
#define TIDL_PoolingLayer              ((int32_t) 2)
#define TIDL_ReLULayer                 ((int32_t) 3)
#define TIDL_PReLULayer                ((int32_t) 4)
#define TIDL_EltWiseLayer              ((int32_t) 5)
#define TIDL_InnerProductLayer         ((int32_t) 6)
#define TIDL_SoftMaxLayer              ((int32_t) 7)
#define TIDL_BatchNormLayer            ((int32_t) 8)
#define TIDL_BiasLayer                 ((int32_t) 9)
#define TIDL_ScaleLayer                ((int32_t) 10)
#define TIDL_Deconv2DLayer             ((int32_t) 11)
#define TIDL_ConcatLayer               ((int32_t) 12)
#define TIDL_SplitLayer                ((int32_t) 13)
#define TIDL_SliceLayer                ((int32_t) 14)
#define TIDL_CropLayer                 ((int32_t) 15)
#define TIDL_FlattenLayer              ((int32_t) 16)
#define TIDL_DropOutLayer              ((int32_t) 17)
#define TIDL_ArgMaxLayer               ((int32_t) 18)
#define TIDL_DetectionOutputLayer      ((int32_t) 19)
#define TIDL_ShuffleChannelLayer       ((int32_t) 20)
#define TIDL_ResizeLayer               ((int32_t) 21)
#define TIDL_RoiPoolingLayer           ((int32_t) 22)
#define TIDL_OdPostProcessingLayer     ((int32_t) 23)
#define TIDL_DepthToSpaceLayer         ((int32_t) 24)
#define TIDL_SigmoidLayer              ((int32_t) 25)
#define TIDL_PadLayer                  ((int32_t) 26)
#define TIDL_ColorConversionLayer      ((int32_t) 27)
#define TIDL_OdOutputReformatLayer     ((int32_t) 28)
#define TIDL_DataConvertLayer          ((int32_t) 29)
#define TIDL_CustomLayer               ((int32_t) 30)
#define TIDL_BatchReshapeLayer         ((int32_t) 31)
#define TIDL_ReduceLayer               ((int32_t) 32)
#define TIDL_ScatterElementsLayer      ((int32_t) 33)
#define TIDL_SqueezeLayer              ((int32_t) 34)
#define TIDL_TanhLayer                 ((int32_t) 35)
#define TIDL_HardSigmoidLayer          ((int32_t) 36)
#define TIDL_ELULayer                  ((int32_t) 37)
#define TIDL_ReshapeLayer              ((int32_t) 38)
#define TIDL_ConstDataLayer            ((int32_t) 39)
#define TIDL_GatherLayer               ((int32_t) 40)
#define TIDL_TransposeLayer            ((int32_t) 41)
#define TIDL_LayerNormLayer            ((int32_t) 42)
#define TIDL_UnsupportedLayer          ((int32_t) 43)
/* @} */

/**
 *  \anchor eTIDL_LayerKernelType
 *  \name   TIDL Layer Kernel Type
 *
 *  This group defines the particular kernel to execute during inference
 *
 *  @{
 */
#define TIDL_HighPrecisionKernel        ((int32_t) 0)
#define TIDL_HighThroughputKernel       ((int32_t) 1)
/* @} */

/**
 *  \anchor eTIDL_ElementType
 *  \name   TIDL Element Type
 *
 *  This group defines the different types of element type used by TIDL library
 *
 *  @{
 */
#define TIDL_UnsignedChar          ((int32_t) 0)
#define TIDL_SignedChar            ((int32_t) 1)
#define TIDL_UnsignedShort         ((int32_t) 2)
#define TIDL_SignedShort           ((int32_t) 3)
#define TIDL_UnsignedWord          ((int32_t) 4)
#define TIDL_SignedWord            ((int32_t) 5)
#define TIDL_SinglePrecFloat       ((int32_t) 6)
#define TIDL_UnsignedDoubleWord    ((int32_t) 7)
#define TIDL_SignedDoubleWord      ((int32_t) 8)
/* @} */

/**
 *  \anchor eTIDL_TensorLayout
 *  \name   Tenosr Layout Type
 *
 *  This group defines the indices Tenosr Layout Type suported by TIDL library
 *  TIDL_LT_NCHW --- Channels first, (Number_of_ROIs, Number_Of_Channels, Height , Width)- example (1,3,360,720 )
 *  TIDL_LT_NHWC --- Channels last,  (Number_of_ROIs, Height , Width, Number_Of_Channels)- example (1,360,720,3 )
 *
 *  @{
 */
#define TIDL_LT_NCHW             ((uint32_t) 0)
#define TIDL_LT_NHWC             ((uint32_t) 1)
#define TIDL_LT_MAX              (TIDLRT_LT_NHWC+1)
/* @} */


/**
 *  \anchor eTIDL_DataConvertType
 *  \name   Tenosr Data convertion layer type
 *
 *  This group defines the Tenosr Data convertion layer typesuported by TIDL library
 *
 *  @{
 */
#define TIDL_DC_TYPE_INPUT       ((uint32_t) 0)
#define TIDL_DC_TYPE_OUTPUT      ((uint32_t) 1)
#define TIDL_DC_TYPE_INTERMEDIATE      ((uint32_t) 2)
#define TIDL_DC_TYPE_MAX              (TIDL_DC_TYPE_INTERMEDIATE+1)
/* @} */


/**
 *  \anchor eTIDL_QuantStyle
 *  \name   TIDL Quantization Style
 *
 *  This group defines the different types of quantization Styles supported by TIDL library
 *
 *  @{
 */
/** \brief In this case, TIDL expects the Quantization information via interface.
 *         This is supported by caffe-jacinto training framework - Currently not supported for user */
#define TIDL_QuantStyleFixed       ((int32_t) 0)
/** \brief IDynamic quantization does not depend on training. Quantization of kernel
 *         weights happen during model import using tool */
#define TIDL_QuantStyleDynamic     ((int32_t) 1)
/** \brief Non Power of two scale for Weight and Power of two scale for Feature.
 *          Weight scale is Fixed and Feature scale is Dynamic */
#define TIDL_QuantStyleNP2Fixed    ((int32_t) 2)
/** \brief Power of two scale for both Weight and Feature.
 *          Weight scale is Fixed and Feature scale is Dynamic */
#define TIDL_QuantStyleP2Dynamic   ((int32_t) 3)
/** \brief Non Power of two scale for Weight and activation
 *          Scales for pre-quantized models come externally */
#define TIDL_QuantStyleNonP2
/** \brief Asymmetric quantization mode with quantization performed
 *          by external frameworks (Currently TF-Lite) */
#define TIDL_QuantStyleAsymNP2_TFL ((int32_t) 5)
/** \brief Asymmetric quantization mode with per channel quantization
 *          where applicable
 */
#define TIDL_QuantStyleAsymNP2      ((int32_t) 6)

/** \brief Currently not supported for user */
#define TIDL_QuantStyleCustom      ((int32_t) 10)
/* @} */

/**
 *  \anchor eTIDL_CalibOption
 *  \name   TIDL Calibration Option
 *
 *  This group defines the different types of calibration options supported by TIDL library
 *
 *  @{
 */
/** \brief This option is to enable different methods to
            find activation range for quantization. If this
            bit field is 0 then simple min and max based range
            will be used for activation quantization */
#define TIDL_CalibOptionActivationRange        ((int32_t) 1)
/** \brief This option is to enable different methods to
            find weight range for quantization. If this
            bit field is 0 then simple min and max based range
            will be used for weight quantization */
#define TIDL_CalibOptionWeightRange            ((int32_t) 2)
/** \brief This option enables advanced bias calibration*/
#define TIDL_CalibOptionBiasCalibration           ((int32_t) 4)
/** \brief This option enables per channel quantization
           for weights for depthwise separable convolution
           layers */
#define TIDL_CalibOptionPerChannelWeightQuantization ((int32_t) 8)
/** \brief This option enables mixed precision for all
           depthwise convolution layers i.e. all depthwise
           convolution layers weights will be 16 bit and
           processing will happen in 16 bit with final
           weights in 8 bits*/
#define TIDL_CalibOptionMixedPrecisionDepthwise ((int32_t) 16)
/** \brief This option enables mixed precision ( 16 bit weights) for
           the first convolution layer after the intput data layer */
#define TIDL_CalibOptionMixedPrecisionInput ((int32_t) 32)

/** \brief This option enables Bias Range clipping  to avoid weight scale saturation */
#define TIDL_CalibOptionBiasRange ((int32_t) 64)

/* @} */

/**
 *  \anchor eTIDL_ActivationRangeMethod
 *  \name   TIDL Activation Range Method
 *
 *  This group defines the various supported algos for activation range collection
 *
 *  @{
 */
/** \brief This option uses histogram to find the
           minimum and maximum activation range of each layer.
           Here Histogram is reset after each frame*/
#define TIDL_ActivationRangeMethodHistogram           ((int32_t) 0)

/** \brief This option uses histogram to find the
           minimum and maximum activation range. Here
           the histogram is computed across all the frames
           used for calibration*/
#define TIDL_ActivationRangeMethodGlobalHistogram           ((int32_t) 1)


/* @} */

/**
 *  \anchor eTIDL_WeightRangeMethod
 *  \name   TIDL Weight Range Method
 *
 *  This group defines the various supported algos for weight range collection
 *
 *  @{
 */
/** \brief This option uses histogram to find the
           minimum and maximum weight range*/
#define TIDL_WeightRangeMethodHistogram        ((int32_t) 0)
/** \brief This option uses median to find the
           minimum and maximum weight range*/
#define TIDL_WeightRangeMethodMedian        ((int32_t) 1)
/* @} */


/**
 *  \anchor eTIDL_OptimiseExtMem
 *  \name   TIDL Memory Optimization Method
 *
 *  This group defines the different types of optimizations supported by TIDL library
 *
 *  @{
 */
/** \brief In this case, no optimization in output memory, output buffer will not re-use
 *         memory from previous layers, so algorithm uses more external memory.
 *         This is Currently used for debugging */
#define TIDL_OptimiseExtMemL0      ((int32_t) 0)
/** \brief In this case, output buffer will re-use memory from previous layers when
 *         they have same width and height, so algorithm uses less external memory
 *         compare to L0 case */
#define TIDL_OptimiseExtMemL1      ((int32_t) 1)
/** \brief In this case, output buffer will re-use memory from previous layers when
 *         they have different width and height, so algorithm uses less external memory
 *         compare to L1 case */
#define TIDL_OptimiseExtMemL2      ((int32_t) 2)
/* @} */

/**
 *  \anchor eTIDL_KernelType
 *  \name   TIDL Kernel Type
 *
 *  This group defines the different types of optimizations in kernel types supported by TIDL
 *
 *  @{
 */
/** \brief In this case, only non zero coefficients are considered for convolution */
#define TIDL_Sparse                ((int32_t) 0)
/** \brief In this case, all coefficients are considered for convolution */
#define TIDL_Dense                 ((int32_t) 1)
/* @} */

/**
 *  \anchor eTIDL_StrideOffsetMethod
 *  \name   TIDL Stride Offset Method
 *
 *  This group defines the different logic to choose offset for kernels/windows with
 *  stride greater than 1
 *
 *  @{
 */
#define TIDL_StrideOffsetTopLeft   ((int32_t) 0)
#define TIDL_StrideOffsetCenter    ((int32_t) 1)
#define TIDL_StrideOffsetMax       ((int32_t) 2)
/* @} */

/**
 *  \anchor eTIDL_PoolType
 *  \name   TIDL Pooling Type
 *
 *  This group defines the different types of spatial pooling supported by TIDL library
 *
 *  @{
 */
#define TIDL_MaxPooling            ((int32_t) 0)
#define TIDL_AveragePooling        ((int32_t) 1)
/* @} */


/**
 *  \anchor eTIDL_ActType
 *  \name   TIDL Activation Type
 *
 *  This group defines the different activation types supported by TIDL library
 *
 *  @{
 */
#define TIDL_NoAct                 ((int32_t) 0)
#define TIDL_RelU                  ((int32_t) 1)
#define TIDL_PRelU                 ((int32_t) 2)
#define TIDL_RelU6                 ((int32_t) 3)
#define TIDL_Clip                  ((int32_t) 4)
#define TIDL_Sigmoid               ((int32_t) 5)
#define TIDL_Tanh                  ((int32_t) 6)
#define TIDL_HardSigmoid           ((int32_t) 7)
#define TIDL_ELU                   ((int32_t) 8)
#define TIDL_GELU                  ((int32_t) 9)
/* @} */

/**
 *  \anchor eTIDL_EltWiseType
 *  \name   TIDL Element Wise Type
 *
 *  This group defines the different types of eltWise layer operations supported by TIDL library
 *
 *  @{
 */
#define TIDL_EltWiseProduct        ((int32_t) 0)
#define TIDL_EltWiseSum            ((int32_t) 1)
#define TIDL_EltWiseMax            ((int32_t) 2)
/* @} */

/**
 *  \anchor eTIDL_DataDimIndex
 *  \name   TIDL Data Dimension Index
 *
 *  This group defines the indices of dimension array of layer data buffer in TIDL library
 *
 *  @{
 */
#define TIDL_DIM_BATCH             ((int32_t) 0)
#define TIDL_DIM_DIM1              ((int32_t) 1)
#define TIDL_DIM_DIM2              ((int32_t) 2)
#define TIDL_DIM_NUMCH             ((int32_t) 3)
#define TIDL_DIM_HEIGHT            ((int32_t) 4)
#define TIDL_DIM_WIDTH             ((int32_t) 5)
#define TIDL_DIM_MAX               ((int32_t) 6)
/* @} */


/**
 *  \anchor eTIDL_tensorType
 *  \name   TIDL Data Tensor type
 *
 *  This field indicates whether the tensor is symmetric or asymmetric
 *
 *  @{
 */
#define TIDL_SYMMETRIC_TENSOR  (0U)
#define TIDL_ASYMMETRIC_TENSOR (1U)
/* @} */

/**
 *  \anchor eTIDL_PitchDimIndex
 *  \name   TIDL Pitch Dimension Index
 *
 *  This group defines the indices of pitch array of layer data buffer in TIDL library
 *
 *  @{
 */
#define TIDL_ROI_PITCH             ((uint32_t) 0)
#define TIDL_DIM1_PITCH            ((uint32_t) 1)
#define TIDL_DIM2_PITCH            ((uint32_t) 2)
#define TIDL_CHANNEL_PITCH         ((uint32_t) 3)
#define TIDL_LINE_PITCH            ((uint32_t) 4)
#define TIDL_PITCH_MAX             ((uint32_t) (TIDL_DIM_MAX - 1))
/* @} */

/**
 *  \anchor eTIDL_ErrorType
 *  \name   TIDL Error Type
 *
 *  This group defines error numbers that have to be set when layer parameters deviate from
 *  expected range of values.
 *
 *  @{
 */
/* Convolution layer error types */
#define TIDL_E_CONVOLUTION                         ((int32_t) -1000)
#define TIDL_E_CONV_INVALID_INPUT_WIDTH            ((int32_t) (TIDL_E_CONVOLUTION - 0))
#define TIDL_E_CONV_INVALID_INPUT_HEIGHT           ((int32_t) (TIDL_E_CONVOLUTION - 1))
#define TIDL_E_CONV_INVALID_OUTPUT_WIDTH           ((int32_t) (TIDL_E_CONVOLUTION - 2))
#define TIDL_E_CONV_INVALID_OUTPUT_HEIGHT          ((int32_t) (TIDL_E_CONVOLUTION - 3))
#define TIDL_E_CONV_INVALID_NUM_IN_CHANNELS        ((int32_t) (TIDL_E_CONVOLUTION - 4))
#define TIDL_E_CONV_INVALID_NUM_OUT_CHANNELS       ((int32_t) (TIDL_E_CONVOLUTION - 5))
#define TIDL_E_CONV_INVALID_KER_WIDTH              ((int32_t) (TIDL_E_CONVOLUTION - 6))
#define TIDL_E_CONV_INVALID_KER_HEIGHT             ((int32_t) (TIDL_E_CONVOLUTION - 7))
#define TIDL_E_CONV_INVALID_KER_TYPE               ((int32_t) (TIDL_E_CONVOLUTION - 8))
#define TIDL_E_CONV_INVALID_STRIDE_WIDTH           ((int32_t) (TIDL_E_CONVOLUTION - 9))
#define TIDL_E_CONV_INVALID_STRIDE_HEIGHT          ((int32_t) (TIDL_E_CONVOLUTION - 10))
#define TIDL_E_CONV_NEGATIVE_OUTPUT_SHIFT          ((int32_t) (TIDL_E_CONVOLUTION - 11))
/* Convolution pooling error types */
#define TIDL_E_CONV_POOL_INVALID_POOL_TYPE         ((int32_t) (TIDL_E_CONVOLUTION - 12))
#define TIDL_E_CONV_POOL_INVALID_KER_WIDTH         ((int32_t) (TIDL_E_CONVOLUTION - 13))
#define TIDL_E_CONV_POOL_INVALID_KER_HEIGHT        ((int32_t) (TIDL_E_CONVOLUTION - 14))
#define TIDL_E_CONV_POOL_INVALID_STRIDE_WIDTH      ((int32_t) (TIDL_E_CONVOLUTION - 15))
#define TIDL_E_CONV_POOL_INVALID_STRIDE_HEIGHT     ((int32_t) (TIDL_E_CONVOLUTION - 16))
/* EltWise layer error types */
#define TIDL_E_ELTWISE                             ((int32_t) -1020)
#define TIDL_E_ELTWISE_INVALID_INPUT_WIDTH         ((int32_t) (TIDL_E_ELTWISE - 0))
#define TIDL_E_ELTWISE_INVALID_INPUT_HEIGHT        ((int32_t) (TIDL_E_ELTWISE - 1))
#define TIDL_E_ELTWISE_INVALID_OUTPUT_WIDTH        ((int32_t) (TIDL_E_ELTWISE - 2))
#define TIDL_E_ELTWISE_INVALID_OUTPUT_HEIGHT       ((int32_t) (TIDL_E_ELTWISE - 3))
#define TIDL_E_ELTWISE_INVALID_ELTWISE_TYPE        ((int32_t) (TIDL_E_ELTWISE - 4))
#define TIDL_E_ELTWISE_INVALID_NUM_CHANNELS        ((int32_t) (TIDL_E_ELTWISE - 5))
/* Pooling error types */
#define TIDL_E_POOLING                             ((int32_t) -1030)
#define TIDL_E_POOL_INVALID_INPUT_WIDTH            ((int32_t) (TIDL_E_POOLING - 0))
#define TIDL_E_POOL_INVALID_INPUT_HEIGHT           ((int32_t) (TIDL_E_POOLING - 1))
#define TIDL_E_POOL_INVALID_OUTPUT_WIDTH           ((int32_t) (TIDL_E_POOLING - 2))
#define TIDL_E_POOL_INVALID_OUTPUT_HEIGHT          ((int32_t) (TIDL_E_POOLING - 3))
#define TIDL_E_POOL_INVALID_POOL_TYPE              ((int32_t) (TIDL_E_POOLING - 4))
#define TIDL_E_POOL_INVALID_NUM_CHANNELS           ((int32_t) (TIDL_E_POOLING - 5))
#define TIDL_E_POOL_INVALID_KER_WIDTH              ((int32_t) (TIDL_E_POOLING - 6))
#define TIDL_E_POOL_INVALID_KER_HEIGHT             ((int32_t) (TIDL_E_POOLING - 7))
#define TIDL_E_POOL_INVALID_STRIDE_WIDTH           ((int32_t) (TIDL_E_POOLING - 8))
#define TIDL_E_POOL_INVALID_STRIDE_HEIGHT          ((int32_t) (TIDL_E_POOLING - 9))
/* Inner product error types */
#define TIDL_E_INNER_PRODUCT                       ((int32_t) -1040)
#define TIDL_E_IP_INVALID_NUM_IN_NODES             ((int32_t) (TIDL_E_INNER_PRODUCT - 0))
#define TIDL_E_IP_INVALID_NUM_OUT_NODES            ((int32_t) (TIDL_E_INNER_PRODUCT - 1))
#define TIDL_E_IP_NEGATIVE_OUTPUT_SHIFT            ((int32_t) (TIDL_E_INNER_PRODUCT - 2))
/* Argmax error types */
#define TIDL_E_ARGMAX                              ((int32_t) -1050)
#define TIDL_E_ARGMAX_INVALID_NUM_CHANNELS         ((int32_t) (TIDL_E_ARGMAX - 0))
/* Bias error types */
#define TIDL_E_BN                                  ((int32_t) -1060)
#define TIDL_E_BN_INVALID_INPUT_WIDTH              ((int32_t) (TIDL_E_BN - 0))
#define TIDL_E_BN_INVALID_INPUT_HEIGHT             ((int32_t) (TIDL_E_BN - 1))
#define TIDL_E_BN_INVALID_OUTPUT_WIDTH             ((int32_t) (TIDL_E_BN - 2))
#define TIDL_E_BN_INVALID_OUTPUT_HEIGHT            ((int32_t) (TIDL_E_BN - 3))
#define TIDL_E_BN_INVALID_NUM_CHANNELS             ((int32_t) (TIDL_E_BN - 4))
#define TIDL_E_BN_INVALID_ENABLE_RELU              ((int32_t) (TIDL_E_BN - 5))
#define TIDL_E_BN_NEGATIVE_OUTPUT_SHIFT            ((int32_t) (TIDL_E_BN - 6))
/* Crop layer error types */
#define TIDL_E_CROP                                ((int32_t) -1070)
#define TIDL_E_CROP_INVALID_INPUT_WIDTH            ((int32_t) (TIDL_E_CROP - 0))
#define TIDL_E_CROP_INVALID_INPUT_HEIGHT           ((int32_t) (TIDL_E_CROP - 1))
#define TIDL_E_CROP_INVALID_OUTPUT_WIDTH           ((int32_t) (TIDL_E_CROP - 2))
#define TIDL_E_CROP_INVALID_OUTPUT_HEIGHT          ((int32_t) (TIDL_E_CROP - 3))
#define TIDL_E_CROP_INVALID_NUM_CHANNELS           ((int32_t) (TIDL_E_CROP - 4))
#define TIDL_E_CROP_INVALID_OFFSET_WIDTH           ((int32_t) (TIDL_E_CROP - 5))
#define TIDL_E_CROP_INVALID_OFFSET_HEIGHT          ((int32_t) (TIDL_E_CROP - 6))
/* Flatten layer error types */
#define TIDL_E_FLATTEN                             ((int32_t) -1080)
#define TIDL_E_FLATTEN_INVALID_INPUT_WIDTH         ((int32_t) (TIDL_E_FLATTEN - 0))
#define TIDL_E_FLATTEN_INVALID_INPUT_HEIGHT        ((int32_t) (TIDL_E_FLATTEN - 1))
#define TIDL_E_FLATTEN_INVALID_OUTPUT_WIDTH        ((int32_t) (TIDL_E_FLATTEN - 2))
#define TIDL_E_FLATTEN_INVALID_OUTPUT_HEIGHT       ((int32_t) (TIDL_E_FLATTEN - 3))
/* SoftMax error types */
#define TIDL_E_SOFTMAX                             ((int32_t) -1090)
#define TIDL_E_SOFTMAX_INVALID_NUM_CHANNELS        ((int32_t) (TIDL_E_SOFTMAX - 0))
/* ShuffleChanel error types */
#define TIDL_E_SHUFFLECHANNEL                       ((int32_t) -1100)
#define TIDL_E_SHUFFLECHANNEL_INVALID_INPUT_WIDTH   ((int32_t) (TIDL_E_SHUFFLECHANNEL - 0))
#define TIDL_E_SHUFFLECHANNEL_INVALID_INPUT_HEIGHT  ((int32_t) (TIDL_E_SHUFFLECHANNEL - 1))
#define TIDL_E_SHUFFLECHANNEL_INVALID_OUTPUT_WIDTH  ((int32_t) (TIDL_E_SHUFFLECHANNEL - 2))
#define TIDL_E_SHUFFLECHANNEL_INVALID_OUTPUT_HEIGHT ((int32_t) (TIDL_E_SHUFFLECHANNEL - 3))
#define TIDL_E_SHUFFLECHANNEL_INVALID_NUM_GROUPS    ((int32_t) (TIDL_E_SHUFFLECHANNEL - 4))
/*Multi Core Modes */
#define TIDL_NOT_MULTI_CORE                         ((int32_t) 0)
#define TIDL_MULTI_CORE                             ((int32_t) (TIDL_NOT_MULTI_CORE + 1))
#define TIDL_MULTI_CORE_CROP_JOIN                   ((int32_t) (TIDL_NOT_MULTI_CORE + 2))
#define TIDL_MULTI_CORE_CROP_SPLIT                  ((int32_t) (TIDL_NOT_MULTI_CORE + 3))
/* Layernorm error types */
#define TIDL_E_LAYERNORM                            ((int32_t) -1110)
#define TIDL_E_LAYERNORM_INSUFFICIENT_REF_SCRATCH   ((int32_t) (TIDL_E_LAYERNORM - 0))
#define TIDL_E_LAYERNORM_UNSUPPORTED_AXIS           ((int32_t) (TIDL_E_LAYERNORM - 1))

/* Error types common to layers */
#define TIDL_E_COMMON                              ((int32_t) -1120)
#define TIDL_E_UNSUPPORTED_LAYER                   ((int32_t) (TIDL_E_COMMON - 0))
#define TIDL_E_DATAFLOW_INFO_NULL                  ((int32_t) (TIDL_E_COMMON - 1))
#define TIDL_E_INVALID_DEBUG_TRACE_PRM             ((int32_t) (TIDL_E_COMMON - 2))
#define TIDL_E_INVALID_NET_VERSION                 ((int32_t) (TIDL_E_COMMON - 3))
#define TIDL_E_INVALID_IO_LINE_PITCH               ((int32_t) (TIDL_E_COMMON - 4))
#define TIDL_E_QUANT_STATS_NOT_AVAILABALE          ((int32_t) (TIDL_E_COMMON - 5))
#define TIDL_E_INVALID_DDR_INFO_FROM_GC            ((int32_t) (TIDL_E_COMMON - 6))
#define TIDL_E_EXCEED_PREEMPTION_LEVEL             ((int32_t) (TIDL_E_COMMON - 7))
#define TIDL_E_EXCEED_OBJECTS_PER_LEVEL            ((int32_t) (TIDL_E_COMMON - 8))

/*Device specific error codes*/
#define TIDL_E_DEV_ERROR                            ((int32_t) -1130)
#define TIDL_E_OTF_ERROR                            ((int32_t) (TIDL_E_DEV_ERROR - 0))
#define TIDL_E_ASYM_ERROR                           ((int32_t) (TIDL_E_DEV_ERROR - 1))
/* @} */

/**
 *  \anchor eSysMemScratch
 *  \name   TIDL Scratch memory Type
 *
 *  Memory records for scratch memories
 *
 *  @{
 */
#define TIDL_SYSMEM_L1_SCRATCH     ((uint32_t) 0)
#define TIDL_SYSMEM_L2_SCRATCH     ((uint32_t) 1)
#define TIDL_SYSMEM_L3_SCRATCH     ((uint32_t) 2)
#define TIDL_SYSMEM_DDR_SCRATCH    ((uint32_t) 3)
#define TIDL_SYSMEM_MAX            ((uint32_t) 4)
/* @} */

/**
 *  \anchor eTIDL_CustomLayerExecMode
 *  \name   TIDL Custom Layer Execution Model/Network
 *
 *  This group defines the different types of execution modes available for custom layer
 *
 *  @{
 */
/** \brief In this case, custom layer should calculate the min and max of buffer used for stats
 *         collection and based on that it should update the output tensor scale. */
#define TIDL_EXEC_MODE_STATS_COLLECTION       ((int32_t) 0)
/** \brief This is the case when custom layer API is called during processing */
#define TIDL_EXEC_MODE_INFER_PROCESS          ((int32_t) 2)
#define TIDL_EXEC_MODE_INFER_PROCESS_REF      ((int32_t) 3)


/**
 *  \anchor TIDL_ProfilePoints
 *  \name   TIDL Profile Points
 *
 *  Profile points for TIDL
 *
 *  @{
 */
#define TIDL_PROFILE_LAYER                      ((int32_t) 0)
#define TIDL_PROFILE_KERNEL_ONLY                ((int32_t) 1)
#define TIDL_PROFILE_CORE_LOOP                  ((int32_t) 2)
#define TIDL_PROFILE_LAYER_SETUP                ((int32_t) 3)
#define TIDL_PROFILE_PREFETCH                   ((int32_t) 4)
#define TIDL_PROFILE_DMA_PIPEUP                 ((int32_t) 5)
#define TIDL_PROFILE_LAYER_WITHOUT_PADDING      ((int32_t) 6)
#define TIDL_PROFILE_LAYER_PADDING_TRIGGER      ((int32_t) 7)
#define TIDL_PROFILE_LAYER_PADDING_WAIT         ((int32_t) 8)
#define TIDL_PROFILE_KERNEL_COPY                ((int32_t) 9)
#define TIDL_PROFILE_LAYER_SETUP_HANDLE_COPY    ((int32_t) 10)
#define TIDL_PROFILE_LAYER_DEINIT               ((int32_t) 11)
#define TIDL_PROFILE_LAST_BLOCK_CYCLES          ((int32_t) 12)
#define TIDL_PROFILE_DMA_PIPEDOWN               ((int32_t) 13)
#define TIDL_PROFILE_BACKUP                     ((int32_t) 14)
#define TIDL_PROFILE_RESTORE                    ((int32_t) 15)
#define TIDL_PROFILE_CONTEXT_CROSSCORE_COPY     ((int32_t) 16)
#define TIDL_PROFILE_MAX                        ((int32_t) 17)

/* @} */

/**
 @struct  sBuffer_t
 @brief   This structure define the parameters of data or kernel buffer
           memory in TIDL
 @param  ptr
          Address pointing to the actual buffer
 @param  offset
          offset in a large buffer pointed by the parent structure
 @param  bufSize
          Size of the buffer in bytes
*/
typedef struct
{
  void* ptr;
  int32_t bufSize;
  int32_t offset;
  int32_t reserved[1];
}sBuffer_t;

/**
 @struct  sTIDL_sysMemHandle_t
 @brief   This structure defines the system memory handles in TIDL
          There are total four handles:
          0: L1, 1:L2, 2:L3 and 3:DDR

          This is scratch memory and it can be used while implementing custom
          layer. The life of this memory is only within the context of the
          specific custom layer and shall not be assumed to be available after
          completion of processing for that custom layer

 @param   base
          Base address of system memory
 @param   size
          Size of the system memory
 @param   offset
          Offset up to which system memory is already occupied
*/
typedef struct
{
  void    *base;
  int32_t  size;
  int32_t  offset;
}sTIDL_sysMemHandle_t;

typedef sTIDL_sysMemHandle_t sTIDL_NetBuffer_t ;

/**
 @struct  sTIDL_DataParams_t
 @brief   This structure define the parameters of data or kernel buffer
          used by TIDL layers (In,Out)
*/
typedef struct {
  /** Unique Data ID for the tensor holding feature vector */
  int32_t dataId;
  /** Element type of the feature vector as defined in \ref eTIDL_ElementType*/
  int32_t elementType;
  /** Number of dimensions supported by the tensor */
  int32_t numDim;
  /** Q factor used for fixed point : NOT used now */
  int32_t dataQ;
  /** Minimum value of Accumulator in fixed point : NOT used now */
  int32_t minValue;
  /** Maximum value of Accumulator in fixed point : NOT used now */
  int32_t maxValue;
  /** Minimum value of Accumulator in floating point */
  float32_tidl   minTensorValue;
  /** Maximum value of Accumulator in floating point*/
  float32_tidl   maxTensorValue;
  /** Scale value applied on the tensor for final fixed point*/
  float32_tidl   tensorScale;
  /** Zero point value used in conjunction with the tensor scale to quantize/dequantize tensors*/
  int32_t  tensorZeroPoint;
  /** Is the tensor symmetric (zero point = 0) or asymmetric*/
  int32_t tensorType;
  /** Horizontal Padding */
  int32_t padW;
  /** Vertical Padding */
  int32_t padH;
  /** Padding between horizontal Batches in stitched image*/
  int32_t batchPadW;
  /** Padding between vertical Batches in the stitched image*/
  int32_t batchPadH;
  /** Number of Batches stitched in the width direction for batch processing */
  int32_t numBatchW;
  /** Number of Batches stitched in the height direction for batch processing */
  int32_t numBatchH;
  /** Rounding bit used while converting to small bit depth (8 or 16 bit) */
  int32_t roundBits;
  /** Pitch for each dimension as defined by \ref eTIDL_PitchDimIndex*/
  int32_t pitch[TIDL_DIM_MAX-1];
  /** Values of dimensions as defined by \ref eTIDL_DataDimIndex*/
  int32_t dimValues[TIDL_DIM_MAX];
}sTIDL_DataParams_t;



/**
 @struct  sTIDL_DataLayerParams_t
 @brief   This structure define the parameters of Data layer
          in TIDL
*/
typedef struct {
  /**  Number of channel in the In or Out data buffer */
  int32_t   numChannels;
  /** Q value of the data */
  int32_t   dataQ;
}sTIDL_DataLayerParams_t;

typedef struct {
  /**  Axis to perform layernorm on */
  int32_t   axis;
  /** Epsilon value for numerical stability of division */
  float32_tidl   epsilon;
}sTIDL_LayerNormParams_t;

/**
 @struct  sTIDL_GatherLayerParams_t
 @brief   This structure define the parameters of Gather layer
          in TIDL
*/
typedef struct {
  /** Axis of the Gather Operation */
  int32_t   axis;
}sTIDL_GatherLayerParams_t;


/**
 @struct  sTIDL_ShuffleLayerParams_t
 @brief   This structure define the parameters of Shuffle layer
          in TIDL
*/
typedef struct {
  /** Number of groups */
  int32_t   numGroups;
  /** Dummy / reserved */
  int32_t   resvd;
}sTIDL_ShuffleLayerParams_t;

/**
 @struct  sTIDL_DepthToSpaceParams_t
 @brief   This structure define the parameters of Depth To Space layer
          in TIDL
*/
typedef struct {
  /** This basically upsample ratio for this layer */
  int32_t   blockSize;
}sTIDL_DepthToSpaceParams_t;

/**
 @struct  sTIDL_PadLayerParams_t
 @brief   This structure define the parameters of Pad layer
          in TIDL
*/
typedef struct {
  /** Top padding to be added as part of the pad layer */
  int32_t padT;
  /** Bottom padding to be added as part of the pad layer */
  int32_t padB;
  /** Left padding to be added as part of the pad layer */
  int32_t padL;
  /** Right padding to be added as part of the pad layer */
  int32_t padR;
  /** The value of the constant to be padded to the layer */
  int32_t   padConstValue;
  /** Type of padding to be done as defined in \ref eTIDL_PadLayerPadType  */
  int32_t   padType;
  /** Offset where per channel values to be used for padding are stored.
  A value of zero indicates that this infomation is not valid */
  int32_t   perChannelPadConstTensorOffset;
  /** size of buffer pointed by above mentioned offset */
  int32_t   perChannelPadConstTensorSize ;
}sTIDL_PadLayerParams_t;

/**
 @struct  sTIDL_odOutputReformatLayerParams_t
 @brief   This structure define the parameters of the OD output reformatting layer
          in TIDL
*/
typedef struct {
  /** type of OD output as defined in \ref eTIDL_odOutputReformatType*/
  int32_t layerType;
  int32_t inWidthOdNetwork;
  int32_t inHeightOdNetwork;
}sTIDL_odOutputReformatLayerParams_t;

/**
@enum    eTIDL_ResizeType
@brief   This enumerator defines the different types of element type
used by TIDL library
*/
typedef enum
{
  TIDL_ResizeNearest = 0,
  TIDL_ResizeBilinear = 1,
  TIDL_ResizeMax,
}eTIDL_ResizeType;

/**
@struct  sTIDL_ResizeLayerParams_t
@brief   This structure define the parameters of Resize layer
in TIDL
*/
typedef struct {
  /** Number of groups  @ref eTIDL_ResizeType */
  int32_t   mode;
  /** Scale ratio for each DIM */
  float32_tidl     resizeRatio[TIDL_DIM_MAX];
}sTIDL_ResizeLayerParams_t;

/**
@enum    eTIDL_RoiPoolingType
@brief   This enumerator defines the different types of ROI pooling
supported by TIDL library
*/
typedef enum
{
  TIDL_RoiPoolingFasterRCNN = 0,
  TIDL_RoiPoolingMax,
}eTIDL_RoiPoolingType;

/**
@enum    eTIDL_PadLayerPadType
@brief   This enumerator defines the different types of padding
supported by TIDL library
*/
typedef enum
{
  TIDL_PadZero = 0,
  TIDL_PadNonZeroConstant,
  TIDL_PadPerChannel,
}eTIDL_PadLayerPadType;

/**
@enum    eTIDL_odOutputReformatType
@brief   This enumerator defines the different types of output
types for the OD output reformatting layer
*/
typedef enum
{
  TIDL_DetectionBoxesTflite = 0,
  TIDL_DetectionClassesTflite,
  TIDL_DetectionScoresTflite,
  TIDL_NumDetectionsTflite,
  TIDL_DetectionBoxesSsd,
  TIDL_DetectionClassesSsd,
  TIDL_DetectionScoresSsd,
  TIDL_DetectionBoxesAndScoresMmdetection,
  TIDL_DetectionClassesMmdetection,
  TIDL_DetectionBoxesScoresLabelsYoloV5,
  TIDL_DetectionBoxesScoresLabelsTfliteEfficientdet,
  TIDL_Detection3dOd
}eTIDL_odOutputReformatType;

/**
@struct  sTIDL_RoiPoolingLayerParams_t
@brief   This structure define the parameters of ROI Pooling layer
in TIDL
*/
typedef struct {
  /** Type of the pooling  @ref eTIDL_RoiPoolingType */
  int32_t   poolingType;
  /** Base Image Width*/
  int32_t imWidth;
  /** Base Image Height*/
  int32_t imHeight;
}sTIDL_RoiPoolingLayerParams_t;


/**
@struct  sTIDL_ShuffleLayerParams_t
@brief   This structure define the parameters of Shuffle layer
in TIDL
*/
typedef struct {
  /** Num channel in the each out buffer /tensor */
  int32_t   slicePoints[TIDL_NUM_OUT_BUFS+1];
  /** Axis on which the concat is performed */
  int32_t  axis;
  /** Stride in Slice axis - Only Supported value is 1 */
  int32_t  stride;
}sTIDL_SliceLayerParams_t;

/**
 @struct  sTIDL_ActParams_t
 @brief   This structure define the parameters of ReLU activation layer
           in TIDL
*/
typedef struct {
  /** Buffer containing Slope vales for PRelU */
  int32_t   slope;
  /**  Floating point scale value of slope values for PRelU */
  float32_tidl     slopeScale;
  /**  minimum value for clip */
  float32_tidl     clipMin;
  //**  maximum value for clip */
  float32_tidl     clipMax;
  /** value indicates different types of ReLU supported \ref eTIDL_ActType*/
  int32_t   actType;
}sTIDL_ActParams_t;

/**
 @struct  sTIDL_CalibParams_t
 @brief   This structure define the parameters of ReLU activation layer
           in TIDL
*/
typedef struct {
  /** This parameter is only applicable when calibrationOption is
      TIDL_CalibOptionActivationRange. This option tells the method to be
      used for activation range collection. Refer eTIDL_ActivationRangeMethod
      for various supported methods*/
  int32_t activationRangeMethod;
  /** This parameter is only applicable when calibrationOption is
      TIDL_CalibOptionWeightRange. This option tells the method to be
      used for weights range collection. Refer eTIDL_WeightRangeMethod
      for various supported methods. It is highly recommended that this
      option is used with bias calibration otherwise it may result in
      accuracy degradation*/
  int32_t weightRangeMethod;
  /** This parameter is only applicable when activationRangeMethod is
     TIDL_ActivationRangeMethodHistogram. This is percentile of the total number
     of elements in a activation tensor which needs to be discarded from both
     side of activation distribution. If input is unsigned then this is applied
     to only one side of activation distribution. For example
     percentileActRangeShrink = 0.01, means to discard 1/10000 elements
     from both or one side of activation distribution.*/
  float32_tidl percentileActRangeShrink;
  /** This parameter is only applicable when weightRangeMethod is
     TIDL_weightRangeMethodHistogram. This is percentile of the total number
     of elements in a weight filter which needs to be discarded from both
     side of weight distribution. For example
     percentileWtRangeShrink = 0.01, means to discard 1/10000 elements
     from both or one side of weight distribution.*/
  float32_tidl percentileWtRangeShrink;
  /** This is contribution which is used to update the bias in each iteration based
     on the difference of actual mean with respect to the mean after quantization */
  float32_tidl biasCalibrationFactor;
  /** This parameter is only applicable when calibrationOption is
      TIDL_CalibOptionBiasCalibration. This is the number of iterations
      for which bias calibration will be iteratively run. */
  int32_t biasCalibrationIterations;
}sTIDL_CalibParams_t;


/**
 @struct  sTIDL_ArgMaxParams_t
 @brief   This structure define the parameters Arg max layer
           in TIDL
*/
typedef struct {
  /** Number of channels channels to be processed */
  int32_t   numChannels;
  /** Q value of the in data */
  int32_t   inDataQ;
  /** Q value expected for out data */
  int32_t   outDataQ;
}sTIDL_ArgMaxParams_t;

/**
 @struct  sTIDL_PoolingParams_t
 @brief   This structure define the parameters spatial Pooling layer
           in TIDL
*/
typedef struct {
  /** Number of channels channels to be processed */
  int32_t   numChannels;
  /** Type of the Pooling as defined in \ref eTIDL_PoolType */
  int32_t   poolingType;
  /** Kernel width */
  int32_t   kernelW;
  /**  Kernel height */
  int32_t   kernelH;
  /** Stride in horizontal direction */
  int32_t   strideW;
  /**  Stride in vertical direction */
  int32_t   strideH;
  /** Horizontal Padding requirement in number of elements */
  int32_t   padW;
  /** Vertical Padding requirement in number of elements */
  int32_t   padH;
  /** Top Padding requirement in number of elements */
  int32_t   padT;
  /** Bottom Padding requirement in number of elements */
  int32_t   padB;
  /** Left Padding requirement in number of elements */
  int32_t   padL;
  /** Right Padding requirement in number of elements */
  int32_t   padR;
  /** Q value of the in data */
  int32_t   inDataQ;
  /** Q value expected for out data */
  int32_t   outDataQ;
  /** ceil condition for caffe models */
  int32_t   useCeil;
  /** Global average pooling parameter for non-image data*/
  int32_t   avgDims;
}sTIDL_PoolingParams_t;

/**
 @struct  sTIDL_CustomParams_t
 @brief   This structure defines the parameters for custom layer in TIDL
*/
typedef struct {
  /** Type of the custom layer */
  int32_t   customLayerType;
  /** Horizontal Padding requirement in number of elements. This is the
  padding amount expected by the processing function on input*/
  int32_t padW;
  /** Vertical Padding requirement in number of elements.This is the
  padding amount expected by the processing function on input*/
  int32_t padH;
  /** Parameter specifying whether input and output memory can overlap
    * 0: Input cannot be overwritten by output
    * 1: Input can be overwritten by input
    Note : By default this will be 0
    */
  int32_t memOverlapType;
  /** Parameter specifying whether the layer should be forced to do in place operation
    * 0: Output is not forced to be in palce
    * 1: Output is forced to be in palce
    Note : By default this will be 0
    */
  int32_t forceInPlace ;
  /** Parameter to indicate whether the input and output padding of the custom
   layer can change or not. This information is used by TIDL to reomve the output
   padding if consumer layer's doesn't need it and similarly add output padding if
   required by the consumer layer's.
   Note : By default this will be 0 */
  int32_t doesLayerChangePadding;
  /** This parameter indicates whether the custom layer can fill the column padding
  area with zero as parts of its processing. As such TIDL always perform the padding
  in Y direction (top and bottom padding, marked as y in below figure) so it is advisable
  that custom layer writer doesn't worry about making the padding pixels in Y direction
  to be zero. But in X direction (Left and right padding) padding pixels can be zeroed out
  by custom layer writer if it is easy and optimal as part of kernel processing of that layer
  and if not then TIDL will perform the padding.
  Another point to note is that TIDL has only one padding in X direction and it uses
  the same padding for left and right padding. This is how the padding layout looks in
  the buffer, here "y" is the top and bottom padding region, x is the left padding and
  "-" the active region of the kernel. Note that there is no right padding in the buffer,
  so processing function is expected to use this layout for processing:

  yyyyyyyyyyyyyyyyyyyyyyyyyyyy
  y---------------------------
  x---------------------------
  x---------------------------
  x---------------------------
  x---------------------------
  x---------------------------
  xyyyyyyyyyyyyyyyyyyyyyyyyyyy
  y

  Note : By default this will be 0
  */
  int32_t doesLayerFillOutXPadding;
  /** This is a pass through variable and user shouldn't program/update it */
  int32_t rsvdPassThrough;
  /** This is a pass through variable and user shouldn't program/update it */
  int32_t rsvdPassThroughSize;
}sTIDL_CustomParams_t;

/**
 @struct  sTIDL_ConvParams_t
 @brief   This structure define the parameters Convolution Layer
           in TIDL
*/
typedef struct {
  /** Offset to the kernel parameters */
  int32_t   weights;
  /** Offset to the bias parameters  */
  int32_t   bias;
  /** Offset to weight scales       */
  int32_t   weightScales;
  /** Offset to weight zero points  */
  int32_t   weightZeroPoints;
  /** Offset to bias scales         */
  int32_t   biasScales;
  /** Offset to bias Zero points    */
  int32_t   biasZeroPoints;
  /** Offset to derived bias values*/
  int32_t   derivedBias;
  /** Offset to derived bias values*/
  int32_t   derivedScales;
  /** Offset to derived bias values*/
  int32_t   derivedShifts;
  /** Offset where per channel weight scales are stored for depthwise convolution layers.
 A value of zero indicates that this infomation is not valid (Deprecated)*/
  int32_t   perChannelWeightScaleOffset;
  /** Type of the convolution, Reserved for future use */
  int32_t   convolutionType;
  /** Number of input channels channels to be processed */
  int32_t   numInChannels;
  /** Number of output channels to be processed */
  int32_t   numOutChannels;
  /** Number of groups in the convolutions */
  int32_t   numGroups;
  /** Kernel width */
  int32_t   kernelW;
  /** Kernel height */
  int32_t   kernelH;
  /** Stride in horizontal direction */
  int32_t   strideW;
  /** Stride in vertical direction */
  int32_t   strideH;
  /** Dilation in horizontal direction */
  int32_t   dilationW;
  /** Dilation in vertical direction */
  int32_t   dilationH;
  /** Horizontal Padding requirement in number of elements */
  int32_t   padW;
  /** Vertical Padding requirement in number of elements */
  int32_t   padH;
  /** left Padding requirement in number of elements */
  int32_t   padL;
  /** Right Padding requirement in number of elements */
  int32_t   padR;
  /** Top Padding requirement in number of elements */
  int32_t   padT;
  /** Bottom Padding requirement in number of elements */
  int32_t   padB;
  /** Floating point scale on Kernel weights */
  float32_tidl     weightScale;
  /** Floating point scale on bias */
  float32_tidl     biasScale;
  /**  Q value of Kernel weights */
  int32_t   weightsQ;
  /** value of weights added for dynamic quantSytle  */
  int32_t   zeroWeightValue;
  /** Not used */
  int32_t   biasB;
  /** Q value kernel Bias */
  int32_t   biasQ;
  /** Q value expected for in data */
  int32_t   inDataQ;
  /** Q value expected for out data */
  int32_t   outDataQ;
  /** Q value intermediate output data */
  int32_t   interDataQ;
  /** Enable/Disable output bias */
  int32_t   enableBias;
  /** Enable/Disable 2x2 Spatial pooling */
  int32_t   enablePooling;
  /** Enable/Disable EltWise */
  int32_t   enableEltWise;
  /** Enable/Disable Relu for EltWise */
  int32_t   enableEWRelU;
  /** Defines the different types of optimizations  in kernel types supported by TIDL  */
  int32_t   kernelType;
  /** Enable/Disable depth to Space layer */
  int32_t   enableDepthToSpace;
  /** Used only when depth to Space layer is enabled - This is
   * derived from numOutChannels and data Params Out num channels*/
  int32_t   upscaleFactor;
  /** Used only if enablePooling is true */
  sTIDL_PoolingParams_t poolParams;
  /** PSAT Min Value */
  int32_t minPSAT;
  /** PSAT Max Value */
  int32_t maxPSAT;
  /** Quantization Style*/
  int32_t quantizationStyle;
}sTIDL_ConvParams_t;

/**
@enum    eTIDL_metaArchType
@brief   This enumerator defines the different Meta Arch types
*/
typedef enum
{
  TIDL_metaArchCaffeJacinto = 0,
  TIDL_metaArchTFSSD = 1,
  TIDL_metaArchTFFasterRcnn = 2,
  TIDL_metaArchTIDLSSD = 3,
  TIDL_metaArchTIDLYolo= 4,
  TIDL_metaArchTIDLRetinaNet= 5,
  TIDL_metaArchTIDLYoloV5 = 6,
  TIDL_metaArchTIDL3DOD= 7,
  TIDL_metaArchTIDLYoloV8 = 8,
  TIDL_metaArchTIDLCenterPoint = 9,
  TIDL_metaArchMax = 10
}eTIDL_metaArchType;

/**
@enum    eTIDL_processingType
@brief   This enumerator defines the different Meta Arch types
*/
typedef enum
{
  TIDL_processingTypeDetectionOutput = 0,
  TIDL_processingTypeOdPostProcessing = 1,
  TIDL_processingTypeMax = 3
}eTIDL_processingType;

/**
@enum    eTIDL_scoreConverterType
@brief   This enumerator defines the different Meta Arch types
*/
typedef enum
{
  TIDL_scoreConverterLINEAR = 0,
  TIDL_scoreConverterSIGMOID = 1,
  TIDL_scoreConverterSOFTMAX = 2
}eTIDL_scoreConverterType;

/**
@struct TIDL_CameraIntrinsicsParams
@brief  This structure contains the camera intrinsic information of BBox detected by OD layer
*/

typedef struct
{
 /** Camera focal length along X */
  float32_tidl  fX;
  /** Camera focal length along Y */
  float32_tidl  fY;
  /**  Principal point along X */
  float32_tidl  pX;
  /** Principal point along Y */
  float32_tidl  pY;

} sTIDL_CameraIntrinsicsParams_t;

/**
 @struct  sTIDL_DetectionLayerExtraParams_t``
 @brief   This union define the extra parameters that can be associated with a detection layer
          supported layers in TIDL
*/
/* CHECK_MISRA("-18.4")  -> Disable rule 18.4  */

typedef union {
  sTIDL_CameraIntrinsicsParams_t        cameraParams;
} sTIDL_DetectionLayerSubCodeParams_t;


/**
 @struct  sTIDL_DetectOutputParams_t
 @brief   This structure define the parameters of Detection Output Layer
           in TIDL

*/
typedef struct {
  /** Processing type to be done refer eTIDL_processingType */
   int32_t  processingType;
  /** Buffer containing the data required to form prior Bboxs */
  int32_t  priorBox;
  /** Size of the priorBox buffer required to form prior Bboxs  */
  int32_t  priorBoxSize;
  /** number of classes to be detected in the detection Output */
  int32_t  numClasses;
  /** To indicate whether or not to ignore background class */
  int32_t  backgroundLabelId;
  /** Indicates the coding type to be used for decoding Bboxs */
  int32_t  codeType;
  /** Value to indicates threshold above which objects to be
          considered for detection */
  float32_tidl  confThreshold;
  /**  Threshold Value used for finding overlap between the
          bboxs in the NMS */
  float32_tidl  nmsThreshold;
  /** Value used to update the adaptive Threshold in the NMS  */
  float32_tidl  eta;
  /** Number of top k objects to keep for each class before applying NMS */
  int32_t  topK;
  /** Number of top k objects to keep for all classes before applying NMS. Default is -1 and in that case this process of selection is disabled */
  int32_t  topKAllClasses;
  /**  Number of top k objects to Keep in the final output */
  int32_t  keepTopK;
  /** Indicate whether same size Boxes used for all classes or not,
          it is not supported in ti_dl */
  int32_t  shareLocation;
  /** Flag to indicate the variance used in decoding bboxes is
          encoded along with locations are not */
  int32_t  varianceEncoded;
  /** Number of keyPoints to be detected along with box */
  int32_t  numKeypoints;
  /** Flag to indicate whether there is an associated confidence for each key-point */
  int32_t  keypointConfidence;
  /** subCodeType to determine the type of subCodeParams */
  int32_t  subCodeType;
   /** subCodeParams as determined by subCodeType */
   sTIDL_DetectionLayerSubCodeParams_t subCodeParams;

  /** Num heads used by the Post processing layer */
  int32_t  numHeads;

  /** Num Sub heads under one head. This is useful if one head has multiple scale of prior boxes as in 3DOD.
   * In otehr cases this is always one.
  */
  int32_t  numSubHeads;

  /** Base Image Width used in 2D OD*/
  int32_t imWidth;

  /** Base Image Height usedin 2D OD*/
  int32_t imHeight;

  /** 3d data range(min-max) in x,y,z direction. Used in 3D OD*/
  float32_tidl point3dRange[3][2];

  /** voxel size in cm for x, y and z dim*/
  float32_tidl voxelSize[3];

  /** Maximum number of 3D points per voxel*/
  int32_t maxPointsPerVoxel;

  /*0 -> IDENTITY, 1 -> SIGMOID, 2 -> SOFTMAX*/
  int32_t scoreConverter;

   /** Meta Arch Type @ref eTIDL_metaArchType */
  int32_t metaArchType;

  /* 0 -> All the Data from given Anchor Box are together,
     1 -> Data from all Anchor Box are interleaved */
  int32_t dataLayout;

}sTIDL_DetectOutputParams_t;  //Layer Params


/**
@enum    eTIDL_SubCodeType
@brief   This enumerator defines the differnt types of subCodeParams
         indicated by sTIDL_DetectOutputParams_t::subCodeType
*/
typedef enum
{
  TIDL_Invalid = 0,  //sTIDL_DetectOutputParams_t.subCodeParams are not populated and used.
  TIDL_ObjectPose = 1, //sTIDL_DetectOutputParams_t.subCodeParams are used for objectPose processing.
} eTIDL_SubCodeType;


#define TIDL_OD_MAX_KEYPOINTS         (16)
#define TIDL_OD_MAX_ANCHORS_PER_HEAD  (16)

/**
@struct  sTIDL_AnchorBoxParams_t
@brief   This structure define the parameters of AnchorBox used by Detection Output Layer
in TIDL

*/
typedef struct {

  /** Num Anchors used by the Current Head */
  int32_t  numAnchors;
  /** Num Key Points to be detected, Default zero */
  int32_t  numKeyPoints;
  /** Head Width as grid size */
  int32_t headWidth;
  /** Head Height  as grid size*/
  int32_t headHeight;
  /** Normalized Stride in Horizontal (x) Dir*/
  float32_tidl strideW;
  /** Normalized Stride in Vertical (y) Dir*/
  float32_tidl strideH;
  /** Normalized Stride in Height (z) Dir*/
  float32_tidl strideL;
  /** Normalized Offset in Horizontal (x) Dir*/
  float32_tidl offsetW;
  /** Normalized Offset in Vertical (y) Dir*/
  float32_tidl offsetH;
  /** Normalized Offset in Height (z) Dir used in 3D OD*/
  float32_tidl offsetL;
  /** Normalized Offset for Dir used in 3D OD*/
  float32_tidl offsetDir;
  /** Scales used for Box decoding Y,X,H,W*/
  float32_tidl boxScales[4];
  /** Scales used for Key points decoding */
  float32_tidl kpScales[TIDL_OD_MAX_KEYPOINTS];
  /** Normalized Anchors Box Width used in 3D OD only*/
  float32_tidl boxLength[TIDL_OD_MAX_ANCHORS_PER_HEAD];
  /** Normalized Anchors Box Width used in 3D and 2D OD*/
  float32_tidl boxWidth[TIDL_OD_MAX_ANCHORS_PER_HEAD];
  /** Normalized Anchors Box height used in 3D and 2D OD*/
  float32_tidl boxHeight[TIDL_OD_MAX_ANCHORS_PER_HEAD];
  /** Normalized Anchors Box rotation angles used in 3D and 2D OD*/
  float32_tidl boxAngle[TIDL_OD_MAX_ANCHORS_PER_HEAD];

  float32_tidl * anchorInputs;

}sTIDL_AnchorBoxParams_t;

/**
 @struct  sTIDL_ConcatParams_t
 @brief   This structure define the parameters of PriorBox layer
           in TIDL
*/
typedef struct {
  /** Axis on which the concat is performed */
  int32_t  axis;
  /** Q value for output data */
  int32_t  outDataQ;
}sTIDL_ConcatParams_t;


/**
 @struct  sTIDL_BatchNormParams_t
 @brief   This structure define the parameters of Batch Norm layer
           in TIDL

*/
typedef struct {
  /** Offset for kernel parameters  */
  int32_t   weights;
  /** Offset for bias parameters  */
  int32_t   bias;
  /** Number of channels channels to be processed */
  int32_t   numChannels;
  /** Q value of Bias parameter */
  int32_t   biasQ;
  /** Q value of the in data */
  int32_t   inDataQ;
  /** Q value expected for out data */
  int32_t   outDataQ;
  /** Q value of Kernel weights */
  int32_t   weightsQ;
  /** floating point scale for weight */
  float32_tidl weightScale;
  /** floating point scale for bias */
  float32_tidl biasScale;
  /** value of weights added for dynamic quantSytle  */
  int32_t   zeroWeightValue;
}sTIDL_BatchNormParams_t;

/**
 @struct  sTIDL_BiasParams_t
 @brief   This structure define the parameters of Bias layer
           in TIDL
*/
typedef struct {
  /**  Offset for bias parameters  */
  int32_t   bias;
  /** Number of channels channels to be processed */
  int32_t   numChannels;
  /** Q value of Bias parameter */
  int32_t   biasQ;
  /** Q value of the in data */
  int32_t   inDataQ;
  /** Q value expected for out data */
  int32_t   outDataQ;
}sTIDL_BiasParams_t;

/**
 @struct  sTIDL_InnerProductParams_t
 @brief   This structure define the parameters of Inner Product
          (Fully connected) layer in TIDL
*/
typedef struct {
  /** Offset for kernel parameters */
  int32_t   weights;
  /** Offset for bias parameters */
  int32_t   bias;
  /** Offset to weight scales       */
  int32_t   weightScales;
  /** Offset to weight zero points  */
  int32_t   weightZeroPoints;
  /** Offset to bias scales         */
  int32_t   biasScales;
  /** Offset to bias Zero points    */
  int32_t   biasZeroPoints;
  /** Offset to derived bias values*/
  int32_t   derivedBias;
  /** Offset to derived bias values*/
  int32_t   derivedScales;
  /** Offset to derived bias values*/
  int32_t   derivedShifts;
  /** activation type to be used  */
  int32_t   activationType;
  /** Number of Baches of AxB matrix multiplication */
  int32_t   numBatches;
  /** Number of rows in matrix A */
  int32_t   numInRows;
  /** Number of cols in matrix A */
  int32_t   numInCols;
  /** Number of cols in matrix B */
  int32_t   numOutCols;
  /** Q value of Kernel weights */
  int32_t   weightsQ;
  /** floating point scale for weight */
  float32_tidl     weightScale;
  /** floating point scale for bias */
  float32_tidl     biasScale;
  /** value of weights added for dynamic quantSytle  */
  int32_t   zeroWeightValue;
  /** Q value kernel Bias */
  int32_t   biasQ;
  /** Q value of the in data */
  int32_t   inDataQ;
  /** Q value expected for out data */
  int32_t   outDataQ;
  /** Q value intermediate output data */
  int32_t   interDataQ;
  /** Newly added */
  int32_t   biasB;
  /** is Bias present or not*/
  int32_t   isBias;
  /** matrix A is transposed or not*/
  int32_t inputATranspose;
  /** matrix B is transposed or not*/
  int32_t inputBTranspose;
  int32_t constIdx;
}sTIDL_InnerProductParams_t;

/**
 @struct  sTIDL_EltWiseParams_t
 @brief   This structure define the parameters of Element wise layer
           in TIDL
*/
typedef struct {
  /**  Offset for bias parameters */
  int32_t bias;
  /** Number of channels channels to be processed */
  int32_t numChannels;
  /** Type of the element wise operation */
  int32_t eltWiseType;
  /** Number of input data buffers on which  EltWise operation will be done */
  int32_t numInData;
  /**  Q value kernel Bias */
  int32_t biasQ;
  /** Q value of the in data buffers */
  int32_t inDataQ[TIDL_NUM_IN_BUFS];
  /** Q value expected for out data */
  int32_t outDataQ;
  /*Asymmetric Parameters*/
  uint8_t mmaScale;
  uint8_t mmaShift;
  int32_t biasTerm;
  /*Is one tensor broadcasted*/
  int32_t isBroadcast;
}sTIDL_EltWiseParams_t;

/**
 @struct  sTIDL_SoftMaxParams_t
 @brief   This structure define the parameters Soft max layer
           in TIDL
*/
typedef struct {
  /** Number of channels channels to be processed */
  int32_t   numChannels;
  /**  Q value of the in data */
  int32_t   inDataQ;
  /** Q value expected for out data */
  int32_t   outDataQ;
  /** Axis to perform softmax on*/
  int32_t   axis;
  /** Transpose output buffer of softmax*/
  int32_t   outTranspose;
}sTIDL_SoftMaxParams_t;

/**
 @struct  sTIDL_CropParams_t
 @brief   This structure define the parameters Crop layer
           in TIDL
*/
typedef struct {
  /** Number of channels channels to be processed */
  int32_t   numChannels;
  /**  Q value of the in data */
  int32_t   inDataQ;
  /**  Q value expected for out data */
  int32_t   outDataQ;
  /** Horizontal offset */
  int32_t		offsetW;
  /** Vertical offset */
  int32_t		offsetH;
  /*crop for multic7x or normal crop*/
  int32_t   multiCoreMode;
}sTIDL_CropParams_t;


/**
 @struct  sTIDL_dataConvertParams_t
 @brief   This structure define the parameters Dat Convert layer
           in TIDL
*/
typedef struct {
   /** Layer Type as defined by \ref eTIDL_DataConvertType */
  int32_t type;
  int32_t layout;
  int32_t outLayout;
  int32_t inZeroPoint;
  int32_t outZeroPoint;
}sTIDL_dataConvertParams_t;

/**
 @struct  sTIDL_ReduceParams_t
 @brief   This structure define common parameters ReduceMax/ReduceMin layer
           in TIDL
*/
typedef struct {
  /** Axis along which max/min has to happen */
  int32_t   axis;
  /*keep the dimension or squeeze the dimension*/
  int32_t   keepDims;
  /*which operation to happen as per @ref eTIDL_inReduceOp*/
  int32_t   ops;
}sTIDL_ReduceParams_t;

typedef enum
{
  TIDL_ScatterElementsAdd = 0,
  TIDL_ScatterElementsMax,
  TIDL_ScatterElementsMin,
  TIDL_ScatterElementsNone
} eTIDL_ScatterElementsReductionType;

/**
 @struct  sTIDL_ScatterElementsParams_t
 @brief   This structure define the parameters ScatterElements layer
           in TIDL
*/
typedef struct {
  /** Axis for which scatter has to happen */
  int32_t   axis;
  /** Data Tensor Name */
  int8_t    dataTensorName[TIDL_STRING_SIZE];
  /** Indices Tensor Name */
  int8_t    indicesTensorName[TIDL_STRING_SIZE];
  /** Update Tensor Name*/
  int8_t    updateTensorName[TIDL_STRING_SIZE];
  /* ADD/MAX are presently supported. ADD output would be clipped to 8bit */
  int32_t   reduction;
}sTIDL_ScatterElementsParams_t;

/**
 @struct  sTIDL_ConstDataParams_t
 @brief   This structure define the parameters ConstData layer
           in TIDL
*/
typedef struct {
  /*Offset for data parameter*/
  int32_t offset;
  /** floating point scale for weight */
  float32_tidl weightScale;
  /** Broadcast parameter*/
  int32_t isBroadcast;
}sTIDL_ConstDataParams_t;

/**
 @struct  sTIDL_TransposeParams_t
 @brief   This structure define the parameters Transpose layer
           in TIDL
*/
typedef struct {
  /** Indicates if transpose uses dataconvert*/
  int32_t isDataConvertNeeded;
  /** Transpose permutation */
  int32_t perm[TIDL_DIM_MAX];
}sTIDL_TransposeParams_t;

/**
 @struct  sTIDL_LayerParams_t
 @brief   This union define the layer specific parameters of all the
          supported layers in TIDL
*/
/* CHECK_MISRA("-18.4")  -> Disable rule 18.4  */
typedef union {
  sTIDL_ConvParams_t                    convParams;
  sTIDL_EltWiseParams_t                 eltWiseParams;
  sTIDL_PoolingParams_t                 poolParams;
  sTIDL_InnerProductParams_t            innerProductParams;
  sTIDL_DataLayerParams_t               dataLayerParams;
  sTIDL_ArgMaxParams_t                  argMaxParams;
  sTIDL_SoftMaxParams_t                 softMaxParams;
  sTIDL_CropParams_t                    cropParams;
  sTIDL_ConcatParams_t                  concatParams;
  sTIDL_DetectOutputParams_t            detectOutParams;
  sTIDL_BiasParams_t                    biasParams;
  sTIDL_BatchNormParams_t               batchNormParams;
  sTIDL_ShuffleLayerParams_t            shuffleLayerParams ;
  sTIDL_SliceLayerParams_t              sliceParams;
  sTIDL_ResizeLayerParams_t             resizeParams;
  sTIDL_RoiPoolingLayerParams_t         roiPoolingParams;
  sTIDL_DepthToSpaceParams_t            depthToSpaceParams ;
  sTIDL_PadLayerParams_t                padLayerParams;
  sTIDL_odOutputReformatLayerParams_t   odOutputReformatLayerParams;
  sTIDL_dataConvertParams_t             dataConvertParams;
  sTIDL_CustomParams_t                  customParams;
  sTIDL_ReduceParams_t                  reduceParams;
  sTIDL_ScatterElementsParams_t         scatterElementsParams;
  sTIDL_ConstDataParams_t               constDataParams;
  sTIDL_TransposeParams_t               transposeParams;
  sTIDL_GatherLayerParams_t             gatherParams;
  sTIDL_LayerNormParams_t               layerNormParams;
}sTIDL_LayerParams_t;
/*RESET_MISRA("18.4")  -> Reset rule 18.4 */

/**
 @struct  sTIDL_Layer_t
 @brief   This structure define the common layer parameters
           in TIDL
*/
typedef struct {
  sTIDL_LayerParams_t layerParams;
   /** Feature activation type used by TIDL layer */
  sTIDL_ActParams_t    actParams;
  /** Layer Type as defined by \ref eTIDL_LayerType */
  int32_t layerType;
  /** Layer Type as defined by \ref eTIDL_LayerKernelType */
  int32_t layerKernelType;
  /** Number of input data buffers for the layer */
  int32_t numInBufs;
  /** Number of output data buffers for the layer */
  int32_t numOutBufs;
  /** Input data buffers IDs */
  int32_t inData[TIDL_NUM_IN_BUFS];
  /** output data buffers details */
  sTIDL_DataParams_t outData;
  /** Input data buffers details - Used only in the case of Custom layer for now */
  sTIDL_DataParams_t * inDataPtr;
  /** Processing core ID  */
  int32_t coreID;
  /** Group of layers in the net are processed together. This unique number
          identify the currently processing group */
  int32_t layersGroupId;
  /** Size of compute layer weight parameters in bytes */
  int32_t weightsElementSizeInBits;
  /** Offset selection method for stride. \ref eTIDL_StrideOffsetMethod */
  int32_t strideOffsetMethod;
  /* Indicates whether layer is split across multiple cores */
  int32_t multiCoreMode;
  /*To indicate wether the layer requires scratch Memory region*/
  int32_t scratchMemRequired;
}sTIDL_Layer_t;

/**
 *  \anchor eTIDL_networkBufInfo
 *  \name   eTIDL_networkBufInfo
 *
 *  TIDL Network has few information prepared during compile time and these
 *  are of variable size and at variable offset in single chunk of memory
 *  logical grouping of infomration is :
 *   (A) NETWORK STRUCTURE
 *   (B) NETWORK PARAMETERS
 *   (C) NETWORK COMPILED INFO
 *   (D) NETWORK DATAFLOW (GC) INFO
 *  Defining a data structure for offset and size of these and these IDs to
 *  get relevant inforamtion
 *  @{
 */
#define TIDL_NET_STRUCTURE_BUF ((uint32_t) 0)
#define TIDL_NET_PARAMS_BUF    ((uint32_t) 1)
#define TIDL_NET_COMPILER_BUF  ((uint32_t) 2)
#define TIDL_NET_GC_BUF        ((uint32_t) 3)
#define TIDL_NET_TOTAL_BUF     ((uint32_t) 4)
/* @} */

/* Based on last Updated Date */
#define TIDL_NET_VERSION  (0x20240401)
/**
 @struct  sTIDL_Network_t
 @brief   This structure define the parameters CNN/Deep learning net
           in TIDL
*/
typedef struct {
  /** Version Number for TIDl net format */
  int32_t netVersion;
  /** Device for which the network has been compiled*/
  int32_t deviceName;
  /** Number of layers in the network including the input and output data Layers */
  int32_t numLayers;
  /** Size of compute layer weight parameters in bytes */
  int32_t weightsElementSize;
  /**  Size of PRelU layer weight/slope parameters in bytes */
  int32_t slopeElementSize;
  /**  Size of compute layer Bias parameters in bytes */
  int32_t biasElementSize;
  /** Size of compute layer input and data buffers in bytes */
  int32_t dataElementSize;
  /** Size of compute layer intermediate data in bytes */
  int32_t interElementSize;
  /** Variable to indicate different types of quantization Styles */
  int32_t quantizationStyle;
  /** Variable to indicate different types of calibration options. Please
  refer eTIDL_CalibOption for various options available. Even though
  user can try multiple options following options are recommended :
  0 : Simple calibration ( Default)
  1 : Histogram based activation range collection
  7 : Advanced bias calibration*/
  int32_t calibrationOption;
  /** Calibration specific parameters, parameters are specific to
  calibration method and are only applicable if the corresponding
  option is set by user as part of eTIDL_CalibOption*/
  sTIDL_CalibParams_t    calibrationParams;
  /** dataFlow Info from offline Analysis */
  int32_t dataFlowInfo;
  /** Graph compiler Info offset from offline Analysis */
  int32_t graphCompilerInfoOffset;
  /** Quantization statistics availability - Updated with tidl_quant_stats_tool */
  int32_t isQuantStatsAvailable;
  /** It is the WorstCaseDelay in milliSecond for pre-emption for the given network */
  float32_tidl preEmptWorstCaseDelay;
  /** Reserved Parameters */
  int32_t reserved;
  /** Model Group Id provided by user during compiling the network*/
  int32_t modelGroupId;
  /** Inference mode - Default (single c7x)/ High throughput (batch processing using multiple cores) / Low latency (Network split across multiple cores) */
  int32_t inferenceMode;
  /** Number of cores to be used for inference - default value derived from device_config.cfg */
  int32_t numCores;
  /** Information of different buffers in the network. This instance is
   *  define for purpose of having information about a small buffer in large buffer
   * primarily intended to be used for network contained buffers */
  sTIDL_NetBuffer_t netBufInfo[TIDL_NET_TOTAL_BUF];

  /** Layer Parameter for each layer  */
  sTIDL_Layer_t TIDLLayers[TIDL_NUM_MAX_LAYERS];
}sTIDL_Network_t;

/**
@enum    eTIDL_inDataFormat
@brief   This enumerator defines the different indata formats
*/
typedef enum
{
  TIDL_inDataFormatBGRPlanar = 0,
  TIDL_inDataFormatRGBPlanar = 1,
  TIDL_inDataFormatRGBMax    = 2
}eTIDL_inDataFormat;

/**
@enum    eTIDL_inferenceMode
@brief   This enumerator defines the different inference implementation modes supported by TIDL
*/
typedef enum
{
  TIDL_inferenceModeDefault = 0,         /* Inference using single c7x/MMA core */
  TIDL_inferenceModeHighThroughput = 1,  /* Batch processing mode - Multiple frames parallely infer on multiple cores */
  TIDL_inferenceModeLowLatency    = 2    /* Single batch inference using network split on multiple cores */
}eTIDL_inferenceMode;

/**
@enum    eTIDL_inYuvFormat
@brief   This enumerator defines the different YUV formats
*/
typedef enum
{
  TIDL_inYuvFormatYuv420_NV12  = 0,
  TIDL_inYuvFormatYuv422I_YUYV = 1,
  TIDL_inYuvFormatYuv422I_UYVY = 2,
  TIDL_inYuvFormatYuvMax       = 3
}eTIDL_inYuvFormat;

/**
@enum    eTIDL_inResizeType
@brief   This enumerator defines the different indata formats
*/
typedef enum
{
  TIDL_inResizeTypeDefault   = 0,
  TIDL_inResizeTypeKeepAR    = 1,
  TIDL_inResizeTypeMax       = 2
}eTIDL_inResizeType;

/**
@enum    eTIDL_inReduceOp
@brief   This enumerator defines the different indata formats
*/
typedef enum
{
  TIDL_inReduceOpMax       = 0,
  TIDL_inReduceOpMin       = 1,
  TIDL_inReduceOpMean      = 2,
}eTIDL_inReduceOp;

/**
@struct  sTIDL_IOBufDesc_t
@brief   This structure defines the Input and output buffer descriptors
         required for a given Layer group
*/
typedef struct
{
  /** Number of Input buffer required by the Layer group  */
  int32_t numInputBuf;
  /** Number of Output buffer required by the Layer group  */
  int32_t numOutputBuf;
  /** Number of cores used for compute */
  int32_t numCores;
  /** Number of virtual cores from application point of view for a given mode of implementation */
  int32_t numVirtualCores;
  /** Number of times batch processing call needs to be invoked in multi core devices (each call can handle numVirtualCores number of batches)  */
  int32_t numSuperBatches;
  /** TIDL inference implementation mode @ref eTIDL_inferenceMode */
  int32_t inferenceMode;
  /** Input Tensor format  @ref eTIDL_inDataFormat */
  int32_t inDataFormat[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_IN_BUFS];
  /** Input Tensor resize  Type @ref eTIDL_inResizeType */
  int32_t inResizeType[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_IN_BUFS];
  /** Resize width for compressed image before crop */
  int32_t resizeWidth[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_IN_BUFS];
  /** Resize Height for compressed image before crop */
  int32_t resizeHeight[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_IN_BUFS];
  /** Feature width of each input buffer */
  int32_t inWidth[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_IN_BUFS];
  /** Feature Height of each input buffer */
  int32_t inHeight[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_IN_BUFS];
  /** Number of channels in each input buffer */
  int32_t inNumChannels[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_IN_BUFS];

  int32_t inDIM2[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_IN_BUFS];

  int32_t inDIM1[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_IN_BUFS];
  /** Minimum Channel pitch for the input tensor */
  int32_t inChannelPitch[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_IN_BUFS];
  /** Number of Batches in each input buffer */
  int32_t inNumBatches[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_IN_BUFS];
  /** Left zero padding required for each input buffer */
  int32_t inPadL[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_IN_BUFS];
  /** Top zero padding required for each input buffer */
  int32_t inPadT[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_IN_BUFS];
  /** Right zero padding required for each input buffer */
  int32_t inPadR[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_IN_BUFS];
  /** Bottom zero padding required for each input buffer */
  int32_t inPadB[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_IN_BUFS];
  /** Number of extra channels required in each input buffer*/
  int32_t inPadCh[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_IN_BUFS];
  /** Element type of each input data buffer  \ref eTIDL_ElementType */
  int32_t rawDataInElementType[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_IN_BUFS];
  /** Element type of each input buffer \ref eTIDL_ElementType */
  int32_t inElementType[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_IN_BUFS];
  /** zero Point of each input data buffer */
  int32_t inZeroPoint[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_IN_BUFS];
  /** Data Layout of each input data buffer \ref eTIDL_TensorLayout */
  int32_t inLayout[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_IN_BUFS];
  /** Data ID as per Net structure for each input buffer */
  int32_t inDataId[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_IN_BUFS];
    /** Tensor scale for input data */
  float32_tidl inTensorScale[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_IN_BUFS];
  /** In Tensor name in the original input networks */
  int8_t inDataName[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_IN_BUFS][TIDL_STRING_SIZE];
  /** Feature width of each output buffer */
  int32_t outWidth[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_OUT_BUFS];
  /** Feature Height of each output buffer */
  int32_t outHeight[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_OUT_BUFS];
  /** Feature DIM2 of each output buffer */
  int32_t outDIM2[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_OUT_BUFS];
  /** Feature DIM1 of each output buffer */
  int32_t outDIM1[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_OUT_BUFS];
  /** Number of channels in each output buffer */
  int32_t outNumChannels[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_OUT_BUFS];
  /** Channel pitch for the output tensor */
  int32_t outChannelPitch[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_OUT_BUFS];
  /** Number of Batches in each output buffer */
  int32_t outNumBatches[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_OUT_BUFS];
  /** Left zero padding required for each output buffer */
  int32_t outPadL[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_OUT_BUFS];
  /** top zero padding required for each output buffer */
  int32_t outPadT[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_OUT_BUFS];
  /** Right zero padding required for each output buffer */
  int32_t outPadR[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_OUT_BUFS];
  /** Bottom zero padding required for each output buffer */
  int32_t outPadB[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_OUT_BUFS];
  /** Number of extra channels required in each output buffer*/
  int32_t outPadCh[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_OUT_BUFS];
  /** Element type of each output buffer \ref eTIDL_ElementType */
  int32_t outElementType[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_OUT_BUFS];
  /** Data ID as per Net structure for each output buffer */
  int32_t outDataId[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_OUT_BUFS];
  /** Out Tensor name in the original input networks */
  int8_t outDataName[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_OUT_BUFS][TIDL_STRING_SIZE];
  /** TensorScale of each input data buffer */
  float32_tidl outTensorScale[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_OUT_BUFS];
  /** zero Point of each input data buffer */
  int32_t outZeroPoint[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_OUT_BUFS];
  /** Data Layout of each input data buffer \ref eTIDL_TensorLayout */
  int32_t outLayout[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_OUT_BUFS];
  /** Number of valid dimensions in the output tensor (ONNX)*/
  int32_t numValidTensorDims[TIDL_MAX_NUM_CORES * TIDL_MAX_ALG_OUT_BUFS];

} sTIDL_IOBufDesc_t;


/**
@enum    eTIDL_DetectionOutputFormat
@brief   This enumerator defines the differnt types of OD format
         indicated by TIDL_ODLayerHeaderInfo::odObjectType
*/
typedef enum
{
  TIDL_Detect2DBox = 0,  //Bit position in TIDL_ODLayerHeaderInfo.odObjectType for 2Dbox
  TIDL_Detect3DBox = 1,  //Bit position in TIDL_ODLayerHeaderInfo.odObjectType for 3Dbox
  TIDL_DetectKeyPoints = 2, //Bit position in TIDL_ODLayerHeaderInfo.odObjectType for keypoints
  TIDL_DetectObjectPose = 3, //Bit position in TIDL_ODLayerHeaderInfo.odObjectType for objectPose
} eTIDL_DetectionOutputFormat;

static inline int32_t TIDL_SetObjDetectionFormat( int32_t* odObjectType,
  int32_t odObjectFormat){
    *odObjectType = *odObjectType | ( 1 << odObjectFormat) ;
    return  *odObjectType;
  }
static inline int32_t TIDL_GetObjDetectionFormatState(int32_t odObjectType,
  int32_t odObjectFormat){
    int32_t odObjectFormatState;
    odObjectFormatState = odObjectType & ( 1 << odObjectFormat) ;
    return odObjectFormatState;
  }

/**
@struct TIDL_ODLayerHeaderInfo
@brief  This structure contains the header information of Objection detection layer

Application writer(user) should use this structure as below for visualization
or any other action
+-------------+---------------------------+--------------------------+
|  Header     |  payload0 (variable size) | payload1 (variable size) | .....
+-------------+---------------------------+--------------------------+
TIDL_ODLayerHeaderInfo *headerPtr = (TIDL_ODLayerHeaderInfo*) outputPtr;
if(TIDL_GetObjDetectionFormat(headerPtr->odObjectType, TIDL_Detect2DBox))
{
  TIDL_ODLayerObjInfo *detect2DBoxPtr  =
  (TIDL_ODLayerObjInfo*) ((uint8_t*)headerPtr + headerPtr->objInfoOffset) ;
  TIDL_ODLayerObjInfo *keyPointPtr = NULL;
  TIDL_ODLayerObjectPose *objPosePtr = NULL;
  for(objId = 0 ; objId < headerPtr->numDetObjects ; objId++)
  {
    consume 2D box with detect2DBoxPtr
    keyPointPtr = (TIDL_ODLayerKeyPoint*) detect2DBoxPtr->keyPoints ;
    if(TIDL_GetObjDetectionFormat(odObjectType, TIDL_DetectKeyPoints){
      for(keyPoint = 0; keyPoint < headerPtr->odNumKeyPoints; keyPoint++){
          consume keyPoints using keyPointPtr
          keyPointPtr++;
        }
    }
    if(TIDL_GetObjDetectionFormat(odObjectType, TIDL_DetectObjectPose){
      objPosePtr = (TIDL_ODLayerObjectPose*) (keyPointPtr + headerPtr->odNumKeyPoints);
      consume keyPoints using objPosePtr
    }
    detect2DBoxPtr = (TIDL_ODLayerObjInfo*)((uint8_t*)detect2DBoxPtr + objInfoSize);
  }
  else if(TIDL_GetObjDetectionFormat(headerPtr->odObjectType, TIDL_Detect3DBox)){
    Same as 2D except the payload style is as below:
    TIDL_3DODLayerObjInfo *detect3DBoxPtr =
    (TIDL_3DODLayerObjInfo*) ((uint8_t*)(headerPtr) + headerPtr->objInfoOffset);
  }
*/

typedef struct
{
  /** Total Number of objects detected in the current process */
  float32_tidl numDetObjects;
  /** Size of objInfo in bytes */
  float32_tidl objInfoSize;
  /** Number of key points per Obj */
  float32_tidl odNumKeyPoints;
  /** Start Offset of first Object info in bytes*/
  float32_tidl objInfoOffset;
  /** Indicates type of the object, refer @ref eTIDL_DetectionOutputFormat for all possible combinations
   */
   int32_t odObjectType;

} TIDL_ODLayerHeaderInfo;

typedef struct
{
  /** X */
  float32_tidl x;
  /** Y */
  float32_tidl y;
  /** CONF */
  float32_tidl kpt_confidence;

} TIDL_ODLayerKeyPoint;

#define TIDL_OD_MAX_KEY_POINTS     (32)


/**
@struct TIDL_ODLayerObjectPose
@brief  This structure contains the 6D object pose information of BBox detected by OD layer
*/

typedef struct
{
  /** r11 Component of Pose*/
  float32_tidl r11;
  /** r21 Component of Pose*/
  float32_tidl r21;
  /** r31 Component of Pose*/
  float32_tidl r31;
  /** r11 Component of Pose*/
  float32_tidl r12;
  /** r21 Component of Pose*/
  float32_tidl r22;
  /** r31 Component of Pose*/
  float32_tidl r32;
  /** X Component of Pose*/
  float32_tidl tx;
  /** Y Component of Pose*/
  float32_tidl ty;
  /** Z Component of Pose*/
  float32_tidl tz;

} TIDL_ODLayerObjectPose;

/**
@struct TIDL_ODLayerObjInfo
@brief  This structure contains the  each object's information detected by Objection detection layer
*/

typedef struct
{
  /** Total Number of objects detected in the current process */
  float32_tidl ObjId;
  /** Size of objInfo in bytes */
  float32_tidl label;
  /** Number of key points per Obj */
  float32_tidl score;
  /** xmin of B Box*/
  float32_tidl xmin;
  /** ymin of B Box*/
  float32_tidl ymin;
  /** xax of B Box*/
  float32_tidl xmax;
  /** ymax of B Box*/
  float32_tidl ymax;
  TIDL_ODLayerKeyPoint keyPoints[TIDL_OD_MAX_KEY_POINTS];

} TIDL_ODLayerObjInfo;

/**
@struct TIDL_3DODLayerObjInfo
@brief  This structure contains the  each object's information detected by 3D Objection detection layer
*/

typedef struct
{
  /** Total Number of objects detected in the current process */
  float32_tidl ObjId;
  /** Size of objInfo in bytes */
  float32_tidl label;
  /** Number of key points per Obj */
  float32_tidl score;
  /** X location of box in 3D world*/
  float32_tidl x;
  /** X location of box in 3D world*/
  float32_tidl y;
  /** X location of box in 3D world*/
  float32_tidl z;
  /** length of box in 3D world*/
  float32_tidl w;
  /** length of box in 3D world*/
  float32_tidl l;
  /** length of box in 3D world*/
  float32_tidl h;
  /** yaw of box in 3D world*/
  float32_tidl yaw;
} TIDL_3DODLayerObjInfo;

typedef enum
{
  TIDL_L1 = 0,
  TIDL_L2 = 1,
  TIDL_MSMC = 2,
  TIDL_DDR = 3,
  TIDL_DDR_PERSIST = 4,
  MAX_MEMSPACE_TYPES
} eTIDL_MemSpaces;

typedef enum
{
  TIDL_Physical = 0,
  TIDL_Virtual = 1,
  MAX_MEMORY_TYPES
} eTIDL_MemTypes;

#if USE_IVISION
typedef struct
{
  void * ncScratchPtr[MAX_MEMORY_TYPES][MAX_MEMSPACE_TYPES];
  void * syncBufferPtr[MAX_MEMORY_TYPES];
} TIDL_sharedBufferProperties;

/**
  @struct TIDL_controlGetArgs
  @brief  This structure contains all the parameters which
          are required by TIDL control callbacks for GET command
*/
typedef struct
{
  TIDL_sharedBufferProperties ctrlGetArgs;
} TIDL_controlGetArgs;

/**
  @struct TIDL_controlSetArgs
  @brief  This structure contains all the parameters which
          are required by TIDL control callbacks for SET command
          Contains information collected by GET APIs from all cores individually
          to be passed as aggregated information to all cores
*/
typedef struct
{
  TIDL_sharedBufferProperties ctrlSetArgs[TIDL_MAX_NUM_CORES];
} TIDL_controlSetArgs;


/**
  @struct TIDL_CreateParams
  @brief  This structure contains all the parameters which TI DL
          library at create time
*/
typedef struct
{
  /** Common parameters for all ivision based modules */
  IVISION_Params visionParams;
  /** Group ID of the each layer */
  int32_t currLayersGroupId;
  /**  Indicates Whether the input buffers are padded or Not.  */
  int32_t isInbufsPaded;
  /** Optimization level for External memory used bu TIDL */
  int32_t optimiseExtMem;
  /** Expansion factor used for range of activation while quantization */
  float32_tidl quantRangeExpansionFactor;

  /** Update factor used for range of activation while quantization */
  float32_tidl quantRangeUpdateFactor;
  /** Level for debug messages */
  int32_t traceLogLevel;
  /** Level for debug trace dumps of tensors and other data buffers */
  int32_t traceWriteLevel;
   /** Reserved control for debug */
  int32_t reservedCtrl;
   /** control for selecting different flows. Applicable only for advanced user */
  int32_t flowCtrl;
  /** Maximum Tolerated delay for TIDL pre-emption in milliSecond,
     The delay is assuming C7x processor frequency as 1GHz.
     If the clocked frequency is different then this variable
     should be scaled up/down appropriately.
     Example : to achieve pre-emption within 3 ms (wall clock delay):
     CPU clocked frequency: 1000 MHz, then maxPreEmptDelay = 3
     CPU clocked frequency: 2000 MHz, then maxPreEmptDelay = 6
     CPU clocked frequency: 500 MHz, then maxPreEmptDelay = 1.5  */
  float32_tidl maxPreEmptDelay;
  /**  trace files base Name */
  char traceBaseName[TIDL_STRING_SIZE];
  /**  UDMA driver object pointer .
  Note : All the addresses given to TIDL are assumed to be virtual addresses.
  If udma driver is initialized with virtual to physical function pointer initPrms.virtToPhyFxn
  then the same function will be used for this conversion in TIDL whereever it is needed.*/
  void * udmaDrvObj;
  /**  Pointer to TILD Model/Network */
  sTIDL_Network_t * net;

  /** Core id of the core to be used for inference - default is 0 */
  uint32_t coreId;
  /** Core id of the core from which to start processing, e.g. coreStartIdx = 2 and numCores = 2 indicates process on C7x_2 and C7x_3 */
  int32_t coreStartIdx;
  /** Target priority for Object, can be unique number but
   * has to be less than TIDL_MAX_PRIORITY_LEVEL  */
  uint32_t targetPriority ;

  /** Call back Function pointer to lock critical section/interrupts */
  TIDL_Lock_t pFxnLock;
  /** Call back Function pointer to unlock critical section/interrupts */
  TIDL_Unlock_t pFxnUnLock;
  /** Call back Function pointer to Write Log*/
  int32_t(*TIDLVprintf)(const char * format, va_list arg);
  /** Trace buffer pointer to write trace buffers when TIDL running in multiple instances */
  void *tracePtr;
  /** Call back Function pointer to Write Binary data to a file*/
  int32_t(*TIDLWriteBinToFile)(const char * fileName, void * addr, int32_t size, int32_t count);
  /** Call back Function pointer to read data from a binary file */
  int32_t(*TIDLReadBinFromFile)(const char * fileName, void * addr, int32_t size, int32_t count);
  /**
   *  \brief Custom Layer Process API.
   *
   *  \param  tidlHandle        Pointer to the tidlHandle.
   *  \param  tidlLayer         TIDL layer parameters. Refer #sTIDL_Layer_t for details.
   *  \param  inPtrs            Input buffers to custom layer.
   *  \param  outPtrs           Output buffers from custom layer.
   *  \param  params            Custom layer parameters.
   *  \param  dmaUtilsContext   Pointer to DMA Utils Handle. Current TIDL initializes 12
   *                            DMA channels and same can be used by custom layer
   *  \param  sysMems           System Memory Handle.
   *  \param  execMode          Execution mode of custom layer process API. Refer \ref
   *                            eTIDL_CustomLayerExecMode for details.
   *
   *  \return None
   */
  int32_t (*TIDL_CustomLayerProcess)(void * tidlHandle,
                                sTIDL_Layer_t *tidlLayer,
                                void *inPtrs[],
                                void *outPtrs[],
                                void *params,
                                void *dmaUtilsContext,
                                const sTIDL_sysMemHandle_t sysMems[TIDL_SYSMEM_MAX],
                                int32_t execMode);
}
TIDL_CreateParams;

/**
  @struct TIDL_InArgs
  @brief  This structure contains all the parameters which controls
          the applet at create time
*/

typedef struct
{
  /** Common inArgs for all ivision based modules */
  IVISION_InArgs iVisionInArgs;
  /** Number of input buffers  */
  int32_t numInBufs;
  /** Scale factor for each input tensor  */
  float32_tidl scale[TIDL_NUM_IN_BUFS];
  /** if enable layer level traces are generated in that particular process call */
  int32_t enableLayerPerfTraces;
} TIDL_InArgs;

/**
  @struct TIDL_outArgs
  @brief  This structure contains all the parameters which controls
          the applet at create time
*/

/**
  @struct TIDL_LayerMetaData
  @brief  This structure contains meta data for TIDL output
          ***IMP*** this structure content may change b/w releases and
          compatability may not be gauranteed, It is meta data and optional
          so applications are not encouraged to rely on this till this
          comment is removed and subject to change. This is only for debug
          purpose as of now
*/
typedef struct {
  int32_t totalOps;
  int32_t actualOps;
  int32_t layerExecId;
  uint64_t profilePoint[TIDL_PROFILE_MAX];
}TIDL_LayerMetaData;

/**
  @struct TIDL_outArgs
  @brief  This structure contains all the parameters which controls
          the applet at create time
*/

typedef struct
{
  /** Common outArgs for all ivision based modules */
  IVISION_OutArgs iVisionOutArgs;
  /** Number of output buffers  */
  int32_t numOutBufs;
  /** Scale factor for each output tensor  */
  float32_tidl scale[TIDL_NUM_OUT_BUFS];
  int32_t numLayers ;
  /* Private arguments - For debug and internal use */
  void *privArgs ;
  TIDL_LayerMetaData metaDataLayer[TIDL_NUM_MAX_LAYERS];
} TIDL_outArgs;

/*! \brief Control Commands to get/set memory pointers allocated in all c7x cores
 *         TIVX_TIDL_CMD_GET_C7X_PTRS -- Get c7x pointers for all cores from corresponding memrecs
 *         TIVX_TIDL_CMD_SET_C7X_PTRS -- Set c7x pointers for all cores as part of each core's handles
 *         TIVX_TIDL_CMD_INIT         -- TIDL init control call
 *
 *  \ingroup group_vision_function_tidl
 */
#define TIVX_TIDL_CMD_GET_C7X_PTRS     (0x10000001u)
#define TIVX_TIDL_CMD_SET_C7X_PTRS     (0x10000002u)
#define TIVX_TIDL_CMD_INIT             (0x10000003u)

/**
@struct TIDL_ODLayerKeyPoint
@brief  This structure contains the key points information of BBox detected by OD layer
*/


static inline float32_tidl TIDL_getWorstcaseDelayForPreemption(sTIDL_Network_t * net)
{
  return net->preEmptWorstCaseDelay;
}

static inline int TIDL_createParamsInit(TIDL_CreateParams * params)
{
  params->net                       = NULL;
  params->udmaDrvObj                = NULL;
  params->currLayersGroupId         = 1;
  params->isInbufsPaded             = 0;
  params->optimiseExtMem            = TIDL_OptimiseExtMemL1;
  params->quantRangeExpansionFactor = 1.0;
  params->quantRangeUpdateFactor    = -1.0;
  params->maxPreEmptDelay           = FLT_MAX;
  params->visionParams.algParams.size = sizeof(TIDL_CreateParams);
  params->visionParams.cacheWriteBack = NULL;
  params->TIDLWriteBinToFile         = NULL;
  params->TIDLReadBinFromFile        = NULL;
  params->TIDLVprintf                = NULL;
  params->pFxnLock                   = NULL;
  params->pFxnUnLock                 = NULL;
  params->traceLogLevel              = 0;
  params->traceWriteLevel            = 0;
  params->traceBaseName[0]           = '\0';
  params->tracePtr                 = NULL;
  params->reservedCtrl             = 0;
  params->coreId                   = 0;
#if defined(x86_64) || defined(HOST_EMULATION)
  params->flowCtrl                 = 1;
#else
  params->flowCtrl                 = 0;
#endif
  params->targetPriority           = 0;
  params->coreStartIdx             = 0;
  return IALG_EOK;
}

#endif

#ifdef __cplusplus
}
#endif

/*@}*/
/* iTI_DL */

#endif /*ITIDL_H_ */
/*==========================================================================*/
/*     END of the FILE                                                      */
/*==========================================================================*/
