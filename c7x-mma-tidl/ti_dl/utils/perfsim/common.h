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
  *  @file     common.h
  *
  *  @brief    Mainly to hold common macros, enums, constants across project
  *
  *  @author   PKS
  *
  *  @version  0.1 - Sep 2017 : Initial Version [PKS]
  *
 *****************************************************************************
*/

#ifndef _COMMON_H_
#define _COMMON_H_

/**
* @addtogroup networkAnalyzer
* @{
*/
#define MAX_CORES (TIDL_MAX_NUM_CORES)
#define MAX_MEM_PER_DRU_CHANNEL  (0xFFFFU)

#define MIN(A,B) ((A) < (B) ? (A) : (B))
#define MAX(A,B) ((A) > (B) ? (A) : (B))
#define ABS(A)   ((A) > (0) ? (A) : (-(A)))

/** To align to a integer boundary  -
    mainly used for aligning to matrix panel size or any buswidth size */
#define ALIGN(VAL, ALIGN) ((((VAL) + (ALIGN) - 1)/(ALIGN)) * (ALIGN) )

/** To align to a integer boundary by using floor*/
#define ALIGN_FLOOR(VAL, ALIGN) (((VAL)/(ALIGN)) * (ALIGN) )


/** To Align in unit of K*Align + offset -
    Mainly used for Memory arrangement to minimize bank conflicts */
#define ALIGN_WITHOFFSET(VAL, ALIGN, OFFSET) ( ALIGN(VAL, ALIGN) + OFFSET )

/*To get Numcores based on Start Core Idx */
#define GET_NUMCORES_FOR_STARTCOREIDX(NUMCORES, STARTCOREIDX) (NUMCORES + STARTCOREIDX)
#define GET_RELATIVE_COREIDX(COREID, STARTCOREIDX) (COREID - STARTCOREIDX)

#define   NONE (-1)

/**  */
#define MAX_ADDITIONAL_INPUTS (1)

/** to extract number of bytes of an elements */
#define BYTES_PER_ELEMENT(dataType) (getByteCnt(dataType))

/**
*  @enum       eTileType
*  @brief      Enumeration for different Tile types in SuperTiling
*              No Tile represents absence of supertiling in the layer
*              Every tile except the first and last is a normal tile
*/
typedef enum{
    NO_TILE = 0,
    FIRST_TILE,
    FIRST_NORMAL_TILE,
    NORMAL_TILE,
    LAST_NORMAL_TILE,
    LAST_TILE,
    MAX_TILE_TYPES
}eTileType;


/**
*  @enum       eDataType
*  @brief      Enumeration for different data type for MMA
*
*/
typedef enum
{
  DATA_8_BIT    = 0,  /**< 8-bit data type for weights and feature map*/
  DATA_16_BIT   = 1,  /**< 16-bit data type for weights and feature map*/
  DATA_32_BIT   = 2,   /**< 32-bit data type for weights and feature map*/
  DATA_64_BIT   = 3,   /**< 64-bit data type for weights and feature map*/
  DATA_8S_BIT   = 4,  /**<  8-bit data type (signed)*/
  DATA_16S_BIT  = 5,  /**< 16-bit data type (signed)*/
  DATA_32S_BIT  = 6,   /**< 32-bit data type (signed)*/
  DATA_64S_BIT  = 7,   /**< 64-bit data type (signed)*/
  FLOAT_32S_BIT = 8 /**< 32-bit float data type (signed). 
                    This datatype needs to be separately needed to differentiate between 
                    a 32 bit integer and 32 bit float data*/
} eDataType;

static inline int32_t getByteCnt(int32_t dataType)
{
  if(dataType == FLOAT_32S_BIT) dataType = 2 ;
  else if (dataType > DATA_64_BIT) dataType -= DATA_8S_BIT;
  return (1 << dataType );
}
//0 postive, 1 negetive
static inline int32_t getSign(int32_t dataType)
{
  int32_t sign;
  if (dataType <= DATA_32_BIT) sign = 0;
  else sign = 1;
  return sign;
}

/**
 *  @enum       eMemType
 *  @brief      Enumeration for different level of memory
 *
 */
typedef enum
{
  L2 = 0,     /**< DSP L2  Memory       */
  MSMC,       /**< MSMC Memory          */
  DDR,        /**< External memory DDR  */
  MEMTYPE_TOTAL
} eMemType;

typedef enum
{
  DDR_PERSIST = DDR+1,  /**< External Persistent Memory in DDR  */
  SUPERMEMTYPE_TOTAL
} eSuperMemType;

/**
 *  @enum   eStatus
 *  @brief  Enumeration for state variables (some times used for function return)
 *
 */
typedef enum
{
  NOT_VALID    = -1,  /**< Not Valid or Not initialized  */
  NOT_PRODUCED = -2,  /**< Data is not produced - specific for state */
  PRODUCED     = 100, /**< Data is not produced - specific for state */
  VALID        = 200  /**< Valid  - Use for state variable as well as return*/
} eStatus;

/**
 *  @enum   eReturnType
 *  @brief  Enumeration for function return indicating success/failure
 *
 */
typedef enum
{
  RETURN_PASS = 0,
  RETURN_FAIL = -1
} eReturnType;

/**
 *  @enum       eMemAccessType
 *  @brief      Enumeration for different access type
 *
 */
typedef enum
{
  READ = 0, /**< Read  Access */
  WRITE,    /**< Write Access */
  ACCTYPE_TOTAL
} eMemAccessType;

/**
*  @enum       eMemAccessor
*  @brief      Enumeration for accessor
*
*/
typedef enum
{
  CPU =   0, /**< CPU */
  DMA,       /**< DMA */
  NO_ACCESS, /**< DMA */
  DMA_ONCE,/** Persistent DMA */

  ACCESSOR_TOTAL
} eMemAccessor;

/**
 *  @enum       eBufferName
 *  @brief      Enumeration for different type of buffers
 *
 */
typedef enum
{
  IN_FEAT_MAP = 0, /**< Input Feature Map  */
  OUT_FEAT_MAP,    /**< Output Feature Map */
  WT_MAP,          /**< Wight Feature Map */
  BUFTYPE_TOTAL
} eBufferName;

/**
*  @enum   ePaddingType
*  @brief  Enumeration for padding types
*/
typedef enum
{
  PAD_ACTUAL = 0, /**< Physical padding space in buffer    */
  PAD_TOFILL,     /**< Padding to be filled by processing  */
  PAD_MAX_TYPES,  /**< Max padding types                   */
} ePaddingType;

/**
 *  @enum       eComputeCycleCategory
 *  @brief      Enumeration for different type of compute cycles
 *
 */
typedef enum
{
  CORERAW = 0,    /**< Raw cycles of core loop */
  COREL2,         /**< Cycles of core loop including L2 conflict*/
  SETUPOH,        /**< Overhead cycles of setup in outer loop*/
  COMPUTE,        /**< Total compute cycles = setup + core loop */
  COMP_TYPES      /**< Total types of compute cycles being categorized */
} eComputeCycleCategory;

/**
 *  @enum       eIOCycleCategory
 *  @brief      Enumeration for different type of IO cycles
 *
 */
typedef enum
{
  DDRACCESS = 0 , /**< Equivalent cycles for DDR access */
  MSMCACCESS,     /**< Equivalent cycles for MSMC access */
  IO,             /**< Total IO cycles (MSMC/DDR) */
  IO_TYPES        /**< Total types of IO cycles being categorized */
} eIOCycleCategory;

/**
 *  @enum   eOpType
 *  @brief  Enumeration for different type of operations
 *
 */
typedef enum
{
  OPTYPE_VEC = 0, /**< Vector Type operations*/
  OPTYPE_MAT,     /**< Matrix Type operations*/
  OPTYPE_TOTAL,   /**< Total Operations*/
  OPTYPE_MAT_CONV,
  OPTYPE_MAT_DECONV,
  OPTYPE_MAT_DWSCONV,
  OPTYPE_MAT_FC,
  OPTYPE_VEC_RELU,
  OPTYPE_VEC_POOL_3x3_S1,
  OPTYPE_VEC_POOL_3x3_S2,
  OPTYPE_VEC_POOL_OTHERS,
  OPTYPE_VECS_DUMMY,
  OPTYPE_VECS_OTHERS,
  OPTYPE_MAX
} eOpType ;

typedef enum
{
  CANT_OVERWRITE,                 /*Overwrite not allowed to any of the inputs*/
  CAN_OVERWRITE,                  /*Overwrite allowed to any of the inputs*/
  CAN_OVERWRITE_ONLY_FIRST_INPUT, /*Overwrite allowed only to first input  buffer*/
  OVERWRITE_TYPES_MAX
} eOverwriteType ;

/** @} */ // end of group

#define START_CORE (0)
/*temp variable this will come from user imput*/

#endif
