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
  *  @file     perfsim.h
  *
  *  @brief    Interface Header file for performance simulator for C7x + MMA
  *
  *  @author   PKS
  *
  *  @version  0.1 - Sep 2017 : Initial Version [PKS]
  *
 *****************************************************************************
*/

#ifndef _PERFSIM_H_
#define _PERFSIM_H_

#include "itidl_ti.h"
#include "common.h"
#include "perfsim_v1.h"
#include "gc.h"

/**
* @defgroup networkAnalyzer NetworkAnalyzer: CNN Network Aanlyzer,
   Optimizer and Performance Simulator
*/

/**
* @addtogroup networkAnalyzer
* @{
*/

extern int32_t gTempChanges ; /**< Temporary -to be removed, used only
                                  for some fixes required for TIDL to
                                  use until its best optimal*/

#define MAXLAYERS                 (TIDL_NUM_MAX_LAYERS)    /**< Maximum number of supported layers*/
#define TI_NW_COMPILER_VERSION    "TI_NETWORK_COMPILER_08.05.00"

#define L3MEM_SCRATCH_BUF_SIZEKB (56U) /**< Size of L3 MEM used for TIDL scratch buffers */
#define L2MEM_SCRATCH_BUF_SIZEKB (2U)  /**< Size of L2 MEM used for TIDL scratch buffers */

/**
* @defgroup dataFlowAnalysis DataFlowAnalyzer: Performs the analysis for dataflow
* @ingroup  networkAnalyzer
* @{
*/
  // int32_t firstTransferRemainder    ; /*Effective Prefetch for OTF*/

typedef enum
{
  ERR_UNSUPPORTED_DATA_TYPE = 0 ,
  ERR_MMEORY_OVERLAP ,
  ERR_MMEORY_INSERTION,
  ERR_MMEORY_ALLOCATION,
  ERR_HIGHER_INSTANCEMEM,
  ERR_LESSER_SD_PARTS,
  ERR_LESSER_MAX_CONSTRAINED_DATA,
  ERR_LESSER_MAX_BA_SUBCONSTRAINT,
  ERR_LESSER_MAX_MEM_CHUNKS,
  ERR_NO_OVERLAP_ALLOCATION,
  ERR_WRONG_NETWORK_STRUCTURE,
  ERR_UNSUPPORTED_ST_LAYER,
  ERR_MAX = 32
} eErroCodeBits ;

extern int32_t gErrCode ;
/**
 *******************************************************************************
 *  @func          getDbgInfoBufSize
 *  @brief         This is the public function of performance simulator
 *  @details       This function returns the amount of memory required for
 *                 the to store the internal information, which can be used as debug
 *                 in future.
 *
 *  @return        NONE
 *  @remarks
 *******************************************************************************
 */
int32_t getDbgInfoBufSize();

/**
 *******************************************************************************
 *  @func          analyzeNetwork
 *  @brief         This is the public function of performance simulator
 *  @details       The flow of this function is as follows
 *                 -# It reads the TIDL network format and puts into a simple
 *                    structure #sLayerProp_t
 *                 -# Then it computes all the raw stats of the network like
 *                    voulme of each buffer (in, out and weight)and raw
 *                    operations and genrats data structure #sRawStats_t
 *                 -# Then it does the anlysis for execution order for layer.
 *                    while doing this it puts the layer in a slot also
 *                    indicating the layers which are parallel. Data structure
 *                    #sLayerExecution_t is formed. With this info it will
 *                    merge the layers which can be connected at local memory
 *                    level
 *                 -# Then it finds the data flow among different possible
 *                    options while doing this it fine tunes the parameters
 *                    like block size, number of instances for input and
 *                    output blocks etc and computes the iterations required
 *                    for the call of core MMALib function, refer #sDataFlowInfo_t
 *                 -# The it models the MMALib core function by decomposing
 *                    large matrix multiply in small matrix to fit in MMA
 *                    and account all aspects of aligning to panel size,
 *                    loop overheads and other overheads for SE/DMA programming
 *                    and computes the cycles. refer #sNetComputeStats_t
 *
 *  @param [in]    pNetIn       : TIDL Network format - produced from translator
 *  @param [in]    pPerfSimParams : Pointer to struct holding device information
 *  @param [out]   pPerfInfoOut : Key information exposed by simualtor
 *  @param [out]   pDbgInfoOut  : Internal information, can be used as debug
 *                 in future. Format is not known to external user
 *  @param [in]    dataType     : Refer #eDataType
 *  @return        NONE
 *  @remarks
 *******************************************************************************
 */
 
 int32_t analyzeNetwork(const sTIDL_Network_t *pNetIn,
                        const sPerfSimConfig_t * pPerfSimParams,
                        sPerfSim_t *pPerfInfoOut,
                        void *pDbgInfoOut,
                        int32_t dataType,
                        char * fileNameGrpInfo) ;

void setDefaultConfigParams(sPerfSimConfig_t* pPerfSimParams);

/** @} */ // end of group networkAnalyzer

#endif
