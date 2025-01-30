/******************************************************************************/
/*!
 * \file FFTLIB_debugPrintMMAReg.c
 */
/* Copyright (C) 2019 Texas Instruments Incorporated - http://www.ti.com/
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
#include "FFTLIB_debug.h"

void FFTLIB_debugPrintMMAReg (const int32_t regNumber)
{
   /*****************************************************************************
    * commented out until compiler fix is in place for HWXFER to accept 0 to 15 *
    *****************************************************************************/
   /* FFTLIB_asm(" NOP");
    * FFTLIB_asm(" NOP");
    * FFTLIB_asm(" NOP");
    * FFTLIB_asm(" NOP");
    * FFTLIB_asm(" NOP");
    * __HWAXFER(__MMA_XFER_SRC_HWA_STATUS); // 8->HWACONFIG, 9->HWAOFFSET, 10->HWASTATUS, 11->HWABUSY
    * __HWAADV();
    * FFTLIB_asm(" NOP");
    * FFTLIB_asm(" NOP");
    * FFTLIB_asm(" NOP");
    * FFTLIB_asm(" NOP");
    * FFTLIB_asm(" NOP");
    *
    * uchar64 vb_hwaconfig = __HWARCV(0);
    * __HWAADV();
    * FFTLIB_asm(" NOP");
    * FFTLIB_asm(" NOP");
    * FFTLIB_asm(" NOP");
    * FFTLIB_asm(" NOP");
    * FFTLIB_asm(" NOP");
    *
    * /\* printf("\n%d:", regNumber); *\/
    * /\* Since hard-coded here: __HWAXFER(__MMA_XFER_SRC_HWA_STATUS); *\/
    * printf("\n10:");
    * FFTLIB_debugPrintVReg_i64x(vb_hwaconfig); */
}
