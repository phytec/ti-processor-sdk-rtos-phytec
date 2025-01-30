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
**|         Copyright (c) 2007-2012 Texas Instruments Incorporated           |**
**|                        ALL RIGHTS RESERVED                               |**
**|                                                                          |**
**| Permission to use, copy, modify, or distribute this software,            |**
**| whether in part or in whole, for any purpose is forbidden without        |**
**| a signed licensing agreement and NDA from Texas Instruments              |**
**| Incorporated (TI).                                                       |**
**|                                                                          |**
**| TI makes no representation or warranties with respect to the             |**
**| performance of this computer program, and specifically disclaims         |**
**| any responsibility for any damages, special or consequential,            |**
**| connected with the use of this program.                                  |**
**|                                                                          |**
**+--------------------------------------------------------------------------+**
*******************************************************************************/

#include "../FFTLIB_types.h"

/* -------------------------------------------------------------------------- */
/*  MISRAC Rule 8.2(UNMATCHED.PARAMS) Deviation: This is the compiler         */
/*  recommended way to initialize a vector.                                   */
/* -------------------------------------------------------------------------- */  
//char FFTLIB_vperm_data_0_63[64] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63};
#if defined(_HOST_BUILD)

const c7x::uchar_vec FFTLIB_zeroVect_uchar_vec = c7x::uchar_vec(0);
const c7x::ushort_vec FFTLIB_zeroVect_ushort_vec = c7x::ushort_vec(0);

#if __C7X_VEC_SIZE_BYTES__ == 64
const c7x::uchar_vec FFTLIB_vperm_data_0_63 = c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);

#elif __C7X_VEC_SIZE_BYTES__ == 32

const c7x::uchar_vec FFTLIB_vperm_data_0_63 = c7x::uchar_vec(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);


#endif // #if __C7X_VEC_SIZE_BYTES__

#else  // not host build.........................................................................................................................

const c7x::uchar_vec FFTLIB_zeroVect_uchar_vec = (c7x::uchar_vec)(0);
const c7x::ushort_vec FFTLIB_zeroVect_ushort_vec = (c7x::ushort_vec)(0);

#if __C7X_VEC_SIZE_BYTES__ == 64

const c7x::uchar_vec FFTLIB_vperm_data_0_63 = (const c7x::uchar_vec)(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);

#elif __C7X_VEC_SIZE_BYTES__ == 32

const c7x::uchar_vec FFTLIB_vperm_data_0_63 = (const c7x::uchar_vec)(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);

#endif // #if __C7X_VEC_SIZE_BYTES__
#endif // _HOST_BUILD


///* -------------------------------------------------------------------------- */
///*  MISRAC Rule 4.9(DEFINE.FUNC) Deviation: The advisory is not being         */
///*  addressed so as not  to lose portability across different platforms.      */
///* -------------------------------------------------------------------------- */
//#ifdef WIN32
//#define ASSIGN(param, value) value
//#else
//#define ASSIGN(param, value) .param = value
//#endif
//const __HWA_OFFSET_REG offsetRegStruct_zeros =
//{
//    ASSIGN(offset0  , 0),
//    ASSIGN(offset1  , 0),
//    ASSIGN(offset2  , 0),
//    ASSIGN(offset3  , 0),
//   ASSIGN(A_LUT_VAL_0      , 0),
//    ASSIGN(offset4  , 0),
//    ASSIGN(offset5  , 0),
//    ASSIGN(offset6  , 0),
//    ASSIGN(offset7  , 0),
//   ASSIGN(A_LUT_VAL_1      , 0),
//    ASSIGN(offset8  , 0),
//    ASSIGN(offset9  , 0),
//    ASSIGN(offset10 , 0),
//    ASSIGN(offset11 , 0),
//   ASSIGN(A_LUT_VAL_2      , 0),
//    ASSIGN(offset12 , 0),
//    ASSIGN(offset13 , 0),
//    ASSIGN(offset14 , 0),
//    ASSIGN(offset15 , 0),
//   ASSIGN(A_LUT_VAL_3      , 0),
//    ASSIGN(offset16 , 0),
//    ASSIGN(offset17 , 0),
//    ASSIGN(offset18 , 0),
//    ASSIGN(offset19 , 0),
//   ASSIGN(A_LUT_VAL_4      , 0),
//    ASSIGN(offset20 , 0),
//    ASSIGN(offset21 , 0),
//    ASSIGN(offset22 , 0),
//    ASSIGN(offset23 , 0),
//   ASSIGN(A_LUT_VAL_5      , 0),
//    ASSIGN(offset24 , 0),
//    ASSIGN(offset25 , 0),
//    ASSIGN(offset26 , 0),
//    ASSIGN(offset27 , 0),
//   ASSIGN(A_LUT_VAL_6      , 0),
//    ASSIGN(offset28 , 0),
//    ASSIGN(offset29 , 0),
//    ASSIGN(offset30 , 0),
//    ASSIGN(offset31 , 0),
//   ASSIGN(A_LUT_VAL_7      , 0),
//    ASSIGN(offset32 , 0),
//    ASSIGN(offset33 , 0),
//    ASSIGN(offset34 , 0),
//    ASSIGN(offset35 , 0),
//   ASSIGN(A_LUT_VAL_8      , 0),
//    ASSIGN(offset36 , 0),
//    ASSIGN(offset37 , 0),
//    ASSIGN(offset38 , 0),
//    ASSIGN(offset39 , 0),
//   ASSIGN(A_LUT_VAL_9      , 0),
//    ASSIGN(offset40 , 0),
//    ASSIGN(offset41 , 0),
//    ASSIGN(offset42 , 0),
//    ASSIGN(offset43 , 0),
//   ASSIGN(A_LUT_VAL_10     , 0),
//    ASSIGN(offset44 , 0),
//    ASSIGN(offset45 , 0),
//    ASSIGN(offset46 , 0),
//    ASSIGN(offset47 , 0),
//   ASSIGN(A_LUT_VAL_11     , 0),
//    ASSIGN(offset48 , 0),
//    ASSIGN(offset49 , 0),
//    ASSIGN(offset50 , 0),
//    ASSIGN(offset51 , 0),
//   ASSIGN(A_LUT_VAL_12     , 0),
//    ASSIGN(offset52 , 0),
//    ASSIGN(offset53 , 0),
//    ASSIGN(offset54 , 0),
//    ASSIGN(offset55 , 0),
//   ASSIGN(A_LUT_VAL_13     , 0),
//    ASSIGN(offset56 , 0),
//    ASSIGN(offset57 , 0),
//    ASSIGN(offset58 , 0),
//    ASSIGN(offset59 , 0),
//   ASSIGN(A_LUT_VAL_14     , 0),
//    ASSIGN(offset60 , 0),
//    ASSIGN(offset61 , 0),
//    ASSIGN(offset62 , 0),
//    ASSIGN(offset63 , 0),
//    ASSIGN(A_LUT_VAL_15     , 0)
//};
//
//const __HWA_OFFSET_REG offsetRegStruct_diagonal_32bit =
//{
//    ASSIGN(offset0  , 0),
//    ASSIGN(offset1  , 0),
//    ASSIGN(offset2  , 0),
//    ASSIGN(offset3  , 0),
//   ASSIGN(A_LUT_VAL_0      , 0),
//    ASSIGN(offset4  , 1),
//    ASSIGN(offset5  , 0),
//    ASSIGN(offset6  , 0),
//    ASSIGN(offset7  , 0),
//   ASSIGN(A_LUT_VAL_1      , 0),
//    ASSIGN(offset8  , 2),
//    ASSIGN(offset9  , 0),
//    ASSIGN(offset10 , 0),
//    ASSIGN(offset11 , 0),
//   ASSIGN(A_LUT_VAL_2      , 0),
//    ASSIGN(offset12 , 3),
//    ASSIGN(offset13 , 0),
//    ASSIGN(offset14 , 0),
//    ASSIGN(offset15 , 0),
//   ASSIGN(A_LUT_VAL_3      , 0),
//    ASSIGN(offset16 , 4),
//    ASSIGN(offset17 , 0),
//    ASSIGN(offset18 , 0),
//    ASSIGN(offset19 , 0),
//   ASSIGN(A_LUT_VAL_4      , 0),
//    ASSIGN(offset20 , 5),
//    ASSIGN(offset21 , 0),
//    ASSIGN(offset22 , 0),
//    ASSIGN(offset23 , 0),
//   ASSIGN(A_LUT_VAL_5      , 0),
//    ASSIGN(offset24 , 6),
//    ASSIGN(offset25 , 0),
//    ASSIGN(offset26 , 0),
//    ASSIGN(offset27 , 0),
//   ASSIGN(A_LUT_VAL_6      , 0),
//    ASSIGN(offset28 , 7),
//    ASSIGN(offset29 , 0),
//    ASSIGN(offset30 , 0),
//    ASSIGN(offset31 , 0),
//   ASSIGN(A_LUT_VAL_7      , 0),
//    ASSIGN(offset32 , 8),
//    ASSIGN(offset33 , 0),
//    ASSIGN(offset34 , 0),
//    ASSIGN(offset35 , 0),
//    ASSIGN(A_LUT_VAL_8      , 0),
//    ASSIGN(offset36 , 9),
//    ASSIGN(offset37 , 0),
//    ASSIGN(offset38 , 0),
//    ASSIGN(offset39 , 0),
//   ASSIGN(A_LUT_VAL_9      , 0),
//    ASSIGN(offset40 , 10),
//    ASSIGN(offset41 , 0),
//    ASSIGN(offset42 , 0),
//    ASSIGN(offset43 , 0),
//   ASSIGN(A_LUT_VAL_10      , 0),
//    ASSIGN(offset44 , 11),
//    ASSIGN(offset45 , 0),
//    ASSIGN(offset46 , 0),
//    ASSIGN(offset47 , 0),
//   ASSIGN(A_LUT_VAL_11      , 0),
//    ASSIGN(offset48 , 12),
//    ASSIGN(offset49 , 0),
//    ASSIGN(offset50 , 0),
//    ASSIGN(offset51 , 0),
//   ASSIGN(A_LUT_VAL_12      , 0),
//    ASSIGN(offset52 , 13),
//    ASSIGN(offset53 , 0),
//    ASSIGN(offset54 , 0),
//    ASSIGN(offset55 , 0),
//   ASSIGN(A_LUT_VAL_13      , 0),
//    ASSIGN(offset56 , 14),
//    ASSIGN(offset57 , 0),
//    ASSIGN(offset58 , 0),
//    ASSIGN(offset59 , 0),
//   ASSIGN(A_LUT_VAL_14      , 0),
//    ASSIGN(offset60 , 15),
//    ASSIGN(offset61 , 0),
//    ASSIGN(offset62 , 0),
//    ASSIGN(offset63 , 0),
//   ASSIGN(A_LUT_VAL_15      , 0)
//};
//
//const __HWA_OFFSET_REG offsetRegStruct_diagonal_16bit =
//{
//    ASSIGN(offset0  , 0),
//    ASSIGN(offset1  , 0),
//    ASSIGN(offset2  , 1),
//    ASSIGN(offset3  , 0),
//     ASSIGN(A_LUT_VAL_0      , 0),
//    ASSIGN(offset4  , 2),
//    ASSIGN(offset5  , 0),
//    ASSIGN(offset6  , 3),
//    ASSIGN(offset7  , 0),
//     ASSIGN(A_LUT_VAL_1      , 0),
//    ASSIGN(offset8  , 4),
//    ASSIGN(offset9  , 0),
//    ASSIGN(offset10 , 5),
//    ASSIGN(offset11 , 0),
//     ASSIGN(A_LUT_VAL_2      , 0),
//    ASSIGN(offset12 , 6),
//    ASSIGN(offset13 , 0),
//    ASSIGN(offset14 , 7),
//    ASSIGN(offset15 , 0),
//     ASSIGN(A_LUT_VAL_3      , 0),
//    ASSIGN(offset16 , 8),
//    ASSIGN(offset17 , 0),
//    ASSIGN(offset18 , 9),
//    ASSIGN(offset19 , 0),
//     ASSIGN(A_LUT_VAL_4      , 0),
//    ASSIGN(offset20 , 10),
//    ASSIGN(offset21 , 0),
//    ASSIGN(offset22 , 11),
//    ASSIGN(offset23 , 0),
//     ASSIGN(A_LUT_VAL_5      , 0),
//    ASSIGN(offset24 , 12),
//    ASSIGN(offset25 , 0),
//    ASSIGN(offset26 , 13),
//    ASSIGN(offset27 , 0),
//     ASSIGN(A_LUT_VAL_6      , 0),
//    ASSIGN(offset28 , 14),
//    ASSIGN(offset29 , 0),
//    ASSIGN(offset30 , 15),
//    ASSIGN(offset31 , 0),
//     ASSIGN(A_LUT_VAL_7      , 0),
//    ASSIGN(offset32 , 16),
//    ASSIGN(offset33 , 0),
//    ASSIGN(offset34 , 17),
//    ASSIGN(offset35 , 0),
//     ASSIGN(A_LUT_VAL_8      , 0),
//    ASSIGN(offset36 , 18),
//    ASSIGN(offset37 , 0),
//    ASSIGN(offset38 , 19),
//    ASSIGN(offset39 , 0),
//     ASSIGN(A_LUT_VAL_9      , 0),
//    ASSIGN(offset40 , 20),
//    ASSIGN(offset41 , 0),
//    ASSIGN(offset42 , 21),
//    ASSIGN(offset43 , 0),
//     ASSIGN(A_LUT_VAL_10      , 0),
//    ASSIGN(offset44 , 22),
//    ASSIGN(offset45 , 0),
//    ASSIGN(offset46 , 23),
//    ASSIGN(offset47 , 0),
//     ASSIGN(A_LUT_VAL_11      , 0),
//    ASSIGN(offset48 , 24),
//    ASSIGN(offset49 , 0),
//    ASSIGN(offset50 , 25),
//    ASSIGN(offset51 , 0),
//     ASSIGN(A_LUT_VAL_12      , 0),
//    ASSIGN(offset52 , 26),
//    ASSIGN(offset53 , 0),
//    ASSIGN(offset54 , 27),
//    ASSIGN(offset55 , 0),
//     ASSIGN(A_LUT_VAL_13      , 0),
//    ASSIGN(offset56 , 28),
//    ASSIGN(offset57 , 0),
//    ASSIGN(offset58 , 29),
//    ASSIGN(offset59 , 0),
//     ASSIGN(A_LUT_VAL_14      , 0),
//    ASSIGN(offset60 , 30),
//    ASSIGN(offset61 , 0),
//    ASSIGN(offset62 , 31),
//    ASSIGN(offset63 , 0),
//     ASSIGN(A_LUT_VAL_15      , 0)
//};
//
//const __HWA_OFFSET_REG offsetRegStruct_diagonal_8bit =
//{
//    ASSIGN(offset0  , 0),
//    ASSIGN(offset1  , 1),
//    ASSIGN(offset2  , 2),
//    ASSIGN(offset3  , 3),
//   ASSIGN(A_LUT_VAL_0      , 0),
//    ASSIGN(offset4  , 4),
//    ASSIGN(offset5  , 5),
//    ASSIGN(offset6  , 6),
//    ASSIGN(offset7  , 7),
//   ASSIGN(A_LUT_VAL_1      , 0),
//    ASSIGN(offset8  , 8),
//    ASSIGN(offset9  , 9),
//    ASSIGN(offset10 , 10),
//    ASSIGN(offset11 , 11),
//   ASSIGN(A_LUT_VAL_2      , 0),
//    ASSIGN(offset12 , 12),
//    ASSIGN(offset13 , 13),
//    ASSIGN(offset14 , 14),
//    ASSIGN(offset15 , 15),
//   ASSIGN(A_LUT_VAL_3      , 0),
//    ASSIGN(offset16 , 16),
//    ASSIGN(offset17 , 17),
//    ASSIGN(offset18 , 18),
//    ASSIGN(offset19 , 19),
//   ASSIGN(A_LUT_VAL_4      , 0),
//    ASSIGN(offset20 , 20),
//    ASSIGN(offset21 , 21),
//    ASSIGN(offset22 , 22),
//    ASSIGN(offset23 , 23),
//   ASSIGN(A_LUT_VAL_5      , 0),
//    ASSIGN(offset24 , 24),
//    ASSIGN(offset25 , 25),
//    ASSIGN(offset26 , 26),
//    ASSIGN(offset27 , 27),
//   ASSIGN(A_LUT_VAL_6      , 0),
//    ASSIGN(offset28 , 28),
//    ASSIGN(offset29 , 29),
//    ASSIGN(offset30 , 30),
//    ASSIGN(offset31 , 31),
//   ASSIGN(A_LUT_VAL_7      , 0),
//    ASSIGN(offset32 , 32),
//    ASSIGN(offset33 , 33),
//    ASSIGN(offset34 , 34),
//    ASSIGN(offset35 , 35),
//   ASSIGN(A_LUT_VAL_8      , 0),
//    ASSIGN(offset36 , 36),
//    ASSIGN(offset37 , 37),
//    ASSIGN(offset38 , 38),
//    ASSIGN(offset39 , 39),
//   ASSIGN(A_LUT_VAL_9      , 0),
//    ASSIGN(offset40 , 40),
//    ASSIGN(offset41 , 41),
//    ASSIGN(offset42 , 42),
//    ASSIGN(offset43 , 43),
//   ASSIGN(A_LUT_VAL_10     , 0),
//    ASSIGN(offset44 , 44),
//    ASSIGN(offset45 , 45),
//    ASSIGN(offset46 , 46),
//    ASSIGN(offset47 , 47),
//   ASSIGN(A_LUT_VAL_11     , 0),
//    ASSIGN(offset48 , 48),
//    ASSIGN(offset49 , 49),
//    ASSIGN(offset50 , 50),
//    ASSIGN(offset51 , 51),
//   ASSIGN(A_LUT_VAL_12     , 0),
//    ASSIGN(offset52 , 52),
//    ASSIGN(offset53 , 53),
//    ASSIGN(offset54 , 54),
//    ASSIGN(offset55 , 55),
//   ASSIGN(A_LUT_VAL_13     , 0),
//    ASSIGN(offset56 , 56),
//    ASSIGN(offset57 , 57),
//    ASSIGN(offset58 , 58),
//    ASSIGN(offset59 , 59),
//   ASSIGN(A_LUT_VAL_14     , 0),
//    ASSIGN(offset60 , 60),
//    ASSIGN(offset61 , 61),
//    ASSIGN(offset62 , 62),
//    ASSIGN(offset63 , 63),
//   ASSIGN(A_LUT_VAL_15     , 0)
//};


/* ======================================================================== */
/*  End of file:  FFTLIB_configurations.c                                            */
/* ======================================================================== */
