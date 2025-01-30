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
/*  MISRAC Rule 4.9(DEFINE.FUNC) Deviation: The advisory is not being         */
/*  addressed so as not  to lose portability across different platforms.      */
/* -------------------------------------------------------------------------- */
#ifdef WIN32
#define ASSIGN(param, value) value
#else
#define ASSIGN(param, value) .param = value
#endif
const FFTLIB_MMA_CONFIG_REG configRegisterStruct_i32s_i32s_o32s =
{
      /* -------------------------------------------------------------------- */
      /*  MISRAC Rule 10.3(ETYPE.ASSIGN.2012) Deviation: The data types of    */
      /*  fields and the enum values are set by compiler according to the     */
      /*  hardware specification, and are used as is.                         */
      /* -------------------------------------------------------------------- */
      ASSIGN(A_ATYPE  , __MMA_A_CONFIG_ATYPE_INT32),
      ASSIGN(A_RSVD1  , 0),
      ASSIGN(A_ALUTEN , __MMA_A_CONFIG_NOLUT),
      ASSIGN(A_RSVD2  , 0),
      ASSIGN(A_ARF_CTRL , __MMA_A_CONFIG_ARF_DISABLE),
      ASSIGN(A_ARF_BASE , 0),
      ASSIGN(A_RSVD3  , 0),
      ASSIGN(A_ARF_SIZE , 0),
      ASSIGN(A_RSVD4  , 0),

      ASSIGN(B_BSWPER  , FFTLIB_MMA_SIZE_32_BIT), // 32 bits
      ASSIGN(B_BRSTPER , FFTLIB_MMA_SIZE_32_BIT), // 8 bits
      ASSIGN(B_BTYPE   , __MMA_B_CONFIG_SIZE32), // 2 bits
      ASSIGN(B_LDBOPT  , __MMA_LDBOPT_MMA1),
      ASSIGN(B_B4EXP   , __MMA_B_CONFIG_B4EXP_MMA1),
      ASSIGN(B_RSVD1   , 0),
      ASSIGN(B_ORDER   , __MMA_B_CONFIG_ROW),     // 1 bit
      ASSIGN(B_RSVD2   , 0),
      ASSIGN(B_BSTART  , 0),    // 1 bits
      ASSIGN(B_BCNT1_ENABLE , 0),
      ASSIGN(B_RSVD3   , 0),
      ASSIGN(B_BOFFSET , 0),  // 8 bits
      ASSIGN(B_RSVD4   , 0),

      ASSIGN(C_ATYPE , __MMA_C_CONFIG_ATYPE_SA),
      ASSIGN(C_ARF_BASE , 0),
      ASSIGN(C_ARF_C7 , 0),
      ASSIGN(C_BTYPE , __MMA_C_CONFIG_BTYPE_INT32),
      ASSIGN(C_RSVD2 , 0),
      ASSIGN(C_OPERATION0 , __MMA_C_CONFIG_MUL),
      ASSIGN(C_LOP0,   __MMA_C_CONFIG_LOP_C),
      ASSIGN(C_RSVD3 , 0),
      ASSIGN(C_OPERATION1 , __MMA_C_CONFIG_MULPLUS),
      ASSIGN(C_LOP1,   __MMA_C_CONFIG_LOP_C),
      ASSIGN(C_BIASORDER , __MMA_C_CONFIG_BIAS_ORDER_COLUMN),
      ASSIGN(C_RSVD4 , 0),
      ASSIGN(C_HWLDDST , __MMA_C_CONFIG_HWLDDST_X4_0),
      ASSIGN(C_RSVD5 , 0),
      ASSIGN(C_HWLDTYPE , __MMA_C_CONFIG_HWLDTYPE_INT32),
      ASSIGN(C_RSVD6 , 0),

      ASSIGN(C_OPSTART  , __MMA_C_CONFIG_OPSTART_OPERATION0), // No enum in MMA spec? Initial C operand selections
      ASSIGN(C_BSTART   , 0x0),  // Initial B bank selection for reading B matrix data
      ASSIGN(C_CRSTART  , 0x0), // Initial C bank selection for reading operands
      ASSIGN(C_CWSTART  , 0x0), // Initial C bank selection for writing computation results
      ASSIGN(C_CLSTART  , 0x0), // Initial C bank selection for writing operands from HWALD*
      ASSIGN(C_RSVD7    , 0),
      ASSIGN(C_CROFFSET , 0x0),       // 6-bits C row read offset
      ASSIGN(C_RSVD8    , 0),
      ASSIGN(C_CWOFFSET , 0x0),       // C row write offset for computations
      ASSIGN(C_RSVD9    , 0),
      ASSIGN(C_CLOFFSET , 0x0),       // C row write offset for HWALD* instructions
      ASSIGN(C_RSVD10   , 0),
      ASSIGN(C_CLSWPER  , 0),  // C bank switch period for HWALD* instruction writes
      ASSIGN(C_CLRSTPER , 0), // C write row offset reset period for HWALD*
      ASSIGN(C_OP1PER   , 0),  // Operation 1 period
      ASSIGN(C_OP0PER   , FFTLIB_MMA_SIZE_32_BIT),  // Operation 0 period
      ASSIGN(C_BSWPER   , FFTLIB_MMA_SIZE_32_BIT),  // B bank switch period
      ASSIGN(C_CRSWPER  , 0), // C bank switch period for read instructions
      ASSIGN(C_CWSWPER  , 0), // C bank switch period for computation writes
      ASSIGN(C_CRRSTPER , FFTLIB_MMA_SIZE_32_BIT), // C read row offset reset period
      ASSIGN(C_CWRSTPER , FFTLIB_MMA_SIZE_32_BIT), // C write row offset reset period for computations

      ASSIGN(X_ReLU  , 0x0), // Enable Rectified Linear Units non-linearity after optional saturation
      ASSIGN(X_PSAT  , 0),
      ASSIGN(X_SAT_MIN_5_0 , 0),
      ASSIGN(X_SAT   , 0x1),  // Enable saturation in the transfer buffer element type after optional rounding
      ASSIGN(X_SAT_MIN_12_6 , 0),
      ASSIGN(X_RE    , 0x1),   // Enable routing via 1/2 LSB addition after shifting
      ASSIGN(X_SAT_MIN_15_13, 0),
      ASSIGN(X_RANGE, __MMA_X_CONFIG_RANGE_DISABLE_NOINIT),
      ASSIGN(X_SCALE_SHIFT_CTRL, __MMA_X_CONFIG_SCALE_SHIFT_CTRL_DISABLE),
      ASSIGN(X_RSVD3 , 0),
      ASSIGN(X_SHIFT , 0), // 7 bits Right shift amount), signed or unsigned depending on CTYPE fieldASSIGN(
      ASSIGN(X_VPACKN , __MMA_X_CONFIG_VPACKN_DISABLE),
      ASSIGN(X_XTYPE , __MMA_X_CONFIG_XTYPE_INT32), // Transfer buffer element typeASSIGN( Not all combinations of CTYPE and XTYPE are supported
      ASSIGN(X_SAT_MAX_3_0 , 0),
      ASSIGN(X_CTYPE , __MMA_X_CONFIG_CTYPE_INT128), // C matrix element typeASSIGN(  This must be consistent with the B FSM setting
      ASSIGN(X_SAT_MAX_8_4 , 0),
      ASSIGN(X_CSWPER , FFTLIB_MMA_SIZE_32_BIT), // C read bank switch period
      ASSIGN(X_CRRSTPER , FFTLIB_MMA_SIZE_32_BIT),     // C read row offset reset period
      ASSIGN(X_COFFSET  , 0x0),          // C matrix row read address offset
      ASSIGN(X_CSTART   , 0x0),          // Initial C bank selection
      ASSIGN(X_SAT_MAX_15_9 , 0x0),

      ASSIGN(RSVD , 0),
      ASSIGN(PARITYCTRL , __MMA_NORMAL)
};

/*********************************
 * Typical 16-bit configurations *
 *********************************/

const FFTLIB_MMA_CONFIG_REG configRegisterStruct_i16s_i16s_o16s =
{
      ASSIGN(A_ATYPE  , __MMA_A_CONFIG_ATYPE_INT16),
      ASSIGN(A_RSVD1  , 0),
      ASSIGN(A_ALUTEN , __MMA_A_CONFIG_NOLUT),
      ASSIGN(A_RSVD2  , 0),
      ASSIGN(A_ARF_CTRL , __MMA_A_CONFIG_ARF_DISABLE),
      ASSIGN(A_ARF_BASE , 0),
      ASSIGN(A_RSVD3  , 0),
      ASSIGN(A_ARF_SIZE , 0),
      ASSIGN(A_RSVD4  , 0),

      ASSIGN(B_BSWPER  , FFTLIB_MMA_SIZE_16_BIT), // 32 bits
      ASSIGN(B_BRSTPER , FFTLIB_MMA_SIZE_16_BIT), // 8 bits
      ASSIGN(B_BTYPE   , __MMA_B_CONFIG_SIZE16), // 2 bits
      ASSIGN(B_LDBOPT  , __MMA_LDBOPT_MMA1),
      ASSIGN(B_B4EXP   , __MMA_B_CONFIG_B4EXP_MMA1),
      ASSIGN(B_RSVD1   , 0),
      ASSIGN(B_ORDER   , __MMA_B_CONFIG_ROW),     // 1 bit
      ASSIGN(B_RSVD2   , 0),
      ASSIGN(B_BSTART  , 0),    // 1 bits
      ASSIGN(B_BCNT1_ENABLE , 0),
      ASSIGN(B_RSVD3   , 0),
      ASSIGN(B_BOFFSET , 0),  // 8 bits
      ASSIGN(B_RSVD4   , 0),

      ASSIGN(C_ATYPE , __MMA_C_CONFIG_ATYPE_SA),
      ASSIGN(C_ARF_BASE , 0),
      ASSIGN(C_ARF_C7 , 0),
      ASSIGN(C_BTYPE , __MMA_C_CONFIG_BTYPE_INT16),
      ASSIGN(C_RSVD2 , 0),
      ASSIGN(C_OPERATION0 , __MMA_C_CONFIG_MUL),
      ASSIGN(C_LOP0,   __MMA_C_CONFIG_LOP_C),
      ASSIGN(C_RSVD3 , 0),
      ASSIGN(C_OPERATION1 , __MMA_C_CONFIG_MULPLUS),
      ASSIGN(C_LOP1,   __MMA_C_CONFIG_LOP_C),
      ASSIGN(C_BIASORDER , __MMA_C_CONFIG_BIAS_ORDER_COLUMN),
      ASSIGN(C_RSVD4 , 0),
      ASSIGN(C_HWLDDST , __MMA_C_CONFIG_HWLDDST_X4_0),
      ASSIGN(C_RSVD5 , 0),
      ASSIGN(C_HWLDTYPE , __MMA_C_CONFIG_HWLDTYPE_INT16),
      ASSIGN(C_RSVD6 , 0),

      ASSIGN(C_OPSTART  , __MMA_C_CONFIG_OPSTART_OPERATION0), // No enum in MMA spec? Initial C operand selections
      ASSIGN(C_BSTART   , 0x0),  // Initial B bank selection for reading B matrix data
      ASSIGN(C_CRSTART  , 0x0), // Initial C bank selection for reading operands
      ASSIGN(C_CWSTART  , 0x0), // Initial C bank selection for writing computation results
      ASSIGN(C_CLSTART  , 0x0), // Initial C bank selection for writing operands from HWALD*
      ASSIGN(C_RSVD7    , 0),
      ASSIGN(C_CROFFSET , 0x0),       // 6-bits C row read offset
      ASSIGN(C_RSVD8    , 0),
      ASSIGN(C_CWOFFSET , 0x0),       // C row write offset for computations
      ASSIGN(C_RSVD9    , 0),
      ASSIGN(C_CLOFFSET , 0x0),       // C row write offset for HWALD* instructions
      ASSIGN(C_RSVD10   , 0),
      ASSIGN(C_CLSWPER  , 0),  // C bank switch period for HWALD* instruction writes
      ASSIGN(C_CLRSTPER , 0), // C write row offset reset period for HWALD*
      ASSIGN(C_OP1PER   , 0),  // Operation 1 period
      ASSIGN(C_OP0PER   , FFTLIB_MMA_SIZE_16_BIT),  // Operation 0 period
      ASSIGN(C_BSWPER   , FFTLIB_MMA_SIZE_16_BIT),  // B bank switch period
      ASSIGN(C_CRSWPER  , 0), // C bank switch period for read instructions
      ASSIGN(C_CWSWPER  , 0), // C bank switch period for computation writes
      ASSIGN(C_CRRSTPER , FFTLIB_MMA_SIZE_16_BIT), // C read row offset reset period
      ASSIGN(C_CWRSTPER , FFTLIB_MMA_SIZE_16_BIT), // C write row offset reset period for computations

      ASSIGN(X_ReLU  , 0x0), // Enable Rectified Linear Units non-linearity after optional saturation
      ASSIGN(X_PSAT  , 0),
      ASSIGN(X_SAT_MIN_5_0 , 0),
      ASSIGN(X_SAT   , 0x1),  // Enable saturation in the transfer buffer element type after optional rounding
      ASSIGN(X_SAT_MIN_12_6 , 0),
      ASSIGN(X_RE    , 0x1),   // Enable routing via 1/2 LSB addition after shifting
      ASSIGN(X_SAT_MIN_15_13, 0),
      ASSIGN(X_RANGE, __MMA_X_CONFIG_RANGE_DISABLE_NOINIT),
      ASSIGN(X_SCALE_SHIFT_CTRL, __MMA_X_CONFIG_SCALE_SHIFT_CTRL_DISABLE),
      ASSIGN(X_RSVD3 , 0),
      ASSIGN(X_SHIFT , 0), // 7 bits Right shift amount), signed or unsigned depending on CTYPE fieldASSIGN(
      ASSIGN(X_VPACKN , __MMA_X_CONFIG_VPACKN_DISABLE),
      ASSIGN(X_XTYPE , __MMA_X_CONFIG_XTYPE_INT16), // Transfer buffer element typeASSIGN( Not all combinations of CTYPE and XTYPE are supported
      ASSIGN(X_SAT_MAX_3_0 , 0),
      ASSIGN(X_CTYPE , __MMA_X_CONFIG_CTYPE_INT64), // C matrix element typeASSIGN(  This must be consistent with the B FSM setting
      ASSIGN(X_SAT_MAX_8_4 , 0),
      ASSIGN(X_CSWPER , FFTLIB_MMA_SIZE_16_BIT), // C read bank switch period
      ASSIGN(X_CRRSTPER , FFTLIB_MMA_SIZE_16_BIT),     // C read row offset reset period
      ASSIGN(X_COFFSET  , 0x0),          // C matrix row read address offset
      ASSIGN(X_CSTART   , 0x0),          // Initial C bank selection
      ASSIGN(X_SAT_MAX_15_9 , 0x0),

      ASSIGN(RSVD , 0),
      ASSIGN(PARITYCTRL , __MMA_NORMAL)
};


const FFTLIB_MMA_CONFIG_REG configRegisterStruct_i16s_i16s_o16u =
{
      ASSIGN(A_ATYPE  , __MMA_A_CONFIG_ATYPE_INT16),
      ASSIGN(A_RSVD1  , 0),
      ASSIGN(A_ALUTEN , __MMA_A_CONFIG_NOLUT),
      ASSIGN(A_RSVD2  , 0),
      ASSIGN(A_ARF_CTRL , __MMA_A_CONFIG_ARF_DISABLE),
      ASSIGN(A_ARF_BASE , 0),
      ASSIGN(A_RSVD3  , 0),
      ASSIGN(A_ARF_SIZE , 0),
      ASSIGN(A_RSVD4  , 0),

      ASSIGN(B_BSWPER  , FFTLIB_MMA_SIZE_16_BIT), // 32 bits
      ASSIGN(B_BRSTPER , FFTLIB_MMA_SIZE_16_BIT), // 8 bits
      ASSIGN(B_BTYPE   , __MMA_B_CONFIG_SIZE16), // 2 bits
      ASSIGN(B_LDBOPT  , __MMA_LDBOPT_MMA1),
      ASSIGN(B_B4EXP   , __MMA_B_CONFIG_B4EXP_MMA1),
      ASSIGN(B_RSVD1   , 0),
      ASSIGN(B_ORDER   , __MMA_B_CONFIG_ROW),     // 1 bit
      ASSIGN(B_RSVD2   , 0),
      ASSIGN(B_BSTART  , 0),    // 1 bits
      ASSIGN(B_BCNT1_ENABLE , 0),
      ASSIGN(B_RSVD3   , 0),
      ASSIGN(B_BOFFSET , 0),  // 8 bits
      ASSIGN(B_RSVD4   , 0),

      ASSIGN(C_ATYPE , __MMA_C_CONFIG_ATYPE_SA),
      ASSIGN(C_ARF_BASE , 0),
      ASSIGN(C_ARF_C7 , 0),
      ASSIGN(C_BTYPE , __MMA_C_CONFIG_BTYPE_INT16),
      ASSIGN(C_RSVD2 , 0),
      ASSIGN(C_OPERATION0 , __MMA_C_CONFIG_MUL),
      ASSIGN(C_LOP0,   __MMA_C_CONFIG_LOP_C),
      ASSIGN(C_RSVD3 , 0),
      ASSIGN(C_OPERATION1 , __MMA_C_CONFIG_MULPLUS),
      ASSIGN(C_LOP1,   __MMA_C_CONFIG_LOP_C),
      ASSIGN(C_BIASORDER , __MMA_C_CONFIG_BIAS_ORDER_COLUMN),
      ASSIGN(C_RSVD4 , 0),
      ASSIGN(C_HWLDDST , __MMA_C_CONFIG_HWLDDST_X4_0),
      ASSIGN(C_RSVD5 , 0),
      ASSIGN(C_HWLDTYPE , __MMA_C_CONFIG_HWLDTYPE_INT16),
      ASSIGN(C_RSVD6 , 0),

      ASSIGN(C_OPSTART  , __MMA_C_CONFIG_OPSTART_OPERATION0), // No enum in MMA spec? Initial C operand selections
      ASSIGN(C_BSTART   , 0x0),  // Initial B bank selection for reading B matrix data
      ASSIGN(C_CRSTART  , 0x0), // Initial C bank selection for reading operands
      ASSIGN(C_CWSTART  , 0x0), // Initial C bank selection for writing computation results
      ASSIGN(C_CLSTART  , 0x0), // Initial C bank selection for writing operands from HWALD*
      ASSIGN(C_RSVD7    , 0),
      ASSIGN(C_CROFFSET , 0x0),       // 6-bits C row read offset
      ASSIGN(C_RSVD8    , 0),
      ASSIGN(C_CWOFFSET , 0x0),       // C row write offset for computations
      ASSIGN(C_RSVD9    , 0),
      ASSIGN(C_CLOFFSET , 0x0),       // C row write offset for HWALD* instructions
      ASSIGN(C_RSVD10   , 0),
      ASSIGN(C_CLSWPER  , 0),  // C bank switch period for HWALD* instruction writes
      ASSIGN(C_CLRSTPER , 0), // C write row offset reset period for HWALD*
      ASSIGN(C_OP1PER   , 0),  // Operation 1 period
      ASSIGN(C_OP0PER   , FFTLIB_MMA_SIZE_16_BIT),  // Operation 0 period
      ASSIGN(C_BSWPER   , FFTLIB_MMA_SIZE_16_BIT),  // B bank switch period
      ASSIGN(C_CRSWPER  , 0), // C bank switch period for read instructions
      ASSIGN(C_CWSWPER  , 0), // C bank switch period for computation writes
      ASSIGN(C_CRRSTPER , FFTLIB_MMA_SIZE_16_BIT), // C read row offset reset period
      ASSIGN(C_CWRSTPER , FFTLIB_MMA_SIZE_16_BIT), // C write row offset reset period for computations

      ASSIGN(X_ReLU  , 0x1), // Enable Rectified Linear Units non-linearity after optional saturation
      ASSIGN(X_PSAT  , 0),
      ASSIGN(X_SAT_MIN_5_0 , 0),
      ASSIGN(X_SAT   , 0x0),  // Enable saturation in the transfer buffer element type after optional rounding
      ASSIGN(X_SAT_MIN_12_6 , 0),
      ASSIGN(X_RE    , 0x1),   // Enable routing via 1/2 LSB addition after shifting
      ASSIGN(X_SAT_MIN_15_13, 0),
      ASSIGN(X_RANGE, __MMA_X_CONFIG_RANGE_DISABLE_NOINIT),
      ASSIGN(X_SCALE_SHIFT_CTRL, __MMA_X_CONFIG_SCALE_SHIFT_CTRL_DISABLE),
      ASSIGN(X_RSVD3, 0),

      ASSIGN(X_SHIFT , 0), // 7 bits Right shift amount), signed or unsigned depending on CTYPE fieldASSIGN(
      ASSIGN(X_VPACKN , __MMA_X_CONFIG_VPACKN_DISABLE),
      ASSIGN(X_XTYPE , __MMA_X_CONFIG_XTYPE_UINT16), // Transfer buffer element typeASSIGN( Not all combinations of CTYPE and XTYPE are supported
      ASSIGN(X_SAT_MAX_3_0 , 0),
      ASSIGN(X_CTYPE , __MMA_X_CONFIG_CTYPE_INT64), // C matrix element typeASSIGN(  This must be consistent with the B FSM setting
      ASSIGN(X_SAT_MAX_8_4 , 0),
      ASSIGN(X_CSWPER , FFTLIB_MMA_SIZE_16_BIT), // C read bank switch period
      ASSIGN(X_CRRSTPER , FFTLIB_MMA_SIZE_16_BIT),     // C read row offset reset period
      ASSIGN(X_COFFSET  , 0x0),          // C matrix row read address offset
      ASSIGN(X_CSTART   , 0x0),          // Initial C bank selection
      ASSIGN(X_SAT_MAX_15_9 , 0x0),

      ASSIGN(RSVD , 0),
      ASSIGN(PARITYCTRL , __MMA_NORMAL)
};

const FFTLIB_MMA_CONFIG_REG configRegisterStruct_i16u_i16s_o16s =
{
      ASSIGN(A_ATYPE  , __MMA_A_CONFIG_ATYPE_UINT16),
      ASSIGN(A_RSVD1  , 0),
      ASSIGN(A_ALUTEN , __MMA_A_CONFIG_NOLUT),
      ASSIGN(A_RSVD2  , 0),
      ASSIGN(A_ARF_CTRL , __MMA_A_CONFIG_ARF_DISABLE),
      ASSIGN(A_ARF_BASE , 0),
      ASSIGN(A_RSVD3  , 0),
      ASSIGN(A_ARF_SIZE , 0),
      ASSIGN(A_RSVD4  , 0),

      ASSIGN(B_BSWPER  , FFTLIB_MMA_SIZE_16_BIT), // 32 bits
      ASSIGN(B_BRSTPER , FFTLIB_MMA_SIZE_16_BIT), // 8 bits
      ASSIGN(B_BTYPE   , __MMA_B_CONFIG_SIZE16), // 2 bits
      ASSIGN(B_LDBOPT  , __MMA_LDBOPT_MMA1),
      ASSIGN(B_B4EXP   , __MMA_B_CONFIG_B4EXP_MMA1),
      ASSIGN(B_RSVD1   , 0),
      ASSIGN(B_ORDER   , __MMA_B_CONFIG_ROW),     // 1 bit
      ASSIGN(B_RSVD2   , 0),
      ASSIGN(B_BSTART  , 0),    // 1 bits
      ASSIGN(B_BCNT1_ENABLE , 0),
      ASSIGN(B_RSVD3   , 0),
      ASSIGN(B_BOFFSET , 0),  // 8 bits
      ASSIGN(B_RSVD4   , 0),

      ASSIGN(C_ATYPE , __MMA_C_CONFIG_ATYPE_UA),
      ASSIGN(C_ARF_BASE , 0),
      ASSIGN(C_ARF_C7 , 0),
      ASSIGN(C_BTYPE , __MMA_C_CONFIG_BTYPE_INT16),
      ASSIGN(C_RSVD2 , 0),
      ASSIGN(C_OPERATION0 , __MMA_C_CONFIG_MUL),
      ASSIGN(C_LOP0,   __MMA_C_CONFIG_LOP_C),
      ASSIGN(C_RSVD3 , 0),
      ASSIGN(C_OPERATION1 , __MMA_C_CONFIG_MULPLUS),
      ASSIGN(C_LOP1,   __MMA_C_CONFIG_LOP_C),
      ASSIGN(C_BIASORDER , __MMA_C_CONFIG_BIAS_ORDER_COLUMN),
      ASSIGN(C_RSVD4 , 0),
      ASSIGN(C_HWLDDST , __MMA_C_CONFIG_HWLDDST_X4_0),
      ASSIGN(C_RSVD5 , 0),
      ASSIGN(C_HWLDTYPE , __MMA_C_CONFIG_HWLDTYPE_INT16),
      ASSIGN(C_RSVD6 , 0),

      ASSIGN(C_OPSTART  , __MMA_C_CONFIG_OPSTART_OPERATION0), // No enum in MMA spec? Initial C operand selections
      ASSIGN(C_BSTART   , 0x0),  // Initial B bank selection for reading B matrix data
      ASSIGN(C_CRSTART  , 0x0), // Initial C bank selection for reading operands
      ASSIGN(C_CWSTART  , 0x0), // Initial C bank selection for writing computation results
      ASSIGN(C_CLSTART  , 0x0), // Initial C bank selection for writing operands from HWALD*
      ASSIGN(C_RSVD7    , 0),
      ASSIGN(C_CROFFSET , 0x0),       // 6-bits C row read offset
      ASSIGN(C_RSVD8    , 0),
      ASSIGN(C_CWOFFSET , 0x0),       // C row write offset for computations
      ASSIGN(C_RSVD9    , 0),
      ASSIGN(C_CLOFFSET , 0x0),       // C row write offset for HWALD* instructions
      ASSIGN(C_RSVD10   , 0),
      ASSIGN(C_CLSWPER  , 0),  // C bank switch period for HWALD* instruction writes
      ASSIGN(C_CLRSTPER , 0), // C write row offset reset period for HWALD*
      ASSIGN(C_OP1PER   , 0),  // Operation 1 period
      ASSIGN(C_OP0PER   , FFTLIB_MMA_SIZE_16_BIT),  // Operation 0 period
      ASSIGN(C_BSWPER   , FFTLIB_MMA_SIZE_16_BIT),  // B bank switch period
      ASSIGN(C_CRSWPER  , 0), // C bank switch period for read instructions
      ASSIGN(C_CWSWPER  , 0), // C bank switch period for computation writes
      ASSIGN(C_CRRSTPER , FFTLIB_MMA_SIZE_16_BIT), // C read row offset reset period
      ASSIGN(C_CWRSTPER , FFTLIB_MMA_SIZE_16_BIT), // C write row offset reset period for computations

      ASSIGN(X_ReLU  , 0x0), // Enable Rectified Linear Units non-linearity after optional saturation
      ASSIGN(X_PSAT  , 0),
      ASSIGN(X_SAT_MIN_5_0 , 0),
      ASSIGN(X_SAT   , 0x1),  // Enable saturation in the transfer buffer element type after optional rounding
      ASSIGN(X_SAT_MIN_12_6 , 0),
      ASSIGN(X_RE    , 0x1),   // Enable routing via 1/2 LSB addition after shifting
      ASSIGN(X_SAT_MIN_15_13, 0),
      ASSIGN(X_RANGE, __MMA_X_CONFIG_RANGE_DISABLE_NOINIT),
      ASSIGN(X_SCALE_SHIFT_CTRL, __MMA_X_CONFIG_SCALE_SHIFT_CTRL_DISABLE),
      ASSIGN(X_RSVD3 , 0),
      ASSIGN(X_SHIFT , 0), // 7 bits Right shift amount), signed or unsigned depending on CTYPE fieldASSIGN(
      ASSIGN(X_VPACKN , __MMA_X_CONFIG_VPACKN_DISABLE),
      ASSIGN(X_XTYPE , __MMA_X_CONFIG_XTYPE_INT16), // Transfer buffer element typeASSIGN( Not all combinations of CTYPE and XTYPE are supported
      ASSIGN(X_SAT_MAX_3_0 , 0),
      ASSIGN(X_CTYPE , __MMA_X_CONFIG_CTYPE_INT64), // C matrix element typeASSIGN(  This must be consistent with the B FSM setting
      ASSIGN(X_SAT_MAX_8_4 , 0),
      ASSIGN(X_CSWPER , FFTLIB_MMA_SIZE_16_BIT), // C read bank switch period
      ASSIGN(X_CRRSTPER , FFTLIB_MMA_SIZE_16_BIT),     // C read row offset reset period
      ASSIGN(X_COFFSET  , 0x0),          // C matrix row read address offset
      ASSIGN(X_CSTART   , 0x0),          // Initial C bank selection
      ASSIGN(X_SAT_MAX_15_9 , 0x0),

      ASSIGN(RSVD , 0),
      ASSIGN(PARITYCTRL , __MMA_NORMAL)
};

const FFTLIB_MMA_CONFIG_REG configRegisterStruct_i16u_i16s_o16u =
{
      ASSIGN(A_ATYPE  , __MMA_A_CONFIG_ATYPE_UINT16),
      ASSIGN(A_RSVD1  , 0),
      ASSIGN(A_ALUTEN , __MMA_A_CONFIG_NOLUT),
      ASSIGN(A_RSVD2  , 0),
      ASSIGN(A_ARF_CTRL , __MMA_A_CONFIG_ARF_DISABLE),
      ASSIGN(A_ARF_BASE , 0),
      ASSIGN(A_RSVD3  , 0),
      ASSIGN(A_ARF_SIZE , 0),
      ASSIGN(A_RSVD4  , 0),

      ASSIGN(B_BSWPER  , FFTLIB_MMA_SIZE_16_BIT), // 32 bits
      ASSIGN(B_BRSTPER , FFTLIB_MMA_SIZE_16_BIT), // 8 bits
      ASSIGN(B_BTYPE   , __MMA_B_CONFIG_SIZE16), // 2 bits
      ASSIGN(B_LDBOPT  , __MMA_LDBOPT_MMA1),
      ASSIGN(B_B4EXP   , __MMA_B_CONFIG_B4EXP_MMA1),
      ASSIGN(B_RSVD1   , 0),
      ASSIGN(B_ORDER   , __MMA_B_CONFIG_ROW),     // 1 bit
      ASSIGN(B_RSVD2   , 0),
      ASSIGN(B_BSTART  , 0),    // 1 bits
      ASSIGN(B_BCNT1_ENABLE , 0),
      ASSIGN(B_RSVD3   , 0),
      ASSIGN(B_BOFFSET , 0),  // 8 bits
      ASSIGN(B_RSVD4   , 0),

      ASSIGN(C_ATYPE , __MMA_C_CONFIG_ATYPE_UA),
      ASSIGN(C_ARF_BASE , 0),
      ASSIGN(C_ARF_C7 , 0),
      ASSIGN(C_BTYPE , __MMA_C_CONFIG_BTYPE_INT16),
      ASSIGN(C_RSVD2 , 0),
      ASSIGN(C_OPERATION0 , __MMA_C_CONFIG_MUL),
      ASSIGN(C_LOP0,   __MMA_C_CONFIG_LOP_C),
      ASSIGN(C_RSVD3 , 0),
      ASSIGN(C_OPERATION1 , __MMA_C_CONFIG_MULPLUS),
      ASSIGN(C_LOP1,   __MMA_C_CONFIG_LOP_C),
      ASSIGN(C_BIASORDER , __MMA_C_CONFIG_BIAS_ORDER_COLUMN),
      ASSIGN(C_RSVD4 , 0),
      ASSIGN(C_HWLDDST , __MMA_C_CONFIG_HWLDDST_X4_0),
      ASSIGN(C_RSVD5 , 0),
      ASSIGN(C_HWLDTYPE , __MMA_C_CONFIG_HWLDTYPE_INT16),
      ASSIGN(C_RSVD6 , 0),
      ASSIGN(C_OPSTART  , __MMA_C_CONFIG_OPSTART_OPERATION0), // No enum in MMA spec? Initial C operand selections
      ASSIGN(C_BSTART   , 0x0),  // Initial B bank selection for reading B matrix data
      ASSIGN(C_CRSTART  , 0x0), // Initial C bank selection for reading operands
      ASSIGN(C_CWSTART  , 0x0), // Initial C bank selection for writing computation results
      ASSIGN(C_CLSTART  , 0x0), // Initial C bank selection for writing operands from HWALD*
      ASSIGN(C_RSVD7    , 0),
      ASSIGN(C_CROFFSET , 0x0),       // 6-bits C row read offset
      ASSIGN(C_RSVD8    , 0),
      ASSIGN(C_CWOFFSET , 0x0),       // C row write offset for computations
      ASSIGN(C_RSVD9    , 0),
      ASSIGN(C_CLOFFSET , 0x0),       // C row write offset for HWALD* instructions
      ASSIGN(C_RSVD10   , 0),
      ASSIGN(C_CLSWPER  , 0),  // C bank switch period for HWALD* instruction writes
      ASSIGN(C_CLRSTPER , 0), // C write row offset reset period for HWALD*
      ASSIGN(C_OP1PER   , 0),  // Operation 1 period
      ASSIGN(C_OP0PER   , FFTLIB_MMA_SIZE_16_BIT),  // Operation 0 period
      ASSIGN(C_BSWPER   , FFTLIB_MMA_SIZE_16_BIT),  // B bank switch period
      ASSIGN(C_CRSWPER  , 0), // C bank switch period for read instructions
      ASSIGN(C_CWSWPER  , 0), // C bank switch period for computation writes
      ASSIGN(C_CRRSTPER , FFTLIB_MMA_SIZE_16_BIT), // C read row offset reset period
      ASSIGN(C_CWRSTPER , FFTLIB_MMA_SIZE_16_BIT), // C write row offset reset period for computations

      ASSIGN(X_ReLU  , 0x1), // Enable Rectified Linear Units non-linearity after optional saturation
      ASSIGN(X_PSAT  , 0),
      ASSIGN(X_SAT_MIN_5_0 , 0),
      ASSIGN(X_SAT   , 0x1),  // Enable saturation in the transfer buffer element type after optional rounding
      ASSIGN(X_SAT_MIN_12_6 , 0),
      ASSIGN(X_RE    , 0x1),   // Enable routing via 1/2 LSB addition after shifting
      ASSIGN(X_SAT_MIN_15_13, 0),
      ASSIGN(X_RANGE, __MMA_X_CONFIG_RANGE_DISABLE_NOINIT),
      ASSIGN(X_SCALE_SHIFT_CTRL, __MMA_X_CONFIG_SCALE_SHIFT_CTRL_DISABLE),
      ASSIGN(X_RSVD3 , 0),
      ASSIGN(X_SHIFT , 0), // 7 bits Right shift amount), signed or unsigned depending on CTYPE fieldASSIGN(
      ASSIGN(X_VPACKN , __MMA_X_CONFIG_VPACKN_DISABLE),
      ASSIGN(X_XTYPE , __MMA_X_CONFIG_XTYPE_UINT16), // Transfer buffer element typeASSIGN( Not all combinations of CTYPE and XTYPE are supported
      ASSIGN(X_SAT_MAX_3_0 , 0),
      ASSIGN(X_CTYPE , __MMA_X_CONFIG_CTYPE_INT64), // C matrix element typeASSIGN(  This must be consistent with the B FSM setting
      ASSIGN(X_SAT_MAX_8_4 , 0),
      ASSIGN(X_CSWPER , FFTLIB_MMA_SIZE_16_BIT), // C read bank switch period
      ASSIGN(X_CRRSTPER , FFTLIB_MMA_SIZE_16_BIT),     // C read row offset reset period
      ASSIGN(X_COFFSET  , 0x0),          // C matrix row read address offset
      ASSIGN(X_CSTART   , 0x0),          // Initial C bank selection
      ASSIGN(X_SAT_MAX_15_9 , 0x0),

      ASSIGN(RSVD , 0),
      ASSIGN(PARITYCTRL , __MMA_NORMAL)
};


/********************************
 * Typical 8-bit configurations *
 ********************************/

const FFTLIB_MMA_CONFIG_REG configRegisterStruct_i8s_i8s_o8s =
{
      ASSIGN(A_ATYPE  , __MMA_A_CONFIG_ATYPE_INT8),
      ASSIGN(A_RSVD1  , 0),
      ASSIGN(A_ALUTEN , __MMA_A_CONFIG_NOLUT),
      ASSIGN(A_RSVD2  , 0),
      ASSIGN(A_ARF_CTRL , __MMA_A_CONFIG_ARF_DISABLE),
      ASSIGN(A_ARF_BASE , 0),
      ASSIGN(A_RSVD3  , 0),
      ASSIGN(A_ARF_SIZE , 0),
      ASSIGN(A_RSVD4  , 0),

      ASSIGN(B_BSWPER  , FFTLIB_MMA_SIZE_8_BIT), // 32 bits
      ASSIGN(B_BRSTPER , FFTLIB_MMA_SIZE_8_BIT), // 8 bits
      ASSIGN(B_BTYPE   , __MMA_B_CONFIG_SIZE8), // 2 bits
      ASSIGN(B_LDBOPT  , __MMA_LDBOPT_MMA1),
      ASSIGN(B_B4EXP   , __MMA_B_CONFIG_B4EXP_MMA1),
      ASSIGN(B_RSVD1   , 0),
      ASSIGN(B_ORDER   , __MMA_B_CONFIG_ROW),     // 1 bit
      ASSIGN(B_RSVD2   , 0),
      ASSIGN(B_BSTART  , 0),    // 1 bits
      ASSIGN(B_BCNT1_ENABLE , 0),
      ASSIGN(B_RSVD3   , 0),
      ASSIGN(B_BOFFSET , 0),  // 8 bits
      ASSIGN(B_RSVD4   , 0),

      ASSIGN(C_ATYPE , __MMA_C_CONFIG_ATYPE_SA),
      ASSIGN(C_ARF_BASE , 0),
      ASSIGN(C_ARF_C7 , 0),
      ASSIGN(C_BTYPE , __MMA_C_CONFIG_BTYPE_INT8),
      ASSIGN(C_RSVD2 , 0),
      ASSIGN(C_OPERATION0 , __MMA_C_CONFIG_MUL),
      ASSIGN(C_LOP0,   __MMA_C_CONFIG_LOP_C),
      ASSIGN(C_RSVD3 , 0),
      ASSIGN(C_OPERATION1 , __MMA_C_CONFIG_MULPLUS),
      ASSIGN(C_LOP1,   __MMA_C_CONFIG_LOP_C),
      ASSIGN(C_BIASORDER , __MMA_C_CONFIG_BIAS_ORDER_COLUMN),
      ASSIGN(C_RSVD4 , 0),
      ASSIGN(C_HWLDDST , __MMA_C_CONFIG_HWLDDST_X4_0),
      ASSIGN(C_RSVD5 , 0),
      ASSIGN(C_HWLDTYPE , __MMA_C_CONFIG_HWLDTYPE_INT8),
      ASSIGN(C_RSVD6 , 0),

      ASSIGN(C_OPSTART  , __MMA_C_CONFIG_OPSTART_OPERATION0), // No enum in MMA spec? Initial C operand selections
      ASSIGN(C_BSTART   , 0x0),  // Initial B bank selection for reading B matrix data
      ASSIGN(C_CRSTART  , 0x0), // Initial C bank selection for reading operands
      ASSIGN(C_CWSTART  , 0x0), // Initial C bank selection for writing computation results
      ASSIGN(C_CLSTART  , 0x0), // Initial C bank selection for writing operands from HWALD*
      ASSIGN(C_RSVD7    , 0),
      ASSIGN(C_CROFFSET , 0x0),       // 6-bits C row read offset
      ASSIGN(C_RSVD8    , 0),
      ASSIGN(C_CWOFFSET , 0x0),       // C row write offset for computations
      ASSIGN(C_RSVD9    , 0),
      ASSIGN(C_CLOFFSET , 0x0),       // C row write offset for HWALD* instructions
      ASSIGN(C_RSVD10   , 0),
      ASSIGN(C_CLSWPER  , 0),  // C bank switch period for HWALD* instruction writes
      ASSIGN(C_CLRSTPER , 0), // C write row offset reset period for HWALD*
      ASSIGN(C_OP1PER   , 0),  // Operation 1 period
      ASSIGN(C_OP0PER   , FFTLIB_MMA_SIZE_8_BIT),  // Operation 0 period
      ASSIGN(C_BSWPER   , FFTLIB_MMA_SIZE_8_BIT),  // B bank switch period
      ASSIGN(C_CRSWPER  , 0), // C bank switch period for read instructions
      ASSIGN(C_CWSWPER  , 0), // C bank switch period for computation writes
      ASSIGN(C_CRRSTPER , FFTLIB_MMA_SIZE_8_BIT), // C read row offset reset period
      ASSIGN(C_CWRSTPER , FFTLIB_MMA_SIZE_8_BIT), // C write row offset reset period for computations

      ASSIGN(X_ReLU  , 0x0), // Enable Rectified Linear Units non-linearity after optional saturation
      ASSIGN(X_PSAT  , 0),
      ASSIGN(X_SAT_MIN_5_0 , 0),
      ASSIGN(X_SAT   , 0x1),  // Enable saturation in the transfer buffer element type after optional rounding
      ASSIGN(X_SAT_MIN_12_6 , 0),
      ASSIGN(X_RE    , 0x1),   // Enable routing via 1/2 LSB addition after shifting
      ASSIGN(X_SAT_MIN_15_13, 0),
      ASSIGN(X_RANGE, __MMA_X_CONFIG_RANGE_DISABLE_NOINIT),
      ASSIGN(X_SCALE_SHIFT_CTRL, __MMA_X_CONFIG_SCALE_SHIFT_CTRL_DISABLE),
      ASSIGN(X_RSVD3 , 0),
      ASSIGN(X_SHIFT , 0), // 7 bits Right shift amount), signed or unsigned depending on CTYPE fieldASSIGN(
      ASSIGN(X_VPACKN , __MMA_X_CONFIG_VPACKN_DISABLE),
      ASSIGN(X_XTYPE , __MMA_X_CONFIG_XTYPE_INT8), // Transfer buffer element typeASSIGN( Not all combinations of CTYPE and XTYPE are supported
      ASSIGN(X_SAT_MAX_3_0 , 0),
      ASSIGN(X_CTYPE , __MMA_X_CONFIG_CTYPE_INT32), // C matrix element typeASSIGN(  This must be consistent with the B FSM setting
      ASSIGN(X_SAT_MAX_8_4 , 0),
      ASSIGN(X_CSWPER , FFTLIB_MMA_SIZE_8_BIT), // C read bank switch period
      ASSIGN(X_CRRSTPER , FFTLIB_MMA_SIZE_8_BIT),     // C read row offset reset period
      ASSIGN(X_COFFSET  , 0x0),          // C matrix row read address offset
      ASSIGN(X_CSTART   , 0x0),          // Initial C bank selection
      ASSIGN(X_SAT_MAX_15_9 , 0x0),

      ASSIGN(RSVD , 0),
      ASSIGN(PARITYCTRL , __MMA_NORMAL)
};


const FFTLIB_MMA_CONFIG_REG configRegisterStruct_i8s_i8s_o8u =
{
      ASSIGN(A_ATYPE  , __MMA_A_CONFIG_ATYPE_INT8),
      ASSIGN(A_RSVD1  , 0),
      ASSIGN(A_ALUTEN , __MMA_A_CONFIG_NOLUT),
      ASSIGN(A_RSVD2  , 0),
      ASSIGN(A_ARF_CTRL , __MMA_A_CONFIG_ARF_DISABLE),
      ASSIGN(A_ARF_BASE , 0),
      ASSIGN(A_RSVD3  , 0),
      ASSIGN(A_ARF_SIZE , 0),
      ASSIGN(A_RSVD4  , 0),

      ASSIGN(B_BSWPER  , FFTLIB_MMA_SIZE_8_BIT), // 32 bits
      ASSIGN(B_BRSTPER , FFTLIB_MMA_SIZE_8_BIT), // 8 bits
      ASSIGN(B_BTYPE   , __MMA_B_CONFIG_SIZE8), // 2 bits
      ASSIGN(B_LDBOPT  , __MMA_LDBOPT_MMA1),
      ASSIGN(B_B4EXP   , __MMA_B_CONFIG_B4EXP_MMA1),
      ASSIGN(B_RSVD1   , 0),
      ASSIGN(B_ORDER   , __MMA_B_CONFIG_ROW),     // 1 bit
      ASSIGN(B_RSVD2   , 0),
      ASSIGN(B_BSTART  , 0),    // 1 bits
      ASSIGN(B_BCNT1_ENABLE , 0),
      ASSIGN(B_RSVD3   , 0),
      ASSIGN(B_BOFFSET , 0),  // 8 bits
      ASSIGN(B_RSVD4   , 0),

      ASSIGN(C_ATYPE , __MMA_C_CONFIG_ATYPE_SA),
      ASSIGN(C_ARF_BASE , 0),
      ASSIGN(C_ARF_C7 , 0),
      ASSIGN(C_BTYPE , __MMA_C_CONFIG_BTYPE_INT8),
      ASSIGN(C_RSVD2 , 0),
      ASSIGN(C_OPERATION0 , __MMA_C_CONFIG_MUL),
      ASSIGN(C_LOP0,   __MMA_C_CONFIG_LOP_C),
      ASSIGN(C_RSVD3 , 0),
      ASSIGN(C_OPERATION1 , __MMA_C_CONFIG_MULPLUS),
      ASSIGN(C_LOP1,   __MMA_C_CONFIG_LOP_C),
      ASSIGN(C_BIASORDER , __MMA_C_CONFIG_BIAS_ORDER_COLUMN),
      ASSIGN(C_RSVD4 , 0),
      ASSIGN(C_HWLDDST , __MMA_C_CONFIG_HWLDDST_X4_0),
      ASSIGN(C_RSVD5 , 0),
      ASSIGN(C_HWLDTYPE , __MMA_C_CONFIG_HWLDTYPE_INT8),
      ASSIGN(C_RSVD6 , 0),

      ASSIGN(C_OPSTART  , __MMA_C_CONFIG_OPSTART_OPERATION0), // No enum in MMA spec? Initial C operand selections
      ASSIGN(C_BSTART   , 0x0),  // Initial B bank selection for reading B matrix data
      ASSIGN(C_CRSTART  , 0x0), // Initial C bank selection for reading operands
      ASSIGN(C_CWSTART  , 0x0), // Initial C bank selection for writing computation results
      ASSIGN(C_CLSTART  , 0x0), // Initial C bank selection for writing operands from HWALD*
      ASSIGN(C_RSVD7    , 0),
      ASSIGN(C_CROFFSET , 0x0),       // 6-bits C row read offset
      ASSIGN(C_RSVD8    , 0),
      ASSIGN(C_CWOFFSET , 0x0),       // C row write offset for computations
      ASSIGN(C_RSVD9    , 0),
      ASSIGN(C_CLOFFSET , 0x0),       // C row write offset for HWALD* instructions
      ASSIGN(C_RSVD10   , 0),
      ASSIGN(C_CLSWPER  , 0),  // C bank switch period for HWALD* instruction writes
      ASSIGN(C_CLRSTPER , 0), // C write row offset reset period for HWALD*
      ASSIGN(C_OP1PER   , 0),  // Operation 1 period
      ASSIGN(C_OP0PER   , FFTLIB_MMA_SIZE_8_BIT),  // Operation 0 period
      ASSIGN(C_BSWPER   , FFTLIB_MMA_SIZE_8_BIT),  // B bank switch period
      ASSIGN(C_CRSWPER  , 0), // C bank switch period for read instructions
      ASSIGN(C_CWSWPER  , 0), // C bank switch period for computation writes
      ASSIGN(C_CRRSTPER , FFTLIB_MMA_SIZE_8_BIT), // C read row offset reset period
      ASSIGN(C_CWRSTPER , FFTLIB_MMA_SIZE_8_BIT), // C write row offset reset period for computations

      ASSIGN(X_ReLU  , 0x1), // Enable Rectified Linear Units non-linearity after optional saturation
      ASSIGN(X_PSAT  , 0),
      ASSIGN(X_SAT_MIN_5_0 , 0),
      ASSIGN(X_SAT   , 0x0),  // Enable saturation in the transfer buffer element type after optional rounding
      ASSIGN(X_SAT_MIN_12_6 , 0),
      ASSIGN(X_RE    , 0x1),   // Enable routing via 1/2 LSB addition after shifting
      ASSIGN(X_SAT_MIN_15_13, 0),
      ASSIGN(X_RANGE, __MMA_X_CONFIG_RANGE_DISABLE_NOINIT),
      ASSIGN(X_SCALE_SHIFT_CTRL, __MMA_X_CONFIG_SCALE_SHIFT_CTRL_DISABLE),
      ASSIGN(X_RSVD3 , 0),
      ASSIGN(X_SHIFT , 0), // 7 bits Right shift amount), signed or unsigned depending on CTYPE fieldASSIGN(
      ASSIGN(X_VPACKN , __MMA_X_CONFIG_VPACKN_DISABLE),
      ASSIGN(X_XTYPE , __MMA_X_CONFIG_XTYPE_UINT8), // Transfer buffer element typeASSIGN( Not all combinations of CTYPE and XTYPE are supported
      ASSIGN(X_SAT_MAX_3_0 , 0),
      ASSIGN(X_CTYPE , __MMA_X_CONFIG_CTYPE_INT32), // C matrix element typeASSIGN(  This must be consistent with the B FSM setting
      ASSIGN(X_SAT_MAX_8_4 , 0),
      ASSIGN(X_CSWPER , FFTLIB_MMA_SIZE_8_BIT), // C read bank switch period
      ASSIGN(X_CRRSTPER , FFTLIB_MMA_SIZE_8_BIT),     // C read row offset reset period
      ASSIGN(X_COFFSET  , 0x0),          // C matrix row read address offset
      ASSIGN(X_CSTART   , 0x0),          // Initial C bank selection
      ASSIGN(X_SAT_MAX_15_9 , 0x0),

      ASSIGN(RSVD , 0),
      ASSIGN(PARITYCTRL , __MMA_NORMAL)
};


const FFTLIB_MMA_CONFIG_REG configRegisterStruct_i8u_i8s_o8s =
{
      ASSIGN(A_ATYPE  , __MMA_A_CONFIG_ATYPE_UINT8),
      ASSIGN(A_RSVD1  , 0),
      ASSIGN(A_ALUTEN , __MMA_A_CONFIG_NOLUT),
      ASSIGN(A_RSVD2  , 0),
      ASSIGN(A_ARF_CTRL , __MMA_A_CONFIG_ARF_DISABLE),
      ASSIGN(A_ARF_BASE , 0),
      ASSIGN(A_RSVD3  , 0),
      ASSIGN(A_ARF_SIZE , 0),
      ASSIGN(A_RSVD4  , 0),

      ASSIGN(B_BSWPER  , FFTLIB_MMA_SIZE_8_BIT), // 32 bits
      ASSIGN(B_BRSTPER , FFTLIB_MMA_SIZE_8_BIT), // 8 bits
      ASSIGN(B_BTYPE   , __MMA_B_CONFIG_SIZE8), // 2 bits
      ASSIGN(B_LDBOPT  , __MMA_LDBOPT_MMA1),
      ASSIGN(B_B4EXP   , __MMA_B_CONFIG_B4EXP_MMA1),
      ASSIGN(B_RSVD1   , 0),
      ASSIGN(B_ORDER   , __MMA_B_CONFIG_ROW),     // 1 bit
      ASSIGN(B_RSVD2   , 0),
      ASSIGN(B_BSTART  , 0),    // 1 bits
      ASSIGN(B_BCNT1_ENABLE , 0),
      ASSIGN(B_RSVD3   , 0),
      ASSIGN(B_BOFFSET , 0),  // 8 bits
      ASSIGN(B_RSVD4   , 0),

      ASSIGN(C_ATYPE , __MMA_C_CONFIG_ATYPE_UA),
      ASSIGN(C_ARF_BASE , 0),
      ASSIGN(C_ARF_C7 , 0),
      ASSIGN(C_BTYPE , __MMA_C_CONFIG_BTYPE_INT8),
      ASSIGN(C_RSVD2 , 0),
      ASSIGN(C_OPERATION0 , __MMA_C_CONFIG_MUL),
      ASSIGN(C_LOP0,   __MMA_C_CONFIG_LOP_C),
      ASSIGN(C_RSVD3 , 0),
      ASSIGN(C_OPERATION1 , __MMA_C_CONFIG_MULPLUS),
      ASSIGN(C_LOP1,   __MMA_C_CONFIG_LOP_C),
      ASSIGN(C_BIASORDER , __MMA_C_CONFIG_BIAS_ORDER_COLUMN),
      ASSIGN(C_RSVD4 , 0),
      ASSIGN(C_HWLDDST , __MMA_C_CONFIG_HWLDDST_X4_0),
      ASSIGN(C_RSVD5 , 0),
      ASSIGN(C_HWLDTYPE , __MMA_C_CONFIG_HWLDTYPE_INT8),
      ASSIGN(C_RSVD6 , 0),

      ASSIGN(C_OPSTART  , __MMA_C_CONFIG_OPSTART_OPERATION0), // No enum in MMA spec? Initial C operand selections
      ASSIGN(C_BSTART   , 0x0),  // Initial B bank selection for reading B matrix data
      ASSIGN(C_CRSTART  , 0x0), // Initial C bank selection for reading operands
      ASSIGN(C_CWSTART  , 0x0), // Initial C bank selection for writing computation results
      ASSIGN(C_CLSTART  , 0x0), // Initial C bank selection for writing operands from HWALD*
      ASSIGN(C_RSVD7    , 0),
      ASSIGN(C_CROFFSET , 0x0),       // 6-bits C row read offset
      ASSIGN(C_RSVD8    , 0),
      ASSIGN(C_CWOFFSET , 0x0),       // C row write offset for computations
      ASSIGN(C_RSVD9    , 0),
      ASSIGN(C_CLOFFSET , 0x0),       // C row write offset for HWALD* instructions
      ASSIGN(C_RSVD10   , 0),
      ASSIGN(C_CLSWPER  , 0),  // C bank switch period for HWALD* instruction writes
      ASSIGN(C_CLRSTPER , 0), // C write row offset reset period for HWALD*
      ASSIGN(C_OP1PER   , 0),  // Operation 1 period
      ASSIGN(C_OP0PER   , FFTLIB_MMA_SIZE_8_BIT),  // Operation 0 period
      ASSIGN(C_BSWPER   , FFTLIB_MMA_SIZE_8_BIT),  // B bank switch period
      ASSIGN(C_CRSWPER  , 0), // C bank switch period for read instructions
      ASSIGN(C_CWSWPER  , 0), // C bank switch period for computation writes
      ASSIGN(C_CRRSTPER , FFTLIB_MMA_SIZE_8_BIT), // C read row offset reset period
      ASSIGN(C_CWRSTPER , FFTLIB_MMA_SIZE_8_BIT), // C write row offset reset period for computations

      ASSIGN(X_ReLU  , 0x0), // Enable Rectified Linear Units non-linearity after optional saturation
      ASSIGN(X_PSAT  , 0),
      ASSIGN(X_SAT_MIN_5_0 , 0),
      ASSIGN(X_SAT   , 0x1),  // Enable saturation in the transfer buffer element type after optional rounding
      ASSIGN(X_SAT_MIN_12_6 , 0),
      ASSIGN(X_RE    , 0x1),   // Enable routing via 1/2 LSB addition after shifting
      ASSIGN(X_SAT_MIN_15_13, 0),
      ASSIGN(X_RANGE, __MMA_X_CONFIG_RANGE_DISABLE_NOINIT),
      ASSIGN(X_SCALE_SHIFT_CTRL, __MMA_X_CONFIG_SCALE_SHIFT_CTRL_DISABLE),
      ASSIGN(X_RSVD3 , 0),
      ASSIGN(X_SHIFT , 0), // 7 bits Right shift amount), signed or unsigned depending on CTYPE fieldASSIGN(
      ASSIGN(X_VPACKN , __MMA_X_CONFIG_VPACKN_DISABLE),
      ASSIGN(X_XTYPE , __MMA_X_CONFIG_XTYPE_INT8), // Transfer buffer element typeASSIGN( Not all combinations of CTYPE and XTYPE are supported
      ASSIGN(X_SAT_MAX_3_0 , 0),
      ASSIGN(X_CTYPE , __MMA_X_CONFIG_CTYPE_INT32), // C matrix element typeASSIGN(  This must be consistent with the B FSM setting
      ASSIGN(X_SAT_MAX_8_4 , 0),
      ASSIGN(X_CSWPER , FFTLIB_MMA_SIZE_8_BIT), // C read bank switch period
      ASSIGN(X_CRRSTPER , FFTLIB_MMA_SIZE_8_BIT),     // C read row offset reset period
      ASSIGN(X_COFFSET  , 0x0),          // C matrix row read address offset
      ASSIGN(X_CSTART   , 0x0),          // Initial C bank selection
      ASSIGN(X_SAT_MAX_15_9 , 0x0),

      ASSIGN(RSVD , 0),
      ASSIGN(PARITYCTRL , __MMA_NORMAL)
};

const FFTLIB_MMA_CONFIG_REG configRegisterStruct_i8u_i8s_o8u =
{
      ASSIGN(A_ATYPE  , __MMA_A_CONFIG_ATYPE_UINT8),
      ASSIGN(A_RSVD1  , 0),
      ASSIGN(A_ALUTEN , __MMA_A_CONFIG_NOLUT),
      ASSIGN(A_RSVD2  , 0),
      ASSIGN(A_ARF_CTRL , __MMA_A_CONFIG_ARF_DISABLE),
      ASSIGN(A_ARF_BASE , 0),
      ASSIGN(A_RSVD3  , 0),
      ASSIGN(A_ARF_SIZE , 0),
      ASSIGN(A_RSVD4  , 0),

      ASSIGN(B_BSWPER  , FFTLIB_MMA_SIZE_8_BIT), // 32 bits
      ASSIGN(B_BRSTPER , FFTLIB_MMA_SIZE_8_BIT), // 8 bits
      ASSIGN(B_BTYPE   , __MMA_B_CONFIG_SIZE8), // 2 bits
      ASSIGN(B_LDBOPT  , __MMA_LDBOPT_MMA1),
      ASSIGN(B_B4EXP   , __MMA_B_CONFIG_B4EXP_MMA1),
      ASSIGN(B_RSVD1   , 0),
      ASSIGN(B_ORDER   , __MMA_B_CONFIG_ROW),     // 1 bit
      ASSIGN(B_RSVD2   , 0),
      ASSIGN(B_BSTART  , 0),    // 1 bits
      ASSIGN(B_BCNT1_ENABLE , 0),
      ASSIGN(B_RSVD3   , 0),
      ASSIGN(B_BOFFSET , 0),  // 8 bits
      ASSIGN(B_RSVD4   , 0),

      ASSIGN(C_ATYPE , __MMA_C_CONFIG_ATYPE_UA),
      ASSIGN(C_ARF_BASE , 0),
      ASSIGN(C_ARF_C7 , 0),
      ASSIGN(C_BTYPE , __MMA_C_CONFIG_BTYPE_INT8),
      ASSIGN(C_RSVD2 , 0),
      ASSIGN(C_OPERATION0 , __MMA_C_CONFIG_MUL),
      ASSIGN(C_LOP0,   __MMA_C_CONFIG_LOP_C),
      ASSIGN(C_RSVD3 , 0),
      ASSIGN(C_OPERATION1 , __MMA_C_CONFIG_MULPLUS),
      ASSIGN(C_LOP1,   __MMA_C_CONFIG_LOP_C),
      ASSIGN(C_BIASORDER , __MMA_C_CONFIG_BIAS_ORDER_COLUMN),
      ASSIGN(C_RSVD4 , 0),
      ASSIGN(C_HWLDDST , __MMA_C_CONFIG_HWLDDST_X4_0),
      ASSIGN(C_RSVD5 , 0),
      ASSIGN(C_HWLDTYPE , __MMA_C_CONFIG_HWLDTYPE_INT8),
      ASSIGN(C_RSVD6 , 0),
      ASSIGN(C_OPSTART  , __MMA_C_CONFIG_OPSTART_OPERATION0), // No enum in MMA spec? Initial C operand selections
      ASSIGN(C_BSTART   , 0x0),  // Initial B bank selection for reading B matrix data
      ASSIGN(C_CRSTART  , 0x0), // Initial C bank selection for reading operands
      ASSIGN(C_CWSTART  , 0x0), // Initial C bank selection for writing computation results
      ASSIGN(C_CLSTART  , 0x0), // Initial C bank selection for writing operands from HWALD*
      ASSIGN(C_RSVD7    , 0),
      ASSIGN(C_CROFFSET , 0x0),       // 6-bits C row read offset
      ASSIGN(C_RSVD8    , 0),
      ASSIGN(C_CWOFFSET , 0x0),       // C row write offset for computations
      ASSIGN(C_RSVD9    , 0),
      ASSIGN(C_CLOFFSET , 0x0),       // C row write offset for HWALD* instructions
      ASSIGN(C_RSVD10   , 0),
      ASSIGN(C_CLSWPER  , 0),  // C bank switch period for HWALD* instruction writes
      ASSIGN(C_CLRSTPER , 0), // C write row offset reset period for HWALD*
      ASSIGN(C_OP1PER   , 0),  // Operation 1 period
      ASSIGN(C_OP0PER   , FFTLIB_MMA_SIZE_8_BIT),  // Operation 0 period
      ASSIGN(C_BSWPER   , FFTLIB_MMA_SIZE_8_BIT),  // B bank switch period
      ASSIGN(C_CRSWPER  , 0), // C bank switch period for read instructions
      ASSIGN(C_CWSWPER  , 0), // C bank switch period for computation writes
      ASSIGN(C_CRRSTPER , FFTLIB_MMA_SIZE_8_BIT), // C read row offset reset period
      ASSIGN(C_CWRSTPER , FFTLIB_MMA_SIZE_8_BIT), // C write row offset reset period for computations

      ASSIGN(X_ReLU  , 0x1), // Enable Rectified Linear Units non-linearity after optional saturation
      ASSIGN(X_PSAT  , 0),
      ASSIGN(X_SAT_MIN_5_0 , 0),
      ASSIGN(X_SAT   , 0x1),  // Enable saturation in the transfer buffer element type after optional rounding
      ASSIGN(X_SAT_MIN_12_6 , 0),
      ASSIGN(X_RE    , 0x1),   // Enable routing via 1/2 LSB addition after shifting
      ASSIGN(X_SAT_MIN_15_13, 0),
      ASSIGN(X_RANGE, __MMA_X_CONFIG_RANGE_DISABLE_NOINIT),
      ASSIGN(X_SCALE_SHIFT_CTRL, __MMA_X_CONFIG_SCALE_SHIFT_CTRL_DISABLE),
      ASSIGN(X_RSVD3 , 0),
      ASSIGN(X_SHIFT , 0), // 7 bits Right shift amount), signed or unsigned depending on CTYPE fieldASSIGN(
      ASSIGN(X_VPACKN , __MMA_X_CONFIG_VPACKN_DISABLE),
      ASSIGN(X_XTYPE , __MMA_X_CONFIG_XTYPE_UINT8), // Transfer buffer element typeASSIGN( Not all combinations of CTYPE and XTYPE are supported
      ASSIGN(X_SAT_MAX_3_0 , 0),
      ASSIGN(X_CTYPE , __MMA_X_CONFIG_CTYPE_INT32), // C matrix element typeASSIGN(  This must be consistent with the B FSM setting
      ASSIGN(X_SAT_MAX_8_4 , 0),
      ASSIGN(X_CSWPER , FFTLIB_MMA_SIZE_8_BIT), // C read bank switch period
      ASSIGN(X_CRRSTPER , FFTLIB_MMA_SIZE_8_BIT),     // C read row offset reset period
      ASSIGN(X_COFFSET  , 0x0),          // C matrix row read address offset
      ASSIGN(X_CSTART   , 0x0),          // Initial C bank selection
      ASSIGN(X_SAT_MAX_15_9 , 0x0),

      ASSIGN(RSVD , 0),
      ASSIGN(PARITYCTRL , __MMA_NORMAL)
};

/* -------------------------------------------------------------------------- */
/*  MISRAC Rule 4.9(DEFINE.FUNC) Deviation: The advisory is not being         */
/*  addressed so as not  to lose portability across different platforms.      */
/* -------------------------------------------------------------------------- */
#ifdef WIN32
#define ASSIGN(param, value) value
#else
#define ASSIGN(param, value) .param = value
#endif
const __HWA_OFFSET_REG offsetRegStruct_zeros =
{
    ASSIGN(offset0  , 0),
    ASSIGN(offset1  , 0),
    ASSIGN(offset2  , 0),
    ASSIGN(offset3  , 0),
   ASSIGN(A_LUT_VAL_0      , 0),
    ASSIGN(offset4  , 0),
    ASSIGN(offset5  , 0),
    ASSIGN(offset6  , 0),
    ASSIGN(offset7  , 0),
   ASSIGN(A_LUT_VAL_1      , 0),
    ASSIGN(offset8  , 0),
    ASSIGN(offset9  , 0),
    ASSIGN(offset10 , 0),
    ASSIGN(offset11 , 0),
   ASSIGN(A_LUT_VAL_2      , 0),
    ASSIGN(offset12 , 0),
    ASSIGN(offset13 , 0),
    ASSIGN(offset14 , 0),
    ASSIGN(offset15 , 0),
   ASSIGN(A_LUT_VAL_3      , 0),
    ASSIGN(offset16 , 0),
    ASSIGN(offset17 , 0),
    ASSIGN(offset18 , 0),
    ASSIGN(offset19 , 0),
   ASSIGN(A_LUT_VAL_4      , 0),
    ASSIGN(offset20 , 0),
    ASSIGN(offset21 , 0),
    ASSIGN(offset22 , 0),
    ASSIGN(offset23 , 0),
   ASSIGN(A_LUT_VAL_5      , 0),
    ASSIGN(offset24 , 0),
    ASSIGN(offset25 , 0),
    ASSIGN(offset26 , 0),
    ASSIGN(offset27 , 0),
   ASSIGN(A_LUT_VAL_6      , 0),
    ASSIGN(offset28 , 0),
    ASSIGN(offset29 , 0),
    ASSIGN(offset30 , 0),
    ASSIGN(offset31 , 0),
   ASSIGN(A_LUT_VAL_7      , 0),
    ASSIGN(offset32 , 0),
    ASSIGN(offset33 , 0),
    ASSIGN(offset34 , 0),
    ASSIGN(offset35 , 0),
   ASSIGN(A_LUT_VAL_8      , 0),
    ASSIGN(offset36 , 0),
    ASSIGN(offset37 , 0),
    ASSIGN(offset38 , 0),
    ASSIGN(offset39 , 0),
   ASSIGN(A_LUT_VAL_9      , 0),
    ASSIGN(offset40 , 0),
    ASSIGN(offset41 , 0),
    ASSIGN(offset42 , 0),
    ASSIGN(offset43 , 0),
   ASSIGN(A_LUT_VAL_10     , 0),
    ASSIGN(offset44 , 0),
    ASSIGN(offset45 , 0),
    ASSIGN(offset46 , 0),
    ASSIGN(offset47 , 0),
   ASSIGN(A_LUT_VAL_11     , 0),
    ASSIGN(offset48 , 0),
    ASSIGN(offset49 , 0),
    ASSIGN(offset50 , 0),
    ASSIGN(offset51 , 0),
   ASSIGN(A_LUT_VAL_12     , 0),
    ASSIGN(offset52 , 0),
    ASSIGN(offset53 , 0),
    ASSIGN(offset54 , 0),
    ASSIGN(offset55 , 0),
   ASSIGN(A_LUT_VAL_13     , 0),
    ASSIGN(offset56 , 0),
    ASSIGN(offset57 , 0),
    ASSIGN(offset58 , 0),
    ASSIGN(offset59 , 0),
   ASSIGN(A_LUT_VAL_14     , 0),
    ASSIGN(offset60 , 0),
    ASSIGN(offset61 , 0),
    ASSIGN(offset62 , 0),
    ASSIGN(offset63 , 0),
    ASSIGN(A_LUT_VAL_15     , 0)
};

const __HWA_OFFSET_REG offsetRegStruct_diagonal_32bit =
{
    ASSIGN(offset0  , 0),
    ASSIGN(offset1  , 0),
    ASSIGN(offset2  , 0),
    ASSIGN(offset3  , 0),
   ASSIGN(A_LUT_VAL_0      , 0),
    ASSIGN(offset4  , 1),
    ASSIGN(offset5  , 0),
    ASSIGN(offset6  , 0),
    ASSIGN(offset7  , 0),
   ASSIGN(A_LUT_VAL_1      , 0),
    ASSIGN(offset8  , 2),
    ASSIGN(offset9  , 0),
    ASSIGN(offset10 , 0),
    ASSIGN(offset11 , 0),
   ASSIGN(A_LUT_VAL_2      , 0),
    ASSIGN(offset12 , 3),
    ASSIGN(offset13 , 0),
    ASSIGN(offset14 , 0),
    ASSIGN(offset15 , 0),
   ASSIGN(A_LUT_VAL_3      , 0),
    ASSIGN(offset16 , 4),
    ASSIGN(offset17 , 0),
    ASSIGN(offset18 , 0),
    ASSIGN(offset19 , 0),
   ASSIGN(A_LUT_VAL_4      , 0),
    ASSIGN(offset20 , 5),
    ASSIGN(offset21 , 0),
    ASSIGN(offset22 , 0),
    ASSIGN(offset23 , 0),
   ASSIGN(A_LUT_VAL_5      , 0),
    ASSIGN(offset24 , 6),
    ASSIGN(offset25 , 0),
    ASSIGN(offset26 , 0),
    ASSIGN(offset27 , 0),
   ASSIGN(A_LUT_VAL_6      , 0),
    ASSIGN(offset28 , 7),
    ASSIGN(offset29 , 0),
    ASSIGN(offset30 , 0),
    ASSIGN(offset31 , 0),
   ASSIGN(A_LUT_VAL_7      , 0),
    ASSIGN(offset32 , 8),
    ASSIGN(offset33 , 0),
    ASSIGN(offset34 , 0),
    ASSIGN(offset35 , 0),
    ASSIGN(A_LUT_VAL_8      , 0),
    ASSIGN(offset36 , 9),
    ASSIGN(offset37 , 0),
    ASSIGN(offset38 , 0),
    ASSIGN(offset39 , 0),
   ASSIGN(A_LUT_VAL_9      , 0),
    ASSIGN(offset40 , 10),
    ASSIGN(offset41 , 0),
    ASSIGN(offset42 , 0),
    ASSIGN(offset43 , 0),
   ASSIGN(A_LUT_VAL_10      , 0),
    ASSIGN(offset44 , 11),
    ASSIGN(offset45 , 0),
    ASSIGN(offset46 , 0),
    ASSIGN(offset47 , 0),
   ASSIGN(A_LUT_VAL_11      , 0),
    ASSIGN(offset48 , 12),
    ASSIGN(offset49 , 0),
    ASSIGN(offset50 , 0),
    ASSIGN(offset51 , 0),
   ASSIGN(A_LUT_VAL_12      , 0),
    ASSIGN(offset52 , 13),
    ASSIGN(offset53 , 0),
    ASSIGN(offset54 , 0),
    ASSIGN(offset55 , 0),
   ASSIGN(A_LUT_VAL_13      , 0),
    ASSIGN(offset56 , 14),
    ASSIGN(offset57 , 0),
    ASSIGN(offset58 , 0),
    ASSIGN(offset59 , 0),
   ASSIGN(A_LUT_VAL_14      , 0),
    ASSIGN(offset60 , 15),
    ASSIGN(offset61 , 0),
    ASSIGN(offset62 , 0),
    ASSIGN(offset63 , 0),
   ASSIGN(A_LUT_VAL_15      , 0)
};

const __HWA_OFFSET_REG offsetRegStruct_diagonal_16bit =
{
    ASSIGN(offset0  , 0),
    ASSIGN(offset1  , 0),
    ASSIGN(offset2  , 1),
    ASSIGN(offset3  , 0),
     ASSIGN(A_LUT_VAL_0      , 0),
    ASSIGN(offset4  , 2),
    ASSIGN(offset5  , 0),
    ASSIGN(offset6  , 3),
    ASSIGN(offset7  , 0),
     ASSIGN(A_LUT_VAL_1      , 0),
    ASSIGN(offset8  , 4),
    ASSIGN(offset9  , 0),
    ASSIGN(offset10 , 5),
    ASSIGN(offset11 , 0),
     ASSIGN(A_LUT_VAL_2      , 0),
    ASSIGN(offset12 , 6),
    ASSIGN(offset13 , 0),
    ASSIGN(offset14 , 7),
    ASSIGN(offset15 , 0),
     ASSIGN(A_LUT_VAL_3      , 0),
    ASSIGN(offset16 , 8),
    ASSIGN(offset17 , 0),
    ASSIGN(offset18 , 9),
    ASSIGN(offset19 , 0),
     ASSIGN(A_LUT_VAL_4      , 0),
    ASSIGN(offset20 , 10),
    ASSIGN(offset21 , 0),
    ASSIGN(offset22 , 11),
    ASSIGN(offset23 , 0),
     ASSIGN(A_LUT_VAL_5      , 0),
    ASSIGN(offset24 , 12),
    ASSIGN(offset25 , 0),
    ASSIGN(offset26 , 13),
    ASSIGN(offset27 , 0),
     ASSIGN(A_LUT_VAL_6      , 0),
    ASSIGN(offset28 , 14),
    ASSIGN(offset29 , 0),
    ASSIGN(offset30 , 15),
    ASSIGN(offset31 , 0),
     ASSIGN(A_LUT_VAL_7      , 0),
    ASSIGN(offset32 , 16),
    ASSIGN(offset33 , 0),
    ASSIGN(offset34 , 17),
    ASSIGN(offset35 , 0),
     ASSIGN(A_LUT_VAL_8      , 0),
    ASSIGN(offset36 , 18),
    ASSIGN(offset37 , 0),
    ASSIGN(offset38 , 19),
    ASSIGN(offset39 , 0),
     ASSIGN(A_LUT_VAL_9      , 0),
    ASSIGN(offset40 , 20),
    ASSIGN(offset41 , 0),
    ASSIGN(offset42 , 21),
    ASSIGN(offset43 , 0),
     ASSIGN(A_LUT_VAL_10      , 0),
    ASSIGN(offset44 , 22),
    ASSIGN(offset45 , 0),
    ASSIGN(offset46 , 23),
    ASSIGN(offset47 , 0),
     ASSIGN(A_LUT_VAL_11      , 0),
    ASSIGN(offset48 , 24),
    ASSIGN(offset49 , 0),
    ASSIGN(offset50 , 25),
    ASSIGN(offset51 , 0),
     ASSIGN(A_LUT_VAL_12      , 0),
    ASSIGN(offset52 , 26),
    ASSIGN(offset53 , 0),
    ASSIGN(offset54 , 27),
    ASSIGN(offset55 , 0),
     ASSIGN(A_LUT_VAL_13      , 0),
    ASSIGN(offset56 , 28),
    ASSIGN(offset57 , 0),
    ASSIGN(offset58 , 29),
    ASSIGN(offset59 , 0),
     ASSIGN(A_LUT_VAL_14      , 0),
    ASSIGN(offset60 , 30),
    ASSIGN(offset61 , 0),
    ASSIGN(offset62 , 31),
    ASSIGN(offset63 , 0),
     ASSIGN(A_LUT_VAL_15      , 0)
};

const __HWA_OFFSET_REG offsetRegStruct_diagonal_8bit =
{
    ASSIGN(offset0  , 0),
    ASSIGN(offset1  , 1),
    ASSIGN(offset2  , 2),
    ASSIGN(offset3  , 3),
   ASSIGN(A_LUT_VAL_0      , 0),
    ASSIGN(offset4  , 4),
    ASSIGN(offset5  , 5),
    ASSIGN(offset6  , 6),
    ASSIGN(offset7  , 7),
   ASSIGN(A_LUT_VAL_1      , 0),
    ASSIGN(offset8  , 8),
    ASSIGN(offset9  , 9),
    ASSIGN(offset10 , 10),
    ASSIGN(offset11 , 11),
   ASSIGN(A_LUT_VAL_2      , 0),
    ASSIGN(offset12 , 12),
    ASSIGN(offset13 , 13),
    ASSIGN(offset14 , 14),
    ASSIGN(offset15 , 15),
   ASSIGN(A_LUT_VAL_3      , 0),
    ASSIGN(offset16 , 16),
    ASSIGN(offset17 , 17),
    ASSIGN(offset18 , 18),
    ASSIGN(offset19 , 19),
   ASSIGN(A_LUT_VAL_4      , 0),
    ASSIGN(offset20 , 20),
    ASSIGN(offset21 , 21),
    ASSIGN(offset22 , 22),
    ASSIGN(offset23 , 23),
   ASSIGN(A_LUT_VAL_5      , 0),
    ASSIGN(offset24 , 24),
    ASSIGN(offset25 , 25),
    ASSIGN(offset26 , 26),
    ASSIGN(offset27 , 27),
   ASSIGN(A_LUT_VAL_6      , 0),
    ASSIGN(offset28 , 28),
    ASSIGN(offset29 , 29),
    ASSIGN(offset30 , 30),
    ASSIGN(offset31 , 31),
   ASSIGN(A_LUT_VAL_7      , 0),
    ASSIGN(offset32 , 32),
    ASSIGN(offset33 , 33),
    ASSIGN(offset34 , 34),
    ASSIGN(offset35 , 35),
   ASSIGN(A_LUT_VAL_8      , 0),
    ASSIGN(offset36 , 36),
    ASSIGN(offset37 , 37),
    ASSIGN(offset38 , 38),
    ASSIGN(offset39 , 39),
   ASSIGN(A_LUT_VAL_9      , 0),
    ASSIGN(offset40 , 40),
    ASSIGN(offset41 , 41),
    ASSIGN(offset42 , 42),
    ASSIGN(offset43 , 43),
   ASSIGN(A_LUT_VAL_10     , 0),
    ASSIGN(offset44 , 44),
    ASSIGN(offset45 , 45),
    ASSIGN(offset46 , 46),
    ASSIGN(offset47 , 47),
   ASSIGN(A_LUT_VAL_11     , 0),
    ASSIGN(offset48 , 48),
    ASSIGN(offset49 , 49),
    ASSIGN(offset50 , 50),
    ASSIGN(offset51 , 51),
   ASSIGN(A_LUT_VAL_12     , 0),
    ASSIGN(offset52 , 52),
    ASSIGN(offset53 , 53),
    ASSIGN(offset54 , 54),
    ASSIGN(offset55 , 55),
   ASSIGN(A_LUT_VAL_13     , 0),
    ASSIGN(offset56 , 56),
    ASSIGN(offset57 , 57),
    ASSIGN(offset58 , 58),
    ASSIGN(offset59 , 59),
   ASSIGN(A_LUT_VAL_14     , 0),
    ASSIGN(offset60 , 60),
    ASSIGN(offset61 , 61),
    ASSIGN(offset62 , 62),
    ASSIGN(offset63 , 63),
   ASSIGN(A_LUT_VAL_15     , 0)
};


/* ======================================================================== */
/*  End of file:  FFTLIB_configurations.c                                            */
/* ======================================================================== */
