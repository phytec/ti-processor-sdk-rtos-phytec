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
 *  \file     DioApp.c
 *
 *  \brief    This file contains the DIO test example
 */

/*******************************************************************************
 *  INCLUDES
 ******************************************************************************/
/* Include Files */
#include "stdio.h"
#include "Det.h"
#include "Dem.h"

#include "Dio.h"
#include "SchM_Dio.h"
#include "EcuM_Cbk.h"
#include "app_utils.h"

#include <hw_include/soc.h>
#include <hw_include/lld_gpio.h>
#include <hw_include/arch/lld_arch.h>

#if defined (SOC_AM62X)
#include <examples/Dio/soc/am62x/Generated_Dpl/ti_drivers_config.h>
#include <examples/Dio/soc/am62x/Generated_Dpl/ti_board_config.h>
#endif
#if defined (SOC_AM62AX)
#include <examples/Dio/soc/am62ax/Generated_Dpl/ti_drivers_config.h>
#include <examples/Dio/soc/am62ax/Generated_Dpl/ti_board_config.h>
#endif
#if defined (SOC_AM62PX)
#include <examples/Dio/soc/am62px/Generated_Dpl/ti_drivers_config.h>
#include <examples/Dio/soc/am62px/Generated_Dpl/ti_board_config.h>
#endif
#if defined (SOC_J722S)
#include <examples/Dio/soc/j722s/Generated_Dpl/ti_drivers_config.h>
#include <examples/Dio/soc/j722s/Generated_Dpl/ti_board_config.h>
#endif
#if defined (SOC_AM62X) || defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
#include<kernel/dpl/ClockP.h>
#endif
/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* ========================================================================== */
/*                 Internal Function Declarations                             */
/* ========================================================================== */
static void DioApp_Startup(void);
static void DioApp_McuDomainPinMux(uint32 offset, uint32 muxmode);
#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_AM62AX) || defined (SOC_AM62X)  || defined (SOC_AM62PX) || defined (SOC_J722S)
static void DioApp_MainDomainPinMux(uint32 offset, uint32 muxmode);
#endif
static void DioApp_unlockMmrRegisters(uint32 lockKey0, uint32 lockKey1);
static void DioApp_mainTest(void);
static void DioApp_PowerAndClkSrc(void);
#if defined (SOC_AM62X) || defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
int32_t Dio_delay(uint32_t nTicks);
#endif
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
uint32                          DIO_TestPassed = E_OK;
/*Global to inidicate status of APP */
#if (STD_ON == DIO_VERSION_INFO_API)
Std_VersionInfoType       DIO_VersionInfo;
#endif

#if defined (SOC_J721E)
#if defined (BUILD_MCU1_0)
const CSL_ArmR5MpuRegionCfg gCslR5MpuCfg[CSL_ARM_R5F_MPU_REGIONS_MAX]
__attribute__((section(".CDD_IPC_MPU_CFG_OCMRAM"))) =
#else /* MCU 1 0 */
/* const CSL_ArmR5MpuRegionCfg gCslR5MpuCfg[CSL_ARM_R5F_MPU_REGIONS_MAX] __attribute__ ((section(".CDD_IPC_MPU_CFG_OCMRAM"))) =  No support for other
    cores/compilers at this point */
const CSL_ArmR5MpuRegionCfg gCslR5MpuCfg[CSL_ARM_R5F_MPU_REGIONS_MAX] =
#endif /* MCU 1 0 */
{
    {
        /* Region 0 configuration: complete 32 bit address space = 4Gbits */
        .regionId         = 0U,
        .enable           = 1U,
        .baseAddr         = 0x0U,
        .size             = CSL_ARM_R5_MPU_REGION_SIZE_4GB,
        .subRegionEnable  = CSL_ARM_R5_MPU_SUB_REGION_ENABLE_ALL,
        .exeNeverControl  = 1U,
        .accessPermission = CSL_ARM_R5_ACC_PERM_PRIV_USR_RD_WR,
        .shareable        = 0U,
        .cacheable        = (uint32_t)FALSE,
        .cachePolicy      = 0U,
        .memAttr          = 0U,
    },
    {
        /* Region 1 configuration: 128 bytes memory for exception vector execution */
        .regionId         = 1U,
        .enable           = 1U,
        .baseAddr         = 0x0U,
        .size             = CSL_ARM_R5_MPU_REGION_SIZE_128B,
        .subRegionEnable  = CSL_ARM_R5_MPU_SUB_REGION_ENABLE_ALL,
        .exeNeverControl  = 0U,
        .accessPermission = CSL_ARM_R5_ACC_PERM_PRIV_USR_RD_WR,
        .shareable        = 0U,
        .cacheable        = (uint32_t)TRUE,
        .cachePolicy      = CSL_ARM_R5_CACHE_POLICY_WB_WA,
        .memAttr          = 0U,
    },
    {
        /* Region 2 configuration: 512 KB OCMS RAM */
        .regionId         = 2U,
        .enable           = 1U,
        .baseAddr         = 0x41C00000,
        .size             = CSL_ARM_R5_MPU_REGION_SIZE_512KB,
        .subRegionEnable  = CSL_ARM_R5_MPU_SUB_REGION_ENABLE_ALL,
        .exeNeverControl  = 0U,
        .accessPermission = CSL_ARM_R5_ACC_PERM_PRIV_USR_RD_WR,
        .shareable        = 0U,
        .cacheable        = (uint32_t)TRUE,
        .cachePolicy      = CSL_ARM_R5_CACHE_POLICY_WB_WA,
        .memAttr          = 0U,
    },
    {
        /* Region 3 configuration: 2 MB MCMS3 RAM */
        .regionId         = 3U,
        .enable           = 1U,
        .baseAddr         = 0x70000000,
        .size             = CSL_ARM_R5_MPU_REGION_SIZE_8MB,
        .subRegionEnable  = CSL_ARM_R5_MPU_SUB_REGION_ENABLE_ALL,
        .exeNeverControl  = 0U,
        .accessPermission = CSL_ARM_R5_ACC_PERM_PRIV_USR_RD_WR,
        .shareable        = 0U,
        .cacheable        = (uint32_t)TRUE,
        .cachePolicy      = CSL_ARM_R5_CACHE_POLICY_WB_WA,
        .memAttr          = 0U,
    },
    {
        /* Region 4 configuration: 2 GB DDR RAM */
        .regionId         = 4U,
        .enable           = 1U,
        .baseAddr         = 0x80000000,
        .size             = CSL_ARM_R5_MPU_REGION_SIZE_2GB,
        .subRegionEnable  = CSL_ARM_R5_MPU_SUB_REGION_ENABLE_ALL,
        .exeNeverControl  = 0U,
        .accessPermission = CSL_ARM_R5_ACC_PERM_PRIV_USR_RD_WR,
        .shareable        = 0U,
        .cacheable        = (uint32_t)TRUE,
        .cachePolicy      = CSL_ARM_R5_CACHE_POLICY_WB_WA,
        .memAttr          = 0U,
    },
    {
        /* Region 5 configuration: 64 KB BTCM */
        .regionId         = 5U,
        .enable           = 1U,
        .baseAddr         = 0x41010000,
        .size             = CSL_ARM_R5_MPU_REGION_SIZE_64KB,
        .subRegionEnable  = CSL_ARM_R5_MPU_SUB_REGION_ENABLE_ALL,
        .exeNeverControl  = 0U,
        .accessPermission = CSL_ARM_R5_ACC_PERM_PRIV_USR_RD_WR,
        .shareable        = 0U,
        .cacheable        = (uint32_t)TRUE,
        .cachePolicy      = CSL_ARM_R5_CACHE_POLICY_NON_CACHEABLE,
        .memAttr          = 0U,
    },
    {
        /* Region 6 configuration: 128 MB FSS DAT */
        .regionId         = 6U,
        .enable           = 1U,
        .baseAddr         = 0x50000000,
        .size             = CSL_ARM_R5_MPU_REGION_SIZE_128MB,
        .subRegionEnable  = CSL_ARM_R5_MPU_SUB_REGION_ENABLE_ALL,
        .exeNeverControl  = 0U,
        .accessPermission = CSL_ARM_R5_ACC_PERM_PRIV_USR_RD_WR,
        .shareable        = 0U,
        .cacheable        = (uint32_t)TRUE,
        .cachePolicy      = CSL_ARM_R5_CACHE_POLICY_NON_CACHEABLE,
        .memAttr          = 0U,
    },
    {
        /* Region 7 configuration: 2 MB for IPC Shared memory */
        .regionId         = 7U,
        .enable           = 1U,
        .baseAddr         = 0xAA000000,
        .size             = CSL_ARM_R5_MPU_REGION_SIZE_32MB,
        .subRegionEnable  = CSL_ARM_R5_MPU_SUB_REGION_ENABLE_ALL,
        .exeNeverControl  = 0U,
        .accessPermission = CSL_ARM_R5_ACC_PERM_PRIV_USR_RD_WR,
        .shareable        = 1U,
        .cacheable        = (uint32_t)FALSE,
        .cachePolicy      = CSL_ARM_R5_CACHE_POLICY_NON_CACHEABLE,
        .memAttr          = 0U,
    },
};
#endif

/* GPIO Version Information */
#define DIO_START_SEC_CONST_32
#include "Dio_MemMap.h"
static CONST(uint32, DIO_CONST)
    DIO_GPIOBaseAddr[DIO_HW_UNIT_CNT] =
{
#if defined (SOC_J721E)
    (uint32)CSL_WKUP_GPIO0_BASE,
    (uint32)CSL_WKUP_GPIO1_BASE,
    (uint32)CSL_GPIO0_BASE,
    (uint32)CSL_GPIO1_BASE,
    (uint32)CSL_GPIO2_BASE,
    (uint32)CSL_GPIO3_BASE,
    (uint32)CSL_GPIO4_BASE,
    (uint32)CSL_GPIO5_BASE,
    (uint32)CSL_GPIO6_BASE,
    (uint32)CSL_GPIO7_BASE,
#elif defined (SOC_J7200) || defined (SOC_J721S2)
    (uint32)CSL_WKUP_GPIO0_BASE,
    (uint32)CSL_WKUP_GPIO1_BASE,
    (uint32)CSL_GPIO0_BASE,
    (uint32)CSL_GPIO2_BASE,
    (uint32)CSL_GPIO4_BASE,
    (uint32)CSL_GPIO6_BASE,
#elif defined (SOC_J784S4)
    (uint32)CSL_WKUP_GPIO0_BASE,
    (uint32)CSL_WKUP_GPIO1_BASE,
    (uint32)CSL_GPIO0_BASE,
    (uint32)CSL_GPIO2_BASE,
    (uint32)CSL_GPIO4_BASE,
    (uint32)CSL_GPIO6_BASE,
#elif defined (SOC_AM62X) || defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
	(uint32)CSL_MCU_GPIO0_BASE,
    (uint32)CSL_GPIO0_BASE,
    (uint32)CSL_GPIO1_BASE,
#endif
};
/* GPIO Instance Base Addresses */

#define DIO_STOP_SEC_CONST_32
#include "Dio_MemMap.h"

/* ========================================================================== */
/*                 Internal Function Defination                               */
/* ========================================================================== */
#if defined (SOC_AM62X) || defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
int32_t Dio_delay(uint32_t nTicks)
{
    uint64_t sec = (ClockP_ticksToUsec(nTicks)) / 1000000;

    ClockP_sleep((uint32_t)sec);

    return 0;
}
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4) || defined (SOC_AM62X)
/* Same Pin is connected to CAN transceiver in J7 */
#define WKUP_GPIO0_B01_Ch0      CAN_TRCV_MCU_MCAN_0
#elif  defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
#define MCU_GPIO0_B01_Ch0      CAN_TRCV_MCU_MCAN_0
#endif
/*  */
/**  int main(void)
 * \brief Application main function
 *   The user can use this function to implement the application.
 */
int main(void)
{
    DIO_TestPassed = E_OK;
#if defined (SOC_AM62X) || defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
    System_init();
    Board_init();
#endif
#ifdef UART_ENABLED
    AppUtils_Init();
#endif
/*Pin Mux and initialize for Dio Driver*/
    DioApp_Startup();
    DioApp_PowerAndClkSrc();

    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "Sample Application - STARTS !!!\n");
    /*Start Main test case*/
    DioApp_mainTest();

    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "\n----------------------------------\n");
    if (E_OK == DIO_TestPassed)
    {

        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                "\nDioApp: All tests have passed !!!\n");
        AppUtils_LogResult(APP_UTILS_TEST_STATUS_PASS);
    }
    else
    {
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                                        "\nDioApp: Some tests have failed!!!\n");
        AppUtils_LogResult(APP_UTILS_TEST_STATUS_FAIL);
    }

    return (0U);
}


/**
 *\brief   This function is has the app test cases
*/
void DioApp_mainTest()
{
    uint32_least i;
    uint8              DIO_chGrpShift;
    Dio_PortLevelType  DIO_chGrpMask;
    Dio_LevelType      DIO_PinLevel[3U];
#if defined (SOC_AM62X) ||  defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
    Dio_LevelType      DIO_PinLevel2[3U];
    Dio_LevelType      DIO_PinLevel3[3U];
#endif
    Dio_PortLevelType  DIO_PortLevel[3U];
#if (STD_ON == DIO_REGISTER_READBACK_API)
#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_AM62X)
    uint32 readback_init = 0;

#endif

#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_AM62X)
	Std_ReturnType retVal = 0;
    Dio_RegisterReadbackType Dio_RegRb;
	uint32 readback_final = 0;
#endif
#if defined (SOC_J721E) || defined (SOC_J7200)
    uint32 readback_init_gpio = 0 ;
#endif
#endif
#if (STD_ON == DIO_REGISTER_READBACK_API)
#if defined (SOC_J721E) || defined (SOC_J7200)  || defined (SOC_AM62X)
    /* ReadBack API check */
    retVal = Dio_RegisterReadback(WKUP_GPIO0_B01_Ch8, &Dio_RegRb);
    if (retVal != E_NOT_OK)
    {
        readback_init = Dio_RegRb.pid;
    }
#endif
#if defined (SOC_J721E) || defined (SOC_J7200)
    retVal = Dio_RegisterReadback(GPIO0_B01_Ch0, &Dio_RegRb);
    if (retVal != E_NOT_OK)
    {
        readback_init_gpio = Dio_RegRb.pid;
    }
#endif
#endif
    /* Version Information Test */
#if (STD_ON == DIO_VERSION_INFO_API)
    Dio_GetVersionInfo(&DIO_VersionInfo);
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL," \n");
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "DIO MCAL Version Info\n");
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "---------------------\n");
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "Vendor ID           : %d\n",
                    DIO_VersionInfo.vendorID);
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "Module ID           : %d\n",
                    DIO_VersionInfo.moduleID);
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "SW Major Version    : %d\n",
                    DIO_VersionInfo.sw_major_version);
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "SW Minor Version    : %d\n",
                    DIO_VersionInfo.sw_minor_version);
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "SW Patch Version    : %d\n",
                    DIO_VersionInfo.sw_patch_version);
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL," \n");
#endif
    /*
     * 4 Tests are performed / demonstrated
     * Test A. Service API Dio_WriteChannel & Dio_ReadChannel
     * Test B. Service API Dio_WriteChannelGroup & Dio_ReadChannelGroup
     * Test C. Service API Dio_WritePort, Dio_ReadPort & Dio_FlipChannel
     * Test D. Service API Dio_FlipChannel
     * Test E. Service API Dio_RegisterReadback
     */

    /* Test A.
     * 1. Generate Sqaure Wave on channel
       2. Set 2 channel of GPIO 2/GPIO 3 to 0x1 and 0x0 via Dio_WriteChannel
     * 3. Read and confirm the same via Dio_ReadChannel
     */

    /*Square wave generation on GPIO1_B45_Ch25, can be checked on CRO*/
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                     "Test A. Write and Read Channel\n");
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "------------------------------\n");

#if defined (SOC_AM62X) || defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
    /*Write Channel*/
    Dio_WriteChannel(WKUP_GPIO0_B01_Ch8, STD_LOW);
    Dio_WriteChannel(WKUP_GPIO0_B01_Ch0, STD_HIGH);
    Dio_WriteChannel(WKUP_GPIO0_B01_Ch1, STD_LOW);

    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "Channels written\n");
    /*Read Channel*/
    i = 0U;
    DIO_PinLevel[i] = Dio_ReadChannel(WKUP_GPIO0_B01_Ch8);
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "Channel read DIO_PinLevel[%d] = %d \n",
                    i,DIO_PinLevel[i]);
    i++;
    DIO_PinLevel[i] = Dio_ReadChannel(WKUP_GPIO0_B01_Ch0);
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "Channel read DIO_PinLevel[%d] = %d \n",i,DIO_PinLevel[i]);
    i++;
    DIO_PinLevel[i] = Dio_ReadChannel(WKUP_GPIO0_B01_Ch1);
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "Channel read DIO_PinLevel[%d] = %d\n",i,DIO_PinLevel[i]);
    /* Confirmation checks performed */
    if ((DIO_PinLevel[0U] != STD_LOW) || (DIO_PinLevel[1U] != STD_HIGH)||
        ((DIO_PinLevel[2U] != STD_LOW) ))
    {
        DIO_TestPassed = E_NOT_OK;
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "Please check if Read-back is enabled.\n");
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "DIO Service API Read-back Channel fails !!!\n");
    }
    else
    {
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "DIO Service API Read-back Channel Succeeds !!!\n");
    }
#if defined (SOC_AM62X)
    Dio_WriteChannel(Main_GPIO0_Channel_0,STD_HIGH); 
    DIO_PinLevel2[0] = Dio_ReadChannel(Main_GPIO0_Channel_0);

    Dio_WriteChannel(Main_GPIO0_Channel_0,STD_HIGH); 
    DIO_PinLevel2[1] = Dio_ReadChannel(Main_GPIO0_Channel_0);

    Dio_WriteChannel(Main_GPIO0_Channel_0,STD_LOW); 
    DIO_PinLevel2[2] = Dio_ReadChannel(Main_GPIO0_Channel_0);
    if ((DIO_PinLevel2[0U] != STD_HIGH) || (DIO_PinLevel2[1U] != STD_HIGH)||
        ((DIO_PinLevel2[2U] != STD_LOW) ))
    {
        DIO_TestPassed = E_NOT_OK;
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "Please check if Read-back is enabled for Main_GPIO_0.\n");

        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "DIO Service API Read-back Channel fails for Main_GPIO_0 !!!\n");
    }
    else
    {
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "DIO Service API Read-back Channel Succeeds for Main_GPIO_0 !!!\n");
    }
    
    Dio_WriteChannel(Main_GPIO1_Channel_0,STD_HIGH); 
    DIO_PinLevel3[0] = Dio_ReadChannel(Main_GPIO1_Channel_0);

    Dio_WriteChannel(Main_GPIO1_Channel_0,STD_HIGH); 
    DIO_PinLevel3[1] = Dio_ReadChannel(Main_GPIO1_Channel_0);

    Dio_WriteChannel(Main_GPIO1_Channel_0,STD_LOW); 
    DIO_PinLevel3[2] = Dio_ReadChannel(Main_GPIO1_Channel_0);
    if ((DIO_PinLevel3[0U] != STD_HIGH) || (DIO_PinLevel3[1U] != STD_HIGH)||
        ((DIO_PinLevel3[2U] != STD_LOW) ))
    {
        DIO_TestPassed = E_NOT_OK;
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "Please check if Read-back is enabled for Main_GPIO_1.\n");

        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "DIO Service API Read-back Channel fails for Main_GPIO_1 !!!\n");
    }
    else
    {
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "DIO Service API Read-back Channel Succeeds for Main_GPIO_1 !!!\n");
    }
#endif
#endif

#if defined (SOC_J721E) || defined (SOC_J7200)
    Dio_WriteChannel(GPIO0_B01_Ch0, STD_LOW);
    Dio_WriteChannel(GPIO0_B01_Ch1, STD_HIGH);
    Dio_WriteChannel(GPIO0_B01_Ch2, STD_LOW);
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "Main Domain Channels written\n");
    /*Read Channel*/
    i = 0U;
    DIO_PinLevel[i] = Dio_ReadChannel(GPIO0_B01_Ch0);
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "Channel read DIO_PinLevel[%d] = %d \n",
                    i,DIO_PinLevel[i]);
    i++;
    DIO_PinLevel[i] = Dio_ReadChannel(GPIO0_B01_Ch1);
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "Channel read DIO_PinLevel[%d] = %d \n",i,DIO_PinLevel[i]);
    i++;
    DIO_PinLevel[i] = Dio_ReadChannel(GPIO0_B01_Ch2);
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "Channel read DIO_PinLevel[%d] = %d\n",i,DIO_PinLevel[i]);
    /* Confirmation checks performed */
    if ((DIO_PinLevel[0U] != STD_LOW) || (DIO_PinLevel[1U] != STD_HIGH)||
        ((DIO_PinLevel[2U] != STD_LOW) ))
    {
        DIO_TestPassed = E_NOT_OK;
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "Please check if Read-back is enabled.\n");
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                   "DIO Service API Read-back Channel Main Domain fails !!!\n");
    }
    else
    {
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                "DIO Service API Read-back Channel Main Domain Succeeds !!!\n");
    }
#elif  defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
    /*Write Channel*/
    Dio_WriteChannel(MCU_GPIO0_B01_Ch8, STD_LOW);
    Dio_WriteChannel(MCU_GPIO0_B01_Ch0, STD_HIGH);
    Dio_WriteChannel(MCU_GPIO0_B01_Ch1, STD_LOW);

    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "Channels written\n");
    /*Read Channel*/
    i = 0U;
    DIO_PinLevel[i] = Dio_ReadChannel(MCU_GPIO0_B01_Ch8);
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "Channel read DIO_PinLevel[%d] = %d \n",
                    i,DIO_PinLevel[i]);

    i++;
    DIO_PinLevel[i] = Dio_ReadChannel(MCU_GPIO0_B01_Ch0);
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "Channel read DIO_PinLevel[%d] = %d \n",i,DIO_PinLevel[i]);

    i++;
    DIO_PinLevel[i] = Dio_ReadChannel(MCU_GPIO0_B01_Ch1);
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "Channel read DIO_PinLevel[%d] = %d\n",i,DIO_PinLevel[i]);

    /* Confirmation checks performed */
    if ((DIO_PinLevel[0U] != STD_LOW) || (DIO_PinLevel[1U] != STD_HIGH)||
        ((DIO_PinLevel[2U] != STD_LOW) ))
    {
        DIO_TestPassed = E_NOT_OK;
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "Please check if Read-back is enabled.\n");

        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "DIO Service API Read-back Channel fails !!!\n");
    }
    else
    {
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "DIO Service API Read-back Channel Succeeds !!!\n");
    }


    Dio_WriteChannel(Main_GPIO0_Channel_0,STD_HIGH); 
    DIO_PinLevel2[0] = Dio_ReadChannel(Main_GPIO0_Channel_0);

    Dio_WriteChannel(Main_GPIO0_Channel_0,STD_HIGH); 
    DIO_PinLevel2[1] = Dio_ReadChannel(Main_GPIO0_Channel_0);

    Dio_WriteChannel(Main_GPIO0_Channel_0,STD_LOW); 
    DIO_PinLevel2[2] = Dio_ReadChannel(Main_GPIO0_Channel_0);
    if ((DIO_PinLevel2[0U] != STD_HIGH) || (DIO_PinLevel2[1U] != STD_HIGH)||
        ((DIO_PinLevel2[2U] != STD_LOW) ))
    {
        DIO_TestPassed = E_NOT_OK;
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "Please check if Read-back is enabled for Main_GPIO_0.\n");

        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "DIO Service API Read-back Channel fails for Main_GPIO_0 !!!\n");
    }
    else
    {
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "DIO Service API Read-back Channel Succeeds for Main_GPIO_0 !!!\n");
    }
    
    Dio_WriteChannel(Main_GPIO1_Channel_0,STD_HIGH); 
    DIO_PinLevel3[0] = Dio_ReadChannel(Main_GPIO1_Channel_0);

    Dio_WriteChannel(Main_GPIO1_Channel_0,STD_HIGH); 
    DIO_PinLevel3[1] = Dio_ReadChannel(Main_GPIO1_Channel_0);

    Dio_WriteChannel(Main_GPIO1_Channel_0,STD_LOW); 
    DIO_PinLevel3[2] = Dio_ReadChannel(Main_GPIO1_Channel_0);
    if ((DIO_PinLevel3[0U] != STD_HIGH) || (DIO_PinLevel3[1U] != STD_HIGH)||
        ((DIO_PinLevel3[2U] != STD_LOW) ))
    {
        DIO_TestPassed = E_NOT_OK;
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "Please check if Read-back is enabled for Main_GPIO_1.\n");

        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "DIO Service API Read-back Channel fails for Main_GPIO_1 !!!\n");
    }
    else
    {
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "DIO Service API Read-back Channel Succeeds for Main_GPIO_1 !!!\n");
    }

#endif

    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                  "\nDIO Test A :Service API: Write/Read Channel completed\n");
    /* Test B.
     * 1. Use Dio_WriteChannelGroup to write into channels group
     * 2. Use Dio_ReadChannelGroup to read
    */
#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4) || defined (SOC_AM62X)
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "\nTest B. Write and Read Channel Group\n");
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "------------------------------------\n");
    /*Updating Channel Group Offset,Mask and Level*/
    DIO_chGrpShift = WKUP_GPIO0_B01_Bit0_1_PTR->offset;
    DIO_chGrpMask = WKUP_GPIO0_B01_Bit0_1_PTR->mask;
    DIO_PortLevel[0U] = 0U;
    DIO_PortLevel[0U] = ((STD_HIGH << 0U) | (STD_LOW << 1U));

    /* Confirmation checks performed */
    if ((DIO_PortLevel[0U] << DIO_chGrpShift) !=
        (DIO_chGrpMask & (DIO_PortLevel[0U] << DIO_chGrpShift)))
    {
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "This demo application requies 2 consecutive pins,");
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "to demonstrate group write\n");
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        " DIO Test B : Skipping this test ");
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        ": Write/Read Channel Group skipped\n");
    }
    else
    {
        /*Write Channel Group*/
        Dio_WriteChannelGroup(WKUP_GPIO0_B01_Bit0_1_PTR,
                                DIO_PortLevel[0U]);
        /*Read Channel Group*/
        DIO_PortLevel[1U] = Dio_ReadChannelGroup(
                                WKUP_GPIO0_B01_Bit0_1_PTR);
        /*Check if written value is same as read*/
        if (DIO_PortLevel[1U] != DIO_PortLevel[0U])
        {
            DIO_TestPassed = E_NOT_OK;
            AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                            "Please check if Read-back is enabled.\n");
            AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                            "DIO Service Read/Write Channel Group Read-back");
            AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                            "fails !!!\n");
        }
        else
        {
            AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                            "DIO Service Read/Write Channel Group Read-back ");
            AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                            "Succeeds !!!\n");
        }
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "\nDIO Test B : Service API ");
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        ": Write/Read Channel Group completed\n");
    }

    /* Test C.
     * 1. Use Dio_WritePort to write into channels of GPIO 3
     * 2. Used Dio_ReadPort to confirm bit's are set
     */
    /*  Set Pins 4,5,6,7 of port GPIO0_B01*/
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "\nTest C. Write and Read Port\n");
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "---------------------------\n");
    /*Update level to be written to port*/
    DIO_PortLevel[0U] = 3U;
    DIO_PortLevel[1U] = 0U;

    DIO_PortLevel[1U] = Dio_ReadPort(DioConf_DioPort_WKUP_GPIO0_B01);
    DIO_PortLevel[0U] = DIO_PortLevel[1U] | 3U;
    Dio_WritePort(DioConf_DioPort_WKUP_GPIO0_B01, DIO_PortLevel[0U]);
    DIO_PortLevel[1U] = Dio_ReadPort(DioConf_DioPort_WKUP_GPIO0_B01);

    /* Confirmation checks performed*/
    if (((DIO_PortLevel[1U] & 3U) != 3U))
    {
        DIO_TestPassed = E_NOT_OK;
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "Please check if Read-back is enabled.\n");
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "DIO Service API Read-Back Port fails !!!\n");
    }
    else
    {
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "DIO Service API Read-Back Port succeeds !!!\n");
    }
#endif
#if defined (SOC_J721E) || defined (SOC_J7200)
    /*Update level to be written to port*/
    DIO_PortLevel[0U] = 3U;
    DIO_PortLevel[1U] = 0U;

    DIO_PortLevel[1U] = Dio_ReadPort(DioConf_DioPort_GPIO0_B01);
    DIO_PortLevel[0U] = DIO_PortLevel[1U] | 3U;
    Dio_WritePort(DioConf_DioPort_GPIO0_B01, DIO_PortLevel[0U]);
    DIO_PortLevel[1U] = Dio_ReadPort(DioConf_DioPort_GPIO0_B01);

    /* Confirmation checks performed*/
    if (((DIO_PortLevel[1U] & 3U) != 3U))
    {
        DIO_TestPassed = E_NOT_OK;
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "Please check if Read-back is enabled.\n");
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                "DIO Service API Read-Back Port Main Domain fails !!!\n");
    }
    else
    {
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                "DIO Service API Read-Back Port Main Domain succeeds !!!\n");
    }
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "\nDIO Test C : Service API: Write/Read Port completed\n");
#endif

#if defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "\nTest B. Write and Read Channel Group\n");

    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "---------------------\n");

    /*Updating Channel Group Offset,Mask and Level*/
    DIO_chGrpShift = MCU_GPIO0_B01_Bit0_1_PTR->offset;
    DIO_chGrpMask = MCU_GPIO0_B01_Bit0_1_PTR->mask;
    DIO_PortLevel[0U] = 0U;
    DIO_PortLevel[0U] = ((STD_HIGH << 0U) | (STD_LOW << 1U));

    /* Confirmation checks performed */
    if ((DIO_PortLevel[0U] << DIO_chGrpShift) !=
        (DIO_chGrpMask & (DIO_PortLevel[0U] << DIO_chGrpShift)))
    {
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "This demo application requies 2 consecutive pins,");

        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "to demonstrate group write\n");

        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        " DIO Test B : Skipping this test ");

        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        ": Write/Read Channel Group skipped\n");
    }
    else
    {
        /*Write Channel Group*/
        Dio_WriteChannelGroup(MCU_GPIO0_B01_Bit0_1_PTR,
                                DIO_PortLevel[0U]);
        /*Read Channel Group*/
        DIO_PortLevel[1U] = Dio_ReadChannelGroup(
                MCU_GPIO0_B01_Bit0_1_PTR);
        /*Check if written value is same as read*/
        if (DIO_PortLevel[1U] != DIO_PortLevel[0U])
        {
            DIO_TestPassed = E_NOT_OK;
            AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                            "Please check if Read-back is enabled.\n");
            AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                            "DIO Service Read/Write Channel Group Read-back");
            AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                            "fails !!!\n");

        }
        else
        {
            AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                            "DIO Service Read/Write Channel Group Read-back ");

            AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                            "Succeeds !!!\n");
        }
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "\nDIO Test B : Service API ");
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        ": Write/Read Channel Group completed\n");
    }

    /* Test C.
     * 1. Use Dio_WritePort to write into channels of GPIO 3
     * 2. Used Dio_ReadPort to confirm bit's are set
     */
    /*  Set Pins 4,5,6,7 of port GPIO0_B01*/
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "\nTest C. Write and Read Port\n");

    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "---------------------\n");

    /*Update level to be written to port*/
    DIO_PortLevel[0U] = 3U;
    DIO_PortLevel[1U] = 0U;

    DIO_PortLevel[1U] = Dio_ReadPort(DioConf_DioPort_MCU_GPIO0_B01);
    DIO_PortLevel[0U] = DIO_PortLevel[1U] | 3U;
    Dio_WritePort(DioConf_DioPort_MCU_GPIO0_B01, DIO_PortLevel[0U]);
    DIO_PortLevel[1U] = Dio_ReadPort(DioConf_DioPort_MCU_GPIO0_B01);

    /* Confirmation checks performed*/
    if (((DIO_PortLevel[1U] & 3U) != 3U))
    {
        DIO_TestPassed = E_NOT_OK;
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "Please check if Read-back is enabled.\n");

        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "DIO Service API Read-Back Port fails !!!\n");

    }
    else
    {
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "DIO Service API Read-Back Port succeeds !!!\n");

    }
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "\nDIO Test C : Service API: Write/Read Port completed\n");
#endif

#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
#if (STD_ON == DIO_FLIP_CHANNEL_API)
    /* Test C.
     * 3. Flip the set channels, read pin and confirm the flip
     */
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "\nTest D. Flip Channel \n");
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                     "----------------------\n");
    /*Read before flip*/
    DIO_PinLevel[1U] = 0U;
    DIO_PinLevel[1U] = Dio_ReadChannel(WKUP_GPIO0_B01_Ch8);
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "Pin Value Before Flip: %d \n",DIO_PinLevel[1U]);
    /*Flip Channel*/
    Dio_FlipChannel(WKUP_GPIO0_B01_Ch8);
    /*Read after flip*/
    DIO_PinLevel[2U] = 0U;
    DIO_PinLevel[2U] = Dio_ReadChannel(WKUP_GPIO0_B01_Ch8);
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "Pin Value After Flip: %d \n",DIO_PinLevel[2U]);

    if (DIO_PinLevel[1U] == DIO_PinLevel[2U])
    {
        DIO_TestPassed = E_NOT_OK;
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "DIO Service API Flip Channel fails !!!\n");
    }
    else
    {
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "\nDIO Test D : Service API: Flip Channel completed\n");
    }


#if defined (SOC_J721E) || defined (SOC_J7200)
    /*Read before flip*/
    DIO_PinLevel[1U] = 0U;
    DIO_PinLevel[1U] = Dio_ReadChannel(GPIO0_B01_Ch0);
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "Pin Value Before Flip: %d \n",DIO_PinLevel[1U]);
    /*Flip Channel*/
    Dio_FlipChannel(GPIO0_B01_Ch0);
    /*Read after flip*/
    DIO_PinLevel[2U] = 0U;
    DIO_PinLevel[2U] = Dio_ReadChannel(GPIO0_B01_Ch0);
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "Pin Value After Flip: %d \n",DIO_PinLevel[2U]);

    if (DIO_PinLevel[1U] == DIO_PinLevel[2U])
    {
        DIO_TestPassed = E_NOT_OK;
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
           "DIO Service API Flip Channel Main Domain fails !!!\n");
    }
    else
    {
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
            "\nDIO Test D : Service API: Flip Channel Main Domain completed\n");
    }
#endif	
#endif	/*(STD_ON == DIO_FLIP_CHANNEL_API)*/
#endif
#if defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
#if (STD_ON == DIO_FLIP_CHANNEL_API)
    /* Test C.
     * 3. Flip the set channels, read pin and confirm the flip
     */
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "\nTest D. Flip Channel \n");

    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                     "----------------------\n");

    /*Read before flip*/
    DIO_PinLevel[1U] = 0U;
    DIO_PinLevel[1U] = Dio_ReadChannel(MCU_GPIO0_B01_Ch8);
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "Pin Value Before Flip: %d \n",DIO_PinLevel[1U]);

    /*Flip Channel*/
    Dio_FlipChannel(MCU_GPIO0_B01_Ch8);
    /*Read after flip*/
    DIO_PinLevel[2U] = 0U;
    DIO_PinLevel[2U] = Dio_ReadChannel(MCU_GPIO0_B01_Ch8);
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "Pin Value After Flip: %d \n",DIO_PinLevel[2U]);

    if (DIO_PinLevel[1U] == DIO_PinLevel[2U])
    {
        DIO_TestPassed = E_NOT_OK;
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "DIO Service API Flip Channel fails !!!\n");
    }
    else
    {
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "\nDIO Test D : Service API: Flip Channel completed\n");
    }
#endif
#endif

#if (STD_ON == DIO_REGISTER_READBACK_API)
#if defined (SOC_J721E) || defined (SOC_J7200)  || defined (SOC_AM62X)
    /*Test E. Service API Dio_RegisterReadback */
    retVal = Dio_RegisterReadback(WKUP_GPIO0_B01_Ch8, &Dio_RegRb);
    if (retVal != E_NOT_OK)
    {
        readback_final = Dio_RegRb.pid;
    }
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "\nTest E. Dio_RegisterReadback \n");
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "----------------------\n");
    if (readback_final != readback_init)
    {
        DIO_TestPassed = E_NOT_OK;
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "DIO register readback compare Failed!!\n");
    }
    else
    {
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "DIO register readback compare Passed!!\n");
        if (DIO_DIR_INPUT == (Dio_RegRb.Dio_Direction))
	      {
            AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "DIO register(WKUP_GPIO0_B01_Ch8) direction is Output !!\n");
        }
        else
        {
            AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "DIO register(WKUP_GPIO0_B01_Ch8) direction is Input !!\n");
        }
    }

#if defined (SOC_J721E) || defined (SOC_J7200)
    retVal = Dio_RegisterReadback(GPIO0_B01_Ch0, &Dio_RegRb);
    if (retVal != E_NOT_OK)
    {
        readback_final = Dio_RegRb.pid;
    }
    if (readback_final != readback_init_gpio)
    {
        DIO_TestPassed = E_NOT_OK;
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "DIO register readback compare Main Domain Failed!!\n");
    }
    else
    {
        AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "DIO register readback compare Main Domain Passed!!\n");
        if (DIO_DIR_INPUT == (Dio_RegRb.Dio_Direction))
        {
            AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "DIO register(GPIO0_B01_Ch0) direction is Output !!\n");
        }
        else
        {
            AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "DIO register direction(GPIO0_B01_Ch0) is Input !!\n");
        }
    }

    retVal = Dio_RegisterReadback(GPIO0_B01_Ch22, &Dio_RegRb);
    if (retVal != E_NOT_OK)
    {
        if (DIO_DIR_INPUT == (Dio_RegRb.Dio_Direction))
        {
            AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "DIO register(GPIO0_B01_Ch22) direction is Output !!\n");
        }
        else
        {
            AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "DIO register direction(GPIO0_B01_Ch22) is Input !!\n");
        }
    }
#endif
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "\nDIO Test E : Service API: Register Read-back ");
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                        "completed\n");
#endif
#endif /*(STD_ON == DIO_REGISTER_READBACK_API)*/

    AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                                                "DIO Stack Usage: %d bytes\n",
                                                AppUtils_GetStackUsage());

    if (AppUtils_DioCheckStackAndSectionCorruption() != E_OK)
    {
        DIO_TestPassed = E_NOT_OK;
        AppUtils_Printf(APP_UTILS_PRINT_MSG_NORMAL,
                                        " DIO Stack/section corruption!!!\n");
    }

    return;
}

/**
 *\brief   This function is to perform Initialization and pinmuxingfor DIO
 *
 */
static void DioApp_Startup(void)
{
    uint32 regVal = 0U, outputEnableMask = 0U, inputEnableMask = 0U;
    uint32       setMask;
    uint32       clrMask;
    CSL_GpioHandle hGpio;
    uint32                          DIO_PortId;
    uint32                          DIO_InstanceId;
    uint32                          DIO_InputEnableMask;
    uint32                          DIO_OutputEnableMask;
    uint32                          DIO_OutputPinInitialLevel;
    uint32                          DIO_InstanceAddr;
 #if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
  /* Unlock lock key registers for Partition 7: IO PAD
       configuration registers in MAIN_CTRL_MMR */
    /* write Partition 7 Lock Key 0 Register */
    LLD_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x5008, 0x68EF3490);
    /* write Partition 7 Lock Key 1 Register */
    LLD_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x500C, 0xD172BC5A);
    /* Check for unlock */
    regVal = LLD_REG32_RD(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x5008);
    while ((regVal & 0x1) != 0x1U)
    {
        regVal = LLD_REG32_RD(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x5008);
    }
    regVal = LLD_REG32_RD(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x8070);
    regVal &= 0U;
    regVal |= 0x00001U;
    LLD_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE + 0x8070, regVal);

    /*Unlock Control mmr regions Wakeup domain */
    DioApp_unlockMmrRegisters(CSL_WKUP_CTRL_MMR_CFG0_LOCK0_KICK0,
                                CSL_WKUP_CTRL_MMR_CFG0_LOCK0_KICK1);
    DioApp_unlockMmrRegisters(CSL_WKUP_CTRL_MMR_CFG0_LOCK1_KICK0,
                                CSL_WKUP_CTRL_MMR_CFG0_LOCK1_KICK1);
    DioApp_unlockMmrRegisters(CSL_WKUP_CTRL_MMR_CFG0_LOCK2_KICK0,
                                CSL_WKUP_CTRL_MMR_CFG0_LOCK2_KICK1);
    DioApp_unlockMmrRegisters(CSL_WKUP_CTRL_MMR_CFG0_LOCK3_KICK0,
                                CSL_WKUP_CTRL_MMR_CFG0_LOCK3_KICK1);
    DioApp_unlockMmrRegisters(CSL_WKUP_CTRL_MMR_CFG0_LOCK4_KICK0,
                                CSL_WKUP_CTRL_MMR_CFG0_LOCK4_KICK1);
 #endif
#if defined (SOC_J721E)
    DioApp_unlockMmrRegisters(CSL_WKUP_CTRL_MMR_CFG0_LOCK6_KICK0,
                                CSL_WKUP_CTRL_MMR_CFG0_LOCK6_KICK1);
#endif
#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)

    DioApp_unlockMmrRegisters(CSL_WKUP_CTRL_MMR_CFG0_LOCK7_KICK0,
                                CSL_WKUP_CTRL_MMR_CFG0_LOCK7_KICK1);
#endif
    /*Pin Mux for WKUP_GPIO0_0,1,8,50*/
    /*Updated Muxmode value as per discussion wrt MCAL-2139*/
#if defined (SOC_J721E)
    DioApp_McuDomainPinMux(CSL_WKUP_CTRL_MMR_CFG0_PADCONFIG52, 0x08050007);
    DioApp_McuDomainPinMux(CSL_WKUP_CTRL_MMR_CFG0_PADCONFIG44, 0x08050007);
    DioApp_McuDomainPinMux(CSL_WKUP_CTRL_MMR_CFG0_PADCONFIG45, 0x08050007);
    DioApp_McuDomainPinMux(CSL_WKUP_CTRL_MMR_CFG0_PADCONFIG38, 0x08050007);
#elif defined (SOC_J7200)
    DioApp_McuDomainPinMux(CSL_WKUP_CTRL_MMR_CFG0_PADCONFIG48, 0x08050007);
    DioApp_McuDomainPinMux(CSL_WKUP_CTRL_MMR_CFG0_PADCONFIG49, 0x08050007);
    DioApp_McuDomainPinMux(CSL_WKUP_CTRL_MMR_CFG0_PADCONFIG56, 0x08050007);
    DioApp_McuDomainPinMux(CSL_WKUP_CTRL_MMR_CFG0_PADCONFIG34, 0x08050007);
#elif defined (SOC_AM62X) || defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
    /*Unlock Control mmr regions Wakeup domain */
    DioApp_unlockMmrRegisters(CSL_MAIN_PADCFG_CTRL_MMR_CFG0_LOCK1_KICK0,
                                CSL_MAIN_PADCFG_CTRL_MMR_CFG0_LOCK1_KICK1);
    /*Updated Muxmode value as per discussion wrt MCAL-2139*/


    DioApp_McuDomainPinMux(CSL_MAIN_PADCFG_CTRL_MMR_CFG0_PADCONFIG0,  0x00040007U);
    DioApp_McuDomainPinMux(CSL_MAIN_PADCFG_CTRL_MMR_CFG0_PADCONFIG1,  0x00040007U);
    DioApp_McuDomainPinMux(CSL_MAIN_PADCFG_CTRL_MMR_CFG0_PADCONFIG2,  0x00040007U);
    DioApp_McuDomainPinMux(CSL_MAIN_PADCFG_CTRL_MMR_CFG0_PADCONFIG3,  0x00040007U);
    DioApp_McuDomainPinMux(CSL_MAIN_PADCFG_CTRL_MMR_CFG0_PADCONFIG4,  0x00040007U);
    DioApp_McuDomainPinMux(CSL_MAIN_PADCFG_CTRL_MMR_CFG0_PADCONFIG5,  0x00040007U);
    DioApp_McuDomainPinMux(CSL_MAIN_PADCFG_CTRL_MMR_CFG0_PADCONFIG8,  0x00040007U);
    DioApp_McuDomainPinMux(CSL_MAIN_PADCFG_CTRL_MMR_CFG0_PADCONFIG14, 0x00040007U);
    DioApp_McuDomainPinMux(CSL_MAIN_PADCFG_CTRL_MMR_CFG0_PADCONFIG15, 0x00040007U);

    DioApp_MainDomainPinMux(CSL_MAIN_PADCFG_CTRL_MMR_CFG0_PADCONFIG0, 0x00040007U);
    DioApp_MainDomainPinMux(CSL_MAIN_PADCFG_CTRL_MMR_CFG0_PADCONFIG94, 0x00040007U);
 #endif
    /*Initializing DIO module: Instance of DIO, Pin Direction, Initial Value*/
    /* Dio Instance is 0: Wake Up domain */
    DIO_InstanceId = 0U;
    DIO_PortId = 0U;
    DIO_OutputEnableMask = ((1U << 0U) | (1U << 1U) | (1U << 8U));
    DIO_OutputPinInitialLevel = (0U);
    DIO_InputEnableMask = (0U | (1U << 22U));
    DIO_InstanceAddr = DIO_GPIOBaseAddr[DIO_InstanceId];
    hGpio = (CSL_GpioHandle)DIO_InstanceAddr;
    /* Set output pin levels for GPIO */
    if (DIO_OutputEnableMask != 0U)
    {
        /* Enable output pin */
        regVal = hGpio->BANK_REGISTERS[DIO_PortId].DIR;
        outputEnableMask = (regVal & (~DIO_OutputEnableMask));
        hGpio->BANK_REGISTERS[DIO_PortId].DIR = outputEnableMask;
        setMask = DIO_OutputEnableMask & DIO_OutputPinInitialLevel;
        clrMask = DIO_OutputEnableMask & (~DIO_OutputPinInitialLevel);
        hGpio->BANK_REGISTERS[DIO_PortId].SET_DATA = setMask;
        hGpio->BANK_REGISTERS[DIO_PortId].CLR_DATA = clrMask;
    }
        /* Set input pin levels for GPIO */
    if (DIO_InputEnableMask != 0U)
    {
        /* Enable input pin */
        regVal = hGpio->BANK_REGISTERS[DIO_PortId].DIR;
        inputEnableMask = (regVal | DIO_InputEnableMask);
        hGpio->BANK_REGISTERS[DIO_PortId].DIR = inputEnableMask;
    }
    
#if defined (SOC_AM62X) || defined (SOC_AM62AX) || defined (SOC_AM62PX) || defined (SOC_J722S)
    DIO_InstanceId = 1U;
    DIO_PortId = 0U;
    DIO_OutputEnableMask = ((1U << 0U) | (1U << 1U) | (1U << 2U));
    DIO_OutputPinInitialLevel = (0U);
    DIO_InputEnableMask = (0U | (1U << 22U));
    DIO_InstanceAddr = DIO_GPIOBaseAddr[DIO_InstanceId];
    hGpio = (CSL_GpioHandle)DIO_InstanceAddr;
    /* Set output pin levels for GPIO */
    if (DIO_OutputEnableMask != 0U)
    {
        /* Enable output pin */
        regVal = hGpio->BANK_REGISTERS[DIO_PortId].DIR;
        outputEnableMask = (regVal & (~DIO_OutputEnableMask));
        hGpio->BANK_REGISTERS[DIO_PortId].DIR = (~DIO_OutputEnableMask);
        setMask = DIO_OutputEnableMask & DIO_OutputPinInitialLevel;
        clrMask = DIO_OutputEnableMask & (~DIO_OutputPinInitialLevel);
        hGpio->BANK_REGISTERS[DIO_PortId].SET_DATA = setMask;
        hGpio->BANK_REGISTERS[DIO_PortId].CLR_DATA = clrMask;
    }
        /* Set input pin levels for GPIO */
    if (DIO_InputEnableMask != 0U)
    {
        /* Enable input pin */
        regVal = hGpio->BANK_REGISTERS[DIO_PortId].DIR;
        inputEnableMask = (regVal | DIO_InputEnableMask);
        hGpio->BANK_REGISTERS[DIO_PortId].DIR = inputEnableMask;
    }

    DIO_InstanceId = 2U;
    DIO_PortId = 0U;
    DIO_OutputEnableMask = ((1U << 0U) | (1U << 1U) | (1U << 2U));
    DIO_OutputPinInitialLevel = (0U);
    DIO_InputEnableMask = (0U | (1U << 22U));
    DIO_InstanceAddr = DIO_GPIOBaseAddr[DIO_InstanceId];
    hGpio = (CSL_GpioHandle)DIO_InstanceAddr;
    /* Set output pin levels for GPIO */
    if (DIO_OutputEnableMask != 0U)
    {
        /* Enable output pin */
        regVal = hGpio->BANK_REGISTERS[DIO_PortId].DIR;
        outputEnableMask = (regVal & (~DIO_OutputEnableMask));
        hGpio->BANK_REGISTERS[DIO_PortId].DIR = (~DIO_OutputEnableMask);
        setMask = DIO_OutputEnableMask & DIO_OutputPinInitialLevel;
        clrMask = DIO_OutputEnableMask & (~DIO_OutputPinInitialLevel);
        hGpio->BANK_REGISTERS[DIO_PortId].SET_DATA = setMask;
        hGpio->BANK_REGISTERS[DIO_PortId].CLR_DATA = clrMask;
    }
        /* Set input pin levels for GPIO */
    if (DIO_InputEnableMask != 0U)
    {
        /* Enable input pin */
        regVal = hGpio->BANK_REGISTERS[DIO_PortId].DIR;
        inputEnableMask = (regVal | DIO_InputEnableMask);
        hGpio->BANK_REGISTERS[DIO_PortId].DIR = inputEnableMask;
    }
#endif

#if defined (SOC_J721E) || defined (SOC_J7200)
    /*Unlock Control mmr regions and enable pin mux*/
    DioApp_unlockMmrRegisters(CSL_MAIN_CTRL_MMR_CFG0_LOCK0_KICK0,
                                CSL_MAIN_CTRL_MMR_CFG0_LOCK0_KICK1);
    DioApp_unlockMmrRegisters(CSL_MAIN_CTRL_MMR_CFG0_LOCK1_KICK0,
                                CSL_MAIN_CTRL_MMR_CFG0_LOCK1_KICK1);
    DioApp_unlockMmrRegisters(CSL_MAIN_CTRL_MMR_CFG0_LOCK2_KICK0,
                                CSL_MAIN_CTRL_MMR_CFG0_LOCK2_KICK1);
    DioApp_unlockMmrRegisters(CSL_MAIN_CTRL_MMR_CFG0_LOCK3_KICK0,
                                CSL_MAIN_CTRL_MMR_CFG0_LOCK3_KICK1);
    DioApp_unlockMmrRegisters(CSL_MAIN_CTRL_MMR_CFG0_LOCK4_KICK0,
                                CSL_MAIN_CTRL_MMR_CFG0_LOCK4_KICK1);
#if defined (SOC_J721E)
    DioApp_unlockMmrRegisters(CSL_MAIN_CTRL_MMR_CFG0_LOCK6_KICK0,
                                CSL_MAIN_CTRL_MMR_CFG0_LOCK6_KICK1);
#endif
    DioApp_unlockMmrRegisters(CSL_MAIN_CTRL_MMR_CFG0_LOCK7_KICK0,
                                CSL_MAIN_CTRL_MMR_CFG0_LOCK7_KICK1);
#if defined (SOC_J721E)
    DioApp_MainDomainPinMux(CSL_MAIN_CTRL_MMR_CFG0_PADCONFIG0, 0x08214007);
    DioApp_MainDomainPinMux(CSL_MAIN_CTRL_MMR_CFG0_PADCONFIG1, 0x08214007);
    DioApp_MainDomainPinMux(CSL_MAIN_CTRL_MMR_CFG0_PADCONFIG2, 0x08214007);
#elif defined (SOC_J7200)
    DioApp_MainDomainPinMux(CSL_MAIN_CTRL_MMR_CFG0_PADCONFIG0, 0x08214007);
    DioApp_MainDomainPinMux(CSL_MAIN_CTRL_MMR_CFG0_PADCONFIG1, 0x08214007);
    DioApp_MainDomainPinMux(CSL_MAIN_CTRL_MMR_CFG0_PADCONFIG73, 0x08214007);
#endif
    /* Main Domain GPIO0 Instance */
    DIO_InstanceId = 2U;
    DIO_PortId = 0U;
    DIO_OutputEnableMask = ((1U << 0U) | (1U << 1U) | (1U << 2U));
    DIO_OutputPinInitialLevel = (0U);
    DIO_InputEnableMask = (0U | (1U << 22U));
    DIO_InstanceAddr = DIO_GPIOBaseAddr[DIO_InstanceId];
    hGpio = (CSL_GpioHandle)DIO_InstanceAddr;
    /* Set output pin levels for GPIO */
    if (DIO_OutputEnableMask != 0U)
    {
        /* Enable output pin */
        regVal = hGpio->BANK_REGISTERS[DIO_PortId].DIR;
        outputEnableMask = (regVal & (~DIO_OutputEnableMask));
        hGpio->BANK_REGISTERS[DIO_PortId].DIR = outputEnableMask;
        setMask = DIO_OutputEnableMask & DIO_OutputPinInitialLevel;
        clrMask = DIO_OutputEnableMask & (~DIO_OutputPinInitialLevel);
        hGpio->BANK_REGISTERS[DIO_PortId].SET_DATA = setMask;
        hGpio->BANK_REGISTERS[DIO_PortId].CLR_DATA = clrMask;
    }
        /* Set input pin levels for GPIO */
    if (DIO_InputEnableMask != 0U)
    {
        /* Enable input pin */
        regVal = hGpio->BANK_REGISTERS[DIO_PortId].DIR;
        inputEnableMask = (regVal | DIO_InputEnableMask);
        hGpio->BANK_REGISTERS[DIO_PortId].DIR = inputEnableMask;
    }

#endif
    /* Initialize memory sections  */
    AppUtils_DioSectionInit();

    return;
}
/**
 *\brief   This function is set a unloack MMR register required for Pin Mux
 *
 */
static void DioApp_unlockMmrRegisters(uint32 lockKey0, uint32 lockKey1)
{
#if defined (SOC_AM62X)
    uint32 regVal = 0U;

    /* write Partition 1 Lock Key 0 Register */
    LLD_REG32_WR(0x04080000UL + 0x5008, 0x68EF3490);

    /* write Partition 1 Lock Key 1 Register */
    LLD_REG32_WR(0x04080000UL + 0x500C, 0xD172BC5A);
    Dio_delay(1000);
    /* Check for unlock */
    regVal = LLD_REG32_RD(0x4080000UL + 0x5008);
    while ((regVal & 0x1) != 0x1U)
    {
        regVal = LLD_REG32_RD(0x4080000UL + 0x500C);
    }
    /* write Partition 1 Lock Key 0 Register */
    /*Main GPIO- PADCFG_CTRL0_CFG0 Register*/
    LLD_REG32_WR(0xF0000UL + 0x5008, 0x68EF3490);

    /* write Partition 1 Lock Key 1 Register */
    LLD_REG32_WR(0xF0000UL + 0x500C, 0xD172BC5A);
    Dio_delay(1000);
    /* Check for unlock */
    regVal = LLD_REG32_RD(0xF0000UL + 0x5008);
    while ((regVal & 0x1) != 0x1U)
    {
        regVal = LLD_REG32_RD(0xF0000UL + 0x500C);
    }
#elif defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
	uint32 regVal = 0U;
    /* write Partition Lock Key 0 Register */
    LLD_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE + lockKey0, 0x68EF3490);
    /* write Partition Lock Key 1 Register */
    LLD_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE + lockKey1, 0xD172BC5A);
    /* Check for unlock */
    regVal = LLD_REG32_RD(CSL_WKUP_CTRL_MMR0_CFG0_BASE + lockKey0);
    while ((regVal & 0x1) != 0x1U)
    {
        regVal = LLD_REG32_RD(CSL_WKUP_CTRL_MMR0_CFG0_BASE + lockKey0);
    }
#endif
#if defined (SOC_J721E) || defined (SOC_J7200)
    /* write Partition Lock Key 0 Register */
    LLD_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + lockKey0, 0x68EF3490);
    /* write Partition Lock Key 1 Register */
    LLD_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + lockKey1, 0xD172BC5A);
    /* Check for unlock */
    regVal = LLD_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + lockKey0);
    while ((regVal & 0x1) != 0x1U)
    {
        regVal = LLD_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + lockKey0);
    }
#elif defined (SOC_AM62AX)
    uint32 regVal = 0U;

    /* write Partition 1 Lock Key 0 Register */
    LLD_REG32_WR(CSL_WKUP_PADCFG_CTRL0_CFG0_BASE + 0x5008, 0x68EF3490);

    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "\n");

    /* write Partition 1 Lock Key 1 Register */
    LLD_REG32_WR(CSL_WKUP_PADCFG_CTRL0_CFG0_BASE + 0x500C, 0xD172BC5A);

    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "\n");

    /* Check for unlock */
    regVal = LLD_REG32_RD(CSL_WKUP_PADCFG_CTRL0_CFG0_BASE + 0x5008);
    while ((regVal & 0x1) != 0x1U)
    {
        regVal = LLD_REG32_RD(CSL_WKUP_PADCFG_CTRL0_CFG0_BASE + 0x5008);
    }
	
	/*Main GPIO- PADCFG_CTRL0_CFG0 Register*/
	/* write Partition 1 Lock Key 0 Register */
	LLD_REG32_WR(CSL_PADCFG_CTRL0_CFG0_BASE + 0x5008, 0x68EF3490);
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "\n");

    /* write Partition 1 Lock Key 1 Register */
	LLD_REG32_WR(CSL_PADCFG_CTRL0_CFG0_BASE + 0x500C, 0xD172BC5A);
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "\n");

    /* Check for unlock */
	regVal = LLD_REG32_RD(CSL_PADCFG_CTRL0_CFG0_BASE + 0x5008); 
    while ((regVal & 0x1) != 0x1U)
    {
		regVal = LLD_REG32_RD(CSL_PADCFG_CTRL0_CFG0_BASE + 0x5008);
    }
#elif defined (SOC_AM62PX) || defined (SOC_J722S)
    uint32 regVal = 0U;

    /* write Partition 1 Lock Key 0 Register */
    LLD_REG32_WR(CSL_MCU_PADCFG_CTRL0_CFG0_BASE + 0x5008, 0x68EF3490);

    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "\n");

    /* write Partition 1 Lock Key 1 Register */
    LLD_REG32_WR(CSL_MCU_PADCFG_CTRL0_CFG0_BASE + 0x500C, 0xD172BC5A);

    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "\n");

    /* Check for unlock */
    regVal = LLD_REG32_RD(CSL_MCU_PADCFG_CTRL0_CFG0_BASE + 0x5008);
    while ((regVal & 0x1) != 0x1U)
    {
        regVal = LLD_REG32_RD(CSL_MCU_PADCFG_CTRL0_CFG0_BASE + 0x5008);
    }
	
	/*Main GPIO- PADCFG_CTRL0_CFG0 Register*/
	/* write Partition 1 Lock Key 0 Register */
	LLD_REG32_WR(CSL_PADCFG_CTRL0_CFG0_BASE + 0x5008, 0x68EF3490);
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "\n");

    /* write Partition 1 Lock Key 1 Register */
	LLD_REG32_WR(CSL_PADCFG_CTRL0_CFG0_BASE + 0x500C, 0xD172BC5A);
    AppUtils_Printf((uint32)APP_UTILS_PRINT_MSG_NORMAL,
                    "\n");
    /* Check for unlock */
	regVal = LLD_REG32_RD(CSL_PADCFG_CTRL0_CFG0_BASE + 0x5008); 
    while ((regVal & 0x1) != 0x1U)
    {
		regVal = LLD_REG32_RD(CSL_PADCFG_CTRL0_CFG0_BASE + 0x5008);
    }
#endif

    return;
}

/**
 *\brief   This function is enable Pin Muxing: Wakeup domain
 *
 */

#if defined (SOC_AM62X) || defined (SOC_AM62AX)
static void DioApp_McuDomainPinMux(uint32 offset, uint32 muxmode)
{
    uint32 regVal = 0U;

    regVal = LLD_REG32_RD(LLD_WKUP_PADCFG_CTRL0_CFG0_BASE + offset);
    regVal &= 0U;
    regVal |= muxmode;
    LLD_REG32_WR(LLD_WKUP_PADCFG_CTRL0_CFG0_BASE + offset, regVal);

    return;
}
static void DioApp_MainDomainPinMux(uint32 offset, uint32 muxmode)
{
    uint32 regVal = 0U;

    regVal = LLD_REG32_RD(CSL_PADCFG_CTRL0_CFG0_BASE + offset);
    regVal &= 0U;
    regVal |= muxmode;
    LLD_REG32_WR(CSL_PADCFG_CTRL0_CFG0_BASE + offset, regVal);

    return;
}
#elif defined (SOC_AM62PX) || defined (SOC_J722S)
static void DioApp_McuDomainPinMux(uint32 offset, uint32 muxmode)
{
    uint32 regVal = 0U;

    regVal = LLD_REG32_RD(CSL_MCU_PADCFG_CTRL0_CFG0_BASE + offset);
    regVal &= 0U;
    regVal |= muxmode;
    LLD_REG32_WR(CSL_MCU_PADCFG_CTRL0_CFG0_BASE + offset, regVal);

    return;
}
static void DioApp_MainDomainPinMux(uint32 offset, uint32 muxmode)
{
    uint32 regVal = 0U;

    regVal = LLD_REG32_RD(CSL_PADCFG_CTRL0_CFG0_BASE + offset);
    regVal &= 0U;
    regVal |= muxmode;
    LLD_REG32_WR(CSL_PADCFG_CTRL0_CFG0_BASE + offset, regVal);

    return;
}

#elif defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)

static void DioApp_McuDomainPinMux(uint32 offset, uint32 muxmode)
{
    uint32 regVal = 0U;

    regVal = LLD_REG32_RD(CSL_WKUP_CTRL_MMR0_CFG0_BASE + offset);
    regVal &= 0U;
    regVal |= muxmode;
    LLD_REG32_WR(CSL_WKUP_CTRL_MMR0_CFG0_BASE + offset, regVal);

    return;
}
#endif
#if defined (SOC_J721E) || defined (SOC_J7200)
static void DioApp_MainDomainPinMux(uint32 offset, uint32 muxmode)
{
    uint32 regVal = 0U;
    regVal = LLD_REG32_RD(CSL_CTRL_MMR0_CFG0_BASE + offset);
    regVal &= 0U;
    regVal |= muxmode;
    LLD_REG32_WR(CSL_CTRL_MMR0_CFG0_BASE + offset, regVal);

    return;
}
#endif

/** \brief None, SBL/GEL powers up the timers and clock sources */
static void DioApp_PowerAndClkSrc(void)
{
    /* Mcu module, when included will replace this operation */
    return;
}
