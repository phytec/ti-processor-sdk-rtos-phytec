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
  *  @file     tidl_model_secure.c
  *  @brief    This file is kind of main file for this module
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

void TIDL_play(sTIDL_Network_t *pNet, int32_t mode) ;

void model_secure_usage()
{
  fprintf(stderr, "\n");
  fprintf(stderr, "Secures TIDL model, accepts net binary file and provides a file with post fix _secure\n");
  fprintf(stderr, "Usage: tidl_model_secure <network file name> <secure_byte>\n");
  fprintf(stderr, "tidl_model_secure             = Executable name\n");
  fprintf(stderr, "network file name <mandatory> = Name of the input network file\n");
  fprintf(stderr, "secure_byte <optional>        = Byte to be used as key for enabling model security.\n\
  Value of 0 doesn't perform any security. any non-zero value will perform security.\n\
  After applying security model can not be parsed and debugged\n");
  return;
}

int32_t filelength(FILE *fp)
{
  int32_t len;
  if(fp == NULL) return 0;
  fseek(fp, 0, SEEK_END);
  len = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  return len;
}

static void print_model_header(const sTIDL_Network_t * pNet)
{
  fprintf(stdout, "netVersion = 0x%0X\n", pNet->netVersion);
  fprintf(stdout, "deviceName = %d\n", pNet->deviceName);
  fprintf(stdout, "network offset = %d and size = %d\n", 
  pNet->netBufInfo[TIDL_NET_STRUCTURE_BUF].offset, 
  pNet->netBufInfo[TIDL_NET_STRUCTURE_BUF].size);
  return ;
}

int32_t main(int32_t argc, char *argv[])
{
  int32_t ret_status = 0 ;
  int8_t secure_byte = 0x96;
  int32_t srcFileLength = 0 ;
  int32_t operatingMode = 0 ; //0:secure, 0x12: Reverse Secure
  sTIDL_Network_t  *pNetwork   ;
  void *pSrc = NULL ;
  char sourceFileName[1024], dstFileName[1024];
  const char *postFix = "_secure";
  const char *postFix2 = "_nonsecure";

  if(argc<2)
  {
    model_secure_usage();
    ret_status = -1 ;
  }
  if(argc > 2)
  {
    secure_byte = atoi(argv[2]); //Secure Byte
    if(secure_byte == 0){
      fprintf(stdout, "security Byte is provided as *ZERO*, will not perfrom any security\n");
      model_secure_usage();
    }
  }
  if(argc > 3)
  {
    int32_t nonSecureMode = atoi(argv[3]); //Mode to undo security - not documented, only for test purpose
    if(nonSecureMode == 0x12E5EC12){ //RESECUR : 12(R)E5EC(ESEC) (U) 12(R) //317058066
      operatingMode = 0x12 ;
    }
  }
  if(ret_status == 0){
    /* Read the Network from file to structure */
    strcpy(sourceFileName,argv[1]);
    if(operatingMode == 0)
    {
      sprintf(dstFileName,"%s%s",sourceFileName,postFix);
      fprintf(stdout, "Performing security on model file %s and generated file will be %s\n",
      sourceFileName, dstFileName);
    }
    else if(operatingMode == 0x12)
    {
      char *ptr = strstr(sourceFileName,postFix);
      if(ptr) {
        strcpy(dstFileName,sourceFileName);
        ptr = strstr(dstFileName,postFix);
        *ptr = '\0';
      }
      else
      {
        sprintf(dstFileName,"%s%s",sourceFileName,postFix2);
      }
      fprintf(stdout, "Performing reverse security on model file %s and generated file will be %s\n",
      sourceFileName, dstFileName);
    }
    //Read file
    {
      FILE * fptr;
      fptr = fopen((const char *)sourceFileName, "rb");
      if(fptr)
      {
        srcFileLength = filelength(fptr);
        pSrc = malloc(srcFileLength);
        fread(pSrc,1,srcFileLength, fptr);
        fclose(fptr);
      }
      else
      {
        fprintf(stderr,"Could Not Open Files %s\n",srcFileLength);
        srcFileLength = 0; ret_status = -1 ;
        model_secure_usage();
      }
    }
    pNetwork = (sTIDL_Network_t*) pSrc ;
    print_model_header(pNetwork);
  }

  if(ret_status != -1){
    void *ptr = (void*)((uint8_t *)(pSrc) + pNetwork->netBufInfo[TIDL_NET_STRUCTURE_BUF].offset) ;
    int32_t size = pNetwork->netBufInfo[TIDL_NET_STRUCTURE_BUF].size ;
    //Write data in destination file
    FILE *fp = fopen(dstFileName,"wb");

    if(operatingMode == 0x12)
    {
      TIDL_play(pNetwork, 1);
    }
    else
    {
      pNetwork->reserved = secure_byte ;
      TIDL_play(pNetwork, 0);
    } 
    if(fp == NULL)
    {
      fprintf(stderr,"Couldn't open file %s for writing, returning without generating output file\n", dstFileName);
      ret_status = -1 ;
    }
    else
    {
      fwrite(ptr,1,srcFileLength,fp);
    }
  }
  if(pSrc) free(pSrc);
  return ret_status ;
}
