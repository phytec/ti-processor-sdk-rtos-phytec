/**
 * @file  startup.c
 *
 * @brief
 *  ARM R5 start-up code
 *
 *  \par
 *  ============================================================================
 *  @n   (C) Copyright 2017-2020, Texas Instruments, Inc.
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
#include "startup.h"
#include "sdlr.h"
#include "sdl_types.h"

#define SDL_MCU_ARMSS_STARTUP_VIM_NULL_ADDR           ((void *) 0 )

#define SDL_FMK(PER_REG_FIELD, val)                                         \
    (((val) << SDL_##PER_REG_FIELD##_SHIFT) & SDL_##PER_REG_FIELD##_MASK)

/*  Declarations    */
/**
 * \brief  TEX[2:0], C and B values.
 *         SDL_ArmR5MemAttr is used as intex here.
 *         gMemAttr[x][0]: TEX[2:0] values
 *         gMemAttr[x][1]: C bit value
 *         gMemAttr[x][2]: B bit value
 */
static const uint32_t gMemAttr[SDL_ARM_R5_MEM_ATTR_MAX][3U] __attribute__((section(".startupData"))) =
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

const SDL_ArmR5MpuRegionCfg __attribute__((section(".startupData")))  __attribute__((weak)) gSdlR5MpuCfg[SDL_ARM_R5F_MPU_REGIONS_MAX] =
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
        /* Region 2 configuration: 1MB KB MCU MSRAM */
        .regionId         = 2U,
        .enable           = 1U,
        .baseAddr         = 0x41C00000,
#if defined (SOC_J721E) || defined (SOC_J7200) || defined (SOC_J721S2) || defined (SOC_J784S4)
        .size             = SDL_ARM_R5_MPU_REGION_SIZE_1MB,
#else
        .size             = SDL_ARM_R5_MPU_REGION_SIZE_512KB,
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
        /* Region 7 configuration: 32 KB ATCM */
        .regionId         = 6U,
        .enable           = 1U,
        .baseAddr         = 0x0,
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
        /**
        * Region 8 configuration: 4 KB  MCRC_MCU_NAVSS
        * To perform mcrc 64-bit atomic write be need to make
        * the MCRC_MCU_NAVSS region non-cacheable 
        */
        .regionId         = 7U,
        .enable           = 1U,
        .baseAddr         = 0x2A264000,
        .size             = SDL_ARM_R5_MPU_REGION_SIZE_4KB,
        .subRegionEnable  = SDL_ARM_R5_MPU_SUB_REGION_ENABLE_ALL,
        .exeNeverControl  = 0U,
        .accessPermission = SDL_ARM_R5_ACC_PERM_PRIV_USR_RD_WR,
        .shareable        = 0U,
        .cacheable        = (uint32_t)FALSE,
        .cachePolicy      = SDL_ARM_R5_CACHE_POLICY_NON_CACHEABLE,
        .memAttr          = SDL_ARM_R5_MEM_ATTR_STRONGLY_NON_CACHED,
    },

    {
        /**
        * Region 8 configuration: 4 KB  NAVSS0_MCRC_0
        * To perform mcrc 64-bit atomic write be need to make
        * the NAVSS0_MCRC_0 region non-cacheable 
        */
        .regionId         = 8U,
        .enable           = 1U,
        .baseAddr         = 0x31f70000,
        .size             = SDL_ARM_R5_MPU_REGION_SIZE_4KB,
        .subRegionEnable  = SDL_ARM_R5_MPU_SUB_REGION_ENABLE_ALL,
        .exeNeverControl  = 0U,
        .accessPermission = SDL_ARM_R5_ACC_PERM_PRIV_USR_RD_WR,
        .shareable        = 0U,
        .cacheable        = (uint32_t)FALSE,
        .cachePolicy      = SDL_ARM_R5_CACHE_POLICY_NON_CACHEABLE,
        .memAttr          = SDL_ARM_R5_MEM_ATTR_STRONGLY_NON_CACHED,
    },
};

__attribute__((section(".startupCode"))) void __mpu_init(void);
__attribute__((section(".startupCode"))) void SDL_armR5MPUCfg(void);

__attribute__((section(".startupCode"))) static void _enable_mpu()
{
    SDL_armR5MPUCfg();                      /* Custom MPU configuration */
}

__attribute__((section(".startupCode"))) static void _enable_cache()
{
    SDL_armR5StartupCacheEnableAllCache( 0 );      /* Disable I/D caches */
    SDL_armR5StartupCacheEnableForceWrThru( 0 );   /* Disable force write-thru */
    SDL_armR5StartupCacheInvalidateAllCache();     /* Invalidate I/D caches */
    SDL_armR5StartupCacheEnableAllCache( 1 );      /* Enable I/D caches */
}

__attribute__((section(".startupCode"))) void SDL_armR5MPUCfg(void)
{
    uint32_t loopCnt = 0U;
    uint32_t baseAddrRegVal = 0U, sizeRegVal = 0U, accessCtrlRegVal = 0U, tex;

    SDL_armR5StartupCacheInvalidateAllCache();     /* Invalidate caches */
    SDL_armR5StartupCacheEnableDCache(0);          /* Disable D-cache */

    /* Disable MPU */
    SDL_armR5StartupMpuEnable(0U);
    /* Disable all MPU regions */
    for (loopCnt = 0U ;
         loopCnt < SDL_ARM_R5F_MPU_REGIONS_MAX ;
         loopCnt++)
    {
        SDL_armR5StartupMpuCfgRegion(loopCnt,
                                     baseAddrRegVal,
                                     sizeRegVal,
                                     accessCtrlRegVal);
    }

    /* Configure MPU regions only for provided configuration */
    for (loopCnt = 0U ;
         loopCnt < SDL_ARM_R5F_MPU_REGIONS_MAX ;
         loopCnt++)
    {
        if (SDL_ARM_R5_MPU_REGION_SIZE_32B <= gSdlR5MpuCfg[loopCnt].size)
        {
            baseAddrRegVal = 0U;
            sizeRegVal = 0U;
            accessCtrlRegVal = 0U;
            baseAddrRegVal   |= ( gSdlR5MpuCfg[loopCnt].baseAddr &
                                    SDL_ARM_R5_MPU_REGION_BASE_ADDR_MASK);
            sizeRegVal       |= ( gSdlR5MpuCfg[loopCnt].enable <<
                                    SDL_ARM_R5_MPU_REGION_SZEN_EN_SHIFT);
            sizeRegVal       |= ( gSdlR5MpuCfg[loopCnt].size <<
                                    SDL_ARM_R5_MPU_REGION_SZEN_SZ_SHIFT);
            sizeRegVal       |= ( gSdlR5MpuCfg[loopCnt].subRegionEnable <<
                                    SDL_ARM_R5_MPU_REGION_SZEN_SRD_SHIFT);
            accessCtrlRegVal |= ( gSdlR5MpuCfg[loopCnt].exeNeverControl <<
                                   SDL_ARM_R5_MPU_REGION_AC_XN_SHIFT);
            accessCtrlRegVal |= ( gSdlR5MpuCfg[loopCnt].accessPermission <<
                                   SDL_ARM_R5_MPU_REGION_AC_AP_SHIFT);
            accessCtrlRegVal |= ( gSdlR5MpuCfg[loopCnt].shareable <<
                                   SDL_ARM_R5_MPU_REGION_AC_S_SHIFT);
            if (gSdlR5MpuCfg[loopCnt].cacheable == (uint32_t)TRUE)
            {
                tex = (1U << 2U);
                tex |= (gSdlR5MpuCfg[loopCnt].cachePolicy);
                accessCtrlRegVal |=
                                ( tex << SDL_ARM_R5_MPU_REGION_AC_TEX_SHIFT);
                accessCtrlRegVal |= ( gSdlR5MpuCfg[loopCnt].cachePolicy <<
                                   SDL_ARM_R5_MPU_REGION_AC_CB_SHIFT);
            }
            else
            {
                tex = gMemAttr[gSdlR5MpuCfg[loopCnt].memAttr][0U];
                accessCtrlRegVal |=
                                ( tex << SDL_ARM_R5_MPU_REGION_AC_TEX_SHIFT);
                accessCtrlRegVal |=
                                ( gMemAttr[gSdlR5MpuCfg[loopCnt].memAttr][1U] <<
                                SDL_ARM_R5_MPU_REGION_AC_B_SHIFT);
                accessCtrlRegVal |=
                                ( gMemAttr[gSdlR5MpuCfg[loopCnt].memAttr][2U] <<
                                SDL_ARM_R5_MPU_REGION_AC_C_SHIFT);
            }
            /* configure MPU region here */
            SDL_armR5StartupMpuCfgRegion(gSdlR5MpuCfg[loopCnt].regionId,
                                         baseAddrRegVal,
                                         sizeRegVal,
                                         accessCtrlRegVal);
        }
    }

    SDL_armR5StartupCacheInvalidateAllCache();     /* Invalidate caches */
    SDL_armR5StartupCacheEnableDCache(0);          /* Disable D-cache */

    /* Enable MPU */
    SDL_armR5StartupMpuEnable(1U);
}

/*****************************************************************************/
/* \brief __MPU_INIT() - __mpu_init() is called in the C/C++ startup routine,*/
/* _c_int00(), and provides a mechanism for tailoring mpu init by device     */
/* prior to calling main().                                                  */
/*                                                                           */
/*****************************************************************************/
/*  Usage notes:
    On entry to this function from boot, R5F must be in System (privileged) mode. */
__attribute__((section(".startupCode")))  __attribute__((weak)) void __mpu_init(void)
{
    uint32_t loopCnt = 0U, regAddr;
    SDL_ArmR5CPUInfo info;
    uint32_t    maxIntrs;
    SDL_vimRegs     *pRegs;
    SDL_armR5StartupGetCpuID(&info);
#if defined(SDL_MAIN_DOMAIN_VIM_BASE_ADDR0) && defined(SDL_MAIN_DOMAIN_VIM_BASE_ADDR1)
    if (info.grpId == (uint32_t)SDL_ARM_R5_CLUSTER_GROUP_ID_0)
    {
        /* MCU SS Pulsar R5 SS */
        regAddr = (info.cpuID == SDL_ARM_R5_CPU_ID_0)?
                                 SDL_MCU_DOMAIN_VIM_BASE_ADDR0:
                                 SDL_MCU_DOMAIN_VIM_BASE_ADDR1;

    }
    else
    {
        /* MAIN SS Pulsar R5 SS */
        regAddr = (info.cpuID == SDL_ARM_R5_CPU_ID_0)?
                                 SDL_MAIN_DOMAIN_VIM_BASE_ADDR0:
                                 SDL_MAIN_DOMAIN_VIM_BASE_ADDR1;

    }
#else
    /* MCU SS Pulsar R5 SS */
    regAddr = (info.cpuID == SDL_ARM_R5_CPU_ID_0)?
                             SDL_MCU_DOMAIN_VIM_BASE_ADDR0:
                             SDL_MCU_DOMAIN_VIM_BASE_ADDR1;
#endif
    pRegs       = (SDL_vimRegs *)(uintptr_t) regAddr;
    maxIntrs    = pRegs->INFO;

#if defined (SOC_J721E)
    /* Limit the outstanding transactions to 2
     * only for AM65xx and J721e platforms
     * Later SoCs do not have this issue
     */
    SDL_armR5SetDLFOBit();
#else
    /* Fix for PRSDK-8161
     * For AM64x and J7200, there is no issue of limitting outstanding
     * transactions. R5F core can support full 7 outstanding transactions
     */
#endif
    _enable_mpu();              /* Enable MPU */
    _enable_cache();            /* Enable all caches */
    SDL_armR5StartupFpuEnable( 1 );    /* Enable FPU */
    SDL_armR5StartupIntrEnableVic(1);  /* Enable VIC */

    /* Disable/Clear pending Interrupts in VIM before enabling CPU Interrupts */
    /* This is done to prevent serving any bogus interrupt */
    for (loopCnt = 0U ; loopCnt < maxIntrs; loopCnt++)
    {
        /* Disable interrupt in vim */
        SDL_startupVimSetIntrEnable((SDL_vimRegs *)(uintptr_t)regAddr,
                                    loopCnt,
                                    false);
        /* Clear interrupt status */
        SDL_startupVimClrIntrPending((SDL_vimRegs *)(uintptr_t)regAddr,
                                     loopCnt);
    }
    SDL_armR5StartupIntrEnableFiq(1);  /* Enable FIQ */
    SDL_armR5StartupIntrEnableIrq(1);  /* Enable IRQ */
}

__attribute__((section(".startupCode"))) void SDL_armR5StartupGetCpuID( SDL_ArmR5CPUInfo *cpuInfo )
{
    uint32_t regVal;

    regVal = SDL_armR5StartupReadMpidrReg();
    cpuInfo->cpuID = (uint32_t)((regVal & SDL_R5_MPIDR_AFF0_MASK) >>
                                          SDL_R5_MPIDR_AFF0_SHIFT);
    cpuInfo->grpId = (uint32_t)((regVal & SDL_R5_MPIDR_AFF1_MASK) >>
                                          SDL_R5_MPIDR_AFF1_SHIFT);
    cpuInfo->multiprocessingExt = (uint32_t)(
                                (regVal & SDL_R5_MPIDR_MULEXT_MASK) >>
                                          SDL_R5_MPIDR_MULEXT_SHIFT);
}

int32_t SDL_startupVimSetIntrEnable( SDL_vimRegs *pRegs, uint32_t intrNum, bool bEnable )
{
    int32_t  retVal = SDL_PASS;
    uint32_t bitNum, groupNum, mask;
    void       *pChkRegs = (void *) pRegs;
    uint32_t    maxIntrs, num_groups;

    maxIntrs   = pRegs->INFO;
    num_groups = maxIntrs / SDL_VIM_NUM_INTRS_PER_GROUP;

    if (pChkRegs == SDL_MCU_ARMSS_STARTUP_VIM_NULL_ADDR)
    {
        /* No actions - API fails functionality */
        retVal = SDL_EFAIL;
    }

    groupNum = intrNum / SDL_VIM_NUM_INTRS_PER_GROUP;
    if( ( groupNum < num_groups ) &&
        ( retVal   == SDL_PASS) )
    {
        bitNum = intrNum & (SDL_VIM_NUM_INTRS_PER_GROUP-1U);
        mask   = (((uint32_t)(1U)) << bitNum);
        if( bEnable == (bool) true)
        {
            SDL_REG32_WR(&pRegs->GRP[groupNum].INTR_EN_SET, mask );
        }
        else
        {
            SDL_REG32_WR( &pRegs->GRP[groupNum].INTR_EN_CLR, mask );
        }
    }
    else
    {
        retVal = SDL_EFAIL;
    }
    return retVal;
}

__attribute__((section(".startupCode"))) int32_t SDL_startupVimClrIntrPending( SDL_vimRegs *pRegs, uint32_t intrNum )
{
    int32_t  retVal = SDL_PASS;
    uint32_t bitNum, groupNum, mask;
    void       *pChkRegs = (void *) pRegs;
    uint32_t    maxIntrs, num_groups;

    maxIntrs   = pRegs->INFO;
    num_groups = maxIntrs / SDL_VIM_NUM_INTRS_PER_GROUP;

    groupNum = intrNum / SDL_VIM_NUM_INTRS_PER_GROUP;
    bitNum   = intrNum & (SDL_VIM_NUM_INTRS_PER_GROUP-1U);
    mask   = (((uint32_t)(1U)) << bitNum);

    if ( (pChkRegs != SDL_MCU_ARMSS_STARTUP_VIM_NULL_ADDR) &&
         (groupNum < num_groups) )
    {
        SDL_REG32_WR( &pRegs->GRP[groupNum].STS, mask );
    }
    else
    {
        retVal = SDL_EFAIL;
    }
    return retVal;
}

