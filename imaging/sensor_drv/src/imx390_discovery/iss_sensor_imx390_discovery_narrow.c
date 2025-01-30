/*
* The MIT License (MIT)
*
* Copyright (C) 2024 Define Design Deploy Corp.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of
* this software and associated documentation files (the “Software”), to deal in
* the Software without restriction, including without limitation the rights to use,
* copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
* Software, and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*/

/*!
 * @file        iss_sensor_imx390_discovery_narrow.c
 * @brief       Driver implementation for narrow-type cameras
 * @details    ************************************************* 
 * @copyright   Copyright (C) 2024 Define Design Deploy Corp. All Rights Reserved.
 */

#include "iss_sensor_imx390_discovery.h"
#include "imx390_discovery_serdes_config.h"
#include "d3_eeprom.h"

static int32_t IMX390_Probe_Narrow(uint32_t chId, void *pSensorHdl);

static uint8_t i2cAddrMisc[ISS_SENSORS_MAX_CHANNEL] = {
    MISC_0_I2C_ALIAS, MISC_1_I2C_ALIAS, MISC_2_I2C_ALIAS, MISC_3_I2C_ALIAS,
    MISC_4_I2C_ALIAS, MISC_5_I2C_ALIAS, MISC_6_I2C_ALIAS, MISC_7_I2C_ALIAS,
    MISC_8_I2C_ALIAS, MISC_9_I2C_ALIAS, MISC_10_I2C_ALIAS, MISC_11_I2C_ALIAS};

I2cParams ub953SerCfg_D3IMX390_DISCOVERY_NARROW[IMX390_DISCOVERY_SER_CFG_SIZE] = {
    {0x01, 0x01, 0x20},
    {0x02, 0x72, 0x10},
    //{0x07, 0x7D, 0x1F},
    {0x07, 0xFA, 0x1F},
    {0x06, 0x9B, 0x10},
    {0xB0, 0x04, 0x10},
    {0xB1, 0x08, 0x10},
    {0xB2, 0x07, 0x10},
    {0xFFFF, 0x00, 0x0} //End of script
};

IssSensorConfig     imx390DiscoveryNarrowSensorRegConfigWdr = {
    NULL,                               /*desCfgPreScript*/
    ub953SerCfg_D3IMX390_DISCOVERY_NARROW,     /*serCfgPreScript*/
    iMX390WdrConfig,                    /*sensorCfgPreScript*/
    NULL,                               /*desCfgPostScript*/
    NULL,                               /*serCfgPostScript*/
    NULL,                               /*sensorCfgPostScript*/
};

static IssSensor_CreateParams  imx390DISCOVERYCreatePrms = {
    SENSOR_SONY_IMX390_UB953_DISCOVERY_NARROW,     /*sensor name*/
    0x6,                             /*i2cInstId*/
    {
        SENSOR_0_I2C_ALIAS, SENSOR_1_I2C_ALIAS, SENSOR_2_I2C_ALIAS, SENSOR_3_I2C_ALIAS,
        SENSOR_4_I2C_ALIAS, SENSOR_5_I2C_ALIAS, SENSOR_6_I2C_ALIAS, SENSOR_7_I2C_ALIAS,
        SENSOR_8_I2C_ALIAS, SENSOR_9_I2C_ALIAS, SENSOR_10_I2C_ALIAS, SENSOR_11_I2C_ALIAS
    },/*i2cAddrSensor*/
    {
        SER_0_I2C_ALIAS, SER_1_I2C_ALIAS, SER_2_I2C_ALIAS, SER_3_I2C_ALIAS,
        SER_4_I2C_ALIAS, SER_5_I2C_ALIAS, SER_6_I2C_ALIAS, SER_7_I2C_ALIAS,
        SER_8_I2C_ALIAS, SER_9_I2C_ALIAS, SER_10_I2C_ALIAS, SER_11_I2C_ALIAS
    },/*i2cAddrSer*/
    /*IssSensor_Info*/
    {
        {
            IMX390_OUT_WIDTH,               /*width*/
            IMX390_OUT_HEIGHT-IMX390_META_HEIGHT_AFTER,            /*height*/
            1,                              /*num_exposures*/
            vx_false_e,                     /*line_interleaved*/
            {
                {TIVX_RAW_IMAGE_16_BIT, 11},    /*dataFormat and MSB [0]*/
            },
            0,                              /*meta_height_before*/
            IMX390_META_HEIGHT_AFTER,      /*meta_height_after*/
        },
        ISS_SENSOR_IMX390_DISCOVERY_FEATURES,     /*features*/
        ALGORITHMS_ISS_AEWB_MODE_AEWB,  /*aewbMode*/
        30,                             /*fps*/
        4,                              /*numDataLanes*/
        {1, 2, 3, 4},                   /*dataLanesMap*/
        {0, 0, 0, 0},                   /*dataLanesPolarity*/
        CSIRX_LANE_BAND_SPEED_720_TO_800_MBPS,                            /*CSI Clock*/
    },
    8,                                  /*numChan*/
    390,                                /*dccId*/
};

static IssSensorFxns           im390SensorFxns = {
    IMX390_Probe_Narrow,
    IMX390_Config,
    IMX390_StreamOn,
    IMX390_StreamOff,
    IMX390_PowerOn,
    IMX390_PowerOff,
    IMX390_GetExpParams,
    IMX390_SetAeParams,
    IMX390_GetDccParams,
    IMX390_InitAewbConfig,
    IMX390_GetIspConfig,
    IMX390_ReadWriteReg,
    IMX390_GetExpPrgFxn,
    IMX390_deinit,
    IMX390_GetWBPrgFxn,
    IMX390_SetAwbParams   
};

static IssSensorIntfParams     imx390SensorIntfPrms = {
    0,             /*sensorBroadcast*/
    0,             /*enableFsin*/
    0,             /*numCamerasStreaming*/
};


IssSensors_Handle imx390DISCOVERYNarrowSensorHandle = {
    1,                                  /*isUsed*/
    &imx390DISCOVERYCreatePrms,         /*CreatePrms*/
    &im390SensorFxns,                   /*SensorFxns*/
    &imx390SensorIntfPrms,              /*SensorIntfPrms*/
};
/*
 * \brief Init
 */
int32_t IssSensor_IMX390DISCOVERY_NARROW_Init()
{    
    int32_t status;
    
    issLogPrintf("IssSensor_IMX390DISCOVERY_NARROW_Init \n");

    status = IssSensor_Register(&imx390DISCOVERYNarrowSensorHandle);    
    if(0 != status)
    {
        printf("%s failed \n", __func__);
    }
    return status;
}

/*******************************************************************************
 *  Local Functions Definition
 *******************************************************************************
 */

static int32_t IMX390_Probe_Narrow(uint32_t chId, void *pSensorHdl)
{
    int32_t status = -1;
    uint32_t i2cInstId;
    uint8_t sensorI2cAddr;
    uint8_t pcalI2cAddr;
    uint8_t eepromI2cAddr;
    uint16_t chipIdRegAddr = IMX390_CHIP_ID_REG_ADDR;
    uint8_t chipIdRegValueRead = 0xAB;
    IssSensors_Handle * pSenHandle = (IssSensors_Handle*)pSensorHdl;
    IssSensor_CreateParams * pCreatePrms;
    I2cParams    * serCfg = NULL;
    uint8_t count=0;
    uint8_t max_retries = 1;

    assert(NULL != pSenHandle);
    pCreatePrms = pSenHandle->createPrms;
    assert(NULL != pCreatePrms);

    i2cInstId = pCreatePrms->i2cInstId;
    sensorI2cAddr = pCreatePrms->i2cAddrSensor[chId];
    pcalI2cAddr = i2cAddrMisc[chId];
    eepromI2cAddr = i2cAddrMisc[chId];

    
    status = UB960_SetAlias(chId, 1, IMX390_DISCOVERY_I2C_ADDR >> 1, sensorI2cAddr);
    status = UB960_SetAlias(chId, 2, IMX390_PCAL_ADDRESS >> 1, pcalI2cAddr);
    //status = UB960_SetSensorAlias(chId, IMX390_DISCOVERY_I2C_ADDR >> 1, sensorI2cAddr);
    //status = UB960_SetAlias(chId, 2, IMX390_PCAL_ADDRESS >> 1, pcalI2cAddr);
    I2C_Handle sensorI2cHandle = NULL;
    static uint8_t sensorI2cByteOrder = 255U;
    getIssSensorI2cInfo(&sensorI2cByteOrder, &sensorI2cHandle);
    uint8_t regVal = 0xFE;
    status = Board_i2c8BitRegWr(sensorI2cHandle, pcalI2cAddr, 0x03, &regVal, 1, SENSOR_I2C_TIMEOUT);
    regVal = 0x01;
    status = Board_i2c8BitRegWr(sensorI2cHandle, pcalI2cAddr, 0x01, &regVal, 1, SENSOR_I2C_TIMEOUT);
    
    if(status != 0) {
        printf("IMX390 Error: Configuring PCAL for Reset line of ch %d returned %d \n", chId, status);
        return status;
    }

    appLogWaitMsecs(200);

    //Camera Type
    issLogPrintf("IMX390_Probe_Narrow-> getCameraTypeFromEEPROM \n");

    // Set serializer alias
    status = UB960_SetAlias(chId, 0, 0x18, pCreatePrms->i2cAddrSer[chId]);
    if(0 != status)
    {
        printf("IMX390_Probe_Narrow Error: UB960_SetSensorAlias for chId %d returned %d \n", chId, status);
        return status;
    }

    serCfg = imx390DiscoveryNarrowSensorRegConfigWdr.serCfgPreScript;
    /*The code assumes that I2C instance is the same for sensor and serializer*/
    if(NULL != serCfg)
    {
        status = ub953_cfgScript(i2cInstId, pCreatePrms->i2cAddrSer[chId], serCfg);
    }

    /*Read chip ID to detect if the sensor can be detected*/
    while( (chipIdRegValueRead != IMX390_CHIP_ID_REG_VAL) && (count < max_retries))
    {
        status = IMX390_ReadReg(i2cInstId, sensorI2cAddr, chipIdRegAddr, &chipIdRegValueRead, 1U);
        if(status == 0 )
        {
            if(chipIdRegValueRead == IMX390_CHIP_ID_REG_VAL)
            {
                status = 0;
                issLogPrintf("IMX390_Probe_Narrow SUCCESS : Read expected value 0x%x at chip ID register 0x%x \n", IMX390_CHIP_ID_REG_VAL, chipIdRegAddr);
            }
            else
            {
                status = -1;
                issLogPrintf("IMX390_Probe_Narrow : 0x%x read at chip ID register 0x%x. Expected 0x%x \n", chipIdRegValueRead, chipIdRegAddr, IMX390_CHIP_ID_REG_VAL);
                issLogPrintf("IMX390_Probe_Narrow Failed.. Retrying \n");
                appLogWaitMsecs(100);
            }
        }
        else
        {
            issLogPrintf("IMX390 Probe : Failed to read CHIP_ID register 0x%x \n", chipIdRegAddr);
        }
        count++;
    }

    #ifndef SKIP_EEPROM
    // Steal PCAL I2C Alias and use it for eeprom instead - we no longer need the PCAL I2C address
    status = UB960_SetAlias(chId, 2, M24C64_MEM_I2C >> 1, eepromI2cAddr);
    camera_type_t camera = getCameraTypeFromEEPROM(sensorI2cHandle, eepromI2cAddr);
    if(camera == IMX390_NARROW) {
        issLogPrintf("Success: Camera is IMX390 Discovery Narrow FOV");
        status = 0;
    } else if(camera == IMX390_MID) {
        issLogPrintf("Failure: Camera is IMX390 Discovery Mid FOV");
        status = -1;
    } else if(camera == IMX390_WIDE) {
        issLogPrintf("Failure: Camera is IMX390 Discovery Wide FOV");
        status = -1;
    } else {
        issLogPrintf("Failure: Could not query camera EEPROM");
        status = -1;
    }
    #endif

    return (status);
}
