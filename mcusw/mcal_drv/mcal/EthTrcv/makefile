include $(MCUSW_INSTALL_PATH)/build/Rules.make

MODULE_NAME = ethtrcv

SRCDIR = src
INCDIR =

ifeq ($(ETH_SPECIFIC_CONFIG_PATH), )
# Configuration Variants
# - By default precompile configuration is used and is already included
#   in INCLUDE_EXTERNAL_INTERFACES = autosarConfig
ifeq ($(BUILD_VARIANT), linktime)
  INCDIR += $(autosarConfigSrc_PATH)/Eth_Demo_Lcfg/output/generated/soc/$(SOC)/$(CORE)/include
else ifeq ($(BUILD_VARIANT), postbuild)
  INCDIR += $(autosarConfigSrc_PATH)/Eth_Demo_PBcfg/output/generated/soc/$(SOC)/$(CORE)/include
endif
else
  INCDIR += $(ETH_SPECIFIC_CONFIG_PATH)/include
endif

# List all the external components/interfaces, whose interface header files
# need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk autosarBSW autosarConfig autosarCompiler

# List all the internal submodules whose interface header files
# need to be included for this module
INCLUDE_INTERNAL_INTERFACES = eth ethtrcv bsw_stubs
ifeq ($(MCUSW_ETH_WRAPPER_ENABLE),TRUE)
  INCLUDE_INTERNAL_INTERFACES += eth_wrapper
endif

MODULE_SRC_BASE_PATH := $(mcal_PATH)/EthTrcv

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON += EthTrcv.c EthTrcv_Priv.c

PACKAGE_SRCS_COMMON = .
CFLAGS_LOCAL_COMMON = $(MCUSW_CFLAGS)
ifeq ($(MCUSW_ETH_WRAPPER_ENABLE),TRUE)
  CFLAGS_LOCAL_COMMON += -DETH_WRAPPER_ENABLED=STD_ON
else
  CFLAGS_LOCAL_COMMON += -DETH_WRAPPER_ENABLED=STD_OFF
endif

# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
include $(MCUSW_INSTALL_PATH)/build/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk
#     and need not be explicitly specified here

# Nothing beyond this point
