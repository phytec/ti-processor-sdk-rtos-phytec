/*
 * Module Name        :TIDL
 *
 * Module Description :TI Deep learning Library module is TI's CNN/DNN
 *                     acceleration on EVE and DSP
 *
 * Copyright (C) 2016-2019 Texas Instruments Incorporated - http://www.ti.com/
 *
 * ALL RIGHTS RESERVED
 *
 *
 */

/**
 *  \file tidl_custom_maxpool_ixX_oxX_cn.c
 *
 *  \brief This file contains the natural C implementation for spatial max pool
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "tidsp/tidl_custom_maxpool_ixX_oxX.h"
#include "tidsp/tidl_custom_maxpool_ixX_oxX_priv.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                  Internal/Private Function Declarations                    */
/* ========================================================================== */
static int32_t TIDL_customMaxPooling_3x3_skip2_exec_cn(const uint8_t *src,
                                                        uint8_t *dst,
                                                        TIDL_bufParams3D_t *srcAddr,
                                                        TIDL_bufParams3D_t *dstAddr,
                                                        int32_t outHeight);




/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int32_t TIDL_customMaxPool_ixX_oxX_exec_cn(void * handle,
                                                      const void *src,
                                                      void *dst)
{
  int32_t status = CUSTOM_SUCCESS;
  int32_t kernelW, kernelH, strideW, strideH, outHeight;
  TIDL_CustomMaxPoolIxXOxXPrivArgs *pKerPrivArgs = (TIDL_CustomMaxPoolIxXOxXPrivArgs *)handle;
  TIDL_CustomMaxPoolIxXOxXInitArgs *pKerInitArgs = &(pKerPrivArgs->initArgs);

  const uint8_t *uint8_tSrc = (const uint8_t*) src;
  uint8_t *uint8_tDst = (uint8_t*) dst;
  TIDL_bufParams3D_t *srcAddr = &pKerPrivArgs->bufParams.natc.srcBuf3D;
  TIDL_bufParams3D_t *dstAddr = &pKerPrivArgs->bufParams.natc.dstBuf3D;

  /* Calculate the height of the last block for large feature map case */
  outHeight = dstAddr->dim_y;

  kernelW = pKerInitArgs->customMaxPoolParam.kernelW;
  kernelH = pKerInitArgs->customMaxPoolParam.kernelH;
  strideW = pKerInitArgs->customMaxPoolParam.strideW;
  strideH = pKerInitArgs->customMaxPoolParam.strideH;

  if((strideW == 2) && (strideH == 2))
  {
    if((kernelW == 3) && (kernelH == 3))
    {
      status = TIDL_customMaxPooling_3x3_skip2_exec_cn(uint8_tSrc, uint8_tDst, srcAddr, dstAddr, outHeight);
    }
    else
    {
      status = CUSTOM_FAIL;
    }
  }
  else
  {
    status = CUSTOM_FAIL;
  }

  return status;
}

/* ========================================================================== */
/*                       Static Function Definitions                          */
/* ========================================================================== */



static int32_t TIDL_customMaxPooling_3x3_skip2_exec_cn(const uint8_t *src,
                                                        uint8_t *dst,
                                                        TIDL_bufParams3D_t *srcAddr,
                                                        TIDL_bufParams3D_t *dstAddr,
                                                        int32_t outHeight)
{
  uint16_t width, height;
  uint16_t inPitch, outPitch;
  uint32_t inChPitch, outChPitch;
  uint16_t numOutChannels;
  uint8_t   inRowCol;
  uint16_t i1, i2, i3, i4, i5;
  int32_t  initVal;
  uint8_t  maxValue, maxvalueBlock;

  width          = dstAddr->dim_x;
  height         = outHeight; //dstAddr->dim_y;
  inPitch        = srcAddr->stride_y/sizeof(uint8_t);
  outPitch       = dstAddr->stride_y/sizeof(uint8_t);
  inChPitch      = srcAddr->stride_z/sizeof(uint8_t);
  outChPitch     = dstAddr->stride_z/sizeof(uint8_t);
  numOutChannels = dstAddr->dim_z;
  int32_t status = CUSTOM_SUCCESS;

  for(i1=0; i1<numOutChannels; i1++)
  {
    for(i2=0; i2<height ; i2++)
    {
      for(i3=0; i3<width ; i3++)
      {
        if(srcAddr->data_type == TIDL_UINT8)
        {
          initVal = 0;
        }
        else
        {
          status = CUSTOM_FAIL;
        }

        if(CUSTOM_SUCCESS == status)
        {
          maxValue = (uint8_t) initVal;
          for(i4=0; i4<3U; i4++)
          {
            for(i5=0; i5<3U; i5++)
            {
              inRowCol = src[(i1* inChPitch) + ((((uint32_t)i2 * 2U) + i4) * inPitch) + ((uint32_t)i3 * 2U) + i5];

              /* Max pooling operation */
              maxvalueBlock = (maxValue > inRowCol) ? (uint8_t)maxValue : (uint8_t)inRowCol;
              maxValue = maxvalueBlock;
            }
          }
          dst[(i1*outChPitch) + ((uint32_t)i2 * outPitch) + i3] = maxvalueBlock;
        }
      }
    }
  }

  return status;
}


