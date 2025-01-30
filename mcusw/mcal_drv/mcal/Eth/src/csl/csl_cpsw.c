/**
 * @file  csl_cpsw.c
 *
 * @brief
 *  API Function layer file for Ethernet switch module CSL.
 *
 *  Contains the different control command and status query functions definations
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

#include "csl_cpsw.h"
#include "cslr_ale.h"
#include "cslr_xge_cpsw.h"


#if defined (SOC_AM273X) || defined (SOC_AWR294X) || defined(SOC_AM263X)
#define CSL_NUM_ALE_VLAN_MASK_MUX1_ENTRIES    (3U)
#else
#define CSL_NUM_ALE_VLAN_MASK_MUX1_ENTRIES    (7U)
#endif

/********************************************************************************
************************* Ethernet Switch (CPSW) Submodule **********************
********************************************************************************/


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_nGF_getCpswVersionInfo
 *
 *   @b Description
 *   @n This function retrieves the CPSW identification and version information.
 *
 *   @b Arguments
     @verbatim
        pVersionInfo        ETH_CSL_CPSW_VERSION structure that needs to be populated
                            with the version info read from the hardware.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Reads
 *   @n XGE_CPSW_CPSW_ID_VER_REG_CPSW_5GF_MINOR_VER,
 *      XGE_CPSW_CPSW_ID_VER_REG_CPSW_5GF_MAJ_VER,
 *      XGE_CPSW_CPSW_ID_VER_REG_CPSW_5GF_RTL_VER,
 *      XGE_CPSW_CPSW_ID_VER_REG_CPSW_5GF_IDENT
 *
 *   @b Example
 *   @verbatim
        ETH_CSL_CPSW_VERSION    versionInfo;

        ETH_CSL_CPSW_getCpswVersionInfo (&versionInfo);

   @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_getCpswVersionInfo (CSL_Xge_cpswRegs *hCpswRegs,
  ETH_CSL_CPSW_VERSION*       pVersionInfo
)
{

    pVersionInfo->minorVer  =   CSL_FEXT (hCpswRegs->ID_VER_REG, XGE_CPSW_ID_VER_REG_MINOR_VER);
    pVersionInfo->majorVer  =   CSL_FEXT (hCpswRegs->ID_VER_REG, XGE_CPSW_ID_VER_REG_MAJOR_VER);
    pVersionInfo->rtlVer    =   CSL_FEXT (hCpswRegs->ID_VER_REG, XGE_CPSW_ID_VER_REG_RTL_VER);
    pVersionInfo->id        =   CSL_FEXT (hCpswRegs->ID_VER_REG, XGE_CPSW_ID_VER_REG_IDENT);

    return;
}

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_enablePort0
 *
 *   @b Description
 *   @n This function configures the CPSW control register to enable the Port 0.
 *
 *   @b Arguments
 *   @n None
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n XGE_CPSW_CONTROL_REG_P0_ENABLE=1
 *
 *   @b Example
 *   @verbatim

        ETH_CSL_CPSW_enablePort0 ();

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_enablePort0 (CSL_Xge_cpswRegs *hCpswRegs)
{
    CSL_FINS (hCpswRegs->CONTROL_REG, XGE_CPSW_CONTROL_REG_P0_ENABLE, 1U);

    return;
}

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_disablePort0
 *
 *   @b Description
 *   @n This function configures the CPSW control register to disable the Port 0.
 *
 *   @b Arguments
 *   @n None
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n XGE_CPSW_CONTROL_REG_P0_ENABLE=0
 *
 *   @b Example
 *   @verbatim

        ETH_CSL_CPSW_disablePort0 ();

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_disablePort0 (CSL_Xge_cpswRegs *hCpswRegs)
{
    CSL_FINS (hCpswRegs->CONTROL_REG, XGE_CPSW_CONTROL_REG_P0_ENABLE, 0U);

    return;
}

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_getCpswControlReg
 *
 *   @b Description
 *   @n This function retrieves the contents of the CPSW Control register.
 *
 *   @b Arguments
     @verbatim
        pControlRegInfo     ETH_CSL_CPSW_CONTROL structure that needs to be populated
                            with the control register contents.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Reads
 *   @n XGE_CPSW_CONTROL_REG_VLAN_AWARE,
 *      XGE_CPSW_CONTROL_REG_P0_ENABLE,
 *      XGE_CPSW_CONTROL_REG_P0_PASS_PRI_TAGGED,
 *      XGE_CPSW_CONTROL_REG_P1_PASS_PRI_TAGGED,
 *      XGE_CPSW_CONTROL_REG_P0_RX_PAD,
 *      XGE_CPSW_CONTROL_REG_P0_RX_PASS_CRC_ERR,
 *      XGE_CPSW_CONTROL_REG_EEE_ENABLE
 *
 *
 *   @b Example
 *   @verbatim
        ETH_CSL_CPSW_CONTROL    controlRegInfo;

        ETH_CSL_CPSW_getCpswControlReg (&controlRegInfo);

   @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_getCpswControlReg (CSL_Xge_cpswRegs *hCpswRegs,
  ETH_CSL_CPSW_CONTROL*   pControlRegInfo
)
{
    pControlRegInfo->fifoLb         =   0;
    pControlRegInfo->vlanAware      =   CSL_FEXT (hCpswRegs->CONTROL_REG, XGE_CPSW_CONTROL_REG_VLAN_AWARE);
    pControlRegInfo->p0Enable       =   CSL_FEXT (hCpswRegs->CONTROL_REG, XGE_CPSW_CONTROL_REG_P0_ENABLE);
    pControlRegInfo->p0PassPriTag   =   CSL_FEXT (hCpswRegs->CONTROL_REG, XGE_CPSW_CONTROL_REG_P0_PASS_PRI_TAGGED);
    pControlRegInfo->p1PassPriTag   =   CSL_FEXT (hCpswRegs->CONTROL_REG, XGE_CPSW_CONTROL_REG_P1_PASS_PRI_TAGGED);
    pControlRegInfo->p2PassPriTag   =   CSL_FEXT (hCpswRegs->CONTROL_REG, XGE_CPSW_CONTROL_REG_P2_PASS_PRI_TAGGED);
    pControlRegInfo->p3PassPriTag   =   CSL_FEXT (hCpswRegs->CONTROL_REG, XGE_CPSW_CONTROL_REG_P3_PASS_PRI_TAGGED);
    pControlRegInfo->p4PassPriTag   =   CSL_FEXT (hCpswRegs->CONTROL_REG, XGE_CPSW_CONTROL_REG_P4_PASS_PRI_TAGGED);
    pControlRegInfo->p5PassPriTag   =   CSL_FEXT (hCpswRegs->CONTROL_REG, XGE_CPSW_CONTROL_REG_P5_PASS_PRI_TAGGED);
    pControlRegInfo->p6PassPriTag   =   CSL_FEXT (hCpswRegs->CONTROL_REG, XGE_CPSW_CONTROL_REG_P6_PASS_PRI_TAGGED);
    pControlRegInfo->p7PassPriTag   =   CSL_FEXT (hCpswRegs->CONTROL_REG, XGE_CPSW_CONTROL_REG_P7_PASS_PRI_TAGGED);
    pControlRegInfo->p8PassPriTag   =   CSL_FEXT (hCpswRegs->CONTROL_REG, XGE_CPSW_CONTROL_REG_P8_PASS_PRI_TAGGED);
    pControlRegInfo->p0TxCrcRemove  =   CSL_FEXT (hCpswRegs->CONTROL_REG, XGE_CPSW_CONTROL_REG_P0_TX_CRC_REMOVE);
    pControlRegInfo->p0RxPad        =   CSL_FEXT (hCpswRegs->CONTROL_REG, XGE_CPSW_CONTROL_REG_P0_RX_PAD);
    pControlRegInfo->p0RxPassCrcErr =   CSL_FEXT (hCpswRegs->CONTROL_REG, XGE_CPSW_CONTROL_REG_P0_RX_PASS_CRC_ERR);
    pControlRegInfo->eeeEnable      =   CSL_FEXT (hCpswRegs->CONTROL_REG, XGE_CPSW_CONTROL_REG_EEE_ENABLE);


    return;
}

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_setCpswControlReg
 *
 *   @b Description
 *   @n This function populates the contents of the CPSW Control register.
 *
 *   @b Arguments
     @verbatim
        pControlRegInfo     ETH_CSL_CPSW_CONTROL structure that holds the values
                            that need to be configured to the CPSW control register.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  CPSW control register modified with values provided.
 *
 *   @b Writes
 *   @n XGE_CPSW_CONTROL_REG_VLAN_AWARE,
 *      XGE_CPSW_CONTROL_REG_P0_ENABLE,
 *      XGE_CPSW_CONTROL_REG_P0_PASS_PRI_TAGGED,
 *      XGE_CPSW_CONTROL_REG_P1_PASS_PRI_TAGGED,
 *      XGE_CPSW_CONTROL_REG_P1_PASS_PRI_TAGGED,
 *      XGE_CPSW_CONTROL_REG_P0_TX_CRC_REMOVE,
 *      XGE_CPSW_CONTROL_REG_P0_RX_PAD,
 *      XGE_CPSW_CONTROL_REG_P0_RX_PASS_CRC_ERR,
 *      XGE_CPSW_CONTROL_REG_EEE_ENABLE
 *
 *   @b Example
 *   @verbatim
        ETH_CSL_CPSW_CONTROL    controlRegInfo;

        controlRegInfo.vlanAware    =   0;
        ...

        ETH_CSL_CPSW_setCpswControlReg (&controlRegInfo);

   @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_setCpswControlReg (CSL_Xge_cpswRegs *hCpswRegs,
  ETH_CSL_CPSW_CONTROL*   pControlRegInfo
)
{
    CSL_FINS (hCpswRegs->CONTROL_REG, XGE_CPSW_CONTROL_REG_VLAN_AWARE, pControlRegInfo->vlanAware);
    CSL_FINS (hCpswRegs->CONTROL_REG, XGE_CPSW_CONTROL_REG_P0_ENABLE, pControlRegInfo->p0Enable);
    CSL_FINS (hCpswRegs->CONTROL_REG, XGE_CPSW_CONTROL_REG_P0_PASS_PRI_TAGGED, pControlRegInfo->p0PassPriTag);
    CSL_FINS (hCpswRegs->CONTROL_REG, XGE_CPSW_CONTROL_REG_P1_PASS_PRI_TAGGED, pControlRegInfo->p1PassPriTag);
    CSL_FINS (hCpswRegs->CONTROL_REG, XGE_CPSW_CONTROL_REG_P2_PASS_PRI_TAGGED, pControlRegInfo->p2PassPriTag);
    CSL_FINS (hCpswRegs->CONTROL_REG, XGE_CPSW_CONTROL_REG_P3_PASS_PRI_TAGGED, pControlRegInfo->p3PassPriTag);
    CSL_FINS (hCpswRegs->CONTROL_REG, XGE_CPSW_CONTROL_REG_P4_PASS_PRI_TAGGED, pControlRegInfo->p4PassPriTag);
    CSL_FINS (hCpswRegs->CONTROL_REG, XGE_CPSW_CONTROL_REG_P5_PASS_PRI_TAGGED, pControlRegInfo->p5PassPriTag);
    CSL_FINS (hCpswRegs->CONTROL_REG, XGE_CPSW_CONTROL_REG_P6_PASS_PRI_TAGGED, pControlRegInfo->p6PassPriTag);
    CSL_FINS (hCpswRegs->CONTROL_REG, XGE_CPSW_CONTROL_REG_P7_PASS_PRI_TAGGED, pControlRegInfo->p7PassPriTag);
    CSL_FINS (hCpswRegs->CONTROL_REG, XGE_CPSW_CONTROL_REG_P8_PASS_PRI_TAGGED, pControlRegInfo->p8PassPriTag);
    CSL_FINS (hCpswRegs->CONTROL_REG, XGE_CPSW_CONTROL_REG_P0_TX_CRC_REMOVE, pControlRegInfo->p0TxCrcRemove);
    CSL_FINS (hCpswRegs->CONTROL_REG, XGE_CPSW_CONTROL_REG_P0_RX_PAD, pControlRegInfo->p0RxPad);
    CSL_FINS (hCpswRegs->CONTROL_REG, XGE_CPSW_CONTROL_REG_P0_RX_PASS_CRC_ERR, pControlRegInfo->p0RxPassCrcErr);
    CSL_FINS (hCpswRegs->CONTROL_REG, XGE_CPSW_CONTROL_REG_EEE_ENABLE, pControlRegInfo->eeeEnable);

    return;
}

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_getPortStatsEnableReg
 *
 *   @b Description
 *   @n This function retrieves the contents of the CPSW Port Statistics
 *      Enable register.
 *
 *   @b Arguments
     @verbatim
        pPortStatsCfg       CSL_XGE_CPSW_PORTSTAT structure that needs to be populated
                            with the port statistics enable register contents.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Reads
 *   @n XGE_CPSW_STAT_PORT_EN_REG_P0_STAT_EN,
 *      XGE_CPSW_STAT_PORT_EN_REG_P1_STAT_EN,
 *
 *   @b Example
 *   @verbatim
        ETH_CSL_CPSW_PORTSTAT       portStatsCfg;

        ETH_CSL_CPSW_getPortStatsEnableReg (&portStatsCfg);

   @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_getPortStatsEnableReg (CSL_Xge_cpswRegs *hCpswRegs,
  ETH_CSL_CPSW_PORTSTAT*  pPortStatsCfg
)
{
    pPortStatsCfg->p0StatEnable    =   CSL_FEXT (hCpswRegs->STAT_PORT_EN_REG, XGE_CPSW_STAT_PORT_EN_REG_P0_STAT_EN);
    pPortStatsCfg->p1StatEnable    =   CSL_FEXT (hCpswRegs->STAT_PORT_EN_REG, XGE_CPSW_STAT_PORT_EN_REG_P1_STAT_EN);
    pPortStatsCfg->p2StatEnable    =   CSL_FEXT (hCpswRegs->STAT_PORT_EN_REG, XGE_CPSW_STAT_PORT_EN_REG_P2_STAT_EN);
    pPortStatsCfg->p3StatEnable    =   CSL_FEXT (hCpswRegs->STAT_PORT_EN_REG, XGE_CPSW_STAT_PORT_EN_REG_P3_STAT_EN);
    pPortStatsCfg->p4StatEnable    =   CSL_FEXT (hCpswRegs->STAT_PORT_EN_REG, XGE_CPSW_STAT_PORT_EN_REG_P4_STAT_EN);
    pPortStatsCfg->p5StatEnable    =   CSL_FEXT (hCpswRegs->STAT_PORT_EN_REG, XGE_CPSW_STAT_PORT_EN_REG_P5_STAT_EN);
    pPortStatsCfg->p6StatEnable    =   CSL_FEXT (hCpswRegs->STAT_PORT_EN_REG, XGE_CPSW_STAT_PORT_EN_REG_P6_STAT_EN);
    pPortStatsCfg->p7StatEnable    =   CSL_FEXT (hCpswRegs->STAT_PORT_EN_REG, XGE_CPSW_STAT_PORT_EN_REG_P7_STAT_EN);
    pPortStatsCfg->p8StatEnable    =   CSL_FEXT (hCpswRegs->STAT_PORT_EN_REG, XGE_CPSW_STAT_PORT_EN_REG_P8_STAT_EN);

    return;
}

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_setPortStatsEnableReg
 *
 *   @b Description
 *   @n This function sets up the contents of the CPSW Port Statistics
 *      Enable register.
 *
 *   @b Arguments
     @verbatim
        pPortStatsCfg       ETH_CSL_CPSW_PORTSTAT structure that contains the values
                            to be used to setup port statistics enable register.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n XGE_CPSW_STAT_PORT_EN_REG_P0_STAT_EN,
 *      XGE_CPSW_STAT_PORT_EN_REG_P1_STAT_EN,
 *
 *   @b Example
 *   @verbatim
        ETH_CSL_CPSW_PORTSTAT       portStatsCfg;

        portStatsCfg.p0StatEnable  =   1;
        portStatsCfg.p1StatEnable  =   1;
        ...

        ETH_CSL_CPSW_setPortStatsEnableReg (&portStatsCfg);

   @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_setPortStatsEnableReg (CSL_Xge_cpswRegs *hCpswRegs,
  ETH_CSL_CPSW_PORTSTAT*  pPortStatsCfg
)
{
    CSL_FINS (hCpswRegs->STAT_PORT_EN_REG, XGE_CPSW_STAT_PORT_EN_REG_P0_STAT_EN, pPortStatsCfg->p0StatEnable);
    CSL_FINS (hCpswRegs->STAT_PORT_EN_REG, XGE_CPSW_STAT_PORT_EN_REG_P1_STAT_EN, pPortStatsCfg->p1StatEnable);
    CSL_FINS (hCpswRegs->STAT_PORT_EN_REG, XGE_CPSW_STAT_PORT_EN_REG_P2_STAT_EN, pPortStatsCfg->p2StatEnable);
    CSL_FINS (hCpswRegs->STAT_PORT_EN_REG, XGE_CPSW_STAT_PORT_EN_REG_P3_STAT_EN, pPortStatsCfg->p3StatEnable);
    CSL_FINS (hCpswRegs->STAT_PORT_EN_REG, XGE_CPSW_STAT_PORT_EN_REG_P4_STAT_EN, pPortStatsCfg->p4StatEnable);
    CSL_FINS (hCpswRegs->STAT_PORT_EN_REG, XGE_CPSW_STAT_PORT_EN_REG_P5_STAT_EN, pPortStatsCfg->p5StatEnable);
    CSL_FINS (hCpswRegs->STAT_PORT_EN_REG, XGE_CPSW_STAT_PORT_EN_REG_P6_STAT_EN, pPortStatsCfg->p6StatEnable);
    CSL_FINS (hCpswRegs->STAT_PORT_EN_REG, XGE_CPSW_STAT_PORT_EN_REG_P7_STAT_EN, pPortStatsCfg->p7StatEnable);
    CSL_FINS (hCpswRegs->STAT_PORT_EN_REG, XGE_CPSW_STAT_PORT_EN_REG_P8_STAT_EN, pPortStatsCfg->p8StatEnable);

    return;
}

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_setPortRxMaxLen
 *
 *   @b Description
 *   @n This function sets up the Port Receive Maximum length register.
 *
 *   @b Arguments
     @verbatim
        portNum             CPSW port number for which the Receive Maximum Length
                            must be retrieved.
        rxMaxLen            Maximum receive frame length to configure.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n XGE_CPSW_P0_RX_MAXLEN_REG_RX_MAXLEN
 *      XGE_CPSW_PN_RX_MAXLEN_REG_RX_MAXLEN
 *
 *   @b Example
 *   @verbatim
 *      uint32      portNum, rxMaxLen;

        portNum     =   1;
        rxMaxLen    =   1518;

        ETH_CSL_CPSW_setPortRxMaxLen (portNum, rxMaxLen);

   @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_setPortRxMaxLen (CSL_Xge_cpswRegs *hCpswRegs,
    uint32                  portNum,
    uint32                  rxMaxLen
)
{
    if(portNum == 0U)
    {
        CSL_FINS (hCpswRegs->P0_RX_MAXLEN_REG, XGE_CPSW_P0_RX_MAXLEN_REG_RX_MAXLEN, rxMaxLen);
    }
    else if (portNum <= 8U)
    {
        CSL_FINS (hCpswRegs->ENETPORT[portNum-1U].PN_RX_MAXLEN_REG, XGE_CPSW_PN_RX_MAXLEN_REG_RX_MAXLEN, rxMaxLen);
    }
    else
    {
        /* Do Nothing */
    }

    return;
}

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_setPortMACAddress
 *
 *   @b Description
 *   @n This function sets up the source MAC address the Tx Pause Frame corresponding to the
 *      CPSW port number specified.
 *
 *   @b Arguments
     @verbatim
        portNum                 CPSW port number for which the source MAC address
                                must be setup. (1-8)
        pMacAddress             6 byte Source MAC address to configure.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  The input parameter 'pMacAddres' is expected to be 6 bytes long.
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n XGE_CPSW_PN_SA_L_REG_MACSRCADDR_7_0,
 *      XGE_CPSW_PN_SA_L_REG_MACSRCADDR_15_8,
 *      XGE_CPSW_PN_SA_H_REG_MACSRCADDR_23_16,
 *      XGE_CPSW_PN_SA_H_REG_MACSRCADDR_31_24,
 *      XGE_CPSW_PN_SA_H_REG_MACSRCADDR_39_32,
 *      XGE_CPSW_PN_SA_H_REG_MACSRCADDR_47_40
 *
 *   @b Example
 *   @verbatim
 *      uint8   macAddress [6], portNum;

        portNum         =   1;
        macAddress [0]  =   0x01;
        macAddress [1]  =   0x02;
        macAddress [2]  =   0x03;
        macAddress [3]  =   0x04;
        macAddress [4]  =   0x05;
        macAddress [5]  =   0x06;

        ETH_CSL_CPSW_setPortMACAddress (portNum, macAddress);

   @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_setPortMACAddress (CSL_Xge_cpswRegs *hCpswRegs,
    uint32                      portNum,
    uint8*                      pMacAddress
)
{
    if ((portNum >= 1U) &&  (portNum <= 8U))
    {
        CSL_FINS (hCpswRegs->ENETPORT[portNum-1U].PN_SA_L_REG, XGE_CPSW_PN_SA_L_REG_MACSRCADDR_7_0,   ((uint32)pMacAddress [0]));
        CSL_FINS (hCpswRegs->ENETPORT[portNum-1U].PN_SA_L_REG, XGE_CPSW_PN_SA_L_REG_MACSRCADDR_15_8,  ((uint32)pMacAddress [1]));
        CSL_FINS (hCpswRegs->ENETPORT[portNum-1U].PN_SA_H_REG, XGE_CPSW_PN_SA_H_REG_MACSRCADDR_23_16, ((uint32)pMacAddress [2]));
        CSL_FINS (hCpswRegs->ENETPORT[portNum-1U].PN_SA_H_REG, XGE_CPSW_PN_SA_H_REG_MACSRCADDR_31_24, ((uint32)pMacAddress [3]));
        CSL_FINS (hCpswRegs->ENETPORT[portNum-1U].PN_SA_H_REG, XGE_CPSW_PN_SA_H_REG_MACSRCADDR_39_32, ((uint32)pMacAddress [4]));
        CSL_FINS (hCpswRegs->ENETPORT[portNum-1U].PN_SA_H_REG, XGE_CPSW_PN_SA_H_REG_MACSRCADDR_47_40, ((uint32)pMacAddress [5]));
    }

    return;
}

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_setPortTimeSyncConfig
 *
 *   @b Description
 *   @n This function sets up the contents of Time sync control registers
 *      corresponding to the CPSW port number specified per user configuration.
 *
 *   @b Arguments
     @verbatim
        portNum                 CPSW port number for which the registers must be
                                configured.
        pTimeSyncConfig         ETH_CSL_CPSW_TSCONFIG containing settings for time
                                sync control registers.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n XGE_CPSW_PN_TS_CTL_REG_TS_RX_ANNEX_F_EN,
 *      XGE_CPSW_PN_TS_CTL_REG_TS_RX_VLAN_LTYPE1_EN,
 *      XGE_CPSW_PN_TS_CTL_REG_TS_RX_VLAN_LTYPE2_EN,
 *      XGE_CPSW_PN_TS_CTL_REG_TS_RX_ANNEX_D_EN,
 *      XGE_CPSW_PN_TS_CTL_REG_TS_TX_ANNEX_F_EN,
 *      XGE_CPSW_PN_TS_CTL_REG_TS_TX_VLAN_LTYPE1_EN,
 *      XGE_CPSW_PN_TS_CTL_REG_TS_TX_VLAN_LTYPE2_EN,
 *      XGE_CPSW_PN_TS_CTL_REG_TS_TX_ANNEX_D_EN,
 *      XGE_CPSW_PN_TS_CTL_REG_TS_LTYPE2_EN,
 *      XGE_CPSW_PN_TS_CTL_REG_TS_RX_ANNEX_E_EN,
 *      XGE_CPSW_PN_TS_CTL_REG_TS_TX_ANNEX_E_EN,
 *      XGE_CPSW_PN_TS_CTL_REG_TS_TX_HOST_TS_EN,
 *      XGE_CPSW_PN_TS_CTL_REG_TS_MSG_TYPE_EN
 *
 *      XGE_CPSW_PN_TS_SEQ_LTYPE_REG_TS_LTYPE1,
 *      XGE_CPSW_PN_TS_SEQ_LTYPE_REG_TS_SEQ_ID_OFFSET,
 *
 *      XGE_CPSW_PN_TS_VLAN_LTYPE_REG_TS_VLAN_LTYPE1,
 *      XGE_CPSW_PN_TS_VLAN_LTYPE_REG_TS_VLAN_LTYPE2,
 *
 *      XGE_CPSW_PN_TS_CTL_LTYPE2_REG_TS_LTYPE2,
 *      XGE_CPSW_PN_TS_CTL_LTYPE2_REG_TS_107,
 *      XGE_CPSW_PN_TS_CTL_LTYPE2_REG_TS_129,
 *      XGE_CPSW_PN_TS_CTL_LTYPE2_REG_TS_130,
 *      XGE_CPSW_PN_TS_CTL_LTYPE2_REG_TS_131,
 *      XGE_CPSW_PN_TS_CTL_LTYPE2_REG_TS_132,
 *      XGE_CPSW_PN_TS_CTL_LTYPE2_REG_TS_319,
 *      XGE_CPSW_PN_TS_CTL_LTYPE2_REG_TS_330,
 *      XGE_CPSW_PN_TS_CTL_LTYPE2_REG_TS_TTL_NONZERO,
 *      XGE_CPSW_PN_TS_CTL_LTYPE2_REG_TS_UNI_EN,
 *
 *      XGE_CPSW_PN_TS_CTL2_REG_TS_MCAST_TYPE_EN,
 *      XGE_CPSW_PN_TS_CTL2_REG_TS_DOMAIN_OFFSET
 *
 *   @b Example
 *   @verbatim
 *      uint32              portNum;
        ETH_CSL_CPSW_TSCONFIG   tsConfig;

        portNum =   1;

        tsConfig.tsRxVlanLType1Enable   =   0;
        tsConfig.tsRxVlanLType2Enable   =   0;
        ...

        ETH_CSL_CPSW_setPortTimeSyncConfig (portNum, &tsConfig);

   @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_setPortTimeSyncConfig (CSL_Xge_cpswRegs *hCpswRegs,
    uint32                  portNum,
    ETH_CSL_CPSW_TSCONFIG*        pTimeSyncConfig
)
{
    if ((portNum >= 1U) &&  (portNum <= 8U))
    {
        uint32 value = 0;

        /* Time Sync Control Register */
        CSL_FINS (value, XGE_CPSW_PN_TS_CTL_REG_TS_RX_ANNEX_D_EN, pTimeSyncConfig->tsRxAnnexDEnable);
        CSL_FINS (value, XGE_CPSW_PN_TS_CTL_REG_TS_RX_ANNEX_E_EN, pTimeSyncConfig->tsRxAnnexEEnable);
        CSL_FINS (value, XGE_CPSW_PN_TS_CTL_REG_TS_RX_ANNEX_F_EN, pTimeSyncConfig->tsRxAnnexFEnable);
        CSL_FINS (value, XGE_CPSW_PN_TS_CTL_REG_TS_RX_VLAN_LTYPE1_EN, pTimeSyncConfig->tsRxVlanLType1Enable);
        CSL_FINS (value, XGE_CPSW_PN_TS_CTL_REG_TS_RX_VLAN_LTYPE2_EN, pTimeSyncConfig->tsRxVlanLType2Enable);
        CSL_FINS (value, XGE_CPSW_PN_TS_CTL_REG_TS_TX_ANNEX_D_EN, pTimeSyncConfig->tsTxAnnexDEnable);
        CSL_FINS (value, XGE_CPSW_PN_TS_CTL_REG_TS_TX_ANNEX_E_EN, pTimeSyncConfig->tsTxAnnexEEnable);
        CSL_FINS (value, XGE_CPSW_PN_TS_CTL_REG_TS_TX_ANNEX_F_EN, pTimeSyncConfig->tsTxAnnexFEnable);
        CSL_FINS (value, XGE_CPSW_PN_TS_CTL_REG_TS_TX_VLAN_LTYPE1_EN, pTimeSyncConfig->tsTxVlanLType1Enable);
        CSL_FINS (value, XGE_CPSW_PN_TS_CTL_REG_TS_TX_VLAN_LTYPE2_EN, pTimeSyncConfig->tsTxVlanLType2Enable);
        CSL_FINS (value, XGE_CPSW_PN_TS_CTL_REG_TS_TX_HOST_TS_EN, pTimeSyncConfig->tsTxHostEnable);
        CSL_FINS (value, XGE_CPSW_PN_TS_CTL_REG_TS_LTYPE2_EN, pTimeSyncConfig->tsLType2Enable);
        CSL_FINS (value, XGE_CPSW_PN_TS_CTL_REG_TS_MSG_TYPE_EN, pTimeSyncConfig->tsMsgTypeEnable);
        hCpswRegs->ENETPORT[portNum-1U].PN_TS_CTL_REG = value;


        /* Time Sync Sequence and LTYPE Register */
        value = 0;

        CSL_FINS (value, XGE_CPSW_PN_TS_SEQ_LTYPE_REG_TS_LTYPE1, pTimeSyncConfig->tsLType1);
        CSL_FINS (value, XGE_CPSW_PN_TS_SEQ_LTYPE_REG_TS_SEQ_ID_OFFSET, pTimeSyncConfig->tsSeqIdOffset);
        hCpswRegs->ENETPORT[portNum-1U].PN_TS_SEQ_LTYPE_REG = value;

        /* Time Sync VLAN Register */
        value = 0;

        CSL_FINS (value, XGE_CPSW_PN_TS_VLAN_LTYPE_REG_TS_VLAN_LTYPE1, pTimeSyncConfig->tsVlanLType1);
        CSL_FINS (value, XGE_CPSW_PN_TS_VLAN_LTYPE_REG_TS_VLAN_LTYPE2, pTimeSyncConfig->tsVlanLType2);
        hCpswRegs->ENETPORT[portNum-1U].PN_TS_VLAN_LTYPE_REG = value;

        /* Time Sync Control and LTYPE 2 Register */
        value = 0;

        CSL_FINS (value, XGE_CPSW_PN_TS_CTL_LTYPE2_REG_TS_LTYPE2, pTimeSyncConfig->tsLType2);
        CSL_FINS (value, XGE_CPSW_PN_TS_CTL_LTYPE2_REG_TS_107, pTimeSyncConfig->ts107Enable);
        CSL_FINS (value, XGE_CPSW_PN_TS_CTL_LTYPE2_REG_TS_129, pTimeSyncConfig->ts129Enable);
        CSL_FINS (value, XGE_CPSW_PN_TS_CTL_LTYPE2_REG_TS_130, pTimeSyncConfig->ts130Enable);
        CSL_FINS (value, XGE_CPSW_PN_TS_CTL_LTYPE2_REG_TS_131, pTimeSyncConfig->ts131Enable);
        CSL_FINS (value, XGE_CPSW_PN_TS_CTL_LTYPE2_REG_TS_132, pTimeSyncConfig->ts132Enable);
        CSL_FINS (value, XGE_CPSW_PN_TS_CTL_LTYPE2_REG_TS_319, pTimeSyncConfig->ts319Enable);
        CSL_FINS (value, XGE_CPSW_PN_TS_CTL_LTYPE2_REG_TS_320, pTimeSyncConfig->ts320Enable);
        CSL_FINS (value, XGE_CPSW_PN_TS_CTL_LTYPE2_REG_TS_TTL_NONZERO, pTimeSyncConfig->tsTTLNonzeroEnable);
        CSL_FINS (value, XGE_CPSW_PN_TS_CTL_LTYPE2_REG_TS_UNI_EN, pTimeSyncConfig->tsUniEnable);
        hCpswRegs->ENETPORT[portNum-1U].PN_TS_CTL_LTYPE2_REG = value;

        /* Time Sync Control 2 Register */
        value = 0;

        CSL_FINS (value, XGE_CPSW_PN_TS_CTL2_REG_TS_MCAST_TYPE_EN, pTimeSyncConfig->tsMcastTypeEnable);
        CSL_FINS (value, XGE_CPSW_PN_TS_CTL2_REG_TS_DOMAIN_OFFSET, pTimeSyncConfig->tsDomainOffset);
        hCpswRegs->ENETPORT[portNum-1U].PN_TS_CTL2_REG = value;
    }

    return;
}



/********************************************************************************
*************************  Statistics (STATS) Submodule *************************
********************************************************************************/

void ETH_CSL_CPSW_getPortStats (CSL_Xge_cpswRegs *hCpswRegs,
    uint32                  portNum,
    ETH_CSL_CPSW_STATS*         pCpswStats
)
{
    uint32                      numStats;
    volatile uint32             *pRegAddr;
    uint32                      *pStatAddr;
    uint32                      statval;

    pStatAddr   =   (uint32 *)(pCpswStats);

    /* Read the entire stats block for both the
     * Host and the MAC ports and reset the stats
     * block when done.
     */
    pRegAddr    = &hCpswRegs->STATS[portNum].RXGOODFRAMES;
    for (numStats = 0; numStats < (sizeof(ETH_CSL_CPSW_STATS)/sizeof(uint32)); numStats++)
    {
        statval         =   *pRegAddr;
        *pRegAddr       =   statval;
        pRegAddr++;
        *pStatAddr      =   statval;
        pStatAddr++;
    }

    return;
}


/********************************************************************************
********************  Address Lookup Engine (ALE) Submodule *********************
********************************************************************************/

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_isAleVlanAwareEnabled
 *
 *   @b Description
 *   @n This function indicates if ALE is programmed to be VLAN aware.
 *
 *   @b Arguments
 *   @n None
 *
 *   <b> Return Value </b>
 *   @n  TRUE                   ALE VLAN aware. ALE drops packets if VLAN not found.
 *   @n  FALSE                  ALE not VLAN aware. Floods if VLAN not found.
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Reads
 *   @n ALE_ALE_CONTROL_ALE_VLAN_AWARE
 *
 *   @b Example
 *   @verbatim
        if (ETH_CSL_CPSW_isAleVlanAwareEnabled () == TRUE)
        {
            // ALE VLAN aware
        }
        else
        {
            // ALE not VLAN aware
        }
   @endverbatim
 * =============================================================================
 */
uint32 ETH_CSL_CPSW_isAleVlanAwareEnabled (CSL_AleRegs *hCpswAleRegs)
{

    return CSL_FEXT (hCpswAleRegs->ALE_CONTROL, ALE_ALE_CONTROL_ALE_VLAN_AWARE);
}

/** ============================================================================
 *   @n@b CSL_CPSW_isAleBypassEnabled
 *
 *   @b Description
 *   @n This function indicates if ALE is programmed to be in bypass mode.
 *
 *   @b Arguments
 *   @n None
 *
 *   <b> Return Value </b>
 *   @n  TRUE                   ALE bypass mode enabled.
 *   @n  FALSE                  ALE bypass mode disabled.
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Reads
 *   @n ALE_ALE_CONTROL_ENABLE_BYPASS
 *
 *   @b Example
 *   @verbatim
        if (CSL_CPSW_isAleBypassEnabled () == TRUE)
        {
            // ALE bypass mode on
        }
        else
        {
            // ALE bypass mode off
        }
     @endverbatim
 * =============================================================================
 */
uint32 CSL_CPSW_isAleBypassEnabled (CSL_AleRegs *hCpswAleRegs)
{

    return CSL_FEXT (hCpswAleRegs->ALE_CONTROL, ALE_ALE_CONTROL_ENABLE_BYPASS);
}

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_enableAleBypass
 *
 *   @b Description
 *   @n This function configures the ALE control register to enable bypass mode.
 *
 *   @b Arguments
 *   @n None
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n ALE_ALE_CONTROL_ENABLE_BYPASS=1
 *
 *   @b Example
 *   @verbatim

        ETH_CSL_CPSW_enableAleBypass ();

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_enableAleBypass (CSL_AleRegs *hCpswAleRegs)
{
    CSL_FINS (hCpswAleRegs->ALE_CONTROL, ALE_ALE_CONTROL_ENABLE_BYPASS, 1U);

    return;
}

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_disableAleBypass
 *
 *   @b Description
 *   @n This function configures the ALE control register to disable bypass mode.
 *
 *   @b Arguments
 *   @n None
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n ALE_ALE_CONTROL_ENABLE_BYPASS=0
 *
 *   @b Example
 *   @verbatim

        ETH_CSL_CPSW_disableAleBypass ();

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_disableAleBypass (CSL_AleRegs *hCpswAleRegs)
{
    CSL_FINS (hCpswAleRegs->ALE_CONTROL, ALE_ALE_CONTROL_ENABLE_BYPASS, 0U);

    return;
}

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_clearAleTable
 *
 *   @b Description
 *   @n This function initiates a full ALE table cleanup. The ALE hardware
 *      clears all table entries.
 *
 *   @b Arguments
 *   @n None
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n ALE_ALE_CONTROL_CLEAR_TABLE=1
 *
 *   @b Example
 *   @verbatim

        ETH_CSL_CPSW_clearAleTable ();

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_clearAleTable (CSL_AleRegs *hCpswAleRegs)
{
    CSL_FINS (hCpswAleRegs->ALE_CONTROL, ALE_ALE_CONTROL_CLEAR_TABLE, ((uint32)1U));

    return;
}

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_disableAle
 *
 *   @b Description
 *   @n This function configures the ALE control register to disable ALE processing.
 *
 *   @b Arguments
 *   @n None
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n ALE_ALE_CONTROL_ENABLE_ALE=0
 *
 *   @b Example
 *   @verbatim

        ETH_CSL_CPSW_disableAle ();

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_disableAle (CSL_AleRegs *hCpswAleRegs)
{
    CSL_FINS (hCpswAleRegs->ALE_CONTROL, ALE_ALE_CONTROL_ENABLE_ALE, ((uint32)0U));

    return;
}

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_getAleControlReg
 *
 *   @b Description
 *   @n This function retrieves the contents of the ALE control register.
 *
 *   @b Arguments
 *   @n None
 *
 *   <b> Return Value </b>
 *   @n  >=0                ALE control register contents.
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Reads
 *   @n ALE_CONTROL_REG
 *
 *   @b Example
 *   @verbatim
        uint32      aleCtrlVal;

        aleCtrlVal  =   ETH_CSL_CPSW_getAleControlReg ();

     @endverbatim
 * =============================================================================
 */
uint32 ETH_CSL_CPSW_getAleControlReg (CSL_AleRegs *hCpswAleRegs)
{
    return hCpswAleRegs->ALE_CONTROL;
}

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_getAleCOSPolicerEntry
 *
 *   @b Description
 *   @n This function reads the ALE Policer table entry for the index specified and
 *      fills the output parameter structure with Policer configuration
 *      read from the hardware.
 *      This function only reads data related to COS mapping in ALE policer entry.
 *
 *   @b Arguments
     @verbatim
        index                   ALE Policer table index to be read.
        pPolCfg                 ALE Policer entry contents read.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n ALE_POLICETBLCTL_POL_TBL_IDX
 *      ALE_POLICETBLCTL_WRITE_ENABLE=0
 *
 *   @b Reads
 *   @n ALE_POLICECFG0_PORT_MEN,
 *      ALE_POLICECFG0_PRI_MEN,
 *      ALE_THREADMAPVAL_THREAD_EN,
 *      ALE_THREADMAPVAL_THREADVAL
 *
 *   @b Example
 *   @verbatim
        Uint32                                  index;
        ETH_CSL_CPSW_ALE_POLICER_ENTRY          polCfg;

        index   =   0;

         // Read Policer Entry config from hardware
         ETH_CSL_CPSW_getAleCOSPolicerEntry (index, &polCfg);

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_getAleCOSPolicerEntry
(CSL_AleRegs *hCpswAleRegs,
    uint32                                  index,
    ETH_CSL_CPSW_ALE_POLICER_ENTRY*         pPolCfg
)
{
    uint32                                  aleTblCtrlVal, value;

    /* Set the index in the ALE table to "read" operation and let
     * the hardware load the ALE table entry corresponding to index
     * specified into the ALE table word registers.
     */
    aleTblCtrlVal           =   CSL_FMK (ALE_POLICETBLCTL_POL_TBL_IDX, index) |
                                CSL_FMK (ALE_POLICETBLCTL_WRITE_ENABLE, (uint32) 0);

    hCpswAleRegs->POLICETBLCTL    =   aleTblCtrlVal;


    pPolCfg->validBitmap = 0u;
    /* Read the policer entry configuration */
    value  =  hCpswAleRegs->POLICECFG0;

    if(CSL_FEXT(value, ALE_POLICECFG0_PORT_MEN))
    {
        pPolCfg->validBitmap |= ETH_CSL_CPSW_ALE_POLICER_PORT_VALID;
        pPolCfg->port = CSL_FEXT(value, ALE_POLICECFG0_PORT_NUM);
    }

    if(CSL_FEXT(value, ALE_POLICECFG0_PRI_MEN))
    {
        pPolCfg->validBitmap |= ETH_CSL_CPSW_ALE_POLICER_PRI_VALID;
        pPolCfg->pri = CSL_FEXT(value, ALE_POLICECFG0_PRI_VAL);
    }

    /* Read Thread value */
    aleTblCtrlVal           =   CSL_FMK (ALE_THREADMAPCTL_CLASSINDEX, index);

    hCpswAleRegs->THREADMAPCTL    =   aleTblCtrlVal;

    value  =  hCpswAleRegs->THREADMAPVAL;

    if(CSL_FEXT(value, ALE_THREADMAPVAL_THREAD_EN))
    {
        pPolCfg->validBitmap |= ETH_CSL_CPSW_ALE_POLICER_THREAD_VALID;
        pPolCfg->thread = CSL_FEXT(value, ALE_THREADMAPVAL_THREADVAL);
    }

    return;
}

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_setAleControlReg
 *
 *   @b Description
 *   @n This function sets up the contents of the ALE control register.
 *
 *   @b Arguments
     @verbatim
        aleCtrlVal          Value to be configured to the ALE control register.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n ALE_CONTROL_REG
 *
 *   @b Example
 *   @verbatim
 *      uint32          aleCtrlVal = 0;

        aleCtrlVal      =   ETH_CSL_CPSW_getAleControlReg ();
        aleCtrlVal      |=  CSL_XGE_CPSW_ALECONTROL_CLRTABLE_EN;

        ETH_CSL_CPSW_setAleControlReg (&aleCtrlRegInfo);

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_setAleControlReg
(CSL_AleRegs *hCpswAleRegs,
    uint32                      aleCtrlVal
)
{
    hCpswAleRegs->ALE_CONTROL  =   aleCtrlVal;

    return;
}

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_setAleCOSPolicerEntry
 *
 *   @b Description
 *   @n This function sets up the ALE COS table entry for the index specified with
 *      COS configuration specified here.
 *
 *   @b Arguments
     @verbatim
        index                   ALE table index.
        pPolCfg                 ALE entry contents to be configured.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n ALE_POLICECFG0_PRI_VAL,
 *      ALE_THREADMAPVAL_THREAD_EN,
 *      ALE_THREADMAPVAL_THREADVAL
 *
 *   @b Example
 *   @verbatim
        uint32                                  index;
        ETH_CSL_CPSW_ALE_POLICER_ENTRY          polCfg;

        index   =   0;
        polEntry.pri = priority;
        polEntry.thread = threadId;
        ...

        // Add ALE Policer entry
        ETH_CSL_CPSW_setAleCOSPolicerEntry (index, &polCfg);

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_setAleCOSPolicerEntry
(CSL_AleRegs *hCpswAleRegs,
    uint32                              index,
    ETH_CSL_CPSW_ALE_POLICER_ENTRY*         pPolCfg
)
{
    uint32                                  aleTblCtrlVal, value;

    /* Read the Policer Entry configuration */
    value = 0;

    if((pPolCfg->validBitmap & ETH_CSL_CPSW_ALE_POLICER_PORT_VALID) != 0u)
    {
        value |= CSL_FMK (ALE_POLICECFG0_PORT_MEN, (uint32) 1) |
                 CSL_FMK (ALE_POLICECFG0_PORT_NUM, pPolCfg->port);
    }

    if((pPolCfg->validBitmap &  ETH_CSL_CPSW_ALE_POLICER_PRI_VALID) != 0u)
    {
        value |= CSL_FMK (ALE_POLICECFG0_PRI_MEN, (uint32) 1) |
                 CSL_FMK (ALE_POLICECFG0_PRI_VAL, pPolCfg->pri);
    }

    hCpswAleRegs->POLICECFG0 = value;

    /* Set the index in the ALE Policertable to "write" operation and let
     * the hardware load the ALE Policer entry corresponding to index
     * specified into the ALE Policer registers.
     */
    aleTblCtrlVal           =   CSL_FMK (ALE_POLICETBLCTL_POL_TBL_IDX, index) |
                                CSL_FMK (ALE_POLICETBLCTL_WRITE_ENABLE, (uint32) 1);

    hCpswAleRegs->POLICETBLCTL    =   aleTblCtrlVal;
    /* Write Thread value */
    if((pPolCfg->validBitmap & ETH_CSL_CPSW_ALE_POLICER_THREAD_VALID) != 0u)
    {

        value =  CSL_FMK (ALE_THREADMAPVAL_THREAD_EN, (uint32) 1) |
                 CSL_FMK (ALE_THREADMAPVAL_THREADVAL, pPolCfg->thread);

        hCpswAleRegs->THREADMAPVAL = value;

        aleTblCtrlVal       =   CSL_FMK (ALE_THREADMAPCTL_CLASSINDEX, index);

        hCpswAleRegs->THREADMAPCTL    =   aleTblCtrlVal;

    }

    return;
}


void ETH_CSL_CPSW_disableAlePolicerThread(CSL_AleRegs *hCpswAleRegs,
                                                    uint32           index)
{
  uint32 aleTblCtrlVal;


    aleTblCtrlVal =  CSL_FMK (ALE_THREADMAPVAL_THREAD_EN, (uint32) 0U) |
             CSL_FMK (ALE_THREADMAPVAL_THREADVAL, 0U);

    hCpswAleRegs->THREADMAPVAL = aleTblCtrlVal;

    aleTblCtrlVal       =   CSL_FMK (ALE_THREADMAPCTL_CLASSINDEX, index);

    hCpswAleRegs->THREADMAPCTL    =   aleTblCtrlVal;
    return;
}


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_getAleUnkownVlanReg
 *
 *   @b Description
 *   @n This function retrieves the contents of the ALE Unknown VLAN and etc registers.
 *
 *   @b Arguments
     @verbatim
        pUnVlanMemList          Unknown VLAN member list.
        pUnMcastFloodMask       Unknown VLAN Multicast flood mask.
        pUnRegMcastFloodMask    Unknown VLAN Registered Multicast Flood mask.
        pUnForceUntagEgress     Unknown VLAN Force Untagged Egress.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Reads
 *   @n ALE_UNKNOWN_VLAN_REG_UNKNOWN_LIST,
 *      ALE_UNKNOWN_MCAST_FLOOD_REG_MASK,
 *      ALE_UNKNOWN_REG_MCAST_FLOOD_REG_MASK,
 *      ALE_ALE_UVLAN_UNTAG_UVLAN_FORCE_UNTAGGED_EGRESS
 *
 *   @b Example
 *   @verbatim
        uint32      unVlanMemList, unMcastFloodMask, unRegMcastFloodMask, unForceUntagEgress;

        ETH_CSL_CPSW_getAleUnkownVlanReg (&unVlanMemList,
                                      &unMcastFloodMask,
                                      &unRegMcastFloodMask,
                                      &unForceUntagEgress);

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_getAleUnkownVlanReg
(CSL_AleRegs *hCpswAleRegs,
    uint32*                        pUnVlanMemList,
    uint32*                        pUnMcastFloodMask,
    uint32*                        pUnRegMcastFloodMask,
    uint32*                        pUnForceUntagEgress
)
{
    *pUnVlanMemList         =   CSL_FEXT(hCpswAleRegs->ALE_UVLAN_MEMBER, ALE_ALE_UVLAN_MEMBER_UVLAN_MEMBER_LIST);
    *pUnMcastFloodMask      =   CSL_FEXT(hCpswAleRegs->ALE_UVLAN_URCAST, ALE_ALE_UVLAN_RMCAST_UVLAN_REG_MCAST_FLOOD_MASK);
    *pUnRegMcastFloodMask   =   CSL_FEXT(hCpswAleRegs->ALE_UVLAN_RMCAST, ALE_ALE_UVLAN_URCAST_UVLAN_UNREG_MCAST_FLOOD_MASK);
    *pUnForceUntagEgress    =   CSL_FEXT(hCpswAleRegs->ALE_UVLAN_UNTAG, ALE_ALE_UVLAN_UNTAG_UVLAN_FORCE_UNTAGGED_EGRESS);
}

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_setAleUnkownVlanReg
 *
 *   @b Description
 *   @n This function sets up the contents of the ALE Unknown VLAN and etc. register.
 *
 *   @b Arguments
     @verbatim
        unVlanMemList           Unknown VLAN member list.
        unMcastFloodMask        Unknown VLAN Multicast flood mask.
        unRegMcastFloodMask     Unknown VLAN Registered Multicast Flood mask.
        unForceUntagEgress      Unknown VLAN Force Untagged Egress.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n ALE_UNKNOWN_VLAN_REG_UNKNOWN_LIST,
 *      ALE_UNKNOWN_MCAST_FLOOD_REG_MASK,
 *      ALE_UNKNOWN_REG_MCAST_FLOOD_REG_MASK,
 *      ALE_ALE_UVLAN_UNTAG_UVLAN_FORCE_UNTAGGED_EGRESS
 *
 *   @b Example
 *   @verbatim
        uint32      unVlanMemList, unMcastFloodMask, unRegMcastFloodMask, unForceUntagEgress;

        unVlanMemList           =   0;
        unMcastFloodMask        =   3;
        unRegMcastFloodMask     =   0;
        unForceUntagEgress      =   0;

        ETH_CSL_CPSW_setAleUnkownVlanReg (unVlanMemList,
                                      unMcastFloodMask,
                                      unRegMcastFloodMask,
                                      unForceUntagEgress);

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_setAleUnkownVlanReg
(CSL_AleRegs *hCpswAleRegs,
    uint32                      unVlanMemList,
    uint32                      unMcastFloodMask,
    uint32                      unRegMcastFloodMask,
    uint32                      unForceUntagEgress
)
{
    CSL_FINS (hCpswAleRegs->ALE_UVLAN_MEMBER, ALE_ALE_UVLAN_MEMBER_UVLAN_MEMBER_LIST, unVlanMemList);
    CSL_FINS (hCpswAleRegs->ALE_UVLAN_URCAST, ALE_ALE_UVLAN_RMCAST_UVLAN_REG_MCAST_FLOOD_MASK, unMcastFloodMask);
    CSL_FINS (hCpswAleRegs->ALE_UVLAN_RMCAST, ALE_ALE_UVLAN_URCAST_UVLAN_UNREG_MCAST_FLOOD_MASK, unRegMcastFloodMask);
    CSL_FINS (hCpswAleRegs->ALE_UVLAN_UNTAG, ALE_ALE_UVLAN_UNTAG_UVLAN_FORCE_UNTAGGED_EGRESS, unForceUntagEgress);
}


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_getAleTableEntry
 *
 *   @b Description
 *   @n This function retrieves an ALE table entry corresponding to the
 *      ALE entry index specified in 'index' input parameter. The ALE
 *      entry values corresponding to the ALE_TBLW0, ALE_TBLW1 and
 *      ALE_TBLW2 registers are returned in 'pAleInfoWd0', 'pAleInfoWd1', 'pAleInfoWd2'
 *      output parameters.
 *
 *   @b Arguments
     @verbatim
        index                   ALE table index to be read.
        pAleInfoWd0             Contents of ALE Table Word 0 Register (ALE_TBLW0).
        pAleInfoWd1             Contents of ALE Table Word 1 Register (ALE_TBLW1).
        pAleInfoWd2             Contents of ALE Table Word 2 Register (ALE_TBLW2).
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n ALE_ALE_TBLCTL_TABLEIDX,
 *      ALE_ALE_TBLCTL_TABLEWR=0
 *
 *   @b Reads
 *   @n ALE_ALE_TBLW0_TABLEWRD0,
 *      ALE_ALE_TBLW1_TABLEWRD1,
 *      ALE_ALE_TBLW2_TABLEWRD2
 *
 *   @b Example
 *   @verbatim
        uint32      index, info0, info1, info2;

        index   =   0;

        ETH_CSL_CPSW_getAleTableEntry (index,
                                   &info0,
                                   &info1,
                                   &info2);

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_getAleTableEntry
(CSL_AleRegs *hCpswAleRegs,
    uint32                      index,
    uint32*                     pAleInfoWd0,
    uint32*                     pAleInfoWd1,
    uint32*                     pAleInfoWd2
)
{
    uint32                      aleTblCtrlVal   =   0;

    /* Set the index in the ALE table to "read" operation and let
     * the hardware load the ALE table entry corresponding to index
     * specified into the ALE table word registers.
     */
    aleTblCtrlVal           =   CSL_FMK (ALE_ALE_TBLCTL_TABLEIDX, index) |
                                CSL_FMK (ALE_ALE_TBLCTL_TABLEWR, ((uint32)0U));

    hCpswAleRegs->ALE_TBLCTL    =   aleTblCtrlVal;

    /* Read the ALE table word registers */
    *pAleInfoWd0            =   CSL_FEXT(hCpswAleRegs->ALE_TBLW0, ALE_ALE_TBLW0_TABLEWRD0);
    *pAleInfoWd1            =   CSL_FEXT(hCpswAleRegs->ALE_TBLW1, ALE_ALE_TBLW1_TABLEWRD1);
    *pAleInfoWd2            =   CSL_FEXT(hCpswAleRegs->ALE_TBLW2, ALE_ALE_TBLW2_TABLEWRD2);

    return;
}


/** ============================================================================
 *   @n@b ETH_CSL_CPSW_clearAleEntry
 *
 *   @b Description
 *   @n This function clears the ALE entry corresponding to the index
 *      specified
 *
 *   @b Arguments
     @verbatim
        index                   ALE table index.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n ALE_ALE_TBLCTL_TABLEIDX,
 *      ALE_ALE_TBLCTL_TABLEWR=1
 *      ALE_TABLE_WORD0=0,
 *      ALE_TABLE_WORD1=0,
 *      ALE_TABLE_WORD2=0
 *
 *   @b Example
 *   @verbatim
        uint32                              index;

        index   =   0;

        ETH_CSL_CPSW_clearAleEntry (index);
     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_clearAleEntry(CSL_AleRegs *hCpswAleRegs,
    uint32                                  index)
{
    uint32                                  aleTblCtrlVal   =   0;

    /* Clear all the ALE words */
    hCpswAleRegs->ALE_TBLW0   =   0;
    hCpswAleRegs->ALE_TBLW1   =   0;
    hCpswAleRegs->ALE_TBLW2   =   0;

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
 *   @n@b ETH_CSL_CPSW_getAlePortControlReg
 *
 *   @b Description
 *   @n This function retrieves the contents of ALE Port control register
 *      corresponding to the port number specified.
 *
 *   @b Arguments
     @verbatim
        portNo                  Port number for which the ALE port control register
                                must be read.
        pPortControlInfo        ETH_CSL_CPSW_ALE_PORTCONTROL structure that needs to be
                                filled with Port control register info read from
                                the hardware.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Reads
 *   @n ALE_I0_ALE_PORTCTL0_I0_REG_P0_PORTSTATE,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_DROP_UN_TAGGED,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_VID_INGRESS_CHECK,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_NO_LEARN,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_NO_SA_UPDATE,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_MACONLY,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_DIS_PAUTHMOD,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_MACONLY_CAF,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_MCAST_LIMIT,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_BCAST_LIMIT
 *
 *   @b Example
 *   @verbatim
        uint32                          index;
        ETH_CSL_CPSW_ALE_PORTCONTROL    portControlInfo;

        index   =   0;

        ETH_CSL_CPSW_getAlePortControlReg (index, &portControlInfo);

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_getAlePortControlReg
(CSL_AleRegs *hCpswAleRegs,
    uint32                      portNo,
    ETH_CSL_CPSW_ALE_PORTCONTROL*   pPortControlInfo
)
{
    uint32 portState =
        CSL_FEXT (hCpswAleRegs->I0_ALE_PORTCTL0 [portNo], ALE_I0_ALE_PORTCTL0_I0_REG_P0_PORTSTATE);

    pPortControlInfo->portState             =   (ETH_CSL_CPSW_ALE_PORTSTATE) portState;
    pPortControlInfo->dropUntaggedEnable    =   CSL_FEXT (hCpswAleRegs->I0_ALE_PORTCTL0 [portNo],
                                                          ALE_I0_ALE_PORTCTL0_I0_REG_P0_DROP_UN_TAGGED);
    pPortControlInfo->vidIngressCheckEnable =   CSL_FEXT (hCpswAleRegs->I0_ALE_PORTCTL0 [portNo],
                                                          ALE_I0_ALE_PORTCTL0_I0_REG_P0_VID_INGRESS_CHECK);
    pPortControlInfo->noLearnModeEnable     =   CSL_FEXT (hCpswAleRegs->I0_ALE_PORTCTL0 [portNo],
                                                          ALE_I0_ALE_PORTCTL0_I0_REG_P0_NO_LEARN);
    pPortControlInfo->noSaUpdateEnable      =   CSL_FEXT (hCpswAleRegs->I0_ALE_PORTCTL0 [portNo],
                                                          ALE_I0_ALE_PORTCTL0_I0_REG_P0_NO_SA_UPDATE);
    pPortControlInfo->macOnlyEnable         =   CSL_FEXT (hCpswAleRegs->I0_ALE_PORTCTL0 [portNo],
                                                          ALE_I0_ALE_PORTCTL0_I0_REG_P0_MACONLY);
    pPortControlInfo->macAuthDisable        =   CSL_FEXT (hCpswAleRegs->I0_ALE_PORTCTL0 [portNo],
                                                          ALE_I0_ALE_PORTCTL0_I0_REG_P0_DIS_PAUTHMOD);
    pPortControlInfo->macOnlyCafEnable      =   CSL_FEXT (hCpswAleRegs->I0_ALE_PORTCTL0 [portNo],
                                                          ALE_I0_ALE_PORTCTL0_I0_REG_P0_MACONLY_CAF);
    pPortControlInfo->mcastLimit            =   CSL_FEXT (hCpswAleRegs->I0_ALE_PORTCTL0 [portNo],
                                                          ALE_I0_ALE_PORTCTL0_I0_REG_P0_MCAST_LIMIT);
    pPortControlInfo->bcastLimit            =   CSL_FEXT (hCpswAleRegs->I0_ALE_PORTCTL0 [portNo],
                                                          ALE_I0_ALE_PORTCTL0_I0_REG_P0_BCAST_LIMIT);
    pPortControlInfo->dropDualVlan = CSL_FEXT (hCpswAleRegs->I0_ALE_PORTCTL0 [portNo],
                                               ALE_I0_ALE_PORTCTL0_I0_REG_P0_DROP_DUAL_VLAN);
    pPortControlInfo->dropDoubleVlan = CSL_FEXT (hCpswAleRegs->I0_ALE_PORTCTL0 [portNo],
                                               ALE_I0_ALE_PORTCTL0_I0_REG_P0_DROP_DOUBLE_VLAN);

    return;
}

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_setAlePortControlReg
 *
 *   @b Description
 *   @n This function sets up the contents of ALE Port control register
 *      corresponding to the port number specified.
 *
 *   @b Arguments
     @verbatim
        portNo                  Port number for which the ALE port control register
                                must be configured.
        pPortControlInfo        ETH_CSL_CPSW_ALE_PORTCONTROL structure that contains
                                port control register settings to be written.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n ALE_I0_ALE_PORTCTL0_I0_REG_P0_PORTSTATE,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_DROP_UN_TAGGED,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_VID_INGRESS_CHECK,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_NO_LEARN,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_NO_SA_UPDATE,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_MACONLY,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_DIS_PAUTHMOD,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_MACONLY_CAF,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_MCAST_LIMIT,
 *      ALE_I0_ALE_PORTCTL0_I0_REG_P0_BCAST_LIMIT
 *
 *   @b Example
 *   @verbatim
        uint32                          index;
        ETH_CSL_CPSW_ALE_PORTCONTROL    portControlInfo;

        index   =   0;
        portControlInfo.portState   =   ALE_PORTSTATE_FORWARD |
                                        ALE_PORTSTATE_LEARN;

        ETH_CSL_CPSW_setAlePortControlReg (index, &portControlInfo);

     @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_setAlePortControlReg
(CSL_AleRegs *hCpswAleRegs,
    uint32                      portNo,
    ETH_CSL_CPSW_ALE_PORTCONTROL*   pPortControlInfo
)
{
    CSL_FINS (hCpswAleRegs->I0_ALE_PORTCTL0 [portNo], ALE_I0_ALE_PORTCTL0_I0_REG_P0_PORTSTATE,
              ((uint32)pPortControlInfo->portState));
    CSL_FINS (hCpswAleRegs->I0_ALE_PORTCTL0 [portNo], ALE_I0_ALE_PORTCTL0_I0_REG_P0_DROP_UN_TAGGED,
              pPortControlInfo->dropUntaggedEnable);
    CSL_FINS (hCpswAleRegs->I0_ALE_PORTCTL0 [portNo], ALE_I0_ALE_PORTCTL0_I0_REG_P0_VID_INGRESS_CHECK,
              pPortControlInfo->vidIngressCheckEnable);
    CSL_FINS (hCpswAleRegs->I0_ALE_PORTCTL0 [portNo], ALE_I0_ALE_PORTCTL0_I0_REG_P0_NO_LEARN,
              pPortControlInfo->noLearnModeEnable);
    CSL_FINS (hCpswAleRegs->I0_ALE_PORTCTL0 [portNo], ALE_I0_ALE_PORTCTL0_I0_REG_P0_NO_SA_UPDATE,
              pPortControlInfo->noSaUpdateEnable);
    CSL_FINS (hCpswAleRegs->I0_ALE_PORTCTL0 [portNo], ALE_I0_ALE_PORTCTL0_I0_REG_P0_MACONLY,
              pPortControlInfo->macOnlyEnable);
    CSL_FINS (hCpswAleRegs->I0_ALE_PORTCTL0 [portNo], ALE_I0_ALE_PORTCTL0_I0_REG_P0_DIS_PAUTHMOD,
              pPortControlInfo->macAuthDisable);
    CSL_FINS (hCpswAleRegs->I0_ALE_PORTCTL0 [portNo], ALE_I0_ALE_PORTCTL0_I0_REG_P0_MACONLY_CAF,
              pPortControlInfo->macOnlyCafEnable);
    CSL_FINS (hCpswAleRegs->I0_ALE_PORTCTL0 [portNo], ALE_I0_ALE_PORTCTL0_I0_REG_P0_MCAST_LIMIT,
              pPortControlInfo->mcastLimit);
    CSL_FINS (hCpswAleRegs->I0_ALE_PORTCTL0 [portNo], ALE_I0_ALE_PORTCTL0_I0_REG_P0_BCAST_LIMIT,
              pPortControlInfo->bcastLimit);
    CSL_FINS (hCpswAleRegs->I0_ALE_PORTCTL0 [portNo], ALE_I0_ALE_PORTCTL0_I0_REG_P0_DROP_DUAL_VLAN,
              pPortControlInfo->dropDualVlan);
    CSL_FINS (hCpswAleRegs->I0_ALE_PORTCTL0 [portNo], ALE_I0_ALE_PORTCTL0_I0_REG_P0_DROP_DOUBLE_VLAN,
              pPortControlInfo->dropDoubleVlan);

    return;
}

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_setAlePolicerGlobConfig
 *
 *   @b Description
 *   @n This function sets up the contents of the CPSW ALE Policer/Classifier
 *      global registers
 *      per user-specified ALE Policer Global Configuration.
 *
 *   @b Arguments
     @verbatim
        pGlobConfig         ETH_CSL_CPSW_ALE_POLICER_GLOB_CONFIG structure that holds the values
                            that need to be configured to the ALE Policer global control
                            registers.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  CPSW ALE Policer Global control register modified with values provided.
 *
 *   @b Writes
 *   @n ALE_THREADMAPDEF_DEFTHREAD_EN,
 *      ALE_THREADMAPDEF_DEFTHREADVAL
 *
 *   @b Example
 *   @verbatim
        ETH_CSL_CPSW_ALE_POLICER_GLOB_CONFIG    globConfig;

        globConfig.defThreadEnable  =   1;
        ...

        ETH_CSL_CPSW_setAlePolicerGlobConfig (&globConfig);

   @endverbatim
 * =============================================================================
 */
void ETH_CSL_CPSW_setAlePolicerGlobConfig (CSL_AleRegs *hCpswAleRegs,
  ETH_CSL_CPSW_ALE_POLICER_GLOB_CONFIG*   pGlobConfig
)
{
    CSL_FINS (hCpswAleRegs->THREADMAPDEF, ALE_THREADMAPDEF_DEFTHREAD_EN, pGlobConfig->defThreadEnable);
    CSL_FINS (hCpswAleRegs->THREADMAPDEF, ALE_THREADMAPDEF_DEFTHREADVAL, pGlobConfig->defThread);
}

void ETH_CSL_CPSW_setPriCirEir(CSL_Xge_cpswRegs *hCpswRegs,uint32 portNo, uint32 pri, uint32 cir, uint32 eir)
{
    CSL_FINS (hCpswRegs->ENETPORT[portNo].PN_PRI_CIR_REG[pri], XGE_CPSW_PN_PRI_CIR_REG_PRI_CIR, cir);
    CSL_FINS (hCpswRegs->ENETPORT[portNo].PN_PRI_EIR_REG[pri], XGE_CPSW_PN_PRI_EIR_REG_PRI_EIR, eir);
}

void ETH_CSL_CPSW_getPriCirEir(CSL_Xge_cpswRegs *hCpswRegs,uint32 portNo, uint32 pri, uint32 *cir, uint32 *eir)
{
    *cir = CSL_FEXT (hCpswRegs->ENETPORT[portNo].PN_PRI_CIR_REG[pri], XGE_CPSW_PN_PRI_CIR_REG_PRI_CIR);
    *eir = CSL_FEXT (hCpswRegs->ENETPORT[portNo].PN_PRI_EIR_REG[pri], XGE_CPSW_PN_PRI_EIR_REG_PRI_EIR);
}

/** ============================================================================
 *   @n@b ETH_CSL_CPSW_setCppiP0Control
 *
 *   @b Description
 *   @n This function sets the P0_CONTROL_REG register contents.
 *
 *   @b Arguments
 *    @verbatim
 *       pCppiP0ControlCfg   P0_CONTROL_REG configuration structure
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  none
 *
 * =============================================================================
 */
void  ETH_CSL_CPSW_setCppiP0Control(CSL_Xge_cpswRegs *hCpswRegs,
                                ETH_CSL_CPSW_CPPI_P0_CONTROL *pCppiP0ControlCfg)
{
    CSL_FINS(hCpswRegs->P0_CONTROL_REG,XGE_CPSW_P0_CONTROL_REG_RX_CHECKSUM_EN,
             pCppiP0ControlCfg->p0RxChksumEn);

    CSL_FINS(hCpswRegs->P0_CONTROL_REG,XGE_CPSW_P0_CONTROL_REG_DSCP_IPV4_EN,
             pCppiP0ControlCfg->p0DscpIpv4En);

    CSL_FINS(hCpswRegs->P0_CONTROL_REG,XGE_CPSW_P0_CONTROL_REG_DSCP_IPV6_EN,
             pCppiP0ControlCfg->p0DscpIpv6En);

    CSL_FINS(hCpswRegs->P0_CONTROL_REG,XGE_CPSW_P0_CONTROL_REG_TX_ECC_ERR_EN,
             pCppiP0ControlCfg->p0TxEccErrEn);

    CSL_FINS(hCpswRegs->P0_CONTROL_REG,XGE_CPSW_P0_CONTROL_REG_RX_ECC_ERR_EN,
             pCppiP0ControlCfg->p0RxEccErrEn);

    CSL_FINS(hCpswRegs->P0_CONTROL_REG,XGE_CPSW_P0_CONTROL_REG_RX_REMAP_VLAN,
             pCppiP0ControlCfg->p0RxRemapVlan);

    CSL_FINS(hCpswRegs->P0_CONTROL_REG,XGE_CPSW_P0_CONTROL_REG_RX_REMAP_DSCP_V4,
             pCppiP0ControlCfg->p0RxRemapDscpIpv4);

    CSL_FINS(hCpswRegs->P0_CONTROL_REG,XGE_CPSW_P0_CONTROL_REG_RX_REMAP_DSCP_V6,
             pCppiP0ControlCfg->p0RxRemapDscpIpv6);
}

/**
@}
*/
