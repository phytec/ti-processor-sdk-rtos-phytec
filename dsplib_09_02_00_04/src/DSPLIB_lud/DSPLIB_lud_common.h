/******************************************************************************
 *                                                                             *
 * module name       :DSPLIB                                                   *
 *                                                                             *
 * module descripton :Digital Signal Processing Library module for C7x+MMA   *
 *                                                                             *
 * Copyright (C) 2017-2018 Texas Instruments Incorporated - http://www.ti.com/ *
 * ALL RIGHTS RESERVED                                                         *
 *                                                                             *
 ******************************************************************************/

/********************************************************************************
 *  @file     DSPLIB_lud_common.h
 *
 *  @brief    File to hold common function DSPLIB_lud_common
 *
 *  @version  0.1 - Jun 2023 : Initial Version
 *
 ********************************************************************************/

#ifndef DSPLIB_LUD_COMMON_IXX_IXX_OXX_H_
#define DSPLIB_LUD_COMMON_IXX_IXX_OXX_H_

#include "../common/DSPLIB_utility.h"

/* ********************** DECLARATION OF COMMON INIT FUNCTIONS ************************* */

template <typename dataType>
void DSPLIB_lud_identity_matrix_generate_init_ci(uint8_t *pBlock, int32_t nRows, int32_t stride);

template <typename dataType>
DSPLIB_STATUS DSPLIB_matMul_init_ci(int32_t  nRowsIn0,
                                    int32_t  nColsIn0,
                                    int32_t  nColsIn1,
                                    int32_t  strideIn0,
                                    int32_t  strideIn1,
                                    int32_t  strideIn2,
                                    uint8_t *pBlock);

template <typename dataType> void DSPLIB_lud_blk_move_init_ci(uint8_t *pBlock, int32_t nRows, int32_t stride);

template <typename dataType> void DSPLIB_lud_inv_matTrans_init_ci(int32_t order, int32_t colStride, uint8_t *pBlock);

/* **************************** DECLARATION OF COMMON OPT FUNCTIONS ************************* */
template <typename dataType>
void DSPLIB_lud_identity_matrix_generate_exec_ci(dataType *pMat, int32_t nRows, int32_t colStride, uint8_t *pBlock);

template <typename dataType>
DSPLIB_STATUS DSPLIB_matMul_exec_ci(int32_t            nRowsIn0,
                                    int32_t            nColsIn0,
                                    int32_t            nColsIn1,
                                    int32_t            strideIn1,
                                    dataType *restrict pIn0,
                                    dataType *restrict pIn1,
                                    dataType *restrict pOut,
                                    uint8_t *          pBlock);

template <typename dataType>
void DSPLIB_lud_blk_move_exec_ci(dataType *pOut,
                                 dataType *pIn,
                                 int32_t   Nrows,
                                 int32_t   Ncols,
                                 int32_t   colOutStride,
                                 int32_t   colInStride,
                                 uint8_t * pBlock);

template <typename dataType>
void DSPLIB_lud_inv_matTrans_ci(dataType *pIn, dataType *pOut, int32_t order, int32_t colStride, uint8_t *pBlock);
#endif /* DSPLIB_LUD_COMMON_IXX_IXX_OXX_H_ */

/* ======================================================================== */
/*  End of file:  DSPLIB_lud_common.h          */
/* ======================================================================== */
