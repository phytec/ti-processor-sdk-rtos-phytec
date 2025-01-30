/**
 * @file  csl_cpgmac_sl.c
 *
 * @brief
 *  API Function layer  for Ethernet MAC submodule CSL.
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

#include "cslr_xge_cpsw.h"
#include "csl_cpsw.h"
#include "csl_cpgmac_sl.h"


/** @addtogroup CSL_CPGMAC_SL_FUNCTION
@{ */


/********************************************************************************
*************** Ethernet Media Access Controller (EMAC) Submodule ***************
********************************************************************************/

/** ============================================================================
 *   @n@b CSL_CPGMAC_SL_enableLoopback
 *
 *   @b Description
 *   @n This function configures the MAC control register to enable lopback mode
 *      for the MAC port specified.
 *
 *   @b Arguments
     @verbatim
        portNum                 MAC port number for which settings must be configured.
 *	 @endverbatim
 *
 *   <b> Return Value </b>
 *	 @n	 None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n XGE_CPSW_PN_MAC_CONTROL_REG_LOOPBACK=1
 *
 *   @b Example
 *   @verbatim
        uint32  portNum = 1;

        CSL_CPGMAC_SL_enableLoopback (portNum);

     @endverbatim
 * =============================================================================
 */
void CSL_CPGMAC_SL_enableLoopback
(CSL_Xge_cpswRegs *hCpswRegs,
    uint32                      portNum
)
{
    CSL_FINS (hCpswRegs->ENETPORT[portNum].PN_MAC_CONTROL_REG, XGE_CPSW_PN_MAC_CONTROL_REG_LOOPBACK, 1U);

    return;
}

/** ============================================================================
 *   @n@b CSL_CPGMAC_SL_enableGMII
 *
 *   @b Description
 *   @n This function configures the MAC control register to enable GMII for the
 *      MAC port specified.
 *
 *   @b Arguments
     @verbatim
        portNum                 MAC port number for which settings must be configured.
 *	 @endverbatim
 *
 *   <b> Return Value </b>
 *	 @n	 None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n XGE_CPSW_PN_MAC_CONTROL_REG_GMII_EN=1
 *
 *   @b Example
 *   @verbatim
        uint32  portNum =   1;

        CSL_CPGMAC_SL_enableGMII (portNum);

     @endverbatim
 * =============================================================================
 */
void CSL_CPGMAC_SL_enableGMII
(CSL_Xge_cpswRegs *hCpswRegs,
    uint32                      portNum
)
{
    CSL_FINS (hCpswRegs->ENETPORT[portNum].PN_MAC_CONTROL_REG, XGE_CPSW_PN_MAC_CONTROL_REG_GMII_EN, 1U);

    return;
}

/** ============================================================================
 *   @n@b CSL_CPGMAC_SL_disableGMII
 *
 *   @b Description
 *   @n This function configures the MAC control register to disable GMII for the
 *      MAC port specified.
 *
 *   @b Arguments
     @verbatim
        portNum                 MAC port number for which settings must be configured.
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
 *   @n XGE_CPSW_PN_MAC_CONTROL_REG_GMII_EN=0
 *
 *   @b Example
 *   @verbatim
        uint32  portNum =   1;

        CSL_CPGMAC_SL_disableGMII (portNum);

     @endverbatim
 * =============================================================================
 */
void CSL_CPGMAC_SL_disableGMII
(CSL_Xge_cpswRegs *hCpswRegs,
    uint32                      portNum
)
{
    CSL_FINS (hCpswRegs->ENETPORT[portNum].PN_MAC_CONTROL_REG, XGE_CPSW_PN_MAC_CONTROL_REG_GMII_EN, 0U);

    return;
}

/** ============================================================================
 *   @n@b CSL_CPGMAC_SL_getMacControlReg
 *
 *   @b Description
 *   @n This function retrieves the contents of MAC control register in entirety.
 *
 *   @b Arguments
     @verbatim
        portNum                 MAC port number for which the API must
                                retrieve the settings.
 *	 @endverbatim
 *
 *   <b> Return Value </b>
 *	 @n	 >=0                MAC control register contents.
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Reads
 *   @n CPGMAC_SL_MACCONTROL_REG
 *
 *   @b Example
 *   @verbatim
        uint32          macControlVal, portNum = 1;

        macControlVal   =   CSL_CPGMAC_SL_getMacControlReg (portNum);

     @endverbatim
 * =============================================================================
 */
uint32 CSL_CPGMAC_SL_getMacControlReg
(CSL_Xge_cpswRegs *hCpswRegs,
    uint32                      portNum
)
{
    return hCpswRegs->ENETPORT[portNum].PN_MAC_CONTROL_REG;
}

/** ============================================================================
 *   @n@b CSL_CPGMAC_SL_setMacControlReg
 *
 *   @b Description
 *   @n This function configures the contents of MAC control register in entirety.
 *
 *   @b Arguments
     @verbatim
        portNum             MAC port number for which the reset must be performed.
        macControlRegVal    value to be configured to the MAC control register.
 *	 @endverbatim
 *
 *   <b> Return Value </b>
 *	 @n	 None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n CPGMAC_SL_MACCONTROL_REG
 *
 *   @b Example
 *   @verbatim
        uint32          macControlVal, portNum = 1;

        macControlVal   =   CSL_CPGMAC_SL_getMacControlReg ();

        // Enable full duplex, giagbit mode too
        macControlVal   |=  CSL_CPGMAC_SL_MACCONTROL_FULLDUPLEX_EN |
                            CSL_CPGMAC_SL_MACCONTROL_GMII_EN;

        CSL_CPGMAC_SL_setMacControlReg (macControlVal, portNum);

     @endverbatim
 * =============================================================================
 */
void CSL_CPGMAC_SL_setMacControlReg
(CSL_Xge_cpswRegs *hCpswRegs,
    uint32                  portNum,
    uint32                  macControlRegVal
)
{
    hCpswRegs->ENETPORT[portNum].PN_MAC_CONTROL_REG   =   macControlRegVal;

    return;
}

/** ============================================================================
 *   @n@b CSL_CPGMAC_SL_resetMac
 *
 *   @b Description
 *   @n This function issues a software reset to the MAC for the MAC port number
 *      specified.
 *
 *   @b Arguments
     @verbatim
        portNum             MAC port number for which the reset must be performed.
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
 *   @n XGE_CPSW_PN_MAC_SOFT_RESET_REG_SOFT_RESET=1
 *
 *   @b Example
 *   @verbatim
        uint32 portNum  =   1;

        CSL_CPGMAC_SL_resetMac (portNum);

     @endverbatim
 * =============================================================================
 */
void CSL_CPGMAC_SL_resetMac
(CSL_Xge_cpswRegs *hCpswRegs,
    uint32                      portNum
)
{
    CSL_FINS (hCpswRegs->ENETPORT[portNum].PN_MAC_SOFT_RESET_REG, XGE_CPSW_PN_MAC_SOFT_RESET_REG_SOFT_RESET, 1);

    return;
}

/** ============================================================================
 *   @n@b CSL_CPGMAC_SL_isMACResetDone
 *
 *   @b Description
 *   @n This function reads the MAC Soft Reset register to check if the software
 *      reset operation has completed.
 *
 *   @b Arguments
     @verbatim
        portNum                 MAC port number for which the API must
                                retrieve the settings.
 *   @endverbatim
 *
 *   <b> Return Value </b>
 *   @n  TRUE               MAC Soft reset done.
 *   @n  FALSE              MAC Soft reset not yet completed.
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Reads
 *   @n XGE_CPSW_PN_MAC_SOFT_RESET_REG_SOFT_RESET
 *
 *   @b Example
 *   @verbatim
        uint32  portNum = 1;

        CSL_CPGMAC_SL_resetMac (portNum);

        // Wait until MAC software reset completes
        while (!CSL_CPGMAC_SL_isMACResetDone (portNum));

     @endverbatim
 * =============================================================================
 */
uint32 CSL_CPGMAC_SL_isMACResetDone
(CSL_Xge_cpswRegs *hCpswRegs,
    uint32                      portNum
)
{
    if (CSL_FEXT (hCpswRegs->ENETPORT[portNum].PN_MAC_SOFT_RESET_REG, XGE_CPSW_PN_MAC_SOFT_RESET_REG_SOFT_RESET) == 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/** ============================================================================
 *   @n@b CSL_CPGMAC_SL_setRxMaxLen
 *
 *   @b Description
 *   @n This function sets up the Receive maximum frame length in Receive Maximum
 *      Length register for the MAC port number specified.
 *
 *   @b Arguments
     @verbatim
        portNum             MAC port number for which the settings must be configured.
        rxMaxLen            the Receive maximum frame length to be configered into
                            the Receive Maximum Length register
 *	 @endverbatim
 *
 *   <b> Return Value </b>
 *	 @n	 None
 *
 *   <b> Pre Condition </b>
 *   @n  None
 *
 *   <b> Post Condition </b>
 *   @n  None
 *
 *   @b Writes
 *   @n XGE_CPSW_PN_RX_MAXLEN_REG_RX_MAXLEN
 *
 *   @b Example
 *   @verbatim
        uint32          mtu, portNum = 1;

        mtu =   1518;

        CSL_CPGMAC_SL_setRxMaxLen (portNum, mtu);

     @endverbatim
 * =============================================================================
 */
uint32 CSL_CPGMAC_SL_setRxMaxLen
(CSL_Xge_cpswRegs *hCpswRegs,
    uint32                  portNum,
    uint32                  rxMaxLen
)
{
    return CSL_FINS (hCpswRegs->ENETPORT[portNum].PN_RX_MAXLEN_REG, XGE_CPSW_PN_RX_MAXLEN_REG_RX_MAXLEN, rxMaxLen);
}

/**
@}
*/
