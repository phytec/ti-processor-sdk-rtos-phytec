/*******************************************************************************
 **+--------------------------------------------------------------------------+**
 **|                            ****                                          |**
 **|                            ****                                          |**
 **|                            ******o***                                    |**
 **|                      ********_///_****                                   |**
 **|                      ***** /_//_/ ****                                   |**
 **|                       ** ** (__/ ****                                    |**
 **|                           *********                                      |**
 **|                            ****                                          |**
 **|                            ***                                           |**
 **|                                                                          |**
 **|        Copyright (c) 2016 Texas Instruments Incorporated                 |**
 **|                                                                          |**
 **|              All rights reserved not granted herein.                     |**
 **|                                                                          |**
 **|                         Limited License.                                 |**
 **|                                                                          |**
 **|  Texas Instruments Incorporated grants a world-wide, royalty-free,       |**
 **|  non-exclusive license under copyrights and patents it now or            |**
 **|  hereafter owns or controls to make, have made, use, import, offer to    |**
 **|  sell and sell ("Utilize") this software subject to the terms herein.    |**
 **|  With respect to the foregoing patent license, such license is granted   |**
 **|  solely to the extent that any such patent is necessary to Utilize the   |**
 **|  software alone.  The patent license shall not apply to any              |**
 **|  combinations which include this software, other than combinations       |**
 **|  with devices manufactured by or for TI ("TI Devices").  No hardware     |**
 **|  patent is licensed hereunder.                                           |**
 **|                                                                          |**
 **|  Redistributions must preserve existing copyright notices and            |**
 **|  reproduce this license (including the above copyright notice and the    |**
 **|  disclaimer and (if applicable) source code license limitations below)   |**
 **|  in the documentation and/or other materials provided with the           |**
 **|  distribution                                                            |**
 **|                                                                          |**
 **|  Redistribution and use in binary form, without modification, are        |**
 **|  permitted provided that the following conditions are met:               |**
 **|                                                                          |**
 **|    *  No reverse engineering, decompilation, or disassembly of this      |**
 **|  software is permitted with respect to any software provided in binary   |**
 **|  form.                                                                   |**
 **|                                                                          |**
 **|    *  any redistribution and use are licensed by TI for use only with    |**
 **|  TI Devices.                                                             |**
 **|                                                                          |**
 **|    *  Nothing shall obligate TI to provide you with source code for      |**
 **|  the software licensed and provided to you in object code.               |**
 **|                                                                          |**
 **|  If software source code is provided to you, modification and            |**
 **|  redistribution of the source code are permitted provided that the       |**
 **|  following conditions are met:                                           |**
 **|                                                                          |**
 **|    *  any redistribution and use of the source code, including any       |**
 **|  resulting derivative works, are licensed by TI for use only with TI     |**
 **|  Devices.                                                                |**
 **|                                                                          |**
 **|    *  any redistribution and use of any object code compiled from the    |**
 **|  source code and any resulting derivative works, are licensed by TI      |**
 **|  for use only with TI Devices.                                           |**
 **|                                                                          |**
 **|  Neither the name of Texas Instruments Incorporated nor the names of     |**
 **|  its suppliers may be used to endorse or promote products derived from   |**
 **|  this software without specific prior written permission.                |**
 **|                                                                          |**
 **|  DISCLAIMER.                                                             |**
 **|                                                                          |**
 **|  THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY      |**
 **|  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE       |**
 **|  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR      |**
 **|  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL TI AND TI'S LICENSORS BE      |**
 **|  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR     |**
 **|  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF    |**
 **|  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR         |**
 **|  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,   |**
 **|  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE    |**
 **|  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,       |**
 **|  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                      |**
 **+--------------------------------------------------------------------------+**
 *******************************************************************************/

#ifndef COMMON_FFTLIB_UTIL_SA2CLOADSA0CSTORE_H_
#define COMMON_FFTLIB_UTIL_SA2CLOADSA0CSTORE_H_ 1

/*******************************************************************************
 *
 * INCLUDES
 *
 ******************************************************************************/




/******************************************************************************/

/* ---------------------------------------------------------------- */
/*  MISRAC Rule 8.13(NEEDS.CONST) Deviation: CbackDestination cannot*/
/*  be declared with const qualifier because data is copied into    */
/*  buffer pointed by the pointer.                                  */
/* ---------------------------------------------------------------- */
/******************************************************************************/
/*!
 * \ingroup  fftlib_util
 * \brief   
 * \details 
 * \param[in] numRows
 * \param[in] numBRows
 * \param[in] CbackSource
 * \param[in] CbackDestination
 * \return  void
 */
/******************************************************************************/

__INLINE_FUNC(FFTLIB_UTIL_SA2CloadSA0Cstore)
static inline void FFTLIB_UTIL_SA2CloadSA0Cstore(int32_t numRows,
                                                 const uint8_t *restrict CbackSource,
                                                 uint8_t *restrict CbackDestination)
{
   int32_t r;
   __mma_vec VB0;

#pragma LOOP_NO_ALIAS
    for(r = 0; r < numRows; r++){
        __mma_vec valC  = *c7x::strm_agen<2, __mma_vec>::get_adv(CbackSource);
    	 __HWALDC(valC);
    	 __HWAXFER(__MMA_XFER_SRC_C);
    	 VB0 = __HWARCV(0);
       __HWAADV();

       __vpred tmp = c7x::strm_agen<0, __mma_vec>::get_vpred();
       __mma_vec * VB1 = c7x::strm_agen<0, __mma_vec>::get_adv(CbackDestination);
       __vstore_pred(tmp, VB1, VB0);

   }
   return;
}
#endif /* COMMON_FFTLIB_UTIL_SA2CLOADSA0CSTORE_H_ */
