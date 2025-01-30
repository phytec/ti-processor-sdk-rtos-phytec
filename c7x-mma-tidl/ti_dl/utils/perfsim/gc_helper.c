/*
*******************************************************************************
*
* Module Name: Graph compiler - Primarilly designed for execution plan for DNN
* based graphs, but should scale for any graph based execution planner
*
* "Performance simulator - Predicts time for overall graph execution for TI's
* C7x + MMA processor architecture. It models the performance of different
* processing elements of graph and provides a summarized output of performance
*
* Copyright (C) 2015-21 Texas Instruments Incorporated - http://www.ti.com/
* ALL RIGHTS RESERVED
*******************************************************************************
*/

/**
******************************************************************************
*  @file     gc_helper.c
*
*  @brief    Implementation of helper function module for GC,
*            can be used by client of GC module
*
*  @author   PKS
*
*  @version  0.1 - Sep 2021: Initial Version [PKS]
*
*****************************************************************************
*/

#include "gc_helper.h"
#include "string.h"
#include <math.h>

sWorkloadUnit_t* gWLUnitDBHandle;
int32_t   gWLUnitCnt;
sWorkloadUnitAuxilary_t* gWLAuxUnitDBHandle;
int32_t   gWLAuxUnitCnt;

//private
typedef struct
{
  int32_t numSplit : 8;
  int32_t tileType : 4;
  int32_t inTileHeight : 20;
  int32_t outTileHeight : 20;
  int32_t passiveLinkIndex : 4;
  int32_t isFirstTile : 2;
  int32_t isLastTile  : 2;
  int32_t rsvd : 4;
} sIntWLMetaDataID_t;

typedef struct
{
  int64_t numSplitB : 8;
  int64_t rsvd : 56;
} sIntWLMetaDataIDExt_t;

typedef struct
{
  int64_t networkId : 4;
  int64_t layerId : 16;
  int64_t subLayerId : 4;
  int64_t type : 5;
  int64_t dependentLayerId : 16;
  int64_t tileType : 4;
  int64_t multiBufIndex : 2;
  int64_t rsvd : 13;
} sIntMetaDataID_t;


int64_t getWLId(const sWLMetaDataID_t * pWLMetaDataID)
{
  sIntWLMetaDataID_t * psWLIntMetaDataID;
  int64_t WLId;
  psWLIntMetaDataID = (sIntWLMetaDataID_t*)&WLId;
  psWLIntMetaDataID->numSplit = pWLMetaDataID->numSplit;
  psWLIntMetaDataID->tileType = pWLMetaDataID->tileType;
  psWLIntMetaDataID->inTileHeight = pWLMetaDataID->inTileHeight;
  psWLIntMetaDataID->outTileHeight = pWLMetaDataID->outTileHeight;
  psWLIntMetaDataID->passiveLinkIndex =
                   pWLMetaDataID->passiveLinkIndex;
  psWLIntMetaDataID->isFirstTile =
                   pWLMetaDataID->isFirstTile;
  psWLIntMetaDataID->isLastTile =
                   pWLMetaDataID->isLastTile;
  psWLIntMetaDataID->rsvd = 0;

  return WLId;
}

int64_t getWLIdExt(const sWLMetaDataIDExt_t * pWLMetaDataIDExt)
{
  sIntWLMetaDataIDExt_t * psWLIntMetaDataIDExt;
  int64_t WLId;
  psWLIntMetaDataIDExt = (sIntWLMetaDataIDExt_t*)&WLId;
  psWLIntMetaDataIDExt->numSplitB = pWLMetaDataIDExt->numSplitB;
  psWLIntMetaDataIDExt->rsvd = 0;

  return WLId;
}

void getWLMetaDataID(int64_t WLId, sWLMetaDataID_t *pWLMetaDataID)
{
  sIntWLMetaDataID_t *psIntWLMetaDataID;
  psIntWLMetaDataID = (sIntWLMetaDataID_t*)&WLId;
  pWLMetaDataID->numSplit = psIntWLMetaDataID->numSplit;
  pWLMetaDataID->tileType = psIntWLMetaDataID->tileType;
  pWLMetaDataID->inTileHeight = psIntWLMetaDataID->inTileHeight;
  pWLMetaDataID->outTileHeight = psIntWLMetaDataID->outTileHeight;
  pWLMetaDataID->passiveLinkIndex =
                psIntWLMetaDataID->passiveLinkIndex;
  pWLMetaDataID->isFirstTile = 
                psIntWLMetaDataID->isFirstTile;
  pWLMetaDataID->isLastTile = 
                psIntWLMetaDataID->isLastTile;

  return;
}

void getWLMetaDataIDExt(int64_t WLId, sWLMetaDataIDExt_t *pWLMetaDataIDExt)
{
  sIntWLMetaDataIDExt_t *psIntWLMetaDataIDExt;
  psIntWLMetaDataIDExt = (sIntWLMetaDataIDExt_t*)&WLId;
  pWLMetaDataIDExt->numSplitB = psIntWLMetaDataIDExt->numSplitB;

  return;
}
int64_t getId(const sMetaDataID_t *pMetaDataID)
{
  sIntMetaDataID_t *psIntMetaDataID;
  int64_t id;
  psIntMetaDataID = (sIntMetaDataID_t*)&id;
  psIntMetaDataID->networkId = pMetaDataID->networkId;
  psIntMetaDataID->layerId = pMetaDataID->layerId;
  psIntMetaDataID->subLayerId = pMetaDataID->subLayerId;
  psIntMetaDataID->type = pMetaDataID->type;
  psIntMetaDataID->dependentLayerId =
                          pMetaDataID->dependentLayerId;
  psIntMetaDataID->multiBufIndex =
                          pMetaDataID->multiBufIndex;
  psIntMetaDataID->tileType = pMetaDataID->tileType;
  psIntMetaDataID->rsvd = 0;

  return id;

}
void getMetaDataID(int64_t id, sMetaDataID_t *pMetaDataID)
{
  sIntMetaDataID_t *psIntMetaDataID;
  psIntMetaDataID = (sIntMetaDataID_t*)&id;
  pMetaDataID->networkId = psIntMetaDataID->networkId;
  pMetaDataID->layerId = psIntMetaDataID->layerId;
  pMetaDataID->subLayerId = psIntMetaDataID->subLayerId;
  pMetaDataID->type = psIntMetaDataID->type;
  pMetaDataID->dependentLayerId =
                      psIntMetaDataID->dependentLayerId;
  pMetaDataID->multiBufIndex =
                      psIntMetaDataID->multiBufIndex;
  // some issue with how the int to MetaData casting happens , causing all zero binary bits to give -1 in decimal
  pMetaDataID->tileType = psIntMetaDataID->tileType < NO_TILE ? NO_TILE : psIntMetaDataID->tileType;

  return;
}

int64_t getDataIdFromLinkId(int64_t linkId, int32_t bufType)
{
  int64_t dataId;
  sMetaDataID_t metaDataId;
  getMetaDataID(linkId, &metaDataId);
  metaDataId.type = bufType;
  //dependent layer ID needs to be set to NOT_VALID before searching bufferDB with the dataID
  metaDataId.dependentLayerId = NOT_VALID;
  // all buffers except BUF_COEFF_FULL are created with tiletype=NOT_TILE as default
  // if this func is to be used for BUFF_COEFF_FULL , will have to make changes
  metaDataId.tileType = NO_TILE;
  dataId = getId(&metaDataId);
  return dataId;
}

void initGCHelperHandle(sGCHelperHandle * gcHelperHandle,
                               const sTIDL_Network_t *pNet,
                               sGCDataBaseInfo_t *pGCDataBaseInfo,
                               sGCCommonDataBaseInfo_t *pGCCommonDataBaseInfo)
{
  if ( (pNet != NULL ) && (pGCDataBaseInfo != NULL ) && (pGCCommonDataBaseInfo != NULL ) )
  {
    gcHelperHandle->pNet     = pNet;
    gcHelperHandle->pBufDB   = &pGCCommonDataBaseInfo->bufDB;
    gcHelperHandle->pWLDB    = &pGCDataBaseInfo->wlDB;
    gcHelperHandle->pAuxWLDB = &pGCDataBaseInfo->wlAuxDB;
  }

  return;
}


const sWorkloadUnit_t* getWLUnit(sGCHelperHandle * gcHelperHandle, int32_t linkId)
{
   const sWorkloadUnit_t* pWL = NULL, *pWLList = gcHelperHandle->pWLDB->pWLList;
  if (pWLList != NULL)
  {
    sWorkloadUnit_t* pWLCur = NULL ;
    sLink_t *pLink ;
    int32_t wlUnitCnt = gcHelperHandle->pWLDB->count;
    while (wlUnitCnt--) {
      pWLCur = getWLUnitPtr(gcHelperHandle, wlUnitCnt);
      for (int32_t linkNum = 0; (pWLCur != NULL) && (linkNum < pWLCur->numLinks); linkNum++) {
        pLink   = getLinkPtr(pWLCur, NOT_VALID, linkNum);
        if (pLink->id == linkId) {
          pWL = pWLCur;  break;
        }
      }
      if (pWL != NULL) break;
    }
  }
  return pWL;
}

sBufParams_t * getBufParamsFromBufIndex(const sGCHelperHandle * gcHelperHandle, int32_t bufIndex)
{
  sBufParams_t* pBufParam = NULL;
  sBufParams_t* pBufList = gcHelperHandle->pBufDB->pBufList;
  if(bufIndex != NOT_VALID)
  {
    if (pBufList != NULL)
    {
      pBufParam = &pBufList[bufIndex];
    }
  }
  return pBufParam;
}

int32_t getLinkIdx(const sWorkloadUnit_t *pWL, int32_t linkSubType){
  int32_t linkIdx = NOT_VALID ;
  const sLink_t *pLink = NULL;
  for (int32_t linkNum = 0; (pWL != NULL) && (linkNum < pWL->numLinks); linkNum++) {
    pLink = getLinkPtr(pWL, NOT_VALID,linkNum);
    if (pLink->subType == linkSubType) {
      linkIdx = linkNum ;
      break;
    }
  }
  return linkIdx;
}

const sLink_t *getProcLink(sGCHelperHandle * gcHelperHandle, const sLink_t *pLinkUser, int32_t linkId) {
  const sLink_t *pLink = pLinkUser;
  linkId = (pLink == NULL) ? linkId : pLink->id;
  if (pLink == NULL || pLink->subType != LINK_P)
    pLink = getLinkPtr(getWLUnit(gcHelperHandle, linkId), LINK_P, NOT_VALID);
  return pLink;
}

const sTIDL_Layer_t *getLayerInfo(sGCHelperHandle * gcHelperHandle,const sLink_t *pLinkUser,
  int32_t linkId)
{
  const sTIDL_Layer_t *PTIDLLayer = NULL;
  const sLink_t *pLink = NULL;
  sMetaDataID_t sMetaDataID;

  pLink = pLinkUser == NULL ? getProcLink(gcHelperHandle, pLinkUser, linkId) : pLinkUser;
  if (pLink != NULL)
  {
    getMetaDataID(pLinkUser->id, &sMetaDataID);
    if (gcHelperHandle->pNet)
      PTIDLLayer = &gcHelperHandle->pNet->TIDLLayers[sMetaDataID.layerId];
  }
  return PTIDLLayer;
}

int32_t getCountOfConsumers(sWorkloadUnit_t* WLUnitDB, int32_t bufDBindex)
{
  int32_t consumerCount = 0;

  #if 0

  int32_t i,j,k;
  sWorkloadUnit_t* pWLUnit;
  sLink_t * pLink;
  for(i= 0 ;i < getWLCount(); i++)
  {
    pWLUnit = &(WLUnitDB[i]);
    for(j=0; j<pWLUnit->numLinks; j++)
    {
      pLink   = getLinkPtr(pWLUnit, NOT_VALID, j);
      if(pLink->subType == LINK_X_FM_IN_FULL_TO_PART)
      {
        for(k=0 ; k < pLink->numSrc ; k++)
        {
          if(pLink->src[k].bufDBindex == bufDBindex) consumerCount++;
        }
      }
    }
  }
  #endif

  return consumerCount;
}

int32_t isPaddingRequired(sWorkloadUnit_t* WLUnitDB, int32_t Wlidx, int32_t bufDBindex)
{
  int32_t padRequired = 0;
  // sWorkloadUnit_t *pWLUnit = &(WLUnitDB[Wlidx]);
  // sBufParams_t *Buff = indexIntoBufDB(bufDBindex);

  // GetPad(pLayer, bufType, REG_ACTUAL, LEFT, PIXELS);
  return padRequired;
}


void initWLDB(sGCDataBase_t *pGCDataBase)
{
  pGCDataBase->dbParams[DB_WL].numElements = 0;
  pGCDataBase->dbParams[DB_WL].type = DB_WL;
  pGCDataBase->dbParams[DB_WL].size = 0;
  pGCDataBase->dbParams[DB_WL].startOffset = pGCDataBase->dbSize;
  pGCDataBase->dbParams[DB_WL].type = DB_WL;
  gWLUnitDBHandle = (sWorkloadUnit_t*)&pGCDataBase->dbPayLoad[pGCDataBase->dbParams[DB_WL].startOffset];
  gWLUnitCnt = 0;
}
void initWLOffset(sGCHelperHandle* gcHelperHandle)
{
  gcHelperHandle->pWLDB->wlOffset[0] = 0;
  
  for(int32_t i = 1;i < MAX_WORKLOAD_PER_CORE;i++)
  {
    gcHelperHandle->pWLDB->wlOffset[i] = NOT_VALID;
  }
}

void deInitWLOffset(sGCHelperHandle* gcHelperHandle)
{
  int wlCount = getWLCount();
  gcHelperHandle->pWLDB->wlOffset[wlCount] = NOT_VALID;
}

void deInitWLDB()
{
  return;
}
int32_t getWLCount()
{
  return gWLUnitCnt;
}

void incrementWLCnt()
{
  gWLUnitCnt++;
}

sWorkloadUnit_t * getWLUnitPtr(sGCHelperHandle * gcHelperHandle, int32_t unitIdx){
  sWorkloadUnit_t *pWLUnit = NULL;
#if 0
  if(unitIdx>=0) pWLUnit = gWLUnitDBHandle + unitIdx ;
#else
   if(unitIdx >= 0 && gcHelperHandle->pWLDB->wlOffset[unitIdx] != NOT_VALID)/* for unitIdx < 0 or unitIndx > number of workloadsreturn NULL*/
  {
    int32_t offset = gcHelperHandle->pWLDB->wlOffset[unitIdx]  ;
    pWLUnit = (sWorkloadUnit_t *)((int8_t *)gcHelperHandle->pWLDB->pWLList + offset) ;
  }
#endif
  return pWLUnit ;
}

int32_t getWLUnitSize(sGCHelperHandle * gcHelperHandle, int32_t unitIdx)
{
  int32_t size = gcHelperHandle->pWLDB->wlOffset[unitIdx + 1] - gcHelperHandle->pWLDB->wlOffset[unitIdx];
  return size;
}

void initWLAuxDB(sGCDataBase_t *pGCDataBase, int32_t offset)
{
  pGCDataBase->dbParams[DB_AUXWL].numElements = 0;
  pGCDataBase->dbParams[DB_AUXWL].type = DB_WL;
  pGCDataBase->dbParams[DB_AUXWL].size = 0;
  pGCDataBase->dbParams[DB_AUXWL].startOffset = offset;
  pGCDataBase->dbParams[DB_AUXWL].type = DB_AUXWL;
  gWLAuxUnitDBHandle = (sWorkloadUnitAuxilary_t*)&pGCDataBase->dbPayLoad[pGCDataBase->dbParams[DB_AUXWL].startOffset];
  gWLAuxUnitCnt = 0;
}
void deInitWLAuxDB()
{
  gWLAuxUnitDBHandle = NULL;
  gWLAuxUnitCnt = 0;
}

int32_t getAuxWLCount()
{
  return gWLAuxUnitCnt;
}

sWorkloadUnitAuxilary_t * getAuxWLUnitPtr(int32_t unitIdx){
  sWorkloadUnitAuxilary_t *pAuxWLUnit = NULL;
  if(unitIdx>=0) pAuxWLUnit = gWLAuxUnitDBHandle + unitIdx ;
  return pAuxWLUnit ;
}

void incrementAuxWLCnt()
{
  gWLAuxUnitCnt++;
}

sLink_t * getLinkPtr(const sWorkloadUnit_t *pWLUnit, int32_t linkSubType, int32_t linkIdx){
  sLink_t* pLink = NULL ;
  int32_t offset;
  if(linkSubType != NOT_VALID){ //Query is based on linkSubType so need to find linkIdx
    linkIdx = getLinkIdx(pWLUnit,linkSubType);
  }
  if(linkIdx != NOT_VALID){
    if(linkIdx ==0){
      offset = sizeof(sWorkloadUnit_t) - (sizeof(sLink_t) * NUM_LINKS_PER_WL_STATIC);
    }
    else
    {
      offset = pWLUnit->linkOffset[linkIdx]  ;
    }
    pLink = (sLink_t *)((int8_t *)pWLUnit + offset) ;
  }

  return pLink ;
}

sJoint_t * getSrcJointPtr(sLink_t *pLink, int32_t jointIdx){
  sJoint_t* pJoint = &pLink->src[jointIdx];
  return pJoint;
}

static void getLastLinkInfo(sWorkloadUnit_t *pWLUnit, int32_t* pSizeOfLink, int32_t* pLinkIdx)
{
  //sWorkloadUnit_t
  //  xxx
  //  xxx
  //  link0 (sLink_t)
  //     xxx
  //     xxx
  //     src0(sJoint_t)
  //     src1(sJoint_t)
  //  link1 (sLink_t)
  //  ...
  sLink_t *pLink;
  int32_t numSrcInLink, linkIdx, sizeOfLink ;
  int32_t sizeofLinkWithOneSrc      = sizeof(sLink_t) - (sizeof(sJoint_t) * (NUM_SRC_STATIC - 1)) ;

  linkIdx = pWLUnit->numLinks - 1;
  pLink   = getLinkPtr(pWLUnit, NOT_VALID, linkIdx);
  numSrcInLink = pLink->numSrc ;
  sizeOfLink = sizeofLinkWithOneSrc + (numSrcInLink - 1)*sizeof(sJoint_t);
  if(pSizeOfLink) *pSizeOfLink = sizeOfLink;
  if(pLinkIdx) *pLinkIdx = linkIdx;

  return ;
}

int32_t getLinkSize(sWorkloadUnit_t *pWLUnit, int32_t linkSubType, int32_t linkIdx){
  int32_t linkSize = 0;

  if(linkSubType != NOT_VALID){ //Query is based on linkSubType so need to find linkIdx
    linkIdx = getLinkIdx(pWLUnit,linkSubType);
  }

  if(linkIdx != NOT_VALID){
    if(linkIdx == pWLUnit->numLinks-1)
    {
      getLastLinkInfo(pWLUnit, &linkSize, &linkIdx);
    }
    else
    {
    linkSize =  pWLUnit->linkOffset[linkIdx + 1] - pWLUnit->linkOffset[linkIdx];
    }
  }

  return linkSize;
}

void copyLink(sLink_t * pLinkSrc, sLink_t * pLinkDst,int32_t linkSize){
  if(pLinkSrc == NULL || pLinkDst == NULL)
  {
    return;
  }

  memcpy(pLinkDst, pLinkSrc, linkSize);
  return;
}

void updateBufHeader()
{

}
void updateWLHeader(sGCHelperHandle * gcHelperHandle)
{
  //enters when a lnk is addedd to workload, wlCount is not incremented and numLinks is incremented(link is already added)

  int32_t sizeofWorkloadWithoutLink = sizeof(sWorkloadUnit_t) - (sizeof(sLink_t) * NUM_LINKS_PER_WL_STATIC);
  int32_t sizeOfLink;
  int32_t linkIdx, wlIdx   ;
  int32_t prevWlIdx, sizeOfPrevWL, prevWLOffset ;
  sWorkloadUnit_t *pWLCur = NULL;

  wlIdx = getWLCount(); //At this time count doesn't include current workload so count can be used as index variable

  //Collect info for previous workload
  prevWlIdx = wlIdx - 1 ;
  if(prevWlIdx >=0)
  {
    sWorkloadUnit_t *pWLPrev = getWLUnitPtr(gcHelperHandle, prevWlIdx);
    prevWLOffset = gcHelperHandle->pWLDB->wlOffset[prevWlIdx] ;
    getLastLinkInfo(pWLPrev, &sizeOfLink, &linkIdx);
    sizeOfPrevWL  =  pWLPrev->linkOffset[linkIdx] + sizeOfLink;
  }
  else
  {
    sizeOfPrevWL = 0 ;
    prevWLOffset = 0 ;
  }

  pWLCur = getWLUnitPtr(gcHelperHandle, wlIdx);
  getLastLinkInfo(pWLCur, &sizeOfLink, &linkIdx);

  if(linkIdx ==0)
  {
    pWLCur->linkOffset[linkIdx] = sizeofWorkloadWithoutLink ;
    gcHelperHandle->pWLDB->wlOffset[wlIdx] = prevWLOffset + sizeOfPrevWL ;
 }
  //Size of Link is for current link so need to update linkOffset for next link
  if(linkIdx < MAX_LINKS_PER_WL)
  {
    int32_t sizeOfWLCur = sizeOfLink + pWLCur->linkOffset[linkIdx];
    pWLCur->linkOffset[linkIdx + 1] = sizeOfWLCur ;
    gcHelperHandle->pWLDB->wlOffset[wlIdx + 1] = gcHelperHandle->pWLDB->wlOffset[wlIdx] + sizeOfWLCur ;

  }

  return;
}

void getLayerIdInit(void * getIdContext,
                              int32_t coreId,
                              int32_t layerGroupId,   //Needed for backwards compatibility
                              sTIDL_Network_t       *net,
                              sGCHelperHandle  *gcHelperHandle,
                              sWorkloadSuperGroup_t *wlSuperGroup)
{
  sGetLayerIdContext_t *context = (sGetLayerIdContext_t*)getIdContext;

  context->groupIdx    = 0;
  context->subGroupIdx = 0;
  context->countIdx    = 0;
  context->workLoadIdx = 0;
  context->algLayerIdx = 0;

  context->coreId       = coreId;
  context->layerGroupId = layerGroupId;
  context->net          = net;
  context->gcHelperHandle   = gcHelperHandle;
  context->wlSuperGroup = wlSuperGroup;

  if( wlSuperGroup != NULL)
  {
    context->workLoadIdx = wlSuperGroup->workloadGrpList[0].subGroup[0].startIndexWLUnitDB;
  }

  return;
}

int32_t getLayerIdToExecute(void * getIdContext,
                                         int32_t                isInit,
                                         int32_t                *wlRepeatIter,
                                         int32_t                *currAlgLayerIdx,
                                         sWorkloadUnit_t        **workLoadUnit)
{
  sGetLayerIdContext_t * context = (sGetLayerIdContext_t *)getIdContext;
  int32_t currAlgLayer = VALID;
  int32_t workLoadId;
  int32_t layerId;
  int32_t wlRepeat = 0;

  //:TODO: Need to handle going to old flow if supertiling is enabled

  if (context->wlSuperGroup != NULL )
  {

    const sWorkloadGroup_t * wlGroup =  &context->wlSuperGroup->workloadGrpList[context->groupIdx];
    const sWorkloadSubGroup_t * wlSubGroup = &wlGroup->subGroup[context->subGroupIdx];

    workLoadId = context->workLoadIdx;
    context->algLayerIdx = workLoadId;
    currAlgLayer = context->algLayerIdx;
    wlRepeat     = context->countIdx;

    if ( workLoadId != NOT_VALID)
    {
      context->workLoadIdx++;

      if (( context->workLoadIdx >= wlSubGroup->startIndexWLUnitDB ) &&
                  ( context->workLoadIdx <= wlSubGroup->endIndexWLUnitDB ))
      {
        /* Next iteration will be part of same subGroup hence do nothing */
      }
      else
      {
        context->countIdx++;
        if (( context->countIdx < wlSubGroup->count ) && (isInit == 0 ))
        {
          /* Count indicates that we have to repeat the same sub group again.
          Subtraction by 1 so that next time when we increment we get the right index */
          context->workLoadIdx = wlSubGroup->startIndexWLUnitDB;
        }
        else
        {
          context->countIdx = 0;
          context->subGroupIdx++;
          if ( context->subGroupIdx < wlGroup->numSubGroups )
          {
            const sWorkloadSubGroup_t * wlSubGroupNext = &wlGroup->subGroup[context->subGroupIdx];
            context->workLoadIdx = wlSubGroupNext->startIndexWLUnitDB;
          }
          else
          {
            context->groupIdx++;
            context->subGroupIdx = 0;
            if (context->groupIdx < context->wlSuperGroup->numGroups)
            {
              const sWorkloadGroup_t * wlGroup =  &context->wlSuperGroup->workloadGrpList[context->groupIdx];
              const sWorkloadSubGroup_t * wlSubGroupNext = &wlGroup->subGroup[0];
              context->workLoadIdx = wlSubGroupNext->startIndexWLUnitDB;
            }
            else
            {
              context->workLoadIdx = NOT_VALID;
              context->groupIdx = 0;
            }
          }
        }
      }
    }

    if ( workLoadId != NOT_VALID )
    {
      *workLoadUnit = getWLUnitPtr(context->gcHelperHandle, workLoadId);
      layerId      = (*workLoadUnit)->layerId;
    }
    else
    {
      *workLoadUnit = NULL;
      layerId      = NOT_VALID;
    }
  }
  else
  {
    /* Reference flow with GC information*/
    sTIDL_Layer_t *tidlLayer;

    *workLoadUnit = NULL;
    workLoadId = NOT_VALID;
    currAlgLayer = context->algLayerIdx;

    if ( context->workLoadIdx < context->net->numLayers)
    {
      layerId = context->workLoadIdx;
      context->workLoadIdx++;

      tidlLayer = &context->net->TIDLLayers[layerId];
      if(tidlLayer->layersGroupId == context->layerGroupId)
      {
        context->algLayerIdx++;
      }
    }
    else
    {
      layerId = NOT_VALID;
      context->algLayerIdx = NOT_VALID;
    }
  }

  if(currAlgLayerIdx != NULL)
  {
    *currAlgLayerIdx = currAlgLayer;
  }
  if(wlRepeatIter != NULL)
  {
    *wlRepeatIter = wlRepeat;
  }

  return layerId;
}

int32_t getNumSplits(const void *workloadHandle)
{
  sWLMetaDataID_t pWLMetaDataID;
  getWLMetaDataID(((const sWorkloadUnit_t*)workloadHandle)->metadata, &pWLMetaDataID);
  return pWLMetaDataID.numSplit;
}

int8_t * get_int8_t_pointer(int8_t arr[], int32_t offset)
{
    return &arr[offset];
}

void TIDL_factorizeBy64K(uint32_t icnt0, uint32_t icnt1, uint32_t *factorLarge, uint32_t *factorSmall)
{
  uint32_t maxFactor = (uint32_t)sqrt(icnt1);
  while(maxFactor > 1)
  {
    if((icnt1 % maxFactor == 0) && ((icnt0 * maxFactor) < ICNT_MAX_16_BIT))
    {
      break;
    }
    maxFactor--;
  }
  *factorLarge = icnt0 * maxFactor;
  *factorSmall = icnt1 / maxFactor;
}

int8_t * getGCDataBase(sGraphCompilerOutArgs_t *gcInfo, int32_t dataBaseType)
{
  int8_t * pointerToDataBase = NULL;
  pointerToDataBase = &gcInfo->dataBase[gcInfo->dataBaseRegionInfo[dataBaseType].offset];
  return pointerToDataBase;
}
