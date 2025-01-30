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
  *  \file      Fls.c
  *
  *  \brief    This file contains Fls MCAL driver
  *
  */

 /* ========================================================================== */
 /*                             Include Files                                  */
 /* ========================================================================== */
#include "string.h"
#include "Fls_Cfg.h"

#if defined (FLS_NOR)
#include "Fls_Brd_Nor.h"
#endif

#include "Det.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* AUTOSAR version information check has to match definition in header file */
#if ((FLS_AR_RELEASE_MAJOR_VERSION != (4U)) || \
    (FLS_AR_RELEASE_MINOR_VERSION != (3U)) ||  \
    (FLS_AR_RELEASE_REVISION_VERSION != (1U)))
    #error "Fls: AUTOSAR Version Numbers of Fls are different!!"
#endif

/* AUTOSAR version information check has to match definition in header file */
#if ((FLS_SW_MAJOR_VERSION != (10U)) || \
    (FLS_SW_MINOR_VERSION != (1U)) ||  \
    (FLS_SW_PATCH_VERSION != (0U)))
    #error "Fls: Software Version Numbers are inconsistent!!"
#endif

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

#if (STD_ON == FLS_DEV_ERROR_DETECT)
/**
 * Check Flash bank ranges for a valid address
 *
 */
static Std_ReturnType checkValidAddress( Fls_AddressType SourceAddress )
{
    Std_ReturnType retVal = E_NOT_OK;
    uint32 startAddr = 0U;
    /* FLS_CoverageGap_02:Dynamic branch coverage for this check for ( SourceAddress >= startAddr ) is an additional check,
     * which will never be reached as address cannot be less than zero.
     */
    if( (SourceAddress >= startAddr) &&
    	(SourceAddress <= (startAddr + NOR_SIZE)) )
    {
        retVal = E_OK;
    }
    return retVal;
}
#endif /*#if (STD_ON == FLS_DEV_ERROR_DETECT)*/

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

#define FLS_START_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Fls_MemMap.h"
/** \brief FLS driver object */

Fls_DriverObjType Fls_DrvObj =
{
    .status = MEMIF_UNINIT
};
#define FLS_STOP_SEC_VAR_NO_INIT_UNSPECIFIED
#include "Fls_MemMap.h"

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

#define FLS_START_SEC_CODE
#include "Fls_MemMap.h"
/*
 * Design : MCAL-7254,MCAL-7366,MCAL-7312,MCAL-7425,MCAL-7289,MCAL-7259,MCAL-7330,MCAL-7332,MCAL-7394,MCAL-7275,MCAL-7446,MCAL-7251
 */
FUNC(void, FLS_CODE) Fls_Init(
        P2CONST(Fls_ConfigType, AUTOMATIC, FLS_CONFIG_DATA) ConfigPtr)
{
    const Fls_ConfigType *CfgPtr = ConfigPtr;
    Std_ReturnType ret;

#if (STD_ON == FLS_PRE_COMPILE_VARIANT)
    if (NULL_PTR == ConfigPtr)
    {
      CfgPtr = &FLS_INIT_CONFIG_PC;
    }
#endif  /* (STD_ON == SPI_PRE_COMPILE_VARIANT) */

#if (STD_ON == FLS_DEV_ERROR_DETECT)
    if (MEMIF_BUSY == Fls_DrvObj.status)
    {
        (void) Det_ReportError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_INIT, FLS_E_BUSY);
    }
    else if(NULL_PTR == CfgPtr)
    {
		(void) Det_ReportError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_INIT, FLS_E_PARAM_CONFIG);
    }
    else if ((CfgPtr->maxReadNormalMode == 0U) || ((CfgPtr->maxReadNormalMode % NOR_PAGE_SIZE) != 0U))
    {
        (void) Det_ReportError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_INIT, FLS_E_PARAM_CONFIG);
    }
    else if ((CfgPtr->maxWriteNormalMode == 0U) || ((CfgPtr->maxWriteNormalMode % NOR_PAGE_SIZE) != 0U))
    {
        (void) Det_ReportError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_INIT, FLS_E_PARAM_CONFIG);
    }
    else
#endif /* #if (STD_ON == FLS_DEV_ERROR_DETECT) */
    {
        /* Instantiate a Driver Obj to be used by module */
        Fls_resetDrvObj(&Fls_DrvObj);
        /*Copy the input config parameters to Driver Object */
        Fls_copyConfig(&Fls_DrvObj, CfgPtr);
        Fls_DrvObj.jobResultType = MEMIF_JOB_PENDING;
        ret = Fls_hwUnitInit();

        /*Init the HW */
        if(ret == (Std_ReturnType)E_OK)
        {
            Fls_DrvObj.jobResultType = MEMIF_JOB_OK;
            Fls_DrvObj.status = MEMIF_IDLE;
        }
    }
    return;
}

/*
 * Design : MCAL-7445,MCAL-7464,MCAL-7327,MCAL-7286,MCAL-7249,MCAL-7399,MCAL-7398,MCAL-7418,MCAL-7421,MCAL-7394,MCAL-7332,MCAL-7275,MCAL-7400,MCAL-7307
 */
FUNC(Std_ReturnType, FLS_CODE) Fls_Erase(
        Fls_AddressType TargetAddress, Fls_LengthType Length)
{
    Std_ReturnType      retVal = (Std_ReturnType) E_OK;

#if (STD_ON == FLS_DEV_ERROR_DETECT)
    if (MEMIF_UNINIT == Fls_DrvObj.status)
    {
		(void) Det_ReportError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_ERASE, FLS_E_UNINIT);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else if(MEMIF_BUSY == Fls_DrvObj.status)
    {
		(void) Det_ReportError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_ERASE, FLS_E_BUSY);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else if (E_NOT_OK == checkValidAddress(TargetAddress))
    {
		(void) Det_ReportError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_ERASE, FLS_E_PARAM_ADDRESS);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else if ((Length == 0U) || (E_NOT_OK == checkValidAddress(TargetAddress + Length)))
    {
		(void) Det_ReportError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_ERASE, FLS_E_PARAM_LENGTH);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else
#endif /* #if (STD_ON == FLS_DEV_ERROR_DETECT) */
    {
        Fls_DrvObj.status = MEMIF_BUSY;
        Fls_DrvObj.jobResultType = MEMIF_JOB_PENDING;
        Fls_DrvObj.jobType = FLS_JOB_ERASE;

        Fls_DrvObj.flashAddr = TargetAddress;
        Fls_DrvObj.ramAddr = NULL;

        Fls_DrvObj.transferred = 0;
#if defined(FLS_XSPI)
        Fls_DrvObj.length = Length + (TargetAddress % NOR_BLOCK_SIZE);
        Fls_DrvObj.jobChunkSize = NOR_BLOCK_SIZE;
#elif defined(FLS_OSPI) || defined(FLS_QSPI)
        Fls_DrvObj.length = Length + (TargetAddress % NOR_SECTOR_SIZE);
        Fls_DrvObj.jobChunkSize = NOR_SECTOR_SIZE;

#endif
    }

    return retVal;
};
/*
 * Design : MCAL-7381,MCAL-7301,MCAL-7453,MCAL-7285,MCAL-7462,MCAL-7394,MCAL-7332,MCAL-7275,MCAL-7247,MCAL-7282,MCAL-7297,MCAL-7304,MCAL-7337,MCAL-7341,MCAL-7356,MCAL-7386,MCAL-7436
 */
FUNC(Std_ReturnType, FLS_CODE) Fls_Read(
        Fls_AddressType SourceAddress,
        P2VAR(uint8, AUTOMATIC, FLS_APPL_DATA) TargetAddressPtr,
        Fls_LengthType Length)
{
    Std_ReturnType      retVal = (Std_ReturnType) E_OK;

#if (STD_ON == FLS_DEV_ERROR_DETECT)
    if (MEMIF_UNINIT == Fls_DrvObj.status)
    {
		(void) Det_ReportError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_READ, FLS_E_UNINIT);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    /* FLS_CoverageGap_01:Dynamic branch coverage for this conditional else check is additional check which does not occur as there are already checks done in Fls_Init,
     * which sets the status as idle but if the API is called without initialization then there will be a DET for driver Uninit.
     */
    else if(MEMIF_BUSY == Fls_DrvObj.status)
    {
		(void) Det_ReportError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_READ, FLS_E_BUSY);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else if(NULL_PTR == TargetAddressPtr)
    {
		(void) Det_ReportError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_READ, FLS_E_PARAM_DATA);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else if (E_NOT_OK == checkValidAddress(SourceAddress))
    {
		(void) Det_ReportError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_READ, FLS_E_PARAM_ADDRESS);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else if ((Length == 0U) || (E_NOT_OK == checkValidAddress(SourceAddress + Length)))
    {
		(void) Det_ReportError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_READ, FLS_E_PARAM_LENGTH);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else
#endif /* #if (STD_ON == FLS_DEV_ERROR_DETECT) */
    {
        Fls_DrvObj.status = MEMIF_BUSY;
        Fls_DrvObj.jobResultType = MEMIF_JOB_PENDING;
        Fls_DrvObj.jobType = FLS_JOB_READ;

        Fls_DrvObj.flashAddr = SourceAddress;
        Fls_DrvObj.ramAddr = (uint8*) TargetAddressPtr;
        Fls_DrvObj.length = Length;
        Fls_DrvObj.transferred = 0;

        Fls_DrvObj.jobChunkSize = Fls_DrvObj.maxReadNormalMode;
#if (STD_ON == FLS_USE_INTERRUPTS)
        processJobs(Fls_DrvObj.jobType);
#endif
    }

    return retVal;
}

/*
 * Design : MCAL-7295,MCAL-7265,MCAL-7248,MCAL-7288,MCAL-7261,MCAL-7271,MCAL-7271,MCAL-7394,MCAL-7332,MCAL-7275,MCAL-7390,MCAL_7442,MCAL-7303,MCAL-7315,MCAL-7350,MCAL-7359,MCAL-7368,MCAL-7442
 */
FUNC(Std_ReturnType, FLS_CODE) Fls_Write(
        Fls_AddressType TargetAddress,
        P2VAR(const uint8, AUTOMATIC, FLS_APPL_DATA) SourceAddressPtr,
        Fls_LengthType Length)
{
    Std_ReturnType      retVal = (Std_ReturnType) E_OK;

#if (STD_ON == FLS_DEV_ERROR_DETECT)
    if (MEMIF_UNINIT == Fls_DrvObj.status)
    {
		(void) Det_ReportError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_WRITE, FLS_E_UNINIT);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    /* FLS_CoverageGap_01:Dynamic branch coverage for this conditional else check is additional check which does not occur as there are already checks done in Fls_Init,
     * which sets the status as idle but if the API is called without initialization then there will be a DET for driver Uninit.
     */
    else if(MEMIF_BUSY == Fls_DrvObj.status)
    {
		(void) Det_ReportError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_WRITE, FLS_E_BUSY);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else if(NULL_PTR == SourceAddressPtr)
    {
		(void) Det_ReportError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_WRITE, FLS_E_PARAM_DATA);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else if (((TargetAddress % NOR_PAGE_SIZE) != 0U) || (E_NOT_OK == checkValidAddress(TargetAddress)))
    {
		(void) Det_ReportError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_WRITE, FLS_E_PARAM_ADDRESS);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else if (((Length == 0U) || ((((TargetAddress + Length) % NOR_PAGE_SIZE) != 0U)
    || (E_NOT_OK == checkValidAddress(TargetAddress + Length)))))
    {
		(void) Det_ReportError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_WRITE, FLS_E_PARAM_LENGTH);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else
#endif /* #if (STD_ON == FLS_DEV_ERROR_DETECT) */
    {
        Fls_DrvObj.jobResultType = MEMIF_JOB_PENDING;
        Fls_DrvObj.status = MEMIF_BUSY;
        Fls_DrvObj.jobType = FLS_JOB_WRITE;

        Fls_DrvObj.flashAddr = TargetAddress;
        Fls_DrvObj.ramAddr = (uint8*)SourceAddressPtr;
        Fls_DrvObj.length = Length;
        Fls_DrvObj.transferred = 0;

        Fls_DrvObj.jobChunkSize = Fls_DrvObj.maxWriteNormalMode;
#if (STD_ON == FLS_USE_INTERRUPTS)
        processJobs(Fls_DrvObj.jobType);
#endif
    }
    return retVal;

};
/*
 * Design : MCAL-7345,MCAL-7321,MCAL-7380,MCAL-7336,MCAL-7393,MCAL-7459,MCAL-7463,MCAL-7420,MCAL-7435,MCAL-7394,MCAL-7332,MCAL-7275,MCAL-7258,MCAL-7266,MCAL-7281,MCAL-7293,MCAL-7406
 */
#if ( FLS_COMPARE_API == STD_ON )
FUNC(Std_ReturnType, FLS_CODE) Fls_Compare(
        Fls_AddressType SourceAddress,
        P2VAR(const uint8, AUTOMATIC, FLS_APPL_DATA) TargetAddressPtr,
        Fls_LengthType Length)
{
    Std_ReturnType      retVal = (Std_ReturnType) E_OK;

#if (STD_ON == FLS_DEV_ERROR_DETECT)
    if (MEMIF_UNINIT == Fls_DrvObj.status)
    {
		(void) Det_ReportError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_COMPARE, FLS_E_UNINIT);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    /* FLS_CoverageGap_01:Dynamic branch coverage for this conditional else check is additional check which does not occur as there are already checks done in Fls_Init,
     * which sets the status as idle but if the API is called without initialization then there will be a DET for driver Uninit.
     */
    else if(MEMIF_BUSY == Fls_DrvObj.status)
    {
		(void) Det_ReportError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_COMPARE, FLS_E_BUSY);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else if(NULL_PTR == TargetAddressPtr)
    {
		(void) Det_ReportError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_COMPARE, FLS_E_PARAM_DATA);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else if (E_NOT_OK == checkValidAddress(SourceAddress))
    {
		(void) Det_ReportError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_COMPARE, FLS_E_PARAM_ADDRESS);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else if ((Length == 0U) || (E_NOT_OK == checkValidAddress(SourceAddress + Length)))
    {
		(void) Det_ReportError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_COMPARE, FLS_E_PARAM_LENGTH);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else
#endif /* #if (STD_ON == FLS_DEV_ERROR_DETECT) */
    {
        Fls_DrvObj.status = MEMIF_BUSY;
        Fls_DrvObj.jobResultType = MEMIF_JOB_PENDING;
        Fls_DrvObj.jobType = FLS_JOB_COMPARE;

        Fls_DrvObj.flashAddr = SourceAddress;
        Fls_DrvObj.ramAddr = (uint8*)TargetAddressPtr;
        Fls_DrvObj.length = Length;
        Fls_DrvObj.transferred = 0;

        Fls_DrvObj.jobChunkSize = Fls_DrvObj.maxReadNormalMode;
    }
    return retVal;
}
#endif

/*
 * Design : MCAL-7383,MCAL-7456,MCAL-7256,MCAL-7294,MCAL-7306,MCAL-7279,MCAL-7310,MCAL-7361,MCAL-7433,MCAL-7394,MCAL-7332,MCAL-7275,MCAL-7419,MCAL-7437,MCAL-7447
 */
#if ( FLS_BLANK_CHECK_API == STD_ON )
FUNC(Std_ReturnType, FLS_CODE) Fls_BlankCheck(
        Fls_AddressType TargetAddress, Fls_LengthType Length)
{
    Std_ReturnType      retVal = (Std_ReturnType) E_OK;

#if (STD_ON == FLS_DEV_ERROR_DETECT)
    if (MEMIF_UNINIT == Fls_DrvObj.status)
    {
		(void) Det_ReportError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_BLANK_CHECK, FLS_E_UNINIT);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    /* FLS_CoverageGap_01:Dynamic branch coverage for this conditional else check is additional check which does not occur as there are already checks done in Fls_Init,
     * which sets the status as idle but if the API is called without initialization then there will be a DET for driver Uninit.
     */
    else if(MEMIF_BUSY == Fls_DrvObj.status)
    {
		(void) Det_ReportError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_BLANK_CHECK, FLS_E_BUSY);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else if (E_NOT_OK == checkValidAddress(TargetAddress))
    {
		(void) Det_ReportError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_BLANK_CHECK, FLS_E_PARAM_ADDRESS);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else if ((Length == 0U) || (E_NOT_OK == checkValidAddress(TargetAddress + Length)))
    {
		(void) Det_ReportError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_BLANK_CHECK, FLS_E_PARAM_LENGTH);
        retVal = (Std_ReturnType) E_NOT_OK;
    }
    else
#endif /* #if (STD_ON == FLS_DEV_ERROR_DETECT) */
    {
        Fls_DrvObj.status = MEMIF_BUSY;
        Fls_DrvObj.jobResultType = MEMIF_JOB_PENDING;
        Fls_DrvObj.jobType = FLS_JOB_BLANKCHECK;

        Fls_DrvObj.flashAddr = TargetAddress;
        Fls_DrvObj.ramAddr = NULL;
        Fls_DrvObj.length = Length;
        Fls_DrvObj.transferred = 0;

        Fls_DrvObj.jobChunkSize = Fls_DrvObj.maxReadNormalMode;
    }
    return retVal;
}
#endif


/*
 * Design : MCAL-7378,MCAL-7439,MCAL-7408
 */
#if ( STD_ON == FLS_GET_STATUS_API)
FUNC(MemIf_StatusType, FLS_CODE)
        Fls_GetStatus( void )
{
    return Fls_DrvObj.status;
}
#endif


/*
 * Design : MCAL-7340,MCAL-7410,MCAL-7416,MCAL-7413,MCAL-7353,MCAL-7332
 */
#if ( FLS_GET_JOB_RESULT_API == STD_ON )
FUNC(MemIf_JobResultType, FLS_CODE)
        Fls_GetJobResult( void )
{
    MemIf_JobResultType jobResult = MEMIF_JOB_FAILED;
#if (STD_ON == FLS_DEV_ERROR_DETECT)
    if (MEMIF_UNINIT == Fls_DrvObj.status)
    {
		(void) Det_ReportError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_GET_JOB_RESULT, FLS_E_UNINIT);
    }
    else
#endif /* #if (STD_ON == FLS_DEV_ERROR_DETECT) */
    {
        jobResult = Fls_DrvObj.jobResultType;
    }
    return jobResult;
}
#endif

/*
 * Design : MCAL-7252,MCAL-7394
 */
#if (STD_ON == FLS_VERSION_INFO_API)
FUNC(Std_ReturnType, FLS_CODE) Fls_GetVersionInfo(
        P2VAR(Std_VersionInfoType, AUTOMATIC, FLS_APPL_DATA) versioninfo)
{
    Std_ReturnType      retVal = (Std_ReturnType) E_OK;

#if (STD_ON == FLS_DEV_ERROR_DETECT)
    if (NULL_PTR == versioninfo)
    {
		(void) Det_ReportError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_GET_VERSION_INFO, FLS_E_PARAM_POINTER);
        retVal = E_NOT_OK;
    }
    else
#endif  /* #if (STD_ON == FLS_DEV_ERROR_DETECT) */
    {
        versioninfo->vendorID         = FLS_VENDOR_ID;
        versioninfo->moduleID         = FLS_MODULE_ID;
        versioninfo->sw_major_version = (uint8) FLS_SW_MAJOR_VERSION;
        versioninfo->sw_minor_version = (uint8) FLS_SW_MINOR_VERSION;
        versioninfo->sw_patch_version = (uint8) FLS_SW_PATCH_VERSION;
    }
    return retVal;
}
#endif  /* #if (STD_ON == FLS_VERSION_INFO_API) */


/*
 * Design : MCAL-7403,MCAL-7376,MCAL-7305,MCAL-7443,MCAL-7268,MCAL-7448,MCAL-7332,MCAL-7255,MCAL-7278,MCAL-7318,MCAL-7324,MCAL-7335,MCAL-7422,MCAL-7426,MCAL-7449,MCAL-7395,MCAL-7362
 */

/* FLS_CoverageGap_03:Dynamic branch coverage for this check for default case is an additional check. */
FUNC(void, FLS_CODE) Fls_MainFunction(void)
{
  #if (FLS_TIMEOUT_SUPERVISION_ENABLED == STD_ON)
    TickType startCount = 0U,tempCount = 0U, elapsedCount = 0U;
    StatusType status;
  #endif

#if (STD_ON == FLS_DEV_ERROR_DETECT)
    if (MEMIF_UNINIT == Fls_DrvObj.status)
    {
		(void) Det_ReportError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_MAIN_FUNCTION, FLS_E_UNINIT);
    }
    else
#endif  /* #if (STD_ON == FLS_DEV_ERROR_DETECT) */
    {
        if ( Fls_DrvObj.jobResultType == MEMIF_JOB_PENDING)
        {
            switch (Fls_DrvObj.jobType)
            {
                case FLS_JOB_COMPARE:
                    processJobs(FLS_JOB_COMPARE);
                    break;
                case FLS_JOB_ERASE:
                  #if (FLS_TIMEOUT_SUPERVISION_ENABLED == STD_ON)
                    status = GetCounterValue(FLS_OS_COUNTER_ID, &startCount);
                  #endif
                    processJobs(FLS_JOB_ERASE);
                  #if (FLS_TIMEOUT_SUPERVISION_ENABLED == STD_ON)
                      if (E_OK == status)
                      {
                            tempCount = startCount;
                            status = GetElapsedValue(FLS_OS_COUNTER_ID, &tempCount, &elapsedCount);
                            if ((E_OK == status) && (elapsedCount > (1000000U * FLS_MAX_ERASE_TIME)))
                            {
                                Det_ReportRuntimeError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_ERASE, FLS_E_TIMEOUT);
                                if( Fls_DrvObj.Fls_JobErrorNotification != NULL_PTR )
                                {
                                    Fls_DrvObj.Fls_JobErrorNotification();
                                }
                                else{
                                    /* Do Nothing */
                                }
                            }
                            else{
                                /* Do Nothing */
                            }
                        }
                        else{
                            /* Do Nothing */
                        }
                  #endif
                    break;
                case FLS_JOB_READ:
                    processJobs(FLS_JOB_READ);
                    break;
                case FLS_JOB_WRITE:
                  #if (FLS_TIMEOUT_SUPERVISION_ENABLED == STD_ON)
                    status = GetCounterValue(FLS_OS_COUNTER_ID, &startCount);
                  #endif

                    processJobs(FLS_JOB_WRITE);
                    
                  #if (FLS_TIMEOUT_SUPERVISION_ENABLED == STD_ON)
                    if (E_OK == status)
                    {
                        /* Below API can change start time, so use temp variable */
                        tempCount = startCount;
                        status = GetElapsedValue(FLS_OS_COUNTER_ID, &tempCount, &elapsedCount);

                        if ((E_OK == status) && (elapsedCount > (1000000U * (Fls_DrvObj.maxWriteNormalMode/Fls_DrvObj.sectorList[0].sectorPageSize) * FLS_MAX_WRITE_TIME)))
                        {
                          Det_ReportRuntimeError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_WRITE, FLS_E_TIMEOUT);
                              if( Fls_DrvObj.Fls_JobErrorNotification != NULL_PTR )
                              {
                                  Fls_DrvObj.Fls_JobErrorNotification();
                              }
                              else{
                                  /* Do Nothing */
                              }
                          }
                          else{
                                /* Do Nothing */
                          }
                    }
                    else{
                      /* Do Nothing */
                    }
                  #endif
                    break;
                case FLS_JOB_BLANKCHECK:
                    processJobs(FLS_JOB_BLANKCHECK);
                    break;
                case FLS_JOB_NONE:
                    break;
/* FLS_CoverageGap_03:Dynamic branch coverage for this check for default case is an additional check. */
                default:
                    break;
            } /* switch */
        }   /* if */
        else if ( Fls_DrvObj.jobResultType == MEMIF_JOB_CANCELED)
        {
          if( Fls_DrvObj.Fls_JobErrorNotification != NULL )
            {
              Fls_DrvObj.Fls_JobErrorNotification();
            }
        }
        else
        {
          if(Fls_DrvObj.Fls_JobEndNotification != NULL_PTR)
          {
           /* If not pending job, then return end notification */
           Fls_DrvObj.Fls_JobEndNotification();
          }
        }
    }
}

FUNC(void, FLS_CODE) Fls_SwitchMode(boolean dacEnable, boolean xipEnable)
{
    Fls_NorOspiClose(); /*Close the device before reopening with new modes */
    Fls_DrvObj.dacEnable = dacEnable;
    Fls_DrvObj.xipEnable = xipEnable;
    Fls_norOpen();
}

/*
 * Design : MCAL-7264,MCAL-7270,MCAL-7274,MCAL-7458,MCAL-7299,MCAL-7319,MCAL-7389,MCAL-7351,MCAL-7349,MCAL-7412,MCAL-7260,MCAL-7326,MCAL-7346,MCAL-7296,
 */
#if ( FLS_CANCEL_API == STD_ON )
FUNC(void, FLS_CODE) Fls_Cancel(void)
{
	#if (STD_ON == FLS_DEV_ERROR_DETECT)
	if (MEMIF_UNINIT == Fls_DrvObj.status)
	{
		(void) Det_ReportError(FLS_MODULE_ID, FLS_INSTANCE_ID, FLS_SID_CANCEL, FLS_E_UNINIT);
	}
	else
	#endif /* #if (STD_ON == FLS_DEV_ERROR_DETECT) */
	{
		/* Reset internal job processing variables (like address, length and data pointer) */
		Fls_DrvObj.flashAddr = 0U;
		Fls_DrvObj.ramAddr = NULL;
		Fls_DrvObj.length = 0U;
		Fls_DrvObj.transferred = 0;
		Fls_DrvObj.status = MEMIF_IDLE;
		Fls_DrvObj.jobType = FLS_JOB_NONE;
		
    if (Fls_DrvObj.jobResultType == MEMIF_JOB_PENDING)
    {
		    Fls_DrvObj.jobResultType = MEMIF_JOB_CANCELED;
    }
		if( Fls_DrvObj.Fls_JobErrorNotification != NULL )
			{
				Fls_DrvObj.Fls_JobErrorNotification();
			}
	}

};
#endif


#define FLS_STOP_SEC_CODE
#include "Fls_MemMap.h"
