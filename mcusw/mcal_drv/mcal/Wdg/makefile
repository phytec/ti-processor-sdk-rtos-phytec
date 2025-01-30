include $(MCUSW_INSTALL_PATH)/build/Rules.make

MODULE_NAME = wdg

SRCDIR = src
INCDIR = src $(mcal_PATH)

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = autosarBSW autosarConfig autosarCompiler

# List all the internal submodules whose interface header files
# need to be included for this module
INCLUDE_INTERNAL_INTERFACES = wdg bsw_stubs

MODULE_SRC_BASE_PATH := $(mcal_PATH)/Wdg

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON += Wdg.c Wdg_Priv.c
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
