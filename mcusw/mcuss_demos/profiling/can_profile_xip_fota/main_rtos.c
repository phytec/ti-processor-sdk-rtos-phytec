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
#include <ti/csl/csl_rat.h>

#include <ti/osal/osal.h>
#include <ti/osal/TaskP.h>
#include <ti/osal/src/nonos/Nonos_config.h>
#include <ti/board/board.h>

#include "CanApp_Startup.h"
#include "can_profile.h"
#include "fota_monitor.h"
#include "fota_writer.h"
#include "CanIf.h"

/* UART Header files */
#include <ti/drv/uart/UART.h>
#include <ti/drv/uart/UART_stdio.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Test application stack size */
#define APP_TASK_STACK                  (10U * 1024U)
/**< Stack required for the stack */
#define CAN_PROFILE_DEMO_TASK_NAME      ("CAN PROFILE")
/**< Task name */

/** \brief WKUP Domain CTRL base address + offset to beginning of PAD CONFIG
    section */
#define CAN_XIP_WKUP_PMUX_CTRL	(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x1C000U)
/** \brief MAIN Domain CTRL base address + offset to beginning of PAD CONFIG
    section */
#define CAN_XIP_MAIN_PMUX_CTRL	(CSL_CTRL_MMR0_CFG0_BASE + 0x1C000U)

#if defined (SOC_J721E)
#if defined (BUILD_MCU1_0)
/** \brief MCU_UART_TXD pad offset */
#define CAN_XIP_PIN_MCU_UART_TXD   (0x00E0U)
/** \brief MCU_UART_RXD pad offset */
#define CAN_XIP_PIN_MCU_UART_RXD   (0x00E4U)
/** \brief MCU_UART_CTS pad offset */
#define CAN_XIP_PIN_MCU_UART_CTS   (0x00E8U)
/** \brief MCU_UART_RTS pad offset */
#define CAN_XIP_PIN_MCU_UART_RTS   (0x00ECU)

#elif defined (BUILD_MCU2_1)

/** \brief MAIN_UART_RXD pad offset */
#define CAN_XIP_PIN_UART0_RXD     (0x1E8U)
/** \brief MAIN_UART_TXD pad offset */
#define CAN_XIP_PIN_UART0_TXD     (0x1ECU)
/** \brief MAIN_UART_CTS pad offset */
#define CAN_XIP_PIN_UART0_CTSN    (0x1F0U)
/** \brief MAIN_UART_RTS pad offset */
#define CAN_XIP_PIN_UART0_RTSN    (0x1F4U)

#endif /* (BUILD_MCU2_1) */

#elif defined (SOC_J7200)
#if defined (BUILD_MCU1_0)
/** \brief MCU_UART_TXD pad offset */
#define CAN_XIP_PIN_MCU_UART_TXD   (0x00F0U)
/** \brief MCU_UART_RXD pad offset */
#define CAN_XIP_PIN_MCU_UART_RXD   (0x00F4U)
/** \brief MCU_UART_CTS pad offset */
#define CAN_XIP_PIN_MCU_UART_CTS   (0x00F8U)
/** \brief MCU_UART_RTS pad offset */
#define CAN_XIP_PIN_MCU_UART_RTS   (0x00FCU)

#elif defined (BUILD_MCU2_1)

/** \brief MAIN_UART_RXD pad offset */
#define CAN_XIP_PIN_UART0_RXD     (0x0B0U)
/** \brief MAIN_UART_TXD pad offset */
#define CAN_XIP_PIN_UART0_TXD     (0x0B4U)
/** \brief MAIN_UART_CTS pad offset */
#define CAN_XIP_PIN_UART0_CTSN    (0x0C0U)
/** \brief MAIN_UART_RTS pad offset */
#define CAN_XIP_PIN_UART0_RTSN    (0x0C4U)

#endif /* (BUILD_MCU2_1) */
#endif /* (SOC_J7200) */

/** \brief Pin Mode Enable */
#define CAN_XIP_PIN_MODE(mode)	   (mode)

/** \brief Active mode configurations */
/** \brief Resistor disable */
#define CAN_XIP_PIN_PULL_DISABLE                (0x1U << 16U)
/** \brief Pull direction */
#define	CAN_XIP_PIN_PULL_DIRECTION              (0x1U << 17U)
/** \brief Receiver enable */
#define	CAN_XIP_PIN_INPUT_ENABLE                (0x1U << 18U)
/** \brief Driver disable */
#define	CAN_XIP_PIN_OUTPUT_DISABLE              (0x1U << 21U)

#define UART0_ADDR  ((int)0x40A00000)

#define UART_RHR    ((int)0x00U)
#define UART_LSR    ((int)0x14U)

#define UART_RHR_REG_ADDR   ((volatile unsigned int *)(UART0_ADDR + UART_RHR))
#define UART_LSR_REG_ADDR   ((volatile unsigned int *)(UART0_ADDR + UART_LSR))

#if defined(SOC_J721E)
#define RAT_CFG_BASE_ADDR CSL_MCU_ARMSS_RAT_CFG_BASE
#else
#define RAT_CFG_BASE_ADDR CSL_MCU_R5FSS0_RAT_CFG_BASE
#endif

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static void MainApp_TaskFxn(void* a0, void* a1);
static void CanApp_TaskFxn(void* a0, void* a1);
static void MonitorApp_TaskFxn(void* a0, void* a1) __attribute__((section(".fota_writer_app")));
static void WriterApp_TaskFxn(void* a0, void* a1) __attribute__((section(".fota_writer_app")));
static void CanApp_Shutdown(void);
static void AppUtils_XIP_Init(void);
Board_STATUS Board_uartStdioInit(void);
void sbl_putc(unsigned char c) __attribute__((section(".fota_writer_app")));
void sbl_puts(char *str) __attribute__((section(".fota_writer_app")));
void sbl_putsNum(char *str, uint32_t num) __attribute__((section(".fota_writer_app")));
extern void xipRatConf(void) __attribute__((section(".fota_writer_app")));
extern void myExceptionHandler(void) __attribute__((section(".fota_writer_app")));
void updateImageCopyIn(void) __attribute__((section(".fota_writer_app")));

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
/* application stack */
static uint8_t MainApp_TaskStack[APP_TASK_STACK] __attribute__((aligned(32)));
static uint8_t CanApp_TaskStack[APP_TASK_STACK] __attribute__((aligned(32)));
static uint8_t WriterApp_TaskStack[APP_TASK_STACK] __attribute__((aligned(32)));
static uint8_t MonitorApp_TaskStack[APP_TASK_STACK] __attribute__((aligned(32)));
/**< Stack for the task */
static canAppTaskObj_t  CanApp_TestPrms;
/**< Test parameters */
static HwiP_Handle      CanApp_IsrHndls[CAN_MAX_CONTROLLER];
/**< Stores the ISR handles */
extern SemaphoreP_Handle CanIf_TxConfirmationSemaphore;
/**< TX Confirmation semaphore, would be posted when TX completes */
extern SemaphoreP_Handle CanIf_RxConfirmationSemaphore;
/**< Rx Confirmation semaphore, would be posted when TX completes */
uint32_t updateImageOffset;
/**< Update Imgae Offset in Flash */
TaskP_Handle gMainTask, gCanTask, gMonitorTask, gWriterTask;
/**< Task handles */
SemaphoreP_Handle WriterApp_WriteCompletionSemaphore;
/**< Write completion semaphore handle */
SemaphoreP_Handle WriterApp_FlashRequestSemaphore;
/**< Flash request semaphore handle */
SemaphoreP_Handle WriterApp_FlashAccessSemaphore;
/**< Flash access semaphore handle */
SemaphoreP_Handle WriterApp_ChunkWriteCompleteSemaphore;
/**< Chunk write complete semaphore handle */
SemaphoreP_Handle MonitorApp_ShutdownRequestSemaphore;
/**< Shutdown request semaphore handle */
SemaphoreP_Handle MonitorApp_InitCompleteSemaphore;
/**< Initialization completion semaphore handle */
uint8_t *dataBufAppimage = (uint8_t*)0x80000000;
uint8_t *dataBufAppimageXip = (uint8_t*)0x81000000;
uint8_t *chkBuf = (uint8_t*)0x82000000;
/**< Buffers in DDR containing update image */
/* ========================================================================== */
/*                            External Variables                              */
/* ========================================================================== */
extern uint32 CanIf_DrvStatus;

/**< CAN IF Driver Status, defined in CanIf.c */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/** \brief Puts a character on UART terminal */
void sbl_putc(unsigned char c)
{
    while((*UART_LSR_REG_ADDR & 0x20) == 0);
    *UART_RHR_REG_ADDR = c;
}

/** \brief Puts a string on UART terminal */
void sbl_puts(char *str)
{
    for (; *str != '\0'; str++ )
        sbl_putc(*str);
}

/** \brief Puts a string and a number on UART terminal */
void sbl_putsNum(char *str, uint32_t num)
{
    static char printBuf[200U];
    snprintf(printBuf, 200U, str, num);

    sbl_puts(printBuf);

}

/** \brief Does the necessary RAT Config for P2 image */
extern void xipRatConf(void)
{
    uint32_t xipSectionsOffset;
    uint32_t *bkRegionLocation;
    CSL_RatTranslationCfgInfo translationCfg;

#ifdef BUILD_P1
    sbl_puts("\rIn xipRatConf function for P1 image!\n");
#else
    sbl_puts("\rIn xipRatConf function for P2 image!\n");
#endif

    bkRegionLocation = (uint32_t*)(OSPI_FLASH_DATA_BASE_ADDR + BK_REGION_OFFSET);
    xipSectionsOffset = *bkRegionLocation;

    /* Check if it is a valid application location */
    if(xipSectionsOffset == (OSPI_FLASH_DATA_BASE_ADDR + P1_REGION_OFFSET))
    {
        /* do nothing */
    }
    else if(xipSectionsOffset == (OSPI_FLASH_DATA_BASE_ADDR + P2_REGION_OFFSET))
    {
        /* do nothing */
    }
    else
    {
        /* Set the application location as the default location */
        xipSectionsOffset = OSPI_FLASH_DATA_BASE_ADDR + P1_REGION_OFFSET;
    }

    if(xipSectionsOffset == (OSPI_FLASH_DATA_BASE_ADDR + P2_REGION_OFFSET))
    {
        /* Add RAT configuration to access address > 32bit address range */
        translationCfg.translatedAddress    = OSPI_FLASH_DATA_BASE_ADDR + P2_REGION_OFFSET;
        translationCfg.sizeInBytes          = P2_ARTIFICIAL_OFFSET;
        translationCfg.baseAddress          = (uint32_t)(OSPI_FLASH_DATA_BASE_ADDR + P1_REGION_OFFSET);

        /* Set up RAT translation */
        bool result = CSL_ratConfigRegionTranslation((CSL_ratRegs *)RAT_CFG_BASE_ADDR,
                                                    0U,
                                                    &translationCfg);
        if (result == false)
        {
            sbl_puts("Selector App: CSL_ratConfigRegionTranslation Failed! \n");
        }
        else
        {
            sbl_puts("Selector App: CSL_ratConfigRegionTranslation Successful! \n");
        }
    }
}

#ifdef BUILD_FREERTOS
void _system_post_cinit(void)
{
    osalArch_Config_t cfg;

    cfg.disableIrqOnInit = true;
    osalArch_Init(&cfg);

    xipRatConf();
}
#endif

/** \brief Calls XIP init and starts the bios */
int main(void)
{
    TaskP_Params taskParams;

    OS_init();

    /* Configure MCU UART0 and MAIN UART0 module for serial console prints. */
    AppUtils_XIP_Init();

#ifdef BUILD_P1
    sbl_puts("\rP1 program started! In main function!\n");
#else
    sbl_puts("\rP2 program started! In main function!\n");
#endif

    /* Initialize the task params */
    TaskP_Params_init(&taskParams);
    /* Set the task priority higher than the default priority (1) */
    taskParams.priority     = 15;
    taskParams.stack        = MainApp_TaskStack;
    taskParams.stacksize    = sizeof (MainApp_TaskStack);

    gMainTask = TaskP_create(&MainApp_TaskFxn, &taskParams);
    if(NULL == gMainTask)
    {
        OS_stop();
    }
    OS_start();    /* does not return */

    return(0);
}

/** \brief Starts the three application tasks - CAN, Writer and Monitor */
static void MainApp_TaskFxn(void* a0, void* a1)
{
    TaskP_Params taskParams;
    SemaphoreP_Params semParams;
    Board_initCfg boardCfg = BOARD_INIT_UART_STDIO;

    sbl_puts("\rIn main task function. Calling Board_init\n");

    int status = Board_init(boardCfg);
    if (status != BOARD_SOK) 
    {
        sbl_putsNum("\rError: Board_init failed: error %d\n", status);
    }
    else
    {
        sbl_putsNum("\rSuccess! Board_init successful: status is: %d\n", status);
    }
    UART_printf("\rUART_STDIO is enabled\n");

    CanApp_Startup();
    CanApp_PowerAndClkSrc();
    CanApp_PlatformInit();

    /* Initialize dummy CAN IF */
    CanIf_Init(NULL);

    if (CANIF_DRV_INITIALIZED == CanIf_DrvStatus)
    {
        /* Initialize the task params */
        TaskP_Params_init(&taskParams);
        taskParams.name = CAN_PROFILE_DEMO_TASK_NAME;
        /* Set the task priority higher than the default priority (1) */
        taskParams.priority     = 4;
        taskParams.stack        = CanApp_TaskStack;
        taskParams.stacksize    = sizeof (CanApp_TaskStack);
        
        gCanTask = TaskP_create(&CanApp_TaskFxn, &taskParams);

        if(NULL == gCanTask)
        {
            UART_printf("\rError: CAN task creation failed!\n");
            OS_stop();
        }
    }
    else
    {
        AppUtils_Printf(MSG_NORMAL, MSG_APP_NAME
                " Error : CAN IF not initialized !!!\n");
    }

    SemaphoreP_Params_init(&semParams);
    semParams.mode = SemaphoreP_Mode_BINARY;
    MonitorApp_ShutdownRequestSemaphore = SemaphoreP_create(0U, &semParams);

    SemaphoreP_Params_init(&semParams);
    semParams.mode = SemaphoreP_Mode_BINARY;
    MonitorApp_InitCompleteSemaphore = SemaphoreP_create(0U, &semParams);

    /* Initialize the task params */
    TaskP_Params_init(&taskParams);
    /* Set the task priority higher than the default priority (1) */
    taskParams.priority     = 3;
    taskParams.stack        = MonitorApp_TaskStack;
    taskParams.stacksize    = sizeof (MonitorApp_TaskStack);
    /* Create the task */
    gMonitorTask = TaskP_create(&MonitorApp_TaskFxn, &taskParams);

    if(NULL == gMonitorTask)
    {
        UART_printf("\rError: monitor task creation failed!\n");
        OS_stop();
    }

    /* Initialize the task params */
    TaskP_Params_init(&taskParams);
    /* Set the task priority as 2 */
    taskParams.priority     = 2;
    taskParams.stack        = WriterApp_TaskStack;
    taskParams.stacksize    = sizeof (WriterApp_TaskStack);
    /* Create the task */
    gWriterTask = TaskP_create(&WriterApp_TaskFxn, &taskParams);

    if(NULL == gWriterTask)
    {
        UART_printf("\rError: writer task creation failed!\n");
        OS_stop();
    }

    return;
}

/** \brief CAN Profiling App Task Function */
static void CanApp_TaskFxn(void* a0, void* a1)
{
    Utils_prfInit();

    Utils_prfLoadRegister (TaskP_self(), CAN_PROFILE_DEMO_TASK_NAME);

    CanApp_TestPrms.numTxPackets = APP_NUM_MSG_PER_ITERATION;
    CanApp_TestPrms.numIterations = APP_NUM_ITERATION;

    TaskP_yield();

    CanApp_ProfileTest(&CanApp_TestPrms);

    CanApp_Shutdown();

    Utils_prfLoadUnRegister (TaskP_self());
    Utils_prfDeInit();
    return;
}

/** \brief Application tear down functions */
static void CanApp_Shutdown(void)
{
    uint32 idx;

    for (idx = 0U; idx < CAN_MAX_CONTROLLER; idx++)
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

/** \brief Monitor App Task Function */
static void MonitorApp_TaskFxn(void* a0, void* a1)
{
    uint32_t retVal;

    /* Authenticate the update image */
    MonitorApp_AuthenticateImage();

    /* Initialize the writer app */
    if(MonitorApp_InitWriter(&updateImageOffset))
    {
        UART_printf("\rMonitor App: Init Writer failed! Writer Task not initialized!\n");
    }

    /* Signal the initialization completion semaphore */
    SemaphoreP_post(MonitorApp_InitCompleteSemaphore);

    UART_printf("\rMonitor App: Monitoring the semaphore from Writer App\n");

    retVal = MonitorApp_IsUpdateComplete();

    if(retVal == SemaphoreP_OK)
    {
        UART_printf("\rMonitor App: Update image write complete!\n");

        /* Initiate a shutdown */
        MonitorApp_Shutdown();

        /* Update the RAT to boot from update location and issue a warm reset */
        if(MonitorApp_UpdateApplicationLocation(updateImageOffset))
        {
            UART_printf("\rMonitor App: Reset failed!\n");
        }
    }
    else
    {
        UART_printf("\rMonitor App: Update image failed!\n");
    }
}

/** \brief Writer App Task Function */
static void WriterApp_TaskFxn(void* a0, void* a1)
{
    /* Wait for Monitor task to  */
    SemaphoreP_pend(MonitorApp_InitCompleteSemaphore, SemaphoreP_WAIT_FOREVER);

    WriterApp_Test(updateImageOffset);
}

/** \brief Application local exception handler */
extern void myExceptionHandler(void)
{
    volatile int loop;
    loop = 1;
    while(loop)
    {
        /* Trap CPU */
    }
}

/** \brief Copies the update image form P3 to DDR */
void updateImageCopyIn(void)
{
    uint32_t imageLen;
    uint8_t *dst;
    uint8_t *src;

    dst = (uint8_t*)dataBufAppimage;
    src = (uint8_t*)(OSPI_FLASH_DATA_BASE_ADDR + P3_REGION_OFFSET_APPIMAGE);
    imageLen = UPDATE_APPIMAGE_SIZE;
    while (imageLen--)
    {
        *dst++ = *src++;
    }

    dst = (uint8_t*)dataBufAppimageXip;
    src = (uint8_t*)(OSPI_FLASH_DATA_BASE_ADDR + P3_REGION_OFFSET_APPIMAGE_XIP);
    imageLen = UPDATE_APPIMAGE_XIP_SIZE;
    while (imageLen--)
    {
        *dst++ = *src++;
    }
}

/** \brief Performs copy in and uart init */
static void AppUtils_XIP_Init(void)
{
    uint32 regVal;

    /* Copy the update image to DDR */
    updateImageCopyIn();

    /* Configure MCU UART0 and MAIN UART0 module for serial console prints. */
    Board_uartStdioInit();

    /* Unlock lock key registers for Partition 7: IO PAD
       configuration registers in WKUP_CTRL_MMR */
    /* write Partition 7 Lock Key 0 Register */
    CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE +
        CSL_WKUP_CTRL_MMR_CFG0_LOCK7_KICK0, 0x68EF3490U);
    /* write Partition 7 Lock Key 1 Register */
    CSL_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE +
        CSL_WKUP_CTRL_MMR_CFG0_LOCK7_KICK1, 0xD172BC5AU);
    /* Check for unlock */
    regVal = CSL_REG32_RD(CSL_WKUP_CTRL_MMR0_CFG0_BASE +
        CSL_WKUP_CTRL_MMR_CFG0_LOCK7_KICK0);
    while ((regVal & 0x1U) != 0x1U)
    {
        regVal = CSL_REG32_RD(CSL_WKUP_CTRL_MMR0_CFG0_BASE +
            CSL_WKUP_CTRL_MMR_CFG0_LOCK7_KICK0);
    }

    /* Below code will be replaced by Port module in further releases */
    /* UART Pin Mux */
#if defined (SOC_J721E)

#if defined (BUILD_MCU1_0)
    /* MCU_UART0 -> MCU_UART0_RXD */
    regVal = CAN_XIP_PIN_MODE(0U) | ((CAN_XIP_PIN_PULL_DISABLE | CAN_XIP_PIN_INPUT_ENABLE) &
            (~CAN_XIP_PIN_PULL_DIRECTION));
    CSL_REG32_WR(CAN_XIP_WKUP_PMUX_CTRL + CAN_XIP_PIN_MCU_UART_RXD, regVal);
    /* MCU_UART0 -> MCU_UART0_TXD */
    regVal = CAN_XIP_PIN_MODE(0U) | ((CAN_XIP_PIN_PULL_DISABLE) &
            (~CAN_XIP_PIN_PULL_DIRECTION & ~CAN_XIP_PIN_INPUT_ENABLE));
    CSL_REG32_WR(CAN_XIP_WKUP_PMUX_CTRL + CAN_XIP_PIN_MCU_UART_TXD, regVal);
    /* MCU_UART0 -> MCU_UART0_CTS */
    regVal = CAN_XIP_PIN_MODE(0U) | ((CAN_XIP_PIN_PULL_DISABLE | CAN_XIP_PIN_INPUT_ENABLE) &
            (~CAN_XIP_PIN_PULL_DIRECTION));
    CSL_REG32_WR(CAN_XIP_WKUP_PMUX_CTRL + CAN_XIP_PIN_MCU_UART_CTS, regVal);
    /* MCU_UART0 -> MCU_UART0_RTS */
    regVal = CAN_XIP_PIN_MODE(0U) | ((CAN_XIP_PIN_PULL_DISABLE | CAN_XIP_PIN_INPUT_ENABLE) &
            (~CAN_XIP_PIN_PULL_DIRECTION));
    CSL_REG32_WR(CAN_XIP_WKUP_PMUX_CTRL + CAN_XIP_PIN_MCU_UART_RTS, regVal);

#endif /* (BUILD_MCU1_0) */

#elif defined (SOC_J7200)

#if defined (BUILD_MCU1_0)
    /* MCU_UART0 -> MCU_UART0_RXD */
    regVal = CAN_XIP_PIN_MODE(0U) | ((CAN_XIP_PIN_PULL_DISABLE | CAN_XIP_PIN_INPUT_ENABLE) &
            (~CAN_XIP_PIN_PULL_DIRECTION));
    CSL_REG32_WR(CAN_XIP_WKUP_PMUX_CTRL + CAN_XIP_PIN_MCU_UART_RXD, regVal);
    /* MCU_UART0 -> MCU_UART0_TXD */
    regVal = CAN_XIP_PIN_MODE(0U) | ((CAN_XIP_PIN_PULL_DISABLE) &
            (~CAN_XIP_PIN_PULL_DIRECTION & ~CAN_XIP_PIN_INPUT_ENABLE));
    CSL_REG32_WR(CAN_XIP_WKUP_PMUX_CTRL + CAN_XIP_PIN_MCU_UART_TXD, regVal);
    /* MCU_UART0 -> MCU_UART0_CTS */
    regVal = CAN_XIP_PIN_MODE(0U) | ((CAN_XIP_PIN_PULL_DISABLE | CAN_XIP_PIN_INPUT_ENABLE) &
            (~CAN_XIP_PIN_PULL_DIRECTION));
    CSL_REG32_WR(CAN_XIP_WKUP_PMUX_CTRL + CAN_XIP_PIN_MCU_UART_CTS, regVal);
    /* MCU_UART0 -> MCU_UART0_RTS */
    regVal = CAN_XIP_PIN_MODE(0U) | ((CAN_XIP_PIN_PULL_DISABLE | CAN_XIP_PIN_INPUT_ENABLE) &
            (~CAN_XIP_PIN_PULL_DIRECTION));
    CSL_REG32_WR(CAN_XIP_WKUP_PMUX_CTRL + CAN_XIP_PIN_MCU_UART_RTS, regVal);

#endif /* (BUILD_MCU2_1) */

#endif /* (SOC_J7200) */

    return;
}
