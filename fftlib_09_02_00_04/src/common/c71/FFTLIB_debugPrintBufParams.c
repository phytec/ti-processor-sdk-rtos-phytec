/******************************************************************************/
/*!
 * \file FFTLIB_debugPrintBufParams.c
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
#include "FFTLIB_debug.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/******************************************************************************/

/******************************************************************************/
/*!
 * \ingroup
 * \brief
 * \details
 * \return  void
 */
/******************************************************************************/

   void FFTLIB_debugPrintBufParams1D(const FFTLIB_bufParams1D_t *params)
   {
      FFTLIB_PRINTF("   dim_x        = %d\n", params->dim_x);
      FFTLIB_PRINTF("   data_type    = %d\n", params->data_type);

      return;
   }
   
   void FFTLIB_debugPrintBufParams(const FFTLIB_bufParams2D_t *params)
   {
      FFTLIB_PRINTF("   dim_x        = %d\n", params->dim_x);
      FFTLIB_PRINTF("   dim_y        = %d\n", params->dim_y);
      FFTLIB_PRINTF("   data_type    = %d\n", params->data_type);
      FFTLIB_PRINTF("   stride_y     = %d\n", params->stride_y);
      
      return;
   }
   
   void FFTLIB_debugPrintBufParams3D(const FFTLIB_bufParams3D_t *params)
   {
      FFTLIB_PRINTF("   dim_x        = %d\n", params->dim_x);
      FFTLIB_PRINTF("   dim_y        = %d\n", params->dim_y);
      FFTLIB_PRINTF("   dim_z        = %d\n", params->dim_z);
      FFTLIB_PRINTF("   data_type    = %d\n", params->data_type);
      FFTLIB_PRINTF("   stride_y     = %d\n", params->stride_y);
      FFTLIB_PRINTF("   stride_z     = %d\n", params->stride_z);
      
      return;
   }
#ifdef __cplusplus
}
#endif /* __cplusplus */
