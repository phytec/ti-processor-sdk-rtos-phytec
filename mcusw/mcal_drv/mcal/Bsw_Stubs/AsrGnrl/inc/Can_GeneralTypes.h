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
/*
 * Design: MCAL-6158
 */
#ifndef CAN_GENERALTYPES_H_
#define CAN_GENERALTYPES_H_

#ifdef __cplusplus
extern "C"{
#endif

/* ========================================================================== */
/*                           Include Files                                    */
/* ========================================================================== */
#include "ComStack_Types.h"

/*
 * Design: MCAL-6014,MCAL-5983,MCAL-6158,MCAL-6170,MCAL-6027,MCAL-6142,MCAL-6083,MCAL-6094,MCAL-5910,MCAL-6099,MCAL-5991
 */
/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
/** \brief Represents the hardware object handles of a CAN hardware unit.
  *  For CAN hardware units with more than 255 HW objects use extended range. */
typedef uint32 Can_HwHandleType;

#if defined (AUTOSAR_421)

/** \brief State transitions that are used by the function
  * Can_SetControllerMode. */
typedef enum
{
    CAN_T_START,
    /**< CAN controller transition value to request state STARTED. */
    CAN_T_STOP,
    /**< CAN controller transition value to request state STOPPED. */
    CAN_T_SLEEP,
    /**< CAN controller transition value to request state SLEEP. */
    CAN_T_WAKEUP
    /**< CAN controller transition value to request state STOPPED
      *  from state SLEEP. */
} Can_StateTransitionType;

/** \brief Return values of CAN driver API. */
typedef enum
{
    CAN_OK,
    /**< Success */
    CAN_NOT_OK,
    /**< Error occurred or wakeup event occurred during sleep transition */
    CAN_BUSY
    /**< Transmit request could not be processed because no transmit
      * object was available. */
} Can_ReturnType;

#elif defined (AUTOSAR_431)

/** \brief State transitions that are used by the function
  * Can_SetControllerMode. */
typedef enum
{
    CAN_CS_UNINIT,
    /**< CAN controller transition value to request state STARTED. */
    CAN_CS_STARTED,
    /**< CAN controller transition value to request state STOPPED. */
    CAN_CS_STOPPED,
    /**< CAN controller transition value to request state STOPPED. */
    CAN_CS_SLEEP
    /**< CAN controller transition value to request state SLEEP
      *  from state SLEEP. */
} Can_ControllerStateType;

/** \brief Return values of CAN driver API. */
typedef enum
{
    CAN_BUSY = 0x02U
    /**< Transmit request could not be processed because no transmit
      * object was available. */
} Can_ReturnType;

/** \brief Error states of a CAN controller. */
typedef enum
{
    CAN_ERRORSTATE_ACTIVE,
    /**< The CAN controller takes fully part in communication */
    CAN_ERRORSTATE_PASSIVE,
    /**< The CAN controller takes part in communication,
      *  but does not send active error frames. */
    CAN_ERRORSTATE_BUSOFF,
    /**< The CAN controller does not take part in communication */
} Can_ErrorStateType;

#endif

typedef uint32 Can_IdType;

typedef struct Can_PduType_s {
    PduIdType  swPduHandle;
    /**< private data for CanIf,just save and use for callback */
    uint8      length;
    /**< Length, max 8 bytes/64 bytes for CAN FD */
    Can_IdType id;
    /**< the CAN ID, 29 or 11-bit */
    uint8     *sdu;
    /**< data ptr */
} Can_PduType;

/*
 * Design: MCAL-6083
 */
typedef struct
{
    Can_IdType CanId;
    /**< Standard/Extended CAN ID of CAN L-PDU */
    Can_HwHandleType Hoh;
    /**< ID of the corresponding Hardware Object Range */
    uint8 ControllerId;
    /**< Mailbox Corresponding Controller Id */
}Can_HwType;


#ifdef __cplusplus
}
#endif

#endif /* CAN_GENERAL_TYPES_H */
