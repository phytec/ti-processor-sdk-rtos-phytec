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
*  @file     perfsim_v2.h
*
*  @brief    Interface Header file for Graph Compiler for C7x + MMA
*
*  @author   PKS
*
*  @version  0.1 - June 2021: Initial Version [PKS]
*
*****************************************************************************
*/

#ifndef _PERFSIM_V2_H_
#define _PERFSIM_V2_H_

#include "itidl_ti.h"
#include "common.h"

/**
* @defgroup networkAnalyzer NetworkAnalyzer: CNN Network Aanlyzer,
Optimizer and Performance Simulator
*/

/**
* @addtogroup networkAnalyzer
* @{
*/

#define MAX_WORKLOAD_PER_CORE     (TIDL_NUM_MAX_LAYERS)    /**< Maximum number of supported layers*/
#define MAX_SRC                   (TIDL_NUM_IN_BUFS)       /**< Maximum number of input source*/


/**
* @defgroup dataFlowAnalysis DataFlowAnalyzer: Performs the analysis for dataflow
* @ingroup  networkAnalyzer
* @{
*/

/*****************************************************************************
* There are 13 data buffers and 7 transfer links as shown below per workload *
*----------+------+--------------+-----+-----------+------+------------------*
* Type    :| SRC  |              |STAGE|           | DST  |                  *
*----------|------|--------------|-----|-----------|------|------------------*
* IN      :| [Bx] | ........(T0).|..x..|.........> |[B0]  |                  *
* WTNext  :| [B1] | ........(T1).|..x..|.........> |[B2]  |                  *
* WT      :| [B3] | ...(T2).....>|[B4].|..(T3)...> |[B5]  |                  *
* OUT     :|      |              |[B6] |...(T4)..> |[B7]  |                  *
* CTX_ST  :| [B8] | ........(T5).|..x..|.........> |[B9]  |                  *
* CTX_RST :| [B10]| ........(T6).|..x..|.........> |[B11] |                  *
*----------+------+--------------+-----+-----------+------+------------------*
* Please note that IN(SRC) is a special buffer which internally can have     *
* multiple buffers                                                           *
* Then there are 3 Processor links which connects the 3 out of 13 buffers to *
* processing element as shown below, where B5 = weight, B0 = i/p and B6 = o/p*
* [B0] ---(P0)------\                                                        *
*                  ==> (Processing) -----(P2) -----> [B6]                    *
* [B5] ---(P1)------/                                                        *
*                                                                            *
* During a particular workload processing some of the links might not be     *
* active and same is the case for some of the buffers                        *
*                                                                            *
*****************************************************************************/

/*****************************************************************************
* Above use case demands a representation in form of nodes and links         *
* so a generic software view/components are defined with below appraoch      *
*                                                                            *
* +------ +                                                      +------ +   *
* |       |  +------ +  +------------------------- +  +------ +  |       |   *
* | Node  |--| Joint |--|        Link              |--| Joint |--| Node  |   *
* |       |  +------ +  +------------------------- +  +------ +  |       |   *
* |       |                                                      |       |   *
* +------ +                                                      +------ +   *
*                                                                            *
* A Node can be a Buffer Node or Processng Node, and Link is a generic link  *
* Joints connects the link to Nodes and makes a full pipe with src and sink  *
* Since the use case demands multi source situation, Joints on source side   *
* are multi instances                                                        *
* Links have a flow property which indicates the granualarity at which       *
* informaiton is transmitted from source node(s) to sink node                *
******************************************************************************
* In order to define the use case mentioned in first section, a data base of *
* processing and buffer nodes is created and then for each workload is a list*
* of ordered links                                                           *
* so a execution engine should perform below operation                       *
* link[0] -> Link[1] --> link [2]-->.........-> link[N]                      *
******************************************************************************/
/**** { All structures and enums definiton related to Nodes. starts */

/**
*  @enum   ePaddingType
*  @brief  Enumeration for padding types
*/
typedef enum
{
  PAD_ACTUAL = 0, /**< Physical padding space in buffer    */
  PAD_REQUIRED,   /**< Required padding by processing      */
  PAD_TOFILL,     /**< Padding to be filled by processing  */
  PAD_MAX_TYPES,  /**< Max padding types                   */
} ePaddingType;

/**<
********************************************************************************
* @struct sPadParams_t                                                         *
* @brief  This structure provides the ifnormation for paddign values in buffer *
*         the unit of values is elemnts ( not bytes)                           *
* @param cols : Array holding the pad values in col direction, this array      *
*               holds 3 valus : Actual physical space for padding, required &  *
*               to be filled                                                   *
* @param rows : Array holding the pad values in row (top & bottom) direction,  *
*               this array holds 3 valus : Actual physical space for padding,  *
*               required & to be filled                                        *
*******************************************************************************/
typedef struct
{
  int32_t cols[PAD_MAX_TYPES];
  int32_t rows[PAD_MAX_TYPES];
} sPadParams_t;


/**
*  @enum       eBufShapeType
*  @brief      Enumeration for different shapes of buffer
*/
typedef enum
{
  BUF_LINEAR = 0,   /**< Linear buffer */
  BUF_CIRCULAR,     /**< Circuar buffer */
  BUF_PING_PONG     /**< Ping Pong buffer */
} eBufShapeType;

/**
*  @enum       eBufType
*  @brief      Enumeration for different types of buffer, This is more of
*              informational tag attached to each buf node and mostly useful
*              for debug, but may not be required to implement processing chain
*/
typedef enum {
  BUF_FM_FULL = 0, /**< Entire feature map  ( mostly DDR/MSMC)           */
  BUF_FM_IN_PART,  /**< Part of feature map ( always L2)                 */
  BUF_FM_OUT_PART, /**< Part of feature map output ( mostly MSMC)        */
  BUF_FM_CTXT,     /**< Context information of feature map               */
  BUF_FC_FULL,     /**< Feature coeffs - full data                       */
  BUF_FC_IN_STAGE, /**< Intermediate buffer for weights (feature coeffs) */
  BUF_FC_IN_FINAL, /**< Final buffer space for  weights (feature coeffs) */
  BUF_FC_IN_ADV,   /**< Advance transfer weights (feature coeffs)        */
  MAX_BUF_PER_WL,  /**< Maximum buffers per workload                     */
} eBuftype;

/**<
********************************************************************************
* @struct sBufParams_t                                                         *
* @brief  This structure holds the information for buffer nodes                *
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
* @param dataId       : Unique ID to the buffer in a buffer data base          *
* @param dataType     : data type of buffer liek 8/16-bit refer @eDataType     *
* @param space        : mmeory space of buffer: L2/DDR/MSMC, refer @eMemType   *
* @param baseMem      : Mmeory address assuming start as 0                     *
* @param bufWidth     : Buffer width (including junk elemnts), unit: elemnet   *
* @param bufHeight    : Buffer height(Mostly Ni), unit: elemnet                *
* @param bufSize      : Physical buffer size, not necessary width*height,      *
*                       explain later why? - TBD, unit: elemnet                *
* @param activeBufWidth: Pixels in width excluding junk data                   *
* @param type          : refer @eBufType                                       *
* @param shapeType     : refer @eBufShapeType                                  *
* @param padParams     : Padding Parameters, refer @sPadParams_t               *
*******************************************************************************/
typedef struct {
  int32_t dataId;
  int32_t dataType;
  int32_t space;
  int32_t baseMem;
  int32_t bufWidth;
  int32_t bufHeight;
  int32_t bufSize;
  int32_t activeBufWidth;
  int32_t type;
  int32_t shapeType;
  sPadParams_t padParams;
} sBufParams_t;

/**
*  @enum       eProcDependencyType
*  @brief      Enumeration for different processing nodes as per dependency type
*/
typedef enum
{
  PROC_DEPEND_POINT = 0,/**< Point to point dependency, example RELU       */
  PROC_DEPEND_RECT,    /**< Spatial dependency, example Pooling           */
  PROC_DEPEND_CUBE      /**< Spatial and depth dependency, eg non-DWS Conv */
} eProcDependencyType;

/**
*  @enum       eProcSourceType
*  @brief      Enumeration for different processing nodes as per number of source
*/
typedef enum
{
  PROC_SRC_SINGLE_TENSOR = 0, /**< Single input tesnor, example conv, pool   */
  PROC_SRC_MULTI_TENSOR,     /**< Multi input tesnor, example Concat, Eltwise*/
} eProcSourceType;

/**<
********************************************************************************
* @struct sProcParams_t                                                        *
* @brief  This structure holds the processing node type related info           *
*                                                                              *
* @param procId    : Unique ID to the processing node                          *
* @param srcType   : type of the source for this node @eProcSourceType         *
* @param dependency: type of source dependency @eProcDependencyType            *
*******************************************************************************/
typedef struct {
  int32_t procId;
  int32_t srcType;
  int32_t dependency;
} sProcParams_t;

/**<
*******************************************************************************
* @struct sBufDataBase_t                                                      *
* @brief  This structure holds the data base of all buffer nodes              *
* @param count     : Number of nodes                                          *
* @param pNodeList : pointer to List of nodes                                 *
******************************************************************************/
typedef struct {
  int32_t count;
  sBufParams_t *pNodeList;
} sBufDataBase_t;

/**<
*******************************************************************************
* @struct sProcDataBase_t                                                     *
* @brief  This structure holds the data base of all processing nodes          *
* @param count     : Number of nodes                                          *
* @param pNodeList : pointer to List of nodes                                 *
******************************************************************************/
typedef struct {
  int32_t count;
  sProcParams_t *pNodeList;
} sProcDataBase_t;

/**<
*******************************************************************************
* @struct sNodeDB_t                                                           *
* @brief  This structure holds the data base of all nodes                     *
* @param bufDB  : Data base for all buffer nodes                              *
* @param procDB : Data base for all proc nodes                                *
******************************************************************************/
typedef struct {
  sBufDataBase_t   bufDB;
  sProcDataBase_t  procDB;
} sNodeDB_t;
/**** All structures and enums definiton related to Nodes ends } */

/**** {All structures and enums definiton related to Link/joints start */
/**
*  @enum       eLinktype
*  @brief      Enumeration for different types of Links, This is critical
*              for processing chain to get implemented. prefix of LINK_X
*              indicates transfer link and LINK_P is for processing link
*/
typedef enum {
  LINK_X_FM_IN_FULL_TO_PART = 0,/**< Feature map full to part               */
  LINK_X_FC_IN_FULL_TO_ADV,     /**< Feature coeffs full to advance         */
  LINK_X_FC_IN_FULL_TO_STAGE,   /**< Feature coeffs full to intermediate    */
  LINK_X_FC_IN_ANY_TO_FINAL,    /**< Feature coeffs from any place to final */
  LINK_X_FM_OUT_PART_TO_FULL,   /**< During write out staging(part) to final*/
  LINK_X_FM_CTXT_ST,            /**< context store link                     */
  LINK_X_FM_CTXT_RST,           /**< context restore link                   */
  LINK_P_FM_IN,                 /**< Feature map in to processing           */
  LINK_P_FC_IN,                 /**< Feature coeffs in to processing        */
  LINK_P_FM_OUT,                /**< Feature mpa processing to output       */
  MAX_LINKS_PER_WL              /**< Maximum links per workload             */
} eLinktype;

/**
*  @enum       eJointType
*  @brief      Enum to differntiate b/w Joints attached to buf or proc node
*/
typedef enum {
  JOINT_TO_BUF_NODE = 0,
  JOINT_TO_PROC_NODE,
} eJointType;
/**
*  @enum       eJointUpdateDirType
*  @brief      Enum to indicate the movement direction of joint after
*              transfering minimum flow unit
*/
typedef enum
{
  JOINT_MOVE_HOR = 0,
  JOINT_MOVE_VER,
  JOINT_MOVE_HOR_VER
} eJointUpdateDirType;

/**<
*****************************************************************************
* @struct sJoint_t                                                          *
* @brief  This structure holds the infomration for joints                   *
*                                                                           *
* @param type       : Joint attached a proc node or buffer Node @eJointType *
* @param index      : Index in the data base to get the node to which joint *
*                     is attached                                           *
* @param offset     : offset within the node (only relevant for joints      *
*                     attached to buffer nodes)                             *
* @param superOffset: Some times joint move after completion of a entire    *
*                     flow. So instead of creating multiple joints and      *
*                     increase memory foot print this parameter tells that  *
*                     how much joint shall move after one flow completion   *
*                     finalOffset =  offset +  superOffset * flowIndex      *
*                     where flowIndex is maintained by processing chain     *
* @param updateDir :  Within a life cycle of flow, joint virtually moves to *
*                     some direction (HOR, VER or sometimes first in HOR    *
*                     and then in VER. So this variable indicates how does  *
*                     the joint move after each minimum flow unit           *
*                     Relevant for buf nodes, refer @eJointUpdateDirType    *
****************************************************************************/
typedef struct {
  int32_t type;
  int32_t index;
  int32_t offset;
  int32_t superOffset;
  int32_t updateDir;
}sJoint_t;


typedef enum
{
  FLOW_PIPEUP = 0,
  FLOW_PIPELINE,
  FLOW_PIPEDOWN,
  MAX_FLOW_STAGES
} eFlowStage;

typedef enum
{
  ZERO_D = 0,
  ONE_D,
  TWO_D,
  THREE_D,
  FOUR_D,
} eDim;

/**<
*******************************************************************************
* @struct sFlow_t                                                             *
* @brief  This structure holds the infomration for flow maintained by pipe    *
*         Information which flows b/w 2 nodes has certain properties like :   *
*         (A) How much is the handshake quantum                               *
*         (B) How many times                                                  *
*         (C) Some times the flow happens at certain periodicity              *
*                                                                             *
* @param size : Minimum flow unit indicated by a 2D array, where a flow unit  *
*               of width x height is represented. Since the flow can be       *
*               different initially to prime the sink node compared to steady *
*               state and then different at the end, so 3 stages are defined  *
*               and indicated by eFlowStage                                   *
*               unit : elements ( not bytes)                                  *
* @param freq : Indicates how many times the flow will happen, including all  *
*               stages indicated by eFlowStage                                *
* @param period : Indicates at what periodicty the flow will happen           *
******************************************************************************/
typedef struct {
  int32_t size[MAX_FLOW_STAGES][TWO_D];
  int32_t freq;
  int32_t period;
} sFlow_t;

/**<
*******************************************************************************
* @struct sLink_t                                                             *
* @brief  This structure defines the complete pipe, it has joints for both src*
*         and sink, joints has all the info to get connected to a node and    *
*         then the pipe flow properties are also defined in this object       *
* @param src    : Array of joints connected to multiple source                *
* @param sink   : Joint connected to sink                                     *
* @param flow   : Flow properties of pipe                                     *
* @param numSrc : Number of source joints                                     *
* @param state  : state of Link: Active ( non-zero) or passive (zero)         *
******************************************************************************/
typedef struct
{
  sJoint_t src[MAX_SRC];
  sJoint_t sink;
  sFlow_t  flow;
  int32_t  numSrc;
  int32_t  state;
} sLink_t;

/**** All structures and enums definiton related to Link/joints ends } */

/**<
*****************************************************************************
* @struct sWorkload_t                                                       *
* @brief  This structure holds the infomration for a single workload unit   *
* @param networkId : Id to indicaite that to which network workload belongs *
* @param layerId   : Id to indicaite that to which layer workload belongs   *
* @param link      : workload is represented by a list of  links            *
****************************************************************************/
typedef struct
{
  int32_t networkId;
  int32_t layerId;
  sLink_t link[MAX_LINKS_PER_WL];
} sWorkload_t;

#define MAX_BUF_NODES_PER_CORE  (MAX_BUF_PER_WL * MAX_WORKLOAD_PER_CORE)
#define MAX_PROC_NODES_PER_CORE (64) /**< Assuming type of processing (layers in context of TIDL) supported */

/**<
*****************************************************************************
* @struct sDataBase_t                                                       *
* @brief  This structure holds the data base of all key infomration to      *
*         represent a processing chain                                      *
* @param bufNodesDB  : Data base to hold all buffer nodes for a core        *
* @param procNodesDB : Data base to hold all processing nodes for a core    *
* @param workLoadDB  : Data base to hold all workload units for a core      *
****************************************************************************/
typedef struct
{
  sBufParams_t  bufNodesDB[MAX_BUF_NODES_PER_CORE];
  sProcParams_t procNodesDB[MAX_PROC_NODES_PER_CORE];
  sWorkload_t   workLoadDB[MAX_WORKLOAD_PER_CORE];
} sDataBase_t;

/**<
*****************************************************************************
* @struct sWorkloadGroup_t                                                  *
* @brief  Object for a group of workloads and its depenency information     *
* @param pWorkloadList : Pointer to workload list, for now considering that *
*                        we can manage the group having contiguous workloads*
*                        from workload data bse but if not then need to     *
/*                       this as a link list                                *
* @param numWorkload   : Number of workloads                                *
* @param id            : Uniqiue ID for thuis workload with below format    *
*                        CORENUM (4 bits)_(STAGE_4bits)_(GRPID_24bits)      *
*                        Lazy to define enums and macros for set and get    *
*                        but STAGE indicates if it is input (0),            *
*                        intermediate (1) or Final (2) workload group.      *
*                        Final (2) workload group means that output of it   *
*                        can be given to application and similarly 0 means  *
*                        that it is entry point and buffer is supplioed by  *
*                        application                                        *
* @param dependId      : Indicates that on which  work group does this work *
*                        group depend on. -1 indicates no-dependency        *
****************************************************************************/
typedef struct
{
  sWorkload_t  *pWorkloadList;
  int32_t      numWorkload;
  int32_t      id; 
  int32_t      dependId[MAX_CORES];
} sWorkloadGroup_t;


#define MAX_WORKLOAD_GRP_PER_CORE (16) //!< For now limiting workload group to be 16 per core
/**<
*****************************************************************************
* @struct sWorkLoadSuperGroup_t                                             *
* @brief  Object for entire workload                                        *
* @param workloadGrpList : List of workload groups scheduled per core       *
****************************************************************************/
typedef struct {
  sWorkloadGroup_t workloadGrpList[MAX_CORES][MAX_WORKLOAD_GRP_PER_CORE];
} sWorkloadSuperGroup_t;

/**<
****************************************************************************
* @struct sGraphCompilerOutArgs_t                                          *
* @brief  Output object from Graph Compiler                                *
* @param simConfig : Input configuration parameters                        *
* @param memorySizeRequirement : Memory size requirement per core          *
* @param superWorkload         : Complete workload dsitribution informatio *
* @param dataBase              : Data base being referred by workloads     *
****************************************************************************/
typedef struct
{
  sWorkloadSuperGroup_t superWorkload;
  sDataBase_t           dataBase;
} sGraphCompilerOutArgs_t;

/** @} */ // end of group networkAnalyzer

#endif
