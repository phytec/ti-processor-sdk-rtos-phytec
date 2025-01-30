/*
 *******************************************************************************
 *
 * Module Name: TIDL Model Security
 *
 * "TIDL Model Secure" is software module developed
 *  for TI's C7x + MMA processor architecture. it perfroms model security
 *  on TIDL netowrk structure

 * Copyright (C) 2024 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *******************************************************************************
*/

/**
  ******************************************************************************
  *  @file     tidl_model_play.c
  *  @brief    Performs descrambling of model
  *  @author   PKS
  *  @version  0.1 - Mar 2024 : Initial Version [PKS]
  *
 *******************************************************************************
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "itidl_ti.h"
#include <string.h>

#define HEADER_SIZE (256)
#define LUTSIZE     (256)
#define OFFSET      (20)

static void TIDL_model_unsecure_data(void *ptr, int32_t size, int8_t secure_byte)
{
  int32_t size_to_operate = size - HEADER_SIZE ;
  uint8_t lookUpPattern[LUTSIZE] ;
  uint8_t offset = OFFSET ; 
  uint8_t *pData = (uint8_t*)ptr  + HEADER_SIZE;

  if(secure_byte == 0) offset = 0;
  for(int32_t i = 0 ; (i < LUTSIZE) ; i++)
  {
    lookUpPattern[((secure_byte + i + offset) & 0xFF)] = i  ;
  }
  for(int32_t i = 0 ; (i < size_to_operate) ; i++)
  {
    pData[i] = lookUpPattern[pData[i]];
  }
#if 0  
  printf("operation unsecure with offset = %d and securebyte = %d\n", offset, secure_byte);
  for(int32_t i = 0 ; (i < LUTSIZE) ; i++)
  {
    printf(" %3d,", lookUpPattern[i]);
  }
#endif  
  return ;
}

static void TIDL_model_secure_data(void *ptr, int32_t size, int8_t secure_byte)
{
  int32_t size_to_operate = size - HEADER_SIZE ;
  uint8_t lookUpPattern[LUTSIZE] ;
  uint8_t offset = OFFSET ; 
  uint8_t *pData = (uint8_t*)ptr  + HEADER_SIZE;

  if(secure_byte == 0) offset = 0;
  
  for(int32_t i = 0 ; (i < LUTSIZE) ; i++)
  {
    lookUpPattern[i] = ((secure_byte + i + offset) & 0xFF) ;
  }
  for(int32_t i = 0 ; (i < size_to_operate) ; i++)
  {
    pData[i] = lookUpPattern[pData[i]];
  }
#if 0  
  printf("operation secure with offset = %d and securebyte = %d\n", offset, secure_byte);
  for(int32_t i = 0 ; (i < LUTSIZE) ; i++)
  {
    printf(" %3d,", lookUpPattern[i]);
  }
#endif    
  return ;
}

//Keeping purposefully mode as not enum
void TIDL_play(sTIDL_Network_t *pNet, int32_t mode)
{
  int8_t secure_byte = pNet->reserved ;
  void *ptr = (void*)((uint8_t *)(pNet) + pNet->netBufInfo[TIDL_NET_STRUCTURE_BUF].offset) ;
  int32_t size = pNet->netBufInfo[TIDL_NET_STRUCTURE_BUF].size ;
#if 1
  if(mode == 0 && secure_byte !=0){
    TIDL_model_secure_data(ptr, size, secure_byte);
  }
  else if(mode == 1 && secure_byte !=0){
    TIDL_model_unsecure_data(ptr, size, secure_byte);
  }
#endif
  return ;
}
