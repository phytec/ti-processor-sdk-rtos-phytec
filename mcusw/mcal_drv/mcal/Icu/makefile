include $(MCUSW_INSTALL_PATH)/build/Rules.make

MODULE_NAME = icu
ifeq ($(SOC),$(filter $(SOC), j7200 j721e j721s2 j784s4))

SRCDIR = src $(mcal_PATH)/Icu/ecap_hw/V0
INCDIR =  src $(mcal_PATH)
INCDIR += ../examples/Icu/icu_app/Icu_Demo_Cfg/output/generated/soc/$(SOC)/$(CORE)/include
SRCDIR += ../example/Icu/icu_app/Icu_Demo_Cfg/output/generated/soc/$(SOC)/$(CORE)/src
else
#other SOCs Requirement
endif
# List all the external components/interfaces, whose interface header files
# need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = autosarBSW autosarConfig autosarCompiler
# List all the internal submodules whose interface header files
# need to be included for this module
INCLUDE_INTERNAL_INTERFACES = icu bsw_stubs

MODULE_SRC_BASE_PATH := $(mcal_PATH)/Icu

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON += Icu.c Icu_Priv.c Icu_Irq.c
ifeq ($(SOC),$(filter $(SOC), j7200 j721e j721s2 j784s4))
SRCS_COMMON += ecap.c
else
#other SOCs Requirement
endif

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
