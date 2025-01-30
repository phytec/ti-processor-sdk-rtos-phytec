/*
 * Copyright (C) 2024 Texas Instruments Incorporated - http://www.ti.com/
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of Texas Instruments Incorporated nor the names of
 * its contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
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
 */

 #ifndef MEMIF_TYPES_H
 #define MEMIF_TYPES_H

 #ifdef __cplusplus
 extern "C"{
 #endif

 /* ========================================================================== */
 /*                           Include Files                                    */
 /* ========================================================================== */


 /* ========================================================================== */
 /*                         Structures and Enums                               */
 /* ========================================================================== */

/**<
 *  @brief  MemIf Module Status Types.
 */
typedef enum MemIf_StatusType {
    MEMIF_UNINIT = 0U,
    /**< Module/Device Driver has not been initialized yet */
    MEMIF_IDLE,
    /**< Module/Device Driver is currently IDLE */
    MEMIF_BUSY,
    /**< Module/Device Driver is currently BUSY */
    MEMIF_BUSY_INTERNAL,
    /**< Module/Device Driver is busy with Internal management operations */
} MemIf_StatusType;

/**<
 *  @brief  MemIf Mode Types.
 */
typedef enum MemIf_ModeType {
    MEMIF_MODE_SLOW = 0U,
    /**< Module/Device Driver are working in slow mode */
    MEMIF_MODE_FAST,
    /**< Module/Device Driver are working in fast mode */
} MemIf_ModeType;

/**<
 *  @brief  MemIf Job Results Types.
 */
typedef enum MemIf_JobResultType {
    MEMIF_JOB_OK = 0U,
    /**< The job has been finished successfully */
    MEMIF_JOB_FAILED,
    /**< The job has not finished successfully */
    MEMIF_JOB_PENDING,
    /**< The job has not yet been finished */
    MEMIF_JOB_CANCELED,
    /**< The job has been canceled */
    MEMIF_BLOCK_INCONSISTENT,
    /**< The requested block is inconsistent or BLOCK not found*/
    MEMIF_BLOCK_INVALID,
    /**< The requested block has been marked invalid, operation cannot be performed */
} MemIf_JobResultType;

#ifdef __cplusplus
}
#endif

#endif /* MEMIF_TYPES_H */
