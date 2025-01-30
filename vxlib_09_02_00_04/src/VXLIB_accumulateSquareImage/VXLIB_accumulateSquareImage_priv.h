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

#ifndef VXLIB_ACCUMULATESQUAREIMAGE_IXX_OXX_PRIV_H_
#define VXLIB_ACCUMULATESQUAREIMAGE_IXX_OXX_PRIV_H_

#include "../common/VXLIB_utility.h"
#include "VXLIB_accumulateSquareImage.h"
#include "VXLIB_types.h"
#include <cstdint>

/**
 * \file
 * \brief Header file for kernel's internal use. For the kernel's interface.
 * Note: The VXLIB_accumulateSquareImage does not contain any implementation. Instead, the implementation of
 * VXLIB_addSquare is used. For this reason, the kerenl's interface of VXLIB_addSquare
 * (defined in VXLIB_addSquare_priv.h) are reused in this kernel.
 * please see @ref VXLIB_accumulateSquareImage
 */

/**
 * @brief Macros that will be useful to check for datatype combinations
 *
 */

#define VXLIB_ACCUMULATESQUAREIMAGE_I8U_O16S ((dTypeIn == VXLIB_UINT8) && (dTypeOut == VXLIB_INT16))

#endif /* VXLIB_ACCUMULATESQUAREIMAGE_IXX_OXX_PRIV_H_ */
