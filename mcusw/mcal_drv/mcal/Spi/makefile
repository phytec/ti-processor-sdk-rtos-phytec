include $(MCUSW_INSTALL_PATH)/build/Rules.make

MODULE_NAME = spi

SRCDIR = src $(mcal_PATH) $(mcal_PATH)/Spi/mcspi_hw/V0
INCDIR += src $(mcal_PATH)

# List all the external components/interfaces, whose interface header files
# need to be included for this component
INCLUDE_EXTERNAL_INTERFACES += autosarBSW autosarConfig autosarCompiler
ifeq ($(SOC),$(filter $(SOC), j7200 j721e j721s2 j784s4))
INCLUDE_EXTERNAL_INTERFACES += pdk
endif

# List all the internal submodules whose interface header files
# need to be included for this module
INCLUDE_INTERNAL_INTERFACES = spi bsw_stubs

MODULE_SRC_BASE_PATH := $(mcal_PATH)/Spi

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON += Spi.c Spi_Priv.c Spi_Utils.c Spi_Irq.c Spi_Mcspi.c Spi_Dma.c mcspi.c

PACKAGE_SRCS_COMMON = .
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
