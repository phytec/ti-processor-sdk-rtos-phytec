/*
 *
 * Copyright (c) 2018 Texas Instruments Incorporated
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

#ifndef APP_HWA_H_
#define APP_HWA_H_

#include <stdint.h>

/**
 *  Remote service name to free/reallocate DMAPC SL2
 */
#define APP_VHWA_SERVICE_NAME  "com.ti.vhwa_remote_service"

/**
 *  Remote service command to free SL2 for SDE
 */
#define APP_DMPAC_SDE_SL2_FREE        (0x0001)
/**
 *  Remote service command to free SL2 for DOF
 */
#define APP_DMPAC_DOF_SL2_FREE        (0x0002)
/**
 *  Remote service command to reallocate SL2 for SDE
 */
#define APP_DMPAC_SDE_SL2_REALLOC     (0x0003)
/**
 *  Remote service command to reallocate SL2 for DOF
 */
#define APP_DMPAC_DOF_SL2_REALLOC     (0x0004)
/**
 *  Remote service command to set VPAC to 720 MHz and DMPAC to 480 MHz
 */
#define APP_VPAC_720_DMPAC_480        (0x0005)
/**
 *  Remote service command to set VPAC to 650 MHz and DMPAC to 520 MHz
 */
#define APP_VPAC_650_DMPAC_520        (0x0006)

/**
 *  Identical to SDE_DEFAULT_DIS_BUFF_DEPTH 
 */
#define APP_SDE_DEFAULT_DIS_BUFF_DEPTH          (2U)
/**
 *  Identical to SDE_SR_192 
 */
#define APP_SDE_SR_192                          (2U)
/**
 *  Identical to FVID2_CCSF_BITS12_UNPACKED16
 */
#define APP_FVID2_CCSF_BITS12_UNPACKED16        ((uint32_t) 0x11U)


/** 
 *  Identical to DOF_DEFAULT_REF_BUFF_DEPTH
 */
#define APP_DOF_DEFAULT_REF_BUFF_DEPTH          (2U)

/** 
 *  Identical to DOF_DEFAULT_CUR_BUFF_DEPTH
 */
#define APP_DOF_DEFAULT_CUR_BUFF_DEPTH          (2U)

/** 
 *  Identical to DOF_DEFAULT_FV_BUFF_DEPTH
 */
#define APP_DOF_DEFAULT_FV_BUFF_DEPTH           (4U)


/**
 *  struct App_m2mSdeSl2AllocPrms
 *  \brief Init Parameters required to allocate SDE driver.
 *         This should be identical to Vhwa_m2mSdeSl2AllocPrms. 
 *         It is defined locally for application to use the same params
 */
typedef struct
{
    /** These parameters are used by the driver to allocater SL2 memory, used
     *  all driver handles.
     *  Ensure to allocate max of all handles requirement.
     */
    uint32_t        maxImgWidth;
    /**< Maximun image width */
    uint32_t        inCcsf;
    /**< CCSF for input image */
    uint32_t        searchRange;
    /**< Search range refer to \ref SDE_DispSearchRange */
    uint32_t        disBuffDepth;
    /**< Maximum disparity buffer depth */
} App_M2mSdeSl2AllocPrms;


/**
 *  struct App_m2mDofSl2AllocPrms
 *  \brief Init Parameters required to allocate DOF driver
 *         This should be identical to Vhwa_m2mDofSl2AllocPrms. 
 *         It is defined locally for application to use the same params
 */
typedef struct
{
    /** These parameters are used by the driver to allocater SL2 memory, used
     *  all driver handles.
     *  Ensure to allocate max of all handles requirement.
     */
    uint32_t        maxImgWidth;
    /**< Maximun image width */
    uint32_t        inCcsf;
    /**< CCSF for input image */
    uint32_t        refBuffDepth;
    /**< Input buffer depth in SL2 for reference image, final depth will be
         calculated based on tob and bottom searc range.
         Minumum configurable depth is 2 */
    uint32_t        currBuffDepth;
    /**< Input buffer depth in SL2 for Current image
         Minumum configurable depth is 2 */
    uint32_t        fvBuffDepth;
    /**< Flow vector buffer depth in Sl2 */
    uint32_t        topSR;
    /**< Top search range */
    uint32_t        botSR;
    /**< Bottom search range */
} App_M2mDofSl2AllocPrms;


/**
 * \defgroup group_vision_apps_utils_hwa HW accelerator initialization APIs (RTOS only)
 *
 * \brief This section contains APIs for HW accelerator initialization 
 *
 * \ingroup group_vision_apps_utils
 *
 * @{
 */

/** \brief Performs initializations needed for Fvid2 must be called before
 *         capture, display and other HWA init
 *
 */
int32_t appFvid2Init(void);

/** \brief Performs initializations needed for CSI2RX driver
 *
 */
int32_t appCsi2RxInit(void);

/** \brief Performs initializations needed for CSI2TX driver
 *
 */
int32_t appCsi2TxInit(void);

/** \brief Performs initializations needed for VHWA drivers
 *
 */
int32_t appVhwaVpacInit(uint32_t vpacInst);
int32_t appVhwaDmpacInit(void);

/** \brief Performs de-initializations needed for CSI2RX driver
 *
 */
int32_t appCsi2RxDeInit(void);

/** \brief Performs de-initializations needed for CSI2TX driver
 *
 */
int32_t appCsi2TxDeInit(void);

/** \brief Performs de-initializations needed for Fvid2
 *
 */
int32_t appFvid2DeInit(void);


/** \brief Performs de-initializations needed for VHWA drivers
 *
 */
int32_t appVhwaVpacDeInit(void);
int32_t appVhwaDmpacDeInit(void);

/** \brief Performs registration of remote service for SL2
 *
 */
int32_t appVhwaRemoteServiceInit();

/** \brief Performs unregistration of remote service for SL2
 *
 */
int32_t appVhwaRemoteServiceDeInit();

/** \brief Performs registration of remote service for VISS node
 *
 */
int32_t appVissRemoteServiceInit();

/** \brief Performs unregistration of remote service for VISS node
 *
 */
int32_t appVissRemoteServiceDeInit();

#if defined(SOC_J722S)

/**
 *  \brief Performs initializations needed for UDMA BCDMA instance
 *         Required for CSIRX/CSITX on J721S2
 *
 *  \return  0 incase of success else returns failure code
 */
int32_t appUdmaCsirxCsitxInit(void);

/**
 *  \brief Performs de-initializations needed for UDMA BCDMA instance
 *         Required for CSIRX/CSITX on J721S2
 *
 *  \return  0 incase of success else returns failure code
 */
int32_t appUdmaCsirxCsitxDeInit(void);

#endif

/* @} */

#endif

