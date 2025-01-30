#
# Copyright (c) 2013-2022, Texas Instruments Incorporated
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# *  Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#
# *  Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# *  Neither the name of Texas Instruments Incorporated nor the names of
#    its contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
# EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

# Filename: platform.mk
#
# Platforms make file - Board/SoC/targets are defined/derived in this file
#
# This file needs to change when:
#     1. a new Board/SoC is added, which also might have its own cores/ISAs
#

#
# Derive SOC from BOARD
#
# TDA2XX
ifeq ($(BOARD),$(filter $(BOARD), tda2xx-evm tda2xx-mc tda2xx-evm-radar tda2xx-rvp tda2xx-cascade-radar))
 SOC = tda2xx
 SBL_RUN_ADDRESS=0x40300000
 SBL_DEV_ID=55
endif

# TDA2PX
ifeq ($(BOARD),$(filter $(BOARD), tda2px-evm tda2px-evm-radar))
 SOC = tda2px
 SBL_RUN_ADDRESS=0x40300000
 SBL_DEV_ID=88
endif

# DRA75X
ifeq ($(BOARD),$(filter $(BOARD), evmDRA75x dra75x))
 SOC = dra75x
 SBL_RUN_ADDRESS=0x40300000
 SBL_DEV_ID=55
endif

# AM572X
ifeq ($(BOARD),$(filter $(BOARD), am572x-evm am572x-idk evmAM572x idkAM572x))
 SOC = am572x
endif

# AM574X
ifeq ($(BOARD),$(filter $(BOARD), am574x-idk idkAM574x))
 SOC = am574x
endif

# TDA2EX
ifeq ($(BOARD),$(filter $(BOARD), tda2ex-evm tda2ex-eth-srv))
 SOC = tda2ex
 SBL_RUN_ADDRESS=0x40300000
 SBL_DEV_ID=66
endif

# DRA72x
ifeq ($(BOARD),$(filter $(BOARD), evmDRA72x dra72x))
 SOC = dra72x
 SBL_RUN_ADDRESS=0x40300000
 SBL_DEV_ID=66
endif

# AM571X
ifeq ($(BOARD),$(filter $(BOARD), am571x-evm  idkAM571x))
 SOC = am571x
endif

# TDA3XX
ifeq ($(BOARD),$(filter $(BOARD), tda3xx-evm tda3xx-rvp tda3xx-evm-radar tda3xx-ar12-booster tda3xx-ar12-alps tda3xx-ar12-rvp))
 SOC = tda3xx
 SBL_RUN_ADDRESS=0x00300000
 SBL_DEV_ID=77
endif

# DRA78X
ifeq ($(BOARD),$(filter $(BOARD), evmDRA78x))
 SOC = dra78x
 SBL_RUN_ADDRESS=0x00300000
 SBL_DEV_ID=77
endif

# K2H
ifeq ($(BOARD),$(filter $(BOARD),  evmK2H))
 SOC = k2h
endif

# K2K
ifeq ($(BOARD),$(filter $(BOARD),  evmK2K))
 SOC = k2k
endif

# K2L
ifeq ($(BOARD),$(filter $(BOARD),  evmK2L))
 SOC = k2l
endif

# K2E
ifeq ($(BOARD),$(filter $(BOARD),  evmK2E))
 SOC = k2e
endif

# C6657
ifeq ($(BOARD),$(filter $(BOARD),  evmC6657))
 SOC = c6657
endif

# C6678
ifeq ($(BOARD),$(filter $(BOARD),  evmC6678))
 SOC = c6678
endif

# k2g
ifeq ($(BOARD),$(filter $(BOARD),  evmK2G iceK2G))
 SOC = k2g
endif

# am335x
ifeq ($(BOARD),$(filter $(BOARD),  evmAM335x icev2AM335x iceAMIC110 skAM335x bbbAM335x))
 SOC = am335x
endif

# am437x
ifeq ($(BOARD),$(filter $(BOARD),  evmAM437x idkAM437x skAM437x))
 SOC = am437x
endif

# omapl137
ifeq ($(BOARD),$(filter $(BOARD),  evmOMAPL137))
 SOC = omapl137
endif

# omapl138
ifeq ($(BOARD),$(filter $(BOARD),  lcdkOMAPL138))
 SOC = omapl138
endif

# AM65xx
# SBL_DEV_ID must match the value defined in function GetDeviceId
# in sbl/src/rprc/sbl_rprc.c
ifeq ($(BOARD),$(filter $(BOARD), am65xx_sim am65xx_evm am65xx_idk))
 SOC = am65xx
 SBL_RUN_ADDRESS=0x41C00100
 SBL_DEV_ID=55
endif

# J721E
ifeq ($(BOARD),$(filter $(BOARD), j721e_sim j721e_hostemu j721e_ccqt j721e_loki j721e_qt j721e_evm))
 SOC = j721e
 SBL_RUN_ADDRESS=0x41C00100
 SBL_DEV_ID=55
endif

# J7200
ifeq ($(BOARD),$(filter $(BOARD), j7200_sim j7200_hostemu j7200_evm))
 SOC = j7200
 SBL_RUN_ADDRESS=0x41C00100
 SBL_DEV_ID=55
endif

# J721S2
ifeq ($(BOARD),$(filter $(BOARD), j721s2_evm j721s2_hostemu))
 SOC = j721s2
 SBL_RUN_ADDRESS=0x41C00100
 SBL_DEV_ID=55
endif

# J784S4
ifeq ($(BOARD),$(filter $(BOARD), j784s4_evm j784s4_hostemu))
 SOC = j784s4
 SBL_RUN_ADDRESS=0x41C00100
 SBL_DEV_ID=55
endif


# SBL related macro
export SBL_CERT_KEY_HS=$(ROOTDIR)/ti/build/makerules/k3_dev_mpk.pem
export SBL_CERT_KEY=$(ROOTDIR)/ti/build/makerules/rom_degenerateKey.pem


#
# Derive Target/ISA from CORE
#

# a8host
ifeq ($(CORE),a8host)
 ISA = a8
 ISA_EXT = a8
 ARCH = armv7a
endif

# a9host
ifeq ($(CORE),a9host)
 ISA = a9
 ISA_EXT = a9
 ARCH = armv7a
endif

# arm9_0
ifeq ($(CORE),arm9_0)
 ISA = arm9
 ISA_EXT = ae9
 ARCH = armv5
endif

# pru_0 and pru_1
ifeq ($(CORE),$(filter $(CORE), pru_0 pru_1))
 ISA = pru
 ISA_EXT = pru
 ifeq ($(CORE),pru_0)
  ARCH = PRU0
 else
  ARCH = PRU1
 endif
endif

# a15_0
ifeq ($(CORE),a15_0)
 ISA = a15
 ISA_EXT = a15
 ARCH = armv7a
endif

# ipu1_0/ipu1_1/ipu2_0/ipu2_1
ifeq ($(CORE),$(filter $(CORE), ipu1_0 ipu1_1 ipu2_0 ipu2_1))
 ISA = m4
 ISA_EXT = m4
 ARCH = armv7m
endif

# m4f single core
ifeq ($(CORE),$(filter $(CORE), m4f_0))
 ISA = m4f
 ISA_EXT = m4f
 ARCH = armv7m
endif

# m3
ifeq ($(CORE),$(filter $(CORE), m3))
 ISA = m3
 ISA_EXT = m3
 ARCH = armv7m
endif

# EVE
ifeq ($(CORE),$(filter $(CORE), arp32_1 arp32_2 arp32_3 arp32_4))
 ISA = arp32
 ISA_EXT = arp32
 ARCH = arp32
endif

# MCU
ifeq ($(CORE),$(filter $(CORE), mcu0_0 mcu1_0 mcu1_1 mcu2_0 mcu2_1 mcu3_0 mcu3_1 mcu4_0 mcu4_1))
 ISA = r5f
 ISA_EXT = r5f
 ARCH = armv7r
endif

# MPU
ifeq ($(CORE),$(filter $(CORE), mpu1_0 mpu1_1 mpu1_2 mpu1_3 mpu2_0 mpu2_1 mpu2_2 mpu2_3))
  ifeq ($(SOC),$(filter $(SOC), j721e j7200 j721s2 j784s4))
    ISA = a72
    ISA_EXT = a72
    ARCH = armv8a
  endif
endif


# C7x DSP
ifeq ($(CORE),$(filter $(CORE), c7x_1 c7x_2 c7x_3 c7x_4))
 ISA = c7x
 ISA_EXT = 71
 ARCH = c71
 ifeq ($(SOC),$(filter $(SOC), j721e)) 
  SI_VER = 7100
 endif
 ifeq ($(SOC),$(filter $(SOC), j721s2 j784s4)) 
  SI_VER = 7120
 endif
endif

# C7x DSP
ifeq ($(CORE),$(filter $(CORE), c7x-hostemu))
 ISA = c7x-hostemu
 ISA_EXT = c7x-hostemu
 ARCH = c7x
 ifeq ($(SOC),$(filter $(SOC), j721e)) 
  SI_VER = 7100
 endif
 ifeq ($(SOC),$(filter $(SOC), j721s2 j784s4)) 
  SI_VER = 7120
 endif
endif

# DSP for tda2xx/tda2px/am572x/tda2ex/dra72x/dra75x/am571x/tda3xx/dra78x/k2h/k2k/k2l/k2e/c6657/c6678/am574x/tpr12/awr294x
ifeq ($(CORE),$(filter $(CORE), c66x c66xdsp_1 c66xdsp_2))
 ISA = c66
 ISA_EXT = 66
 ARCH = c66x
endif

# C67x DSP
ifeq ($(CORE),c67x)
 ISA = c674
 ISA_EXT = 674
 ARCH = c67x
endif

# C674x DSP
ifeq ($(CORE),c674x)
 ISA = c674
 ISA_EXT = 674
 ARCH = c67x
endif

# a8host
ifeq ($(CORE),a8host)
 ISA = a8
 ISA_EXT = a8
 ARCH = armv7a
endif

#
# Derive ISA specific settings
#

ifeq ($(ISA),a15)

  ENDIAN_EXT = fg
  FORMAT_EXT =

  # Define the file extensions
  OBJEXT = o$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  LIBEXT = a$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  EXEEXT = x$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  ASMEXT = s$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
endif

ifeq ($(ISA),$(filter $(ISA), m4 m4f))
  ifeq ($(FORMAT),ELF)
    FORMAT_EXT = e
  endif

  # If ENDIAN is set to "big", set ENDIAN_EXT to "e", that would be used in
  #    in the filename extension of object/library/executable files
  ifeq ($(ENDIAN),big)
    ENDIAN_EXT = e
  endif

  # Define the file extensions
  OBJEXT = o$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  LIBEXT = a$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  EXEEXT = x$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  ASMEXT = s$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
endif

ifeq ($(ISA),r5f)
  ifeq ($(FORMAT),ELF)
    FORMAT_EXT = e
  endif

  # If ENDIAN is set to "big", set ENDIAN_EXT to "e", that would be used in
  #    in the filename extension of object/library/executable files
  ifeq ($(ENDIAN),big)
    ENDIAN_EXT = e
  endif

  # Define the file extensions
  OBJEXT = o$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  # Retaining the r5f extension for backward compatibility
  LIBEXT = a$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  EXEEXT = x$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  ASMEXT = s$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
endif

ifeq ($(ISA),a53)

  ENDIAN_EXT = fg
  FORMAT_EXT =

  # If ENDIAN is set to "big", set ENDIAN_EXT to "e", that would be used in
  #    in the filename extension of object/library/executable files
  ifeq ($(ENDIAN),big)
    ENDIAN_EXT = e
  endif

  # Define the file extensions
  OBJEXT = o$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  LIBEXT = a$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  EXEEXT = x$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  ASMEXT = s$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
endif

ifeq ($(ISA),a72)

  ENDIAN_EXT = fg
  FORMAT_EXT =

  # If ENDIAN is set to "big", set ENDIAN_EXT to "e", that would be used in
  #    in the filename extension of object/library/executable files
  ifeq ($(ENDIAN),big)
    ENDIAN_EXT = e
  endif

  # Define the file extensions
  OBJEXT = o$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  LIBEXT = a$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  EXEEXT = x$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  ASMEXT = s$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
endif


ifeq ($(ISA),m3)
  ifeq ($(FORMAT),ELF)
    FORMAT_EXT = e
  endif

  # If ENDIAN is set to "big", set ENDIAN_EXT to "e", that would be used in
  #    in the filename extension of object/library/executable files
  ifeq ($(ENDIAN),big)
    ENDIAN_EXT = e
  endif

  # Define the file extensions
  OBJEXT = o$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  LIBEXT = a$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  EXEEXT = x$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  ASMEXT = s$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
endif

#########C7x DSP Support############
ifeq ($(ISA),c7x)
  ifeq ($(FORMAT),ELF)
    FORMAT_EXT = e
  endif

  # If ENDIAN is set to "big", set ENDIAN_EXT to "e", that would be used in
  #    in the filename extension of object/library/executable files
  ifeq ($(ENDIAN),big)
    ENDIAN_EXT = e
  endif

  # Define the file extensions
  OBJEXT = o$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  LIBEXT = a$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  EXEEXT = x$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  ASMEXT = s$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
endif

#########C7x Host Support############
ifeq ($(ISA),c7x-hostemu)
  # Define the file extensions
  OBJEXT = obj
  LIBEXT = lib
  EXEEXT = exe
endif

#########DSP Support############
ifeq ($(ISA),c66)
  ifeq ($(FORMAT),ELF)
    FORMAT_EXT = e

  endif

  # If ENDIAN is set to "big", set ENDIAN_EXT to "e", that would be used in
  #    in the filename extension of object/library/executable files
  ifeq ($(ENDIAN),big)
    ENDIAN_EXT = e
  endif

  # Define the file extensions
  OBJEXT = o$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  LIBEXT = a$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  EXEEXT = x$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  ASMEXT = s$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
endif

ifeq ($(ISA),arp32)
  ifeq ($(FORMAT),ELF)
    ENDIAN_EXT = F
    FORMAT_EXT = e
  endif

  # If ENDIAN is set to "big", set ENDIAN_EXT to "e", that would be used in
  #    in the filename extension of object/library/executable files
  ifeq ($(ENDIAN),big)
    ENDIAN_EXT = e
  endif

  # Define the file extensions
  OBJEXT = o$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  LIBEXT = a$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  EXEEXT = x$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  ASMEXT = s$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
endif

ifeq ($(ISA),a8)

  ENDIAN_EXT = fg
  FORMAT_EXT =

  # Define the file extensions
  OBJEXT = o$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  LIBEXT = a$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  EXEEXT = x$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  ASMEXT = s$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
endif

ifeq ($(ISA),pru)

  ENDIAN_EXT =
  FORMAT_EXT =

  # Define the file extensions
  OBJEXT = o$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  HEADEREXT = h$(FORMAT_EXT)$(ENDIAN_EXT)
  OUTEXT = out
endif

ifeq ($(ISA),a9)

  ENDIAN_EXT = fg
  FORMAT_EXT =

  # Define the file extensions
  OBJEXT = o$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  LIBEXT = a$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  EXEEXT = x$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  ASMEXT = s$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
endif

ifeq ($(ISA),arm9)
  FORMAT_EXT =

  # Define the file extensions
  OBJEXT = o$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  LIBEXT = $(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  EXEEXT = x$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  ASMEXT = s$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
endif

#########DSP Support - c67xx ############
ifeq ($(ISA),c674)
  ifeq ($(FORMAT),ELF)
    FORMAT_EXT = e
  endif

  # Define the file extensions
  OBJEXT = o$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  LIBEXT = a$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  EXEEXT = x$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
  ASMEXT = s$(FORMAT_EXT)$(ISA_EXT)$(ENDIAN_EXT)
endif


ifeq ($(SOC),$(filter $(SOC), j7200 j721e j721s2 j784s4))
  SBL_CORE_ID_mpu1_0 = 0
  SBL_CORE_ID_mpu1_1 = 1
  SBL_CORE_ID_mpu1_2 = 2
  SBL_CORE_ID_mpu1_3 = 3
  SBL_CORE_ID_mpu2_0 = 4 
  SBL_CORE_ID_mpu2_1 = 5
  SBL_CORE_ID_mpu2_2 = 6
  SBL_CORE_ID_mpu2_3 = 7
  SBL_CORE_ID_mcu1_0 = 8
  SBL_CORE_ID_mcu1_1 = 9
  SBL_CORE_ID_mcu2_0 = 10
  SBL_CORE_ID_mcu2_1 = 11
  SBL_CORE_ID_mcu3_0 = 12
  SBL_CORE_ID_mcu3_1 = 13
  SBL_CORE_ID_mcu4_0 = 14
  SBL_CORE_ID_mcu4_1 = 15
  SBL_CORE_ID_c66xdsp_1 = 16 
  SBL_CORE_ID_c66xdsp_2 = 17
  SBL_CORE_ID_c7x_1 = 18
  SBL_CORE_ID_c7x_2 = 19
  SBL_CORE_ID_c7x_3 = 20
  SBL_CORE_ID_c7x_4 = 21
  SBL_CORE_ID_c7x-hostemu = 22
  SBL_CORE_ID_hsm = 23
  SBL_CORE_ID_mpu1_smp = 24
  SBL_CORE_ID_mpu2_smp = 25
  SBL_CORE_ID_mpu_smp = 26
  SBL_CORE_ID_mcu1_0_smp = 27
  SBL_CORE_ID_mcu2_0_smp = 28
  SBL_CORE_ID_mcu3_0_smp = 29
  SBL_CORE_ID_mcu4_0_smp = 30
  SBL_CORE_ID_load_only = 31
endif

export SOC
export ARCH

# Nothing beyond this point