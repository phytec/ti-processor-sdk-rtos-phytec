/*
* module name       :TIDL 
*
* module descripton :TI Deep learning Library module is TIs CNN/DNN 
*                    acceleration on TI DSP 
*
* Copyright (C) 2019-2020 Texas Instruments Incorporated - http://www.ti.com/ 
*
* ALL RIGHTS RESERVED 
*
*
*/

/**
 ----------------------------------------------------------------------------
 @file    tidl_stalg.h
 @brief   This file defines APIS and Data structures of TIDL ST algorithms.
 @version 0.1 (Mar 2020) : Intial Version
 ----------------------------------------------------------------------------
*/

#ifndef ITIDL_PRIVALGO_INT_H
#define ITIDL_PRIVALGO_INT_H

#include <stdint.h>
#include "itidl_ti.h"
/* Below Header is a perfsim private header*/
#include "tidl_tileinfo.h"

#define TIDL_ST_TYPE_FIRST                      (0U)
#define TIDL_ST_TYPE_NORM                       (1U)
#define TIDL_ST_TYPE_LAST                       (2U)
#define TIDL_ST_TYPE_MAX                        (3U)
#define TIDL_ST_TYPE_NON_TILED                  (4U)

/**<
 *******************************************************************************
  @struct sTIDL_PrivAlgoHanlde_t
  @brief  This structure holds the key parameters for Priv Algorithm context/Handle

  *******************************************************************************
*/

typedef struct {
  int32_t tileGroupId;
  int32_t stgIdx;
  int32_t stileIdx;
  int32_t layerIdx;
  int32_t algLayerIdx;
  int32_t numStGroups;
  int32_t numTilesStg[TIDL_MAX_NUM_GROUPS];
  /*The private context is kept in MSMC, using 16-bit values since outLineOffset & backupLines
    are number of rows and will not exceed a 16-bit container with ST*/
  int16_t outLineOffsetList[TIDL_NUM_MAX_LAYERS];
  int16_t backupLinesList[TIDL_NUM_MAX_LAYERS];
}sTIDL_PrivAlgoHanlde_t;


/** @} */ // end of group

#endif /*ITIDL_PRIVALGO_INT_H */
