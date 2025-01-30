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
  *  @file     perfsim_v1.h
  *
  *  @brief    Interface Header file for performance simulator for C7x + MMA
  *
  *  @author   PKS
  *
  *  @version  0.1 - Sep 2017 : Initial Version [PKS]
  *
 *****************************************************************************
*/

#ifndef _PERFSIM_V1_H_
#define _PERFSIM_V1_H_

#include "itidl_ti.h"
#include "common.h"
#include "gc.h"


/**
* @defgroup networkAnalyzer NetworkAnalyzer: CNN Network Aanlyzer,
   Optimizer and Performance Simulator
*/

/**
* @addtogroup networkAnalyzer
* @{
*/
#define RESERVED_MEMORY_SIZE  (1000U)
#define MAXLAYERS             (TIDL_NUM_MAX_LAYERS)    /**< Maximum number of supported layers*/
/**< Assumption of maximum forks and joints in CNN network */
#define MAX_BRANCH (128)


/**
* @defgroup dataFlowAnalysis DataFlowAnalyzer: Performs the analysis for dataflow
* @ingroup  networkAnalyzer
* @{
*/

/**
 *  @enum   eDataFlowBuffer
 *  @brief  Enumeration for buffer layout
 *
 */
typedef enum
{
  ROW_CIRCULAR =  0,
         /**< This value indicates that the enrire width of the input feature
          *  map is brought and at least height  = Fc and use
          *  cicrular feature of SE to reuse the data.
          */
  ROW_LINEAR    = 1,
          /**< This value indicates that the enrire width of the input feature
          *  map is brought and at least height  = Fc and grow it vertically
          *  linearly (Frame interleaved) buffer */
  COL           = 2,
          /**< This value indicates that the enrire col of the input feature
          *  map is brought and linearly grown donwards */
  FRAME         = 3  /**< Frame level buffer */
} eDataFlowBuffer ;

/**
 *  @enum   eOptions
 *  @brief  options for DMA/Kernel invocation
 *
*/
typedef enum
{
  ONCE   = 0,  /**< ONCE - DMA/Cache brings data only once*/
  MANY  =  1,  /**< MANY - DMA/Cache brings data multiple times */
  NOTSURE= 2   /**< NOTSURE - Have not decided yet */
} eOptions ;

/**
 *  @enum   eAlignmentOptions
 *  @brief  Alignment options for performance. One can not always make sure that
 *          alignment for all the buffers is taken care. So this enum provides
 *          control among OUT feature CPU write, IN feature DMA Read,
 *          IN Feat DMA Write and Kernel DMA read
*/
typedef enum
{
                                /*| OUT_FEAT_WRITE| IN_FEAT_DMA_SRC|  IN_FEAT_DMA_DST| KERNEL_READ|  */
  COW1_DIR0_DIW1_KIR1 = 0x1011, /*| 1             |     0          |    1            | 1          |  */
  COW1_DIR1_DIW1_KIR0 = 0x1110, /*| 1             |     1          |    1            | 0          |  */
  COW1_DIR1_DIW0_KIR1 = 0x1101, /*| 1             |     1          |    0            | 1          |  */
  COW0_DIR1_DIW1_KIR1 = 0x0111  /*| 0             |     1          |    1            | 1          |  */
} eAlignmentOptions ;

/**
 *  @enum   ePreferenceOrder
 *  @brief  Depending upon layer property sometimes its beneficial to fetch feature
 *          multiple times and sometimes its beneficial to fetch WT multipe times.
 *          This enumeration provides information about the current choice for a layer
*/
typedef enum
{
  PRFER_MULTIFETCH_WT_OVER_FEAT = 0 ,
  PRFER_MULTIFETCH_FEAT_OVER_WT
} ePreferenceOrder ;


/**
 Macro(s) to
 set INDMA type (eOptions), WTDMA type (eOptions), and Buffer Type for input (eDataFlowBuffer)
 and get Buffer Type for input (eDataFlowBuffer)
*/
#define SETDATAFLOW(INDMA, WTDMA, BUFTYPE)         (((INDMA) << 8U) | ((WTDMA) << 4U) | (BUFTYPE) )
#define UPDATEDATAFLOW_BUFTYPE(dataflow, BUFTYPE)  (((dataflow) & 0xFFFFFFF0U) | (BUFTYPE))
#define UPDATEDATAFLOW_WT(dataflow, WTDMA)         (((dataflow) & 0xFFFFFF0FU) | (WTDMA <<4U))
#define UPDATEDATAFLOW_IN(dataflow, INDMA)         (((dataflow) & 0xFFFFF0FFU) | (INDMA << 8U))
#define GETDATAFLOW_BUFTYPE(dataflow)              (((uint32_t)dataflow) & 0xFU)


/**<
 *******************************************************************************
  @struct sMemoryStats_t
  @brief  This structure holds the stats related to different memory
  @param bandwidth           : Array to hold bandwidth of different memory for
                                both read and write access
  @param size                : size of different type of buffer in all memtype
  @param memSpace[READ]            : memory location (L2, OCMC, DDR) to hold srource
                                of a particular feature (input, weight, output)
  @param memSpace[WRITE]            : memory location (L2, OCMC, DDR) to hold dest
                                of a particular feature (input, weight, output)
  @param inFeatMapReadFactor : Factor of bandwidth at DDR for in Feat Map Read
  @param wtFeatMapReadFactor : Factor of bandwidth at DDR for wt Feat Map Read

  @todo : update inFeatMapReadFactor, wtFeatMapReadFactor, currently not used
 *******************************************************************************
*/

typedef struct
{
  //in: SRC, COMPUTE, wt: SRC, DST, out: COMPUTE, DST
  double bandwidth[MEMTYPE_TOTAL][ACCTYPE_TOTAL] ;
  int32_t size[BUFTYPE_TOTAL][MEMTYPE_TOTAL];
  int32_t outSizeWithCoexc;
  int32_t outHeightWithCoexc;
  int32_t memSpace[ACCTYPE_TOTAL][BUFTYPE_TOTAL];
  int32_t inFeatMapReadFactor;
  int32_t wtFeatMapReadFactor;
  double wtVolumeFetchKB;
} sMemoryStats_t ;

/**<
********************************************************************************
* @struct sBufferInfo_t                                                        *
* @brief  This structure holds the buffer information in memory                *
*                                                                              *
* Legends in figure below                                                      *
* (o)padded region, (-)actual pixels,(+)boundary of feature width,(x)junk data *
*                                                                              *
* 1 feature plane 2D is as shown below is considered in form of linear 1D      *
* +o+oooooooo+                                                                 *
* +o+--------+                                                                 *
* +o+        +                                                                 *
* +o+        + => +o+oooooooo+o+--------+o+--------+....o+--------+oxxxx       *
* +o+        +                                                                 *
* +o+--------+                                                                 *
* +o+oooooooo+oxxxx                                                            *
* 2D representation       ==> 1D representation                                *
*                                                                              *
* So tensor (multiple feature planes) is represeted in 2D form as below        *
*                                                                              *
* <------------- Linearized 1D Feature Plane------------>                      *
* +o+oooooooo+o+--------+o+--------+....o+--------+oxxxx  ^                    *
* +o+oooooooo+o+--------+o+--------+....o+--------+oxxxx  |                    *
* .                                                       |                    *
* .                                                       Ni (# Feature Planes)*
* .                                                       |                    *
* +o+oooooooo+o+--------+o+--------+....o+--------+oxxxx  ^                    *
* Physical buffer Size = bufWidth*bufHeight (not always, but mostly)           *
*                                                                              *
* @param accessor     : Indicates who is doing access - CPU/DMA                *
* @param space        : Indicates which mmeory space: L2/DDR/MSMC              *
* @param bufWidth     : Physical buffer width (including junk pixels)          *
* @param bufHeight    : Physical buffer height (Mostly Ni)                     *
* @param bufSize      : Physical buffer size, not necessary width*height,      *
*                       explain later why? - TBD                               *
* @param activeBufWidth: Pixels in width excluding junk data                   *
* @param baseMem      : Mmeory address assuming start as 0                     *
* @param accessoffset : Indicates first byte offset for accessing the data     *
*                       for read/write operation. Mostly during write padded   *
*                       region is not written by CPU processing and during read*
*                       padded data is consumed by CPU processing              *
*                       Sometimes padding can be larger to have common buffer  *
*                       layout across many layers so it also indicates the     *
*                       relevant position to read/write                        *
*******************************************************************************/

typedef struct
{
  int32_t accessor;
  int32_t space;
  int32_t bufWidth   ;
  int32_t bufHeight  ;
  int32_t bufSize    ;
  int32_t activeBufWidth;
  int32_t baseMem ;
  int32_t accessoffset;
  int32_t padC ;
  int32_t padR ;
  int32_t padCZeros ;
  int32_t padRZeros ;
  int32_t padCFillZeros ;
  int32_t padRFillZeros ;
  int32_t padCReq ;
} sBufferInfo_t;

/**<
 *******************************************************************************
  @struct sDataFlowInfo_t
  @brief  This structure holds the information on data flow

  @param dataFlowType        : Type of data flow, refer @eDataFlow
  @param numSplit            : Number of splits in the input feature map,
                               If (numSplit == 1) then it is Small Feature Map
                               If (numSplit > 1) then it is Large Feature Map
  @param procSize            : Per kernel call, the processing size for each output feature
  @param Nci                 : Number of input channels being processed in chunk
  @param Nco                 : Number of output channels being processed in chunk
  @param bufWidth            : Width of the diff type of buffers in int memory
                                - in, out, wt
  @param bufHeight           : Height of the diff type of buffers in int memory
                                - in, out, wt
  @param bufSize             : Size of the diff type of buffers in int memory - in, out, wt
                               (bufSize can be lesser than bufWidth*bufHeight)
  @param kernelFreq          : Frequency of kernel invocation
  @param dmaFreq             : Frequency of kernel invocation
  @param preFetch            : Width on input feature map which is bare minimum and produces 0 pixel
                               Any additional pixel would result in output
  @param preFetchAligned     : Aligned value of preFetch (for efficient DMA )
                               Any additional pixel would result in output
  @param numPartsVer         : Number of vertical portions for DMA (only valid for COL flow)
  @param numPartsHor         : Number of horizontal portions for DMA (only valid for COL flow)
  @param numGroups           : Only relevant for COL flow, otherwise it should hold value of 1.
                               In COL flow it indicates the number of veritical partition of a
                               image required to do to perfrom COL flow
  @param memStats            : Stats of different memory - like bandwidth, size
  @param bufInfo             : Indicates the buffer Info for each IN/OUT/WT and for both read/write access
                               Read means read from DMA and write means write by DMA
                               If it is read/written by CPU then
  @param privContextMemBuff    : Indicates the context memory buffer for Private algorithm
 *******************************************************************************
*/
typedef struct
{
  int32_t dataFlowType              ;
  int32_t numSplit                  ;
  int32_t procSize                  ;
  int32_t numSplitMultiInput[MAX_ADDITIONAL_INPUTS]  ;/* Currently only used for Matmul*/
  int32_t procSizeMultiInput[MAX_ADDITIONAL_INPUTS]  ;/* Currently only used for Matmul*/
  int32_t Nci                       ;
  int32_t Nco                       ;
  int32_t kernelFreq                ;
  int32_t dmaFreq                   ;
  int32_t dmaFreqWt                 ;
  int32_t preferenceOrder           ;
  int32_t preFetchAligned           ;
  int32_t multiInputPreferenceOrder ;/* Used for matmul layer to decide which buffer to refetech, value of 0 will incdicate buffer 1 is re-feteched*/
  int32_t preFetch                  ;
  int32_t firstTransferRemainder    ; /*Effective Prefetch for OTF*/
  int32_t requiredInPlaneSize       ;
  sBufferInfo_t  bufInfo[BUFTYPE_TOTAL][ACCTYPE_TOTAL];
  sBufferInfo_t  wtOneShot[ACCTYPE_TOTAL] ; // This is special DMA and expected to happen only once for weights
  sBufferInfo_t privContextMemBuff;

  //private variables - will not be exposed later
  sMemoryStats_t memStats       ;
  int32_t bufWidth[BUFTYPE_TOTAL]   ;
  int32_t bufHeight[BUFTYPE_TOTAL]  ;
  //int32_t holdingLayersSize[MEMTYPE_TOTAL];

  //iteration control
  int32_t bufState                  ;
//  int32_t wtSkew                    ;
 // int32_t inSkew                    ;
  //int32_t outSkew                   ;
  int32_t forceOutDDR               ;
  int32_t forceOutStgDDR            ;
  int32_t forceWtStgDDR             ;
  int32_t forceWtDDR ;
  int32_t forceContextMemDDR        ;
  int32_t forceDDR;
  int32_t reservedSpace[RESERVED_MEMORY_SIZE];
} sDataFlowInfo_t ;


/**<
*******************************************************************************
@struct sPerfSimConfig_t
@brief  This structure allows user to configure the key parameters for DL-NAPS
@param freqMHz           : Frequency of the C7x+MMA in MHz
@param sizeL2MemKB       : L2 SRAM Memory size which is part of C7x+MMA sub system. This is only SRAM
and L2 cache is not part of this.
@param matPanelSize      : Size of MAtrix Panel (64 in Generation 1)
@param drainLatnecy      : Latency cycles to drain C matrix out of MMA
@param setupCycles       : Setup cycles for a MMA Loop
@param layerSetupCycles  : Setup cycles for a Layer
@param mmaCoreLoopOH     : Overheads on top of Core Loop in MMA, (>100 means OH per output block
and the per Block OH is mmaCoreLoopOH - 100
< 100 means overall overhead cycles for one function call
@param numCores          : The number of C7x + MMA core is varying (from 1 to 2) in different
SOC of J7 and TDA4 product line. Set for nunber of cores in SOC
@param sizeL3MemKB       : MSMC SRAM size
@param busWidth_L3_L2    : Bus width b/w MSMC and L2
@param busWidthDDR       : Bus width b/w DDR and SOC
@param freqDDR           : Frequency of DDR
@param numEMIFPorts      : Number of EMIF ports to DDR
@param ddrEfficiency     : Efficiency of DDR
@param L2Efficiency      : Efficiency of L2
@param msmcEfficiency    : Efficiency of MSMC
@param dataType          : The data type of network model and features. 0 for  8-bit  1 for  16-bit
@param mmaDoesPooling    : 1 if MMA can do pooling, else 0. Gen 1 it is 0
@param numBiasInstance   : Number of bias instances in unit of matPanelSize
@param alignmentOption   : OPtions for alignement of feature planes, refer eAlignmentOptions
@param fileFormatNet     : Network File Format
@param batchSize         : Number of batches
@param deviceName        : Target Device
@param enablePersistWtAlloc : Value of 1 will try to allocate weight buffer in any remaining
memory available in L2/MSMC so that their DMA can only be done during activate/
de-activate function
*******************************************************************************
*/
typedef struct
{
  int32_t   freqMHz;
  int32_t   sizeL2MemKB;
  int32_t   matPanelSize;
  int32_t   drainLatnecy;
  int32_t   setupCycles;
  int32_t   layerSetupCycles;
  int32_t   mmaCoreLoopOH;
  int32_t   numCores;
  int32_t   coreStartIdx;
  int32_t   sizeL3MemKB;
  int32_t   busWidth_L3_L2;
  int32_t   busWidthDDR;
  int32_t   freqDDR;
  int32_t   numEMIFPorts;
  float     ddrEfficiency;
  float     L2Efficiency;
  float     msmcEfficiency;
  int32_t   dataType;
  int32_t   mmaDoesPooling;
  int32_t   numBiasInstance;
  int32_t   alignmentOption;
  int32_t   fileFormatNet;
  int32_t   batchSize;
  int32_t   deviceName;
  int32_t   enablePersistWtAlloc;

  int32_t   numPanelSplitsVertical;
  int32_t   numWritePortsL3;
  int32_t   numSEReadPortsL3;
  int32_t   numSEreadPortsL2;
  int32_t   numDRUPorts;
  int32_t   BPanelWritePorts;
  int32_t   numLUTWriteMMA;

  int32_t   ddrLayers[MAXLAYERS];

} sPerfSimConfig_t;
/**<
 *******************************************************************************
  @struct sPerfSim_t
  @brief  This structure defines the key parameters exported out by simulator
  @param  simConfig : Config Parameters paased to Analysis tool
  @param  contextMemLyrNums : List of layers to be backed up at layer boundary for preemption
  @param  procTime : Processing time from NC required to identify preemption layers  
  @param  bufDBIndex : For Layer i, this holds the buffer information to be backed up at the start of layer i
  @param  sdataFlowInfo : dataflow information for each layer of the network
 *******************************************************************************
*/
typedef struct
{
  sPerfSimConfig_t   simConfig;
  int32_t            memorySizeRequirement[SUPERMEMTYPE_TOTAL];
  int32_t            bufDBIndex[MAXLAYERS][MAX_BRANCH]; // index to buffer data base of (A) all coex, prev, and context
  double             procTime[MAXLAYERS]; 
  int32_t            layerExecutionOrder[MAXLAYERS];
  int32_t            checkErrorFlag[MAXLAYERS];
  double             preEmptWorstCaseDelay;   
  sDataFlowInfo_t    sdataFlowInfo[MAXLAYERS] ;  // This is not allocated memory now
  sGraphCompilerOutArgs_t graphCompOut;
} sPerfSim_t ;
 
/** @} */ // end of group networkAnalyzer

#endif
