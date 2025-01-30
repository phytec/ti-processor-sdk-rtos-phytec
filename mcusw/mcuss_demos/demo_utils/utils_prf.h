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
 *  \ingroup UTILS_API
 *  \defgroup UTILS_PRF_API Profiling API
 *  @{
 */

/**
 *  \file utils_prf.h
 *
 *  \brief Profiling API.
 *
 *   - APIs to measure and print elapsed time @ 64-bit precision
 *      - Utils_prfTsXxxx
 *
 *   - APIs to measure and print CPU load at task, HWI, SWI, global level
 *      - Utils_prfLoadXxxx
 */

#ifndef UTILS_PRF_H_
#define UTILS_PRF_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/osal/TaskP.h>
#include <ti/osal/osal.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief Maximum supported profiling objects */
#define UTILS_PRF_MAX_HNDL              (64U)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  \brief Profiling load object.
 */
typedef struct
{
    float cpuLoad;
    /**< CPU load. */
    float hwiLoad;
    /**< HWI load. */
    float swiLoad;
    /**< SWI load. */
    float tskLoad;
    /**< TSK load. */
} Utils_PrfLoad;

/** \brief Typedef for the load update function for the user. */
typedef void (*Utils_loadUpdate)(Utils_PrfLoad *prfLoad);

/**
 *  \brief Profiling time-stamp object.
 */
typedef struct
{
    char        name[32];
    /**< Name. */
    uint32_t    isAlloc;
    /**< Indicates if the object is initialized (used). */
    uint64_t    startTs;
    /**< Start time stamp value. */
    uint64_t    totalTs;
    /**< Total time stamp value. */
    uint32_t    count;
    /**< Number of times time stamp delta is calculated. */
    uint32_t    numFrames;
    /**< Total number of frames being used for time-stamping. */
} Utils_PrfTsHndl;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief Initializes the profiling utility.
 *
 *  This function must be called before using any peformance or Timestamp utils
 *
 *  \return 0 on success, else failure
 */
int32_t Utils_prfInit(void);

/**
 *  \brief De-initializes the profiling utility.
 *
 *  \return 0 on success, else failure
 */
int32_t Utils_prfDeInit(void);

/**
 *  \brief Creates the handle for the time stamp taking.
 *
 *  \param name     [IN] Name of the time stamp object
 *
 *  \return Valid handle on success, else NULL
 */
Utils_PrfTsHndl *Utils_prfTsCreate(const char *name);

/**
 *  \brief Deletes the handle for the timestamp.
 *
 *  \param pHndl    [IN] Handle to be deleted
 *
 *  \return 0 on success, else failure
 */
int32_t Utils_prfTsDelete(Utils_PrfTsHndl *pHndl);

/**
 *  \brief Start taking the timestamp.
 *
 *  \param pHndl    [IN] Handle to be time stamp object
 *
 *  \return Initial time stamp value on success
 */
uint64_t Utils_prfTsBegin(Utils_PrfTsHndl *pHndl);

/**
 *  \brief Stop taking the timestamp.
 *
 *  \param pHndl     [IN] Handle to be time stamp object
 *  \param numFrames [IN] Number of associated with the time stamp
 *
 *  \return Final time stamp value on success
 */
uint64_t Utils_prfTsEnd(Utils_PrfTsHndl *pHndl, uint32_t numFrames);

/**
 *  \brief Update the CPU load information for all profile handles
 */
void Utils_prfLoadUpdate(void);

/**
 *  \brief Calculates the difference between the timestamp.
 *
 *  \param pHndl     [IN] Handle to be time stamp object
 *  \param startTime [IN] Start time stamp value
 *  \param numFrames [IN] Number of associated with the time stamp
 *
 *  \return Final time stamp value on success
 */
uint64_t Utils_prfTsDelta(Utils_PrfTsHndl *pHndl,
                          uint64_t         startTime,
                          uint32_t         numFrames);

/**
 *  \brief Resets the timestamp counter for that handle.
 *
 *  \param pHndl     [IN] Handle to be time stamp object
 *
 *  \return 0 on success, else failure
 */
int32_t Utils_prfTsReset(Utils_PrfTsHndl *pHndl);

/**
 *  \brief Gets the 64-bit timer ticks
 *
 *  \return Current 64-bit timer ticks value on success
 */
uint64_t Utils_prfTsGet64(void);

/**
 *  \brief Prints the timestamp difference and resets the counter thereafter
 *         depending on specified resetAfterPrint parameter.
 *
 *  \param pHndl           [IN] Handle to be time stamp object
 *  \param resetAfterPrint [IN] Indicates whether time stamp values should be
 *                              reset after printing
 *
 *  \return 0 on success, else failure
 */
int32_t Utils_prfTsPrint(Utils_PrfTsHndl *pHndl, uint32_t resetAfterPrint, uint32_t trace);

/**
 *  \brief Prints the timestamp difference for all registered handles and resets
 *         the counter thereafter depending on specified resetAfterPrint
 *         parameter.
 *
 *  \param resetAfterPrint [IN] Indicates whether time stamp values should be
 *                              reset after printing
 *
 *  \return 0 on success, else failure
 */
int32_t Utils_prfTsPrintAll(uint32_t resetAfterPrint, uint32_t trace);

/**
 *  \brief Registers a task for load calculation.
 *
 *  \param pTsk            [IN] Handle to task object to be registered for load
 *                              calculation
 *  \param name            [IN] Name to be associated with the registered handle
 *
 *  \return 0 on success, else failure
 */
int32_t Utils_prfLoadRegister(TaskP_Handle pTsk, const char *name);
/**
 *  \brief Un-registers a task for load calculation.
 *
 *  \param pTsk            [IN] Handle to task object to be unregistered for
 *                              load calculation
 *
 *  \return 0 on success, else failure
 */
int32_t Utils_prfLoadUnRegister(TaskP_Handle pTsk);

/**
 *  \brief Start taking the performance load for all the registered tasks.
 */
uint32_t Utils_prfLoadCalcStart(void);

/**
 *  \brief Stop taking the load for all the registered tasks.
 */
uint32_t Utils_prfLoadCalcStop(void);

/**
 *  \brief Reset the load calculation mainly for next cycle of run.
 */
uint32_t Utils_prfLoadCalcReset(void);

/**
 *  \brief Return the load for a given task
 */
void Utils_prfLoadGetTaskLoad(TaskP_Handle pTsk, Utils_PrfLoad *load);

/**
 *  \brief Calculates the difference between the timestamp and return delta in
 *          milliseconds
 *
 *  \param pHndl     [IN] Handle to be time stamp object
 *  \param startTime [IN] Start time stamp value
 *  \param numFrames [IN] Number of associated with the time stamp
 *
 *  \return Delata in milliseconds
 */
uint64_t Utils_prfTsGetTimeinMs(uint64_t startTime, uint64_t endTime);

#ifdef __cplusplus
}
#endif

#endif /* ifndef UTILS_PRF_H_ */

/* @} */
