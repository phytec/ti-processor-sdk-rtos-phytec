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

#include <utils/mem/include/app_mem.h>
#include <utils/udma/include/app_udma.h>
#include <utils/hwa/include/app_hwa.h>
#include <utils/console_io/include/app_log.h>
#include <utils/sciclient/include/app_sciclient_wrapper_api.h>
#include <utils/remote_service/include/app_remote_service.h>
#include <utils/ipc/include/app_ipc.h>

#include <vhwa/include/vhwa_m2mLdc.h>
#include <vhwa/include/vhwa_m2mMsc.h>
#include <vhwa/include/vhwa_m2mViss.h>
#include <TI/hwa_viss_srvr_remote.h>

#if !defined(SOC_AM62A) && !defined(SOC_J722S)
#include <vhwa/include/vhwa_m2mNf.h>
#include <csirx/csirx.h>
#include <csitx/csitx.h>
#endif

#if !defined(SOC_AM62A)
#include <vhwa/include/vhwa_m2mSde.h>
#include <vhwa/include/vhwa_m2mDof.h>
#endif

#define APP_DEBUG_VHWA

/* DOF and SDE is enabled for J7 devices alone.*/
#if defined(SOC_AM62A)
#undef ENABLE_DOF
#undef ENABLE_SDE
#else
#define ENABLE_DOF
#define ENABLE_SDE
#endif

/* J722s and AM62A doesn't support NF */
#if defined(VPAC3L)
#undef ENABLE_NF
#else
#define ENABLE_NF
#endif

#define ENABLE_MSC
#define ENABLE_VISS
#define ENABLE_LDC

/* below define's set max limits for line width in order to
 * reserved space in SL2 for VHWA modules during init
 */

#define APP_UTILS_VHWA_MAX_IN_IMG_WIDTH        (1920U)

#define APP_UTILS_VHWA_IN_IMG_CCSF             (FVID2_CCSF_BITS12_UNPACKED16)
#define APP_UTILS_VHWA_MAX_IN_IMG_BUFF_DEPTH   (6)

#define APP_UTILS_VHWA_MAX_OUT_IMG_WIDTH       (1920U)

#define APP_UTILS_VHWA_OUT_IMG_CCSF            (FVID2_CCSF_BITS12_UNPACKED16)
#define APP_UTILS_VHWA_MAX_OUT_IMG_BUFF_DEPTH  (2)

#if defined(VPAC3)
#define APP_UTILS_VHWA_LDC_MAX_BLOCK_WIDTH     (128)
#define APP_UTILS_VHWA_LDC_MAX_BLOCK_HEIGHT    (64)
#elif defined(VPAC3L)
#define APP_UTILS_VHWA_LDC_MAX_BLOCK_WIDTH     (64)
#define APP_UTILS_VHWA_LDC_MAX_BLOCK_HEIGHT    (64)
#elif defined(VPAC1)
#define APP_UTILS_VHWA_LDC_MAX_BLOCK_WIDTH     (192)
#define APP_UTILS_VHWA_LDC_MAX_BLOCK_HEIGHT    (80)
#endif

static void appVhwaVpacMscInit(Vhwa_M2mMscSl2AllocPrms *sl2Prms)
{
    uint32_t cnt;
    uint32_t idx;

    for(cnt = 0; cnt < VHWA_M2M_MSC_MAX_INST; cnt++)
    {
        for(idx = 0; idx < VHWA_M2M_MSC_MAX_IN_CHANNEL; idx++)
        {
            sl2Prms->maxInWidth[cnt][idx]    = APP_UTILS_VHWA_MAX_IN_IMG_WIDTH;
            sl2Prms->inCcsf[cnt][idx]        = APP_UTILS_VHWA_IN_IMG_CCSF;
            sl2Prms->inBuffDepth[cnt][idx]   = APP_UTILS_VHWA_MAX_IN_IMG_BUFF_DEPTH;
        }
    }

    for(cnt = 0; cnt < MSC_MAX_OUTPUT; cnt++)
    {
        sl2Prms->maxOutWidth[cnt]   = APP_UTILS_VHWA_MAX_OUT_IMG_WIDTH;
        sl2Prms->outCcsf[cnt]       = APP_UTILS_VHWA_OUT_IMG_CCSF;
        sl2Prms->outBuffDepth[cnt]  = APP_UTILS_VHWA_MAX_OUT_IMG_BUFF_DEPTH;
    }
}

int32_t appFvid2Init(void)
{
    int32_t retVal = FVID2_SOK;
    Fvid2_InitPrms initPrmsFvid2;

    appLogPrintf("FVID2: Init ... !!!\n");

    Fvid2InitPrms_init(&initPrmsFvid2);
    initPrmsFvid2.printFxn = appLogPrintf;
    retVal = Fvid2_init(&initPrmsFvid2);
    if(retVal!=FVID2_SOK)
    {
        appLogPrintf("FVID2: ERROR: Fvid2_init failed !!!\n");
    }
    appLogPrintf("FVID2: Init ... Done !!!\n");

    return (retVal);
}

int32_t appFvid2DeInit(void)
{
    int32_t retVal = FVID2_SOK;

    retVal = Fvid2_deInit(NULL);
    if(retVal!=FVID2_SOK)
    {
        appLogPrintf("FVID2: ERROR: Fvid2_deInit failed !!!\n");
    }
    return (retVal);
}

#if !defined(SOC_AM62A) && !defined(SOC_J722S)

int32_t appCsi2RxInit(void)
{
    int32_t status = FVID2_SOK;

    Csirx_InitParams initPrmsCsirx;

    appLogPrintf("CSI2RX: Init ... !!!\n");

    SET_DEVICE_STATE_ON(TISCI_DEV_CSI_PSILSS0);
    SET_DEVICE_STATE_ON(TISCI_DEV_CSI_RX_IF0);
    SET_DEVICE_STATE_ON(TISCI_DEV_CSI_RX_IF1);
    #if defined(SOC_J784S4)
    SET_DEVICE_STATE_ON(TISCI_DEV_CSI_RX_IF2);
    #endif
    SET_DEVICE_STATE_ON(TISCI_DEV_DPHY_RX0);
    SET_DEVICE_STATE_ON(TISCI_DEV_DPHY_RX1);
    #if defined(SOC_J784S4)
    SET_DEVICE_STATE_ON(TISCI_DEV_DPHY_RX2);
    #endif

    Csirx_initParamsInit(&initPrmsCsirx);
    initPrmsCsirx.drvHandle = appUdmaCsirxCsitxGetObj();
    status = Csirx_init(&initPrmsCsirx);
    if(status!=FVID2_SOK)
    {
        appLogPrintf("CSI2RX: ERROR: Csirx_init failed !!!\n");
    }
    appLogPrintf("CSI2RX: Init ... Done !!!\n");

    return (status);
}

int32_t appCsi2TxInit(void)
{
    int32_t status = FVID2_SOK;

    uint32_t regVal = 0U, unlocked = 0U;
    Csitx_InitParams initPrmsCsitx;

    appLogPrintf("CSI2TX: Init ... !!!\n");

    SET_DEVICE_STATE_ON(TISCI_DEV_CSI_PSILSS0);
    #if defined(SOC_J721S2)
    SET_DEVICE_STATE_ON(TISCI_DEV_CSI_TX_IF_V2_0);
    SET_DEVICE_STATE_ON(TISCI_DEV_CSI_TX_IF_V2_1);
    #elif defined(SOC_J784S4)
    SET_DEVICE_STATE_ON(TISCI_DEV_CSI_TX_IF0);
    SET_DEVICE_STATE_ON(TISCI_DEV_CSI_TX_IF1);
    #else
    SET_DEVICE_STATE_ON(TISCI_DEV_CSI_TX_IF0);
    #endif
    SET_DEVICE_STATE_ON(TISCI_DEV_DPHY_TX0);
    #if defined(SOC_J721S2) || defined(SOC_J784S4)
    SET_DEVICE_STATE_ON(TISCI_DEV_DPHY_TX1);
    #endif

    regVal = CSL_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE +
                          CSL_MAIN_CTRL_MMR_CFG0_LOCK1_KICK0);
    if ((regVal & 0x1) == 0U)
    {
        /* Unlock MMR */
        unlocked = 1U;
        CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE +
                     CSL_MAIN_CTRL_MMR_CFG0_LOCK1_KICK0,
                     0x68EF3490U);
        CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE +
                     CSL_MAIN_CTRL_MMR_CFG0_LOCK1_KICK1,
                     0xD172BC5AU);
        appLogPrintf("Unlocked MMR to program CSITX DPHY register ... !!!\n");
    }

    /* Select CSITX0 as the source for DPHYTX0 */
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE +
                    CSL_MAIN_CTRL_MMR_CFG0_DPHY_TX0_CTRL,
                    0x1);
    #if defined(SOC_J721S2) || defined(SOC_J784S4)
    /* Select CSITX1 as the source for DPHYTX1 */
    CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE +
                    CSL_MAIN_CTRL_MMR_CFG0_DPHY_TX1_CTRL,
                    0x1);
    #endif
    /* Lock MMR back if unlocked here */
    if (unlocked == 1U)
    {
        CSL_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE +
                     CSL_MAIN_CTRL_MMR_CFG0_LOCK1_KICK0,
                     0U);
        appLogPrintf("Locked MMR after programming CSITX DPHY register ... !!!\n");
    }

    Csitx_initParamsInit(&initPrmsCsitx);
    initPrmsCsitx.drvHandle = appUdmaCsirxCsitxGetObj();
    status = Csitx_init(&initPrmsCsitx);
    if(status!=FVID2_SOK)
    {
        appLogPrintf("CSI2TX: ERROR: Csitx_init failed !!!\n");
    }
    appLogPrintf("CSI2TX: Init ... Done !!!\n");

    return (status);
}

int32_t appCsi2RxDeInit(void)
{
    int32_t retVal = FVID2_SOK;
    retVal = Csirx_deInit();
    if(retVal!=FVID2_SOK)
    {
        appLogPrintf("CSI2RX: ERROR: Csirx_deInit failed !!!\n");
    }
    return (retVal);
}

int32_t appCsi2TxDeInit(void)
{
    int32_t retVal = FVID2_SOK;
    retVal = Csitx_deInit();
    if(retVal!=FVID2_SOK)
    {
        appLogPrintf("CSI2TX: ERROR: Csitx_deInit failed !!!\n");
    }
    return (retVal);
}
#endif /* !defined(SOC_AM62A) && !defined(SOC_J722S) */

#if !defined(SOC_AM62A)

int32_t appVhwaDmpacInit()
{
    int32_t  status = FVID2_SOK;

    appLogPrintf("VHWA: DMPAC: Init ... !!!\n");

    #if defined(ENABLE_DOF) || defined(ENABLE_SDE)
    SET_DEVICE_STATE_ON(TISCI_DEV_DMPAC0);
    #endif
    #if !defined(SOC_J722S)/* SDE requires a separate power domain on j721s2, j721e, j784s4 */
    #if defined(ENABLE_SDE)
    SET_DEVICE_STATE_ON(TISCI_DEV_DMPAC0_SDE_0);
    #endif
    #endif

#if defined(ENABLE_DOF)
    Vhwa_M2mDofSl2AllocPrms sl2AllocPrms;
    Vhwa_M2mDofInitParams   initPrms;

    #if defined(APP_DEBUG_VHWA)
    appLogPrintf("VHWA: DOF Init ... !!!\n");
    #endif

    /* Initialize DOF Init parameters */
    Vhwa_m2mDofInitParamsInit(&initPrms);

    /* Set UDMA driver handle */
    initPrms.udmaDrvHndl = appUdmaGetObj();

    status = Vhwa_m2mDofInit(&initPrms);
    if (0 != status)
    {
        appLogPrintf("VHWA: ERROR: DOF Init Failed !!!\n");
    }
    else
    {
        /* Initilize SL2 parameters */
        Vhwa_M2mDofSl2AllocPrmsInit(&sl2AllocPrms);

        status = Vhwa_m2mDofAllocSl2(&sl2AllocPrms);
        if (0 != status)
        {
            appLogPrintf("VHWA: ERROR: DOF SL2 Alloc Failed !!!\n");
        }
        else
        {
            #if defined(APP_DEBUG_VHWA)
            appLogPrintf("VHWA: DOF Init ... Done !!!\n");
            #endif
        }
    }
#endif

#if defined(ENABLE_SDE)
    /* SDE */
    if (0 == status)
    {
        Vhwa_M2mSdeSl2AllocPrms sl2AllocPrms;
        Vhwa_M2mSdeInitParams   initPrms;

        #if defined(APP_DEBUG_VHWA)
        appLogPrintf("VHWA: SDE Init ... !!!\n");
        #endif

        /* Initialize SDE Init parameters */
        Vhwa_M2mSdeInitParamsInit(&initPrms);

        /* Set UDMA driver handle */
        initPrms.udmaDrvHndl = appUdmaGetObj();

        status = Vhwa_m2mSdeInit(&initPrms);
        if (0 != status)
        {
            appLogPrintf("VHWA: ERROR: SDE Init Failed !!!\n");
        }
        else
        {
            /* Initilize SL2 parameters */
            Vhwa_M2mSdeSl2AllocPrmsInit(&sl2AllocPrms);

            status = Vhwa_m2mSdeAllocSl2(&sl2AllocPrms);
            if (0 != status)
            {
                appLogPrintf("VHWA: ERROR: SDE SL2 Alloc Failed !!!\n");
            }
            else
            {
                #if defined(APP_DEBUG_VHWA)
                appLogPrintf("VHWA: SDE Init ... Done !!!\n");
                #endif
            }
        }
    }
#endif

    appLogPrintf("VHWA: DMPAC: Init ... Done !!!\n");
    return (status);

}

int32_t appVhwaDmpacDeInit()
{
#if defined(ENABLE_DOF)
    Vhwa_m2mDofDeInit();
#endif
#if defined(ENABLE_SDE)
    Vhwa_m2mSdeDeInit();
#endif

    return (0);
}

#endif /* !defined(SOC_AM62A) */

int32_t appVhwaVpacInit(uint32_t vpacInst)
{
    int32_t  status = FVID2_SOK;

    appLogPrintf("VHWA: VPAC Init ... !!!\n");

    #if defined(ENABLE_LDC) || defined(ENABLE_MSC) || defined(ENABLE_NF) || defined(ENABLE_VISS)
    if (0u==vpacInst)
    {
        SET_DEVICE_STATE_ON(TISCI_DEV_VPAC0);
    }
    #if defined(SOC_J784S4)
    else if (1u==vpacInst)
    {
        SET_DEVICE_STATE_ON(TISCI_DEV_VPAC1);
    }
    #endif
    else
    {
        appLogPrintf("VHWA: ERROR: Unsupported VPAC instance!!!\n");
        status = FVID2_EFAIL;
    }
    #endif

#if defined(ENABLE_LDC)
    /* LDC */
    if (0 == status)
    {
        Vhwa_M2mLdcSl2AllocPrms sl2AllocPrms;
        Vhwa_M2mLdcInitParams   initPrms;

        #if defined(APP_DEBUG_VHWA)
        appLogPrintf("VHWA: LDC Init ... !!!\n");
        #endif

        /* Initialize LDC Init parameters */
        Vhwa_m2mLdcInitParamsInit(&initPrms);

        /* Set UDMA driver handle */
        #if defined(SOC_AM62A) || defined(SOC_J722S)
        initPrms.udmaDrvHndl = NULL;
        #else
        initPrms.udmaDrvHndl = appUdmaGetObj();
        #endif

        status = Vhwa_m2mLdcInit(&initPrms);
        if (0 != status)
        {
            appLogPrintf("VHWA: ERROR: LDC Init Failed !!!\n");
        }
        else
        {
            Vhwa_M2mLdcSl2AllocPrmsInit(&sl2AllocPrms);

            sl2AllocPrms.maxBlockWidth  = APP_UTILS_VHWA_LDC_MAX_BLOCK_WIDTH;
            sl2AllocPrms.maxBlockHeight = APP_UTILS_VHWA_LDC_MAX_BLOCK_HEIGHT;

            status = Vhwa_m2mLdcAllocSl2(&sl2AllocPrms);
            if (0 != status)
            {
                appLogPrintf("VHWA: ERROR: LDC SL2 Alloc Failed !!!\n");
            }
            else
            {
                #if defined(APP_DEBUG_VHWA)
                appLogPrintf("VHWA: LDC Init ... Done !!!\n");
                #endif
            }
        }
    }
#endif

#if defined(ENABLE_MSC)
    /* MSC */
    if (0 == status)
    {
        Vhwa_M2mMscInitParams initPrms;
        Vhwa_M2mMscSl2AllocPrms sl2Prms;

        #if defined(APP_DEBUG_VHWA)
        appLogPrintf("VHWA: MSC Init ... !!!\n");
        #endif

        Vhwa_m2mMscInitParamsInit(&initPrms);

        #if defined(SOC_AM62A) || defined(SOC_J722S)
        initPrms.drvHandle = NULL;
        #else
        initPrms.drvHandle = appUdmaGetObj();
        #endif

        status = Vhwa_m2mMscInit(&initPrms);

        if (0 == status)
        {
            appVhwaVpacMscInit(&sl2Prms);

            status = Vhwa_m2mMscAllocSl2(&sl2Prms);
            if (0 != status)
            {
                appLogPrintf("VHWA: ERROR: MSC SL2 Alloc Failed !!!\n");
            }
            else
            {
                #if defined(APP_DEBUG_VHWA)
                appLogPrintf("VHWA: MSC Init ... Done !!!\n");
                #endif
            }
        }
        else
        {
            appLogPrintf("VHWA: ERROR: MSC Init Failed !!!\n");
        }
    }
#endif

#if defined(ENABLE_NF)
    /* NF */
    if (0 == status)
    {
        Vhwa_M2mNfSl2AllocPrms  sl2AllocPrms;
        Vhwa_M2mNfInitPrms      initPrms;

        #if defined(APP_DEBUG_VHWA)
        appLogPrintf("VHWA: NF Init ... !!!\n");
        #endif

        /* Initialize NF Init parameters */
        Vhwa_m2mNfInitPrmsInit(&initPrms);

        /* Set UDMA driver handle */
        initPrms.udmaDrvHndl = appUdmaGetObj();

        status = Vhwa_m2mNfInit(&initPrms);
        if (0 != status)
        {
            appLogPrintf("VHWA: ERROR: NF Init Failed !!!\n");
        }
        else
        {
            /* Initilize SL2 parameters */
            sl2AllocPrms.maxImgWidth  = APP_UTILS_VHWA_MAX_IN_IMG_WIDTH;
            sl2AllocPrms.inCcsf       = APP_UTILS_VHWA_IN_IMG_CCSF;
            sl2AllocPrms.inBuffDepth  = APP_UTILS_VHWA_MAX_IN_IMG_BUFF_DEPTH;
            sl2AllocPrms.outBuffDepth = APP_UTILS_VHWA_MAX_OUT_IMG_BUFF_DEPTH;
            sl2AllocPrms.outCcsf      = APP_UTILS_VHWA_OUT_IMG_CCSF;

            status = Vhwa_m2mNfAllocSl2(&sl2AllocPrms);
            if (0 != status)
            {
                appLogPrintf("VHWA: ERROR: NF SL2 Alloc Failed !!!\n");
            }
            else
            {
                #if defined(APP_DEBUG_VHWA)
                appLogPrintf("VHWA: NF Init ... Done !!!\n");
                #endif
            }
        }
    }
#endif

#if defined(ENABLE_VISS)
    /* VISS */
    if (0 == status)
    {
        Vhwa_M2mVissSl2Params   sl2AllocPrms;
        Vhwa_M2mVissInitParams  initPrms;

        #if defined(APP_DEBUG_VHWA)
        appLogPrintf("VHWA: VISS Init ... !!!\n");
        #endif

        /* Initialize VISS Init parameters */
        Vhwa_m2mVissInitParamsInit(&initPrms);

        /* Set UDMA driver handle */
        #if defined(VPAC3L)
        initPrms.udmaDrvHndl = NULL;
        #else
        initPrms.udmaDrvHndl = appUdmaGetObj();
        #endif

        /* Set configThroughUDMA to true to support multi handle */
        initPrms.configThroughUdmaFlag = true;

        status = Vhwa_m2mVissInit(&initPrms);
        if (0 != status)
        {
            appLogPrintf("VHWA: ERROR: VISS Init Failed !!!\n");
        }
        else
        {
            int32_t cnt;

            Vhwa_m2mVissSl2ParamsInit(&sl2AllocPrms);

            for (cnt = 0U; cnt < VHWA_M2M_VISS_MAX_INPUTS; cnt ++)
            {
                sl2AllocPrms.maxInWidth[cnt] = APP_UTILS_VHWA_MAX_IN_IMG_WIDTH;
            }

            for (cnt = 0U; cnt < VHWA_M2M_VISS_MAX_OUTPUTS; cnt ++)
            {
                if (VHWA_M2M_VISS_OUT_H3A_IDX == cnt)
                {
                    /* This is the default size for H3A output */
                    sl2AllocPrms.maxOutWidth[cnt] = 1024U;
                }
                else
                {
                    sl2AllocPrms.maxOutWidth[cnt] = APP_UTILS_VHWA_MAX_OUT_IMG_WIDTH;
                }
            }

            sl2AllocPrms.inDepth = 3U; /* Minimum 3 */

            for (cnt = 0U; cnt < VHWA_M2M_VISS_MAX_OUTPUTS; cnt ++)
            {
                sl2AllocPrms.outDepth[cnt] = 2U; /* Minimum 2 */
            }

            status = Vhwa_m2mVissAllocSl2(&sl2AllocPrms);
            if (0 != status)
            {
                appLogPrintf("VHWA: ERROR: VISS SL2 Alloc Failed !!!\n");
            }
            else
            {
                #if defined(APP_DEBUG_VHWA)
                appLogPrintf("VHWA: VISS Init ... Done !!!\n");
                #endif
            }
        }
    }
#endif

    appLogPrintf("VHWA: VPAC Init ... Done !!!\n");
    return (status);
}

int32_t appVhwaVpacDeInit()
{
#if defined(ENABLE_LDC)
    Vhwa_m2mLdcDeInit();
#endif
#if defined(ENABLE_MSC)
    Vhwa_m2mMscDeInit();
#endif
#if defined(ENABLE_NF)
    Vhwa_m2mNfDeInit();
#endif
#if defined(ENABLE_VISS)
    Vhwa_m2mVissDeInit();
#endif

    return (0);
}

int32_t appVhwaHandler(char *service_name, uint32_t cmd, void *prm, uint32_t prm_size, uint32_t flags)
{
    int32_t  status = -1;

    if (NULL != prm)
    {
        appLogPrintf(
            " VHWA Remote Service: Received command %08x to configure VHWA !!!\n",
            cmd);

        switch(cmd)
        {
            #if defined(ENABLE_SDE)
            case APP_DMPAC_SDE_SL2_FREE:
                /* free SDE SL2 memory */
                status = Vhwa_m2mSdeFreeSl2();
                break;

            case APP_DMPAC_SDE_SL2_REALLOC:
                /* realloc SDE SL2 for 2MP */
                if (sizeof(Vhwa_M2mSdeSl2AllocPrms) == prm_size)
                {
                    Vhwa_M2mSdeSl2AllocPrms *cmdPrms = (Vhwa_M2mSdeSl2AllocPrms *)prm;
                    status = Vhwa_m2mSdeAllocSl2(cmdPrms);
                } else
                {
                    appLogPrintf(" VHWA Remote Service: ERROR: Invalid SDE SL2 parameters passed !!!\n");
                }

                break;
            #endif

            #if defined(ENABLE_DOF)
            case APP_DMPAC_DOF_SL2_FREE:
                /* free DOF SL2 memory */
                status = Vhwa_m2mDofFreeSl2();
                break;

            case APP_DMPAC_DOF_SL2_REALLOC:
                /* realloc DOF SL2 for 2MP */
                if (sizeof(Vhwa_M2mDofSl2AllocPrms) == prm_size)
                {
                    Vhwa_M2mDofSl2AllocPrms *cmdPrms = (Vhwa_M2mDofSl2AllocPrms *)prm;
                    status = Vhwa_m2mDofAllocSl2(cmdPrms);
                } else
                {
                    appLogPrintf(" VHWA Remote Service: ERROR: Invalid DOF SL2 parameters passed !!!\n");
                }
                break;
            #endif

#if defined(SOC_J721E) || defined(SOC_J721S2) || defined(SOC_J784S4)
            case APP_VPAC_720_DMPAC_480:
                SET_CLOCK_FREQ (TISCI_DEV_DMPAC0, TISCI_DEV_DMPAC0_CLK, 480000000);
                #if defined(SOC_J721S2) || defined(SOC_J784S4)
                SET_CLOCK_FREQ (TISCI_DEV_VPAC0, TISCI_DEV_VPAC0_MAIN_CLK,   720000000);
                #else
                SET_CLOCK_FREQ (TISCI_DEV_VPAC0, TISCI_DEV_VPAC0_CLK,   720000000);
                #endif
                status = 0;
                break;

            case APP_VPAC_650_DMPAC_520:
                SET_CLOCK_FREQ (TISCI_DEV_DMPAC0, TISCI_DEV_DMPAC0_CLK, 520000000);
                #if defined(SOC_J721S2) || defined(SOC_J784S4)
                SET_CLOCK_FREQ (TISCI_DEV_VPAC0, TISCI_DEV_VPAC0_MAIN_CLK,   650000000);
                #else
                SET_CLOCK_FREQ (TISCI_DEV_VPAC0, TISCI_DEV_VPAC0_CLK,   650000000);
                #endif
                status = 0;
                break;
#endif
        }

        if (0 == status)
        {
            appLogPrintf(" VHWA Remote Service: VHWA configuration done !!!\n");
        }
        else
        {
            appLogPrintf(" VHWA Remote Service: ERROR: VHWA configuration failed !!!\n");
        }
    }
    else
    {
        appLogPrintf(" VHWA Remote Service: ERROR: Invalid parameters passed !!!\n");
    }


    return status;
}

int32_t appVhwaRemoteServiceInit()
{
    int32_t status;

    status = appRemoteServiceRegister(APP_VHWA_SERVICE_NAME, appVhwaHandler);
    if(status!=0)
    {
        appLogPrintf("VHWA Remote Service: ERROR: Unable to register service \n");
    }

    return status;
}

int32_t appVhwaRemoteServiceDeInit()
{
    int32_t status;

    status = appRemoteServiceUnRegister(APP_VHWA_SERVICE_NAME);
    if(status!=0)
    {
        appLogPrintf("VHWA Remote Service: ERROR: Unable to unregister service \n");
    }

    return status;
}

int32_t appVissRemoteServiceDeInit()
{
    int32_t viss_status;

    viss_status = VissRemoteServer_DeInit();
    if(viss_status!=0)
    {
        appLogPrintf(" appVissRemoteServiceDeInit: ERROR: Failed to deinitialize VISS remote server \n");
    }

    return (viss_status);
}

int32_t appVissRemoteServiceInit()
{
    int32_t viss_status;

    appLogPrintf("VISS REMOTE SERVICE: Init ... !!!\n");

    viss_status = VissRemoteServer_Init();
    if(viss_status!=0)
    {
        appLogPrintf("ISS: Error: Failed to create remote VISS remote server failed. Live tuning will not work !!!\n");
        return -1;
    }
    else
    {
        appLogPrintf("VISS REMOTE SERVICE: Init ... Done !!!\n");
    }

    return viss_status;
}

