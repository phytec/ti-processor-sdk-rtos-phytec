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
 *  \file     Cdd_IpcIrq.h
 *
 *  \brief    This file contains ISR function declaration for CDD IPC Driver
 *
 */

/**
 *  \ingroup MCAL_IPC_API IPC ISR API's
 *
 *  This file provides ISR API's that is used to receive and process events from
 *  remote cores
 *
 *  - For each remote core an associated ISR is required
 *  - In cases where multiple cores interrupts are mapped to one interrupt
 *      an single ISR would suffice
 *
 *
 *  \version 01.00.00
 *
 *
 *  @sa MCAL_IPC_ISR
 *  @{
 */


#ifndef CDD_IPCIRQ_H_
#define CDD_IPCIRQ_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "Cdd_Ipc.h"
#include "Os.h"


#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \name CDD Driver ISR category level
 *
 *
 *  Defines for CDD Driver ISR ISR category level
 *  @{
 */
/** \brief void ISR type */
#define CDD_IPC_ISR_VOID                    (0x00U)
/** \brief Category 1 ISR type */
#define CDD_IPC_ISR_CAT1                    (0x01U)
/** \brief Category 2 ISR type */
#define CDD_IPC_ISR_CAT2            (0x02U)

/* ========================================================================== */
/*                           Function Prototypes                              */
/* ========================================================================== */

/** \brief A Mailbox can raise multiple interrupts. In this implementation, the
 *          Mailbox new message interrupt is used determine presence of data
 *          from the remote core in the vring.
 *
 *          Typically, we would have 1 interrupt for each remote core. Due to
 *          system resource needs, multiple remote cores could use a single
 *          interrupt.
 *
 *          Depending on the SoC variant, the isr to registered with interrupt
 *          will change. Please refer CDD IPC example application to associate
 *          right ISR with interrupt.
 */

/** \brief ISR for New Message*/
FUNC(void, CDD_IPC_CODE_FAST) Cdd_IpcIrq_Fun(uint32 srcProcId);

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef CDD_IPCIRQ_H_ */

/* @} */
