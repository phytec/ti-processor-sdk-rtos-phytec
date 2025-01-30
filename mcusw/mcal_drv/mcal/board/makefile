include $(MCUSW_INSTALL_PATH)/build/Rules.make

MODULE_NAME = brd

SRCDIR = flash/src
INCDIR = $(mcal_PATH)
# List all the external components/interfaces, whose interface header files
# need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk autosarBSW autosarConfig autosarCompiler
# List all the internal submodules whose interface header files
# need to be included for this module
INCLUDE_INTERNAL_INTERFACES = fls brd bsw_stubs

MODULE_SRC_BASE_PATH := $(mcal_PATH)/board

# Common source files and CFLAGS across all platforms and cores
ifeq ($(SOC),$(filter $(SOC), j721e j7200 j721s2 j784s4))
SRCS_COMMON += nor_spi_phy_tune.c nor_spi_patterns.c
endif
#SRC_COMMON += phyTuning/nor_spi_phy_tune.c phyTuning/nor_spi_patterns.c
#Fls_Priv.c

SRCS_COMMON += Fls_Brd_Nor.c

# Please modify the following lines if a custom flash device is used 
# and has different protocols than the ones on the EVM
# j721e EVM uses OSPI flash 
ifeq ($(SOC),j721e)
   SRCS_COMMON += Fls_Brd_Nor_Ospi.c
endif

ifeq ($(SOC),$(filter $(SOC), j7200 j721s2 j784s4 am62x am62ax am62px j722s))
   # if SOC = j721s2 and FLS_RROTOCOL = qspi, then user is running qspi on j721s2
   # Therefore, qspi.c file should be used
   # Currently only j721s2 supports qspi
   ifeq ($(SOC) $(FLS_PROTOCOL),j721s2 qspi)
      SRCS_COMMON += Fls_Brd_Nor_Qspi.c
   # Used for internal unit testing
   else ifeq ($(SOC) $(MCAL_CONFIG),j721s2 4)
      SRCS_COMMON += Fls_Brd_Nor_Qspi.c
   # all non j721e devices has xspi flash device
   else
      SRCS_COMMON += Fls_Brd_Nor_Xspi.c
   endif
endif


CFLAGS_LOCAL_COMMON = $(MCUSW_CFLAGS)

# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
include $(MCUSW_INSTALL_PATH)/build/common.mk
# OBJs and libraries are built by using rule defined in rules_<target>.mk
#     and need not be explicitly specified here

# Nothing beyond this point
