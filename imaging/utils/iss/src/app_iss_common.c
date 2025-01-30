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
#include <dirent.h>
#include <sys/stat.h>


#include <utils/ipc/include/app_ipc.h>
#include <utils/remote_service/include/app_remote_service.h>
#include <utils/iss/include/app_iss.h>
#include <utils/console_io/include/app_log.h>

#ifdef VPAC3L
#include <dcc_viss_ov2312.h>
#endif

#include <dcc_viss_imx390.h>
#include <dcc_viss_imx390_wdr.h>

#if defined (SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J784S4)
#include <dcc_viss_imx390_discovery_mid.h>
#include <dcc_viss_imx390_discovery_mid_wdr.h>
#include <dcc_viss_imx390_discovery_narrow.h>
#include <dcc_viss_imx390_discovery_narrow_wdr.h>
#include <dcc_viss_imx390_discovery_wide.h>
#include <dcc_viss_imx390_discovery_wide_wdr.h>
#endif

#include <dcc_viss_ar0233.h>
#include <dcc_viss_ar0233_wdr.h>

#include <dcc_viss_ar0820.h>
#include <dcc_viss_ar0820_wdr.h>

#include <dcc_viss_ub9xx_raw_test_pattern.h>

#define _ENABLE_2A_

#ifdef VPAC3L
static uint8_t  dcc_viss_ov2312[DCC_VISS_OV2312_DCC_CFG_NUM_ELEM] = DCC_VISS_OV2312DCC_CFG;
#endif

static uint8_t  dcc_viss_imx390[DCC_VISS_IMX390_DCC_CFG_NUM_ELEM] = DCC_VISS_IMX390DCC_CFG;
static uint8_t  dcc_viss_imx390_wdr[DCC_VISS_IMX390_WDR_DCC_CFG_NUM_ELEM] = DCC_VISS_IMX390_WDRDCC_CFG;

#if defined (SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J784S4)
static uint8_t  dcc_viss_imx390_discovery_mid[DCC_VISS_IMX390_DISCOVERY_MID_DCC_CFG_NUM_ELEM] = DCC_VISS_IMX390_DISCOVERY_MIDDCC_CFG;
static uint8_t  dcc_viss_imx390_discovery_mid_wdr[DCC_VISS_IMX390_DISCOVERY_MID_WDR_DCC_CFG_NUM_ELEM] = DCC_VISS_IMX390_DISCOVERY_MID_WDRDCC_CFG;
static uint8_t  dcc_viss_imx390_discovery_narrow[DCC_VISS_IMX390_DISCOVERY_NARROW_DCC_CFG_NUM_ELEM] = DCC_VISS_IMX390_DISCOVERY_NARROWDCC_CFG;
static uint8_t  dcc_viss_imx390_discovery_narrow_wdr[DCC_VISS_IMX390_DISCOVERY_NARROW_WDR_DCC_CFG_NUM_ELEM] = DCC_VISS_IMX390_DISCOVERY_NARROW_WDRDCC_CFG;
static uint8_t  dcc_viss_imx390_discovery_wide[DCC_VISS_IMX390_DISCOVERY_WIDE_DCC_CFG_NUM_ELEM] = DCC_VISS_IMX390_DISCOVERY_WIDEDCC_CFG;
static uint8_t  dcc_viss_imx390_discovery_wide_wdr[DCC_VISS_IMX390_DISCOVERY_WIDE_WDR_DCC_CFG_NUM_ELEM] = DCC_VISS_IMX390_DISCOVERY_WIDE_WDRDCC_CFG;
#endif
static uint8_t  dcc_viss_ar0233_linear[DCC_VISS_AR0233_DCC_CFG_NUM_ELEM] = DCC_VISS_AR0233DCC_CFG;
static uint8_t  dcc_viss_ar0233_wdr[DCC_VISS_AR0233_WDR_DCC_CFG_NUM_ELEM] = DCC_VISS_AR0233_WDRDCC_CFG;

static uint8_t  dcc_viss_ar0820_linear[DCC_VISS_AR0820_DCC_CFG_NUM_ELEM] = DCC_VISS_AR0820DCC_CFG;
static uint8_t  dcc_viss_ar0820_wdr[DCC_VISS_AR0820_WDR_DCC_CFG_NUM_ELEM] = DCC_VISS_AR0820_WDRDCC_CFG;

static uint8_t  dcc_viss_ub9xx_raw_test_pattern_linear[DCC_VISS_UB9XX_RAW_TEST_PATTERN_DCC_CFG_NUM_ELEM] = DCC_VISS_UB9XX_RAW_TEST_PATTERNDCC_CFG;

#include <dcc_ldc_imx390.h>
#include <dcc_ldc_imx390_wdr.h>

#if defined (SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J784S4)
#include <dcc_ldc_imx390_discovery_mid.h>
#include <dcc_ldc_imx390_discovery_mid_wdr.h>
#include <dcc_ldc_imx390_discovery_narrow.h>
#include <dcc_ldc_imx390_discovery_narrow_wdr.h>
#include <dcc_ldc_imx390_discovery_wide.h>
#include <dcc_ldc_imx390_discovery_wide_wdr.h>
#endif

static uint8_t  dcc_ldc_imx390[DCC_LDC_IMX390_DCC_CFG_NUM_ELEM] = DCC_LDC_IMX390DCC_CFG;
static uint8_t  dcc_ldc_imx390_wdr[DCC_LDC_IMX390_WDR_DCC_CFG_NUM_ELEM] = DCC_LDC_IMX390_WDRDCC_CFG;

#if defined (SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J784S4)
static uint8_t  dcc_ldc_imx390_discovery_mid[DCC_LDC_IMX390_DISCOVERY_MID_DCC_CFG_NUM_ELEM] = DCC_LDC_IMX390_DISCOVERY_MIDDCC_CFG;
static uint8_t  dcc_ldc_imx390_discovery_mid_wdr[DCC_LDC_IMX390_DISCOVERY_MID_WDR_DCC_CFG_NUM_ELEM] = DCC_LDC_IMX390_DISCOVERY_MID_WDRDCC_CFG;
static uint8_t  dcc_ldc_imx390_discovery_narrow[DCC_LDC_IMX390_DISCOVERY_NARROW_DCC_CFG_NUM_ELEM] = DCC_LDC_IMX390_DISCOVERY_NARROWDCC_CFG;
static uint8_t  dcc_ldc_imx390_discovery_narrow_wdr[DCC_LDC_IMX390_DISCOVERY_NARROW_WDR_DCC_CFG_NUM_ELEM] = DCC_LDC_IMX390_DISCOVERY_NARROW_WDRDCC_CFG;
static uint8_t  dcc_ldc_imx390_discovery_wide[DCC_LDC_IMX390_DISCOVERY_WIDE_DCC_CFG_NUM_ELEM] = DCC_LDC_IMX390_DISCOVERY_WIDEDCC_CFG;
static uint8_t  dcc_ldc_imx390_discovery_wide_wdr[DCC_LDC_IMX390_DISCOVERY_WIDE_WDR_DCC_CFG_NUM_ELEM] = DCC_LDC_IMX390_DISCOVERY_WIDE_WDRDCC_CFG;
#endif

#include <dcc_ldc_ar0233.h>
#include <dcc_ldc_ar0233_wdr.h>
static uint8_t  dcc_ldc_ar0233_linear[DCC_LDC_AR0233_DCC_CFG_NUM_ELEM] = DCC_LDC_AR0233DCC_CFG;
static uint8_t  dcc_ldc_ar0233_wdr[DCC_LDC_AR0233_WDR_DCC_CFG_NUM_ELEM] = DCC_LDC_AR0233_WDRDCC_CFG;

#include <dcc_ldc_ar0820.h>
#include <dcc_ldc_ar0820_wdr.h>
static uint8_t  dcc_ldc_ar0820_linear[DCC_LDC_AR0820_DCC_CFG_NUM_ELEM] = DCC_LDC_AR0820DCC_CFG;
static uint8_t  dcc_ldc_ar0820_wdr[DCC_LDC_AR0820_WDR_DCC_CFG_NUM_ELEM] = DCC_LDC_AR0820_WDRDCC_CFG;

#include <dcc_ldc_ub9xx_raw_test_pattern.h>
static uint8_t  dcc_ldc_ub9xx_raw_test_pattern_linear[DCC_LDC_UB9XX_RAW_TEST_PATTERN_DCC_CFG_NUM_ELEM] = DCC_LDC_UB9XX_RAW_TEST_PATTERNDCC_CFG;

#include <dcc_ldc_ub96x_uyvytestpat.h>
static uint8_t  dcc_ldc_ub96x_uyvytestpat[DCC_LDC_UB96X_UYVYTESTPAT_DCC_CFG_NUM_ELEM] = DCC_LDC_UB96X_UYVYTESTPATDCC_CFG;

#include <dcc_ldc_gw_ar0233.h>
static uint8_t  dcc_ldc_gw_ar0233[DCC_LDC_GW_AR0233_DCC_CFG_NUM_ELEM] = DCC_LDC_GW_AR0233DCC_CFG;

#ifdef _ENABLE_2A_
#include <dcc_2a_imx390.h>
#include <dcc_2a_imx390_wdr.h>

#if defined (SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J784S4)
#include <dcc_2a_imx390_discovery_mid.h>
#include <dcc_2a_imx390_discovery_mid_wdr.h>
#include <dcc_2a_imx390_discovery_narrow.h>
#include <dcc_2a_imx390_discovery_narrow_wdr.h>
#include <dcc_2a_imx390_discovery_wide.h>
#include <dcc_2a_imx390_discovery_wide_wdr.h>
#endif

#include <dcc_2a_ar0233.h>
#include <dcc_2a_ar0233_wdr.h>

#include <dcc_2a_ar0820.h>
#include <dcc_2a_ar0820_wdr.h>

#include <dcc_2a_ub9xx_raw_test_pattern.h>

static uint8_t  dcc_2a_imx390[DCC_2A_IMX390_DCC_CFG_NUM_ELEM] = DCC_2A_IMX390DCC_CFG;
static uint8_t  dcc_2a_imx390_wdr[DCC_2A_IMX390_WDR_DCC_CFG_NUM_ELEM] = DCC_2A_IMX390_WDRDCC_CFG;

#if defined (SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J784S4)
static uint8_t  dcc_2a_imx390_discovery_mid[DCC_2A_IMX390_DISCOVERY_MID_DCC_CFG_NUM_ELEM] = DCC_2A_IMX390_DISCOVERY_MIDDCC_CFG;
static uint8_t  dcc_2a_imx390_discovery_mid_wdr[DCC_2A_IMX390_DISCOVERY_MID_WDR_DCC_CFG_NUM_ELEM] = DCC_2A_IMX390_DISCOVERY_MID_WDRDCC_CFG;
static uint8_t  dcc_2a_imx390_discovery_narrow[DCC_2A_IMX390_DISCOVERY_NARROW_DCC_CFG_NUM_ELEM] = DCC_2A_IMX390_DISCOVERY_NARROWDCC_CFG;
static uint8_t  dcc_2a_imx390_discovery_narrow_wdr[DCC_2A_IMX390_DISCOVERY_NARROW_WDR_DCC_CFG_NUM_ELEM] = DCC_2A_IMX390_DISCOVERY_NARROW_WDRDCC_CFG;
static uint8_t  dcc_2a_imx390_discovery_wide[DCC_2A_IMX390_DISCOVERY_WIDE_DCC_CFG_NUM_ELEM] = DCC_2A_IMX390_DISCOVERY_WIDEDCC_CFG;
static uint8_t  dcc_2a_imx390_discovery_wide_wdr[DCC_2A_IMX390_DISCOVERY_WIDE_WDR_DCC_CFG_NUM_ELEM] = DCC_2A_IMX390_DISCOVERY_WIDE_WDRDCC_CFG;
#endif

static uint8_t  dcc_2a_ar0233_linear[DCC_2A_AR0233_DCC_CFG_NUM_ELEM] = DCC_2A_AR0233DCC_CFG;
static uint8_t  dcc_2a_ar0233_wdr[DCC_2A_AR0233_WDR_DCC_CFG_NUM_ELEM] = DCC_2A_AR0233_WDRDCC_CFG;

static uint8_t  dcc_2a_ar0820_linear[DCC_2A_AR0820_DCC_CFG_NUM_ELEM] = DCC_2A_AR0820DCC_CFG;
static uint8_t  dcc_2a_ar0820_wdr[DCC_2A_AR0820_WDR_DCC_CFG_NUM_ELEM] = DCC_2A_AR0820_WDRDCC_CFG;

static uint8_t  dcc_2a_ub9xx_raw_test_pattern_linear[DCC_2A_UB9XX_RAW_TEST_PATTERN_DCC_CFG_NUM_ELEM] = DCC_2A_UB9XX_RAW_TEST_PATTERNDCC_CFG;

#endif

int32_t appIssGetDCCSizeVISS(char * sensor_name, uint32_t wdr_mode)
{
    int32_t size = -1;
    switch(wdr_mode)
    {
        case 0:
            /*Linear mode*/
            #ifdef VPAC3L
            if(0 == strcmp(sensor_name, SENSOR_OV2312_UB953_LI))
                size = DCC_VISS_OV2312_DCC_CFG_NUM_ELEM;
            else
            #endif
            if(0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_D3))
                size = DCC_VISS_IMX390_DCC_CFG_NUM_ELEM;
#if defined (SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J784S4)
            else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_MID))
                size = DCC_VISS_IMX390_DISCOVERY_MID_DCC_CFG_NUM_ELEM;
            else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_NARROW)) {
                    printf("********************imx390 Discovery %s [%d]********************\n", __func__, __LINE__);
                    size = DCC_VISS_IMX390_DISCOVERY_NARROW_DCC_CFG_NUM_ELEM;
            } else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_WIDE))
                size = DCC_VISS_IMX390_DISCOVERY_WIDE_DCC_CFG_NUM_ELEM;
#endif
            else if (0 == strcmp(sensor_name, SENSOR_ONSEMI_AR0233_UB953_MARS))
                size = DCC_VISS_AR0233_DCC_CFG_NUM_ELEM;
            else if (0 == strcmp(sensor_name, SENSOR_ONSEMI_AR0820_UB953_LI))
                size = DCC_VISS_AR0820_DCC_CFG_NUM_ELEM;
            else if (0 == strcmp(sensor_name, UB9XX_RAW_TESTPAT))
                size = DCC_VISS_UB9XX_RAW_TEST_PATTERN_DCC_CFG_NUM_ELEM;
            else
                size = -1;
            break;
        case 1:
            /*WDR mode*/
            if(0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_D3))
                size = DCC_VISS_IMX390_WDR_DCC_CFG_NUM_ELEM;
#if defined (SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J784S4)
            else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_MID))
                size = DCC_VISS_IMX390_DISCOVERY_MID_WDR_DCC_CFG_NUM_ELEM;
            else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_NARROW)){
                printf("********************imx390 Discovery %s [%d]********************\n", __func__, __LINE__);
                size = DCC_VISS_IMX390_DISCOVERY_NARROW_WDR_DCC_CFG_NUM_ELEM;
            }
            else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_WIDE))
                size = DCC_VISS_IMX390_DISCOVERY_WIDE_WDR_DCC_CFG_NUM_ELEM;
#endif
            else if (0 == strcmp(sensor_name, SENSOR_ONSEMI_AR0233_UB953_MARS))
                size = DCC_VISS_AR0233_WDR_DCC_CFG_NUM_ELEM;
            else if (0 == strcmp(sensor_name, SENSOR_ONSEMI_AR0820_UB953_LI))
                size = DCC_VISS_AR0820_WDR_DCC_CFG_NUM_ELEM;
            else
                size = -1;
            break;
        default:
            /*Unsupported mode*/
            size = -1;
    }
    return size;
}

int32_t appIssGetDCCBuffVISS(char * sensor_name, uint32_t wdr_mode,  uint8_t * dcc_buf, int32_t num_bytes)
{
    switch(wdr_mode)
    {
        case 0:
            /*Linear mode*/
            #ifdef VPAC3L
            if(0 == strcmp(sensor_name, SENSOR_OV2312_UB953_LI))
                memcpy(dcc_buf, dcc_viss_ov2312, num_bytes);
            else
            #endif
            if(0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_D3))
                memcpy(dcc_buf, dcc_viss_imx390, num_bytes);
#if defined (SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J784S4)
            else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_MID))
                memcpy(dcc_buf, dcc_viss_imx390_discovery_mid, num_bytes);
            else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_NARROW)){
                printf("********************imx390 Discovery %s [%d]********************\n", __func__, __LINE__);
                memcpy(dcc_buf, dcc_viss_imx390_discovery_narrow, num_bytes);
            }
            else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_WIDE))
                memcpy(dcc_buf, dcc_viss_imx390_discovery_wide, num_bytes);
#endif
            else if (0 == strcmp(sensor_name, SENSOR_ONSEMI_AR0233_UB953_MARS))
                memcpy(dcc_buf, dcc_viss_ar0233_linear, num_bytes);
            else if (0 == strcmp(sensor_name, SENSOR_ONSEMI_AR0820_UB953_LI))
                memcpy(dcc_buf, dcc_viss_ar0820_linear, num_bytes);
            else if (0 == strcmp(sensor_name, UB9XX_RAW_TESTPAT))
                memcpy(dcc_buf, dcc_viss_ub9xx_raw_test_pattern_linear, num_bytes);
            else
                return -1;
            break;
        case 1:
            /*WDR mode*/
            if(0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_D3))
                memcpy(dcc_buf, dcc_viss_imx390_wdr, num_bytes);
#if defined (SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J784S4)
            else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_MID))
                memcpy(dcc_buf, dcc_viss_imx390_discovery_mid_wdr, num_bytes);
            else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_NARROW)){
                printf("********************imx390 Discovery %s [%d]********************\n", __func__, __LINE__);
                memcpy(dcc_buf, dcc_viss_imx390_discovery_narrow_wdr, num_bytes);
            }
            else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_WIDE))
                memcpy(dcc_buf, dcc_viss_imx390_discovery_wide_wdr, num_bytes);
#endif
            else if (0 == strcmp(sensor_name, SENSOR_ONSEMI_AR0233_UB953_MARS))
                memcpy(dcc_buf, dcc_viss_ar0233_wdr, num_bytes);
            else if (0 == strcmp(sensor_name, SENSOR_ONSEMI_AR0820_UB953_LI))
                memcpy(dcc_buf, dcc_viss_ar0820_wdr, num_bytes);
            else
                return -1;
            break;
        default:
            /*Unsupported mode*/
            return -1;
    }

    return 0;
}

int32_t appIssGetDCCSize2A(char * sensor_name, uint32_t wdr_mode)
{
    int32_t size = -1;
    switch(wdr_mode)
    {
        case 0:
            /*Linear mode*/
            if(0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_D3))
                size = DCC_2A_IMX390_DCC_CFG_NUM_ELEM;
#if defined (SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J784S4)
            else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_MID))
                size = DCC_2A_IMX390_DISCOVERY_MID_DCC_CFG_NUM_ELEM;
            else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_NARROW)){
                printf("********************imx390 Discovery %s [%d]********************\n", __func__, __LINE__);
                size = DCC_2A_IMX390_DISCOVERY_NARROW_DCC_CFG_NUM_ELEM;
            }
            else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_WIDE))
                size = DCC_2A_IMX390_DISCOVERY_WIDE_DCC_CFG_NUM_ELEM;
#endif
            else if (0 == strcmp(sensor_name, SENSOR_ONSEMI_AR0233_UB953_MARS))
                size = DCC_2A_AR0233_DCC_CFG_NUM_ELEM;
            else if (0 == strcmp(sensor_name, SENSOR_ONSEMI_AR0820_UB953_LI))
                size = DCC_2A_AR0820_DCC_CFG_NUM_ELEM;
            else if (0 == strcmp(sensor_name, UB9XX_RAW_TESTPAT))
                size = DCC_2A_UB9XX_RAW_TEST_PATTERN_DCC_CFG_NUM_ELEM;
            else
                size = -1;
            break;
        case 1:
            /*WDR mode*/
            if(0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_D3))
                size = DCC_2A_IMX390_WDR_DCC_CFG_NUM_ELEM;
#if defined (SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J784S4)
            else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_MID))
                size = DCC_2A_IMX390_DISCOVERY_MID_WDR_DCC_CFG_NUM_ELEM;
            else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_NARROW)) {
                printf("********************imx390 Discovery %s [%d]********************\n", __func__, __LINE__);
                size = DCC_2A_IMX390_DISCOVERY_NARROW_WDR_DCC_CFG_NUM_ELEM;
            }
            else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_WIDE))
                size = DCC_2A_IMX390_DISCOVERY_WIDE_WDR_DCC_CFG_NUM_ELEM;
#endif
            else if (0 == strcmp(sensor_name, SENSOR_ONSEMI_AR0233_UB953_MARS))
                size = DCC_2A_AR0233_WDR_DCC_CFG_NUM_ELEM;
            else if (0 == strcmp(sensor_name, SENSOR_ONSEMI_AR0820_UB953_LI))
                size = DCC_2A_AR0820_WDR_DCC_CFG_NUM_ELEM;
            else
                size = -1;
            break;
        default:
            /*Unsupported mode*/
            size = -1;
    }
    return size;
}

int32_t appIssGetDCCBuff2A(char * sensor_name, uint32_t wdr_mode,  uint8_t * dcc_buf, int32_t num_bytes)
{
    switch(wdr_mode)
    {
        case 0:
            /*Linear mode*/
            if(0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_D3))
                memcpy(dcc_buf, dcc_2a_imx390, num_bytes);
#if defined (SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J784S4)
            else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_MID))
                memcpy(dcc_buf, dcc_2a_imx390_discovery_mid, num_bytes);
            else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_NARROW)){
                printf("********************imx390 Discovery %s [%d]********************\n", __func__, __LINE__);
                memcpy(dcc_buf, dcc_2a_imx390_discovery_narrow, num_bytes);
            }
            else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_WIDE))
                memcpy(dcc_buf, dcc_2a_imx390_discovery_wide, num_bytes);
#endif
            else if (0 == strcmp(sensor_name, SENSOR_ONSEMI_AR0233_UB953_MARS))
                memcpy(dcc_buf, dcc_2a_ar0233_linear, num_bytes);
            else if (0 == strcmp(sensor_name, SENSOR_ONSEMI_AR0820_UB953_LI))
                memcpy(dcc_buf, dcc_2a_ar0820_linear, num_bytes);
            else if (0 == strcmp(sensor_name, UB9XX_RAW_TESTPAT))
                memcpy(dcc_buf, dcc_2a_ub9xx_raw_test_pattern_linear, num_bytes);
            else
                return -1;
            break;
        case 1:
            /*WDR mode*/
            if(0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_D3))
                memcpy(dcc_buf, dcc_2a_imx390_wdr, num_bytes);
#if defined (SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J784S4)
            else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_MID))
                memcpy(dcc_buf, dcc_2a_imx390_discovery_mid_wdr, num_bytes);
            else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_NARROW)) {
                printf("********************imx390 Discovery %s [%d]********************\n", __func__, __LINE__);
                memcpy(dcc_buf, dcc_2a_imx390_discovery_narrow_wdr, num_bytes);
            }
            else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_WIDE))
                memcpy(dcc_buf, dcc_2a_imx390_discovery_wide_wdr, num_bytes);
#endif
            else if (0 == strcmp(sensor_name, SENSOR_ONSEMI_AR0233_UB953_MARS))
                memcpy(dcc_buf, dcc_2a_ar0233_wdr, num_bytes);
            else if (0 == strcmp(sensor_name, SENSOR_ONSEMI_AR0820_UB953_LI))
                memcpy(dcc_buf, dcc_2a_ar0820_wdr, num_bytes);
            else
                return -1;
            break;
        default:
            /*Unsupported mode*/
            return -1;
    }

    return 0;
}

int32_t appIssGetDCCSizeLDC(char * sensor_name, uint32_t wdr_mode)
{
    int32_t size = -1;

    if (0 == strcmp(sensor_name, UB96X_TESTPATTERN_UYVY))
    {
        size = DCC_LDC_UB96X_UYVYTESTPAT_DCC_CFG_NUM_ELEM;
    }
    else if (0 == strcmp(sensor_name, GW_AR0233_UYVY))
    {
        size = DCC_LDC_GW_AR0233_DCC_CFG_NUM_ELEM;
    }
    else
    {
        switch(wdr_mode)
        {
            case 0:
                /*Linear mode*/
                if(0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_D3))
                    size = DCC_LDC_IMX390_DCC_CFG_NUM_ELEM;
#if defined (SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J784S4)
                else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_MID))
                    size = DCC_LDC_IMX390_DISCOVERY_MID_DCC_CFG_NUM_ELEM;
                else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_NARROW)){
                    printf("********************imx390 Discovery %s [%d]********************\n", __func__, __LINE__);
                    size = DCC_LDC_IMX390_DISCOVERY_NARROW_DCC_CFG_NUM_ELEM;
                }
                else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_WIDE))
                    size = DCC_LDC_IMX390_DISCOVERY_WIDE_DCC_CFG_NUM_ELEM;
#endif
                else if (0 == strcmp(sensor_name, SENSOR_ONSEMI_AR0233_UB953_MARS))
                    size = DCC_LDC_AR0233_DCC_CFG_NUM_ELEM;
                else if (0 == strcmp(sensor_name, SENSOR_ONSEMI_AR0820_UB953_LI))
                    size = DCC_LDC_AR0820_DCC_CFG_NUM_ELEM;
                else if (0 == strcmp(sensor_name, UB9XX_RAW_TESTPAT))
                    size = DCC_LDC_UB9XX_RAW_TEST_PATTERN_DCC_CFG_NUM_ELEM;
                else
                    size = -1;
                break;
            case 1:
                /*WDR mode*/
                if(0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_D3))
                    size = DCC_LDC_IMX390_WDR_DCC_CFG_NUM_ELEM;
#if defined (SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J784S4)
                else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_MID))
                    size = DCC_LDC_IMX390_DISCOVERY_MID_WDR_DCC_CFG_NUM_ELEM;
                else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_NARROW)){
                    printf("********************imx390 Discovery %s [%d]********************\n", __func__, __LINE__);
                    size = DCC_LDC_IMX390_DISCOVERY_NARROW_WDR_DCC_CFG_NUM_ELEM;
                }
                else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_WIDE))
                    size = DCC_LDC_IMX390_DISCOVERY_WIDE_WDR_DCC_CFG_NUM_ELEM;
#endif
                else if (0 == strcmp(sensor_name, SENSOR_ONSEMI_AR0233_UB953_MARS))
                    size = DCC_LDC_AR0233_WDR_DCC_CFG_NUM_ELEM;
                else if (0 == strcmp(sensor_name, SENSOR_ONSEMI_AR0820_UB953_LI))
                    size = DCC_LDC_AR0820_WDR_DCC_CFG_NUM_ELEM;
                else
                    size = -1;
                break;
            default:
                /*Unsupported mode*/
                size = -1;
        }

    }

    return size;
}

int32_t appIssGetDCCBuffLDC(char * sensor_name, uint32_t wdr_mode,  uint8_t * dcc_buf, int32_t num_bytes)
{
    if (0 == strcmp(sensor_name, UB96X_TESTPATTERN_UYVY))
    {
        memcpy(dcc_buf, dcc_ldc_ub96x_uyvytestpat, num_bytes);
    }
    else if (0 == strcmp(sensor_name, GW_AR0233_UYVY))
    {
        memcpy(dcc_buf, dcc_ldc_gw_ar0233, num_bytes);
    }
    else
    {
        switch(wdr_mode)
        {
            case 0:
                /*Linear mode*/
                if(0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_D3))
                    memcpy(dcc_buf, dcc_ldc_imx390, num_bytes);
#if defined (SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J784S4)
                else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_MID))
                    memcpy(dcc_buf, dcc_ldc_imx390_discovery_mid, num_bytes);
                else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_NARROW)){
                    printf("********************imx390 Discovery %s [%d]********************\n", __func__, __LINE__);
                    memcpy(dcc_buf, dcc_ldc_imx390_discovery_narrow, num_bytes);
                }
                else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_WIDE))
                    memcpy(dcc_buf, dcc_ldc_imx390_discovery_wide, num_bytes);                    
#endif
                else if (0 == strcmp(sensor_name, SENSOR_ONSEMI_AR0233_UB953_MARS))
                    memcpy(dcc_buf, dcc_ldc_ar0233_linear, num_bytes);
                else if (0 == strcmp(sensor_name, SENSOR_ONSEMI_AR0820_UB953_LI))
                    memcpy(dcc_buf, dcc_ldc_ar0820_linear, num_bytes);
                else if (0 == strcmp(sensor_name, UB9XX_RAW_TESTPAT))
                    memcpy(dcc_buf, dcc_ldc_ub9xx_raw_test_pattern_linear, num_bytes);
                else
                    return -1;
                break;
            case 1:
                /*WDR mode*/
                if(0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_D3))
                    memcpy(dcc_buf, dcc_ldc_imx390_wdr, num_bytes);
#if defined (SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J784S4)
                else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_MID))
                    memcpy(dcc_buf, dcc_ldc_imx390_discovery_mid_wdr, num_bytes);
                else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_NARROW)){
                    printf("********************imx390 Discovery %s [%d]********************\n", __func__, __LINE__);
                    memcpy(dcc_buf, dcc_ldc_imx390_discovery_narrow_wdr, num_bytes);
                }
                else if (0 == strcmp(sensor_name, SENSOR_SONY_IMX390_UB953_DISCOVERY_WIDE))
                    memcpy(dcc_buf, dcc_ldc_imx390_discovery_wide_wdr, num_bytes);
#endif
                else if (0 == strcmp(sensor_name, SENSOR_ONSEMI_AR0233_UB953_MARS))
                    memcpy(dcc_buf, dcc_ldc_ar0233_wdr, num_bytes);
                else if (0 == strcmp(sensor_name, SENSOR_ONSEMI_AR0820_UB953_LI))
                    memcpy(dcc_buf, dcc_ldc_ar0820_wdr, num_bytes);
                else
                    return -1;
                break;
            default:
                /*Unsupported mode*/
                return -1;
        }
    }

    return 0;
}

int32_t appIssGetResizeParams(uint16_t in_width, uint16_t in_height, uint16_t tgt_width, uint16_t tgt_height, uint16_t * out_width, uint16_t * out_height)
{
    uint32_t tmp = 0;
    uint32_t rsz_ratio = 1024;
    uint32_t rsz_ratio_x = (tgt_width*1024)/in_width;
    uint32_t rsz_ratio_y = (tgt_height*1024)/in_height;

    /*Assuming downscale. Will pick the ratio which has a bigger downscale factor*/
    /*X and Y dimensions will be scaled by same factor to maintain aspect ratio*/
    rsz_ratio = rsz_ratio_x < rsz_ratio_y ? rsz_ratio_x : rsz_ratio_y;
    tmp = (in_width*rsz_ratio)/1024;
    *out_width = (tmp>>3)<<3;/*Align to 8x*/
    tmp = (in_height*rsz_ratio)/1024;
    *out_height = (tmp>>3)<<3;/*Align to 8x*/

    return 0;
}

/*
Returns the sum of sizes all .bin files inside a specified directory.
Does not look inside subfolders.
*/
int32_t get_dcc_dir_size(char* dcc_folder_path)
{
    DIR *d;
    char file_path[MAX_FILE_NAME_LEN];
    struct dirent *dir;
    struct stat FileAttrib;
    int32_t size = 0;
    d = opendir(dcc_folder_path);
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            char *dot = strrchr(dir->d_name, '.');
            if (dot && (strcmp(dot, ".bin") == 0))
            {
                snprintf(file_path, MAX_FILE_NAME_LEN, "%s/%s", dcc_folder_path, dir->d_name);
                stat(file_path, &FileAttrib);
                size += FileAttrib.st_size;
            }
        }
        closedir(d);
    }
    else
    {
        printf("get_dcc_dir_size : Could not open directory or directory is empty %s \n",
        dcc_folder_path);
    }

    return size;
}

/*
Returns the contents of all .bin files inside a specified directory.
Does not look inside subfolders.
*/
int32_t get_dcc_dir_data(char* dcc_folder_path, uint8_t * dcc_buf)
{
    DIR *d;
    char file_path[MAX_FILE_NAME_LEN];
    struct dirent *dir;
    struct stat FileAttrib;
    int32_t offset = 0;
    int32_t bytes_read = 0;
    FILE * fp;
    d = opendir(dcc_folder_path);
    printf("get_dcc_dir_data : %s \n", dcc_folder_path);
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            char *dot = strrchr(dir->d_name, '.');
            if (dot && (strcmp(dot, ".bin") == 0))
            {
                snprintf(file_path, MAX_FILE_NAME_LEN, "%s/%s", dcc_folder_path, dir->d_name);
                fp = fopen(file_path, "rb");
                if(NULL == fp)
                {
                    printf("Error : Unable to open %s \n", file_path);
                    continue;
                }
                stat(file_path, &FileAttrib);
                bytes_read = fread(dcc_buf+offset, sizeof(char), FileAttrib.st_size, fp);
                printf("read %d bytes from %s\n", bytes_read, dir->d_name);
                offset += bytes_read;
                fclose(fp);
            }
        }
        closedir(d);
    }
    return offset;
}

/*
Checks if the plugin is relevant to VISS. Returns
    0, if False
    1, if True
*/
uint8_t is_viss_plugin(uint32_t plugin_id)
{
    switch (plugin_id)
    {
        case DCC_ID_H3A_AEWB_CFG:
        case DCC_ID_H3A_MUX_LUTS:
        case DCC_ID_RFE_DECOMPAND:
        case DCC_ID_IPIPE_RGB_RGB_1:
        case DCC_ID_NSF4:
        case DCC_ID_BLACK_CLAMP:
        case DCC_ID_IPIPE_CFA:
        case DCC_ID_VISS_GLBCE:
        case DCC_ID_VISS_LSC:
        case DCC_ID_VISS_DPC:
        case DCC_ID_VISS_YEE:
        case DCC_ID_RAWFE_WB1_VS:
        case DCC_ID_VISS_GAMMA:
#ifdef VPAC3
        case DCC_ID_VISS_CFAI3_A:
        case DCC_ID_VISS_CFAI3_B:
        case DCC_ID_VISS_CAC:
        case DCC_ID_VISS_RAWHIST:
        case DCC_ID_VISS_CCMV:
#endif
#ifdef VPAC3L
        case DCC_ID_VISS_CFAI3_A:
        case DCC_ID_VISS_CAC:
        case DCC_ID_VISS_RAWHIST:
        case DCC_ID_VISS_DPC_EXT:
        case DCC_ID_VISS_LSC_EXT:
        case DCC_ID_VISS_PCID:
#endif
            return 1U;
            break;
        default:
            return 0;
    }
    return 0;
}

/*
Checks if the plugin is relevant to AEWB. Returns
    0, if False
    1, if True
*/
uint8_t is_aewb_plugin(uint32_t plugin_id)
{
    switch (plugin_id)
    {
        case DCC_ID_H3A_AEWB_CFG:
        case DCC_ID_AAA_ALG_AWB_TI3:
            return 1U;
            break;
        default:
            return 0;
    }
    return 0;
}

/*
Checks if the plugin is relevant to LDC. Returns
    0, if False
    1, if True
*/
uint8_t is_ldc_plugin(uint32_t plugin_id)
{
    switch (plugin_id)
    {
        case DCC_ID_MESH_LDC_J7:
            return 1U;
            break;
        default:
            return 0;
    }
    return 0;
}

int32_t appSplitVpacDcc(uint8_t *dcc_buf_in, uint32_t prmSize,
                        uint8_t ** dcc_buf_viss, uint32_t *dcc_buf_viss_num_bytes,
                        uint8_t ** dcc_buf_aewb, uint32_t *dcc_buf_aewb_num_bytes,
                        uint8_t ** dcc_buf_ldc, uint32_t *dcc_buf_ldc_num_bytes)
{
    int32_t status = 0;
    uint8_t *tmp_buf;
    uint32_t offset_2a = 0;
    uint32_t offset_viss = 0;
    uint32_t offset_ldc = 0;
    uint8_t *pluginBuf;
    dcc_component_header_type     *header_data;
    uint32_t offset = 0;

    tmp_buf = malloc(prmSize);
    if(NULL == tmp_buf)
    {
        printf("Failed to allocate %d bytes for dcc_buf_2a \n", prmSize);
        return -1;
    }
    else
    {
        *dcc_buf_aewb = tmp_buf;
        tmp_buf = NULL;
    }

    tmp_buf = malloc(prmSize);
    if(NULL == tmp_buf)
    {
        printf("Failed to allocate %d bytes for dcc_buf_viss \n", prmSize);
        free(*dcc_buf_aewb);
        return -1;
    }
    else
    {
        *dcc_buf_viss = tmp_buf;
        tmp_buf = NULL;
    }

    tmp_buf = malloc(prmSize);
    if(NULL == tmp_buf)
    {
        printf("Failed to allocate %d bytes for dcc_buf_ldc \n", prmSize);
        free(*dcc_buf_aewb);
        free(*dcc_buf_viss);
        return -1;
    }
    else
    {
        *dcc_buf_ldc = tmp_buf;
        tmp_buf = NULL;
    }

    pluginBuf = dcc_buf_in;

    while (offset < prmSize)
    {
        header_data = (dcc_component_header_type*)pluginBuf;

        if(1U == is_viss_plugin(header_data->dcc_descriptor_id))
        {
            memcpy(*dcc_buf_viss + offset_viss, pluginBuf, header_data->total_file_sz);
            offset_viss += header_data->total_file_sz;
        }

        if(1U == is_aewb_plugin(header_data->dcc_descriptor_id))
        {
            memcpy(*dcc_buf_aewb + offset_2a, pluginBuf, header_data->total_file_sz);
            offset_2a += header_data->total_file_sz;
        }

        if(1U == is_ldc_plugin(header_data->dcc_descriptor_id))
        {
            memcpy(*dcc_buf_ldc + offset_ldc, pluginBuf, header_data->total_file_sz);
            offset_ldc += header_data->total_file_sz;
        }

        pluginBuf += header_data->total_file_sz;
        offset += header_data->total_file_sz;
    }

    *dcc_buf_viss_num_bytes = offset_viss;
    *dcc_buf_ldc_num_bytes = offset_ldc;
    *dcc_buf_aewb_num_bytes = offset_2a;
    return status;
}

#if defined(LINUX)
/*
    This function receives a buffer containing VPAC tuning data.
    The data may have been received through ITT interface or file read.
    It may contain tuning parameters for one or more plugins belonging to
    one or more of the following nodes
        VISS
        AEWB
        LDC

    In case of a multi-camera system, the nodes maybe replicated, in which
    case an addtional parameter node_index is required. Single camera always
    passes this value as 0.

    User is free to send the tuning data in any order. This function is responsible to
        Split the tuning database into VISS, AEWB and LDC
        Send the tuning data to all the nodes one by one
*/
int32_t appUpdateVpacDcc(uint8_t *dcc_buf, uint32_t prmSize, vx_context context,
                        vx_node viss_node, uint32_t viss_node_index,
                        vx_node aewb_node, uint32_t aewb_node_index,
                        vx_node ldc_node, uint32_t ldc_node_index
                    )
{
    uint8_t* dcc_buf_viss;
    uint32_t dcc_numbytes_viss;
    uint8_t* dcc_buf_2a;
    uint32_t dcc_numbytes_2a;
    uint8_t* dcc_buf_ldc;
    uint32_t dcc_numbytes_ldc;
    int32_t status = 0;

    status = appSplitVpacDcc(dcc_buf, prmSize,
                        &dcc_buf_viss, &dcc_numbytes_viss,
                        &dcc_buf_2a, &dcc_numbytes_2a,
                        &dcc_buf_ldc, &dcc_numbytes_ldc);

    if(status == 0)
    {
        if((NULL != viss_node) && (dcc_numbytes_viss > 0))
        {
            status = appDccUpdateNode(dcc_buf_viss, dcc_numbytes_viss, viss_node, viss_node_index, context);
        }

        if((NULL != aewb_node) && (dcc_numbytes_2a > 0))
        {
            status = appDccUpdateNode(dcc_buf_2a, dcc_numbytes_2a, aewb_node, aewb_node_index, context);
        }

        if((NULL != ldc_node) && (dcc_numbytes_ldc > 0))
        {
            status = appDccUpdateNode(dcc_buf_ldc, dcc_numbytes_ldc, ldc_node, ldc_node_index, context);
        }

        if(dcc_buf_2a)
            free(dcc_buf_2a);

        if(dcc_buf_viss)
            free(dcc_buf_viss);

        if(dcc_buf_ldc)
            free(dcc_buf_ldc);

    }


    return status;
}

/*
    This function reads tuning database from the file system.
    The binary data read is then sent to VPAC nodes using appUpdateVpacDcc.

    The database is expected under the path
    "/opt/vision_apps/dcc/<sensor_name>/<operating_mode>", where
        sensor_name is the string received by querying sensor properties.
        operating_mode is "wdr" or "linear" as set by the application.

    For e.g. IMX390 DCC database in WDR mode will be expected under the path
    /opt/vision_apps/dcc/IMX390-UB953_D3/wdr
*/
int32_t appDccUpdatefromFS(char* sensor_name, uint8_t wdr_mode,
                        vx_node node_viss, uint32_t viss_node_index,
                        vx_node node_aewb, uint32_t aewb_node_index,
                        vx_node node_ldc, uint32_t ldc_node_index,
                        vx_context context)
{
    char dcc_dir_path[MAX_FOLDER_NAME_LEN];
     char op_mode[8];
    int dcc_num_bytes;
    int dcc_bytes_read;
    uint8_t * dcc_buf;
    switch(wdr_mode)
    {
        case 0:
            /*Linear mode*/
            snprintf(op_mode, 8, "%s", "linear");
        default:
            /*WDR mode*/
            snprintf(op_mode, 8, "%s", "wdr");
    }

    snprintf(dcc_dir_path, MAX_FOLDER_NAME_LEN, "%s/%s/%s", DCC_ROOT, sensor_name, op_mode);
    dcc_num_bytes = get_dcc_dir_size(dcc_dir_path);
    if(dcc_num_bytes > 0 )
    {
        dcc_buf = malloc(dcc_num_bytes);
        if(NULL != dcc_buf)
        {
            dcc_bytes_read = get_dcc_dir_data(dcc_dir_path, dcc_buf);
            if(dcc_bytes_read > 0)
            {
                appUpdateVpacDcc(dcc_buf, dcc_num_bytes, context,
                    node_viss, viss_node_index,
                    node_aewb, aewb_node_index,
                    node_ldc, ldc_node_index
                );
            }
            free(dcc_buf);
        }
        else
        {
            printf("Error allocating %d bytes\n", dcc_num_bytes);
        }
    }

    return 0;
}

/*
    This function send node specific tuning data to the node using the control
    command ISS_CMD_SET_DCC_PARAMS.
*/
int32_t appDccUpdateNode(uint8_t * dcc_buf, int32_t num_bytes, vx_node node, uint32_t replicate_nodex_idx, vx_context context)
{
    vx_status status = VX_SUCCESS;
    vx_reference refs[1];
    vx_user_data_object dcc_db_obj;
    dcc_db_obj = vxCreateUserDataObject(context, "vx_uint8", num_bytes, NULL);
    if(NULL != dcc_db_obj)
    {
        vxCopyUserDataObject(dcc_db_obj, 0, num_bytes, dcc_buf, VX_WRITE_ONLY, VX_MEMORY_TYPE_HOST);
        refs[0] = (vx_reference)dcc_db_obj;
        tivxNodeSendCommand(node, replicate_nodex_idx, ISS_CMD_SET_DCC_PARAMS, refs, 1u);
        vxReleaseUserDataObject(&dcc_db_obj);
    }
    else
    {
        printf("Error creating dcc_db_obj of size %d\n", num_bytes);
    }
    return status;
}
#endif //LINUX
