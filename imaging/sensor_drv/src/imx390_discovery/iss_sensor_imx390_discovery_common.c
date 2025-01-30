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
 * @file        iss_sensor_imx390_discovery_common.c
 * @brief       Common methods shared by different types of cameras
 * @details     ***************************** 
 * @copyright   Copyright (C) 2024 Define Design Deploy Corp. All Rights Reserved.
 */
#include "iss_sensor_imx390_discovery.h"
#include "imx390_discovery_serdes_config.h"

/*
 * \brief DCC Parameters of IMX390
 */
//IssCapture_CmplxIoLaneCfg           imx390Csi2CmplxIoLaneCfg;

extern IssSensors_Handle * gIssSensorTable[ISS_SENSORS_MAX_SUPPORTED_SENSOR];
static uint16_t sp1hGainRegValueOld[ISS_SENSORS_MAX_CHANNEL];
static uint16_t redGain_prev[ISS_SENSORS_MAX_CHANNEL];
static uint16_t greenGain_prev[ISS_SENSORS_MAX_CHANNEL];
static uint16_t blueGain_prev[ISS_SENSORS_MAX_CHANNEL];

// Discovery Camera
I2cParams ub953SerCfg_D3IMX390_DISCOVERY[IMX390_DISCOVERY_SER_CFG_SIZE] = {
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

IssSensorConfig     imx390DiscoverySensorRegConfigLinear = {
    NULL,                               /*desCfgPreScript*/
    ub953SerCfg_D3IMX390_DISCOVERY,     /*serCfgPreScript*/
    iMX390LinearConfig,                 /*sensorCfgPreScript*/
    NULL,                               /*desCfgPostScript*/
    NULL,                               /*serCfgPostScript*/
    NULL,                               /*sensorCfgPostScript*/
};

IssSensorConfig     imx390DiscoverySensorRegConfigWdr = {
    NULL,                               /*desCfgPreScript*/
    ub953SerCfg_D3IMX390_DISCOVERY,     /*serCfgPreScript*/
    iMX390WdrConfig,                    /*sensorCfgPreScript*/
    NULL,                               /*desCfgPostScript*/
    NULL,                               /*serCfgPostScript*/
    NULL,                               /*sensorCfgPostScript*/
};

IssSensorConfig     imx390DiscoverySensorRegConfigWdr60fps = {
    NULL,                               /*desCfgPreScript*/
    ub953SerCfg_D3IMX390_DISCOVERY,     /*serCfgPreScript*/
    iMX390Wdr60fpsConfig,               /*sensorCfgPreScript*/
    NULL,                               /*desCfgPostScript*/
    NULL,                               /*serCfgPostScript*/
    NULL,                               /*sensorCfgPostScript*/
};

IssSensorFxns           imx390DiscoSensorFxns = {
    NULL,
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

/*******************************************************************************
 *  Local Functions Definition
 *******************************************************************************
 */
int32_t IMX390_Sensor_RegConfig(uint32_t i2cInstId, uint8_t sensorI2cAddr, I2cParams *sensorCfg, uint16_t sensor_cfg_script_len)
{
    int32_t status = 0;
    uint16_t regAddr;
    uint8_t regValue;
    uint16_t delayMilliSec;
    uint32_t regCnt;

    if(NULL != sensorCfg)
    {
        regCnt = 0;
        regAddr  = sensorCfg[regCnt].nRegAddr;
        regValue = sensorCfg[regCnt].nRegValue;
        delayMilliSec = sensorCfg[regCnt].nDelay;

        printf(" Configuring IMX390 imager 0x%x.. Please wait till it finishes \n", sensorI2cAddr);
        while(regCnt<sensor_cfg_script_len)
        {
            status |= IMX390_WriteReg(i2cInstId, sensorI2cAddr, regAddr, regValue, 1u);

            if (0 != status)
            {
                printf(" \n \n IMX390: Sensor Reg Write Failed for regAddr 0x%x \n \n", regAddr);
            }

            if(delayMilliSec > 0)
            {
               appLogWaitMsecs(delayMilliSec);
            }

            regCnt++;
            regAddr  = sensorCfg[regCnt].nRegAddr;
            regValue = sensorCfg[regCnt].nRegValue;
            delayMilliSec = sensorCfg[regCnt].nDelay;
        }
        /*Wait 100ms after the init is done*/
        appLogWaitMsecs(100);
        printf(" IMX390 config done\n");
    }
    else
    {
        printf(" IMX390 config script is NULL \n");
    }
    return status;
}

static uint32_t imx390FeaturesEnabled;
int32_t IMX390_Config(uint32_t chId, void *pSensorHdl, uint32_t sensor_features_requested)
{
    int32_t status = 0;
    uint32_t i2cInstId;
    uint16_t sensor_cfg_script_len = 0;
    I2cParams *sensorCfg = NULL;
    I2cParams *serCfg = NULL;
    IssSensors_Handle * pSenHandle = (IssSensors_Handle*)pSensorHdl;
    IssSensor_CreateParams * pCreatePrms;
    uint8_t sensorI2Caddr = IMX390_DISCOVERY_I2C_ADDR;

    assert(NULL != pSenHandle);
    pCreatePrms = pSenHandle->createPrms;
    assert(NULL != pCreatePrms);

    if(sensor_features_requested != (sensor_features_requested & ISS_SENSOR_IMX390_FEATURES))
    {
        printf("IMX390_Config : Error. feature set 0x%x is not supported \n", sensor_features_requested);
        return -1;
    }

    imx390FeaturesEnabled= sensor_features_requested;
    i2cInstId = pCreatePrms->i2cInstId;

    if(ISS_SENSOR_FEATURE_CFG_UC1 == (sensor_features_requested & ISS_SENSOR_FEATURE_CFG_UC1))
    {
        serCfg = imx390DiscoverySensorRegConfigWdr60fps.serCfgPreScript;
        sensorCfg = imx390DiscoverySensorRegConfigWdr60fps.sensorCfgPreScript;
        sensor_cfg_script_len = IMX390_WDR_CONFIG_60FPS_SIZE;
    }
    else
    {
        if(sensor_features_requested & ISS_SENSOR_FEATURE_COMB_COMP_WDR_MODE)
        {
            serCfg = imx390DiscoverySensorRegConfigWdr.serCfgPreScript;
            sensorCfg = imx390DiscoverySensorRegConfigWdr.sensorCfgPreScript;
            sensor_cfg_script_len = IMX390_WDR_CONFIG_SIZE;
        }else
        {
            serCfg = imx390DiscoverySensorRegConfigLinear.serCfgPreScript;
            sensorCfg = imx390DiscoverySensorRegConfigLinear.sensorCfgPreScript;
            sensor_cfg_script_len = IMX390_LINEAR_CONFIG_SIZE;
        }
    }

    /*Deserializer config is done in IssSensor_PowerOn, Need to set sensor alias*/
    status = UB960_SetAlias(chId, 0, 0x18, pCreatePrms->i2cAddrSer[chId]);
    status = UB960_SetAlias(chId, 1, sensorI2Caddr >> 1, pCreatePrms->i2cAddrSensor[chId]);
    //status |= UB960_SetSerAlias(chId, pCreatePrms->i2cAddrSer[chId]);

    //status = UB960_SetSensorAlias(chId, sensorI2Caddr >> 1, pCreatePrms->i2cAddrSer[chId]);
    if(0 != status)
    {
        printf("IMX390_Config Error : UB960_SetSensorAlias for chId %d returned %d \n", chId, status);
    }else
    {
        status = ub953_cfgScript(i2cInstId, pCreatePrms->i2cAddrSer[chId], serCfg);
        if(0 != status)
        {
            printf("IMX390_Config Error : UB953 config failed for camera # %d \n", chId);
        }else
        {
            status = IMX390_Sensor_RegConfig(i2cInstId, pCreatePrms->i2cAddrSensor[chId], sensorCfg, sensor_cfg_script_len);
         }
    }
    return (status);
}

int32_t IMX390_StreamOn(uint32_t chId, void *pSensorHdl)
{
    int32_t status = 0;

    IssSensors_Handle * pSenHandle = (IssSensors_Handle*)pSensorHdl;
    IssSensor_CreateParams * pCreatePrms;
    uint32_t i2cInstId;
    uint8_t sensorI2cAddr;
    int8_t ub960InstanceId = getUB960InstIdFromChId(chId);

    if(ub960InstanceId < 0)
    {
        printf("Invalid ub960InstanceId \n");
        return -1;
    }

    assert(NULL != pSenHandle);
    pCreatePrms = pSenHandle->createPrms;
    assert(NULL != pCreatePrms);

    if(ISS_SENSOR_FEATURE_CFG_UC1 == (imx390FeaturesEnabled& ISS_SENSOR_FEATURE_CFG_UC1))
    {
        if(pSenHandle->sensorIntfPrms->numCamerasStreaming >= 3U)
        {
            printf("IMX390_StreamOn Error : %d cameras streaming already \n", pSenHandle->sensorIntfPrms->numCamerasStreaming);
            printf("IMX390_StreamOn Error : 60fps mode can support upto 3 cameras because of UB960 b/w limitation \n");
            return -1;
        }
    }

    i2cInstId = pCreatePrms->i2cInstId;
    sensorI2cAddr = pCreatePrms->i2cAddrSensor[chId];

    status |= IMX390_WriteReg(i2cInstId, sensorI2cAddr, 0x0, 0x0, 1u);/*ACTIVE*/
    appLogWaitMsecs(10);
    status |= enableUB960Streaming(chId);
    return (status);
}

int32_t IMX390_StreamOff(uint32_t chId, void *pSensorHdl)
{
    int32_t status = 0;
    IssSensors_Handle * pSenHandle = (IssSensors_Handle*)pSensorHdl;
    IssSensor_CreateParams * pCreatePrms;
    uint32_t i2cInstId;
    uint8_t sensorI2cAddr;

    assert(NULL != pSenHandle);
    pCreatePrms = pSenHandle->createPrms;
    assert(NULL != pCreatePrms);

    i2cInstId = pCreatePrms->i2cInstId;
    sensorI2cAddr = pCreatePrms->i2cAddrSensor[chId];

    status |= IMX390_WriteReg(i2cInstId, sensorI2cAddr, 0x0, 0x1, 1u);/*STANDBY*/
    appLogWaitMsecs(10);
    status |= disableUB960Streaming(chId);
    return status;
}

int32_t IMX390_PowerOn(uint32_t chId, void *pSensorHdl)
{
    int32_t status = 0;

    sp1hGainRegValueOld[chId] = 0;
    redGain_prev[chId] = greenGain_prev[chId] = blueGain_prev[chId] = 512;

    printf("IMX390_PowerOn : chId = 0x%x \n", chId);

    return status;
}

int32_t IMX390_PowerOff(uint32_t chId, void *pSensorHdl)
{
    return (0);
}

static uint16_t sp1hGainRegValueOld[ISS_SENSORS_MAX_CHANNEL];
int32_t IMX390_SetAeParams(void *pSensorHdl, uint32_t chId, IssSensor_ExposureParams *pExpPrms)
{
    uint16_t regAddr;
    uint16_t cnt;
    uint8_t regValue;
    int32_t status = -1;
    IssSensors_Handle * pSenHandle = (IssSensors_Handle*)pSensorHdl;
    IssSensor_CreateParams * pCreatePrms;
    uint32_t i2cInstId;
    uint8_t sensorI2cAddr;
    uint32_t sp1h_again = 0U;

    assert(NULL != pSenHandle);
    pCreatePrms = pSenHandle->createPrms;
    assert(NULL != pCreatePrms);

    i2cInstId = pCreatePrms->i2cInstId;
    sensorI2cAddr = pCreatePrms->i2cAddrSensor[chId];

    /* Exp time is fixed to 11ms for LFM. Set Analog Gain Only */

    for (cnt = 0; cnt < ISS_IMX390_GAIN_TBL_SIZE; cnt ++)
    {
        if (pExpPrms->analogGain[ISS_SENSOR_EXPOSURE_LONG] <= gIMX390GainsTable[cnt][0])
        {
            sp1h_again = gIMX390GainsTable[cnt][1];
            break;
        }
    }

    if(sp1hGainRegValueOld[chId] == sp1h_again)
    {
        /*Reduce I2C transactions.
        Do not write to the sensor if register value does not change */
        return 0;
    }
    sp1hGainRegValueOld[chId] = sp1h_again;

    regAddr = 0x0008;
    regValue = 1;
    status = IMX390_WriteReg(i2cInstId, sensorI2cAddr, regAddr, regValue, 1u);
    if(status != 0)
    {
        printf("Error writing 0x%x to IMX390 register 0x%x \n", regValue, regAddr);
    }

    regAddr = IMX390_SP1H_ANALOG_GAIN_CONTROL_REG_ADDR;
    regValue = sp1h_again & 0xFF;
    status = IMX390_WriteReg(i2cInstId, sensorI2cAddr, regAddr, regValue, 1u);
    if(status != 0)
    {
        printf("Error writing 0x%x to IMX390 register 0x%x \n", regValue, regAddr);
    }

    regAddr = IMX390_SP1H_ANALOG_GAIN_CONTROL_REG_ADDR_HIGH;
    regValue = sp1h_again >> 8;
    status = IMX390_WriteReg(i2cInstId, sensorI2cAddr, regAddr, regValue, 1u);
    if(status != 0)
    {
        printf("Error writing 0x%x to IMX390 register 0x%x \n", regValue, regAddr);
    }

    regAddr = 0x0008;
    regValue = 0;
    status = IMX390_WriteReg(i2cInstId, sensorI2cAddr, regAddr, regValue, 1u);
    if(status != 0)
    {
        printf("Error writing 0x%x to IMX390 register 0x%x \n", regValue, regAddr);
    }

    return (status);
}

int32_t IMX390_GetDccParams(uint32_t chId, void *pSensorHdl, IssSensor_DccParams *pDccPrms)
{
    int32_t status = 0;
    return (status);
}

int32_t IMX390_GetExpParams(uint32_t chId, void *pSensorHdl, IssSensor_ExposureParams *pExpPrms)
{
    int32_t status = 0;

    assert(NULL != pExpPrms);
    pExpPrms->expRatio = ISS_SENSOR_IMX390_DEFAULT_EXP_RATIO;

    return (status);
}

void IMX390_InitAewbConfig(uint32_t chId, void *pSensorHdl)
{
    return;
}

void IMX390_GetIspConfig (uint32_t chId, void *pSensorHdl)
{
    return;
}

void IMX390_deinit (uint32_t chId, void *pSensorHdl)
{
    return;
}

int32_t IMX390_ReadWriteReg (uint32_t chId, void *pSensorHdl, uint32_t readWriteFlag, I2cParams *pReg)
{
    int32_t status = 0;

    uint8_t regValue = 0;
    IssSensors_Handle * pSenHandle = (IssSensors_Handle*)pSensorHdl;
    IssSensor_CreateParams * pCreatePrms;

    assert(NULL != pSenHandle);
    pCreatePrms = pSenHandle->createPrms;

    assert(NULL != pReg);

    if (1u == readWriteFlag)
    {
        /*write*/
        regValue = pReg->nRegValue;
        status = IMX390_WriteReg(pCreatePrms->i2cInstId,
            pCreatePrms->i2cAddrSensor[chId], pReg->nRegAddr, regValue, 1u);
    }
    else
    {
        /*read*/
        status = IMX390_ReadReg(pCreatePrms->i2cInstId,
            pCreatePrms->i2cAddrSensor[chId], pReg->nRegAddr, &regValue, 1u);

        if (0 == status)
        {
            pReg->nRegValue = regValue;
        }
    }
    return (status);
}

int32_t IMX390_ReadReg(uint8_t     i2cInstId,
                            uint8_t         i2cAddr,
                            uint16_t        regAddr,
                            uint8_t         *regVal,
                            uint32_t        numRegs)
{
    int32_t  status = -1;
    I2C_Handle sensorI2cHandle = NULL;
    static uint8_t sensorI2cByteOrder = 255U;
    getIssSensorI2cInfo(&sensorI2cByteOrder, &sensorI2cHandle);
    if(NULL == sensorI2cHandle)
    {
        printf("Sensor I2C Handle is NULL \n");
        return -1;
    }
    status = Board_i2c16BitRegRd(sensorI2cHandle, i2cAddr, regAddr, regVal, numRegs, sensorI2cByteOrder, SENSOR_I2C_TIMEOUT);
    if(0 != status)
    {
        issLogPrintf("Error : I2C Timeout while reading from IMX390 register 0x%x \n", regAddr);
    }
    return (status);
}

int32_t IMX390_WriteReg(uint8_t    i2cInstId,
                             uint8_t       i2cAddr,
                             uint16_t         regAddr,
                             uint8_t          regVal,
                             uint32_t      numRegs)
{
    int32_t  status = -1;
    I2C_Handle sensorI2cHandle = NULL;
    static uint8_t sensorI2cByteOrder = 255U;
    getIssSensorI2cInfo(&sensorI2cByteOrder, &sensorI2cHandle);
    if(NULL == sensorI2cHandle)
    {
        printf("Sensor I2C Handle is NULL \n");
        return -1;
    }
    status = Board_i2c16BitRegWr(sensorI2cHandle, i2cAddr, regAddr, &regVal, numRegs, sensorI2cByteOrder, SENSOR_I2C_TIMEOUT);
    if(0 != status)
    {
        printf("Error : I2C Timeout while writing 0x%x to IMX390 register 0x%x \n", regVal, regAddr);
    }

    return (status);
}

int32_t IMX390_GetExpPrgFxn(uint32_t chId, void *pSensorHdl, IssAeDynamicParams *p_ae_dynPrms)
{
    int32_t  status = -1;
    uint8_t count = 0;

    p_ae_dynPrms->targetBrightnessRange.min = 40;
    p_ae_dynPrms->targetBrightnessRange.max = 50;
    p_ae_dynPrms->targetBrightness = 45;
    p_ae_dynPrms->threshold = 1;
    p_ae_dynPrms->enableBlc = 1;
    p_ae_dynPrms->exposureTimeStepSize = 1;

    p_ae_dynPrms->exposureTimeRange[count].min = 11000;
    p_ae_dynPrms->exposureTimeRange[count].max = 11000;
    p_ae_dynPrms->analogGainRange[count].min = 1024;
    p_ae_dynPrms->analogGainRange[count].max = 8192;
    p_ae_dynPrms->digitalGainRange[count].min = 256;
    p_ae_dynPrms->digitalGainRange[count].max = 256;
    count++;

    p_ae_dynPrms->numAeDynParams = count;
    return (status);
}

int32_t IMX390_GetWBPrgFxn(uint32_t chId, void *pSensorHdl, IssAwbDynamicParams *p_awb_dynPrms)
{
    int32_t  status = 0;

    p_awb_dynPrms->redGainRange.min = 512;
    p_awb_dynPrms->redGainRange.max = 2048;

    p_awb_dynPrms->greenGainRange.min = 512;
    p_awb_dynPrms->greenGainRange.max = 2048;

    p_awb_dynPrms->blueGainRange.min = 512;
    p_awb_dynPrms->blueGainRange.max = 2048;

    p_awb_dynPrms->sensor_pre_gain = 0;

    printf("IMX390_GetWBPrgFxn: sensor_pre_gain = %d \n", p_awb_dynPrms->sensor_pre_gain);
    return (status);
}

int32_t IMX390_SetAwbParams(void *pSensorHdl, uint32_t chId, IssSensor_WhiteBalanceParams *pWbPrms)
{
    int32_t status = 0;
    uint16_t regAddr;
    uint16_t regValue;
    IssSensors_Handle * pSenHandle = (IssSensors_Handle*)pSensorHdl;
    IssSensor_CreateParams * pCreatePrms;

    assert(NULL != pSenHandle);
    pCreatePrms = pSenHandle->createPrms;
    assert(NULL != pCreatePrms);
    assert(NULL != pWbPrms);

    if(redGain_prev[chId] != pWbPrms->rGain[0])
    {
        redGain_prev[chId] = pWbPrms->rGain[0];
        regAddr = IMX390_RED_GAIN_REG_L;
        regValue = (pWbPrms->rGain[0]>>IMX390_ISP_GAIN_OFFSET) & 0xff;/*Sensor gain is Q8, ISP gain is Q10*/
        status |= IMX390_WriteReg(pCreatePrms->i2cInstId, pCreatePrms->i2cAddrSensor[chId], regAddr, regValue, 1u);
        regAddr = IMX390_RED_GAIN_REG_H;
        regValue = (pWbPrms->rGain[0]>>IMX390_ISP_GAIN_OFFSET) >> 8;/*Sensor gain is Q8, ISP gain is Q10*/
        status |= IMX390_WriteReg(pCreatePrms->i2cInstId, pCreatePrms->i2cAddrSensor[chId], regAddr, regValue, 1u);
    }

    if(greenGain_prev[chId] != pWbPrms->gGain[0])
    {
        greenGain_prev[chId] = pWbPrms->gGain[0];
        regAddr = IMX390_GREEN1_GAIN_REG_L;
        regValue = (pWbPrms->gGain[0]>>IMX390_ISP_GAIN_OFFSET) & 0xff;/*Sensor gain is Q8, ISP gain is Q10*/
        status |= IMX390_WriteReg(pCreatePrms->i2cInstId, pCreatePrms->i2cAddrSensor[chId], regAddr, regValue, 1u);
        regAddr = IMX390_GREEN2_GAIN_REG_L;
        status |= IMX390_WriteReg(pCreatePrms->i2cInstId, pCreatePrms->i2cAddrSensor[chId], regAddr, regValue, 1u);

        regAddr = IMX390_GREEN1_GAIN_REG_H;
        regValue = (pWbPrms->gGain[0]>>IMX390_ISP_GAIN_OFFSET) >> 8;/*Sensor gain is Q8, ISP gain is Q10*/
        status |= IMX390_WriteReg(pCreatePrms->i2cInstId, pCreatePrms->i2cAddrSensor[chId], regAddr, regValue, 1u);
        regAddr = IMX390_GREEN2_GAIN_REG_H;
        status |= IMX390_WriteReg(pCreatePrms->i2cInstId, pCreatePrms->i2cAddrSensor[chId], regAddr, regValue, 1u);
    }

    if(blueGain_prev[chId] != pWbPrms->bGain[0])
    {
        blueGain_prev[chId] = pWbPrms->bGain[0];
        regAddr = IMX390_BLUE_GAIN_REG_L;
        regValue = (pWbPrms->bGain[0]>>IMX390_ISP_GAIN_OFFSET) & 0xff ;/*Sensor gain is Q8, ISP gain is Q10*/
        status |= IMX390_WriteReg(pCreatePrms->i2cInstId, pCreatePrms->i2cAddrSensor[chId], regAddr, regValue, 1u);
        regAddr = IMX390_BLUE_GAIN_REG_H;
        regValue = (pWbPrms->bGain[0]>>IMX390_ISP_GAIN_OFFSET) >> 8;/*Sensor gain is Q8, ISP gain is Q10*/
        status |= IMX390_WriteReg(pCreatePrms->i2cInstId, pCreatePrms->i2cAddrSensor[chId], regAddr, regValue, 1u);
    }

    return (status);
}
