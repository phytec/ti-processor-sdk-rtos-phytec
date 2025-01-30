#if (defined (FREERTOS))
#include <ti/csl/arch/c7x/Mmu.h>
#else
#include <stdbool.h>
#include <ti/sysbios/family/c7x/Mmu.h>
#endif

#include "memmap.h"

void tidlMmuMap(bool is_secure)
{
    bool retVal;
    Mmu_MapAttrs    attrs;

    uint32_t ns = 1;

    if(is_secure)
        ns = 0;
    else
        ns = 1;

    Mmu_initMapAttrs(&attrs);

    attrs.attrIndx = Mmu_AttrIndx_MAIR0;
    attrs.ns = ns;

    retVal = Mmu_map(0x00000000, 0x00000000, 0x20000000, &attrs, is_secure);
    if(retVal==false)
    {
        goto mmu_exit;
    }

    retVal = Mmu_map(0x20000000, 0x20000000, 0x20000000, &attrs, is_secure);
    if(retVal==false)
    {
        goto mmu_exit;
    }

    retVal = Mmu_map(0x40000000, 0x40000000, 0x20000000, &attrs, is_secure);
    if(retVal==false)
    {
        goto mmu_exit;
    }

    retVal = Mmu_map(0x60000000, 0x60000000, 0x10000000, &attrs, is_secure);
    if(retVal==false)
    {
        goto mmu_exit;
    }

    retVal = Mmu_map(0x70000000, 0x70000000, 0x10000000, &attrs, is_secure);
    if(retVal==false)
    {
        goto mmu_exit;
    }

    Mmu_initMapAttrs(&attrs);

    attrs.attrIndx = Mmu_AttrIndx_MAIR4;
    attrs.ns = ns;

    retVal = Mmu_map(DDR_C7x_1_IPC_ADDR, DDR_C7x_1_IPC_ADDR, DDR_C7x_1_IPC_SIZE, &attrs, is_secure); /* ddr            */
    if(retVal == false)
    {
        goto mmu_exit;
    }

    Mmu_initMapAttrs(&attrs);
    attrs.attrIndx = Mmu_AttrIndx_MAIR7;
    attrs.ns = ns;
    retVal = Mmu_map(0x70000000, 0x70000000, 0x00800000, &attrs, is_secure); /* msmc  448KB      */
    if(retVal == false)
    {
        goto mmu_exit;
    }

    Mmu_initMapAttrs(&attrs);
    attrs.attrIndx = Mmu_AttrIndx_MAIR7;
    attrs.ns = ns;
    retVal = Mmu_map(0x64800000, 0x64800000, 0x00200000, &attrs, is_secure); /* L2 sram 448KB        */
    if(retVal == false)
    {
        goto mmu_exit;
    }


    Mmu_initMapAttrs(&attrs);
    attrs.attrIndx = Mmu_AttrIndx_MAIR7;
    attrs.ns = ns;

    retVal = Mmu_map(DDR_C7x_1_DTS_ADDR, DDR_C7x_1_DTS_ADDR, DDR_C7x_1_DTS_SIZE, &attrs, is_secure); /* ddr            */
    if(retVal == false)
    {
        goto mmu_exit;
    }

    retVal = Mmu_map(DDR_SHARED_MEM_ADDR, DDR_SHARED_MEM_ADDR, DDR_SHARED_MEM_SIZE, &attrs, is_secure); /* ddr            */
    if(retVal == false)
    {
        goto mmu_exit;
    }

    retVal = Mmu_map(DDR_C7X_1_SCRATCH_ADDR, DDR_C7X_1_SCRATCH_ADDR, DDR_C7X_1_SCRATCH_SIZE, &attrs, is_secure); /* ddr            */
    if(retVal == false)
    {
        goto mmu_exit;
    }

mmu_exit:
    return;
}

