/*
 *
 * Copyright (c) 2020 Texas Instruments Incorporated
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
#include <iss_sensors.h>
#include <iss_sensor_if.h>
#include <iss_sensor_priv.h>
#include <app_remote_service.h>
#include <app_ipc.h>
#include "TI/tivx_mutex.h"

#if defined (j721e_evm)
#include <ti/board/src/j721e_evm/include/board_cfg.h>
#include <ti/board/src/j721e_evm/include/board_i2c_io_exp.h>
#elif defined (j721s2_evm)
#include <ti/board/src/j721s2_evm/include/board_cfg.h>
#include <ti/board/src/j721s2_evm/include/board_i2c_io_exp.h>
#elif defined (j784s4_evm)
#include <ti/board/src/j784s4_evm/include/board_cfg.h>
#include <ti/board/src/j784s4_evm/include/board_i2c_io_exp.h>
#endif

/**
 * @{
 * I2C Addresses for serialisers/Sensors attached to the UB960
 */
#define UB960_SERIALISER_ADDR                (0x18)
#define D3IMX390_SENSOR_ADDR_CM_MODULE       (0x21)
#define D3IMX390_SENSOR_ADDR_RCM_MODULE      (0x1A)
/** @} */

/**
 * @{
 * Generic Alias Addresses for serialisers attached to the UB960 and UB9702 Instance0
 */
#define D3IMX390_INST0_PORT_0_SER_ADDR       (0x74U)
#define D3IMX390_INST0_PORT_1_SER_ADDR       (0x76U)
#define D3IMX390_INST0_PORT_2_SER_ADDR       (0x78U)
#define D3IMX390_INST0_PORT_3_SER_ADDR       (0x7AU)
/** @} */

/**
 * @{
 * Generic Alias Addresses for sensors attached to the UB960 Instance0
 */
#define D3IMX390_INST0_PORT_0_SENSOR_ADDR    (0x40U)
#define D3IMX390_INST0_PORT_1_SENSOR_ADDR    (0x42U)
#define D3IMX390_INST0_PORT_2_SENSOR_ADDR    (0x44U)
#define D3IMX390_INST0_PORT_3_SENSOR_ADDR    (0x46U)
/** @} */

/**
 * @{
 * Generic Alias Addresses for serialisers attached to the UB960 Instance1
 */
#define D3IMX390_INST1_PORT_0_SER_ADDR       (0x6CU)
#define D3IMX390_INST1_PORT_1_SER_ADDR       (0x6EU)
#define D3IMX390_INST1_PORT_2_SER_ADDR       (0x70U)
#define D3IMX390_INST1_PORT_3_SER_ADDR       (0x72U)

/*******************************************************************************
 *  Globals
 *******************************************************************************
 */
IssSensors_Handle *gIssSensorTable[ISS_SENSORS_MAX_SUPPORTED_SENSOR];

#define DS90UB9702

/* Fusion2 board detect flag */
vx_bool gFusion2Det = vx_false_e;

#if defined (SOC_J722S)

#else
#include <ti/board/src/devices/fpd/ds90ub9702.h>
#endif

/*******************************************************************************
 *  Local Functions Declarations
 *******************************************************************************
 */
static int32_t checkForHandle(void* handle);

/*******************************************************************************
 *  Function Definition
 *******************************************************************************
 */
static I2C_Handle gISS_Sensor_I2cHandle = NULL;
static uint8_t gISS_Sensor_ByteOrder = BOARD_I2C_REG_ADDR_MSB_FIRST;
static uint8_t num_sensors_open = 0;
static IssSensors_Handle * g_pSenHndl[ISS_SENSORS_MAX_SUPPORTED_SENSOR];
static uint8_t g_detectedSensors[ISS_SENSORS_MAX_SUPPORTED_SENSOR];

static I2cParams ub9702_971_DesCfg_Common[] = {
    {0x01, 0x20, 0x20},
    /*PORT 0 config */
    {0x4C, 0x01, 0x20},
    {0xB0, 0x04, 0x20},
    {0xE4, 0x00, 0x10},
    {0x58, 0x5E, 0x10},
    {0xB1, 0x2B, 0x20},
    {0xB2, 0x04, 0x20},

    {0x4C, 0x01, 0x10}, /* 0x01 */
    {0x32, 0x03, 0x20}, /*Enable TX ports*/
    {0x33, 0x02, 0x1}, /*Enable Continuous clock mode and CSI output*/
    {0x5D, 0x30, 0x1}, /*Serializer I2C Address*/
    {0x65, 0xFF, 0x1},
    {0x5E, 0xFF, 0x1}, /*Sensor I2C Address*/
    {0x66, 0xFF, 0x1},
    {0x6D, 0x6C,0x0}, /*CSI Mode*/
    {0x72, 0x00,0x0}, /*VC Map - All to 0 */

    {0x4C, 0x01, 0x10},
    {0xB0, 0x04, 0x20},
    {0xB1, 0x04, 0x20},
    {0xB2, 0x00, 0x20},
    {0xB1, 0x1B, 0x20},
    {0xB2, 0x00, 0x20},
    {0xB1, 0x21, 0x20},
    {0xB2, 0x2F, 0x20},
    {0xB1, 0x25, 0x20},
    {0xB2, 0xC1, 0x20},
    {0x3C, 0x0F, 0x10},
    {0x0C, 0x0F, 0x20},
    {0xB2, 0x41, 0x10},
    {0x3C, 0x1F, 0x10},
    {0x01, 0x01, 0x20},

    /*PORT 1 config */
    {0x4C, 0x12, 0x20},
    {0xB0, 0x08, 0x20},
    {0xE4, 0x00, 0x10},
    {0x58, 0x5E, 0x10},
    {0xB1, 0x2B, 0x20},
    {0xB2, 0x04, 0x20},

    {0x4C, 0x12, 0x10}, /* 0x12 */
    {0x32, 0x03, 0x20}, /*Enable TX ports*/
    {0x33, 0x02, 0x1}, /*Enable Continuous clock mode and CSI output*/
    {0x5D, 0x30, 0x1}, /*Serializer I2C Address*/
    {0x65, 0xFF, 0x1},
    {0x5E, 0xFF, 0x1}, /*Sensor I2C Address*/
    {0x66, 0xFF, 0x1},
    {0x6D, 0x6C,0x0}, /*CSI Mode*/
    {0x72, 0x55,0x0}, /*VC Map - All to 1 */

    {0x4C, 0x12, 0x10},
    {0xB0, 0x08, 0x20},
    {0xB1, 0x04, 0x20},
    {0xB2, 0x00, 0x20},
    {0xB1, 0x1B, 0x20},
    {0xB2, 0x00, 0x20},
    {0xB1, 0x21, 0x20},
    {0xB2, 0x2F, 0x20},
    {0xB1, 0x25, 0x20},
    {0xB2, 0xC1, 0x20},
    {0x3C, 0x0F, 0x10},
    {0xB2, 0x41, 0x10},
    {0x3C, 0x1F, 0x10},
    {0x01, 0x01, 0x20},

    /*PORT 2 config */
    {0x4C, 0x24, 0x20},
    {0xB0, 0x0C, 0x20},
    {0xE4, 0x00, 0x10},
    {0x58, 0x5E, 0x10},
    {0xB1, 0x2B, 0x20},
    {0xB2, 0x04, 0x20},

    {0x4C, 0x24, 0x10}, /* 0x24 */
    {0x32, 0x03, 0x20}, /*Enable TX ports*/
    {0x33, 0x02, 0x1}, /*Enable Continuous clock mode and CSI output*/
    {0x5D, 0x30, 0x1}, /*Serializer I2C Address*/
    {0x65, 0xFF, 0x1},
    {0x5E, 0xFF, 0x1}, /*Sensor I2C Address*/
    {0x66, 0xFF, 0x1},
    {0x6D, 0x6C,0x0}, /*CSI Mode*/
    {0x72, 0xaa,0x0}, /*VC Map - All to 2 */

    {0x4C, 0x25, 0x10},
    {0xB0, 0x0C, 0x20},
    {0xB1, 0x04, 0x20},
    {0xB2, 0x00, 0x20},
    {0xB1, 0x1B, 0x20},
    {0xB2, 0x00, 0x20},
    {0xB1, 0x21, 0x20},
    {0xB2, 0x2F, 0x20},
    {0xB1, 0x25, 0x20},
    {0xB2, 0xC1, 0x20},
    {0x3C, 0x0F, 0x10},
    {0xB2, 0x41, 0x10},
    {0x3C, 0x1F, 0x10},
    {0x01, 0x01, 0x20},

    /*PORT 3 config */
    {0x4C, 0x38, 0x20},
    {0xB0, 0x10, 0x20},
    {0xE4, 0x00, 0x10},
    {0x58, 0x5E, 0x10},
    {0xB1, 0x2B, 0x20},
    {0xB2, 0x04, 0x20},

    {0x4C, 0x38, 0x10}, /* 0x38 */
    {0x32, 0x03, 0x20}, /*Enable TX ports*/
    {0x33, 0x02, 0x1}, /*Enable Continuous clock mode and CSI output*/
    {0x5D, 0x30, 0x1}, /*Serializer I2C Address*/
    {0x65, 0xFF, 0x1},
    {0x5E, 0xFF, 0x1}, /*Sensor I2C Address*/
    {0x66, 0xFF, 0x1},
    {0x6D, 0x6C,0x0}, /*CSI Mode*/
    {0x72, 0xFF,0x0}, /*VC Map - All to 3 */

    {0x4C, 0x38, 0x10},
    {0xB0, 0x10, 0x20},
    {0xB1, 0x04, 0x20},
    {0xB2, 0x00, 0x20},
    {0xB1, 0x1B, 0x20},
    {0xB2, 0x00, 0x20},
    {0xB1, 0x21, 0x20},
    {0xB2, 0x2F, 0x20},
    {0xB1, 0x25, 0x20},
    {0xB2, 0xC1, 0x20},
    {0x3C, 0x0F, 0x10},
    {0xB2, 0x41, 0x10},
    {0x3C, 0x1F, 0x10},
    {0x01, 0x01, 0x20},

    {0xC2, 0x00, 0x10},

    {0x32, 0x03, 0x20},
    {0x1F, 0x10, 0x20},
    {0xC9, 0x32, 0x20},
    {0xB0, 0x1C, 0x20},
    {0xB1, 0x92, 0x20},
    {0xB2, 0x40, 0x20},
    {0xC7, 0x10, 0x20},
    {0x20, 0xF0, 0x20},
    {0x3C, 0xAA, 0x10},
    {0x20, 0x00, 0x20},

    {0x01, 0x21, 0x100},
    {0xFFFF, 0x00, 0x0} //End of script
};

static I2cParams ub9702DesCfg_Common[] = {
    {0x01, 0x02, 0x20},
    {0x0C, 0x0F, 0x20},

    /*Sets GPIOS*/
    {0x10, 0x83, 0x1},
    {0x11, 0xA3, 0x1},
    {0x12, 0xC3, 0x1},
    {0x13, 0xE3, 0x1},

    /*Port 0 Config*/
    {0x4C, 0x01, 0x20},
    {0xB0, 0x04, 0x20},
    {0xE4, 0x02, 0x10},
    {0x58, 0x5E, 0x10},

    {0x4C, 0x01, 0x10}, /* 0x01 */
    {0x0C, 0x0F, 0x20},
    {0x32, 0x03, 0x20}, /*Enable TX port 0*/
    {0x33, 0x02, 0x1}, /*Enable Continuous clock mode and CSI output*/
    {0x5D, 0x30, 0x1}, /*Serializer I2C Address*/
    {0x65, 0xFF, 0x1},
    {0x5E, 0xFF, 0x1}, /*Sensor I2C Address*/
    {0x66, 0xFF, 0x1},
    {0x6D, 0x6C,0x0}, /*CSI Mode*/
    {0xA0, 0x10,0x0}, /*VC Map - All to 0 */

    {0x4C, 0x01, 0x20},
    {0xB0, 0x04, 0x20},
    {0xB1, 0xA8, 0x20},
    {0xB2, 0x80, 0x20},
    {0xB1, 0x1B, 0x20},
    {0xB2, 0x08, 0x20},
    {0xB1, 0x0D, 0x20},
    {0xB2, 0x7F, 0x20},
    {0x01, 0x01, 0x10},

    /*Port 1 Config*/
    {0x4C, 0x12, 0x20},
    {0xB0, 0x08, 0x20},
    {0xE4, 0x02, 0x10},
    {0x58, 0x5E, 0x10},

    {0x4C, 0x12, 0x10}, /* 0x12 */
    {0x0C, 0x0F, 0x20},
    {0x32, 0x03, 0x20}, /*Enable TX ports*/
    {0x33, 0x02, 0x1}, /*Enable Continuous clock mode and CSI output*/
    {0x5D, 0x30, 0x1}, /*Serializer I2C Address*/
    {0x65, 0xFF, 0x1},
    {0x5E, 0xFF, 0x1}, /*Sensor I2C Address*/
    {0x66, 0xFF, 0x1},
    {0x6D, 0x6C, 0x0}, /*CSI Mode*/
    {0xA0, 0x11, 0x0}, /*VC Map - All to 1 */

    {0x4C, 0x12, 0x20},
    {0xB0, 0x08, 0x20},
    {0xB1, 0xA8, 0x20},
    {0xB2, 0x80, 0x20},
    {0xB1, 0x1B, 0x20},
    {0xB2, 0x08, 0x20},
    {0xB1, 0x0D, 0x20},
    {0xB2, 0x7F, 0x20},
    {0x01, 0x01, 0x10},

    /*Port 2 Config*/
    {0x4C, 0x24, 0x20},
    {0xB0, 0x0C, 0x20},
    {0xE4, 0x02, 0x10},
    {0x58, 0x5E, 0x10},

    {0x4C, 0x24, 0x10}, /* 0x24 */
    {0x0C, 0x0F, 0x20},
    {0x32, 0x03, 0x20}, /*Enable TX ports*/
    {0x33, 0x02, 0x1}, /*Enable Continuous clock mode and CSI output*/
    {0x5D, 0x30, 0x1}, /*Serializer I2C Address*/
    {0x65, 0xFF, 0x1},
    {0x5E, 0xFF, 0x1}, /*Sensor I2C Address*/
    {0x66, 0xFF, 0x1},
    {0x6D, 0x6C, 0x0}, /*CSI Mode*/
    {0xA0, 0x12, 0x0}, /*VC Map - All to 2 */

    {0x4C, 0x24, 0x20},
    {0xB0, 0x0C, 0x20},
    {0xB1, 0xA8, 0x20},
    {0xB2, 0x80, 0x20},
    {0xB1, 0x1B, 0x20},
    {0xB2, 0x08, 0x20},
    {0xB1, 0x0D, 0x20},
    {0xB2, 0x7F, 0x20},
    {0x01, 0x01, 0x10},

     /*Port 3 Config*/
    {0x4C, 0x38, 0x20},
    {0xB0, 0x10, 0x20},
    {0xE4, 0x02, 0x10},
    {0x58, 0x5E, 0x10},

    {0x4C, 0x38, 0x10}, /* 0x12 */
    {0x0C, 0x0F, 0x20},
    {0x32, 0x03, 0x20}, /*Enable TX ports*/
    {0x33, 0x02, 0x1}, /*Enable Continuous clock mode and CSI output*/
    {0x5D, 0x30, 0x1}, /*Serializer I2C Address*/
    {0x65, 0xFF, 0x1},
    {0x5E, 0xFF, 0x1}, /*Sensor I2C Address*/
    {0x66, 0xFF, 0x1},
    {0x6D, 0x6C, 0x0}, /*CSI Mode*/
    {0xA0, 0x13, 0x0}, /*VC Map - All to 3 */

    {0x4C, 0x38, 0x20},
    {0xB0, 0x10, 0x20},
    {0xB1, 0xA8, 0x20},
    {0xB2, 0x80, 0x20},
    {0xB1, 0x1B, 0x20},
    {0xB2, 0x08, 0x20},
    {0xB1, 0x0D, 0x20},
    {0xB2, 0x7F, 0x20},
    {0x01, 0x01, 0x10},

    {0x32, 0x03, 0x20},
    {0x1F, 0x00, 0x20},
    {0xC9, 0x0F, 0x20},
    {0xB0, 0x1C, 0x20},
    {0xB1, 0x92, 0x20},
    {0xB2, 0x80, 0x20},
    {0x20, 0xF0, 0x20},
    {0xC7, 0x10, 0x20},
    {0x33, 0x03, 0x20},
    {0x20, 0x00, 0x20},
    {0x3C, 0x9F, 0x10},

    {0x01, 0x01, 0x10},
    {0xFFFF, 0x00, 0x0} //End of script
};

static I2cParams ub960DesCfg_Common[] = {
    {0x01, 0x02, 0x20},
    {0x1f, 0x00, 0x00},

    {0x0D, 0x90, 0x1}, /*I/O to 3V3 - Options not valid with datashee*/
    {0x0C, 0x0F, 0x1}, /*Enable All ports*/

    /*Select Channel 0*/                                               
    {0x4C, 0x01, 0x10},
    {0x58, 0x5E, 0x1}, /*Enable Back channel, set to 50Mbs*/
    {0x72, 0x00, 0x1}, /*VC map*/

    /*Select Channel 1*/
    {0x4C, 0x12, 0x10},
    {0x58, 0x5E, 0x1},/*Enable Back channel, set to 50Mbs*/

    /*Select Channel 2*/
    {0x4C, 0x24, 0x10},
    {0x58, 0x5E, 0x1},/*Enable Back channel, set to 50Mbs*/
   
    /*Select Channel 3*/
    {0x4C, 0x38, 0x10},
    {0x58, 0x5E, 0x1},/*Enable Back channel, set to 50Mbs*/
  
    {0x20, 0x00, 0x1}, /*Forwarding and using CSIport 0 */

    /*Sets GPIOS*/     
    {0x10, 0x83, 0x1},
    {0x11, 0xA3, 0x1},
    {0x12, 0xC3, 0x1},
    {0x13, 0xE3, 0x1},

    {0x4C, 0x01, 0x10}, /* 0x01 */
    {0x32, 0x01, 0x1}, /*Enable TX port 0*/
    {0x33, 0x02, 0x1}, /*Enable Continuous clock mode and CSI output*/
    {0xBC, 0x00, 0x1}, /*Unknown*/
    {0x5D, 0x30, 0x1}, /*Serializer I2C Address*/
    {0x65, 0xFF, 0x1},
    {0x5E, 0xFF, 0x1}, /*Sensor I2C Address*/
    {0x66, 0xFF, 0x1},
    {0x6D, 0x6C,0x0}, /*CSI Mode*/
    {0x72, 0x00,0x0}, /*VC Map - All to 0 */

    {0x4C, 0x12, 0x10}, /* 0x12 */
    {0x32, 0x01, 0x1}, /*Enable TX port 0*/
    {0x33, 0x02, 0x1}, /*Enable Continuous clock mode and CSI output*/
    {0xBC, 0x00, 0x1}, /*Unknown*/
    {0x5D, 0x30, 0x1}, /*Serializer I2C Address*/
    {0x65, 0xFF, 0x1},
    {0x5E, 0xFF, 0x1}, /*Sensor I2C Address*/
    {0x66, 0xFF, 0x1},
    {0x6D, 0x6C,0x0}, /*CSI Mode*/
    {0x72, 0x55,0x0}, /*VC Map - All to 1 */

    {0x4C, 0x24, 0x10}, /* 0x24 */
    {0x32, 0x01, 0x1}, /*Enable TX port 0*/
    {0x33, 0x02, 0x1}, /*Enable Continuous clock mode and CSI output*/
    {0xBC, 0x00, 0x1}, /*Unknown*/
    {0x5D, 0x30, 0x1}, /*Serializer I2C Address*/
    {0x65, 0xFF, 0x1},
    {0x5E, 0xFF, 0x1}, /*Sensor I2C Address*/
    {0x66, 0xFF, 0x1},
    {0x6D, 0x6C,0x0}, /*CSI Mode*/
    {0x72, 0xaa,0x0}, /*VC Map - All to 2 */

    {0x4C, 0x38, 0x10}, /* 0x38 */
    {0x32, 0x01, 0x1}, /*Enable TX port 0*/
    {0x33, 0x02, 0x1}, /*Enable Continuous clock mode and CSI output*/
    {0xBC, 0x00, 0x1}, /*Unknown*/
    {0x5D, 0x30, 0x1}, /*Serializer I2C Address*/
    {0x65, 0xFF, 0x1},
    {0x5E, 0xFF, 0x1}, /*Sensor I2C Address*/
    {0x66, 0xFF, 0x1},
    {0x6D, 0x6C,0x0}, /*CSI Mode*/
    {0x72, 0xFF,0x0}, /*VC Map - All to 3 */
    {0xFFFF, 0x00, 0x0} //End of script
};

#if defined (MCU_PLUS_SDK)
/**
 * \brief I2C 16-bit register read fuunction
 *
 * This function is used to read the 16-bit data from the i2c
 * device registers
 *
 * \param   handle      [IN]    Low level driver handle
 * \param   slaveAddr   [IN]    I2C slave address
 * \param   regAddr     [IN]    I2C register offset address
 * \param   regData     [OUT]   I2C register data buffer
 * \param   numOfBytes  [IN]    Receive data width
 * \param   byteOrdSel  [IN]    Register address byte order select
 *
 * \return  BOARD_SOK in case of success or appropriate error code.
 */
int32_t Board_i2c16BitRegRd(void   *handle,
                                 uint32_t slaveAddr,
                                 uint16_t regAddr,
                                 uint8_t *regData,
                                 uint8_t numOfBytes,
                                 uint8_t byteOrdSel,
                                 uint32_t i2cTimeout)
{
    int32_t ret = 0;
    I2C_Transaction transaction;
    uint8_t tx[2];

    I2C_Handle i2cHandle = (I2C_Handle)handle;

    /* Initializes the I2C transaction structure with default values */
    I2C_Transaction_init(&transaction);

    transaction.slaveAddress = slaveAddr;
    transaction.writeBuf     = &tx[0];
    transaction.writeCount   = 2;
    transaction.readBuf      = NULL;
    transaction.readCount    = 0;
    transaction.timeout      = i2cTimeout;

    /* 16-bit regAddr data to be sent */
    if(byteOrdSel == BOARD_I2C_REG_ADDR_MSB_FIRST)
    {
        tx[0] = (uint8_t)((regAddr & 0xFF00) >> 8);
        tx[1] = (uint8_t)(regAddr & 0x00FF);
    }
    else
    {
        tx[0] = (uint8_t)(regAddr & 0x00FF);
        tx[1] = (uint8_t)((regAddr & 0xFF00) >> 8);
    }

    ret = I2C_transfer(i2cHandle, &transaction);
    if(ret != I2C_STS_SUCCESS)
    {
        issLogPrintf("Failing while transmitting the rd reg addr with error code - %d\n", ret);
        ret = -1;
        return ret;
    }

    transaction.writeBuf     = NULL;
    transaction.writeCount   = 0;
    transaction.readBuf      = regData;
    transaction.readCount    = numOfBytes;

    ret = I2C_transfer(i2cHandle, &transaction);
    if(ret != I2C_STS_SUCCESS)
    {
        issLogPrintf("Failing while reading the register data by returning - %d\n", ret);
        ret = -1;
        return ret;
    }

    return 0;
}
#endif

int32_t initFusion2_UB97x()
{
    int32_t status = -1;
    uint8_t regAddr;
    uint8_t regVal;

    #if !defined(MCU_PLUS_SDK)
    regAddr = 0x03;
    uint32_t tca6408I2CSlaveAddr = 0x20;
    static uint8_t fusionBrdInitDone = 0;

    if(fusionBrdInitDone == 0)
    {
        regVal = 0xFF;
    	status = Board_i2c8BitRegWr(gISS_Sensor_I2cHandle, tca6408I2CSlaveAddr, regAddr, &regVal, 1U, SENSOR_I2C_TIMEOUT);

    	appLogWaitMsecs(200);
    
    	regVal = 0xFE;
        status = Board_i2c8BitRegWr(gISS_Sensor_I2cHandle, tca6408I2CSlaveAddr, regAddr, &regVal, 1U, SENSOR_I2C_TIMEOUT);
        if(0 == status)
        {
            issLogPrintf("write 0x%x to TCA6408 register 0x%x \n", regVal, regAddr);
        }else
        {
            printf("Error writing to TCA6408 register 0x%x \n", regAddr);
        }

        appLogWaitMsecs(200);

        fusionBrdInitDone = 0;
        issLogPrintf("RE-SETTING FUSION2 DE-SER !!!!!!\n");
    }
    #else
    uint32_t tca9543I2cSlaveAddr = 0x70;
    regVal = 0x01; /* Enable Channel 0*/

    /* Main I2C2 interacts with CSI0_I2C2 via Channel 0 of the TCA9543APWR,
    *  a 2-Channel I2C Bus Switch. A value of 0x01 has to be written to the
    *  TCA9543APWR to enable I2C communication through Channel 0. The address
    *  of TCA9543APWR is 0x70 on J7AEN (defined by hardware).
    */

    status = Board_i2c8BitRegWrSingle(
        gISS_Sensor_I2cHandle,
        tca9543I2cSlaveAddr,
        &regVal,
        0x20);

    if(status == 0)
    {
        issLogPrintf(
            "Write 0x%.2x to TCA9543APWR (0x%.2x) successful!!!\n",
            regVal,
            tca9543I2cSlaveAddr);
    }else
    {
        printf(
            "Error writing 0x%.2x to TCA9543APWR (0x%.2x)!!!\n",
            regVal,
            tca9543I2cSlaveAddr);
    }
    #endif

    return status;
}

int32_t deInitFusion2_UB97x()
{
    int32_t status = -1;
    uint32_t tca6408I2CSlaveAddr = 0x20;
    uint8_t regAddr = 0x3;
    uint8_t regVal = 0xFF;
    static uint8_t fusionBrdInitDone = 0;

    if(fusionBrdInitDone == 0)
    {
        status = Board_i2c8BitRegWr(gISS_Sensor_I2cHandle, tca6408I2CSlaveAddr, regAddr, &regVal, 1U, SENSOR_I2C_TIMEOUT);
        if(0 == status)
        {
            issLogPrintf("write 0x%x to TCA6408 register 0x%x \n", regVal, regAddr);
        }else
        {
            printf("Error writing to TCA6408 register 0x%x \n", regAddr);
        }

        appLogWaitMsecs(200);

        fusionBrdInitDone = 0U;
    }
    return status;
}

void getIssSensorI2cInfo(uint8_t * byteOrder, I2C_Handle * i2cHndl)
{
    *byteOrder = gISS_Sensor_ByteOrder;
    *i2cHndl = gISS_Sensor_I2cHandle;
}

// Note: this is a hack for now
#define CONFIG_I2C2 (1U)

static int32_t setupI2CInst(uint8_t i2cInst)
{
    int32_t status = -1;

    #if !defined(MCU_PLUS_SDK)
    I2C_Params i2cParams;

    /* Initializes the I2C Parameters */
    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;

    /* Configures the I2C instance with the passed parameters*/
    if(NULL == gISS_Sensor_I2cHandle)
    {
       gISS_Sensor_I2cHandle = I2C_open(i2cInst, &i2cParams);
    }
    #else
    gISS_Sensor_I2cHandle = I2C_getHandle(CONFIG_I2C2);
    #endif

    if(gISS_Sensor_I2cHandle == NULL)
    {
        printf(" I2C: ERROR: I2C Open Failed for instance %d !!!\n", i2cInst);
        status = -1;
    }
    else
    {
        status = 0;
    }

    return status;
}


int32_t IssSensor_Init()
{
    int32_t status = -1;
    uint32_t cnt;

    for(cnt=0;cnt<ISS_SENSORS_MAX_SUPPORTED_SENSOR;cnt++)
    {
        g_pSenHndl[cnt] = NULL;
    }

    status = appRemoteServiceRegister(
        IMAGE_SENSOR_REMOTE_SERVICE_NAME,
        ImageSensor_RemoteServiceHandler
    );

    if(status!=0)
    {
        printf(" REMOTE_SERVICE_SENSOR: ERROR: Unable to register remote service sensor handler\n");
           return status;
    }

    for (cnt = 0U; cnt < ISS_SENSORS_MAX_SUPPORTED_SENSOR; cnt ++)
    {
        gIssSensorTable[cnt] = NULL;
        g_detectedSensors[cnt] = 0xFF;
    }

/*
    Call init function of all supported sensors.
    This will register the sensors with the framework
*/
    status |= IssSensor_IMX390_Init();
#if defined (SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J784S4)
    status |= IssSensor_IMX390DISCOVERY_NARROW_Init();
    status |= IssSensor_IMX390DISCOVERY_MID_Init();
    status |= IssSensor_IMX390DISCOVERY_WIDE_Init();
#endif
    status |= IssSensor_AR0233_Init();
    status |= IssSensor_AR0820_Init();
    status |= IssSensor_rawtestpat_Init();
    status |= IssSensor_testpat_Init();
    status |= IssSensor_gw_ar0233_Init();

    return status;
}

int32_t IssSensor_Register(IssSensors_Handle *pSensorHandle)
{
    int32_t status = 0;
    uint32_t cnt = 0U;

    if (NULL == pSensorHandle)
    {
        status = -2;
    }
    else
    {
        /* Find a free entry in the sensor table */
        for (cnt = 0U; cnt < ISS_SENSORS_MAX_SUPPORTED_SENSOR; cnt ++)
        {
            if (NULL == gIssSensorTable[cnt])
            {
                gIssSensorTable[cnt] = pSensorHandle;
                issLogPrintf("Found sensor %s at location %d \n", pSensorHandle->createPrms->name, cnt);
                break;
            }
        }

        if (cnt == ISS_SENSORS_MAX_SUPPORTED_SENSOR)
        {
            printf(" ISS_SENSOR: Could not register sensor \n");
            status = -2;
        }
    }

    return (status);
}

IssSensors_Handle * getSensorHandleFromName(char *name)
{
    uint32_t cnt;
    IssSensors_Handle *pSensorHandle = NULL;
    /* Check For Errors */
    if (NULL == name)
    {
        return NULL;
    }
    else
    {
        for (cnt = 0U; cnt < ISS_SENSORS_MAX_SUPPORTED_SENSOR; cnt ++)
        {
            pSensorHandle = gIssSensorTable[cnt];
            if(NULL == pSensorHandle)
            {
                issLogPrintf("pSensorHandle is NULL \n");
                return NULL;
            }
            if(NULL == pSensorHandle->createPrms)
            {
                issLogPrintf("createPrms is NULL \n");
                return NULL;
            }
            if (0 == strncmp(pSensorHandle->createPrms->name, name, ISS_SENSORS_MAX_NAME))
            {
                break;
            }
        }
    }
    return pSensorHandle;
}

int32_t IssSensor_GetSensorInfo(char *name, IssSensor_CreateParams *sensor_prms)
{
    int32_t status = -1;
    uint32_t cnt;
    IssSensors_Handle *pSensorHandle = NULL;

    /* Check For Errors */
    if ((NULL == sensor_prms) || (NULL == name))
    {
        status = -2;
    }
    else
    {
        for (cnt = 0U; cnt < ISS_SENSORS_MAX_SUPPORTED_SENSOR; cnt ++)
        {
            pSensorHandle = gIssSensorTable[cnt];
            if(NULL == pSensorHandle)
            {
                issLogPrintf("pSensorHandle is NULL \n");
                return -1;
            }
            if(NULL == pSensorHandle->createPrms)
            {
                issLogPrintf("createPrms is NULL \n");
                return -1;
            }
            if (0 == strncmp(pSensorHandle->createPrms->name, name, ISS_SENSORS_MAX_NAME))
            {
                memcpy(sensor_prms, pSensorHandle->createPrms, sizeof(IssSensor_CreateParams));
                status = 0;
                break;
            }
        }
    }
    return (status);
}

IssSensors_Handle * IssSensor_GetSensorHandle(char * name)
{
    uint32_t cnt;
    IssSensors_Handle *pSensorHandle = NULL;
    IssSensor_CreateParams *pCreatePrms = NULL;

    /* Check For Errors */
    if (NULL != name)
    {
        for (cnt = 0U; cnt < ISS_SENSORS_MAX_SUPPORTED_SENSOR; cnt ++)
        {
            pSensorHandle = gIssSensorTable[cnt];
            if(NULL == pSensorHandle)
            {
                return NULL;
            }

            pCreatePrms = pSensorHandle->createPrms;

            if (vx_true_e == pSensorHandle->isUsed)
            {
                if(0 == strncmp(pCreatePrms->name, name, ISS_SENSORS_MAX_NAME))
                {
                    break;
                }
            }
        }
    }
    return (void*)(pSensorHandle);
}

int32_t IssSensor_Delete(void* handle)
{
    int32_t status = -1;
    IssSensors_Handle *pSensorHandle;

    /* Check if the handle is valid or not */
    status = checkForHandle(handle);

    if (0 == status)
    {
        pSensorHandle = (IssSensors_Handle *)handle;

        memset(pSensorHandle->createPrms, 0, sizeof(IssSensor_CreateParams));
    }

    return (status);
}

int32_t IssSensor_Config(void* handle, uint32_t chId, uint32_t feat)
{
    int32_t status = -1;
    IssSensors_Handle *pSensorHandle;

    /* Check if the handle is valid or not */
    status = checkForHandle(handle);

    if (0 == status)
    {
        pSensorHandle = (IssSensors_Handle *)handle;
        if(chId < pSensorHandle->createPrms->num_channels)
        {
            if (NULL != pSensorHandle->sensorFxns->config)
            {
                status = pSensorHandle->sensorFxns->config(chId, handle, feat);
            }
        }
        else
        {
            printf("IssSensor_Config Error : Incorrect channel ID %d \n", chId);
            status = -1;
        }
    }

    return (status);
}


static int32_t UB953_WriteReg(uint8_t   i2cInstId,
                                 uint8_t   slaveI2cAddr,
                                 uint16_t  regAddr,
                                 uint8_t   regVal)
{
    int32_t status = -1;
    status = Board_i2c8BitRegWr(gISS_Sensor_I2cHandle, slaveI2cAddr, regAddr, &regVal, 1U, SENSOR_I2C_TIMEOUT);
    if(0 != status)
    {
        printf("Error writing 0x%x to UB953 register 0x%x \n", regVal, regAddr);
    }
    return status;
}

static int32_t UB960_ReadReg(uint8_t   i2cInstId,
                                 uint8_t   slaveI2cAddr,
                                 uint16_t  regAddr,
                                 uint8_t   *regVal)
{
    int32_t status = -1;
    status = Board_i2c8BitRegRd(gISS_Sensor_I2cHandle, slaveI2cAddr, regAddr, regVal, 1U, SENSOR_I2C_TIMEOUT);
    if(0 != status)
    {
        printf("Error reading from UB960 register 0x%x \n", regAddr);
    }
    return status;
}

static int32_t UB960_WriteReg(uint8_t   i2cInstId,
                                 uint8_t   slaveI2cAddr,
                                 uint16_t  regAddr,
                                 uint8_t   regVal)
{
    int32_t status = -1;
    status = Board_i2c8BitRegWr(gISS_Sensor_I2cHandle, slaveI2cAddr, regAddr, &regVal, 1U, SENSOR_I2C_TIMEOUT);
    if(0 != status)
    {
        printf("Error writing 0x%x to UB960 register 0x%x \n", regVal, regAddr);
    }
    return status;
}

/*Checks which cameras are connected to the specified Deserializer*/
/*Returns a 4-bit mask - 0 = Ser Not Detected, 1 = Ser Detected*/
static int32_t IssSensor_detect_serializer(int8_t ub960InstanceId, uint8_t * cameras_detected)
{
    uint8_t mask = 0x0;
    uint8_t pageSelectOrig = 0x0;
    uint8_t regVal = 0x0;
    uint8_t found = 0x0;
    int32_t status = -1;
    uint8_t  ub960I2cInstId = 0;
    uint8_t  ub960I2cAddr;

    #if defined (MCU_PLUS_SDK)
    Board_fpdUb960GetI2CAddr(&ub960I2cAddr, ub960InstanceId);
    #else
    if(gFusion2Det)
    {
        uint8_t domain;
        Board_fpdUb9702GetI2CAddr(ub960InstanceId,
                                &domain,
                                &ub960I2cInstId,
                                &ub960I2cAddr);
    }
    else
    {
        Board_fpdU960GetI2CAddr(&ub960I2cInstId, &ub960I2cAddr, ub960InstanceId);
    }
    #endif

    status = UB960_ReadReg(ub960I2cInstId, ub960I2cAddr, 0x4C, &pageSelectOrig);
    if(status != 0)
    {
        return status;
    }

    status = UB960_WriteReg(ub960I2cInstId, ub960I2cAddr, 0x4C, 0x01);
    if(status != 0)
    {
        return status;
    }

    status = UB960_ReadReg(ub960I2cInstId, ub960I2cAddr, 0x4D, &regVal);
    if(status != 0)
    {
        return status;
    }

    found = regVal & 0x3;//LOCK_STS and PORT_PASS
    if(found == 0x3)
    {
        mask |=  0x1;/*Set bit#0*/
    }

    status = UB960_WriteReg(ub960I2cInstId, ub960I2cAddr, 0x4C, 0x12);
    if(status != 0)
    {
        return status;
    }

    status = UB960_ReadReg(ub960I2cInstId, ub960I2cAddr, 0x4D, &regVal);
    if(status != 0)
    {
        return status;
    }

    found = regVal & 0x3;//LOCK_STS and PORT_PASS
    if(found == 0x3)
    {
        mask |=  0x2;/*Set bit#1*/
    }

    status = UB960_WriteReg(ub960I2cInstId, ub960I2cAddr, 0x4C, 0x24);
    if(status != 0)
    {
        return status;
    }

    status = UB960_ReadReg(ub960I2cInstId, ub960I2cAddr, 0x4D, &regVal);
    if(status != 0)
    {
        return status;
    }

    found = regVal & 0x3;//LOCK_STS and PORT_PASS
    if(found == 0x3)
    {
        mask |=  0x4;/*Set bit#2*/
    }

    status = UB960_WriteReg(ub960I2cInstId, ub960I2cAddr, 0x4C, 0x38);
    if(status != 0)
    {
        return status;
    }
    
    status = UB960_ReadReg(ub960I2cInstId, ub960I2cAddr, 0x4D, &regVal);
    if(status != 0)
    {
        return status;
    }
    
    found = regVal & 0x3;//LOCK_STS and PORT_PASS
    if(found == 0x3)
    {
        mask |=  0x8;/*Set bit#3*/
    }

    status = UB960_WriteReg(ub960I2cInstId, ub960I2cAddr, 0x4C, pageSelectOrig);

    if(ub960InstanceId == BOARD_CSI_INST_0)
    {
        *cameras_detected |= mask;
    }else if(ub960InstanceId == BOARD_CSI_INST_1)
    {
        *cameras_detected |= (mask << 4U);
    }

    return status;
}

/*Probes all the sensors in gIssSensorTable*/
/*If a sensor is detected, the index of the sensor in gIssSensorTable is copied to sensor_id_list*/
static int32_t IssSensor_detect_sensor(uint8_t chId)
{
    int32_t probeStatus = -1;
    int32_t i = 0;
    IssSensors_Handle * pSenHndl;
    int32_t sensor_id_found = -1;

    while ((sensor_id_found < 0) && (i<ISS_SENSORS_MAX_SUPPORTED_SENSOR))
    {
        pSenHndl = gIssSensorTable[i];
        if(NULL !=pSenHndl)
        {
            probeStatus = pSenHndl->sensorFxns->probe(chId, pSenHndl);

            if(probeStatus == 0 )
            {
                /*Sensor found at index i*/
                issLogPrintf("Found sensor %s at port # %d\n", pSenHndl->createPrms->name, chId);
                sensor_id_found = i;
            }
        }
        i++;
    }

    return sensor_id_found;
}

/*Checks which cameras are connected to the specified Deserializer*/
/*Returns a 4-bit mask - 0 = Camera Not Detected, 1 = Camera Detected*/
static int32_t IssSensor_DeserializerInit(/*uint8_t *mask*/)
{
    int32_t status = -1;
    uint8_t  ub960I2cInstId = 0;
    uint8_t  ub960I2cAddr;
    static uint8_t desInitDone = 0;
    tivx_mutex desCfgLock;
    vx_bool bDet = vx_false_e;

    tivxMutexCreate(&desCfgLock);

    tivxMutexLock(desCfgLock);


    #if defined (MCU_PLUS_SDK)
    Board_fpdUb960GetI2CAddr(&ub960I2cAddr, BOARD_CSI_INST_0);
    #else
    /* Check for the Fusion2 board */
    bDet = Board_detectBoard(BOARD_ID_FUSION2);
    if(bDet == vx_true_e)
    {
        issLogPrintf("Fusion2 Board Detected, using Fusion2 configuration\n");
        gFusion2Det = vx_true_e;
    }
    else
    {
        issLogPrintf("Fusion1 Board configuration\n");
        gFusion2Det = vx_false_e;
    }
    if(gFusion2Det)
    {
        uint8_t domain;
        Board_fpdUb9702GetI2CAddr(BOARD_FPD_9702_CSI2_DES_HUB1,
                                &domain,
                                &ub960I2cInstId,
                                &ub960I2cAddr);
    }
    else
    {
        Board_fpdU960GetI2CAddr(&ub960I2cInstId, &ub960I2cAddr, BOARD_CSI_INST_0);
    }
    #endif

    status = setupI2CInst(ub960I2cInstId);
    if(status!=0)
    {
        printf(" I2C ERROR \n");
        tivxMutexUnlock(desCfgLock);
        tivxMutexDelete(&desCfgLock);
        return status;
    }

    if(desInitDone == 0)
    {
        status = initFusion2_UB97x();
        if(status != 0)
        {
            printf("rawtestpat_PowerOn Error : initFusion2_UB97x returned 0x%x \n", status);
        }

        /*Des_1 has a different I2C address on Fusion2 because of which second
        UB960 CFG fails. DES_0 is still usable, so we should not return an error.
        */
        desInitDone = 0;

    }

    /* UB960 Global configuration is done here which should be shared by all sensors*/
    /* Sensor driver is not supposed to overwrite global configuration*/
    /* Sensor driver can only modify the page corresponding to the channel ID selected*/
    if(gFusion2Det)
    {
        status = ub960_cfgScript(ub9702DesCfg_Common, 0U);
    }
    else
    {
        status = ub960_cfgScript(ub960DesCfg_Common, 0U);
    }

    if(status!=0)
    {
        printf("Error : ub960_cfgScript returned %d while configuring DES 0 \n", status);
    }

    if(gFusion2Det)
    {
        status = ub960_cfgScript(ub9702DesCfg_Common, 1U);
    }
    else
    {
        status = ub960_cfgScript(ub960DesCfg_Common, 1U);
    }
    
    if(status!=0)
    {
        printf("Error :  returned %d while configuring DES 1 \n", status);
    }

    if(gFusion2Det)
    {
        status = ub960_cfgScript(ub9702DesCfg_Common, 2U);
        if(status!=0)
        {
            printf("Error :  returned %d while configuring DES 2 \n", status);
        }
    }

    tivxMutexUnlock(desCfgLock);

    tivxMutexDelete(&desCfgLock);

    return 0;
}

int32_t IssSensor_PowerOn(void* handle, uint32_t chMask)
{
    int32_t status = -1;
    IssSensors_Handle *pSensorHandle;

    num_sensors_open++;

    /* Check if the handle is valid or not */
    status = checkForHandle(handle);

    if (0 == status)
    {
        uint32_t chId = 0;
        pSensorHandle = (IssSensors_Handle *)handle;
        pSensorHandle->sensorIntfPrms->numCamerasStreaming = 0;
        while ((chMask > 0) && (chId < ISS_SENSORS_MAX_SUPPORTED_SENSOR))
        {
            if(chMask & 0x1)
            {
                if (NULL != pSensorHandle->sensorFxns->powerOn)
                {
                    status = pSensorHandle->sensorFxns->powerOn(chId, handle);
                }
            }
            chMask = chMask >> 1;
            chId++;
        }
    }

    return (status);

}

int32_t IssSensor_PowerOff(void* handle, uint32_t chId)
{
    int32_t status = -1;
    IssSensors_Handle *pSensorHandle;

    /* Check if the handle is valid or not */
    status = checkForHandle(handle);

    if (0 == status)
    {
        pSensorHandle = (IssSensors_Handle *)handle;
        
        if(pSensorHandle->sensorIntfPrms->numCamerasStreaming != 0)
        {
            issLogPrintf("IssSensor_PowerOff : Warning %d cameras are still streaming \n ", pSensorHandle->sensorIntfPrms->numCamerasStreaming);        
        }

        if (NULL != pSensorHandle->sensorFxns->powerOff)
        {
            status = pSensorHandle->sensorFxns->powerOff(chId, handle);
        }
    }

    num_sensors_open--;
    if(0 == num_sensors_open)
    {
        uint8_t  ub960I2cInstId = 0;
        uint8_t  ub960I2cAddr;
        #if defined (MCU_PLUS_SDK)
        Board_fpdUb960GetI2CAddr(&ub960I2cAddr, BOARD_CSI_INST_0);
        #else
        if(gFusion2Det)
        {
            uint8_t domain;
            Board_fpdUb9702GetI2CAddr(BOARD_FPD_9702_CSI2_DES_HUB1,
                                        &domain,
                                        &ub960I2cInstId,
                                        &ub960I2cAddr);
        }
        else
        {
            Board_fpdU960GetI2CAddr(&ub960I2cInstId, &ub960I2cAddr, BOARD_CSI_INST_0);
        }
        I2C_close(gISS_Sensor_I2cHandle);
        #endif
        gISS_Sensor_I2cHandle = NULL;
    }

    return (status);

}

int32_t IssSensor_Start(void* handle, uint32_t chId)
{
    int32_t status = -1;
    uint32_t numCamerasEnabled = 0;
    IssSensors_Handle *pSensorHandle = (IssSensors_Handle *)NULL;

    /* Check if the handle is valid or not */
    status = checkForHandle(handle);

    if (0 == status)
    {
        pSensorHandle = (IssSensors_Handle *)handle;
        if(2U == pSensorHandle->sensorIntfPrms->sensorBroadcast)
        {
            if(0x04 > chId)
            {
                /*Broadcast mode - UB960_0*/
                uint8_t i;
                for(i=0;i<(chId+1);i++)
                {
                    g_pSenHndl[i] = pSensorHandle;
                }
                numCamerasEnabled = chId+1;
            }
            else if(0x08 > chId)
            {
                /*Broadcast mode - UB960_1*/
                uint8_t i;
                for(i=4;i<(chId+1);i++)
                {
                    g_pSenHndl[i] = pSensorHandle;
                }
                numCamerasEnabled = chId-3;
            }
            else if(0x0C /* 12 */ > chId)
            {
                /*Broadcast mode - UB9xx_2*/
                uint8_t i;
                for(i=8;i<ISS_SENSORS_MAX_SUPPORTED_SENSOR;i++)
                {
                    g_pSenHndl[i] = pSensorHandle;
                }
                numCamerasEnabled = chId-7;
            }
        }
        else
        {
            numCamerasEnabled = 1;
        }

        if(NULL != g_pSenHndl[chId])
        {
            issLogPrintf("Warning : channel ID %d already has a registered sensor handle \n", chId);
        }
        g_pSenHndl[chId] = pSensorHandle;

        if(NULL != pSensorHandle->sensorFxns->streamOn)
        {
            status = pSensorHandle->sensorFxns->streamOn(chId, handle);
        }

        if(status == 0)
        {
            pSensorHandle->sensorIntfPrms->numCamerasStreaming += numCamerasEnabled;
        }
    }

    return (status);
}

int32_t IssSensor_Stop(void* handle, uint32_t chId)
{
    int32_t status = -1;
    IssSensors_Handle *pSensorHandle;

    /* Check if the handle is valid or not */
    status = checkForHandle(handle);

    if (0 == status)
    {
        pSensorHandle = (IssSensors_Handle *)handle;

        if (1) //(chId < pSensorHandle->createPrms->num_channels)
        {
            if(NULL != pSensorHandle->sensorFxns->streamOff)
            {
                status |= pSensorHandle->sensorFxns->streamOff(chId, handle);
            }
            pSensorHandle->sensorIntfPrms->numCamerasStreaming -= 1;
        }
        else
        {
            printf("IssSensor_Stop Error: Incorrect channel ID %d \n", chId);
            status = -1;
        }
    }

    g_pSenHndl[chId] = NULL;

    return (status);
}

int32_t IssSensor_SetAeParams(void *handle, uint32_t chId, IssSensor_ExposureParams *pExpPrms)
{
    int32_t status = -1;
    IssSensors_Handle * pSensorHandle = NULL;

    /* Check if the handle is valid or not */
    status = checkForHandle(handle);

    if (0 == status)
    {
        pSensorHandle = (IssSensors_Handle *)handle;
        if(pSensorHandle != NULL)
        {
            if(NULL != pSensorHandle->sensorFxns->setAeParams)
            {
                status |= pSensorHandle->sensorFxns->setAeParams(handle, chId, pExpPrms);
            }
        }
        else
        {
            printf("IssSensor_SetAeParams Error: sensor handle is NULL for channel ID %d \n", chId);
            status = -1;
        }
    }

    return (status);
}

int32_t IssSensor_SetAwbParams(void *handle, uint32_t chId, IssSensor_WhiteBalanceParams *pWbPrms)
{
    int32_t status = -1;
    IssSensors_Handle * pSensorHandle = NULL;

    /* Check if the handle is valid or not */
    status = checkForHandle(handle);

    if (0 == status)
    {
        pSensorHandle = (IssSensors_Handle *)handle;
        //if(chId < pSensorHandle->createPrms->num_channels)
        if(pSensorHandle != NULL)
        {
            if(NULL != pSensorHandle->sensorFxns->setAwbParams)
            {
                status |= pSensorHandle->sensorFxns->setAwbParams(handle, chId, pWbPrms);
            }
            else
            {
                printf("IssSensor_SetAwbParams Error : Sensor setAwbParams callback is NULL \n");
                memset(pWbPrms, 0x0, sizeof(IssSensor_WhiteBalanceParams));
            }
        }
        else
        {
            printf("IssSensor_SetAwbParams Error : sensor handle is NULL for  channel ID %d \n", chId);
            status = -1;
        }
    }

    return (status);
}

int32_t IssSensor_Control(void* handle, uint32_t cmd, void* cmdArgs, void* cmdRetArgs)
{
    int32_t status = VX_FAILURE;
    uint32_t chId = 0xFF;
    IMAGE_SENSOR_CTRLCMD ctrlCmd;
    uint8_t * cmd_ptr = (uint8_t *)cmdArgs;

    if(NULL == cmd_ptr)
    {
        printf("Error : cmd_ptr is NULL \n");
        return VX_FAILURE;
    }

    cmd_ptr += ISS_SENSORS_MAX_NAME;

    memcpy(&chId, cmd_ptr, sizeof(uint32_t));
    cmd_ptr += sizeof(uint32_t);

    memcpy(&ctrlCmd, cmd_ptr, sizeof(IMAGE_SENSOR_CTRLCMD));
    cmd_ptr += sizeof(IMAGE_SENSOR_CTRLCMD);

    switch(ctrlCmd)
    {
        case IMAGE_SENSOR_CTRLCMD_GETEXPPRG:
            {
                IssSensors_Handle * pSenHndl = g_pSenHndl[chId];
                if(NULL != pSenHndl)
                {
                    status = pSenHndl->sensorFxns->getExpPrg(chId, (void*)pSenHndl, (IssAeDynamicParams *)cmd_ptr);
                }
                else
                {
                    printf("Error : sensor handle is NULL for channel %d \n", chId);
                    status = VX_FAILURE;
                }
            }
            break;
        case IMAGE_SENSOR_CTRLCMD_GETWBCFG:
            {
                IssSensors_Handle * pSenHndl = g_pSenHndl[chId];
                if(NULL != pSenHndl)
                {
                    if(NULL != pSenHndl->sensorFxns)
                    {
                        if(NULL != pSenHndl->sensorFxns->getWbCfg)
                        {
                            status = pSenHndl->sensorFxns->getWbCfg(chId, (void*)pSenHndl, (IssAwbDynamicParams *)cmd_ptr);
                        }else
                        {
                            /*Sensor driver does not support WB update API*/
                            memset(cmd_ptr, 0, sizeof(IssAwbDynamicParams));
                        }
                    }
                }
                else
                {
                    memset(cmd_ptr, -1, sizeof(IssAwbDynamicParams));
                    status = VX_FAILURE;
                }
            }
            break;  
        case IMAGE_SENSOR_CTRLCMD_SETEXPGAIN:
            {
                IssSensor_ExposureParams aePrms;
                IssSensors_Handle * pSenHndl = g_pSenHndl[chId];
                if(NULL != pSenHndl)
                {
                    memcpy(&aePrms, cmd_ptr, sizeof(IssSensor_ExposureParams));
                    status = IssSensor_SetAeParams(pSenHndl, chId, &aePrms);
                }
                else
                {
                    printf("Error : sensor handle is NULL for channel %d \n", chId);
                    status = VX_FAILURE;
                }                
            }
            break;
        case IMAGE_SENSOR_CTRLCMD_GETEXPGAIN:
            /*Reserved for future use*/
            status = 0;
            break;
        case IMAGE_SENSOR_CTRLCMD_SETWBGAIN:
            {
                IssSensor_WhiteBalanceParams awbPrms;
                IssSensors_Handle * pSenHndl = g_pSenHndl[chId];
                if(NULL != pSenHndl)
                {
                    memcpy(&awbPrms, cmd_ptr, sizeof(IssSensor_WhiteBalanceParams));
                    status = IssSensor_SetAwbParams(pSenHndl, chId, &awbPrms);
                }
                else
                {
                    printf("Error : sensor handle is NULL for channel %d \n", chId);
                    status = VX_FAILURE;
                }         
            }
            break;
        case IMAGE_SENSOR_CTRLCMD_GETWBGAIN:
            /*Reserved for future use*/
            status = 0;
            break;
        case IMAGE_SENSOR_CTRLCMD_DEBUG:
            /*Reserved for future use*/
            status = 0;
            issLogPrintf("IMAGE_SENSOR_CTRLCMD_DEBUG \n");
            {
                uint32_t * ptr32 = (uint32_t * )cmd_ptr;
                IssSensors_Handle * pSenHndl = g_pSenHndl[chId];
                uint32_t rw_flag = *ptr32++; /*ReadWrite Flag = 1 for Write*/
                uint32_t devType = *ptr32++; /*Device Type*/
                uint32_t regAddr32 = *ptr32++; /*Register Address*/
                uint32_t regVal32 = *ptr32; /*Register Value*/
                uint32_t slaveI2cAddr = 0x0;
                uint8_t regAddr = 0xBC;
                uint8_t regVal = 0xDE;

                if(0U == devType)
                {
                    slaveI2cAddr = 0x3D ;//pSenHndl->createPrms->i2cAddrDes[chId];
                }
                else if(1U == devType)
                {
                    slaveI2cAddr = pSenHndl->createPrms->i2cAddrSer[chId];
                }
                else if(2U == devType)
                {
                    slaveI2cAddr = pSenHndl->createPrms->i2cAddrSensor[chId];
                }
                else
                {
                    printf("IMAGE_SENSOR_CTRLCMD_DEBUG Error : Unsupported devType %d \n", devType);
                    return -1;
                }

                if(2U == devType)
                {
                    I2cParams sensorI2cParams;
                    sensorI2cParams.nDelay = 1U;
                    sensorI2cParams.nRegAddr = (uint16_t)(regAddr32);
                    sensorI2cParams.nRegValue = (uint16_t)(regVal32);
                    status = pSenHndl->sensorFxns->readWriteReg(chId, pSenHndl, rw_flag, &sensorI2cParams);
                    if(0 != status)
                    {
                        printf("IMAGE_SENSOR_CTRLCMD_DEBUG : Error reading from register 0x%x on the image sensor %s \n", regAddr, pSenHndl->createPrms->name);
                    }
                    else
                    {
                        ptr32 = (uint32_t * )cmdArgs;
                        *ptr32 = (uint32_t)sensorI2cParams.nRegValue;
                    }
                }
                else
                {
                    if(0U == rw_flag)
                    {
                        regAddr = (uint8_t)(regAddr32);
                        status = Board_i2c8BitRegRd(gISS_Sensor_I2cHandle, slaveI2cAddr, regAddr, &regVal, 1U, SENSOR_I2C_TIMEOUT);
                        if(0 != status)
                        {
                            printf("IMAGE_SENSOR_CTRLCMD_DEBUG : Error reading from register 0x%x on slave device 0x%x \n", regAddr, slaveI2cAddr);
                        }
                        issLogPrintf("IMAGE_SENSOR_CTRLCMD_DEBUG : Read 0x%x from register 0x%x on slave device 0x%x \n", regVal, regAddr, slaveI2cAddr);
                    }
                    else if (1U == rw_flag)
                    {
                        regAddr = (uint8_t)(regAddr32);
                        regVal =  (uint8_t)(regVal32);
                        status = Board_i2c8BitRegWr(gISS_Sensor_I2cHandle, slaveI2cAddr, regAddr, &regVal, 1U, SENSOR_I2C_TIMEOUT);
                        if(0 != status)
                        {
                            printf("IMAGE_SENSOR_CTRLCMD_DEBUG : Error writing 0x%x to register 0x%x on slave device 0x%x \n", regVal, regAddr, slaveI2cAddr);
                        }
                    }
                    else
                    {
                        status = -1;
                        printf("IMAGE_SENSOR_CTRLCMD_DEBUG Error : Invalid rwflag = %d \n", rw_flag);
                    }

                    if(0 == status)
                    {
                        ptr32 = (uint32_t * )cmdArgs;
                        *ptr32 = (uint32_t)regVal;
                    }
                }
            }
            break;
        case IMAGE_SENSOR_CTRLCMD_READ_SENSOR_REG:
            {
                uint32_t * ptr32 = (uint32_t * )cmd_ptr;
                IssSensors_Handle * pSenHndl = g_pSenHndl[chId];
                I2cParams reg_read;
                reg_read.nDelay = 0;
                reg_read.nRegAddr = (uint16_t)(*ptr32);
                ptr32++;
                reg_read.nRegValue = 0xFF;
                status = pSenHndl->sensorFxns->readWriteReg(chId, pSenHndl, 0, &reg_read);
                *ptr32 = reg_read.nRegValue;
            }
            break;
        case IMAGE_SENSOR_CTRLCMD_WRITE_SENSOR_REG:
            {
                uint32_t * ptr32 = (uint32_t * )cmd_ptr;
                IssSensors_Handle * pSenHndl = g_pSenHndl[chId];
                I2cParams reg_write;
                reg_write.nDelay = 0;
                reg_write.nRegAddr = (uint16_t)(*ptr32);
                ptr32++;
                reg_write.nRegValue = (uint16_t)(*ptr32);
                status = pSenHndl->sensorFxns->readWriteReg(chId, pSenHndl, 1, &reg_write);
                *ptr32 = reg_write.nRegValue;
            }
            break;
        default:
            status = -1;
            printf("IMAGE_SENSOR_CTRLCMD_DEBUG Error : Unknown control command %d \n", ctrlCmd);
            break;
    }

    return (status);
}

/*******************************************************************************
 *  Local Functions Definition
 *******************************************************************************
 */

static int32_t checkForHandle(void* handle)
{
    int32_t found = -1;
    uint32_t cnt;

    /* Find a free entry in the sensor table */
    for (cnt = 0U; cnt < ISS_SENSORS_MAX_SUPPORTED_SENSOR; cnt ++)
    {
        if (handle == (void*)gIssSensorTable[cnt])
        {
            found = 0;
            break;
        }
    }

    return (found);
}

int32_t ub960_cfgScript(I2cParams *script, int8_t ub960InstanceId)
{
    uint16_t regAddr;
    uint8_t  regValue;
    uint16_t delayMilliSec;
    uint32_t cnt;
    int32_t status = 0;
    uint8_t  ub960I2cInstId = 0;
    uint8_t  ub960I2cAddr;

    /*Assumption for Fusion1 board - maximum two UB960s*/
    if((ub960InstanceId > BOARD_CSI_INST_2) || (ub960InstanceId < BOARD_CSI_INST_0))
    {
        printf("Error : Invalid ub960InstanceId %d \n", ub960InstanceId);
        return -1;
    }

    #if defined (MCU_PLUS_SDK)
    Board_fpdUb960GetI2CAddr(&ub960I2cAddr, ub960InstanceId);
    #else
    if(gFusion2Det)
    {
        uint8_t domain;
        Board_fpdUb9702GetI2CAddr(ub960InstanceId,
                                &domain,
                                &ub960I2cInstId,
                                &ub960I2cAddr);
    }
    else
    {
        Board_fpdU960GetI2CAddr(&ub960I2cInstId, &ub960I2cAddr, ub960InstanceId);
    }
    #endif

    issLogPrintf("UB960 config start \n");
    if(NULL != script)
    {
        cnt = 0;
        regAddr  = script[0].nRegAddr;

        while(regAddr != 0xFFFF)
        {
            /* Convert Registers address and value into 8bit array */
            regAddr  = script[cnt].nRegAddr;
            regValue = script[cnt].nRegValue;
            delayMilliSec = script[cnt].nDelay;
            status |= UB960_WriteReg(ub960I2cInstId, ub960I2cAddr, regAddr, regValue);

            if (0 != status)
            {
                printf(" UB960 Error: Reg Write Failed for regAddr %x, cnt = %d\n", regAddr, cnt);
                break;
            }

            if(delayMilliSec > 0)
            {
                appLogWaitMsecs(delayMilliSec);
            }

            cnt++;
        }
    }
    issLogPrintf("End of UB960 config \n");
    return (status);
}

int32_t ub953_cfgScript(uint8_t  i2cInstId, uint8_t  slaveAddr, I2cParams *script)
{
    uint16_t regAddr;
    uint8_t  regValue;
    uint16_t delayMilliSec;
    uint32_t cnt;
    int32_t status = 0;

    issLogPrintf("ub953 config start : slaveAddr = 0x%x \n", slaveAddr);
    if(NULL != script)
    {
        cnt = 0;
        regAddr  = script[0].nRegAddr;

        while(regAddr != 0xFFFF)
        {
            regAddr  = script[cnt].nRegAddr;
            regValue = script[cnt].nRegValue;
            delayMilliSec = script[cnt].nDelay;
            /* Convert Registers address and value into 8bit array */
            status |= UB953_WriteReg(i2cInstId, slaveAddr, regAddr, regValue);

            if (0 != status)
            {
                printf(" UB953 Error: Reg Write Failed for regAddr %x\n", regAddr);
                break;
            }
            cnt++;
            if(delayMilliSec > 0)
            {
                appLogWaitMsecs(delayMilliSec);
            }
        }
    }
    issLogPrintf(" End of UB953 config \n");
    return (status);
}

int32_t enableUB960Broadcast(int8_t ub960InstanceId)
{
    int32_t status = 0;
    I2cParams enableUB960BroadcastScript[5] =
    {
        {0x4C, 0x0F, 0x10},
        {0x65, (SER_0_I2C_ALIAS<< 1U), 0x10},
        {0x66, (SENSOR_0_I2C_ALIAS << 1U), 0x10},
        {0x72, 0xE4,0x10},
        {0xFFFF,0x00, 0x00}
    };

    if(0U == ub960InstanceId)
    {
        enableUB960BroadcastScript[1].nRegValue = (SER_0_I2C_ALIAS<<1U);
        enableUB960BroadcastScript[2].nRegValue = (SENSOR_0_I2C_ALIAS<<1U);
    } else if(1U == ub960InstanceId)
    {
        enableUB960BroadcastScript[1].nRegValue = (SER_4_I2C_ALIAS<<1U);
        enableUB960BroadcastScript[2].nRegValue = (SENSOR_4_I2C_ALIAS<<1U);
    } else if(2U == ub960InstanceId)
    {
        enableUB960BroadcastScript[1].nRegValue = (SER_8_I2C_ALIAS<<1U);
        enableUB960BroadcastScript[2].nRegValue = (SENSOR_8_I2C_ALIAS<<1U);
    }
    else
    {
        printf("enableUB960Broadcast: Error: Invalid instance ID \n");
        return -1;
    }

    status = ub960_cfgScript(enableUB960BroadcastScript, ub960InstanceId);

    return status;
}

int32_t disableUB960Broadcast(int8_t ub960InstanceId)
{
    int32_t status = 0;
    I2cParams disableUB960BroadcastScript[17] =
    {
        {0x4C, 0x01, 0x10},
        {0x65, (SER_0_I2C_ALIAS<< 1U), 0x10},
        {0x66, (SENSOR_0_I2C_ALIAS << 1U), 0x10},
        {0x72, 0x00,0x10},

        {0x4C, 0x12, 0x10},
        {0x65, (SER_1_I2C_ALIAS<< 1U), 0x10},
        {0x66, (SENSOR_1_I2C_ALIAS << 1U), 0x10},
        {0x72, 0x55,0x10},

        {0x4C, 0x24, 0x10},
        {0x65, (SER_2_I2C_ALIAS<< 1U), 0x10},
        {0x66, (SENSOR_2_I2C_ALIAS << 1U), 0x10},
        {0x72, 0xAA,0x10},

        {0x4C, 0x38, 0x10},
        {0x65, (SER_3_I2C_ALIAS<< 1U), 0x10},
        {0x66, (SENSOR_3_I2C_ALIAS << 1U), 0x10},
        {0x72, 0xFF,0x10},

        {0xFFFF,0x00, 0x00}
    };

    if(0U == ub960InstanceId)
    {
        disableUB960BroadcastScript[1].nRegValue = (SER_0_I2C_ALIAS<<1U);
        disableUB960BroadcastScript[2].nRegValue = (SENSOR_0_I2C_ALIAS<<1U);

        disableUB960BroadcastScript[5].nRegValue = (SER_1_I2C_ALIAS<<1U);
        disableUB960BroadcastScript[6].nRegValue = (SENSOR_1_I2C_ALIAS<<1U);

        disableUB960BroadcastScript[9].nRegValue = (SER_2_I2C_ALIAS<<1U);
        disableUB960BroadcastScript[10].nRegValue = (SENSOR_2_I2C_ALIAS<<1U);

        disableUB960BroadcastScript[13].nRegValue = (SER_3_I2C_ALIAS<<1U);
        disableUB960BroadcastScript[14].nRegValue = (SENSOR_3_I2C_ALIAS<<1U);
    } else if(1U == ub960InstanceId)
    {
        disableUB960BroadcastScript[1].nRegValue = (SER_4_I2C_ALIAS<<1U);
        disableUB960BroadcastScript[2].nRegValue = (SENSOR_4_I2C_ALIAS<<1U);

        disableUB960BroadcastScript[5].nRegValue = (SER_5_I2C_ALIAS<<1U);
        disableUB960BroadcastScript[6].nRegValue = (SENSOR_5_I2C_ALIAS<<1U);

        disableUB960BroadcastScript[9].nRegValue = (SER_6_I2C_ALIAS<<1U);
        disableUB960BroadcastScript[10].nRegValue = (SENSOR_6_I2C_ALIAS<<1U);

        disableUB960BroadcastScript[13].nRegValue = (SER_7_I2C_ALIAS<<1U);
        disableUB960BroadcastScript[14].nRegValue = (SENSOR_7_I2C_ALIAS<<1U);
    } else if(2U == ub960InstanceId)
    {
        disableUB960BroadcastScript[1].nRegValue = (SER_8_I2C_ALIAS<<1U);
        disableUB960BroadcastScript[2].nRegValue = (SENSOR_8_I2C_ALIAS<<1U);

        disableUB960BroadcastScript[5].nRegValue = (SER_9_I2C_ALIAS<<1U);
        disableUB960BroadcastScript[6].nRegValue = (SENSOR_9_I2C_ALIAS<<1U);

        disableUB960BroadcastScript[9].nRegValue = (SER_10_I2C_ALIAS<<1U);
        disableUB960BroadcastScript[10].nRegValue = (SENSOR_10_I2C_ALIAS<<1U);

        disableUB960BroadcastScript[13].nRegValue = (SER_11_I2C_ALIAS<<1U);
        disableUB960BroadcastScript[14].nRegValue = (SENSOR_11_I2C_ALIAS<<1U);
    }
    else
    {
        printf("enableUB960Broadcast: Error: Invalid instance ID \n");
        return -1;
    }
    status = ub960_cfgScript(disableUB960BroadcastScript, ub960InstanceId);

    return status;
}

int8_t getUB960InstIdFromChId(uint32_t chId)
{
    int8_t ub960InstanceId = -1;
    /*Assumptions : 
    max 2 UB960 instances
    upto 4 cameras per instance of UB960
    */
    if(chId < 4U)
    {
        ub960InstanceId = BOARD_CSI_INST_0;
    }else if(chId < 8U)
    {
        ub960InstanceId = BOARD_CSI_INST_1;
    }else if(chId < 12U)
    {
        ub960InstanceId = BOARD_CSI_INST_2;
    }else
    {
        printf("Error : Invalid chId 0x%x \n", chId);
        ub960InstanceId = -1;
    }

    return ub960InstanceId;
}

int32_t enableUB960Streaming(uint32_t chId)
{
    int32_t status = -1;
    int8_t ub960InstanceId = getUB960InstIdFromChId(chId);

    I2cParams ub960DesCSI2Enable[3u] = {
        {0x4C, 0x00, 0x10},
        {0x33, 0x03, 0x10},
        {0xFFFF, 0x00, 0x0} //End of script
    };

    if(ub960InstanceId < 0)
    {
        printf("Error : Invalid ub960InstanceId \n");
    }
    else
    {
        uint8_t   ub960I2cInstId = 0;
        uint8_t   ub960I2cAddr;
        uint16_t  regAddr = 0x4C;
        uint8_t   regVal = 0xAB;

        #if defined (MCU_PLUS_SDK)
        Board_fpdUb960GetI2CAddr(&ub960I2cAddr, ub960InstanceId);
        #else
        if(gFusion2Det)
        {
            uint8_t domain;
            Board_fpdUb9702GetI2CAddr(ub960InstanceId,
                                &domain,
                                &ub960I2cInstId,
                                &ub960I2cAddr);
        }
        else
        {
            Board_fpdU960GetI2CAddr(&ub960I2cInstId, &ub960I2cAddr, ub960InstanceId);
        }
        #endif

        status = Board_i2c8BitRegRd(gISS_Sensor_I2cHandle, ub960I2cAddr, regAddr, &regVal, 1U, SENSOR_I2C_TIMEOUT);

        if(0xFF != regVal)
        {
            /*broadcast mode is not set. Need to page select for the channel ID*/
            switch(chId)
            {
                case 0:
                case 4:
                case 8:
                    ub960DesCSI2Enable[0].nRegValue = 0x01;
                    break;
                case 1:
                case 5:
                case 9:
                    ub960DesCSI2Enable[0].nRegValue = 0x12;
                    break;
                case 2:
                case 6:
                case 10:
                    ub960DesCSI2Enable[0].nRegValue = 0x24;
                    break;
                case 3:
                case 7:
                case 11:
                    ub960DesCSI2Enable[0].nRegValue = 0x38;
                    break;
                case 0xFF:
                    ub960DesCSI2Enable[0].nRegValue = 0xF;
                    break;
                default:
                    printf("Error : enableUB960Streaming : Invalid channel ID 0x%x \n", chId);
                    status = -1;
                    break;
            }
        }
        status |= ub960_cfgScript(ub960DesCSI2Enable, ub960InstanceId);
    }

    return status;
}

int32_t disableUB960Streaming(uint32_t chId)
{
    int32_t status = -1;
    int8_t ub960InstanceId = getUB960InstIdFromChId(chId);

    I2cParams ub960DesCSI2Disable[3u] = {
        {0x4C, 0x00, 0x10},
        {0x33, 0x02, 0x10},
        {0xFFFF, 0x00, 0x0} //End of script
    };

    if(ub960InstanceId < 0)
    {
        printf("Error : Invalid ub960InstanceId \n");
    }
    else
    {
    switch(chId)
        {
        case 0:
        case 4:
        case 8:
            ub960DesCSI2Disable[0].nRegValue = 0x01;
            break;
        case 1:
        case 5:
        case 9:
            ub960DesCSI2Disable[0].nRegValue = 0x12;
            break;
        case 2:
        case 6:
        case 10:
            ub960DesCSI2Disable[0].nRegValue = 0x24;
            break;
        case 3:
        case 7:
        case 11:
            ub960DesCSI2Disable[0].nRegValue = 0x38;
            break;
        case 0xFF:
            ub960DesCSI2Disable[0].nRegValue = 0xF;
            break;
        default:
            printf("Error : disableUB960Streaming: Invalid channel ID 0x%x \n", chId);
            status = -1;
            break;
        }
        status = ub960_cfgScript(ub960DesCSI2Disable, ub960InstanceId);
    }

    return status;
}

// Selects a specific port on the UB960.
// This function is unaware of broadcast mode, check for broadcast mode
// before calling, otherwise broadcast mode will be clobbered.
// Returns -1 on errors, 0 on success.
int32_t UB960_SelectPort(uint32_t chId)
{
    int32_t status = -1;
    int8_t ub960InstanceId = getUB960InstIdFromChId(chId);
    uint8_t ub960I2cAddr;
    uint8_t ub960I2cInstId = 0;
    uint8_t portSelReg = 0x4C;
    uint8_t portSelVal = 1 << (chId%4) | (chId%4) << 4; // Select the proper port.

    
    #if defined (MCU_PLUS_SDK)
    Board_fpdUb960GetI2CAddr(&ub960I2cAddr, ub960InstanceId);
    #else
    Board_fpdU960GetI2CAddr(&ub960I2cInstId, &ub960I2cAddr, ub960InstanceId);
    #endif

    if(ub960I2cInstId != 0 && ub960I2cAddr != 0) {
        status = 0;
    }
    if(status == 0) {
        status = UB960_WriteReg(ub960I2cInstId, ub960I2cAddr, portSelReg, portSelVal); 
    }
    return status;
}

int32_t UB960_SetSerAlias(uint32_t chId, uint8_t ser_alias_i2c_addr_7bit)
{
    int32_t status = -1;
    int8_t ub960InstanceId = getUB960InstIdFromChId(chId);
    uint8_t ub960I2cAddr;
    uint8_t  ub960I2cInstId = 0;
    uint8_t portSelReg = 0x4C;
    uint8_t portSelVal;

    I2cParams ub960setSerAlias[2u] = {
        {0x5C, 0x0, 0x1},
        {0xFFFF, 0x0, 0x0}
    };

    #if defined (MCU_PLUS_SDK)
    Board_fpdUb960GetI2CAddr(&ub960I2cAddr, ub960InstanceId);
    #else
    Board_fpdU960GetI2CAddr(&ub960I2cInstId, &ub960I2cAddr, ub960InstanceId);
    #endif

    UB960_ReadReg(ub960I2cInstId, ub960I2cAddr, portSelReg, &portSelVal);
    if(0x0F != portSelVal)
    {
        status = UB960_SelectPort(chId);
    }

    if(status == 0) {
        ub960setSerAlias[0].nRegValue = ser_alias_i2c_addr_7bit << 1;
        status = ub960_cfgScript(ub960setSerAlias, ub960InstanceId);
    }

    return status;
}

int32_t UB960_SetAlias(uint32_t chId, uint8_t slot, uint8_t phy_i2c_addr_7bit, uint8_t alias_i2c_addr_7bit)
{
    int32_t status = -1;
    int8_t ub960InstanceId = getUB960InstIdFromChId(chId);
    uint8_t ub960I2cAddr;
    uint8_t  ub960I2cInstId = 0;
    uint8_t portSelReg = 0x4C;
    uint8_t portSelVal;

    uint8_t alias_register_pairs[8][2] = {
        {0x5D, 0x65}, 
        {0x5E, 0x66},
        {0x5F, 0x67},
        {0x60, 0x68},
        {0x61, 0x69},
        {0x62, 0x70},
        {0x63, 0x71},
        {0x64, 0x72}
    };

    I2cParams ub960setAlias[3u] = {
        {0x0, 0x0, 0x1},
        {0x0, 0x0, 0x1},
        {0xFFFF, 0x0, 0x0}
    };

    ub960setAlias[0].nRegAddr = alias_register_pairs[slot][0];
    ub960setAlias[1].nRegAddr = alias_register_pairs[slot][1]; // Select which alias we are setting.

    printf("Configuring Alias on UB960 CHID: %i, aliasing 0x%x to 0x%x\n", chId, phy_i2c_addr_7bit, alias_i2c_addr_7bit);

    #if defined (MCU_PLUS_SDK)
    Board_fpdUb960GetI2CAddr(&ub960I2cAddr, ub960InstanceId);
    #else
    Board_fpdU960GetI2CAddr(&ub960I2cInstId, &ub960I2cAddr, ub960InstanceId);
    #endif

    UB960_ReadReg(ub960I2cInstId, ub960I2cAddr, portSelReg, &portSelVal);
    if(0x0F != portSelVal)
    {
        status = UB960_SelectPort(chId);
    }

    if(status == 0) {
        ub960setAlias[0].nRegValue = phy_i2c_addr_7bit<<1U;
        ub960setAlias[1].nRegValue = alias_i2c_addr_7bit << 1U;
        status = ub960_cfgScript(ub960setAlias, ub960InstanceId);
    }

    return status;
}

int32_t UB960_SetSensorAlias(uint32_t chId, uint8_t sensor_phy_i2c_addr_7bit, uint8_t ser_alias_i2c_addr_7bit)
{
    int32_t status = -1;
    int8_t ub960InstanceId = getUB960InstIdFromChId(chId);
    uint8_t pageSelReg = 0x4C;
    uint8_t pageSelVal = 0xFF;
    uint8_t ub960I2cAddr;
    uint8_t  ub960I2cInstId;
    uint8_t  sensor_alias[ISS_SENSORS_MAX_SUPPORTED_SENSOR] =
        {SENSOR_0_I2C_ALIAS, SENSOR_1_I2C_ALIAS, SENSOR_2_I2C_ALIAS, SENSOR_3_I2C_ALIAS,
        SENSOR_4_I2C_ALIAS, SENSOR_5_I2C_ALIAS, SENSOR_6_I2C_ALIAS, SENSOR_7_I2C_ALIAS,
        SENSOR_8_I2C_ALIAS, SENSOR_9_I2C_ALIAS, SENSOR_10_I2C_ALIAS, SENSOR_11_I2C_ALIAS};

    I2cParams ub960setSensorAlias[5u] = {
        {0x4C, 0x00, 0x1},
        {0x65, 0x00, 0x1},
        {0x5E, 0x00, 0x1},
        {0x66, 0x00, 0x1},
        {0xFFFF, 0x00, 0x0} //End of script
    };

    if(ub960InstanceId < 0)
    {
        printf("Error : Invalid ub960InstanceId %d\n", ub960InstanceId);
    }
    else
    {
        #if defined (MCU_PLUS_SDK)
        Board_fpdUb960GetI2CAddr(&ub960I2cAddr, ub960InstanceId);
        #else
        if(gFusion2Det)
        {
            uint8_t domain;
            Board_fpdUb9702GetI2CAddr(ub960InstanceId,
                                &domain,
                                &ub960I2cInstId,
                                &ub960I2cAddr);
        }
        else
        {
            Board_fpdU960GetI2CAddr(&ub960I2cInstId, &ub960I2cAddr, ub960InstanceId);
        }
        #endif

        UB960_ReadReg(ub960I2cInstId, ub960I2cAddr, pageSelReg, &pageSelVal);
        if(0x0F == pageSelVal)
        {
            /*Broadcast Enabled. No need to set page select register*/
            ub960setSensorAlias[0].nRegValue = 0x0F;
        }
        else
        {
            /*Broadcast not enabled. Must set page select as per channel ID*/
            switch(chId)
            {
                case 0:
                case 4:
                case 8:
                    ub960setSensorAlias[0].nRegValue = 0x01;
                    break;
                case 1:
                case 5:
                case 9:
                    ub960setSensorAlias[0].nRegValue = 0x12;
                    break;
                case 2:
                case 6:
                case 10:
                    ub960setSensorAlias[0].nRegValue = 0x24;
                    break;
                case 3:
                case 7:
                case 11:
                    ub960setSensorAlias[0].nRegValue = 0x38;
                    break;
                case 0xFF:
                    ub960setSensorAlias[0].nRegValue = 0x0F;
                    break;
                default:
                    printf("Error : UB960_SetSensorAlias: Invalid channel ID 0x%x \n", chId);
                    status = -1;
                    break;
            }
        }

        ub960setSensorAlias[1].nRegValue = ser_alias_i2c_addr_7bit<<1U;
        ub960setSensorAlias[2].nRegValue = sensor_phy_i2c_addr_7bit << 1U;
        ub960setSensorAlias[3].nRegValue = sensor_alias[chId]<<1U;
        status = ub960_cfgScript(ub960setSensorAlias, ub960InstanceId);
    }

    return status;
}

void IssSensor_CloseHandle()
{
        uint8_t  ub960I2cInstId;
        uint8_t  ub960I2cAddr;
        #if defined (MCU_PLUS_SDK)
        Board_fpdUb960GetI2CAddr(&ub960I2cAddr, BOARD_CSI_INST_0);
        #else
        if(gFusion2Det)
        {
            uint8_t domain;
            Board_fpdUb9702GetI2CAddr(BOARD_FPD_9702_CSI2_DES_HUB1,
                                &domain,
                                &ub960I2cInstId,
                                &ub960I2cAddr);
        }
        else
        {
            Board_fpdU960GetI2CAddr(&ub960I2cInstId, &ub960I2cAddr, BOARD_CSI_INST_0);
        }
        I2C_close(gISS_Sensor_I2cHandle);
        #endif

        gISS_Sensor_I2cHandle = NULL;
}

int32_t ImageSensor_RemoteServiceHandler(char *service_name, uint32_t cmd,
    void *prm, uint32_t prm_size, uint32_t flags)
{
    int32_t status = -1;
    uint16_t cmdMemoryNeeded = 0;
    uint8_t * cmd_param = (uint8_t * )prm;
    uint8_t chId;
    uint32_t sensor_features_requested = 0;
    uint32_t channel_mask = 0;
    //uint32_t channel_mask_supported = 0;
    char * sensor_name = NULL;
    IssSensors_Handle * pSenHndl = NULL;
    IssSensor_CreateParams * pSenParams = (IssSensor_CreateParams * )NULL;

    switch(cmd)
    {
        case IM_SENSOR_CMD_ENUMERATE:
            issLogPrintf("ImageSensor_RemoteServiceHandler: IM_SENSOR_CMD_ENUMERATE \n");
            cmdMemoryNeeded = ISS_SENSORS_MAX_NAME*ISS_SENSORS_MAX_SUPPORTED_SENSOR;
            if(prm_size < cmdMemoryNeeded)
            {
                printf("Error : Insufficient prm size %d, need at least %d \n", prm_size, cmdMemoryNeeded);
                return -1;
            }

            IssSensor_DeserializerInit();
            
            {
                uint8_t count;
                for(count=0;count<ISS_SENSORS_MAX_SUPPORTED_SENSOR;count++)
                {
                    pSenHndl = gIssSensorTable[count];
                    if(NULL !=pSenHndl)
                    {
                        memcpy(cmd_param + (count*ISS_SENSORS_MAX_NAME), pSenHndl->createPrms->name, ISS_SENSORS_MAX_NAME);
                    }
                }
            }
            status = 0;

            break;
        case IM_SENSOR_CMD_QUERY:
            issLogPrintf("ImageSensor_RemoteServiceHandler: IM_SENSOR_CMD_QUERY \n");
            sensor_name = (char*)(cmd_param);
            issLogPrintf("Received Query for %s \n", sensor_name);

            /*Copy sensor properties at prm, after sensor name*/
            pSenParams = (IssSensor_CreateParams * )(cmd_param+ISS_SENSORS_MAX_NAME);
            status = IssSensor_GetSensorInfo(sensor_name, pSenParams);
            break;
        case IM_SENSOR_CMD_PWRON:
            issLogPrintf("ImageSensor_RemoteServiceHandler: IM_SENSOR_CMD_PWRON \n");
            sensor_name = (char*)(cmd_param);
            memcpy(&channel_mask, (cmd_param+ISS_SENSORS_MAX_NAME), sizeof(uint32_t));
            issLogPrintf("IM_SENSOR_CMD_PWRON : channel_mask = 0x%x \n", channel_mask);
            pSenHndl = getSensorHandleFromName(sensor_name);
            if(NULL == pSenHndl)
            {
                status = -1;
                printf("ERROR : NULL handle returned for sensor %s \n", sensor_name);
            }
            else
            {
                status = IssSensor_PowerOn((void*)pSenHndl, channel_mask);
            }
            break;
        case IM_SENSOR_CMD_CONFIG:
            issLogPrintf("ImageSensor_RemoteServiceHandler: IM_SENSOR_CMD_CONFIG \n");
            sensor_name = (char*)(cmd_param);
            memcpy(&sensor_features_requested, (cmd_param+ISS_SENSORS_MAX_NAME), sizeof(uint32_t));
            issLogPrintf("Application requested features = 0x%x \n ", sensor_features_requested);
            memcpy(&channel_mask, (cmd_param+ISS_SENSORS_MAX_NAME+sizeof(uint32_t)), sizeof(uint32_t));
            pSenHndl = getSensorHandleFromName(sensor_name);
            if(NULL == pSenHndl)
            {
                status = -1;
                printf("ERROR : NULL handle returned for sensor %s \n", sensor_name);
            }
            else
            {
                status = 0;
                int32_t probeStatus;
                //channel_mask_supported = (1<<pSenHndl->createPrms->num_channels) - 1;
                //channel_mask &= channel_mask_supported;
                if(1U == pSenHndl->sensorIntfPrms->sensorBroadcast)
                {
                    if((channel_mask & 0x00F) != 0)
                    {
                        issLogPrintf("Configuring all cameras on UB9xx_0 Des in broadcast mode \n");
                        status |= enableUB960Broadcast(0);
                        status |= IssSensor_Config((void*)pSenHndl, 0, sensor_features_requested);
                        channel_mask &= 0xFF0;
                    }
                    
                    if((channel_mask & 0x0F0) != 0)
                    {
                        issLogPrintf("Configuring all cameras on UB9xx_1 Des in broadcast mode \n");
                        status |= enableUB960Broadcast(1);
                        status |= IssSensor_Config((void*)pSenHndl, 4, sensor_features_requested);
                        channel_mask &= 0xF0F;
                    }

                    if((channel_mask & 0xF00) != 0)
                    {
                        issLogPrintf("Configuring all cameras on UB9xx_2 Des in broadcast mode \n");
                        status |= enableUB960Broadcast(2);
                        status |= IssSensor_Config((void*)pSenHndl, 8, sensor_features_requested);
                        channel_mask &= 0x0FF;
                    }
                }else
                {
                    issLogPrintf("Disabling broadcast mode \n");
                    disableUB960Broadcast(0);
                    disableUB960Broadcast(1);
                    if(gFusion2Det)
                    {
                        disableUB960Broadcast(2);
                    }
                }

                issLogPrintf("IM_SENSOR_CMD_CONFIG channel_mask = %x\n", channel_mask);

                if(0 != channel_mask)
                {
                    chId = 0;
                    while( (channel_mask > 0) && (chId < ISS_SENSORS_MAX_CHANNEL) )
                    {
                        if((channel_mask & 0x1) == 0x1)
                        {
                            probeStatus = pSenHndl->sensorFxns->probe(chId, pSenHndl);
                            if(probeStatus < 0)
                            {
                                printf("Error : sensor probe failed for channel %d \n ", chId);
                                status |= probeStatus;
                             }
                             else
                             {
                                issLogPrintf("Configuring camera # %d \n", chId);
                                status |= IssSensor_Config((void*)pSenHndl, chId, sensor_features_requested);
                             }
                        }
                        chId++;
                        channel_mask = channel_mask >> 1;
                    }
                }
            }
            issLogPrintf("IM_SENSOR_CMD_CONFIG returning status = %d\n", status);

            break;
        case IM_SENSOR_CMD_STREAM_ON:
            issLogPrintf("ImageSensor_RemoteServiceHandler: IM_SENSOR_CMD_STREAM_ON \n");
            sensor_name = (char*)(cmd_param);
            memcpy(&channel_mask, (cmd_param+ISS_SENSORS_MAX_NAME), sizeof(uint32_t));
            pSenHndl = getSensorHandleFromName(sensor_name);
            if(NULL == pSenHndl)
            {
                status = -1;
                printf("ERROR : NULL handle returned for sensor %s \n", sensor_name);
            }
            else
            {
                issLogPrintf("IM_SENSOR_CMD_STREAM_ON:  channel_mask = 0x%x\n", channel_mask);
                status = 0;

                //channel_mask_supported = (1<<pSenHndl->createPrms->num_channels) - 1;
                //channel_mask &= channel_mask_supported;
                /* Broadcast stream on to sensor is unstable. Setting this to 2 so it never goes in, but leaving the code to be fixed */
                if(2U == pSenHndl->sensorIntfPrms->sensorBroadcast)
                {
                    if((channel_mask & 0x00F) != 0)
                    {
                        issLogPrintf("Starting all cameras on UB9xx_0 Des in broadcast mode \n");
                        status |= enableUB960Broadcast(0);
                        status |= IssSensor_Start((void*)pSenHndl, 0);
                        channel_mask &= 0xFF0;
                    }

                    if((channel_mask & 0x0F0) != 0)
                    {
                        issLogPrintf("Starting all cameras on UB9xx_1 Des in broadcast mode \n");
                        status |= enableUB960Broadcast(1);
                        status |= IssSensor_Start((void*)pSenHndl, 4);
                        channel_mask &= 0xF0F;
                    }

                    if((channel_mask & 0xF00) != 0)
                    {
                        issLogPrintf("Starting all cameras on UB9xx_2 Des in broadcast mode \n");
                        status |= enableUB960Broadcast(2);
                        status |= IssSensor_Start((void*)pSenHndl, 8);
                        channel_mask &= 0x0FF;
                    }
                }

                /*Disable broadcast after enabling streaming 
                so that every camera can have independent 2A control */
                disableUB960Broadcast(0);
                disableUB960Broadcast(1);
                if(gFusion2Det)
                {
                    disableUB960Broadcast(2);
                }

                chId = 0;

                while( (channel_mask > 0) && (chId < ISS_SENSORS_MAX_CHANNEL) )
                {
                    if(channel_mask & 0x1)
                    {
                        status |= IssSensor_Start((void*)pSenHndl, chId);
                        if(status < 0)
                        {
                            printf("Error : Failed to start sensor at channel Id %d \n", chId);
                        }
                        if( NULL ==g_pSenHndl[chId])
                        {
                            printf("Error : sensor handle at channel Id %d = NULL \n", chId);
                        }
                    }

                    chId++;
                    channel_mask = channel_mask >> 1U;
                }
            }

            break;
        case IM_SENSOR_CMD_STREAM_OFF:
            issLogPrintf("ImageSensor_RemoteServiceHandler: IM_SENSOR_CMD_STREAM_OFF \n");

            sensor_name = (char*)(cmd_param);
            memcpy(&channel_mask, (cmd_param+ISS_SENSORS_MAX_NAME), sizeof(uint32_t));
            pSenHndl = getSensorHandleFromName(sensor_name);
            if(NULL == pSenHndl)
            {
                status = -1;
                printf("ERROR : NULL handle returned for sensor %s \n", sensor_name);
            }
            else
            {
                issLogPrintf("IM_SENSOR_CMD_STREAM_ON:  channel_mask = 0x%x\n", channel_mask);
                status = 0;

                chId = 0;

                while( (channel_mask > 0) && (chId < ISS_SENSORS_MAX_CHANNEL) )
                {
                    if(channel_mask & 0x1)
                    {
                        status = IssSensor_Stop((void*)pSenHndl, chId);
                        if(status < 0)
                        {
                            printf("Warning : Failed to stop sensor at channel Id %d \n", chId);
                        }
                        if( NULL !=g_pSenHndl[chId])
                        {
                            printf("Warning : sensor handle at channel Id %d is not NULL \n", chId);
                        }
                    }

                    chId++;
                    channel_mask = channel_mask >> 1U;
                }
            }
            break;
        case IM_SENSOR_CMD_PWROFF:
            IssSensor_CloseHandle();
            status = 0;
            break;
        case IM_SENSOR_CMD_CTL:
            status = IssSensor_Control(NULL, 0, (void*)cmd_param, NULL);
            break;
        case IM_SENSOR_CMD_DETECT:
            {
                uint8_t i = 0;
                uint8_t channel_id = 0;
                int32_t probeStatus;
                uint8_t serializers_detected = 0;
                memcpy(&channel_mask, cmd_param, sizeof(uint32_t));

                probeStatus = IssSensor_detect_serializer(BOARD_CSI_INST_0, &serializers_detected);
                if(probeStatus != 0)
                {
                    issLogPrintf("IssSensor_detect_serializer returned 0x%x \n", probeStatus);
                }

                probeStatus = IssSensor_detect_serializer(BOARD_CSI_INST_1, &serializers_detected);
                if(probeStatus != 0)
                {
                    issLogPrintf("IssSensor_detect_serializer returned 0x%x \n", probeStatus);
                }

                if(gFusion2Det)
                {
                    probeStatus = IssSensor_detect_serializer(BOARD_CSI_INST_2, &serializers_detected);
                    if(probeStatus != 0)
                    {
                        issLogPrintf("IssSensor_detect_serializer returned 0x%x \n", probeStatus);
                    }
                }

                while(channel_mask > 0)
                {
                    if(channel_mask & 0x1)
                    {
                        int32_t detectedSensor;
                        int32_t tmp = (1<<channel_id);
                        if(tmp&serializers_detected)
                        {
                            if(g_detectedSensors[channel_id] == 0xFF)
                            {
                                /*Sensor has not been detected at this port yet. Try to detect */
                                detectedSensor = IssSensor_detect_sensor(channel_id);
                                if(detectedSensor < 0)
                                {
                                    issLogPrintf("IM_SENSOR_CMD_DETECT found serializer but no sensor at chId %d \n", chId);
                                    detectedSensor = 0xFF;
                                }
                                else
                                {
                                    issLogPrintf("IM_SENSOR_CMD_DETECT : Found sensor %d at port %d \n", detectedSensor, channel_id);
                                }
                                g_detectedSensors[channel_id] = (uint8_t)detectedSensor;
                                cmd_param[channel_id] = detectedSensor;
                            }
                            else
                            {
                                /*Sensor has previously been detected at this port. It might be getting used.*/
                                /*Trying to probe can overwrite the configuration*/
                                /*Return the previously detected ID instead of probing again*/
                                issLogPrintf("IM_SENSOR_CMD_DETECT : Previously found sensor %d at port %d \n", g_detectedSensors[chId], chId);
                                cmd_param[channel_id] = (uint8_t)g_detectedSensors[channel_id];
                            }
                        }else
                        {
                            issLogPrintf("IM_SENSOR_CMD_DETECT No serializer found at port %d \n", channel_id);
                            cmd_param[channel_id] = 0xFF;
                        }
                    }
                    else
                    {
                        cmd_param[channel_id] = 0xFF;
                    }
                    channel_mask = channel_mask >> 1;
                    channel_id++;
                }
                status = 0;
            }
            break;
        default:
            issLogPrintf("ImageSensor_RemoteServiceHandler : Unsupported command : %d\n", cmd);
            status = -1;
    }

    return status;
}

int32_t IssSensor_DeInit()
{
    int32_t status = -1;
    status = appRemoteServiceUnRegister(IMAGE_SENSOR_REMOTE_SERVICE_NAME);
    if(status!=0)
    {
        printf(" REMOTE_SERVICE_SENSOR: ERROR: Unable to unregister remote service sensor handler\n");
    }
    return status;
}

