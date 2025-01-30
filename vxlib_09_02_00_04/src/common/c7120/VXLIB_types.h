/**
 ******************************************************************************
 *  @file
 *
 *  @brief    File to hold common structure, enums, macros and functions for
 *            VXLIB
 *
 *  @version  0.1 - Jan 2018 : Initial Version
 *
 *******************************************************************************
 */
#ifndef C7120_VXLIB_TYPES_H_
#define C7120_VXLIB_TYPES_H_ 1

#include <c7x.h> // for streaming engine, streaming address gen.

///* ---------------------------------------------------------------- */
///* Desire C namespace for these defines/includes                    */
///* ---------------------------------------------------------------- */

#ifdef __cplusplus
#include <c7x_scalable.h> // for device scalability

extern "C" {
#endif /* __cplusplus */

/*!
 * \defgroup VXLIB_COMMON Common definitions
 * \brief This module consists of definitions (macros, structures, utility
 *        functions) that are commonly applicable to all VXLIB kernels.
 * \details
 */

/* @{ */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* C7120_VXLIB_TYPES_H_ */

/* ======================================================================== */
/*  End of file:  VXLIB_types.h                                             */
/* ======================================================================== */
