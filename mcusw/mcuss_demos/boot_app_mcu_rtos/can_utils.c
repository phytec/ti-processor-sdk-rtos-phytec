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
 *  \file can_profile.h.c
 *
 *  \brief Main file for TI-RTOS build
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/csl/arch/csl_arch.h>
#include <ti/csl/soc.h>
#include <ti/csl/cslr.h>

#include <ti/osal/osal.h>
#include <ti/osal/TaskP.h>

#include "CanApp_Priv.h"

#if defined(CAN_PROFILE_TASK_ENABLED)
#    include "can_profile.h"
#endif
#if defined(CAN_RESP_TASK_ENABLED)
#    include "can_resp.h"
#endif
#include "CanIf.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Test application stack size */
#define APP_TASK_STACK                  (10U * 1024U)
/**< Stack required for the stack */
#define CAN_PROFILE_DEMO_TASK_NAME      ("CAN PROFILE")
#define CAN_RESP_DEMO_TASK_NAME         ("CAN RESPONSE")
/**< Task name */



/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static void CanApp_BuildIntList(void);
static void CanApp_InterruptConfig(void);
static void CanApp_EnableTransceivers(void);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

Can_PduType             CanApp_Pdu, CanApp_Pdu1;
/**< Variables which contains Can PDU data used in Can transmission */
uint8                   CanApp_InputData[64U] =
{0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x22, 0x44,
0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x22, 0x44,
0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x22, 0x44,
0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x22, 0x44,
0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x22, 0x44,
0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x22, 0x44,
0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x22, 0x44,
0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x22, 0x44,
};
/**< Variable which contains Can SDU data that is transported inside the PDU */
volatile PduInfoType   *CanApp_RxPdu     = NULL;
/**< Pointer received data */
uint32                  CanApp_DemStatus = E_OK;
/**< Variable used to track dem notifications */
uint32                  CanApp_DetStatus = E_OK;
/**< Variable used to track det notifications */

static uint32           CanApp_IntNumbers[CAN_MAX_CONTROLLER];
/**< Stored the interrupt numbers for enabled all timers */
static CanApp_IsrType   CanApp_Isr[CAN_MAX_CONTROLLER];
/**< Associated ISR */
static HwiP_Handle      CanApp_IsrHndls[CAN_MAX_CONTROLLER];
/**< Stores the ISR handles */
extern SemaphoreP_Handle CanIf_TxConfirmationSemaphore;
/**< TX Confirmation semaphore, would be posted when TX completes */
extern SemaphoreP_Handle CanIf_RxConfirmationSemaphore;
/**< Rx Confirmation semaphore, would be posted when TX completes */

/* ========================================================================== */
/*                            External Variables                              */
/* ========================================================================== */

extern uint32 CanIf_DrvStatus;
/**< CAN IF Driver Status, defined in CanIf.c */

uint64_t CanProfileCumulativeTx;
/**< Global used to track cumulative time spent for transmission */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
#define CAN_START_SEC_ISR_CODE
#include "Can_MemMap.h"

FUNC(void, CAN_CODE_FAST) CanApp_CanXIsr (
                                                            uintptr_t CanPtr)
{
#if defined(CAN_PROFILE_TASK_ENABLED)
    volatile uint64_t   preTimeStamp, postTimeStamp;

    preTimeStamp = TimerP_getTimeInUsecs();
#endif
    CanApp_IsrType canChIsr = (CanApp_IsrType)CanPtr;
    /* Associated CAN ISR */
    canChIsr();
#if defined(CAN_PROFILE_TASK_ENABLED)
    postTimeStamp = TimerP_getTimeInUsecs();
    CanProfileCumulativeTx +=
                CanProfileGetTimeSpent(preTimeStamp, postTimeStamp);
#endif
}

/** \brief Start up sequence : Program the interrupt muxes / priorities */
void CanApp_Startup(void)
{
    /* Equivalent to EcuM_AL_SetProgrammableInterrupts */
    CanApp_BuildIntList();

    CanApp_InterruptConfig();

    /* Initialize counters, that would be required for timed operations */
    AppUtils_ProfileInit(0);
}

/** \brief Application tear down functions */
void CanApp_Shutdown(void)
{
    uint32 idx;

    for (idx = 0U; idx < CAN_NUM_CONTROLLER; idx++)
    {
        if (NULL != CanApp_IsrHndls[idx])
        {
            if (HwiP_OK != HwiP_delete(CanApp_IsrHndls[idx]))
            {
                AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                                " Error!!! Could not De register"
                                " the ISR for instance %d!!!\n", idx);
                break;
            }
        }
    }

    if (NULL != CanIf_TxConfirmationSemaphore)
    {
        SemaphoreP_delete(CanIf_TxConfirmationSemaphore);
        if (NULL != CanIf_RxConfirmationSemaphore)
        {
            SemaphoreP_delete(CanIf_RxConfirmationSemaphore);
        }
    }
    return;
}

static void CanApp_BuildIntList(void)
{
    uint32 idx, flag, intNum;
    const Can_ConfigType *Can_ConfigPtr;
    CanApp_IsrType pIsrHandler = NULL;

    AppUtils_Printf(MSG_NORMAL, "\n");
    /*
     * 1. Determine the number of CAN used
     * 3. Build isr and interrupt number for enabled CAN instances
     */
    /* Do Can Init */
#if (STD_ON == CAN_VARIANT_PRE_COMPILE)
    Can_ConfigPtr = &CAN_INIT_CONFIG_PC;
    Can_Init((const Can_ConfigType *) NULL_PTR);
#if (CAN_INITIAL_PRINT_DISABLE_BEFORE_CAN_RESPONSE == STD_OFF)
    AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                    "Variant - Pre Compile being used !!!\n");
#endif
#else
    Can_ConfigPtr = &CanConfigSet;
    Can_Init(Can_ConfigPtr);
#if (CAN_INITIAL_PRINT_DISABLE_BEFORE_CAN_RESPONSE == STD_OFF)
    AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                    "Variant - Post Build being used !!!\n");
#endif
#endif

    for (idx = 0U; idx < Can_ConfigPtr->CanMaxControllerCount; idx++)
    {
        flag = 0U;
        if (CAN_CONTROLLER_INSTANCE_MCU_MCAN0 ==
            CanConfigSet_CanController_List_PC[idx]->CanControllerInst)
        {
            intNum = APP_MCAN_0_INT0;
            pIsrHandler = Can_0_Int0ISR;
            flag = 1U;
        }
        if (CAN_CONTROLLER_INSTANCE_MCU_MCAN1 ==
            CanConfigSet_CanController_List_PC[idx]->CanControllerInst)
        {
            intNum = APP_MCAN_1_INT0;
            pIsrHandler = Can_1_Int0ISR;
            flag = 1U;
        }
        if (0U != flag)
        {
            CanApp_IntNumbers[idx] = intNum;
            CanApp_Isr[idx] = pIsrHandler;
            flag = 0U;
        }
    }

    return;
}

static void CanApp_InterruptConfig(void)
{
    uint32 idx;
    HwiP_Params hwiParams;

    for (idx = 0U; idx < CAN_NUM_CONTROLLER; idx++)
    {
        HwiP_Params_init(&hwiParams);
        hwiParams.arg = (uintptr_t)CanApp_Isr[idx];

        CanApp_IsrHndls[idx] = HwiP_create(CanApp_IntNumbers[idx],
                                                    CanApp_CanXIsr, &hwiParams);
        if (NULL == CanApp_IsrHndls[idx])
        {
            AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                            " Error!!! Could not register the ISR for instance"
                            "%d!!!\n", idx);
            break;
        }
    }
    return;
}

/** \brief None, SBL/GEL powers up the timers and clock sources */
void CanApp_PowerAndClkSrc(void)
{
    /* Mcu module, when included will replace this operation */
    return;
}

void CanApp_PlatformInit()
{
    uint32 regVal = 0U;
    /* Unlock lock key registers for Partition 7: IO PAD
       configuration registers in MAIN_CTRL_MMR */
    /* write Partition 7 Lock Key 0 Register */
    CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1D008, 0x68EF3490);
    /* write Partition 7 Lock Key 1 Register */
    CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1D00C, 0xD172BC5A);
    /* Check for unlock */
    regVal = CSL_REG32_RD(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1D008);
    while ((regVal & 0x1) != 0x1U)
    {
        regVal = CSL_REG32_RD(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1D008);
    }
    /* Unlocking done */
    /* Below code will be replaced by Port module in further releases */
    /* MCAN 0 Tx PAD configuration */
#if defined(SOC_J721E)
    regVal = CSL_REG32_RD(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1C0A8U);
    regVal &= 0U;
    regVal |= 0x60000U;
    CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1C0A8U, regVal);
    /* MCAN 0 Rx PAD configuration */
    regVal = CSL_REG32_RD(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1C0ACU);
    regVal &= 0U;
    regVal |= 0x60000U;
    CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1C0ACU, regVal);
    /* MCAN 1 Tx PAD configuration */
    regVal = CSL_REG32_RD(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1C0C0U);
    regVal &= 0U;
    regVal |= 0x60001U;
    CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1C0C0U, regVal);
    /* MCAN 1 Rx PAD configuration */
    regVal = CSL_REG32_RD(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1C0C4U);
    regVal &= 0U;
    regVal |= 0x60001U;
    CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1C0C4U, regVal);
#elif defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
    regVal = CSL_REG32_RD(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1C0B8U);
    regVal &= 0U;
    regVal |= 0x60000U;
    CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1C0B8U, regVal);
    /* MCAN 0 Rx PAD configuration */
    regVal = CSL_REG32_RD(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1C0BCU);
    regVal &= 0U;
    regVal |= 0x60000U;
    CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1C0BCU, regVal);
    /* MCAN 1 Tx PAD configuration */
    regVal = CSL_REG32_RD(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1C0D0U);
    regVal &= 0U;
    regVal |= 0x60000U;
    CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1C0D0U, regVal);
    /* MCAN 1 Rx PAD configuration */
    regVal = CSL_REG32_RD(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1C0D4U);
    regVal &= 0U;
    regVal |= 0x60000U;
    CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1C0D4U, regVal);
#endif

    /* Take MCAN transceivers out of STB mode i.e NORMAL Mode */
    CanApp_EnableTransceivers();
}

static void CanApp_EnableTransceivers(void)
{
    uint32 regVal = 0U;
    Dio_LevelType dioPinLevel[2U];

#if defined (SOC_J721E)
    /* Take MCAN transceiver out of STB mode for MCU MCAN0 */
    /* Below code will be replaced by Port module in further releases */
    /* Pin mux configuration of Dio Pin 355 */
    regVal = 0x08050007U;
    CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE +
        CSL_WKUP_CTRL_MMR_CFG0_PADCONFIG44, regVal);
    /* Set Pin direction to output */
    regVal = CSL_REG32_RD(CSL_WKUP_GPIO0_BASE + 0x10U);
    regVal &= (~(1U << 0x0U));
    CSL_REG32_WR(CSL_WKUP_GPIO0_BASE + 0x10U, regVal);
    /* Drive Pin to High */
    Dio_WriteChannel(CAN_TRCV_MCU_MCAN_0, STD_HIGH);
     /* Read Pin level */
    dioPinLevel[1U] = Dio_ReadChannel(CAN_TRCV_MCU_MCAN_0);
    /*Read back the pin levels to ensure transceiver is enabled*/
    if (STD_HIGH != dioPinLevel[1U])
    {
        AppUtils_Printf(MSG_NORMAL,
        "CAN_APP: Error in Enabling CAN Transceiver MCU MCAN0!!!\n");
    }
    else
    {
        regVal = 0x08050007U;
        CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE +
            CSL_WKUP_CTRL_MMR_CFG0_PADCONFIG38, regVal);
        /* Set Pin direction to output */
        regVal = CSL_REG32_RD(CSL_WKUP_GPIO0_BASE + 0x38U);
        regVal &= (~(1U << 0x16U));
        CSL_REG32_WR(CSL_WKUP_GPIO0_BASE + 0x38U, regVal);
        /* Drive Pin to High */
        Dio_WriteChannel(54U, STD_HIGH);
         /* Read Pin level */
        dioPinLevel[1U] = Dio_ReadChannel(54U);
        /*Read back the pin levels to ensure transceiver is enabled*/
        if (STD_HIGH != dioPinLevel[1U])
        {
            AppUtils_Printf(MSG_NORMAL,
            "CAN_APP: Error in Enabling CAN Transceiver MCU MCAN0!!!\n");
        }
        else
        {
#if (CAN_INITIAL_PRINT_DISABLE_BEFORE_CAN_RESPONSE == STD_OFF)
            AppUtils_Printf(MSG_NORMAL,
            "CAN_APP: Successfully Enabled CAN Transceiver MCU MCAN0!!!\n");
#endif
        }
    }

    /* Take MCAN transceiver out of STB mode for MCU MCAN1 */
    /* Below code will be replaced by Port module in further releases */
    /* Pin mux configuration of Dio Pin 2 */
    regVal = 0x08050007U;
    CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE +
        CSL_WKUP_CTRL_MMR_CFG0_PADCONFIG46, regVal);
    /* Set Pin direction to output */
    regVal = CSL_REG32_RD(CSL_WKUP_GPIO0_BASE + 0x10U);
    regVal &= (~(1U << 0x2U));
    CSL_REG32_WR(CSL_WKUP_GPIO0_BASE + 0x10U, regVal);
    /* Drive Pin to Low */
    Dio_WriteChannel(CAN_TRCV_MCU_MCAN_1, STD_LOW);
     /* Read Pin level */
    dioPinLevel[1U] = Dio_ReadChannel(CAN_TRCV_MCU_MCAN_1);
    /*Read back the pin levels to ensure transceiver is enabled*/
    if (STD_LOW != dioPinLevel[1U])
    {
        AppUtils_Printf(MSG_NORMAL,
        "CAN_APP: Error in Enabling CAN Transceiver MCU MCAN1!!!\n");
    }
    else
    {
#if (CAN_INITIAL_PRINT_DISABLE_BEFORE_CAN_RESPONSE == STD_OFF)
        AppUtils_Printf(MSG_NORMAL,
        "CAN_APP: Successfully Enabled CAN Transceiver MCU MCAN1!!!\n");
#endif
    }

#elif defined (SOC_J7200)
    /* Take MCAN transceiver out of STB mode for MCU MCAN0 */
    /* Below code will be replaced by Port module in further releases */
    /* Pin mux configuration of Dio Pin 355 */
    regVal = 0x08050007U;
    CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE +
        CSL_WKUP_CTRL_MMR_CFG0_PADCONFIG48, regVal);
    /* Set Pin direction to output */
    regVal = CSL_REG32_RD(CSL_WKUP_GPIO0_BASE + 0x10U);
    regVal &= (~(1U << 0x0U));
    CSL_REG32_WR(CSL_WKUP_GPIO0_BASE + 0x10U, regVal);
    /* Drive Pin to High */
    Dio_WriteChannel(CAN_TRCV_MCU_MCAN_0, STD_HIGH);
     /* Read Pin level */
    dioPinLevel[1U] = Dio_ReadChannel(CAN_TRCV_MCU_MCAN_0);
    /*Read back the pin levels to ensure transceiver is enabled*/
    if (STD_HIGH != dioPinLevel[1U])
    {
        AppUtils_Printf(MSG_NORMAL,
        "CAN_APP: Error in Enabling CAN Transceiver MCU MCAN0!!!\n");
    }
    else
    {
        regVal = 0x08050007U;
        CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE +
            CSL_WKUP_CTRL_MMR_CFG0_PADCONFIG42, regVal);
        /* Set Pin direction to output */
        regVal = CSL_REG32_RD(CSL_WKUP_GPIO0_BASE + 0x38U);
        regVal &= (~(1U << 0x1AU));
        CSL_REG32_WR(CSL_WKUP_GPIO0_BASE + 0x38U, regVal);
        /* Drive Pin to High */
        Dio_WriteChannel(58U, STD_HIGH);
         /* Read Pin level */
        dioPinLevel[1U] = Dio_ReadChannel(58U);
        /*Read back the pin levels to ensure transceiver is enabled*/
        if (STD_HIGH != dioPinLevel[1U])
        {
            AppUtils_Printf(MSG_NORMAL,
            "CAN_APP: Error in Enabling CAN Transceiver MCU MCAN0!!!\n");
        }
        else
        {
#if (CAN_INITIAL_PRINT_DISABLE_BEFORE_CAN_RESPONSE == STD_OFF)
            AppUtils_Printf(MSG_NORMAL,
            "CAN_APP: Successfully Enabled CAN Transceiver MCU MCAN0!!!\n");
#endif
        }
    }

    /* Take MCAN transceiver out of STB mode for MCU MCAN1 */
    /* Below code will be replaced by Port module in further releases */
    /* Pin mux configuration of Dio Pin 2 */
    regVal = 0x08050007U;
    CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE +
        CSL_WKUP_CTRL_MMR_CFG0_PADCONFIG50, regVal);
    /* Set Pin direction to output */
    regVal = CSL_REG32_RD(CSL_WKUP_GPIO0_BASE + 0x10U);
    regVal &= (~(1U << 0x2U));
    CSL_REG32_WR(CSL_WKUP_GPIO0_BASE + 0x10U, regVal);
    /* Drive Pin to Low */
    Dio_WriteChannel(CAN_TRCV_MCU_MCAN_1, STD_LOW);
     /* Read Pin level */
    dioPinLevel[1U] = Dio_ReadChannel(CAN_TRCV_MCU_MCAN_1);
    /*Read back the pin levels to ensure transceiver is enabled*/
    if (STD_LOW != dioPinLevel[1U])
    {
        AppUtils_Printf(MSG_NORMAL,
        "CAN_APP: Error in Enabling CAN Transceiver MCU MCAN1!!!\n");
    }
    else
    {
#if (CAN_INITIAL_PRINT_DISABLE_BEFORE_CAN_RESPONSE == STD_OFF)
        AppUtils_Printf(MSG_NORMAL,
        "CAN_APP: Successfully Enabled CAN Transceiver MCU MCAN1!!!\n");
#endif
    }
#elif defined (SOC_J721S2)
    /* Take MCAN transceiver out of STB mode for MCU MCAN0 */
    /* Below code will be replaced by Port module in further releases */
    /* Pin mux configuration of Dio Pin 355 */
    regVal = 0x08050007U;
    CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE +
        CSL_WKUP_CTRL_MMR_CFG0_PADCONFIG48, regVal);
    /* Set Pin direction to output */
    regVal = CSL_REG32_RD(CSL_WKUP_GPIO0_BASE + 0x10U);
    regVal &= (~(1U << 0x0U));
    CSL_REG32_WR(CSL_WKUP_GPIO0_BASE + 0x10U, regVal);
    /* Drive Pin to High */
    Dio_WriteChannel(CAN_TRCV_MCU_MCAN_0, STD_HIGH);
     /* Read Pin level */
    dioPinLevel[1U] = Dio_ReadChannel(CAN_TRCV_MCU_MCAN_0);
    /*Read back the pin levels to ensure transceiver is enabled*/
    if (STD_HIGH != dioPinLevel[1U])
    {
        AppUtils_Printf(MSG_NORMAL,
        "CAN_APP: Error in Enabling CAN Transceiver MCU MCAN0!!!\n");
    }
    else
    {
        regVal = 0x08050007U;
        CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE +
            CSL_WKUP_CTRL_MMR_CFG0_PADCONFIG42, regVal);
        /* Set Pin direction to output */
        regVal = CSL_REG32_RD(CSL_WKUP_GPIO0_BASE + 0x60U);
        regVal &= (~(1U << 0x05U));
        CSL_REG32_WR(CSL_WKUP_GPIO0_BASE + 0x60U, regVal);
        /* Drive Pin to High */
        Dio_WriteChannel(69U, STD_HIGH);
         /* Read Pin level */
        dioPinLevel[1U] = Dio_ReadChannel(69U);
        /*Read back the pin levels to ensure transceiver is enabled*/
        if (STD_HIGH != dioPinLevel[1U])
        {
            AppUtils_Printf(MSG_NORMAL,
            "CAN_APP: Error in Enabling CAN Transceiver MCU MCAN0!!!\n");
        }
        else
        {
#if (CAN_INITIAL_PRINT_DISABLE_BEFORE_CAN_RESPONSE == STD_OFF)
            AppUtils_Printf(MSG_NORMAL,
            "CAN_APP: Successfully Enabled CAN Transceiver MCU MCAN0!!!\n");
#endif
        }
    }

    /* Take MCAN transceiver out of STB mode for MCU MCAN1 */
    /* Below code will be replaced by Port module in further releases */
    /* Pin mux configuration of Dio Pin 2 */
    regVal = 0x08050007U;
    CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE +
        CSL_WKUP_CTRL_MMR_CFG0_PADCONFIG50, regVal);
    /* Set Pin direction to output */
    regVal = CSL_REG32_RD(CSL_WKUP_GPIO0_BASE + 0x10U);
    regVal &= (~(1U << 0x2U));
    CSL_REG32_WR(CSL_WKUP_GPIO0_BASE + 0x10U, regVal);
    /* Drive Pin to Low */
    Dio_WriteChannel(CAN_TRCV_MCU_MCAN_1, STD_LOW);
     /* Read Pin level */
    dioPinLevel[1U] = Dio_ReadChannel(CAN_TRCV_MCU_MCAN_1);
    /*Read back the pin levels to ensure transceiver is enabled*/
    if (STD_LOW != dioPinLevel[1U])
    {
        AppUtils_Printf(MSG_NORMAL,
        "CAN_APP: Error in Enabling CAN Transceiver MCU MCAN1!!!\n");
    }
    else
    {
#if (CAN_INITIAL_PRINT_DISABLE_BEFORE_CAN_RESPONSE == STD_OFF)
        AppUtils_Printf(MSG_NORMAL,
        "CAN_APP: Successfully Enabled CAN Transceiver MCU MCAN1!!!\n");
#endif
    }
#elif defined (SOC_J784S4)
    /* Take MCAN transceiver out of STB mode for MCU MCAN0 */
    /* Below code will be replaced by Port module in further releases */
    /* Pin mux configuration of Dio Pin 355 */
    regVal = 0x08050007U;
    CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE +
        CSL_WKUP_CTRL_MMR_CFG0_PADCONFIG48, regVal);
    /* Set Pin direction to output */
    regVal = CSL_REG32_RD(CSL_WKUP_GPIO0_BASE + 0x10U);
    regVal &= (~(1U << 0x0U));
    CSL_REG32_WR(CSL_WKUP_GPIO0_BASE + 0x10U, regVal);
    /* Drive Pin to High */
    Dio_WriteChannel(CAN_TRCV_MCU_MCAN_0, STD_HIGH);
     /* Read Pin level */
    dioPinLevel[1U] = Dio_ReadChannel(CAN_TRCV_MCU_MCAN_0);
    /*Read back the pin levels to ensure transceiver is enabled*/
    if (STD_HIGH != dioPinLevel[1U])
    {
        AppUtils_Printf(MSG_NORMAL,
        "CAN_APP: Error in Enabling CAN Transceiver MCU MCAN0!!!\n");
    }
    else
    {
        regVal = 0x08050007U;
        CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE +
            CSL_WKUP_CTRL_MMR_CFG0_PADCONFIG42, regVal);
        /* Set Pin direction to output */
        regVal = CSL_REG32_RD(CSL_WKUP_GPIO0_BASE + 0x60U);
        regVal &= (~(1U << 0x05U));
        CSL_REG32_WR(CSL_WKUP_GPIO0_BASE + 0x60U, regVal);
        /* Drive Pin to High */
        Dio_WriteChannel(69U, STD_LOW);
         /* Read Pin level */
        dioPinLevel[1U] = Dio_ReadChannel(69U);
        /*Read back the pin levels to ensure transceiver is enabled*/
        if (STD_LOW != dioPinLevel[1U])
        {
            AppUtils_Printf(MSG_NORMAL,
            "CAN_APP: Error in Enabling CAN Transceiver MCU MCAN0!!!\n");
        }
        else
        {
#if (CAN_INITIAL_PRINT_DISABLE_BEFORE_CAN_RESPONSE == STD_OFF)
            AppUtils_Printf(MSG_NORMAL,
            "CAN_APP: Successfully Enabled CAN Transceiver MCU MCAN0!!!\n");
#endif
        }
    }
#endif
}
