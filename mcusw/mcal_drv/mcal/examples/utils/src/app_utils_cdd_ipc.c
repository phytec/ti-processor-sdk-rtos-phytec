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
 *  \file     app_utils_cdd_ipc
 *
 *  \brief    Initializes sections with know patterns, used to check for stack
 *              and memory corruption if any
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "Std_Types.h"
#include "app_utils.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
/* None */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

#if !defined (SOC_AM62AX) && ! defined (SOC_AM62X) && ! defined (SOC_AM62PX) && ! defined (SOC_J722S)
/* Indexed list of dst ids */
static const uint16 map_dst_id[] =
{
    TISCI_DEV_COMPUTE_CLUSTER0_GIC500SS,
    TISCI_DEV_MCU_R5FSS0_CORE0,
    TISCI_DEV_MCU_R5FSS0_CORE1,
    TISCI_DEV_R5FSS0_CORE0,
    TISCI_DEV_R5FSS0_CORE1,
#if defined (SOC_J721E)
    TISCI_DEV_R5FSS1_CORE0,
    TISCI_DEV_R5FSS1_CORE1,
    TISCI_DEV_C66SS0_CORE0,
    TISCI_DEV_C66SS1_CORE0,
    TISCI_DEV_COMPUTE_CLUSTER0_CLEC
#endif
#if defined (SOC_J721S2)
    TISCI_DEV_R5FSS1_CORE0,
    TISCI_DEV_R5FSS1_CORE1,
    TISCI_DEV_COMPUTE_CLUSTER0_CLEC,
    TISCI_DEV_COMPUTE_CLUSTER0_CLEC
#endif
#if defined (SOC_J784S4)
    TISCI_DEV_R5FSS1_CORE0,
    TISCI_DEV_R5FSS1_CORE1,
    TISCI_DEV_R5FSS2_CORE0,
    TISCI_DEV_R5FSS2_CORE1,
    TISCI_DEV_COMPUTE_CLUSTER0_CLEC,
    TISCI_DEV_COMPUTE_CLUSTER0_CLEC,
    TISCI_DEV_COMPUTE_CLUSTER0_CLEC,
    TISCI_DEV_COMPUTE_CLUSTER0_CLEC
#endif
};

/* Indexed list of req type */
static const uint16 req_type[] =
{
    /* NOTE: This list should match the Core index */
#if defined (SOC_J721E) || defined (SOC_J7200)
    TISCI_DEV_NAVSS0_INTR_ROUTER_0,
    TISCI_DEV_NAVSS0_INTR_ROUTER_0,
    TISCI_DEV_NAVSS0_INTR_ROUTER_0,
    TISCI_DEV_NAVSS0_INTR_ROUTER_0,
    TISCI_DEV_NAVSS0_INTR_ROUTER_0,
#endif /* defined (SOC_J7200) */
#if defined (SOC_J721E)
    TISCI_DEV_NAVSS0_INTR_ROUTER_0,
    TISCI_DEV_NAVSS0_INTR_ROUTER_0,
    TISCI_DEV_C66SS0_INTROUTER0,
    TISCI_DEV_C66SS1_INTROUTER0,
    TISCI_DEV_NAVSS0_INTR_ROUTER_0
#endif /* defined (SOC_J721E) */
#if defined (SOC_J721S2) || defined (SOC_J784S4)
    TISCI_DEV_NAVSS0_INTR_0,
    TISCI_DEV_NAVSS0_INTR_0,
    TISCI_DEV_NAVSS0_INTR_0,
    TISCI_DEV_NAVSS0_INTR_0,
    TISCI_DEV_NAVSS0_INTR_0,
    TISCI_DEV_NAVSS0_INTR_0,
    TISCI_DEV_NAVSS0_INTR_0,
    TISCI_DEV_NAVSS0_INTR_0,
    TISCI_DEV_NAVSS0_INTR_0,
#endif /* defined (SOC_J721S2) */
#if defined (SOC_J784S4)
    TISCI_DEV_NAVSS0_INTR_0,
    TISCI_DEV_NAVSS0_INTR_0,
    TISCI_DEV_NAVSS0_INTR_0,
    TISCI_DEV_NAVSS0_INTR_0,
    TISCI_DEV_NAVSS0_INTR_0
#endif /* defined (SOC_J784S4) */
};


/* Indexed list of req subtype */
static const uint16 req_subtype[] =
{
    /* NOTE: This list should match the Core index */
    TISCI_RESASG_SUBTYPE_IR_OUTPUT,
    TISCI_RESASG_SUBTYPE_IR_OUTPUT,
    TISCI_RESASG_SUBTYPE_IR_OUTPUT,
    TISCI_RESASG_SUBTYPE_IR_OUTPUT,
    TISCI_RESASG_SUBTYPE_IR_OUTPUT,
#if defined (SOC_J721E) || defined (SOC_J721S2) || defined (SOC_J784S4)
    TISCI_RESASG_SUBTYPE_IR_OUTPUT,
    TISCI_RESASG_SUBTYPE_IR_OUTPUT,
    TISCI_RESASG_SUBTYPE_IR_OUTPUT,
    TISCI_RESASG_SUBTYPE_IR_OUTPUT,
#endif
#if defined (SOC_J721E) || defined (SOC_J784S4)
    TISCI_RESASG_SUBTYPE_IR_OUTPUT,
#endif
#if defined (SOC_J784S4)
    TISCI_RESASG_SUBTYPE_IR_OUTPUT,
    TISCI_RESASG_SUBTYPE_IR_OUTPUT,
    TISCI_RESASG_SUBTYPE_IR_OUTPUT,
    TISCI_RESASG_SUBTYPE_IR_OUTPUT
#endif
};
#endif
/* ========================================================================== */
/*                  Internal Function Declarations                            */
/* ========================================================================== */
/* None */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
/**< Sections defined in linker command file for ADC module */
extern uint32 __linker_cdd_ipc_text_start, __linker_cdd_ipc_text_end,
              __linker_cdd_ipc_const_start, __linker_cdd_ipc_const_end,
              __linker_cdd_ipc_init_start, __linker_cdd_ipc_init_end,
              __linker_cdd_ipc_no_init_start, __linker_cdd_ipc_no_init_end,
              __linker_cdd_ipc_no_init_align_8b_start,
              __linker_cdd_ipc_no_init_align_8b_end;
/**< Address list of sections defined in linker command file for ADC module */
static uint32 CddIpcApp_SecHoleAddrList[] =
{
    ((uint32) & __linker_cdd_ipc_text_start),
    ((uint32) & __linker_cdd_ipc_text_end) - APP_UTILS_LINKER_FILL_LENGTH,
    ((uint32) & __linker_cdd_ipc_const_start),
    ((uint32) & __linker_cdd_ipc_const_end) - APP_UTILS_LINKER_FILL_LENGTH,
    ((uint32) & __linker_cdd_ipc_init_start),
    ((uint32) & __linker_cdd_ipc_init_end) - APP_UTILS_LINKER_FILL_LENGTH,
    ((uint32) & __linker_cdd_ipc_no_init_start),
    ((uint32) & __linker_cdd_ipc_no_init_end) - APP_UTILS_LINKER_FILL_LENGTH,
    ((uint32) & __linker_cdd_ipc_no_init_align_8b_start),
    ((uint32) & __linker_cdd_ipc_no_init_align_8b_end) -
                                                APP_UTILS_LINKER_FILL_LENGTH,
};

/* ========================================================================== */
/*                          Function Implementation                           */
/* ========================================================================== */
/** \brief Utility used to initialize memory sections.
 *   This API is used to initialize fixed size(APP_UTILS_LINKER_FILL_LENGTH)
 *   before start of each section and before end of each section. i.e
 *      .=align(4);
 *       __linker_cdd_ipc_text_start = .;
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      *(ADC_TEXT_SECTION)
 *      .=align(4);
 *      . += APP_UTILS_LINKER_FILL_LENGTH; (This portion is initialized)
 *      __linker_cdd_ipc_text_end = .;
 *
 *   This API should be called during module start up code.
 */
void AppUtils_CddIpcSectionInit(void)
{
    /* Initialize memory sections  */
    AppUtils_SectionInit(&CddIpcApp_SecHoleAddrList[0U],
        MODULEAPP_NUM_SEC_HOLES(CddIpcApp_SecHoleAddrList));
}

/** \brief Utility used to check any memory corruption occurred in
 *   stack and memory sections initialized by AppUtils_AdcSectionInit.
 *   In this API, both stack and memory sections initialized by
 *   AppUtils_AdcSectionInit are checked.
 *
 *   This API expects AppUtils_CddIpcSectionInit should be called in start up
 *   code before calling this.
 */
uint32 AppUtils_CddIpcCheckStackAndSectionCorruption(void)
{
    uint32 retVal = E_OK;
    if (AppUtils_CheckStackAndSectionCorruption(
            &CddIpcApp_SecHoleAddrList[0U],
                MODULEAPP_NUM_SEC_HOLES(CddIpcApp_SecHoleAddrList)) != E_OK)
    {
        retVal = E_NOT_OK;
    }

    return retVal;
}
#if !defined (SOC_AM62AX) && ! defined (SOC_AM62X) && ! defined (SOC_AM62PX) && ! defined (SOC_J722S)
/** \brief Utility used to call the Internal API to 
 *   translate an IR output to the destination processor
 *   input IRQ value.
 */
int32_t Ipc_sciclientIrqTranslate(uint16 coreId, uint32 eventId,
        uint16 *procIrq)
{
    return Sciclient_rmIrqTranslateIrOutput(req_type[coreId],
                                            (uint16)eventId,
                                            map_dst_id[coreId],
                                            procIrq);
}
/** \brief Utility used to get the Interrupt range.
 */
int32_t Ipc_getIntNumRange(uint32 coreIndex,
        uint16 *rangeStartP, uint16 *rangeNumP)
{
    int32_t                                     retVal = E_OK;
    struct tisci_msg_rm_get_resource_range_resp res = {0};
    struct tisci_msg_rm_get_resource_range_req  req = {0};

    req.type           = req_type[coreIndex];
    req.subtype        = (uint8)req_subtype[coreIndex];
    req.secondary_host = (uint8)TISCI_MSG_VALUE_RM_UNUSED_SECONDARY_HOST;

    res.range_num = 0;
    res.range_start = 0;

    /* Get interrupt number range */
    retVal =  Sciclient_rmGetResourceRange(
                &req,
                &res,
                APP_SCICLIENT_TIMEOUT);
    if (E_OK != retVal || res.range_num == 0) {
        /* Try with HOST_ID_ALL */
        req.type           = req_type[coreIndex];
        req.subtype        = (uint8)req_subtype[coreIndex];
        req.secondary_host = TISCI_HOST_ID_ALL;

        retVal = Sciclient_rmGetResourceRange(
                &req,
                &res,
                APP_SCICLIENT_TIMEOUT);
    }
    if (E_OK == retVal)
    {
        *rangeStartP = res.range_start;
        *rangeNumP = res.range_num;
    }
    return retVal;
}
#endif