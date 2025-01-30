/*
 * SDL TEST
 *
 * Software Diagnostics Reference Test MPU default settings
 *
 *  Copyright (c) Texas Instruments Incorporated 2018-2020
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

#include <stdlib.h>
#include <stdint.h>
#include <sdl_types.h>
#include <test/r5f_startup/sdl_arm_r5.h>
#include <src/ip/r5/sdl_arm_r5_mpu.h>
#include <test/r5f_startup/interrupt.h>
#include <test/r5f_startup/sdl_cache.h>

/*  Declarations    */
/**
 * \brief  TEX[2:0], C and B values.
 *         gsdlTestMemAttr[x][0]: TEX[2:0] values
 *         gsdlTestMemAttr[x][1]: C bit value
 *         gsdlTestMemAttr[x][2]: B bit value
 */
static const uint32_t gsdlTestMemAttr[SDL_ARM_R5_MEM_ATTR_MAX][3U] =
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

const SDL_ArmR5MpuRegionCfg  gsdlR5MpuCfg[SDL_ARM_R5F_MPU_REGIONS_MAX] =
{
    {
        /* Region 0 configuration: complete 32 bit address space = 4Gbits */
        .regionId         = 0U,
        .enable           = 1U,
        .baseAddr         = 0x0U,
        .size             = SDL_ARM_R5_MPU_REGION_SIZE_4GB,
        .subRegionEnable  = SDL_ARM_R5_MPU_SUB_REGION_ENABLE_ALL,
        .exeNeverControl  = 1U,
        .accessPermission = SDL_ARM_R5_ACC_PERM_PRIV_USR_RD_WR,
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
        .size             = SDL_ARM_R5_MPU_REGION_SIZE_128B,
        .subRegionEnable  = SDL_ARM_R5_MPU_SUB_REGION_ENABLE_ALL,
        .exeNeverControl  = 0U,
        .accessPermission = SDL_ARM_R5_ACC_PERM_PRIV_USR_RD_WR,
        .shareable        = 0U,
        .cacheable        = (uint32_t)TRUE,
        .cachePolicy      = SDL_ARM_R5_CACHE_POLICY_WB_WA,
        .memAttr          = 0U,
    },
    {
        /* Region 2 configuration: 512 KB OCMS RAM */
        .regionId         = 2U,
        .enable           = 1U,
        .baseAddr         = 0x41C00000,
        .size             = SDL_ARM_R5_MPU_REGION_SIZE_512KB,
        .subRegionEnable  = SDL_ARM_R5_MPU_SUB_REGION_ENABLE_ALL,
        .exeNeverControl  = 0U,
        .accessPermission = SDL_ARM_R5_ACC_PERM_PRIV_USR_RD_WR,
        .shareable        = 0U,
        .cacheable        = (uint32_t)TRUE,
        .cachePolicy      = SDL_ARM_R5_CACHE_POLICY_WB_WA,
        .memAttr          = 0U,
    },
    {
        /* Region 3 configuration: 2 MB MCMS3 RAM */
        .regionId         = 3U,
        .enable           = 1U,
        .baseAddr         = 0x70000000,
#if defined (SOC_J721E) || defined (SOC_J784S4)
        .size             = SDL_ARM_R5_MPU_REGION_SIZE_8MB,
#elif defined (SOC_J7200)
        .size             = SDL_ARM_R5_MPU_REGION_SIZE_1MB,
#elif defined (SOC_J721S2)
        .size             = SDL_ARM_R5_MPU_REGION_SIZE_4MB,	
#else
        .size             = SDL_ARM_R5_MPU_REGION_SIZE_2MB,
#endif
        .subRegionEnable  = SDL_ARM_R5_MPU_SUB_REGION_ENABLE_ALL,
        .exeNeverControl  = 0U,
        .accessPermission = SDL_ARM_R5_ACC_PERM_PRIV_USR_RD_WR,
        .shareable        = 0U,
        .cacheable        = (uint32_t)TRUE,
        .cachePolicy      = SDL_ARM_R5_CACHE_POLICY_WB_WA,
        .memAttr          = 0U,
    },
    {
        /* Region 4 configuration: 2 GB DDR RAM */
        .regionId         = 4U,
        .enable           = 1U,
        .baseAddr         = 0x80000000,
        .size             = SDL_ARM_R5_MPU_REGION_SIZE_2GB,
        .subRegionEnable  = SDL_ARM_R5_MPU_SUB_REGION_ENABLE_ALL,
        .exeNeverControl  = 0U,
        .accessPermission = SDL_ARM_R5_ACC_PERM_PRIV_USR_RD_WR,
        .shareable        = 0U,
        .cacheable        = (uint32_t)TRUE,
        .cachePolicy      = SDL_ARM_R5_CACHE_POLICY_WB_WA,
        .memAttr          = 0U,
    },
    {
        /* Region 5 configuration: 32 KB BTCM */
        /* Address of ATCM/BTCM are configured via MCU_SEC_MMR registers
           It can either be '0x0' or '0x41010000'. Application/Boot-loader shall
           take care this configurations and linker command file shall be
           in sync with this. For either of the above configurations,
           MPU configurations will not changes as both regions will have same
           set of permissions in almost all scenarios.
           Application can chose to overwrite this MPU configuration if needed.
           The same is true for the region corresponding to ATCM. */
        .regionId         = 5U,
        .enable           = 1U,
        .baseAddr         = 0x41010000,
        .size             = SDL_ARM_R5_MPU_REGION_SIZE_32KB,
        .subRegionEnable  = SDL_ARM_R5_MPU_SUB_REGION_ENABLE_ALL,
        .exeNeverControl  = 0U,
        .accessPermission = SDL_ARM_R5_ACC_PERM_PRIV_USR_RD_WR,
        .shareable        = 0U,
        .cacheable        = (uint32_t)TRUE,
        .cachePolicy      = SDL_ARM_R5_CACHE_POLICY_NON_CACHEABLE,
        .memAttr          = 0U,
    },
    {
        /* Region 6 configuration: 128 MB FSS DAT */
        .regionId         = 6U,
        .enable           = 0U,
        .baseAddr         = 0x50000000,
        .size             = SDL_ARM_R5_MPU_REGION_SIZE_128MB,
        .subRegionEnable  = SDL_ARM_R5_MPU_SUB_REGION_ENABLE_ALL,
        .exeNeverControl  = 0U,
        .accessPermission = SDL_ARM_R5_ACC_PERM_PRIV_USR_RD_WR,
        .shareable        = 0U,
        .cacheable        = (uint32_t)TRUE,
        .cachePolicy      = SDL_ARM_R5_CACHE_POLICY_WB_WA,
        .memAttr          = 0U,
    },
    {
        /* Region 7 configuration: Test  memory area */
        .regionId         = 7U,
        .enable           = 1U,
        .baseAddr         = 0x7C00U,
        .size             = SDL_ARM_R5_MPU_REGION_SIZE_128B,
        .subRegionEnable  = SDL_ARM_R5_MPU_SUB_REGION_ENABLE_ALL,
        .exeNeverControl  = 0U,
        .accessPermission = SDL_ARM_R5_ACC_PERM_PRIV_USR_RD_WR,
        .shareable        = 0U,
        .cacheable        = (uint32_t)TRUE,
        .cachePolicy      = SDL_ARM_R5_CACHE_POLICY_WB_WA,
        .memAttr          = 0U,
    },
};

void SDL_armR5MPUCfg_test_def(void);

static void SDL_enable_cache()
{
    SDL_armR5CacheEnableAllCache( 0 );      /* Disable I/D caches */
    SDL_armR5CacheEnableForceWrThru( 1 );   /* Enable force write-thru */
    SDL_armR5CacheInvalidateAllCache();     /* Invalidate I/D caches */
    SDL_armR5CacheEnableAllCache( 1 );      /* Enable I/D caches */
}

/*****************************************************************************/
/* \brief __MPU_INIT() - __mpu_init() is called in the C/C++ startup routine,*/
/* _c_int00(), and provides a mechanism for tailoring mpu init by device     */
/* prior to calling main().                                                  */
/*                                                                           */
/*****************************************************************************/
/*  Usage notes:
    On entry to this function from boot, R5F must be in System (privileged) mode. */
void __mpu_init(void)
{
    uint32_t loopCnt = 0U, regAddr;
    SDL_ArmR5CPUInfo info;

    SDL_armR5GetCpuID(&info);
    if (info.grpId == (uint32_t)SDL_ARM_R5_CLUSTER_GROUP_ID_0)
    {
        /* MCU SS Pulsar R5 SS */
        regAddr = SDL_MCU_DOMAIN_VIM_BASE_ADDR;
    }
    else
    {
        /* MAIN SS Pulsar R5 SS */
        regAddr = SDL_MAIN_DOMAIN_VIM_BASE_ADDR;
    }

    SDL_armR5MPUCfg_test_def();          /* Enable MPU */
#ifndef SDTF_CACHE_DISABLE
    SDL_enable_cache();         /* Enable all caches */
#endif
    SDL_armR5FpuEnable( 1 );    /* Enable FPU */
    SDL_armR5IntrEnableVic(1);  /* Enable VIC */
    /* Disable/Clear pending Interrupts in VIM before enabling CPU Interrupts */
    /* This is done to prevent serving any bogus interrupt */
    for (loopCnt = 0U ; loopCnt < R5_VIM_INTR_NUM; loopCnt++)
    {
        /* Disable interrupt in vim */
        SDL_VIM_setIntrEnable((SDL_vimRegs *)(uintptr_t)regAddr, loopCnt, false);
        /* Clear interrupt status */
        SDL_VIM_clrIntrPending((SDL_vimRegs *)(uintptr_t)regAddr, loopCnt);
    }
    SDL_armR5IntrEnableFiq(1);  /* Enable FIQ */
    SDL_armR5IntrEnableIrq(1);  /* Enable IRQ */
}

void SDL_armR5MPUCfg_test_def(void)
{
    uint32_t loopCnt = 0U;
    uint32_t baseAddrRegVal = 0U, sizeRegVal = 0U, accessCtrlRegVal = 0U, tex;

    SDL_armR5CacheInvalidateAllCache();     /* Invalidate caches */
    SDL_armR5CacheEnableDCache(0);          /* Disable D-cache */
    /* Disable MPU */
    SDL_R5MPU_Enable(0U);
    /* Disable all MPU regions */
    for (loopCnt = 0U ; loopCnt < SDL_ARM_R5F_MPU_REGIONS_MAX ; loopCnt++)
    {
        SDL_R5MPU_Cfg(loopCnt,
                              baseAddrRegVal,
                              sizeRegVal,
                              accessCtrlRegVal);
    }

    /* Configure MPU regions only for provided configuration */
    for (loopCnt = 0U ; loopCnt < SDL_ARM_R5F_MPU_REGIONS_MAX ; loopCnt++)
    {
        if (SDL_ARM_R5_MPU_REGION_SIZE_32B <= gsdlR5MpuCfg[loopCnt].size)
        {
            baseAddrRegVal = 0U;
            sizeRegVal = 0U;
            accessCtrlRegVal = 0U;
            baseAddrRegVal   |= ( gsdlR5MpuCfg[loopCnt].baseAddr &
                                    SDL_ARM_R5_MPU_REGION_BASE_ADDR_MASK);
            sizeRegVal       |= ( gsdlR5MpuCfg[loopCnt].enable <<
                                    SDL_ARM_R5_MPU_REGION_SZEN_EN_SHIFT);
            sizeRegVal       |= ( gsdlR5MpuCfg[loopCnt].size <<
                                    SDL_ARM_R5_MPU_REGION_SZEN_SZ_SHIFT);
            sizeRegVal       |= ( gsdlR5MpuCfg[loopCnt].subRegionEnable <<
                                    SDL_ARM_R5_MPU_REGION_SZEN_SRD_SHIFT);
            accessCtrlRegVal |= ( gsdlR5MpuCfg[loopCnt].exeNeverControl <<
                                   SDL_ARM_R5_MPU_REGION_AC_XN_SHIFT);
            accessCtrlRegVal |= ( gsdlR5MpuCfg[loopCnt].accessPermission <<
                                   SDL_ARM_R5_MPU_REGION_AC_AP_SHIFT);
            accessCtrlRegVal |= ( gsdlR5MpuCfg[loopCnt].shareable <<
                                   SDL_ARM_R5_MPU_REGION_AC_S_SHIFT);
            if (gsdlR5MpuCfg[loopCnt].cacheable == (uint32_t)TRUE)
            {
                tex = (1U << 2U);
                tex |= (gsdlR5MpuCfg[loopCnt].cachePolicy);
                accessCtrlRegVal |=
                                ( tex << SDL_ARM_R5_MPU_REGION_AC_TEX_SHIFT);
                accessCtrlRegVal |= ( gsdlR5MpuCfg[loopCnt].cachePolicy <<
                                   SDL_ARM_R5_MPU_REGION_AC_CB_SHIFT);
            }
            else
            {
                tex = gsdlTestMemAttr[gsdlR5MpuCfg[loopCnt].memAttr][0U];
                accessCtrlRegVal |=
                                ( tex << SDL_ARM_R5_MPU_REGION_AC_TEX_SHIFT);
                accessCtrlRegVal |=
                                ( gsdlTestMemAttr[gsdlR5MpuCfg[loopCnt].memAttr][1U] <<
                                SDL_ARM_R5_MPU_REGION_AC_B_SHIFT);
                accessCtrlRegVal |=
                                ( gsdlTestMemAttr[gsdlR5MpuCfg[loopCnt].memAttr][2U] <<
                                SDL_ARM_R5_MPU_REGION_AC_C_SHIFT);
            }
            /* configure MPU region here */
            SDL_R5MPU_Cfg(gsdlR5MpuCfg[loopCnt].regionId,
                                  baseAddrRegVal,
                                  sizeRegVal,
                                  accessCtrlRegVal);
        }
    }

    SDL_armR5CacheInvalidateAllCache();     /* Invalidate caches */
    SDL_armR5CacheEnableDCache(0);          /* Disable D-cache */
    /* Enable MPU */
    SDL_R5MPU_Enable(1U);
}
