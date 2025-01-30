/******************************************************************************/
/*!
 * \file MMA_debugPrintVReg.c
 */
/* Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
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


/*******************************************************************************
 *
 * INCLUDES
 *
 ******************************************************************************/
 
#include "../FFTLIB_types.h"
#include "../FFTLIB_bufParams.h"

/******************************************************************************/

/******************************************************************************/
/*!
 * \ingroup
 * \brief
 * \details
 * \return  void
 */
/******************************************************************************/

template <typename dataType>
void FFTLIB_debugPrint3DVReg_helper(dataType *matrix, const FFTLIB_bufParams3D_t *params)
{
   uint32_t r,c,t;
   dataType* xPtr;
   dataType* yPtr;
   dataType* zPtr = matrix;
   
   // stride_y is stored in bytes, but easier to use in elements
   uint32_t stride_y_elements = params->stride_y / FFTLIB_sizeof(params->data_type);
   uint32_t stride_z_elements = params->stride_z / FFTLIB_sizeof(params->data_type);
   
   FFTLIB_PRINTF("%s", "{\n");
   for(t = 0; t < params->dim_z; t++){
      yPtr = zPtr;
	   for(r = 0; r < params->dim_y; r++){
         xPtr = yPtr;
		  for(c = 0; c < params->dim_x; c++){
			 FFTLIB_PRINTF("%3d, ", *(xPtr));
           xPtr++;
		  }
		  FFTLIB_PRINTF("%s", "\n");
         yPtr += stride_y_elements;
	   }
      zPtr += stride_z_elements;
   }
   FFTLIB_PRINTF("%s", "};\n");
   return;
}


template void FFTLIB_debugPrint3DVReg_helper<int8_t>  (int8_t   *matrix, const FFTLIB_bufParams3D_t *params);
template void FFTLIB_debugPrint3DVReg_helper<int16_t> (int16_t  *matrix, const FFTLIB_bufParams3D_t *params);
template void FFTLIB_debugPrint3DVReg_helper<uint16_t>(uint16_t *matrix, const FFTLIB_bufParams3D_t *params);
template void FFTLIB_debugPrint3DVReg_helper<int32_t> (int32_t  *matrix, const FFTLIB_bufParams3D_t *params);

/******************************************************************************/

/******************************************************************************/
/*!
 * \ingroup
 * \brief
 * \details
 * \return  void
 */
/******************************************************************************/


// want this function to have C-linkage in library...
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
   
   void FFTLIB_debugPrint3DVReg(void *matrix, const FFTLIB_bufParams3D_t *params)
   {
      switch(params->data_type){
         case FFTLIB_INT32 :
            FFTLIB_debugPrint3DVReg_helper<int32_t>((int32_t *)matrix, params);
            break;
         case FFTLIB_INT16 :
            FFTLIB_debugPrint3DVReg_helper<int16_t>((int16_t *)matrix, params);
            break;
         case FFTLIB_UINT16 :
            FFTLIB_debugPrint3DVReg_helper<uint16_t>((uint16_t *)matrix, params);
            break;
         case FFTLIB_INT8 :
            FFTLIB_debugPrint3DVReg_helper<int8_t>((int8_t *)matrix, params);
            break;
         default :
            FFTLIB_PRINTF("\nERROR: Unrecognized data type in %s.\n", __FUNCTION__);
      }
      
      return;
   }
#ifdef __cplusplus
}
#endif /* __cplusplus */
