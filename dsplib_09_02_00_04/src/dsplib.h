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

#ifndef _DSPLIB_H_
#define _DSPLIB_H_

#include "DSPLIB_add/DSPLIB_add.h"
#include "DSPLIB_addConstant/DSPLIB_addConstant.h"
#include "DSPLIB_bexp/DSPLIB_bexp.h"
#include "DSPLIB_blk_eswap/DSPLIB_blk_eswap.h"
#include "DSPLIB_blk_move/DSPLIB_blk_move.h"
#include "DSPLIB_cascadeBiquad/DSPLIB_cascadeBiquad.h"
#include "DSPLIB_cholesky/DSPLIB_cholesky.h"
#include "DSPLIB_cholesky/DSPLIB_cholesky_inplace.h"
#include "DSPLIB_cholesky/DSPLIB_cholesky_solver.h"
#include "DSPLIB_dotp_sqr/DSPLIB_dotp_sqr.h"
#include "DSPLIB_dotprod/DSPLIB_dotprod.h"
#include "DSPLIB_fir/DSPLIB_fir.h"
#include "DSPLIB_fltoq15/DSPLIB_fltoq15.h"
#include "DSPLIB_lud/DSPLIB_lud.h"
#include "DSPLIB_lud/DSPLIB_lud_inv.h"
#include "DSPLIB_lud/DSPLIB_lud_sol.h"
#include "DSPLIB_matMul/DSPLIB_matMul.h"
#include "DSPLIB_matMul_fixed/DSPLIB_matMul_fixed.h"
#include "DSPLIB_mat_submat_copy/DSPLIB_mat_submat_copy.h"
#include "DSPLIB_matTrans/DSPLIB_matTrans.h"
#include "DSPLIB_max/DSPLIB_max.h"
#include "DSPLIB_maxEvery/DSPLIB_maxEvery.h"
#include "DSPLIB_maxIndex/DSPLIB_maxIndex.h"
#include "DSPLIB_min/DSPLIB_min.h"
#include "DSPLIB_minEvery/DSPLIB_minEvery.h"
#include "DSPLIB_minIndex/DSPLIB_minIndex.h"
#include "DSPLIB_minerror/DSPLIB_minerror.h"
#include "DSPLIB_mul/DSPLIB_mul.h"
#include "DSPLIB_mulConstant/DSPLIB_mulConstant.h"
#include "DSPLIB_negate/DSPLIB_negate.h"
#include "DSPLIB_q15tofl/DSPLIB_q15tofl.h"
#include "DSPLIB_qrd/DSPLIB_qrd.h"
#include "DSPLIB_qrd/DSPLIB_qrd_inverse.h"
#include "DSPLIB_qrd/DSPLIB_qrd_solver.h"
#include "DSPLIB_sqr/DSPLIB_sqr.h"
#include "DSPLIB_sqrAdd/DSPLIB_sqrAdd.h"
#include "DSPLIB_sub/DSPLIB_sub.h"
#include "DSPLIB_subConstant/DSPLIB_subConstant.h"
#include "DSPLIB_svd/DSPLIB_svd.h"
#include "DSPLIB_w_vec/DSPLIB_w_vec.h"
#include "common/DSPLIB_bufParams.h"
#include "common/DSPLIB_types.h"

#include "DSPLIB_recip/DSPLIB_recip.h"
#endif /*_DSPLIB_H_*/
