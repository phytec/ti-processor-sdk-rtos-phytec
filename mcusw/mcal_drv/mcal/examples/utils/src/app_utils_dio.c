/*
*
* Copyright (c) 2024 Texas Instruments Incorporated
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

/**
 *  \file     app_utils_dio.c
 *
 *  \brief    Includes sections defined in linker command file for DIO module
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "Std_Types.h"
#include "app_utils.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
/* None */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */
/* None */

/* ========================================================================== */
/*                  Internal Function Declarations                            */
/* ========================================================================== */
/* None */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
/**< Sections defined in linker command file for DIO module */
extern uint32 __linker_dio_text_start, __linker_dio_text_end,
              __linker_dio_const_start, __linker_dio_const_end,
              __linker_dio_init_start, __linker_dio_init_end,
              __linker_dio_no_init_start, __linker_dio_no_init_end;

/**< Address list of sections defined in linker command file for DIO module */
static uint32 DioApp_SecHoleAddrList[] =
{
    ((uint32) & __linker_dio_text_start),
    ((uint32) & __linker_dio_text_end) - APP_UTILS_LINKER_FILL_LENGTH,
    ((uint32) & __linker_dio_const_start),
    ((uint32) & __linker_dio_const_end) - APP_UTILS_LINKER_FILL_LENGTH,
    ((uint32) & __linker_dio_init_start),
    ((uint32) & __linker_dio_init_end) - APP_UTILS_LINKER_FILL_LENGTH,
    ((uint32) & __linker_dio_no_init_start),
    ((uint32) & __linker_dio_no_init_end) - APP_UTILS_LINKER_FILL_LENGTH,
};


/* ========================================================================== */
/*                          Function Implementation                           */
/* ========================================================================== */
/** \brief Utility used to initialize memory sections.
 *   This API is used to initialize fixed size(APP_UTILS_LINKER_FILL_LENGTH)
 *   before start of each section and before end of each section. i.e
 *      .=align(4);
 *       __linker_dio_text_start = .;
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      *(DIO_TEXT_SECTION)
 *      .=align(4);
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      __linker_dio_text_end = .;
 *
 *   This API should be called during module start up code.
 */
void AppUtils_DioSectionInit(void)
{
    /* Initialize memory sections  */
	AppUtils_SectionInit(&DioApp_SecHoleAddrList[0U],
        MODULEAPP_NUM_SEC_HOLES(DioApp_SecHoleAddrList));
}

/** \brief Utility used to check any memory corruption occured in
 *   stack and memory sections initialized by AppUtils_DioSectionInit.
 *   In this API, both stack and memory sections initialized by
 *   AppUtils_DioSectionInit are checked.
 *
 *   This API expects AppUtils_DioSectionInit should be called in start up code
 *   before calling this.
 */
uint32 AppUtils_DioCheckStackAndSectionCorruption(void)
{
    uint32 retVal = E_OK;
    if (AppUtils_CheckStackAndSectionCorruption(
            &DioApp_SecHoleAddrList[0U],
                MODULEAPP_NUM_SEC_HOLES(DioApp_SecHoleAddrList)) != E_OK)
    {
        retVal = E_NOT_OK;
    }

    return retVal;
}
