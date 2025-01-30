include $(MCUSW_INSTALL_PATH)/build/Rules.make

MODULE_NAME = dio

SRCDIR = src
INCDIR = $(mcal_PATH)

# List all the external components/interfaces, whose interface header files
# need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = autosarBSW autosarConfig autosarCompiler
# List all the internal submodules whose interface header files
# need to be included for this module
INCLUDE_INTERNAL_INTERFACES = dio bsw_stubs

MODULE_SRC_BASE_PATH := $(mcal_PATH)/Dio

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON += Dio.c Dio_Platform.c

PACKAGE_SRCS_COMMON = .
CFLAGS_LOCAL_COMMON = $(MCUSW_CFLAGS)

# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
  MAKERULEDIR := $(MCUSW_INSTALL_PATH)/build/makerules
  export MAKERULEDIR

include $(MCUSW_INSTALL_PATH)/build/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk
#     and need not be explicitly specified here

# Nothing beyond this point
