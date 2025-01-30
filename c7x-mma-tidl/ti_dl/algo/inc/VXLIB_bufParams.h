/*
*
* Copyright (c) {2015 - 2020} Texas Instruments Incorporated
*
* All rights reserved not granted herein.
*
* Limited License.
*
* Texas Instruments Incorporated grants a world-wide, royalty-free, non-exclusive
* license under copyrights and patents it now or hereafter owns or controls to make,
* have made, use, import, offer to sell and sell ("Utilize") this software subject to the
* terms herein.  With respect to the foregoing patent license, such license is granted
* solely to the extent that any such patent is necessary to Utilize the software alone.
* The patent license shall not apply to any combinations which include this software,
* other than combinations with devices manufactured by or for TI ("TI Devices").
* No hardware patent is licensed hereunder.
*
* Redistributions must preserve existing copyright notices and reproduce this license
* (including the above copyright notice and the disclaimer and (if applicable) source
* code license limitations below) in the documentation and/or other materials provided
* with the distribution
*
* Redistribution and use in binary form, without modification, are permitted provided
* that the following conditions are met:
*
* *       No reverse engineering, decompilation, or disassembly of this software is
* permitted with respect to any software provided in binary form.
*
* *       any redistribution and use are licensed by TI for use only with TI Devices.
*
* *       Nothing shall obligate TI to provide you with source code for the software
* licensed and provided to you in object code.
*
* If software source code is provided to you, modification and redistribution of the
* source code are permitted provided that the following conditions are met:
*
* *       any redistribution and use of the source code, including any resulting derivative
* works, are licensed by TI for use only with TI Devices.
*
* *       any redistribution and use of any object code compiled from the source code
* and any resulting derivative works, are licensed by TI for use only with TI Devices.
*
* Neither the name of Texas Instruments Incorporated nor the names of its suppliers
*
* may be used to endorse or promote products derived from this software without
* specific prior written permission.
*
* DISCLAIMER.
*
* THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
* OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/
#ifndef VXLIB_BUFPARAMS_H_
#define VXLIB_BUFPARAMS_H_

#ifndef __OPENCL_VERSION__
#include <stdint.h>
#endif

#if defined (__C7100__) || defined (__C7120__) || defined (__C7504__) || defined (__C7524__)
#if !defined(_HOST_BUILD)
 #include <c7x.h>
 #define RESTRICT restrict
 #else
 #include <c7x.h>
 #define RESTRICT
 #endif
#endif

#ifdef _MSC_VER /* If using MS Visual Studio Compiler */
	#define inline            __inline
#endif

/* ----------------------------------------------------------------
 *  MISRAC Rule 8.5 Deviation: This header file contains definitions
 *  for the following inline funtions.  These functions are shared
 *  among more than one VXLIB kernel, so it is better to have them
 *  in a common header file than to replicate the code.
 * ---------------------------------------------------------------- */
// static inline int32_t VXLIB_sizeof(uint32_t type);

/*! \brief The enumeration of all data types. */
typedef enum _VXLIB_data_type_e
{
    /* Standard fixed point data types */
    VXLIB_INT8  = 0,
    VXLIB_INT16 = 1,
    VXLIB_INT32 = 2,
    VXLIB_INT64 = 3,

    VXLIB_UINT8  = 4,
    VXLIB_UINT16 = 5,
    VXLIB_UINT24 = 6,
    VXLIB_UINT32 = 7,
    VXLIB_UINT64 = 8,

    /* Standard floating point data types */
    VXLIB_FLOAT16 = 9,
    VXLIB_FLOAT32 = 10,
    VXLIB_FLOAT64 = 11

} VXLIB_data_type_e;

/*! \brief The enumeration of multidimensional buffer types. */
typedef enum _VXLIB_dimension_e {
    VXLIB_2D= 0,
    VXLIB_3D= 1,
    VXLIB_4D= 2,
    VXLIB_5D= 3,
    VXLIB_6D= 4
} VXLIB_dimension_e;

/*! \brief A structure for a 1 dimensional buffer descriptor. */
typedef struct
{
    uint32_t data_type;    /*!< \brief Values are of type VXLIB_data_type_e. */

    uint32_t dim_x;        /*!< \brief Width of buffer in X dimension in elements. */

} VXLIB_bufParams1D_t;

/*! \brief A structure for a 2 dimensional buffer descriptor. */
typedef struct
{
    uint32_t data_type;     /*!< \brief Values are of type VXLIB_data_type_e. */

    uint32_t dim_x;        /*!< \brief Width of buffer in X dimension in elements. */

    uint32_t dim_y;        /*!< \brief Height of buffer in Y dimension in elements. */
    int32_t  stride_y;     /*!< \brief Stride in Y dimension in bytes. */

} VXLIB_bufParams2D_t;

/*! \brief A structure for a 3 dimensional buffer descriptor. */
typedef struct
{
    uint32_t data_type;     /*!< \brief Values are of type VXLIB_data_type_e. */

    uint32_t dim_x;        /*!< \brief Width of buffer in X dimension in elements. */

    uint32_t dim_y;        /*!< \brief Height of buffer in Y dimension in elements. */
    int32_t  stride_y;     /*!< \brief Stride in Y dimension in bytes. */

    uint32_t dim_z;        /*!< \brief Depth of patch in Z dimension in elements. */
    int32_t  stride_z;     /*!< \brief Stride in Z dimension in bytes. */

} VXLIB_bufParams3D_t;

/*! \brief A structure for a N dimensional buffer descriptor. */
typedef struct
{
    uint32_t data_type;     /*!< \brief Values are of type VXLIB_data_type_e. */

    uint32_t dim_0;

    uint32_t dim_1;
    int32_t  stride_1; /*!< \brief  Stride in bytes */

    uint32_t dim_2;  /*!< \brief  Uninitialized when dimension = {VXLIB_2D} */
    int32_t  stride_2; /*!< \brief  Stride in bytes; Uninitialized when dimension = {VXLIB_2D} */

    uint32_t dim_3;  /*!< \brief  Uninitialized when dimension = {VXLIB_2D, VXLIB_3D} */
    int32_t  stride_3; /*!< \brief  Stride in bytes; Uninitialized when dimension = {VXLIB_2D, VXLIB_3D} */

    uint32_t dim_4;  /*!< \brief  Uninitialized when dimension = {VXLIB_2D, VXLIB_3D, VXLIB_4D} */
    int32_t  stride_4; /*!< \brief  Stride in bytes; Uninitialized when dimension = {VXLIB_2D, VXLIB_3D, VXLIB_4D} */

    uint32_t dim_5;  /*!< \brief  Uninitialized when dimension = {VXLIB_2D, VXLIB_3D, VXLIB_4D, VXLIB_5D} */
    int32_t  stride_5; /*!< \brief  Stride in bytes; Uninitialized when dimension = {VXLIB_2D, VXLIB_3D, VXLIB_4D, VXLIB_5D} */

    uint32_t dimension;    /*!< \brief Values are of type VXLIB_dimension_e. */

} VXLIB_bufParamsND_t;


/*! \brief Inline function returns number of bytes per element given a type of \ref VXLIB_data_type_e. */
static inline int32_t VXLIB_sizeof(uint32_t type)
{

    int32_t size = 1;

    if((type == VXLIB_INT8)  ||
       (type == VXLIB_UINT8))
    {
        size = 1;
    }
    else if((type == VXLIB_INT16)  ||
            (type == VXLIB_UINT16) ||
            (type == VXLIB_FLOAT16))
    {
        size = 2;
    }
    else if(type == VXLIB_UINT24)
    {
        size = 3;
    }
    else if((type == VXLIB_INT32)  ||
            (type == VXLIB_UINT32) ||
            (type == VXLIB_FLOAT32))
    {
        size = 4;
    }
    else if((type == VXLIB_INT64)  ||
            (type == VXLIB_UINT64) ||
            (type == VXLIB_FLOAT64))
    {
        size = 8;
    }
    else {
        size = 1;
    }

    return size;
}

#endif /* VXLIB_BUFPARAMS_H_ */
