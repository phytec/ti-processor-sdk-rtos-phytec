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
 *  \file     ComStack_Types.h
 *
 *  \brief    This file contains AUTOSAR communication stack type header file.
 */

#ifndef COMSTACK_TYPES_H_
#define COMSTACK_TYPES_H_

/* ========================================================================== */
/*                           Include Files                                    */
/* ========================================================================== */
#include "Std_Types.h"
#include "ComStack_Cfg.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#define ECUC_SW_MAJOR_VERSION   (9U)
#define ECUC_SW_MINOR_VERSION   (0U)
#define ECUC_SW_PATCH_VERSION   (0U)

/** \brief Variables of this type shall be used to store the basic information
  * about a PDU of any type, namely a pointer variable pointing to its
  * SDU (payload), and the corresponding length of the SDU in bytes. */
#if defined (AUTOSAR_421)

typedef struct {
    uint8        *SduDataPtr;
    /**< Pointer to the SDU (i.e. payload data) of the PDU. */
    PduLengthType SduLength;
    /**< length of the SDU. */
} PduInfoType;

#else

typedef struct {
    uint8        *SduDataPtr;
    /**< Pointer to the SDU (i.e. payload data) of the PDU. */
    uint8        *MetaDataPtr;
    /**< Pointer to the Meta Data (eg CAN ID) of the PDU. */
    PduLengthType SduLength;
    /**< length of the SDU. */
} PduInfoType;


#endif

/** \brief Used to store the identifier of a partial network cluster */
typedef uint8 PNCHandleType;

/** \brief Specify the parameter to which the value has to be changed
  * (BS or STmin). */
typedef enum {
    TP_STMIN = 0x00,
    /**< Separation Time */
    TP_BS = 0x01,
    /**< Block Size */
    TP_BC = 0x02
    /**< The Band width control parameter used in FlexRay */
} TPParameterType;

/** \brief Variables of this type shall be used to store the result of a buffer
  *   request. */
typedef enum {
    BUFREQ_OK       = 0x00,
    /**< Buffer request accomplished successful.This status have the value 0. */
    BUFREQ_E_NOT_OK = 0x01,
    /**< Buffer request not successful. Buffer cannot be accessed.
      *  This status shall have the value 1. */
    BUFREQ_E_BUSY   = 0x02,
    /**< Temporarily no buffer available. It's up the requester to retry request
      *  for a certain time. This status shall have the value 2. */
    BUFREQ_E_OVFL   = 0x03U
    /**< No Buffer of the required length can be provided.
      *   This status shall have the value 3. */
} BufReq_ReturnType;

/** \brief Variables of this type shall be used to store the state of TP
  *  buffer. */
#if defined (AUTOSAR_421)

typedef enum {
    TP_DATACONF     = 0x00,
    /**< Indicates that all data, that have been copied so far,
      *  are confirmed and can be removed from the TP buffer. Data
      *  copied by this API call are excluded and will be confirmed later. */
    TP_DATARETRY    = 0x01,
    /**< Indicates that this API call shall copy already copied data in order to
      *  recover from an error. In this case  TxTpDataCnt specifies the offset
      *  of the first byte to be copied by the API call. */
    TP_CONFPENDING  = 0x02,
    /**< Indicates that the previously copied data must remain in the TP. */
    TP_NORETRY      = 0x03
    /**< No explanation in Spec. */
} TpDataStateType;

#else

typedef enum {
    TP_DATACONF     = 0x00,
    /**< Indicates that all data, that have been copied so far,
      *  are confirmed and can be removed from the TP buffer. Data
      *  copied by this API call are excluded and will be confirmed later. */
    TP_DATARETRY    = 0x01,
    /**< Indicates that this API call shall copy already copied data in order to
      *  recover from an error. In this case  TxTpDataCnt specifies the offset
      *  of the first byte to be copied by the API call. */
    TP_CONFPENDING  = 0x02
    /**< Indicates that the previously copied data must remain in the TP. */

} TpDataStateType;

#endif

/** \brief Variables of this type shall be used to store the information about
  *  Tp buffer handling. */
typedef struct {
    TpDataStateType TpDataState;
    /**< The enum type to be used to store the state of Tp buffer. */
    PduLengthType   TxTpDataCnt;
    /**< Offset from the current position which identifies the number of bytes
      *  to be retransmitted. */
} RetryInfoType;

/** \brief Variables of the type NetworkHandleType shall be used to store the
  *  identifier of a communication channel. */
typedef uint8 NetworkHandleType;

/** \brief IcomConfigIdType defines the configuration ID. */
typedef uint8 IcomConfigIdType;

/** \brief IcomSwitch_ErrorType defines the errors which can occur when
            activating or deactivating Pretended Networking */
typedef enum {
    ICOM_SWITCH_E_OK        = 0x00,
    /**< The activation of Pretended Networking was successful */
    ICOM_SWITCH_E_FAILED    = 0x01
    /**< The activation of Pretended Networking was not successful*/
} IcomSwitch_ErrorType;

#if defined (AUTOSAR_421)
/** \brief
 * 0x00--0x1e General return types
 * 0x1f--0x3c Error notif, CAN
 * 0x3d--0x5a Error notif, LIN
 * more
 */
typedef uint8 NotifResultType;
#endif /* 4.2.1 */

#define NTFRSLT_OK                     (0x00U)
#define NTFRSLT_E_NOT_OK               (0x01U)
#define NTFRSLT_E_TIMEOUT_A            (0x02U)
#define NTFRSLT_E_TIMEOUT_BS           (0x03U)
#define NTFRSLT_E_TIMEOUT_CR           (0x04U)
#define NTFRSLT_E_WRONG_SN             (0x05U)
#define NTFRSLT_E_INVALID_FS           (0x06U)
#define NTFRSLT_E_UNEXP_PDU            (0x07U)
#define NTFRSLT_E_WFT_OVRN             (0x08U)
#define NTFRSLT_E_ABORT                (0x09U)
#define NTFRSLT_E_NO_BUFFER            (0x0AU)
#define NTFRSLT_E_CANCELATION_OK       (0x0BU)
#define NTFRSLT_E_CANCELATION_NOT_OK   (0x0CU)
#define NTFRSLT_PARAMETER_OK           (0x0DU)
#define NTFRSLT_E_PARAMETER_NOT_OK     (0x0EU)
#define NTFRSLT_E_RX_ON                (0x0FU)
#define NTFRSLT_E_VALUE_NOT_OK         (0x10U)

#if defined (AUTOSAR_421)
typedef uint8 BusTrcvErrorType;

#define BUSTRCV_NO_ERROR    (0x00U)
#define BUSBUSTRCV_E_ERROR  (0x01U)
#endif

#if defined (AUTOSAR_421)
#define COMSTACKTYPE_AR_MAJOR_VERSION       (4U)
#define COMSTACKTYPE_AR_MINOR_VERSION       (2U)
#define COMSTACKTYPE_AR_PATCH_VERSION       (1U)

#else

#define COMSTACKTYPE_AR_MAJOR_VERSION       (4U)
#define COMSTACKTYPE_AR_MINOR_VERSION       (3U)
#define COMSTACKTYPE_AR_PATCH_VERSION       (1U)

#endif

#ifdef __cplusplus
}
#endif

#endif /*COMSTACK_TYPES_H_*/
