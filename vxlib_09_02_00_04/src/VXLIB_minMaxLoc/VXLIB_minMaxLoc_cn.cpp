/******************************************************************************
 * Copyright (C) 2023 Texas Instruments Incorporated - http://www.ti.com/
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

/**********************************************************************************************************************/
/*                                                                                                                    */
/* INCLUDES                                                                                                           */
/*                                                                                                                    */
/**********************************************************************************************************************/

#include "VXLIB_minMaxLoc_priv.h"

/**********************************************************************************************************************/
/*                                                                                                                    */
/* VXLIB_minMaxLoc_exec_cn */
/*                                                                                                                    */
/**********************************************************************************************************************/

// this method computes add in via natural C code
template <typename dType>
VXLIB_STATUS VXLIB_minMaxLoc_exec_cn(VXLIB_kernelHandle handle,
                                     void *restrict     pIn,
                                     void *restrict     pMinVal,
                                     void *restrict     pMaxVal,
                                     void *restrict     pMinCount,
                                     void *restrict     pMaxCount,
                                     void *restrict     pMinLocCapacity,
                                     void *restrict     pMaxLocCapacity,
                                     void *restrict     pMinLoc,
                                     void *restrict     pMaxLoc,
                                     void *restrict     pStartX,
                                     void *restrict     pStartY)
{
   VXLIB_DEBUGPRINTFN(0, "%s\n", "Entering function");

   VXLIB_STATUS status = VXLIB_SUCCESS; // assign status to success by default

   // typecast handle (void) to struct pointer type associated to kernel
   VXLIB_minMaxLoc_PrivArgs *pKerPrivArgs = (VXLIB_minMaxLoc_PrivArgs *) handle;

   // create local pointers
   dType restrict *pInLocal          = (dType *) pIn;
   dType restrict *pMinValLocal      = (dType *) pMinVal;
   dType restrict *pMaxValLocal      = (dType *) pMaxVal;
   uint32_t restrict *pMinCountLocal = (uint32_t *) pMinCount;
   uint32_t restrict *pMaxCountLocal = (uint32_t *) pMaxCount;
   uint32_t restrict *pMinLocLocal   = (uint32_t *) pMinLoc;
   uint32_t restrict *pMaxLocLocal   = (uint32_t *) pMaxLoc;
   uint32_t restrict *pStartXLocal   = (uint32_t *) pStartX;
   uint32_t restrict *pStartYLocal   = (uint32_t *) pStartY;
   uint32_t restrict *minLocCapacity = (uint32_t *) pMinLocCapacity;
   uint32_t restrict *maxLocCapacity = (uint32_t *) pMaxLocCapacity;

   //  printf("NAT maxLocCapacity: %u\n", *maxLocCapacity);

   // obtain dimensional parameters
   uint32_t width            = pKerPrivArgs->width;
   uint32_t height           = pKerPrivArgs->height;
   uint32_t strideInElements = pKerPrivArgs->strideInElements;
   uint32_t x, y, srcIndex;
   {
      dType    min    = *pMinValLocal;
      dType    max    = *pMaxValLocal;
      uint32_t mincnt = 0;
      uint32_t maxcnt = 0;
      if (pMinCountLocal) {
         mincnt = *pMinCountLocal;
      }
      if (pMaxCountLocal) {
         maxcnt = *pMaxCountLocal;
      }

      for (y = 0; y < height; y++) {

         for (x = 0; x < width; x++) {

            srcIndex = (y * strideInElements) + x;

            if (pInLocal[srcIndex] > max) {
               max    = pInLocal[srcIndex];
               maxcnt = 1;
               if (pMaxLocLocal) {
                  pMaxLocLocal[0] = x + (*pStartXLocal);
                  pMaxLocLocal[1] = y + (*pStartYLocal);
               }
            }
            else if (pInLocal[srcIndex] == max) {
               if (pMaxLocLocal && maxcnt < *maxLocCapacity) {
                  pMaxLocLocal[2 * maxcnt]       = x + (*pStartXLocal);
                  pMaxLocLocal[(2 * maxcnt) + 1] = y + (*pStartYLocal);
               }
               ++maxcnt;
            }
            else {
               /* Do nothing */
            }

            if (pInLocal[srcIndex] < min) {
               min    = pInLocal[srcIndex];
               mincnt = 1;
               if (pMinLocLocal) {
                  pMinLocLocal[0] = x + (*pStartXLocal);
                  pMinLocLocal[1] = y + (*pStartYLocal);
               }
            }
            else if (pInLocal[srcIndex] == min) {
               if (pMinLocLocal && mincnt < *minLocCapacity) {
                  pMinLocLocal[2 * mincnt]     = x + (*pStartXLocal);
                  pMinLocLocal[2 * mincnt + 1] = y + (*pStartYLocal);
               }
               ++mincnt;
            }
            else {
               /* Do nothing */
            }
         }
      }

      *pMinValLocal = min;
      *pMaxValLocal = max;
      if (pMinCountLocal) {
         *pMinCountLocal = mincnt;
      }
      if (pMaxCountLocal) {
         *pMaxCountLocal = maxcnt;
      }
   }

   return (status);
}

/**********************************************************************************************************************/
/*                                                                                                                    */
/* Explicit instantiation for the different data type versions                                                        */
/*                                                                                                                    */
/**********************************************************************************************************************/

template VXLIB_STATUS VXLIB_minMaxLoc_exec_cn<VXLIB_MINMAXLOC_TYPENAME_8U>(VXLIB_kernelHandle handle,
                                                                           void *restrict     pIn,
                                                                           void *restrict     pMinVal,
                                                                           void *restrict     pMaxVal,
                                                                           void *restrict     pMinCount,
                                                                           void *restrict     pMaxCount,
                                                                           void *restrict     pMinLocCapacity,
                                                                           void *restrict     pMaxLocCapacity,
                                                                           void *restrict     pMinLoc,
                                                                           void *restrict     pMaxLoc,
                                                                           void *restrict     pStartX,
                                                                           void *restrict     pStartY);

template VXLIB_STATUS VXLIB_minMaxLoc_exec_cn<VXLIB_MINMAXLOC_TYPENAME_8S>(VXLIB_kernelHandle handle,
                                                                           void *restrict     pIn,
                                                                           void *restrict     pMinVal,
                                                                           void *restrict     pMaxVal,
                                                                           void *restrict     pMinCount,
                                                                           void *restrict     pMaxCount,
                                                                           void *restrict     pMinLocCapacity,
                                                                           void *restrict     pMaxLocCapacity,
                                                                           void *restrict     pMinLoc,
                                                                           void *restrict     pMaxLoc,
                                                                           void *restrict     pStartX,
                                                                           void *restrict     pStartY);

template VXLIB_STATUS VXLIB_minMaxLoc_exec_cn<VXLIB_MINMAXLOC_TYPENAME_16U>(VXLIB_kernelHandle handle,
                                                                            void *restrict     pIn,
                                                                            void *restrict     pMinVal,
                                                                            void *restrict     pMaxVal,
                                                                            void *restrict     pMinCount,
                                                                            void *restrict     pMaxCount,
                                                                            void *restrict     pMinLocCapacity,
                                                                            void *restrict     pMaxLocCapacity,
                                                                            void *restrict     pMinLoc,
                                                                            void *restrict     pMaxLoc,
                                                                            void *restrict     pStartX,
                                                                            void *restrict     pStartY);

template VXLIB_STATUS VXLIB_minMaxLoc_exec_cn<VXLIB_MINMAXLOC_TYPENAME_16S>(VXLIB_kernelHandle handle,
                                                                            void *restrict     pIn,
                                                                            void *restrict     pMinVal,
                                                                            void *restrict     pMaxVal,
                                                                            void *restrict     pMinCount,
                                                                            void *restrict     pMaxCount,
                                                                            void *restrict     pMinLocCapacity,
                                                                            void *restrict     pMaxLocCapacity,
                                                                            void *restrict     pMinLoc,
                                                                            void *restrict     pMaxLoc,
                                                                            void *restrict     pStartX,
                                                                            void *restrict     pStartY);
/* ======================================================================== */
/*  End of file:  VXLIB_minMaxLoc_cn.cpp                                    */
/* ======================================================================== */
