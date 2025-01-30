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
 *  \file     dp83867_priv.h
 *
 *  \brief    This file contains private definitions of the TI DP83867 PHY.
 *  @{
 */

#ifndef DP83867_PRIV_H_
#define DP83867_PRIV_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/*! \brief PHY Control Register (PHYCR) */
#define DP83867_PHYCR                   (0x10U)

/** \brief Configuration Register 3 (CFG3) */
#define DP83867_CFG3                    (0x1EU)

/** \brief Control Register (CTRL) */
#define DP83867_CTRL                    (0x1FU)

/** \brief RGMII Control Register (RGMIICTL) */
#define DP83867_RGMIICTL                (0x32U)

/** \brief RGMII Delay Control Register (RGMIIDCTL) */
#define DP83867_RGMIIDCTL               (0x86U)

/** \brief I/O Configuration Register (IO_MUX_CFG) */
#define DP83867_IOMUXCFG                (0x170U)

/** \brief PHYCR [15:14] TX FIFO Depth mask */
#define DP83867_PHYCR_TXFIFODEPTH_MASK  (0xC000U)

/** \brief PHYCR [15:14] 8-byte/nibbles TX FIFO Depth */
#define DP83867_PHYCR_TXFIFODEPTH_8B    (0xC000U)

/** \brief PHYCR [15:14] 6-byte/nibbles TX FIFO Depth */
#define DP83867_PHYCR_TXFIFODEPTH_6B    (0x8000U)

/** \brief PHYCR [15:14] 4-byte/nibbles TX FIFO Depth */
#define DP83867_PHYCR_TXFIFODEPTH_4B    (0x4000U)

/** \brief PHYCR [15:14] 3-byte/nibbles TX FIFO Depth */
#define DP83867_PHYCR_TXFIFODEPTH_3B    (0x0000U)

/** \brief RGMII enable */
#define DP83867_RGMIICTL_RGMIIEN        (0x0080U)

/** \brief RGMII TX delay enable */
#define DP83867_RGMIICTL_TXCLKDLY       (0x0002U)

/** \brief RGMII RX delay enable */
#define DP83867_RGMIICTL_RXCLKDLY       (0x0001U)

/** \brief RGMII TX delay mask */
#define DP83867_RGMIIDCTL_TXDLYCTRL_MASK   (0x00F0U)

/** \brief RGMII TX delay offset */
#define DP83867_RGMIIDCTL_TXDLYCTRL_OFFSET (4U)

/** \brief RGMII RX delay mask */
#define DP83867_RGMIIDCTL_RXDLYCTRL_MASK   (0x000FU)

/** \brief RGMII RX delay offset */
#define DP83867_RGMIIDCTL_RXDLYCTRL_OFFSET (0U)

/** \brief RGMII max delay (4.00 ns) */
#define DP83867_RGMIIDCTL_DELAY_MAX        (4000U)

/** \brief RGMII delay step (0.25 ns) */
#define DP83867_RGMIIDCTL_DELAY_STEP       (250U)

/** \brief I/O impedance mask */
#define DP83867_IOMUXCFG_IOIMPEDANCE_MASK  (0x001FU)

/** \brief I/O max impedance (35 ohms) */
#define DP83867_IOMUXCFG_IOIMPEDANCE_MIN   (35000U)

/** \brief I/O max impedance (70 ohms) */
#define DP83867_IOMUXCFG_IOIMPEDANCE_MAX   (70000U)

/** \brief I/O impedance range (35 ohms) */
#define DP83867_IOMUXCFG_IOIMPEDANCE_RANGE (DP83867_IOMUXCFG_IOIMPEDANCE_MAX - \
                                            DP83867_IOMUXCFG_IOIMPEDANCE_MIN)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
}
#endif

#endif /* DP83867_PRIV_H_ */

/*! @} */
