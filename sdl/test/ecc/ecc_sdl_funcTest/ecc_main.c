/*
 *   Copyright (c) Texas Instruments Incorporated 2021
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

 /**
 *  \file     ecc_main.c
 *
 * \brief This file demonstrates using the Error Correcting Code Module (ECC),
 *         utilizing the ECC and ESM Software Diagnostic Reference (SDL) functions.
 *
 *  \details  ESM Safety Example module tests
 **/

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/drv/sciclient/sciclient.h>
#include <ti/osal/osal.h>
#include <ti/osal/TimerP.h>
#include <sdl_ecc.h>
#include <osal_interface.h>
#include <soc.h>

#include <ti/csl/csl_serdes.h>
#include <ti/csl/cslr_device.h>
#if !defined(SOC_J721S2) && !defined(SOC_J784S4)
#include <ti/csl/csl_serdes_pcie.h>
#include <ti/csl/cslr_pcie.h>
#endif

#if defined (SOC_J721E)
#include "test/ecc/ecc_sdl_funcTest/soc/j721e/ecc_func.h"
#include <ti/csl/soc/j721e/src/cslr_soc_ctrl_mmr.h>
#include <ti/drv/sciclient/soc/sysfw/include/j721e/tisci_devices.h>
#endif /* SOC_J721E */

#if defined (SOC_J7200)
#include "test/ecc/ecc_sdl_funcTest/soc/j7200/ecc_func.h"
#include <ti/csl/soc/j7200/src/cslr_soc_ctrl_mmr.h>
#include <ti/drv/sciclient/soc/sysfw/include/j7200/tisci_devices.h>
#endif /* SOC_J7200 */


#if defined (SOC_J721S2)
#include "test/ecc/ecc_sdl_funcTest/soc/j721s2/ecc_func.h"
#include <ti/drv/sciclient/soc/sysfw/include/j721s2/tisci_devices.h>
#endif /* SOC_J721S2 */

#if defined (SOC_J784S4)
#include "test/ecc/ecc_sdl_funcTest/soc/j784s4/ecc_func.h"
#include <ti/drv/sciclient/soc/sysfw/include/j784s4/tisci_devices.h>
#endif /* SOC_J784S4 */

#ifdef UNITY_INCLUDE_CONFIG_H
#include <ti/build/unit-test/Unity/src/unity.h>
#include <ti/build/unit-test/config/unity_config.h>
#endif

#include <ti/csl/csl_cbass.h>

/* ========================================================================== */
/*                                Macros                                      */
/* ========================================================================== */



/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
volatile bool esmError = false;
/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/* ========================================================================== */
/*                 Internal Function Definitions                              */
/* ========================================================================== */

#ifdef UNITY_INCLUDE_CONFIG_H
/*
 *  ======== Unity set up and tear down ========
 */
void setUp(void)
{
    /* Do nothing */
}

void tearDown(void)
{
    /* Do nothing */
}
#endif


int32_t SDL_ESM_applicationCallbackFunction(SDL_ESM_Inst esmInst,
                                            SDL_ESM_IntType esmIntrType,
                                            uint32_t grpChannel,
                                            uint32_t index,
                                            uint32_t intSrc,
                                            uintptr_t *arg)
{

    SDL_ECC_MemType eccmemtype;
    SDL_Ecc_AggrIntrSrc eccIntrSrc;
    SDL_ECC_ErrorInfo_t eccErrorInfo;
    int32_t retVal;


    UART_printf("\n  ESM Call back function called : instType 0x%x, intType 0x%x, " \
                "grpChannel 0x%x, index 0x%x, intSrc 0x%x \n",
                esmInst, esmIntrType, grpChannel, index, intSrc);
    UART_printf("  Take action \n");
    if(esmIntrType == 1u){
        UART_printf(" High Priority Interrupt Executed\n");
    }
    else{
        UART_printf(" Low Priority Interrupt Executed\n");
    }
    retVal = SDL_ECC_getESMErrorInfo(esmInst, intSrc, &eccmemtype, &eccIntrSrc);
    if (retVal != SDL_PASS)
    {
        UART_printf("SDL_ECC_getESMErrorInfo failed\n");
    }
    /* Any additional customer specific actions can be added here */
    retVal = SDL_ECC_getErrorInfo(eccmemtype, eccIntrSrc, &eccErrorInfo);

    if (retVal != SDL_PASS)
    {
        UART_printf("SDL_ECC_getErrorInfo failed\n");
    }
    UART_printf("\n  ECC Error Call back function called : eccMemType %d, errorSrc 0x%x, " \
               "ramId %d, bitErrorOffset 0x%04x%04x, bitErrorGroup %d\n",
               eccmemtype, eccIntrSrc, eccErrorInfo.memSubType, (uint32_t)(eccErrorInfo.bitErrorOffset >> 32),
               (uint32_t)(eccErrorInfo.bitErrorOffset & 0x00000000FFFFFFFF), eccErrorInfo.bitErrorGroup);

    if (eccErrorInfo.injectBitErrCnt != 0)
    {
        SDL_ECC_clearNIntrPending(eccmemtype, eccErrorInfo.memSubType, eccIntrSrc, SDL_ECC_AGGR_ERROR_SUBTYPE_INJECT, eccErrorInfo.injectBitErrCnt);
    }
    else
    {
        SDL_ECC_clearNIntrPending(eccmemtype, eccErrorInfo.memSubType, eccIntrSrc, SDL_ECC_AGGR_ERROR_SUBTYPE_NORMAL, eccErrorInfo.bitErrCnt);
    }

    retVal = SDL_ECC_ackIntr(eccmemtype, eccIntrSrc);

    esmError = true;

    return retVal;
}

/* SDL_ECC_applicationCallbackFunction is expected to be defined by the application. It is
 * required by the SDL ECC module. It is called by the SDL ECC module to notify the
 * application of certain ECC errors that are reported as Exception events.
 * Note, however, that it is not executed in this example */
void SDL_ECC_applicationCallbackFunction(SDL_ECC_MemType eccMemType,
                                         uint32_t errorSrc,
                                         uint32_t address,
                                         uint32_t ramId,
                                         uint64_t bitErrorOffset,
                                         uint32_t bitErrorGroup){

    UART_printf("\n  ECC Error Call back function called : eccMemType %d, errorSrc 0x%x, " \
                "address 0x%x, ramId %d, bitErrorOffset 0x%04x%04x, bitErrorGroup %d\n",
                eccMemType, errorSrc, address, ramId, (uint32_t)(bitErrorOffset >> 32),
                (uint32_t)(bitErrorOffset & 0x00000000FFFFFFFF), bitErrorGroup);
    UART_printf("  Take action \n");

    /* Any additional customer specific actions can be added here */

}

static
int32_t ECC_appInitBoard(void)
{
    Board_initCfg boardCfg;
    Board_STATUS  boardStatus;
    int32_t       testResult = SDL_PASS;
    uint64_t mcuClkFreq;

    boardCfg = BOARD_INIT_PINMUX_CONFIG |
               BOARD_INIT_UART_STDIO;
    boardStatus = Board_init(boardCfg);
    if (boardStatus != BOARD_SOK)
    {
        testResult = SDL_EFAIL;
        UART_printf("[Error] Board init failed!!\n");
    }

    /* Following code is needed to set Osal timing */
#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
    /* Get the clock frequency */
    testResult = Sciclient_pmGetModuleClkFreq(TISCI_DEV_MCU_R5FSS0_CORE0,
                                              TISCI_DEV_MCU_R5FSS0_CORE0_CPU_CLK,
                                              &mcuClkFreq,
                                              SCICLIENT_SERVICE_WAIT_FOREVER);
#else
#error "SOC NOT supported please check"
#endif
    if (testResult == 0)
    {
        Osal_HwAttrs  hwAttrs;
        uint32_t      ctrlBitmap;

        testResult = Osal_getHwAttrs(&hwAttrs);
        if (testResult == 0)
        {
            /*
             * Change the timer input clock frequency configuration
               based on R5 CPU clock configured
             */
            hwAttrs.cpuFreqKHz = (int32_t)(mcuClkFreq/1000U);
            ctrlBitmap         = OSAL_HWATTR_SET_CPU_FREQ;
            testResult = Osal_setHwAttrs(ctrlBitmap, &hwAttrs);
        }
    }
    UART_printf("mcuClkFreq %d\n", (uint32_t)mcuClkFreq);

    return (testResult);
}
#if defined (SOC_J721E)
#define AUX_NUM_DEVICES 50
#endif

#if defined (SOC_J721S2)
#define AUX_NUM_DEVICES 54
#endif

#if defined (SOC_J7200)
#define AUX_NUM_DEVICES 31
#endif

#if defined(SOC_J784S4)
#define AUX_NUM_DEVICES 82
#endif

uint32_t aux_devices[AUX_NUM_DEVICES] =
{
#if defined (SOC_J721E) || defined (SOC_J721S2)
    TISCI_DEV_DMPAC0,
    TISCI_DEV_DMPAC0_SDE_0,
    TISCI_DEV_VPAC0,
    TISCI_DEV_R5FSS1_CORE0,
    TISCI_DEV_R5FSS1_CORE1,
    TISCI_DEV_DSS_DSI0,
    TISCI_DEV_DSS0,
    TISCI_DEV_DSS_EDP0,
    TISCI_DEV_CSI_PSILSS0,
    TISCI_DEV_CSI_RX_IF0,
    TISCI_DEV_CSI_RX_IF1,
    TISCI_DEV_A72SS0_CORE0,
    TISCI_DEV_A72SS0_CORE1,
    TISCI_DEV_DPHY_RX0,
    TISCI_DEV_DPHY_RX1,
    TISCI_DEV_DPHY_TX0,
    TISCI_DEV_SERDES_10G0,
    TISCI_DEV_A72SS0,
    TISCI_DEV_MCU_R5FSS0_CORE1,
#endif

#if defined (SOC_J721E)
    TISCI_DEV_MLB0,
    TISCI_DEV_CSI_TX_IF0,
    TISCI_DEV_C71SS0,
    TISCI_DEV_SERDES_16G0,
    TISCI_DEV_SERDES_16G1,
    TISCI_DEV_SERDES_16G2,
    TISCI_DEV_SERDES_16G3,
    TISCI_DEV_PCIE0,
    TISCI_DEV_PCIE2,
    TISCI_DEV_PCIE3,
    TISCI_DEV_UFS0,
    TISCI_DEV_I3C0,
#elif defined (SOC_J721S2)
    TISCI_DEV_COMPUTE_CLUSTER0_C71SS0_0,
    TISCI_DEV_COMPUTE_CLUSTER0_C71SS1_0,
    TISCI_DEV_SA2_UL0,
    TISCI_DEV_SA2_CPSW_PSILSS0,
#endif

#if defined (SOC_J7200)
    TISCI_DEV_A72SS0_CORE0,
    TISCI_DEV_MCU_R5FSS0_CORE1,
    TISCI_DEV_A72SS0_CORE0_0,
    TISCI_DEV_A72SS0_CORE0_1,
    TISCI_DEV_MCAN14,
    TISCI_DEV_MCAN15,
    TISCI_DEV_MCAN16,
    TISCI_DEV_MCAN17,
	TISCI_DEV_MCU_CPSW0,
	TISCI_DEV_CPSW0,
#endif

#if defined (SOC_J784S4)
	TISCI_DEV_MCAN14,
	TISCI_DEV_MCAN15,
	TISCI_DEV_MCAN16,
	TISCI_DEV_MCAN17,
	TISCI_DEV_PCIE0,
	TISCI_DEV_PCIE2,
	TISCI_DEV_PCIE3,
	TISCI_DEV_MCU_CPSW0,
	TISCI_DEV_DSS_DSI0,
	TISCI_DEV_DSS_DSI1,
	TISCI_DEV_USB0,
	TISCI_DEV_VPAC1,
	TISCI_DEV_VPAC0,
	TISCI_DEV_UFS0,
	TISCI_DEV_MCU_MCAN0,
	TISCI_DEV_MCU_MCAN1,
	TISCI_DEV_MMCSD0,
	TISCI_DEV_MMCSD1,
	TISCI_DEV_CSI_RX_IF0,
	TISCI_DEV_CSI_RX_IF1,
	TISCI_DEV_CSI_RX_IF2,
	TISCI_DEV_MCU_NAVSS0,
	TISCI_DEV_DSS_EDP0,
	TISCI_DEV_SA2_UL0,
	TISCI_DEV_CSI_TX_IF0,
	TISCI_DEV_CSI_TX_IF1,
	TISCI_DEV_NAVSS0,
	TISCI_DEV_CPSW1,
	TISCI_DEV_WKUP_VTM0,
	TISCI_DEV_MCU_FSS0,
	TISCI_DEV_WKUP_SMS0,
	TISCI_DEV_R5FSS0,
	TISCI_DEV_R5FSS1,
	TISCI_DEV_R5FSS2,
	TISCI_DEV_R5FSS1_CORE0,
	TISCI_DEV_R5FSS1_CORE1,
	TISCI_DEV_R5FSS2_CORE0,
	TISCI_DEV_R5FSS2_CORE1,
	TISCI_DEV_MCU_R5FSS0,
	TISCI_DEV_DMPAC0,
	TISCI_DEV_DMPAC0_SDE_0,
	TISCI_DEV_A72SS0,
	TISCI_DEV_A72SS1,
	TISCI_DEV_A72SS0_CORE0,
	TISCI_DEV_A72SS0_CORE1,
	TISCI_DEV_A72SS0_CORE2,
	TISCI_DEV_A72SS0_CORE3,
	TISCI_DEV_A72SS1_CORE0,
	TISCI_DEV_A72SS1_CORE1,
	TISCI_DEV_A72SS1_CORE2,
	TISCI_DEV_A72SS1_CORE3,
	TISCI_DEV_COMPUTE_CLUSTER0,
	TISCI_DEV_COMPUTE_CLUSTER0_C71SS0,
	TISCI_DEV_COMPUTE_CLUSTER0_C71SS1,
	TISCI_DEV_COMPUTE_CLUSTER0_C71SS2,
	TISCI_DEV_COMPUTE_CLUSTER0_C71SS3,
	TISCI_DEV_COMPUTE_CLUSTER0_DRU4,
	TISCI_DEV_COMPUTE_CLUSTER0_DRU5,
	TISCI_DEV_COMPUTE_CLUSTER0_DRU6,
	TISCI_DEV_COMPUTE_CLUSTER0_DRU7,
	TISCI_DEV_COMPUTE_CLUSTER0_MSMC2_WRAP_0,
	TISCI_DEV_COMPUTE_CLUSTER0_EN_MSMC_DOMAIN_0,
	TISCI_DEV_COMPUTE_CLUSTER0_GIC500SS,
#endif

    TISCI_DEV_MCU_I3C0,
    TISCI_DEV_MCU_I3C1,
    TISCI_DEV_R5FSS0_CORE0,
    TISCI_DEV_R5FSS0_CORE1,
    TISCI_DEV_MCAN0,
    TISCI_DEV_MCAN1,
    TISCI_DEV_MCAN2,
    TISCI_DEV_MCAN3,
    TISCI_DEV_MCAN4,
    TISCI_DEV_MCAN5,
    TISCI_DEV_MCAN6,
    TISCI_DEV_MCAN7,
    TISCI_DEV_MCAN8,
    TISCI_DEV_MCAN9,
    TISCI_DEV_MCAN10,
    TISCI_DEV_MCAN11,
    TISCI_DEV_MCAN12,
    TISCI_DEV_MCAN13,
    TISCI_DEV_PCIE1,
};

/* define the unlock and lock values */
#define KICK0_UNLOCK_VAL 0x68EF3490
#define KICK1_UNLOCK_VAL 0xD172BC5A
#define KICK_LOCK_VAL    0x00000000

#define MAIN_MMR_BASE_ADDRESS   CSL_CTRL_MMR0_CFG0_BASE
#define MCU_MMR_BASE_ADDRESS    CSL_MCU_CTRL_MMR0_CFG0_BASE
#define WKUP_MMR_BASE_ADDRESS   CSL_WKUP_CTRL_MMR0_CFG0_BASE
#define MAIN_PLL_BASE_ADDRESS   CSL_PLL0_CFG_BASE
#define MCU_PLL_BASE_ADDRESS    CSL_MCU_PLL0_CFG_BASE

uint32_t MMR_unlock_one(uint32_t * kick0, uint32_t * kick1)
{
    /* initialize the status variable */
    uint32_t status = 1;

    /* if either of the kick lock registers are locked */
    if (!(*kick0 & 0x1) | !(*kick1 & 0x1))
    {
        /* unlock the partition by writing the unlock values to the kick lock registers */
        *kick0 = KICK0_UNLOCK_VAL;
        *kick1 = KICK1_UNLOCK_VAL;
    }

    /* check to see if either of the kick registers are unlocked. */
    if (!(*kick0 & 0x1))
    {
        status = 0;
    }

    /* return the status to the calling program */
    return status;

}

#if defined (SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J784S4)
uint32_t MAIN_CTRL_MMR_unlock_all()
{
    /* initialize the status variable */
    uint32_t status = 1;
    /* Unlock the 0th partition */
    status &= MMR_unlock_one(
            (uint32_t *)(MAIN_MMR_BASE_ADDRESS
                    + CSL_MAIN_CTRL_MMR_CFG0_LOCK0_KICK0),
            (uint32_t *)(MAIN_MMR_BASE_ADDRESS
                    + CSL_MAIN_CTRL_MMR_CFG0_LOCK0_KICK1));
    /* Unlock the 1st partition */
    status &= MMR_unlock_one(
            (uint32_t *)(MAIN_MMR_BASE_ADDRESS
                    + CSL_MAIN_CTRL_MMR_CFG0_LOCK1_KICK0),
            (uint32_t *)(MAIN_MMR_BASE_ADDRESS
                    + CSL_MAIN_CTRL_MMR_CFG0_LOCK1_KICK1));
    /* Unlock the 2nd partition */
    status &= MMR_unlock_one(
            (uint32_t *)(MAIN_MMR_BASE_ADDRESS
                    + CSL_MAIN_CTRL_MMR_CFG0_LOCK2_KICK0),
            (uint32_t *)(MAIN_MMR_BASE_ADDRESS
                    + CSL_MAIN_CTRL_MMR_CFG0_LOCK2_KICK1));
    /* Unlock the 3rd partition */
    status &= MMR_unlock_one(
            (uint32_t *)(MAIN_MMR_BASE_ADDRESS
                    + CSL_MAIN_CTRL_MMR_CFG0_LOCK3_KICK0),
            (uint32_t *)(MAIN_MMR_BASE_ADDRESS
                    + CSL_MAIN_CTRL_MMR_CFG0_LOCK3_KICK1));
    /* Unlock the 4th partition */
    status &= MMR_unlock_one(
            (uint32_t *)(MAIN_MMR_BASE_ADDRESS
                    + CSL_MAIN_CTRL_MMR_CFG0_LOCK4_KICK0),
            (uint32_t *)(MAIN_MMR_BASE_ADDRESS
                    + CSL_MAIN_CTRL_MMR_CFG0_LOCK4_KICK1));
/* These two not in the CSL yet
   status &= MMR_unlock_one(
            (uint32_t *)(MAIN_MMR_BASE_ADDRESS
                    + CSL_MAIN_CTRL_MMR_CFG0_LOCK5_KICK0),
            (uint32_t *)(MAIN_MMR_BASE_ADDRESS
                    + CSL_MAIN_CTRL_MMR_CFG0_LOCK5_KICK1));
    status &= MMR_unlock_one(
            (uint32_t *)(MAIN_MMR_BASE_ADDRESS
                    + CSL_MAIN_CTRL_MMR_CFG0_LOCK6_KICK0),
            (uint32_t *)(MAIN_MMR_BASE_ADDRESS
                    + CSL_MAIN_CTRL_MMR_CFG0_LOCK6_KICK1));
*/
    /* Unlock the 7th partition */
    status &= MMR_unlock_one(
            (uint32_t *)(MAIN_MMR_BASE_ADDRESS
                    + CSL_MAIN_CTRL_MMR_CFG0_LOCK7_KICK0),
            (uint32_t *)(MAIN_MMR_BASE_ADDRESS
                    + CSL_MAIN_CTRL_MMR_CFG0_LOCK7_KICK1));
    /* Return status to calling program */
    return status;
}
#endif

typedef enum /* updated for J7 */
{
    SERDES_DIAG_PCIE_GEN1 = 1,

    SERDES_DIAG_PCIE_GEN2 = 2,

    SERDES_DIAG_PCIE_GEN3 = 3,

    SERDES_DIAG_PCIE_GEN4 = 4
} SERDES_DIAG_PCIE_TYPE;

#define SERDES_DIAG_TEST_REF_CLOCK              CSL_SERDES_REF_CLOCK_100M /* Ref clock of serdes */
#define SERDES_DIAG_TEST_SSC_MODE               CSL_SERDES_NO_SSC
#define SERDES_DIAG_TEST_LANE_MASK              0x3 /* All lanes are set */
#define SERDES_DIAG_TEST_PHY_TYPE               CSL_SERDES_PHY_TYPE_PCIe /* For running PCIe tests */
#define SERDES_DIAG_TEST_PRBS_PATTERN           SERDES_PRBS_7 /* prbs7 pattern */
#define SERDES_DIAG_TEST_LANE_RATE              CSL_SERDES_LANE_FULL_RATE /* Set to run at full rate of the link rate */
#define SERDES_DIAG_TEST_LOOPBACK_MODE          CSL_SERDES_LOOPBACK_DISABLED /* For internal near end serial loopback tests */
#define SERDES_DIAG_TEST_OPERATING_MODE         CSL_SERDES_FUNCTIONAL_MODE /* Should always be set to Diagnostic Mode for BER, EYE and PRBS tests */
#define SERDES_DIAG_TEST_FORCEATTBOOST          CSL_SERDES_FORCE_ATT_BOOST_DISABLED
#define SERDES_DIAG_TEST_REF_CLOCK_SRC          CSL_SERDES_REF_CLOCK_INT /* Internally sourced ref clk */

#define SERDES_DIAG_TEST_NUM_LANES              (0x2) /* Number of lanes to be tested */
#define SERDES_DIAG_TEST_LINK_RATE            CSL_SERDES_LINK_RATE_5G
#define SERDES_DIAG_TEST_PCIE_GEN_TYPE          SERDES_DIAG_PCIE_GEN3 /* Generation to be tested */

#if defined (SOC_J721E)
uint32_t serdes_init(uint8_t serdesInstance, CSL_SerdesSSCMode SSC_Mode){
    CSL_SerdesResult status;
    uint32_t i, laneNum;
    CSL_SerdesLaneEnableParams serdesLaneEnableParams;
    CSL_SerdesLaneEnableStatus laneRetVal = CSL_SERDES_LANE_ENABLE_NO_ERR;
    /* SERDES_DIAG_PCIE_TYPE pcie_gen_type;  We can use use link rate to determine the gen type in code */

    if(serdesInstance > 3){
        UART_printf("Invalid Serdes!\n");
        return 1;
    }

    memset(&serdesLaneEnableParams, 0, sizeof(serdesLaneEnableParams));
    serdesLaneEnableParams.serdesInstance = (CSL_SerdesInstance)serdesInstance;

    switch(serdesInstance){
        case 0:
            serdesLaneEnableParams.baseAddr = CSL_SERDES_16G0_BASE;
            break;
        case 1:
            serdesLaneEnableParams.baseAddr = CSL_SERDES_16G1_BASE;
            break;
        case 2:
            serdesLaneEnableParams.baseAddr = CSL_SERDES_16G2_BASE;
            break;
        case 3:
            serdesLaneEnableParams.baseAddr = CSL_SERDES_16G3_BASE;
            break;
    }

    serdesLaneEnableParams.refClock         = SERDES_DIAG_TEST_REF_CLOCK;
    serdesLaneEnableParams.refClkSrc        = SERDES_DIAG_TEST_REF_CLOCK_SRC;
    serdesLaneEnableParams.linkRate         = SERDES_DIAG_TEST_LINK_RATE;
    serdesLaneEnableParams.numLanes         = SERDES_DIAG_TEST_NUM_LANES;
    serdesLaneEnableParams.laneMask         = SERDES_DIAG_TEST_LANE_MASK;
    serdesLaneEnableParams.SSC_mode         = SSC_Mode;
    serdesLaneEnableParams.phyType          = SERDES_DIAG_TEST_PHY_TYPE;
    serdesLaneEnableParams.pcieGenType      = SERDES_DIAG_TEST_PCIE_GEN_TYPE;
    serdesLaneEnableParams.operatingMode    = SERDES_DIAG_TEST_OPERATING_MODE;
    serdesLaneEnableParams.phyInstanceNum   = serdesInstance;
    for(i=0; i< serdesLaneEnableParams.numLanes; i++){
      serdesLaneEnableParams.laneCtrlRate[i] = SERDES_DIAG_TEST_LANE_RATE;
      serdesLaneEnableParams.loopbackMode[i] = SERDES_DIAG_TEST_LOOPBACK_MODE; /* still have to change to correct loopback mode */
    }
    /* pcie_gen_type = SERDES_DIAG_TEST_PCIE_GEN_TYPE; */

    CSL_serdesPorReset(serdesLaneEnableParams.baseAddr);

    /* Select the IP type, IP instance num, Serdes Lane Number */
    for (laneNum=0; laneNum < serdesLaneEnableParams.numLanes; laneNum++){
        CSL_serdesIPSelect(CSL_CTRL_MMR0_CFG0_BASE,
                           serdesLaneEnableParams.phyType,
                           serdesLaneEnableParams.phyInstanceNum,
                           serdesLaneEnableParams.serdesInstance,
                           laneNum);
    }

    /* selects the appropriate clocks for all serdes based on the protocol chosen */
     status = CSL_serdesRefclkSel(CSL_CTRL_MMR0_CFG0_BASE,
                                  serdesLaneEnableParams.baseAddr,
                                  serdesLaneEnableParams.refClock,
                                  serdesLaneEnableParams.refClkSrc,
                                  serdesLaneEnableParams.serdesInstance,
                                  serdesLaneEnableParams.phyType);

     /* Assert PHY reset and disable all lanes */
     CSL_serdesDisablePllAndLanes(serdesLaneEnableParams.baseAddr, serdesLaneEnableParams.numLanes, serdesLaneEnableParams.laneMask);

    /*Load the Serdes Config File */
    status = CSL_serdesPCIeInit(&serdesLaneEnableParams); /* Use this for PCIe serdes config load */

    /* Return error if input params are invalid */
    if (status != CSL_SERDES_NO_ERR){
        UART_printf("Invalid SERDES Init Params \n");
    }

    /* Set this to standard mode defined by Cadence */
    for (laneNum=0; laneNum < serdesLaneEnableParams.numLanes; laneNum++){
        CSL_serdesPCIeModeSelect(serdesLaneEnableParams.baseAddr, serdesLaneEnableParams.pcieGenType, laneNum);
    }

    /* Common Lane Enable API for lane enable, pll enable etc */
    laneRetVal = CSL_serdesLaneEnable(&serdesLaneEnableParams);

    if (laneRetVal != 0){
        UART_printf("Invalid Serdes Lane Enable\n");
        return 2;
    }

    UART_printf("Serdes %d Init Complete\n", serdesInstance);
    return 0;
}

void ecc_aggr_test_pcie_init()
{
  MAIN_CTRL_MMR_unlock_all();
  serdes_init(0x0, CSL_SERDES_NO_SSC);
  serdes_init(0x1, CSL_SERDES_NO_SSC);
  serdes_init(0x2, CSL_SERDES_NO_SSC);
  serdes_init(0x3, CSL_SERDES_NO_SSC);

  UART_printf("Serdes Init Complete\n");
}

void edp_init()
{
    SDL_REG32_WR(((uintptr_t)SDL_CTRL_MMR0_CFG0_BASE) + ((uintptr_t)SDL_MAIN_CTRL_MMR_CFG0_EDP_PHY0_CLKSEL), 0x1);
    SDL_REG32_WR(((uintptr_t)SDL_DSS_EDP0_INTG_CFG_VP_BASE) + ((uintptr_t)0x8), 0x1);
    SDL_REG32_WR(((uintptr_t)SDL_DSS_EDP0_INTG_CFG_VP_BASE) + ((uintptr_t)0x10), 0x1F);

    SDL_REG32_WR(((uintptr_t)SDL_WKUP_CTRL_MMR0_CFG0_BASE) + ((uintptr_t)SDL_WKUP_CTRL_MMR_CFG0_HFOSC1_CTRL), 0x0);

    SDL_REG32_WR(((uintptr_t)0x05050000) + ((uintptr_t)0x408), 0xA0000000);
    SDL_REG32_WR(((uintptr_t)0x05050000) + ((uintptr_t)0x40c), 0x39000000);
    SDL_REG32_WR(((uintptr_t)0x05050000) + ((uintptr_t)0x480), 0x80000000);
    SDL_REG32_WR(((uintptr_t)0x05050000) + ((uintptr_t)0x4c0), 0x80000000);
    SDL_REG32_WR(((uintptr_t)0x05050000) + ((uintptr_t)0x500), 0x80000000);
    SDL_REG32_WR(((uintptr_t)0x05050000) + ((uintptr_t)0x540), 0x80000000);
    SDL_REG32_WR(((uintptr_t)0x05050000) + ((uintptr_t)0x484), 0x10001);
    SDL_REG32_WR(((uintptr_t)0x05050000) + ((uintptr_t)0x4c4), 0x10001);
    SDL_REG32_WR(((uintptr_t)0x05050000) + ((uintptr_t)0x504), 0x10001);
    SDL_REG32_WR(((uintptr_t)0x05050000) + ((uintptr_t)0x544), 0x10001);
    SDL_REG32_WR(((uintptr_t)0x05050000) + ((uintptr_t)0x5fc), 0x00000);

}
#endif
#if defined (SOC_J784S4)
void edp_init()
{
    SDL_REG32_WR(((uintptr_t)SDL_CTRL_MMR0_CFG0_BASE) + ((uintptr_t)SDL_MAIN_CTRL_MMR_CFG0_EDP_PHY0_CLKSEL), 0x1);
    SDL_REG32_WR(((uintptr_t)SDL_DSS_EDP0_INTG_CFG_VP_BASE) + ((uintptr_t)0x8), 0x1);
    SDL_REG32_WR(((uintptr_t)SDL_DSS_EDP0_INTG_CFG_VP_BASE) + ((uintptr_t)0x10), 0x1F);

    SDL_REG32_WR(((uintptr_t)SDL_WKUP_CTRL_MMR0_CFG0_BASE) + ((uintptr_t)SDL_WKUP_CTRL_MMR_CFG0_HFOSC1_CTRL), 0x0);

    SDL_REG32_WR(((uintptr_t)0x05050000) + ((uintptr_t)0x408), 0xA0000000);
    SDL_REG32_WR(((uintptr_t)0x05050000) + ((uintptr_t)0x40c), 0x39000000);
    SDL_REG32_WR(((uintptr_t)0x05050000) + ((uintptr_t)0x480), 0x80000000);
    SDL_REG32_WR(((uintptr_t)0x05050000) + ((uintptr_t)0x4c0), 0x80000000);
    SDL_REG32_WR(((uintptr_t)0x05050000) + ((uintptr_t)0x500), 0x80000000);
    SDL_REG32_WR(((uintptr_t)0x05050000) + ((uintptr_t)0x540), 0x80000000);
    SDL_REG32_WR(((uintptr_t)0x05050000) + ((uintptr_t)0x484), 0x10001);
    SDL_REG32_WR(((uintptr_t)0x05050000) + ((uintptr_t)0x4c4), 0x10001);
    SDL_REG32_WR(((uintptr_t)0x05050000) + ((uintptr_t)0x504), 0x10001);
    SDL_REG32_WR(((uintptr_t)0x05050000) + ((uintptr_t)0x544), 0x10001);
    SDL_REG32_WR(((uintptr_t)0x05050000) + ((uintptr_t)0x5fc), 0x00000);

}
#endif

int32_t sdlApp_initDevices(void)
{
    int32_t status = SDL_PASS;
    uint32_t i;
#if defined (SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J784S4)
    uint32_t read_val;
#endif

    for (i = 0; i < AUX_NUM_DEVICES; i++)
    {
        /* Power up RTI */
        status = Sciclient_pmSetModuleState(aux_devices[i],
                                            TISCI_MSG_VALUE_DEVICE_SW_STATE_ON,
                                            TISCI_MSG_FLAG_AOP,
                                            SCICLIENT_SERVICE_WAIT_FOREVER);

        if (status != SDL_PASS)
        {
            UART_printf("   Sciclient_pmSetModuleState 0x%x ...FAILED: retValue %d\n",
                        aux_devices[i], status);
        }
    }
#if defined (SOC_J721E) || defined (SOC_J784S4)
    // UFS init clocks - SDL_UFS0_P2A_WRAP_CFG_VBP_UFSHCI_BASE
    read_val = SDL_REG32_RD(((uintptr_t)SDL_UFS0_P2A_WRAP_CFG_VBP_UFSHCI_BASE)+((uintptr_t)0x34));
    SDL_REG32_WR(((uintptr_t)SDL_UFS0_P2A_WRAP_CFG_VBP_UFSHCI_BASE)+ ((uintptr_t)0x34), read_val | 0x1);
    read_val = SDL_REG32_RD(((uintptr_t)SDL_UFS0_P2A_WRAP_CFG_VBP_UFSHCI_BASE)+((uintptr_t)0x34));
    while(read_val!= 0x1) {
        read_val = SDL_REG32_RD(((uintptr_t)SDL_UFS0_P2A_WRAP_CFG_VBP_UFSHCI_BASE)+((uintptr_t)0x34));
    }
    read_val = SDL_REG32_RD(((uintptr_t)SDL_UFS0_P2A_WRAP_CFG_VBP_UFSHCI_BASE)+((uintptr_t)0xc8));
    SDL_REG32_WR(((uintptr_t)SDL_UFS0_P2A_WRAP_CFG_VBP_UFSHCI_BASE)+ ((uintptr_t)0xc8), read_val | 0xF8);

    // SA2UL init
    read_val = SDL_REG32_RD(((uintptr_t)SDL_SA2_UL0_BASE)+((uintptr_t)0x1000));
    SDL_REG32_WR(((uintptr_t)SDL_SA2_UL0_BASE)+ ((uintptr_t)0x1000), read_val | 0x10);
#if !defined(SOC_J784S4)
    // PCIE init
    ecc_aggr_test_pcie_init();
#endif
    // EDP init
    edp_init();
#elif defined (SOC_J721S2)
    // SA2UL init
    read_val = SDL_REG32_RD(((uintptr_t)SDL_SA2_UL0_BASE)+((uintptr_t)0x1000));
    SDL_REG32_WR(((uintptr_t)SDL_SA2_UL0_BASE)+ ((uintptr_t)0x1000), read_val | 0x10);
#endif
    return status;
}

static int32_t sdlApp_osalInit(void)
{
    SDL_ErrType_t ret = SDL_PASS;

    ret = SDL_TEST_osalInit();
    if (ret != SDL_PASS)
    {
        UART_printf("Error: Init Failed\n");
    }

    return ret;
}

void ECC_func_app(void)
{
    int32_t    testResult;
    testResult = ECC_funcTest();
    UART_printf("\n ECC func Test");
    if (testResult == SDL_PASS)
    {
        UART_printStatus("\r\nAll test have passed. \r\n");
#ifdef UNITY_INCLUDE_CONFIG_H
        TEST_PASS();
#endif
    }
    else
    {
        UART_printStatus("\r\nSome test have failed. \r\n");
#ifdef UNITY_INCLUDE_CONFIG_H
        TEST_FAIL();
#endif
    }
}

void ecc_app_runner(void)
{
#ifdef UNITY_INCLUDE_CONFIG_H
    UNITY_BEGIN();
    RUN_TEST(ECC_func_app);
    UNITY_END();
    /* Function to print results defined in our unity_config.h file */
    print_unityOutputBuffer_usingUARTstdio();
#else
    ECC_func_app();
#endif
}


int32_t main(void)
{
    /* Declaration of variables */
    int32_t  testResult;

    /* Init Board */
    testResult = ECC_appInitBoard();

    /* Init the modules */
    sdlApp_initDevices();

    if (testResult == SDL_PASS)
    {
        sdlApp_osalInit();

        UART_printf("\nECC test Application\r\n");
        ecc_app_runner();

    }
    else
    {
        UART_printStatus("\r\nBoard Init failed. Exiting the app.\r\n");
    }

    return (0);
}

/* Nothing past this point */
