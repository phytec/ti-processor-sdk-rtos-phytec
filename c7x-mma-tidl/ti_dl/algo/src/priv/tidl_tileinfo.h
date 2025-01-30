/*
 *******************************************************************************
 *
 * Module Name: TIDL Network Analyzer and Performance Simulator
 *
 * "Performance simulator for CNN netowrks" is software module developed
 *  for TI's C7x + MMA processor architecture. it models the performance
 *  of different layers of CNN netowrk and provides a summarized output
 * of performance

 * Copyright (C) 2017 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *******************************************************************************
*/

/**
  ******************************************************************************
  *  @file     tidl_tileinfo.h
  *
  *  @brief    Holds the structure for key infomration of a layer
  *
  *  @author   PKS
  *
  *  @version  0.1 - Dec 2019 : Initial Version
  *
 *****************************************************************************
*/

#ifndef _TIDL_TILEINFO_H_
#define _TIDL_TILEINFO_H_

#include <stdint.h>
#include "itidl_ti.h"
#define TIDL_NUM_SPLITS (256U)
/**<
 *******************************************************************************
  @struct sTIDL_AlgSTileParams_t
  @brief  This structure holds the key parameters for different kind of tiles

  *******************************************************************************
*/
typedef struct {
  int32_t tileGroupId;
  int32_t numTiles;
  int32_t firstTileHeight;
  int32_t lastTileHeight;
  int32_t tileHeight;
  int32_t isInputTiled[TIDL_NUM_IN_BUFS];
  int32_t isOutputTiled;
  int32_t outLinesToBackup;
  int32_t inLineOffset[TIDL_NUM_IN_BUFS];
  int32_t outLineOffset; //Single value since only one output buffer.
  int32_t lagComponent; //(AEP) When two consumers of a layer have different first tile heights, it causes the one with the smaller first tile height to need extra backup, this component indicated how much the layer with smaller 1st tile height needs to shift its inoffset up 
  int32_t currentLineOffset;
  int32_t consumersInSameTileGroup;
  int32_t contextMemBase;
  int32_t contextMemSize;  
}sTIDL_AlgSTileParams_t;


/** @} */ // end of group

#endif /*_TIDL_TILEINFO_H_ */
