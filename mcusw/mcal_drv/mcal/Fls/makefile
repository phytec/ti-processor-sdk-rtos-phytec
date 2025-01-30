include $(MCUSW_INSTALL_PATH)/build/Rules.make

MODULE_NAME = fls

ifeq ($(SOC),$(filter $(SOC), j7200 j721e j721s2 j784s4 am62x am62ax am62px j722s))
SRCDIR += . src $(mcal_PATH)/Fls/fls_hw/ospi/V0/priv
INCDIR += .src $(mcal_PATH)
PACKAGE_SRCS_COMMON = .
else
SRCDIR = . src $(mcal_PATH)
INCDIR =  src $(mcal_PATH)
endif

# List all the external components/interfaces, whose interface header files
# need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk autosarBSW autosarConfig autosarCompiler
# List all the internal submodules whose interface header files
# need to be included for this module
INCLUDE_INTERNAL_INTERFACES = fls brd bsw_stubs

MODULE_SRC_BASE_PATH := $(mcal_PATH)/Fls

# Common source files and CFLAGS across all platforms and cores

SRCS_COMMON += Fls.c Fls_Ospi.c Fls_Irq.c
#SRC_COMMON += phyTuning/nor_spi_phy_tune.c phyTuning/nor_spi_patterns.c
#Fls_Priv.c
ifeq ($(SOC),$(filter $(SOC),j721e j7200 j721s2 j784s4 am62x am62ax am62px j722s))
SRCS_COMMON += lld_ospi.c
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
