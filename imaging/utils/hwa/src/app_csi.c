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

#include <utils/hwa/include/app_hwa.h>
#include <utils/console_io/include/app_log.h>
#include <utils/sciclient/include/app_sciclient_wrapper_api.h>

#if defined(SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J784S4)
#include <csirx/csirx.h>
#include <csitx/csitx.h>
#endif

#if defined(SOC_J722S)
#include <drivers/hw_include/tistdtypes.h>
#include <drivers/hw_include/csl_types.h>
#include <drivers/udma.h>
#include <drivers/csirx.h>
#include <drivers/csitx.h>

#define CSITX_ESC_CLK_FREQ_HZ         (16000000U)
#define CSITX_MAIN_CLK_HZ             (500000000U)
#endif

#define APP_DEBUG_VHWA

#if !defined(SOC_AM62A)
#if defined(SOC_J721S2) || defined(SOC_J784S4)
static struct Udma_DrvObj gAppUdmaDrvObjCsirxCsitx;
#elif defined(SOC_J722S)
static Udma_DrvObject gAppUdmaDrvObjCsirxCsitx;
#else
static struct Udma_DrvObj gAppUdmaDrvObj;
#endif

#if defined(SOC_J721S2) || defined(SOC_J784S4) || defined(SOC_J722S)

int32_t appUdmaCsirxCsitxInit(void)
{
    int32_t         retVal = 0;
    uint32_t        udmaInstId;
    Udma_InitPrms   udmaInitPrms;
    Udma_DrvHandle drvHandle;

    appLogPrintf("UDMA: Init for CSITX/CSIRX ... !!!\n");

    #if defined(SOC_J722S)
    udmaInstId = UDMA_INST_ID_BCDMA_1;
    #else
    udmaInstId = UDMA_INST_ID_BCDMA_0;
    #endif
    drvHandle = &gAppUdmaDrvObjCsirxCsitx;

    UdmaInitPrms_init(udmaInstId, &udmaInitPrms);
    retVal = Udma_init(drvHandle, &udmaInitPrms);
    if(retVal!=0)
    {
        appLogPrintf("UDMA: ERROR: Udma_init for CSITX/CSIRX failed !!!\n");
    }

    appLogPrintf("UDMA: Init for CSITX/CSIRX ... Done !!!\n");

    return (retVal);
}

int32_t appUdmaCsirxCsitxDeInit(void)
{
    int32_t     retVal = 0;
    Udma_DrvHandle drvHandle;

    drvHandle = &gAppUdmaDrvObjCsirxCsitx;

    retVal = Udma_deinit(drvHandle);
    if(retVal != 0)
    {
        appLogPrintf("UDMA: ERROR: Udma_deinit failed !!!\n");
    }

    return (retVal);
}
#endif /* defined(SOC_J721S2) || defined(SOC_J784S4) || defined(SOC_J722S) */

void *appUdmaCsirxCsitxGetObj(void)
{
    #if !defined(SOC_J721E)
    return (void *)&gAppUdmaDrvObjCsirxCsitx;
    #else
    return (void *)&gAppUdmaDrvObj;
    #endif

}

int32_t appCsi2RxInit(void)
{
    int32_t status = FVID2_SOK;

    Csirx_InitParams initPrmsCsirx;

    appLogPrintf("CSI2RX: Init ... !!!\n");

    #if !defined(SOC_J722S)
    SET_DEVICE_STATE_ON(TISCI_DEV_CSI_PSILSS0);
    #endif
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

#endif /* !defined(SOC_AM62A) */

#if defined(SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J784S4) || defined (SOC_J722S)

int32_t appCsi2TxInit(void)
{
    int32_t status = FVID2_SOK;

    uint32_t regVal = 0U, unlocked = 0U;
    Csitx_InitParams initPrmsCsitx;

    appLogPrintf("CSI2TX: Init ... !!!\n");

    #if !defined(SOC_J722S)
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
    #endif

    #if defined(SOC_J722S)
    uint64_t clkFreq = 0U;
    SET_DEVICE_STATE_ON(TISCI_DEV_CSI_TX_IF0);
    SET_DEVICE_STATE_ON(TISCI_DEV_DPHY_TX0);
    SET_CLOCK_FREQ (TISCI_DEV_CSI_TX_IF0, TISCI_DEV_CSI_TX_IF0_ESC_CLK_CLK, CSITX_ESC_CLK_FREQ_HZ);
    SET_CLOCK_FREQ (TISCI_DEV_CSI_TX_IF0, TISCI_DEV_CSI_TX_IF0_MAIN_CLK_CLK, CSITX_MAIN_CLK_HZ);
    #endif

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
#endif /* defined(SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J784S4) || defined (SOC_J722S) */
