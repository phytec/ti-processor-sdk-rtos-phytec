#*******************************************************************************
#                                                                              *
# Copyright (c) 2021 Texas Instruments Incorporated - http://www.ti.com/       *
#                        ALL RIGHTS RESERVED                                   *
#                                                                              *
#*******************************************************************************

# Filename: platform.mk
#
# Platforms make file - Platform/SoC/targets are defined/derived in this file
#
# This file needs to change when:
#     1. a new platform/SoC is added, which also might have its own cores/ISAs
#

#
# Derive SOCFAMILY from SOC
#

# J721E
ifeq ($(SOC),$(filter $(SOC), j721e))
SOCFAMILY = j721e
BOARD = j721e_evm
endif

# J7200
ifeq ($(SOC),$(filter $(SOC), j7200))
SOCFAMILY = j7200
BOARD = j7200_evm
endif

# J721S2
ifeq ($(SOC),$(filter $(SOC), j721s2))
SOCFAMILY = j721s2
BOARD = j721s2_evm
endif

# J784S4
ifeq ($(SOC),$(filter $(SOC), j784s4))
SOCFAMILY = j784s4
BOARD = j784s4_evm
endif

# Derive Target/ISA from CORE

# r5f
ifeq ($(CORE),$(filter $(CORE), r5f))
 ISA = r5f
 ISA_EXT = r5f
 ARCH = armv7m
endif

#
# Derive XDC/ISA specific settings
#
ifeq ($(FORMAT),ELF)
  FORMAT_EXT = e
endif
# If ENDIAN is set to "big", set ENDIAN_EXT to "e", that would be used in
#    in the filename extension of object/library/executable files
ifeq ($(ENDIAN),big)
  ENDIAN_EXT = e
endif

# J721E
ifeq ($(BOARD),$(filter $(BOARD), j721e_sim j721e_hostemu j721e_ccqt j721e_loki j721e_qt j721e_vhwazebu j721e_evm))
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
ifeq ($(BOARD),$(filter $(BOARD), j721s2_evm))
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

# J721E, J7200 J721S2 and J784S4 support for mcu1_0
ifeq ($(SOC),$(filter $(SOC), j721e j7200 j721s2 j784s4))
  SBL_CORE_ID_r5f = 8
  SBL_CORE_ID_r5f_smp = 27
endif

# Define the file extensions
OBJEXT = o$(FORMAT_EXT)$(ISA)$(ENDIAN_EXT)
LIBEXT = a$(FORMAT_EXT)$(ISA)$(ENDIAN_EXT)
EXEEXT = x$(FORMAT_EXT)$(ISA)$(ENDIAN_EXT)
ASMEXT = s$(FORMAT_EXT)$(ISA)$(ENDIAN_EXT)

export SOC
export SOCFAMILY

# Nothing beyond this point
