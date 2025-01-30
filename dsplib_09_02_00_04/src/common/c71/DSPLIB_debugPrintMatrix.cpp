/******************************************************************************/
/*!
 * \file
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

#include "../DSPLIB_bufParams.h"
#include "../DSPLIB_types.h"
#include "../DSPLIB_utility.h"

//----------------------------------------------------------------------------------

template <typename dataType> void DSPLIB_debugPrintMatrix_helper(dataType *matrix, const DSPLIB_bufParams2D_t *params)
{
   uint32_t  x, y;
   dataType *xPtr;
   dataType *yPtr = matrix;

   // stride_y is stored in bytes, but easier to use in elements
   uint32_t stride_y_elements = params->stride_y / DSPLIB_sizeof(params->data_type);

   for (y = 0; y < params->dim_y; y++) {
      xPtr = yPtr;
      DSPLIB_PRINTF("%p |", xPtr);
      for (x = 0; x < params->dim_x; x++) {
         DSPLIB_PRINTF("%3d ", *(xPtr));
         xPtr++;
      }
      DSPLIB_PRINTF("%s", "|\n");
      yPtr += stride_y_elements;
   }

   return;
}

template void DSPLIB_debugPrintMatrix_helper<int8_t>(int8_t *matrix, const DSPLIB_bufParams2D_t *params);
template void DSPLIB_debugPrintMatrix_helper<uint8_t>(uint8_t *matrix, const DSPLIB_bufParams2D_t *params);
template void DSPLIB_debugPrintMatrix_helper<int16_t>(int16_t *matrix, const DSPLIB_bufParams2D_t *params);
template void DSPLIB_debugPrintMatrix_helper<uint16_t>(uint16_t *matrix, const DSPLIB_bufParams2D_t *params);
template void DSPLIB_debugPrintMatrix_helper<int32_t>(int32_t *matrix, const DSPLIB_bufParams2D_t *params);

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

void DSPLIB_debugPrintMatrix(void *matrix, const DSPLIB_bufParams2D_t *params)
{
   switch (params->data_type) {
   case DSPLIB_INT32:
      DSPLIB_debugPrintMatrix_helper<int32_t>((int32_t *) matrix, params);
      break;
   case DSPLIB_INT16:
      DSPLIB_debugPrintMatrix_helper<int16_t>((int16_t *) matrix, params);
      break;
   case DSPLIB_UINT16:
      DSPLIB_debugPrintMatrix_helper<uint16_t>((uint16_t *) matrix, params);
      break;
   case DSPLIB_INT8:
      DSPLIB_debugPrintMatrix_helper<int8_t>((int8_t *) matrix, params);
      break;
   case DSPLIB_UINT8:
      DSPLIB_debugPrintMatrix_helper<uint8_t>((uint8_t *) matrix, params);
      break;
   default:
      DSPLIB_PRINTF("\nERROR: Unrecognized data type in %s.\n", __FUNCTION__);
   }

   return;
}
#ifdef __cplusplus
}
#endif /* __cplusplus */
