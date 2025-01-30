/*
*
* Copyright (C) 2024 Texas Instruments Incorporated
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
  *  \file      Icu.c
  *
  *  \brief    This file contains ICU MCAL driver
  *
  */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "string.h"
#include "Icu.h"
#include "SchM_Icu.h"
#if (STD_ON == ICU_DEV_ERROR_DETECT)
#include "Det.h"
#endif
#include "Icu_Irq.h"
#include "Icu_Priv.h"

/* There are static inline functions in hw_types.h file. Map them as well */
#define ICU_START_SEC_CODE
#include "Icu_MemMap.h"
#define ICU_STOP_SEC_CODE
#include "Icu_MemMap.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* AUTOSAR version information check has to match definition in header file */
#if ((ICU_AR_RELEASE_MAJOR_VERSION != (4U)) || \
    (ICU_AR_RELEASE_MINOR_VERSION != (3U)) ||  \
    (ICU_AR_RELEASE_REVISION_VERSION != (1U)))
    #error "Icu: AUTOSAR Version Numbers of Icu are different!!"
#endif

/* AUTOSAR version information check has to match definition in header file */
#if ((ICU_SW_MAJOR_VERSION != (9U)) || \
    (ICU_SW_MINOR_VERSION != (0U)) ||  \
    (ICU_SW_PATCH_VERSION != (1U)))
    #error "Icu: Software Version Numbers are inconsistent!!"
#endif

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

#define ICU_START_SEC_VAR_INIT_32
#include "Icu_MemMap.h"
/** \brief Icu driver init status */
volatile VAR(uint8, ICU_VAR_INIT)
    Icu_DrvStatus = ICU_STATUS_UNINIT;

#define ICU_STOP_SEC_VAR_INIT_32
#include "Icu_MemMap.h"

#define ICU_START_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Icu_MemMap.h"
/** \brief ICU driver object. */

    VAR(Icu_ChObjType, ICU_VAR_NO_INIT) Icu_ChObj[ICU_MAX_NUM_CHANNELS];
/* Icu_ChObj is not static storage class, as this requires to be accessed by
    debug interface provided.  */
#define ICU_STOP_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Icu_MemMap.h"


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

#define ICU_START_SEC_CODE
#include "Icu_MemMap.h"

/*
*Design: MCAL-6967,MCAL-7100,MCAL-7133,MCAL-7126,MCAL-7021,MCAL-6927,MCAL-6989,MCAL-7094,MCAL-7117,MCAL-6945,MCAL-6971,MCAL-6999,MCAL-7039,MCAL-7071,MCAL-7010,MCAL-7089,MCAL-7107,MCAL-7032,MCAL-7122,MCAL-7112,MCAL-7088,MCAL-7069,MCAL-7038,MCAL-6989,MCAL-6965,MCAL-7029,MCAL-6936
*/
FUNC(void, ICU_CODE) Icu_Init(
                           P2CONST(Icu_ConfigType, AUTOMATIC, ICU_PBCFG) ConfigPtr)
{
    uint32 chIdx, chNum;
    uint32 maxCh;
    const Icu_ConfigType *IcuConfigPtr = ConfigPtr;
#if (STD_ON == ICU_PRE_COMPILE_VARIANT)
    IcuConfigPtr = &ICU_INIT_CONFIG_PC;
#endif

#if (STD_ON == ICU_DEV_ERROR_DETECT)

   /* ICU_CoverageGap_01: Dynamic coverage for the below "if" statement gives a false positive. Thus it cannot be covered.
    * Thus it cannot be covered.
    */

    if (NULL == IcuConfigPtr)
    {
        (void)Icu_reportDetError(ICU_INIT_ID, ICU_E_PARAM_POINTER);
    }
    else if (ICU_STATUS_INIT == Icu_DrvStatus)
    {
        (void)Icu_reportDetError(ICU_INIT_ID, ICU_E_ALREADY_INITIALIZED);
    }
    else
#endif /* (STD_ON == ICU_DEV_ERROR_DETECT) */
    {

        maxCh = (uint32) IcuConfigPtr->icuMaxChannel;

    /* ICU_CoverageGap_02: Dynamic coverage for the below "if" statement
     * gives a false positive expects hardware input for feedback.
     * Thus it cannot be covered.
     */
        for (chNum = 0U; chNum < maxCh; chNum++)
        {
#if (STD_ON == ICU_DEV_ERROR_DETECT)
            if(((&IcuConfigSet_PC)->chCfg[chNum].channelId) >=
            (uint32)ICU_MAX_NUM_CHANNELS)
            {
                /*Invalid channel */
                (void)Icu_reportDetError(ICU_INIT_ID, ICU_E_PARAM_CHANNEL);
            }
            else
#endif /* (STD_ON == ICU_DEV_ERROR_DETECT) */
            {
                for (chIdx = 0; chIdx < ICU_MAX_NUM_CHANNELS; chIdx++)
                {
                    if(chIdx == ((&IcuConfigSet_PC)->chCfg[chNum].channelId))
                    {
                        /* Reset Channel object */
                        Icu_ResetChObj(&Icu_ChObj[chIdx]);
                        /* Copy the configuration */
                        Icu_CopyConfig(&Icu_ChObj[chIdx],
                        &IcuConfigPtr->chCfg[chNum],&IcuConfigSet_PC.chCfg[chNum]);
                        /* HW Channel Init */
                        Icu_HwUnitInit(&Icu_ChObj[chIdx]);

#if (STD_ON == ICU_TIMESTAMP_API)
                        Icu_ChObj[chIdx].IsActive = FALSE;
#endif
                        break;
                    }
                }
            }
        }

#if (STD_ON == ICU_DEV_ERROR_DETECT)
        Icu_DrvStatus = ICU_STATUS_INIT;
#endif

    }

    return;
}

#if (STD_ON == ICU_DE_INIT_API)
/*
*Design: MCAL-6948,MCAL-7133,MCAL-7126,MCAL-7021,MCAL-6927,MCAL-6989,MCAL-7094,MCAL-7117,MCAL-6945,MCAL-6954,MCAL-6995,MCAL-7022,MCAL-7023,MCAL-7025,MCAL-7081,MCAL-7127,MCAL-6909,MCAL-6953,MCAL-7024,MCAL-6965,MCAL-7029,MCAL-6936
*/
FUNC(void, ICU_CODE) Icu_DeInit(void)
{
    uint32          chIdx;
    uint32          maxCh;
    uint32          chNum;
#if (STD_ON == ICU_DEV_ERROR_DETECT)
    if (ICU_STATUS_UNINIT == Icu_DrvStatus)
    {
        (void)Icu_reportDetError(ICU_DEINIT_ID, ICU_E_UNINIT);
    }
    else
#endif /* (STD_ON == ICU_DEV_ERROR_DETECT) */
    {
        Icu_DrvStatus = ICU_STATUS_UNINIT;

        for (chIdx = 0; chIdx < ICU_MAX_NUM_CHANNELS; chIdx++)
        {
          maxCh = (uint32)(&IcuConfigSet)->icuMaxChannel;


          for (chNum=0; chNum < maxCh; chNum++)
          {

            /* ICU_CoverageGap_03: Dynamic coverage for the below "if" statement
             * gives a false positive. Thus it cannot be covered.
             */

            if (chIdx == ((&IcuConfigSet_PC)->chCfg[chNum].channelId))
            {
            /*Check if any channel is running. If yes, cannot call Deinit*/
            if(FALSE == Icu_ChObj[chIdx].IsRunning)
            {
                /*Disable and Clear Interrupts*/
                ECAPIntDisable(Icu_ChObj[chIdx].baseAddr, ECAP_INT_ALL);
                ECAPIntStatusClear(Icu_ChObj[chIdx].baseAddr, ECAP_INT_ALL);

                /* Disable CAP1-CAP4 register loads */
                ECAPCaptureLoadingDisable(Icu_ChObj[chIdx].baseAddr);

                Icu_ResetChObj(&Icu_ChObj[chIdx]);

              }
            }
          }
         }
       }
    return;
}
#endif /*ICU_DE_INIT_API*/

/*
*Design: MCAL-6938,MCAL-7133,MCAL-6923,MCAL-7126,MCAL-7021,MCAL-6927,MCAL-7042,MCAL-7065,MCAL-6911,MCAL-7026,MCAL-6965,MCAL-7029,MCAL-6998,MCAL-6957,MCAL-7074,MCAL-6936
*/
FUNC(void, ICU_CODE) Icu_DisableNotification( Icu_ChannelType Channel)
{
#if (STD_ON == ICU_DEV_ERROR_DETECT)
    if (ICU_STATUS_UNINIT == Icu_DrvStatus)
    {
        (void)Icu_reportDetError(ICU_DISABLENOTIFICATION_ID, ICU_E_UNINIT);
    }
    else if (ICU_MAX_NUM_CHANNELS <= Channel)
    {
        (void)Icu_reportDetError(ICU_DISABLENOTIFICATION_ID, ICU_E_PARAM_CHANNEL);
    }
    else
#endif /* (STD_ON == ICU_DEV_ERROR_DETECT) */
    {
        SchM_Enter_Icu_ICU_EXCLUSIVE_AREA_0();
        Icu_ChObj[Channel].NotificationEnabled = FALSE;
        SchM_Exit_Icu_ICU_EXCLUSIVE_AREA_0();
        /* Disable the notification on the given channel */
    }
}

/*
*Design: MCAL-7013,MCAL-6923,MCAL-7133,MCAL-7126,MCAL-7021,MCAL-6927,MCAL-7096,MCAL-6950,MCAL-6893,MCAL-6994,MCAL-6965,MCAL-7029,MCAL-6998,MCAL-6957,MCAL-7074,MCAL-6936
*/
FUNC(void, ICU_CODE) Icu_EnableNotification( Icu_ChannelType Channel)
{
#if (STD_ON == ICU_DEV_ERROR_DETECT)
    if (ICU_STATUS_UNINIT == Icu_DrvStatus)
    {
        (void)Icu_reportDetError(ICU_ENABLENOTIFICATION_ID, ICU_E_UNINIT);
    }
    else if (ICU_MAX_NUM_CHANNELS <= Channel)
    {
        (void)Icu_reportDetError(ICU_ENABLENOTIFICATION_ID, ICU_E_PARAM_CHANNEL);
    }
    else
#endif /* (STD_ON == ICU_DEV_ERROR_DETECT) */
    {
        SchM_Enter_Icu_ICU_EXCLUSIVE_AREA_0();
        Icu_ChObj[Channel].NotificationEnabled = TRUE;
         SchM_Exit_Icu_ICU_EXCLUSIVE_AREA_0();
        /* Enables the notification on the given channel */
    }
    return;
}

/*
*Design: MCAL-6941,MCAL-7054,MCAL-6923,MCAL-6912,MCAL-7123,MCAL-7133,MCAL-7126,MCAL-7021,MCAL-6927,MCAL-6922,MCAL-7002,MCAL-7076,MCAL-6947,MCAL-6973,MCAL-6953,MCAL-7087,MCAL-7123,MCAL-6965,MCAL-6940,MCAL-7029,MCAL-6936
*/
FUNC(void, ICU_CODE) Icu_SetActivationCondition(
    Icu_ChannelType Channel, Icu_ActivationType Activation )
{
#if (STD_ON == ICU_DEV_ERROR_DETECT)
    if (ICU_STATUS_UNINIT == Icu_DrvStatus)
    {
        (void)Icu_reportDetError(ICU_SETACTIVATIONCONDITION_ID, ICU_E_UNINIT);
    }
    else if (ICU_MAX_NUM_CHANNELS <= Channel)
    {
        (void)Icu_reportDetError(ICU_SETACTIVATIONCONDITION_ID, ICU_E_PARAM_CHANNEL);
    }
    else if (ICU_MODE_SIGNAL_MEASUREMENT == Icu_ChObj[Channel].chCfg.measurementMode)
    {
        (void)Icu_reportDetError(ICU_SETACTIVATIONCONDITION_ID, ICU_E_PARAM_CHANNEL);
    }
    else if ((Activation != ICU_RISING_EDGE) && (Activation != ICU_FALLING_EDGE) && (Activation != ICU_BOTH_EDGES))
    {
        (void)Icu_reportDetError(ICU_SETACTIVATIONCONDITION_ID, ICU_E_PARAM_ACTIVATION);
    }
    else
#endif /* (STD_ON == ICU_DEV_ERROR_DETECT) */
    {
        SchM_Enter_Icu_ICU_EXCLUSIVE_AREA_0();
        Icu_ChObj[Channel].activation_edge = Activation;

#if ((ICU_EDGE_DETECT_API == STD_ON) || (ICU_SIGNAL_MEASUREMENT_API == STD_ON))
        Icu_ChObj[Channel].InputState = ICU_IDLE;
#endif
    SchM_Exit_Icu_ICU_EXCLUSIVE_AREA_0();
    }

    return;
}

#if (STD_ON == ICU_GET_INPUT_STATE_API)
/*
*Design: MCAL-6920,MCAL-7054,MCAL-6912,MCAL-6923,MCAL-7133,MCAL-7126,MCAL-7021,MCAL-6927,MCAL-6922,MCAL-6977,MCAL-7037,MCAL-6940,MCAL-7059,MCAL-7068,MCAL-7099,MCAL-7138,MCAL-7140,MCAL-7075,MCAL-6983,MCAL-7077,MCAL-6957,MCAL-6965,MCAL-7029,MCAL-6936
*/
FUNC(Icu_InputStateType, ICU_CODE) Icu_GetInputState(
    Icu_ChannelType Channel)
{
    Icu_InputStateType retState = ICU_IDLE;

#if (STD_ON == ICU_DEV_ERROR_DETECT)
    if (ICU_STATUS_UNINIT == Icu_DrvStatus)
    {
        (void)Icu_reportDetError(ICU_GETINPUTSTATE_ID, ICU_E_UNINIT);
    }
    else if (ICU_MAX_NUM_CHANNELS <= Channel)
    {
        (void)Icu_reportDetError(ICU_GETINPUTSTATE_ID, ICU_E_PARAM_CHANNEL);
    }
    else if ((ICU_MODE_SIGNAL_EDGE_DETECT != Icu_ChObj[Channel].chCfg.measurementMode) &&
             (ICU_MODE_SIGNAL_MEASUREMENT != Icu_ChObj[Channel].chCfg.measurementMode))
    {
        (void)Icu_reportDetError(ICU_GETINPUTSTATE_ID, ICU_E_PARAM_CHANNEL);
    }
    else
#endif /* (STD_ON == ICU_DEV_ERROR_DETECT) */
    {
        /* Return the current input status for the channel */
        Icu_InputStateType CurrentState = Icu_ChObj[Channel].InputState;
        SchM_Enter_Icu_ICU_EXCLUSIVE_AREA_0();
        if (CurrentState == ICU_ACTIVE)
        {
            Icu_ChObj[Channel].InputState = ICU_IDLE;
        }

        if (Icu_ChObj[Channel].chCfg.measurementMode == ICU_MODE_SIGNAL_MEASUREMENT)
        {


            /* ICU_CoverageGap_04: Dynamic coverage for the below "if" statement
             * expects hardware input for feedback. Thus it cannot be covered.
             */

            /*Check if measurement is completed before returning ICU_ACTIVE*/
            if (Icu_ChObj[Channel].IsRunning == TRUE)
            {
                Icu_ChObj[Channel].InputState = CurrentState;
                retState = ICU_IDLE;
            }
            else
            {
                retState = CurrentState;
            }
        }
        else
        {
            retState = CurrentState;
        }
        SchM_Exit_Icu_ICU_EXCLUSIVE_AREA_0();

    }

    return retState;

}
#endif /* ICU_GET_INPUT_STATE_API*/


#if (STD_ON == ICU_EDGE_DETECT_API)
/*
*Design: MCAL-6919,MCAL-7123,MCAL-7054,MCAL-6923,MCAL-6912,MCAL-7133,MCAL-7126,MCAL-7021,MCAL-6927,MCAL-6989,MCAL-7094,MCAL-7117,MCAL-6945,MCAL-6922,MCAL-6959,MCAL-6928,MCAL-7012,MCAL-7083,MCAL-7097,MCAL-6964,MCAL-6953,MCAL-6957,MCAL-6965,MCAL-6940,MCAL-7029,MCAL-6936
*/
FUNC(void, ICU_CODE) Icu_EnableEdgeDetection(
    Icu_ChannelType Channel)
{
    uint32 baseAddr;

#if (STD_ON == ICU_DEV_ERROR_DETECT)
    if (ICU_STATUS_UNINIT == Icu_DrvStatus)
    {
        (void)Icu_reportDetError(ICU_ENABLEEDGEDETECTION_ID, ICU_E_UNINIT);
    }
    else if (ICU_MAX_NUM_CHANNELS <= Channel)
    {
        (void)Icu_reportDetError(ICU_ENABLEEDGEDETECTION_ID, ICU_E_PARAM_CHANNEL);
    }
    else if (ICU_MODE_SIGNAL_EDGE_DETECT != Icu_ChObj[Channel].chCfg.measurementMode)
    {
        (void)Icu_reportDetError(ICU_ENABLEEDGEDETECTION_ID, ICU_E_PARAM_CHANNEL);
    }
    else
#endif /* (STD_ON == ICU_DEV_ERROR_DETECT) */
    {
        baseAddr = Icu_ChObj[Channel].baseAddr;
        SchM_Enter_Icu_ICU_EXCLUSIVE_AREA_0();
        Icu_ConfigEcap(baseAddr, Icu_ChObj[Channel].activation_edge, ICU_DELTA_MODE, TRUE);
        Icu_ChObj[Channel].IsRunning = TRUE;
        SchM_Exit_Icu_ICU_EXCLUSIVE_AREA_0();
    }
}

/*
*Design: MCAL-6894,MCAL-7054,MCAL-6912,MCAL-6923,MCAL-7100,MCAL-7133,MCAL-7126,MCAL-7021,MCAL-6927,MCAL-6922,MCAL-6916,MCAL-6935,MCAL-6897,MCAL-7024,MCAL-7047,MCAL-6957,MCAL-6965,MCAL-6940,MCAL-7029,MCAL-7032,MCAL-6936
*/
FUNC(void, ICU_CODE) Icu_DisableEdgeDetection(
    Icu_ChannelType Channel)
{
    uint32 baseAddr;

#if (STD_ON == ICU_DEV_ERROR_DETECT)
    if (ICU_STATUS_UNINIT == Icu_DrvStatus)
    {
        (void)Icu_reportDetError(ICU_DISABLEEDGEDETECTION_ID, ICU_E_UNINIT);
    }
    else if (ICU_MAX_NUM_CHANNELS <= Channel)
    {
        (void)Icu_reportDetError(ICU_DISABLEEDGEDETECTION_ID, ICU_E_PARAM_CHANNEL);
    }
    else if (ICU_MODE_SIGNAL_EDGE_DETECT != Icu_ChObj[Channel].chCfg.measurementMode)
    {
        (void)Icu_reportDetError(ICU_DISABLEEDGEDETECTION_ID, ICU_E_PARAM_CHANNEL);
    }
    else
#endif /* (STD_ON == ICU_DEV_ERROR_DETECT) */
    {
        baseAddr = Icu_ChObj[Channel].baseAddr;
        SchM_Enter_Icu_ICU_EXCLUSIVE_AREA_0();
        /*Disable Interrupts*/
        ECAPIntDisable(baseAddr, ECAP_INT_ALL);
        ECAPIntStatusClear(baseAddr, ECAP_INT_ALL);

        /* Disable CAP1-CAP4 register loads */
        ECAPCaptureLoadingDisable(baseAddr);
        Icu_ChObj[Channel].IsRunning = FALSE;
        SchM_Exit_Icu_ICU_EXCLUSIVE_AREA_0();
    }
}

#endif /*ICU_EDGE_DETECT_API*/

#if (STD_ON == ICU_TIMESTAMP_API)
/*
*Design: MCAL-6974,MCAL-7054,MCAL-6912,MCAL-7123,MCAL-6923,MCAL-7145,MCAL-7100,MCAL-7133,MCAL-7126,MCAL-7021,MCAL-6927,MCAL-6989,MCAL-7094,MCAL-6957,MCAL-7074,MCAL-7117,MCAL-6945,MCAL-6922,MCAL-7032,MCAL-6971,MCAL-7003,MCAL-6930,MCAL-6998,MCAL-7028,MCAL-7057,MCAL-6940,MCAL-7137,MCAL-6965,MCAL-6907,MCAL-7029,MCAL-7148,MCAL-6926,MCAL-6921,MCAL-6961,MCAL-6996,MCAL-6946,MCAL-6895,MCAL-7129,MCAL-7135,MCAL-7069,MCAL-7102,MCAL-6936
*/
FUNC(void, ICU_CODE) Icu_StartTimestamp(
    Icu_ChannelType Channel, Icu_ValueType* BufferPtr, uint16 BufferSize, uint16 NotifyInterval)
{
    uint32 baseAddr;

#if (STD_ON == ICU_DEV_ERROR_DETECT)
    if (ICU_STATUS_UNINIT == Icu_DrvStatus)
    {
        (void)Icu_reportDetError(ICU_STARTTIMESTAMP_ID, ICU_E_UNINIT);
    }
    else if (ICU_MAX_NUM_CHANNELS <= Channel)
    {
        (void)Icu_reportDetError(ICU_STARTTIMESTAMP_ID, ICU_E_PARAM_CHANNEL);
    }
    else if (ICU_MODE_TIMESTAMP != Icu_ChObj[Channel].chCfg.measurementMode)
    {
        (void)Icu_reportDetError(ICU_STARTTIMESTAMP_ID, ICU_E_PARAM_CHANNEL);
    }

    /* ICU_CoverageGap_05: Dynamic coverage for the below "else if” statement
     * gives a false positive. Thus it cannot be covered.
    */

    else if (NULL_PTR == BufferPtr)
    {
        (void)Icu_reportDetError(ICU_STARTTIMESTAMP_ID, ICU_E_PARAM_POINTER);
    }

    /* ICU_CoverageGap_06: Dynamic coverage for the below "if" statement
     * gives a false positive. Thus it cannot be covered.
    */

    else if (0U == BufferSize)
    {
        (void)Icu_reportDetError(ICU_STARTTIMESTAMP_ID, ICU_E_PARAM_BUFFER_SIZE);
    }
    else
#endif /* (STD_ON == ICU_DEV_ERROR_DETECT) */
    {
        baseAddr = Icu_ChObj[Channel].baseAddr;
        SchM_Enter_Icu_ICU_EXCLUSIVE_AREA_0();

        Icu_TimeStamp_Init(Channel, BufferPtr, BufferSize, NotifyInterval);

        Icu_ConfigEcap(baseAddr, Icu_ChObj[Channel].activation_edge, ICU_ABSOLUTE_MODE, TRUE);

        Icu_ChObj[Channel].IsRunning = TRUE;
        Icu_ChObj[Channel].IsActive = TRUE;
        SchM_Exit_Icu_ICU_EXCLUSIVE_AREA_0();
    }

    return;
}

/*
*Design: MCAL-7132,MCAL-7054,MCAL-6912,MCAL-7100,MCAL-6923,MCAL-7133,MCAL-7126,MCAL-7021,MCAL-6927,MCAL-6922,MCAL-7032,MCAL-6971,MCAL-7040,MCAL-6901,MCAL-6906,MCAL-6988,MCAL-6929,MCAL-7035,MCAL-7102,MCAL-6907,MCAL-6965,MCAL-6940,MCAL-7029,MCAL-6936
*/
FUNC(void, ICU_CODE) Icu_StopTimestamp(
    Icu_ChannelType Channel)
{
    uint32 baseAddr;

#if (STD_ON == ICU_DEV_ERROR_DETECT)
    if (ICU_STATUS_UNINIT == Icu_DrvStatus)
    {
        (void)Icu_reportDetError(ICU_STOPTIMESTAMP_ID, ICU_E_UNINIT);
    }
    else if (ICU_MAX_NUM_CHANNELS <= Channel)
    {
        (void)Icu_reportDetError(ICU_STOPTIMESTAMP_ID, ICU_E_PARAM_CHANNEL);
    }
    else if (ICU_MODE_TIMESTAMP != Icu_ChObj[Channel].chCfg.measurementMode)
    {
        (void)Icu_reportDetError(ICU_STOPTIMESTAMP_ID, ICU_E_PARAM_CHANNEL);
    }
    else
#endif /* (STD_ON == ICU_DEV_ERROR_DETECT) */
    {

        /* ICU_CoverageGap_07: Dynamic coverage for the below "if” statement
         * expects hardware input for feedback. Thus it cannot be covered.
         */

        if ((FALSE == Icu_ChObj[Channel].IsActive)&&(FALSE == Icu_ChObj[Channel].IsRunning))
        {
            (void)Icu_reportDetRuntimeError(ICU_STOPTIMESTAMP_ID, ICU_E_NOT_STARTED);
        }
        else
        {
            baseAddr = Icu_ChObj[Channel].baseAddr;
             SchM_Enter_Icu_ICU_EXCLUSIVE_AREA_0();
            ECAPIntDisable(baseAddr, ECAP_INT_ALL);
            ECAPIntStatusClear(baseAddr, ECAP_INT_ALL);

            /* Disable CAP1-CAP4 register loads */
            ECAPCaptureLoadingDisable(baseAddr);

            Icu_TimeStamp_Clear(Channel);

            Icu_ChObj[Channel].IsRunning = FALSE;
            Icu_ChObj[Channel].IsActive = FALSE;
            SchM_Exit_Icu_ICU_EXCLUSIVE_AREA_0();
        }
    }
    return;
}

/*
*Design: MCAL-6934,MCAL-7133,MCAL-7054,MCAL-6912,MCAL-7100,MCAL-6923,MCAL-7126,MCAL-7021,MCAL-6927,MCAL-6922,MCAL-7009,MCAL-7032,MCAL-6971,MCAL-7110,MCAL-7018,MCAL-6987,MCAL-6900,MCAL-7072,MCAL-6967,MCAL-7102,MCAL-6907,MCAL-6965,MCAL-7029,MCAL-6936,MCAL-7062,MCAL-7141
*/
FUNC(Icu_IndexType, ICU_CODE) Icu_GetTimestampIndex(
    Icu_ChannelType Channel)
{
    Icu_IndexType index = 0;

#if (STD_ON == ICU_DEV_ERROR_DETECT)
    if (ICU_STATUS_UNINIT == Icu_DrvStatus)
    {
        (void)Icu_reportDetError(ICU_STOPTIMESTAMP_ID, ICU_E_UNINIT);
    }
    else if (ICU_MAX_NUM_CHANNELS <= Channel)
    {
        (void)Icu_reportDetError(ICU_STOPTIMESTAMP_ID, ICU_E_PARAM_CHANNEL);
    }
    else if (ICU_MODE_TIMESTAMP != Icu_ChObj[Channel].chCfg.measurementMode)
    {
        (void)Icu_reportDetError(ICU_STOPTIMESTAMP_ID, ICU_E_PARAM_CHANNEL);
    }
    else
#endif
    {
        SchM_Enter_Icu_ICU_EXCLUSIVE_AREA_0();
        if ((Icu_ChObj[Channel].IsActive) == TRUE)
        {
            index = Icu_ChObj[Channel].NextTimeStampIndex;
        }
        else
        {
            /*return 0*/
        }
        SchM_Exit_Icu_ICU_EXCLUSIVE_AREA_0();
    }

    return index;
}


#endif /* ICU_TIMESTAMP_API*/

#if (STD_ON == ICU_EDGE_COUNT_API)
/*
*Design: MCAL-6949,MCAL-7133,MCAL-7054,MCAL-7054,MCAL-6912,MCAL-6912,MCAL-6923,MCAL-6923,MCAL-7126,MCAL-7021,MCAL-6927,MCAL-6922,MCAL-7985,MCAL-7116,MCAL-6915,MCAL-7111,MCAL-7085,MCAL-6907,MCAL-6957,MCAL-6965,MCAL-6940,MCAL-7029,MCAL-6936
*/
FUNC(void, ICU_CODE) Icu_ResetEdgeCount(
    Icu_ChannelType Channel)
{
#if (STD_ON == ICU_DEV_ERROR_DETECT)
    if (ICU_STATUS_UNINIT == Icu_DrvStatus)
    {
        (void)Icu_reportDetError(ICU_RESETEDGECOUNT_ID, ICU_E_UNINIT);
    }
    else if (ICU_MAX_NUM_CHANNELS <= Channel)
    {
        (void)Icu_reportDetError(ICU_RESETEDGECOUNT_ID, ICU_E_PARAM_CHANNEL);
    }
    else if (ICU_MODE_EDGE_COUNTER != Icu_ChObj[Channel].chCfg.measurementMode)
    {
        (void)Icu_reportDetError(ICU_RESETEDGECOUNT_ID, ICU_E_PARAM_CHANNEL);
    }
    else
#endif /* (STD_ON == ICU_DEV_ERROR_DETECT) */
    {
        SchM_Enter_Icu_ICU_EXCLUSIVE_AREA_0();
        Icu_ChObj[Channel].EdgeCounter = 0;
         SchM_Exit_Icu_ICU_EXCLUSIVE_AREA_0();
    }
}

/*
*Design: MCAL-6898,MCAL-7123,MCAL-7133,MCAL-7054,MCAL-6912,MCAL-7126,MCAL-7021,MCAL-6923,MCAL-6927,MCAL-6989,MCAL-7094,MCAL-7117,MCAL-6945,MCAL-6922,MCAL-6986,MCAL-7067,MCAL-7098,MCAL-7149,MCAL-7084,MCAL-6963,MCAL-6953,MCAL-6965,MCAL-6940,MCAL-7029,MCAL-6936
*/
FUNC(void, ICU_CODE) Icu_EnableEdgeCount(
    Icu_ChannelType Channel)
{
    uint32 baseAddr;

#if (STD_ON == ICU_DEV_ERROR_DETECT)
    if (ICU_STATUS_UNINIT == Icu_DrvStatus)
    {
        (void)Icu_reportDetError(ICU_ENABLEEDGECOUNT_ID, ICU_E_UNINIT);
    }
    else if (ICU_MAX_NUM_CHANNELS <= Channel)
    {
        (void)Icu_reportDetError(ICU_ENABLEEDGECOUNT_ID, ICU_E_PARAM_CHANNEL);
    }
    else if (ICU_MODE_EDGE_COUNTER != Icu_ChObj[Channel].chCfg.measurementMode)
    {
        (void)Icu_reportDetError(ICU_ENABLEEDGECOUNT_ID, ICU_E_PARAM_CHANNEL);
    }
    else
#endif /* (STD_ON == ICU_DEV_ERROR_DETECT) */
    {
        baseAddr = Icu_ChObj[Channel].baseAddr;
        SchM_Enter_Icu_ICU_EXCLUSIVE_AREA_0();
        Icu_ConfigEcap(baseAddr, Icu_ChObj[Channel].activation_edge, ICU_DELTA_MODE, TRUE);

        Icu_ChObj[Channel].IsRunning = TRUE;
        SchM_Exit_Icu_ICU_EXCLUSIVE_AREA_0();
    }
}

/*
* Design: MCAL-6894,MCAL-7133,MCAL-7126,MCAL-7054,MCAL-6912,MCAL-7021,MCAL-6927,MCAL-6923,MCAL-6922,MCAL-6916,MCAL-6935,MCAL-6897,MCAL-6953,MCAL-6965,MCAL-6965,MCAL-6940,MCAL-7029,MCAL-6936,MCAL-6962,MCAL-6981,MCAL-7056,MCAL-7070,MCAL-7095
*/
FUNC(void, ICU_CODE) Icu_DisableEdgeCount(
    Icu_ChannelType Channel)
{
    uint32 baseAddr;

#if (STD_ON == ICU_DEV_ERROR_DETECT)
    if (ICU_STATUS_UNINIT == Icu_DrvStatus)
    {
        (void)Icu_reportDetError(ICU_DISABLEEDGECOUNT_ID, ICU_E_UNINIT);
    }
    else if (ICU_MAX_NUM_CHANNELS <= Channel)
    {
        (void)Icu_reportDetError(ICU_DISABLEEDGECOUNT_ID, ICU_E_PARAM_CHANNEL);
    }
    else if (ICU_MODE_EDGE_COUNTER != Icu_ChObj[Channel].chCfg.measurementMode)
    {
        (void)Icu_reportDetError(ICU_DISABLEEDGECOUNT_ID, ICU_E_PARAM_CHANNEL);
    }
    else
#endif /* (STD_ON == ICU_DEV_ERROR_DETECT) */
    {
        baseAddr = Icu_ChObj[Channel].baseAddr;
        SchM_Enter_Icu_ICU_EXCLUSIVE_AREA_0();
        ECAPIntDisable(baseAddr, ECAP_INT_ALL);
        ECAPIntStatusClear(baseAddr, ECAP_INT_ALL);

        /* Disable CAP1-CAP4 register loads */
        ECAPCaptureLoadingDisable(baseAddr);

        Icu_ChObj[Channel].IsRunning = FALSE;
        SchM_Exit_Icu_ICU_EXCLUSIVE_AREA_0();
    }
}

/*
*Design: MCAL-7020,MCAL-7133,MCAL-7126,MCAL-7054,MCAL-6912,MCAL-7021,MCAL-6927,MCAL-6923,MCAL-6922,MCAL-7044,MCAL-6905,MCAL-7143,MCAL-6957,MCAL-6965,MCAL-7029,MCAL-6936,MCAL-6910,MCAL-7058,MCAL-7104
*/
FUNC(Icu_EdgeNumberType, ICU_CODE) Icu_GetEdgeNumbers(
    Icu_ChannelType Channel)
{
    Icu_EdgeNumberType edgecount = 0;

#if (STD_ON == ICU_DEV_ERROR_DETECT)
    if (ICU_STATUS_UNINIT == Icu_DrvStatus)
    {
        (void)Icu_reportDetError(ICU_GETEDGENUMBERS_ID, ICU_E_UNINIT);
    }
    else if (ICU_MAX_NUM_CHANNELS <= Channel)
    {
        (void)Icu_reportDetError(ICU_GETEDGENUMBERS_ID, ICU_E_PARAM_CHANNEL);
    }
    else if (ICU_MODE_EDGE_COUNTER != Icu_ChObj[Channel].chCfg.measurementMode)
    {
        (void)Icu_reportDetError(ICU_GETEDGENUMBERS_ID, ICU_E_PARAM_CHANNEL);
    }
    else
#endif /* (STD_ON == ICU_DEV_ERROR_DETECT) */
    {
        SchM_Enter_Icu_ICU_EXCLUSIVE_AREA_0();
        edgecount= Icu_ChObj[Channel].EdgeCounter;
        SchM_Exit_Icu_ICU_EXCLUSIVE_AREA_0();
    }

    return edgecount;
}

#endif /*ICU_EDGE_COUNT_API*/


#if (STD_ON == ICU_SIGNAL_MEASUREMENT_API)
/*
*Design: MCAL-6937,MCAL-7133,MCAL-7054,MCAL-6923,MCAL-6912,MCAL-7126,MCAL-7021,MCAL-6927,MCAL-6989,MCAL-7094,MCAL-7117,MCAL-6945,MCAL-6922,MCAL-6899,MCAL-7006,MCAL-7017,MCAL-7027,MCAL-7106,MCAL-7115,MCAL-6918,MCAL-6953,MCAL-7087,MCAL-6957,MCAL-6965,MCAL-6940,MCAL-7029,MCAL-6936
*/
FUNC(void, ICU_CODE) Icu_StartSignalMeasurement(
    Icu_ChannelType Channel)
{
    uint32 baseAddr;

#if (STD_ON == ICU_DEV_ERROR_DETECT)
    if (ICU_STATUS_UNINIT == Icu_DrvStatus)
    {
        (void)Icu_reportDetError(ICU_STARTSIGNALMEASUREMENT_ID, ICU_E_UNINIT);
    }
    else if (ICU_MAX_NUM_CHANNELS <= Channel)
    {
        (void)Icu_reportDetError(ICU_STARTSIGNALMEASUREMENT_ID, ICU_E_PARAM_CHANNEL);
    }
    else if (ICU_MODE_SIGNAL_MEASUREMENT != Icu_ChObj[Channel].chCfg.measurementMode)
    {
        (void)Icu_reportDetError(ICU_STARTSIGNALMEASUREMENT_ID, ICU_E_PARAM_CHANNEL);
    }
    else
#endif /* (STD_ON == ICU_DEV_ERROR_DETECT) */
    {
        baseAddr = Icu_ChObj[Channel].baseAddr;
        SchM_Enter_Icu_ICU_EXCLUSIVE_AREA_0();
        Icu_ChObj[Channel].InputState = ICU_IDLE;

        Icu_SignalMeasurement_Init(Channel);

        Icu_ConfigEcap(baseAddr, ICU_BOTH_EDGES, ICU_DELTA_MODE, FALSE);

        Icu_ChObj[Channel].IsRunning = TRUE;
         SchM_Exit_Icu_ICU_EXCLUSIVE_AREA_0();

    }

    return;

}
#endif /* (ICU_SIGNAL_MEASUREMENT_API == STD_ON) */

#if (ICU_SIGNAL_MEASUREMENT_API == STD_ON)
/*
*Design: MCAL-6939,MCAL-7100,MCAL-7054,MCAL-6912,MCAL-6923,MCAL-7133,MCAL-7126,MCAL-7145,MCAL-7021,MCAL-6927,MCAL-6922,MCAL-6899,MCAL-7032,MCAL-6971,MCAL-6970,MCAL-7063,MCAL-7147,MCAL-6908,MCAL-7036,MCAL-7087,MCAL-7102,MCAL-6957,MCAL-6965,MCAL-6940,MCAL-7029,MCAL-6936
*/
FUNC(void, ICU_CODE) Icu_StopSignalMeasurement(
    Icu_ChannelType Channel)
{
    uint32 baseAddr;
    Icu_ValueType highTime, lowTime, period;

#if (STD_ON == ICU_DEV_ERROR_DETECT)
    if (ICU_STATUS_UNINIT == Icu_DrvStatus)
    {
        (void)Icu_reportDetError(ICU_STOPSIGNALMEASUREMENT_ID, ICU_E_UNINIT);
    }
    else if (ICU_MAX_NUM_CHANNELS <= Channel)
    {
        (void)Icu_reportDetError(ICU_STOPSIGNALMEASUREMENT_ID, ICU_E_PARAM_CHANNEL);
    }
    else if (ICU_MODE_SIGNAL_MEASUREMENT != Icu_ChObj[Channel].chCfg.measurementMode)
    {
        (void)Icu_reportDetError(ICU_STOPSIGNALMEASUREMENT_ID, ICU_E_PARAM_CHANNEL);
    }
    else
#endif /* (STD_ON == ICU_DEV_ERROR_DETECT) */
    {
        baseAddr = Icu_ChObj[Channel].baseAddr;
         SchM_Enter_Icu_ICU_EXCLUSIVE_AREA_0();
        ECAPIntDisable(baseAddr, ECAP_INT_ALL);
        ECAPIntStatusClear(baseAddr, ECAP_INT_ALL);

        /* Disable CAP1-CAP4 register loads */
        ECAPCaptureLoadingDisable(baseAddr);

        Icu_ChObj[Channel].cap1 = ECAPTimeStampRead(baseAddr, ECAP_CAPTURE_EVENT_1);
        Icu_ChObj[Channel].cap2 = ECAPTimeStampRead(baseAddr, ECAP_CAPTURE_EVENT_2);
        Icu_ChObj[Channel].cap3 = ECAPTimeStampRead(baseAddr, ECAP_CAPTURE_EVENT_3);
        Icu_ChObj[Channel].cap4 = ECAPTimeStampRead(baseAddr, ECAP_CAPTURE_EVENT_4);


        /* ICU_CoverageGap_08: Dynamic coverage for the below "else" statement
         * expects hardware input for feedback. Thus it cannot be covered.
         */

        if ((Icu_ChObj[Channel].cap1 == 0U) ||
            (Icu_ChObj[Channel].cap2 == 0U) ||
            (Icu_ChObj[Channel].cap3 == 0U) ||
            (Icu_ChObj[Channel].cap4 == 0U))
        {
            Icu_ChObj[Channel].DutyAcquired = FALSE;
            Icu_ChObj[Channel].PeriodAcquired = FALSE;
        }
        else
        {
            uint32 clkFreq_Mhz = Icu_ChObj[Channel].chCfg.instanceClkMHz;

            highTime = Icu_ChObj[Channel].cap2 / clkFreq_Mhz;
            lowTime = Icu_ChObj[Channel].cap3 / clkFreq_Mhz;
            period = highTime + lowTime;

            Icu_ChObj[Channel].HighTime = highTime;
            Icu_ChObj[Channel].LowTime = lowTime;
            Icu_ChObj[Channel].Period = period;
            Icu_ChObj[Channel].DutyCycle.PeriodTime = period;
            Icu_ChObj[Channel].DutyCycle.ActiveTime = highTime;
            Icu_ChObj[Channel].DutyAcquired = TRUE;
            Icu_ChObj[Channel].PeriodAcquired = TRUE;
        }

        Icu_ChObj[Channel].IsRunning = FALSE;
        SchM_Exit_Icu_ICU_EXCLUSIVE_AREA_0();
    }

    return;
}

#endif /*(ICU_SIGNAL_MEASUREMENT_API == STD_ON)*/


#if (STD_ON == ICU_GET_TIME_ELAPSED_API)
/*
*Design: MCAL-6924,MCAL-7133,MCAL-7054,MCAL-6912,MCAL-6923,MCAL-7126,MCAL-7021,MCAL-6927,MCAL-7145,MCAL-6922,MCAL-6944,MCAL-6965,MCAL-6957,MCAL-6990,MCAL-6975,MCAL-6966,MCAL-6951,MCAL-7000,MCAL-7029,MCAL-7051,MCAL-7052,MCAL-7092,MCAL-7124,MCAL-7066,MCAL-7087,MCAL-6940,MCAL-6936
*/
FUNC(Icu_ValueType, ICU_CODE) Icu_GetTimeElapsed (
        Icu_ChannelType Channel)
{
    Icu_ValueType elapsedTime = 0U;

#if (STD_ON == ICU_DEV_ERROR_DETECT)
    if (ICU_STATUS_UNINIT == Icu_DrvStatus)
    {
        (void)Icu_reportDetError(ICU_GETTIMEELAPSED_ID, ICU_E_UNINIT);
    }
    else if (ICU_MAX_NUM_CHANNELS <= Channel)
    {
        (void)Icu_reportDetError(ICU_GETTIMEELAPSED_ID, ICU_E_PARAM_CHANNEL);
    }
    else if (ICU_MODE_SIGNAL_MEASUREMENT != Icu_ChObj[Channel].chCfg.measurementMode)
    {
        (void)Icu_reportDetError(ICU_GETTIMEELAPSED_ID, ICU_E_PARAM_CHANNEL);
    }
    else
#endif /* (STD_ON == ICU_DEV_ERROR_DETECT) */
    {
        SchM_Enter_Icu_ICU_EXCLUSIVE_AREA_0();
        #if (ICU_SIGNAL_MEASUREMENT_API == STD_ON)
            if (Icu_ChObj[Channel].PeriodAcquired == TRUE)
            {

                /* ICU_CoverageGap_09: Dynamic coveragefor the “switch” statements
                 * expects hardware input for feedback.
                */

                switch (Icu_ChObj[Channel].chCfg.signalMeasurementProperty)
                {
                    case ICU_DUTY_CYCLE:
                        elapsedTime = Icu_ChObj[Channel].Period;
                        break;
                    case ICU_PERIOD_TIME:
                        elapsedTime = Icu_ChObj[Channel].Period;
                        break;
                    case ICU_HIGH_TIME:
                        elapsedTime = Icu_ChObj[Channel].HighTime;
                        break;
                    case ICU_LOW_TIME:
                        elapsedTime = Icu_ChObj[Channel].LowTime;
                        break;
                    default:
                        elapsedTime = 0U;
                        break;
                }
                Icu_ChObj[Channel].PeriodAcquired = FALSE;
            }

            else
            {
                elapsedTime = 0U;
            }
        #endif /* (ICU_SIGNAL_MEASUREMENT_API == STD_ON) */
        SchM_Exit_Icu_ICU_EXCLUSIVE_AREA_0();
    }
    return elapsedTime;
}

#endif

#if (STD_ON == ICU_GET_DUTY_CYCLE_VALUES_API)
/*
*Design: MCAL-6902,MCAL-7054,MCAL-6912,MCAL-7133,MCAL-7126,MCAL-6923,MCAL-6942,MCAL-7021,MCAL-6927,MCAL-6922,MCAL-6914,MCAL-6969,MCAL-6979,MCAL-7019,MCAL-6904,MCAL-7103,MCAL-7130,MCAL-7073,MCAL-7087,MCAL-6965,MCAL-6940,MCAL-7029,MCAL-6936
*/
FUNC (void, ICU_CODE) Icu_GetDutyCycleValues( Icu_ChannelType Channel,
                    Icu_DutyCycleType* DutyCycleValues )
{
#if (STD_ON == ICU_DEV_ERROR_DETECT)
    if (ICU_STATUS_UNINIT == Icu_DrvStatus)
    {
        (void)Icu_reportDetError(ICU_GETDUTYCYCLEVALUES_ID, ICU_E_UNINIT);
    }
    else if (ICU_MAX_NUM_CHANNELS <= Channel)
    {
        (void)Icu_reportDetError(ICU_GETDUTYCYCLEVALUES_ID, ICU_E_PARAM_CHANNEL);
    }
    else if (ICU_MODE_SIGNAL_MEASUREMENT != Icu_ChObj[Channel].chCfg.measurementMode)
    {
        (void)Icu_reportDetError(ICU_GETDUTYCYCLEVALUES_ID, ICU_E_PARAM_CHANNEL);
    }
    else if (NULL_PTR == DutyCycleValues)
    {
        (void)Icu_reportDetError(ICU_GETDUTYCYCLEVALUES_ID, ICU_E_PARAM_POINTER);
    }
    else
#endif /* (STD_ON == ICU_DEV_ERROR_DETECT) */
    {
         SchM_Enter_Icu_ICU_EXCLUSIVE_AREA_0();
    #if (ICU_SIGNAL_MEASUREMENT_API == STD_ON)

            if (Icu_ChObj[Channel].DutyAcquired == TRUE)
            {
                DutyCycleValues->PeriodTime = Icu_ChObj[Channel].DutyCycle.PeriodTime;
                DutyCycleValues->ActiveTime = Icu_ChObj[Channel].DutyCycle.ActiveTime;

                Icu_ChObj[Channel].DutyAcquired = FALSE;
            }
            else
            {
                DutyCycleValues->PeriodTime = 0U;
                DutyCycleValues->ActiveTime = 0U;
            }
    #endif /* (ICU_SIGNAL_MEASUREMENT_API == STD_ON) */
    SchM_Exit_Icu_ICU_EXCLUSIVE_AREA_0();
    }
}

#endif


#if (STD_ON == ICU_GET_VERSION_INFO_API)
/*
*Design: MCAL-7043,MCAL-6936
*/
FUNC(void, ICU_CODE) Icu_GetVersionInfo(
            P2VAR(Std_VersionInfoType, AUTOMATIC, ICU_APPL_DATA) VersionInfoPtr)
{
#if (STD_ON == ICU_DEV_ERROR_DETECT)
    if (NULL_PTR == VersionInfoPtr)
    {
        Icu_reportDetError(ICU_GETVERSIONINFO_ID, ICU_E_PARAM_VINFO);
    }
    else
#endif
    {
         SchM_Enter_Icu_ICU_EXCLUSIVE_AREA_0();
        VersionInfoPtr->vendorID         = ICU_VENDOR_ID;
        VersionInfoPtr->moduleID         = ICU_MODULE_ID;
        VersionInfoPtr->sw_major_version = (uint8) ICU_SW_MAJOR_VERSION;
        VersionInfoPtr->sw_minor_version = (uint8) ICU_SW_MINOR_VERSION;
        VersionInfoPtr->sw_patch_version = (uint8) ICU_SW_PATCH_VERSION;
        SchM_Exit_Icu_ICU_EXCLUSIVE_AREA_0();
    }

    return;
}
#endif /* ICU_GET_VERSION_INFO_API*/

#if (STD_ON == ICU_SAFETY_DIAGNOSTIC_API)
/*
 * Design: MCAL-6980,MCAL-6903,MCAL-6892,MCAL-7133,MCAL-7126,MCAL-7021,MCAL-6927,MCAL-6923,MCAL-6936
 */
FUNC(Std_ReturnType, ICU_CODE) Icu_RegisterReadback(
  Icu_ChannelType Channel,
      P2VAR(Icu_RegisterReadbackType, AUTOMATIC, ICU_APPL_DATA) RegRbPtr)
{

    Std_ReturnType     retVal = ((Std_ReturnType) E_OK);

    #if (STD_ON == ICU_DEV_ERROR_DETECT)
        if (Channel >= (uint32)ICU_MAX_NUM_CHANNELS)
        {
            /*Invalid channel */
            (void)Icu_reportDetError(ICU_SID_REGISTER_READBACK, ICU_E_PARAM_CHANNEL);
        }
        else if (ICU_STATUS_UNINIT == Icu_DrvStatus)
        {
            (void)Icu_reportDetError(ICU_SID_REGISTER_READBACK, ICU_E_UNINIT);
        }
        else if (NULL_PTR == RegRbPtr)
        {
            (void)Icu_reportDetError(ICU_SID_REGISTER_READBACK, ICU_E_PARAM_POINTER);
        }
        else
    #endif

   {

      (void) memset(RegRbPtr, 0, sizeof (Icu_RegisterReadbackType));

            uint32 baseAddr;

            baseAddr = Icu_ChObj[Channel].baseAddr;
            RegRbPtr->ECAP_CNTPHS = HW_RD_REG32((baseAddr + ECAP_CTRPHS));
            RegRbPtr->ECAP_ECCTL = HW_RD_REG32((baseAddr+ ECAP_ECCTL1));
            RegRbPtr->ECAP_ECINT_EN_FLG = HW_RD_REG32((baseAddr+ ECAP_ECEINT));

    }

    return (retVal);
}
#endif
#if (ICU_WAKEUP_FUNCTIONALITY_API == STD_ON)
FUNC(void, ICU_CODE) Icu_CheckWakeup(EcuM_WakeupSourceType WakeupSource)
{
    /*Dummy API, Functionality Not Supported due to HW Limitation */
    return;
}
#endif /* if(ICU_WAKEUP_FUNCTIONALITY_API == STD_ON) */

#if (ICU_DISABLEWAKEUP_API == STD_ON)
FUNC(void, ICU_CODE) Icu_DisableWakeup(Icu_ChannelType Channel)
{
    /*Dummy API, Functionality Not Supported due to HW Limitation */
    return;
}
#endif /* if(ICU_DISABLEWAKEUP_API == STD_ON) */

#if (ICU_ENABLEWAKEUP_API == STD_ON)
FUNC(void, ICU_CODE) Icu_EnableWakeup(Icu_ChannelType Channel)
{
    /*Dummy API, Functionality Not Supported due to HW Limitation */
    return;
}
#endif /* if(ICU_ENABLEWAKEUP_API == STD_ON) */

#if (ICU_SETMODE_API == STD_ON)
FUNC(void, ICU_CODE) Icu_SetMode(Icu_ModeType Mode)
{
    /*Dummy API, Functionality Not Supported due to HW Limitation */
    return;
}
#endif /* if (ICU_SETMODE_API == STD_ON) */

#define ICU_STOP_SEC_CODE
#include "Icu_MemMap.h"
