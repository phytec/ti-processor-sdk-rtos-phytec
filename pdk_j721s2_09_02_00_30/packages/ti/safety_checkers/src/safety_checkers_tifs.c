/*
 *  Copyright (C) 2024 Texas Instruments Incorporated
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
 *  \file     safety_checkers_tifs.c
 *
 *  \brief    This file contains TIFS safety checker library functions
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdint.h>
#include <cslr.h>
#include <sciclient.h>
#include <safety_checkers_common.h>
#include <safety_checkers_tifs.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                  Internal/Private Function Declarations                    */
/* ========================================================================== */

static uint32_t SafetyCheckers_tifsGetFwlRegValue(uint32_t fwlId, uint32_t fwlRegion, uint32_t fwlReg);

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

int32_t SafetyCheckers_tifsReqFwlOpen(void)
{
    int32_t status = SAFETY_CHECKERS_SOK;

    struct tisci_fwl_req request = {0};
    struct tisci_fwl_resp response = {0};

    Sciclient_ReqPrm_t  reqParam = {0};
    Sciclient_RespPrm_t respParam = {0};

    reqParam.messageType    = (uint16_t) SAFETY_CHECKERS_TIFS_FWL_OPEN;
    reqParam.flags          = (uint32_t) TISCI_MSG_FLAG_AOP;
    reqParam.pReqPayload    = (const uint8_t *) &request;
    reqParam.reqPayloadSize = (uint32_t) sizeof (request);
    reqParam.timeout        = (uint32_t) SAFETY_CHECKERS_DEFAULT_TIMEOUT;

    respParam.flags           = (uint32_t) 0;   /* Populated by the API */
    respParam.pRespPayload    = (uint8_t *) &response;
    respParam.respPayloadSize = (uint32_t)  sizeof (response);

    status = Sciclient_service(&reqParam, &respParam);
    if ((status == SAFETY_CHECKERS_SOK)  && (respParam.flags == TISCI_MSG_FLAG_ACK))
    {
        status = SAFETY_CHECKERS_SOK;
    }
    else
    {
        status = SAFETY_CHECKERS_FAIL;
    }
    return status;
}

int32_t SafetyCheckers_tifsGetFwlCfg(SafetyCheckers_TifsFwlConfig *fwlConfig, uint32_t size)
{
    uint32_t i = 0U, j = 0U, id = 0U;
    int32_t status = SAFETY_CHECKERS_SOK;

    for (i = 0U; i<size; i++)
    {
        id = fwlConfig[i].fwlId;
        if (fwlConfig[i].numRegions <= fwlConfig[i].maxNumRegions)
        {
            for (j = 0U; j<fwlConfig[i].numRegions; j++)
            {
                /* Read control register from firewall registers */
                fwlConfig[i].fwlCfgPerRegion[j].controlReg = SafetyCheckers_tifsGetFwlRegValue(id, j, SAFETY_CHECKERS_TIFS_CONTROL_REG);

                /* Read permission registers from firewall registers */
                fwlConfig[i].fwlCfgPerRegion[j].privId0 = SafetyCheckers_tifsGetFwlRegValue(id, j, SAFETY_CHECKERS_TIFS_PRIV_ID0);
                fwlConfig[i].fwlCfgPerRegion[j].privId1 = SafetyCheckers_tifsGetFwlRegValue(id, j, SAFETY_CHECKERS_TIFS_PRIV_ID1);
                fwlConfig[i].fwlCfgPerRegion[j].privId2 = SafetyCheckers_tifsGetFwlRegValue(id, j, SAFETY_CHECKERS_TIFS_PRIV_ID2);

                /* Read start address from firewall registers */
                fwlConfig[i].fwlCfgPerRegion[j].startAddrLow =  SafetyCheckers_tifsGetFwlRegValue(id, j, SAFETY_CHECKERS_TIFS_START_ADDRL);
                fwlConfig[i].fwlCfgPerRegion[j].startAddrHigh = SafetyCheckers_tifsGetFwlRegValue(id, j, SAFETY_CHECKERS_TIFS_START_ADDRH);

                /* Read end address from firewall register */
                fwlConfig[i].fwlCfgPerRegion[j].endAddrLow =  SafetyCheckers_tifsGetFwlRegValue(id, j, SAFETY_CHECKERS_TIFS_END_ADDRL);
                fwlConfig[i].fwlCfgPerRegion[j].endAddrHigh = SafetyCheckers_tifsGetFwlRegValue(id, j, SAFETY_CHECKERS_TIFS_END_ADDRH);
            }
        }
        else
        {
            status = SAFETY_CHECKERS_FAIL;
        }
    }
    return status;
}

int32_t SafetyCheckers_tifsVerifyFwlCfg(const SafetyCheckers_TifsFwlConfig *fwlConfig, uint32_t size)
{
    uint32_t i = 0U, j = 0U, regData = 0U, id = 0U, mismatch = 0U;
    int32_t status = SAFETY_CHECKERS_SOK;

    for (i = 0U; i<size; i++)
    {
        id = fwlConfig[i].fwlId;
        if (fwlConfig[i].numRegions <= fwlConfig[i].maxNumRegions)
        {
            for (j = 0U; j<fwlConfig[i].numRegions; j++)
            {
                mismatch = 0U;

                /* Read control register and check for mismatch */
                regData = SafetyCheckers_tifsGetFwlRegValue(id, j, SAFETY_CHECKERS_TIFS_CONTROL_REG);
                mismatch |= fwlConfig[i].fwlCfgPerRegion[j].controlReg ^ regData;

                /* Read permission registers and check for mismatch */
                regData = SafetyCheckers_tifsGetFwlRegValue(id, j, SAFETY_CHECKERS_TIFS_PRIV_ID0);
                mismatch |= fwlConfig[i].fwlCfgPerRegion[j].privId0 ^ regData;

                regData = SafetyCheckers_tifsGetFwlRegValue(id, j, SAFETY_CHECKERS_TIFS_PRIV_ID1);
                mismatch |= fwlConfig[i].fwlCfgPerRegion[j].privId1 ^ regData;

                regData = SafetyCheckers_tifsGetFwlRegValue(id, j, SAFETY_CHECKERS_TIFS_PRIV_ID2);
                mismatch |= fwlConfig[i].fwlCfgPerRegion[j].privId2 ^ regData;

                /* Read start address and check for mismatch */
                regData = SafetyCheckers_tifsGetFwlRegValue(id, j, SAFETY_CHECKERS_TIFS_START_ADDRL);
                mismatch |= fwlConfig[i].fwlCfgPerRegion[j].startAddrLow ^ regData;

                regData = SafetyCheckers_tifsGetFwlRegValue(id, j, SAFETY_CHECKERS_TIFS_START_ADDRH);
                mismatch |= fwlConfig[i].fwlCfgPerRegion[j].startAddrHigh ^ regData;

                /* Read end address and check for mismatch */
                regData = SafetyCheckers_tifsGetFwlRegValue(id, j, SAFETY_CHECKERS_TIFS_END_ADDRL);
                mismatch |= fwlConfig[i].fwlCfgPerRegion[j].endAddrLow ^ regData;

                regData = SafetyCheckers_tifsGetFwlRegValue(id, j, SAFETY_CHECKERS_TIFS_END_ADDRH);
                mismatch |= fwlConfig[i].fwlCfgPerRegion[j].endAddrHigh ^ regData;

                if(mismatch != 0U)
                {
                    status = SAFETY_CHECKERS_REG_DATA_MISMATCH;
                }
            }
        }
        else
        {
            status = SAFETY_CHECKERS_FAIL;
        }
    }
    return status;
}

int32_t SafetyCheckers_tifsReqFwlClose(void)
{
    int32_t status = SAFETY_CHECKERS_SOK;

    struct tisci_fwl_req request;
    struct tisci_fwl_resp response;

    Sciclient_ReqPrm_t  reqParam = {0};
    Sciclient_RespPrm_t respParam = {0};

    reqParam.messageType    = (uint16_t) SAFETY_CHECKERS_TIFS_FWL_CLOSE;
    reqParam.flags          = (uint32_t) TISCI_MSG_FLAG_AOP;
    reqParam.pReqPayload    = (const uint8_t *) &request;
    reqParam.reqPayloadSize = (uint32_t) sizeof (request);
    reqParam.timeout        = (uint32_t) SAFETY_CHECKERS_DEFAULT_TIMEOUT;

    respParam.flags           = (uint32_t) 0;   /* Populated by the API */
    respParam.pRespPayload    = (uint8_t *) &response;
    respParam.respPayloadSize = (uint32_t)  sizeof (response);

   status = Sciclient_service(&reqParam, &respParam);
   if((status == SAFETY_CHECKERS_SOK) && (respParam.flags == TISCI_MSG_FLAG_ACK))
   {
        status = SAFETY_CHECKERS_SOK;
   }
   else
   {
        status = SAFETY_CHECKERS_FAIL;
   }
   return status;
}

/* ========================================================================== */
/*                   Internal/Private Function Definitions                    */
/* ========================================================================== */

static uint32_t SafetyCheckers_tifsGetFwlRegValue(uint32_t fwlId, uint32_t fwlRegion, uint32_t fwlReg)
{
    uint32_t fwlBlkBaseAddr = 0U, fwlBlkOffset = 0U, fwlRegionOffset = 0U, fwlRegValue = 0U;
    fwlBlkBaseAddr = SAFETY_CHECKERS_TIFS_FWL_BASE;
    fwlBlkOffset = fwlBlkBaseAddr + (0x400U * fwlId);
    fwlRegionOffset = fwlBlkOffset + (0x20U * fwlRegion);
    fwlRegValue = CSL_REG32_RD(fwlRegionOffset + fwlReg);
    return fwlRegValue;
}
