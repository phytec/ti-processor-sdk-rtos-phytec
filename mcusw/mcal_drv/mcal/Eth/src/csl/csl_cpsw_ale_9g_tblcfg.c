/**
 * @file  csl_cpsw_ale_4g_tblcfg.c
 *
 * @brief
 *  API Function layer file for Ethernet switch module ALE table configuration CSL .
 *
 *  Contains the different functions definitions for configuring ALE
 *  table for CPSW 4 port ALE
 *
 *  \par
 *  ============================================================================
 *  @n   (C) Copyright 2024, Texas Instruments, Inc.
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
 * \brief This is to disable HW_SYNC_BARRIER for J7 due to performance
 *        requirement
 */
#if defined (SOC_J721E) || defined (SOC_J7200)
#define MEM_BARRIER_DISABLE
#endif

#include "cslr_ale.h"
#include "cslr_xge_cpsw.h"
#include "cslr_ale_tblwd_cpsw9g.h"
#include "csl_cpsw.h"

#define DebugP_assert(...)

#define ETH_CSL_CPSW_ALE_MACADDR_GET0(word1)       (CSL_FEXT((word1), \
                                                     ALE_TABLE_WORD1_REG_ENTRY_TYPE_ALLADDR_MACADDR_0))
#define ETH_CSL_CPSW_ALE_MACADDR_GET1(word1)       (CSL_FEXT((word1), \
                                                     ALE_TABLE_WORD1_REG_ENTRY_TYPE_ALLADDR_MACADDR_1))
#define ETH_CSL_CPSW_ALE_MACADDR_GET2(word0)       (CSL_FEXT((word0), \
                                                     ALE_TABLE_WORD0_REG_ENTRY_TYPE_ALLADDR_MACADDR_2))
#define ETH_CSL_CPSW_ALE_MACADDR_GET3(word0)       (CSL_FEXT((word0), \
                                                     ALE_TABLE_WORD0_REG_ENTRY_TYPE_ALLADDR_MACADDR_3))
#define ETH_CSL_CPSW_ALE_MACADDR_GET4(word0)       (CSL_FEXT((word0), \
                                                     ALE_TABLE_WORD0_REG_ENTRY_TYPE_ALLADDR_MACADDR_4))
#define ETH_CSL_CPSW_ALE_MACADDR_GET5(word0)       (CSL_FEXT((word0), \
                                                     ALE_TABLE_WORD0_REG_ENTRY_TYPE_ALLADDR_MACADDR_5))


/********************************************************************************
************************* Ethernet Switch (CPSW) Submodule **********************
********************************************************************************/

static void ETH_CSL_CPSW_initAleTblWds(CSL_AleRegs *hCpswAleRegs)
{
    hCpswAleRegs->ALE_TBLW0 = 0;
    hCpswAleRegs->ALE_TBLW1 = 0;
    hCpswAleRegs->ALE_TBLW2 = 0;
}


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_getALEEntryType
 *
 *   @b Description
 *   @n This function returns the ALE entry type for any given ALE table
 *      entry index.
 *
 * =============================================================================
 */
ETH_CSL_CPSW_ALE_ENTRYTYPE ETH_CSL_CPSW_getALEEntryType(CSL_AleRegs *hCpswAleRegs,
                                                   uint32      index,
                                      ETH_CSL_CPSW_ALETABLE_TYPE aleType)
{
    uint32  aleTblCtrlVal   =   0;
    uint32  aleTblEntryType =   0;

    DebugP_assert(aleType == ETH_CSL_CPSW_ALETABLE_TYPE_9PORT);

    /* Set the index in the ALE table to "read" operation and let
     * the hardware load the ALE table entry corresponding to index
     * specified into the ALE table word registers.
     */
    aleTblCtrlVal   =   CSL_FMK (ALE_ALE_TBLCTL_TABLEIDX, index) |
                        CSL_FMK (ALE_ALE_TBLCTL_TABLEWR, ((uint32)0U));

    hCpswAleRegs->ALE_TBLCTL    =   aleTblCtrlVal;

    aleTblEntryType = CSL_ALE_TABLE_GET_ALL_ENTRYTYPE();
    return (ETH_CSL_CPSW_ALE_ENTRYTYPE) aleTblEntryType;
}


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_getAleMcastAddrEntry
 *
 *   @b Description
 *   @n This function reads the ALE table entry for the index specified and
 *      fills the output parameter structure with Multicast address configuration
 *      read from the hardware.
 *
 * =============================================================================
 */
void ETH_CSL_CPSW_getAleMcastAddrEntry(CSL_AleRegs *hCpswAleRegs,
                                      uint32      index,
                                      ETH_CSL_CPSW_ALE_MCASTADDR_ENTRY* pMcastAddrCfg,
                                      ETH_CSL_CPSW_ALETABLE_TYPE aleType)
{
    uint32                                  aleTblCtrlVal   =   0;

    DebugP_assert(aleType == ETH_CSL_CPSW_ALETABLE_TYPE_9PORT);

    /* Set the index in the ALE table to "read" operation and let
     * the hardware load the ALE table entry corresponding to index
     * specified into the ALE table word registers.
     */
    aleTblCtrlVal           =   CSL_FMK (ALE_ALE_TBLCTL_TABLEIDX, index) |
                                CSL_FMK (ALE_ALE_TBLCTL_TABLEWR, ((uint32)0U));

    hCpswAleRegs->ALE_TBLCTL =   aleTblCtrlVal;

    /* Read the multicast address configuration */
    pMcastAddrCfg->macAddress[0]        =    (uint8)(CSL_ALE_TABLE_GET_MCASTADDR_MACADDR_0());
    pMcastAddrCfg->macAddress[1]        =    (uint8)(CSL_ALE_TABLE_GET_MCASTADDR_MACADDR_1());
    pMcastAddrCfg->macAddress[2]        =    (uint8)(CSL_ALE_TABLE_GET_MCASTADDR_MACADDR_2());
    pMcastAddrCfg->macAddress[3]        =    (uint8)(CSL_ALE_TABLE_GET_MCASTADDR_MACADDR_3());
    pMcastAddrCfg->macAddress[4]        =    (uint8)(CSL_ALE_TABLE_GET_MCASTADDR_MACADDR_4());
    pMcastAddrCfg->macAddress[5]        =    (uint8)(CSL_ALE_TABLE_GET_MCASTADDR_MACADDR_5());
    pMcastAddrCfg->mcastFwdState        =    CSL_ALE_TABLE_GET_MCASTADDR_FWDSTLVL();
    pMcastAddrCfg->superEnable          =    CSL_ALE_TABLE_GET_MCASTADDR_SUPER();
    pMcastAddrCfg->portMask             =    CSL_ALE_TABLE_GET_MCASTADDR_PORTMASK();
    pMcastAddrCfg->ignMBits             =    CSL_ALE_TABLE_GET_MCASTADDR_IGNMBITS();

    return;
}


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_setAleMcastAddrEntry
 *
 *   @b Description
 *   @n This function sets up the ALE table entry for the index specified with
 *      Multicast address configuration specified here.
 *
 * =============================================================================
 */
void ETH_CSL_CPSW_setAleMcastAddrEntry(CSL_AleRegs *hCpswAleRegs,
                                      uint32      index,
                                      ETH_CSL_CPSW_ALE_MCASTADDR_ENTRY* pMcastAddrCfg,
                                      ETH_CSL_CPSW_ALETABLE_TYPE aleType)
{
    uint32                                  aleTblCtrlVal   =   0;

    DebugP_assert(aleType == ETH_CSL_CPSW_ALETABLE_TYPE_9PORT);

    ETH_CSL_CPSW_initAleTblWds(hCpswAleRegs);
    /* Setup the multicast address configuration */
    CSL_ALE_TABLE_SET_MCASTADDR_MACADDR_5(((uint32)pMcastAddrCfg->macAddress[5]));
    CSL_ALE_TABLE_SET_MCASTADDR_MACADDR_4(((uint32)pMcastAddrCfg->macAddress[4]));
    CSL_ALE_TABLE_SET_MCASTADDR_MACADDR_3(((uint32)pMcastAddrCfg->macAddress[3]));
    CSL_ALE_TABLE_SET_MCASTADDR_MACADDR_2(((uint32)pMcastAddrCfg->macAddress[2]));
    CSL_ALE_TABLE_SET_MCASTADDR_MACADDR_1(((uint32)pMcastAddrCfg->macAddress[1]));
    CSL_ALE_TABLE_SET_MCASTADDR_MACADDR_0(((uint32)pMcastAddrCfg->macAddress[0]));
    CSL_ALE_TABLE_SET_MCASTADDR_FWDSTLVL(pMcastAddrCfg->mcastFwdState);
    CSL_ALE_TABLE_SET_MCASTADDR_IGNMBITS(pMcastAddrCfg->ignMBits);
    CSL_ALE_TABLE_SET_MCASTADDR_SUPER(pMcastAddrCfg->superEnable);
    CSL_ALE_TABLE_SET_MCASTADDR_PORTMASK(pMcastAddrCfg->portMask);
    /* set entry type to address entry */
    CSL_ALE_TABLE_SET_ALL_ENTRYTYPE(((uint32)ETH_CSL_ALE_ENTRYTYPE_ADDRESS));

    /* Set the index in the ALE table to "write" operation and let
     * the hardware set up the ALE table entry corresponding to index
     * specified.
     */
    aleTblCtrlVal           =   CSL_FMK (ALE_ALE_TBLCTL_TABLEIDX, index) |
                                CSL_FMK (ALE_ALE_TBLCTL_TABLEWR, (uint32) 1);

    hCpswAleRegs->ALE_TBLCTL    =   aleTblCtrlVal;

    return;
}


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_getAleVlanMcastAddrEntry
 *
 *   @b Description
 *   @n This function reads the ALE table entry for the index specified and
 *      fills the output parameter structure with VLAN Multicast address configuration
 *      read from the hardware.
 *
 * =============================================================================
 */
void ETH_CSL_CPSW_getAleVlanMcastAddrEntry(CSL_AleRegs *hCpswAleRegs,
                                          uint32      index,
                                          ETH_CSL_CPSW_ALE_VLANMCASTADDR_ENTRY*   pVlanMcastAddrCfg,
                                          ETH_CSL_CPSW_ALETABLE_TYPE aleType)
{
    uint32                                  aleTblCtrlVal   =   0;

    DebugP_assert(aleType == ETH_CSL_CPSW_ALETABLE_TYPE_9PORT);

    /* Set the index in the ALE table to "read" operation and let
     * the hardware load the ALE table entry corresponding to index
     * specified into the ALE table word registers.
     */
    aleTblCtrlVal           =   CSL_FMK (ALE_ALE_TBLCTL_TABLEIDX, index) |
                                CSL_FMK (ALE_ALE_TBLCTL_TABLEWR, ((uint32)0U));

    hCpswAleRegs->ALE_TBLCTL =   aleTblCtrlVal;

    /* Read the multicast address configuration */
    pVlanMcastAddrCfg->macAddress[0]        =   (uint8)(CSL_ALE_TABLE_GET_MCASTADDRVLAN_MACADDR_0());
    pVlanMcastAddrCfg->macAddress[1]        =   (uint8)(CSL_ALE_TABLE_GET_MCASTADDRVLAN_MACADDR_1());
    pVlanMcastAddrCfg->macAddress[2]        =   (uint8)(CSL_ALE_TABLE_GET_MCASTADDRVLAN_MACADDR_2());
    pVlanMcastAddrCfg->macAddress[3]        =   (uint8)(CSL_ALE_TABLE_GET_MCASTADDRVLAN_MACADDR_3());
    pVlanMcastAddrCfg->macAddress[4]        =   (uint8)(CSL_ALE_TABLE_GET_MCASTADDRVLAN_MACADDR_4());
    pVlanMcastAddrCfg->macAddress[5]        =   (uint8)(CSL_ALE_TABLE_GET_MCASTADDRVLAN_MACADDR_5());
    pVlanMcastAddrCfg->vlanId               =   CSL_ALE_TABLE_GET_MCASTADDRVLAN_VLANID();
    pVlanMcastAddrCfg->mcastFwdState        =   CSL_ALE_TABLE_GET_MCASTADDRVLAN_FWDSTLVL();
    pVlanMcastAddrCfg->superEnable          =   CSL_ALE_TABLE_GET_MCASTADDRVLAN_SUPER();
    pVlanMcastAddrCfg->portMask             =   CSL_ALE_TABLE_GET_MCASTADDRVLAN_PORTMASK();
    pVlanMcastAddrCfg->ignMBits             =   CSL_ALE_TABLE_GET_MCASTADDRVLAN_IGNMBITS();


    return;
}


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_setAleVlanMcastAddrEntry
 *
 *   @b Description
 *   @n This function sets up the ALE table entry for the index specified with
 *      VLAN Multicast address configuration specified here.
 *
 * =============================================================================
 */
void ETH_CSL_CPSW_setAleVlanMcastAddrEntry(CSL_AleRegs *hCpswAleRegs,
                         uint32                              index,
                         ETH_CSL_CPSW_ALE_VLANMCASTADDR_ENTRY*   pVlanMcastAddrCfg,
                         ETH_CSL_CPSW_ALETABLE_TYPE aleType)
{
    uint32                                  aleTblCtrlVal   =   0;

    DebugP_assert(aleType == ETH_CSL_CPSW_ALETABLE_TYPE_9PORT);

    ETH_CSL_CPSW_initAleTblWds(hCpswAleRegs);
    /* Setup the VLAN multicast address configuration */
    CSL_ALE_TABLE_SET_MCASTADDRVLAN_MACADDR_5(pVlanMcastAddrCfg->macAddress[5]);
    CSL_ALE_TABLE_SET_MCASTADDRVLAN_MACADDR_4(pVlanMcastAddrCfg->macAddress[4]);
    CSL_ALE_TABLE_SET_MCASTADDRVLAN_MACADDR_3(pVlanMcastAddrCfg->macAddress[3]);
    CSL_ALE_TABLE_SET_MCASTADDRVLAN_MACADDR_2(pVlanMcastAddrCfg->macAddress[2]);
    CSL_ALE_TABLE_SET_MCASTADDRVLAN_MACADDR_1(pVlanMcastAddrCfg->macAddress[1]);
    CSL_ALE_TABLE_SET_MCASTADDRVLAN_MACADDR_0(pVlanMcastAddrCfg->macAddress[0]);
    CSL_ALE_TABLE_SET_MCASTADDRVLAN_VLANID(pVlanMcastAddrCfg->vlanId)          ;
    CSL_ALE_TABLE_SET_MCASTADDRVLAN_FWDSTLVL(pVlanMcastAddrCfg->mcastFwdState) ;
    CSL_ALE_TABLE_SET_MCASTADDRVLAN_IGNMBITS(pVlanMcastAddrCfg->ignMBits)      ;
    CSL_ALE_TABLE_SET_MCASTADDRVLAN_SUPER(pVlanMcastAddrCfg->superEnable)      ;
    CSL_ALE_TABLE_SET_MCASTADDRVLAN_PORTMASK(pVlanMcastAddrCfg->portMask)      ;

    /* set entry type to VLAN address entry */
    CSL_ALE_TABLE_SET_ALL_ENTRYTYPE(((uint32)ETH_CSL_ALE_ENTRYTYPE_VLANADDRESS));

    /* Set the index in the ALE table to "write" operation and let
     * the hardware set up the ALE table entry corresponding to index
     * specified.
     */
    aleTblCtrlVal           =   CSL_FMK (ALE_ALE_TBLCTL_TABLEIDX, index) |
                                CSL_FMK (ALE_ALE_TBLCTL_TABLEWR, (uint32) 1);

    hCpswAleRegs->ALE_TBLCTL    =   aleTblCtrlVal;

    return;
}


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_setAleUnicastAddrEntry
 *
 *   @b Description
 *   @n This function sets up the ALE table entry for the index specified with
 *      unicast address configuration specified here.
 *
 * =============================================================================
 */
void ETH_CSL_CPSW_setAleUnicastAddrEntry(CSL_AleRegs *hCpswAleRegs,
                                     uint32      index,
                                     ETH_CSL_CPSW_ALE_UNICASTADDR_ENTRY*  pUcastAddrCfg,
                                     ETH_CSL_CPSW_ALETABLE_TYPE aleType)
{
    uint32                                  aleTblCtrlVal   =   0;

    DebugP_assert(aleType == ETH_CSL_CPSW_ALETABLE_TYPE_9PORT);

    ETH_CSL_CPSW_initAleTblWds(hCpswAleRegs);
    /* Setup the Unicast address configuration */
    CSL_ALE_TABLE_SET_UNIADDR_MACADDR_5(((uint32)pUcastAddrCfg->macAddress[5]));
    CSL_ALE_TABLE_SET_UNIADDR_MACADDR_4(((uint32)pUcastAddrCfg->macAddress[4]));
    CSL_ALE_TABLE_SET_UNIADDR_MACADDR_3(((uint32)pUcastAddrCfg->macAddress[3]));
    CSL_ALE_TABLE_SET_UNIADDR_MACADDR_2(((uint32)pUcastAddrCfg->macAddress[2]));
    CSL_ALE_TABLE_SET_UNIADDR_MACADDR_1(((uint32)pUcastAddrCfg->macAddress[1]));
    CSL_ALE_TABLE_SET_UNIADDR_MACADDR_0(((uint32)pUcastAddrCfg->macAddress[0]));
    CSL_ALE_TABLE_SET_UNIADDR_AGABLE(pUcastAddrCfg->ageable);
    CSL_ALE_TABLE_SET_UNIADDR_SECURE(pUcastAddrCfg->secureEnable);
    CSL_ALE_TABLE_SET_UNIADDR_BLOCK(pUcastAddrCfg->blockEnable);
    CSL_ALE_TABLE_SET_UNIADDR_PORTNUM(pUcastAddrCfg->portNumber);
    CSL_ALE_TABLE_SET_UNIADDR_TRUNK(pUcastAddrCfg->trunkFlag);
    /* set entry type to address entry */
    CSL_ALE_TABLE_SET_ALL_ENTRYTYPE(((uint32)ETH_CSL_ALE_ENTRYTYPE_ADDRESS));

    /* Set the index in the ALE table to "write" operation and let
     * the hardware set up the ALE table entry corresponding to index
     * specified.
     */
    aleTblCtrlVal           =   CSL_FMK (ALE_ALE_TBLCTL_TABLEIDX, index) |
                                CSL_FMK (ALE_ALE_TBLCTL_TABLEWR, (uint32) 1);

    hCpswAleRegs->ALE_TBLCTL    =   aleTblCtrlVal;

    return;
}


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_setAleVlanUnicastAddrEntry
 *
 *   @b Description
 *   @n This function sets up the ALE table entry for the index specified with
 *      VLAN unicast address configuration specified here.
 *
 * =============================================================================
 */
void ETH_CSL_CPSW_setAleVlanUnicastAddrEntry(CSL_AleRegs *hCpswAleRegs,
                         uint32                         index,
                         ETH_CSL_CPSW_ALE_VLANUNICASTADDR_ENTRY* pVlanUcastAddrCfg,
                         ETH_CSL_CPSW_ALETABLE_TYPE aleType)
{
    uint32                                  aleTblCtrlVal   =   0;

    DebugP_assert(aleType == ETH_CSL_CPSW_ALETABLE_TYPE_9PORT);

    ETH_CSL_CPSW_initAleTblWds(hCpswAleRegs);

    /* Setup the VLAN Unicast address configuration */
    CSL_ALE_TABLE_SET_UNIADDRVLAN_MACADDR_5(((uint32)pVlanUcastAddrCfg->macAddress[5]));
    CSL_ALE_TABLE_SET_UNIADDRVLAN_MACADDR_4(((uint32)pVlanUcastAddrCfg->macAddress[4]));
    CSL_ALE_TABLE_SET_UNIADDRVLAN_MACADDR_3(((uint32)pVlanUcastAddrCfg->macAddress[3]));
    CSL_ALE_TABLE_SET_UNIADDRVLAN_MACADDR_2(((uint32)pVlanUcastAddrCfg->macAddress[2]));
    CSL_ALE_TABLE_SET_UNIADDRVLAN_MACADDR_1(((uint32)pVlanUcastAddrCfg->macAddress[1]));
    CSL_ALE_TABLE_SET_UNIADDRVLAN_MACADDR_0(((uint32)pVlanUcastAddrCfg->macAddress[0]));
    CSL_ALE_TABLE_SET_UNIADDRVLAN_VLANID(pVlanUcastAddrCfg->vlanId);
    CSL_ALE_TABLE_SET_UNIADDRVLAN_AGABLE(pVlanUcastAddrCfg->ageable);
    CSL_ALE_TABLE_SET_UNIADDRVLAN_SECURE(pVlanUcastAddrCfg->secureEnable);
    CSL_ALE_TABLE_SET_UNIADDRVLAN_BLOCK(pVlanUcastAddrCfg->blockEnable);
    CSL_ALE_TABLE_SET_UNIADDRVLAN_PORTNUM(pVlanUcastAddrCfg->portNumber);
    CSL_ALE_TABLE_SET_UNIADDRVLAN_TRUNK(pVlanUcastAddrCfg->trunkFlag);
    /* set entry type to address entry */
    CSL_ALE_TABLE_SET_ALL_ENTRYTYPE(((uint32)ETH_CSL_ALE_ENTRYTYPE_VLANADDRESS));

    /* Set the index in the ALE table to "write" operation and let
     * the hardware set up the ALE table entry corresponding to index
     * specified.
     */
    aleTblCtrlVal           =   CSL_FMK (ALE_ALE_TBLCTL_TABLEIDX, index) |
                                CSL_FMK (ALE_ALE_TBLCTL_TABLEWR, (uint32) 1);

    hCpswAleRegs->ALE_TBLCTL    =   aleTblCtrlVal;

    return;
}

/**
@}
*/
