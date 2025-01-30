/*
 * SDL MPU Cfg
 *
 * Software Diagnostics Library module for MPU Configuration
 *
 *  Copyright (c) Texas Instruments Incorporated 2018-2021
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

#include <sdl_mpuCfg.h>

/* Local functions */
static void SDL_mpuCfg_local(const SDL_ArmR5MpuRegionCfg *pCfg);

static const uint32_t gMemAttr[SDL_ARM_R5_MEM_ATTR_MAX][3U] =
{
/*    TEX[2:0], C,     B bits */
    {   0x0U,   0x0U,  0x0U,}, /* Strongly-ordered.*/
    {   0x0U,   0x0U,  0x1U,}, /* Shareable Device.*/
    {   0x0U,   0x1U,  0x0U,}, /* Outer and Inner write-through, no write-allocate. */
    {   0x0U,   0x1U,  0x1U,}, /* Outer and Inner write-back, no write-allocate. */
    {   0x1U,   0x0U,  0x0U,}, /* Outer and Inner Non-cacheable. */
    {   0x1U,   0x1U,  0x1U,}, /* Outer and Inner write-back, write-allocate.*/
    {   0x2U,   0x0U,  0x0U,}, /* Non-shareable Device.*/
};

static  const SDL_ArmR5MpuRegionCfg   gCfg =
    {
        /* Region X configuration for 32 byte sizes */
        .regionId         = 0U,
        .enable           = 1U,
        .baseAddr         = 0x0U,
        .size             = SDL_ARM_R5_MPU_REGION_SIZE_32B,
        .subRegionEnable  = SDL_ARM_R5_MPU_SUB_REGION_ENABLE_ALL,
        .exeNeverControl  = 1U,
        .accessPermission = SDL_ARM_R5_ACC_PERM_PRIV_USR_RD_WR,
        .shareable        = 0U,
        .cacheable        = 0U,
        .cachePolicy      = 0U,
        .memAttr          = 0U,
    };


void SDL_MPUResolve(uint32_t baseAddr, uint32_t regId)
{
    SDL_ArmR5MpuRegionCfg   cfg = gCfg;

    /* Update the base addr and region ID configured */
    cfg.baseAddr            = baseAddr;
    cfg.regionId            = regId;
    cfg.accessPermission    = SDL_ARM_R5_ACC_PERM_PRIV_USR_RD_WR;

    SDL_mpuCfg_local(&cfg);
    return;
}

void SDL_MPUCfgAddRegion(const SDL_MPU_memConfig_t *pMemConfig)
{
    SDL_ArmR5MpuRegionCfg   cfg = gCfg;
    /* Update the base addr and region ID configured */
    cfg.baseAddr = (uint32_t) (uintptr_t) pMemConfig->pMemLocation;
    cfg.regionId = pMemConfig->mpuRegionId;

    /* Update the access permission for the configured */
    if (pMemConfig->memAccessType == SDL_MPU_DATA_WRITE_ACCESS) {
         cfg.accessPermission = SDL_ARM_R5_ACC_PERM_PRIV_USR_RD;
    }
    else {
         cfg.accessPermission = SDL_ARM_R5_ACC_PERM_NO_ACCESS;
    }

    SDL_mpuCfg_local(&cfg);
    return;
}

/* This is an internal implementation for MPU configuration for a given Region */
static void SDL_mpuCfg_local(const SDL_ArmR5MpuRegionCfg *pCfg)
{
    uint32_t baseAddrRegVal, sizeRegVal, accessCtrlRegVal, tex;

    /* Create the region base Address register value */
    baseAddrRegVal   = ( pCfg->baseAddr &
                                    SDL_ARM_R5_MPU_REGION_BASE_ADDR_MASK);

    /* Create Region Size and Enable register value */
    sizeRegVal       = ( pCfg->enable <<
                                    SDL_ARM_R5_MPU_REGION_SZEN_EN_SHIFT);
    sizeRegVal       |= ( pCfg->size <<
                                    SDL_ARM_R5_MPU_REGION_SZEN_SZ_SHIFT);
    sizeRegVal       |= ( pCfg->subRegionEnable <<
                                    SDL_ARM_R5_MPU_REGION_SZEN_SRD_SHIFT);
    /* Create the access control register value */
    accessCtrlRegVal  = ( pCfg->exeNeverControl <<
                                   SDL_ARM_R5_MPU_REGION_AC_XN_SHIFT);
    accessCtrlRegVal |= ( pCfg->accessPermission <<
                                   SDL_ARM_R5_MPU_REGION_AC_AP_SHIFT);
    accessCtrlRegVal |= ( pCfg->shareable <<
                                   SDL_ARM_R5_MPU_REGION_AC_S_SHIFT);

    /* Below code is creating the accessCtrlReg value for cachable == 0u */
    tex = gMemAttr[pCfg->memAttr][0U];
    accessCtrlRegVal |=  ( tex << SDL_ARM_R5_MPU_REGION_AC_TEX_SHIFT);
    accessCtrlRegVal |=
                         ( gMemAttr[pCfg->memAttr][1U] <<
                           SDL_ARM_R5_MPU_REGION_AC_B_SHIFT);
    accessCtrlRegVal |=
                         ( gMemAttr[pCfg->memAttr][2U] <<
                           SDL_ARM_R5_MPU_REGION_AC_C_SHIFT);

    /* configure MPU region here */
    SDL_R5MPU_Cfg(pCfg->regionId,
                          baseAddrRegVal,
                          sizeRegVal,
                          accessCtrlRegVal);
    return;
}

/* Internal implementation to remove the MPU configuration */
void SDL_MPUCfgRemoveRegion(const SDL_MPU_memConfig_t *pMemConfig)
{
    uint32_t baseAddrRegVal = 0U, sizeRegVal = 0U, accessCtrlRegVal = 0U;

    /* Remove the MPU configured for that Region */
    SDL_R5MPU_Cfg(pMemConfig->mpuRegionId,
                                  baseAddrRegVal,
                                  sizeRegVal,
                                  accessCtrlRegVal);
    return;
}
