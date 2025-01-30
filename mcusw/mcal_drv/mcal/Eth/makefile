include $(MCUSW_INSTALL_PATH)/build/Rules.make

MODULE_NAME = eth

SRCDIR = src \
         src/cpsw \
         src/udma

INCDIR = src \
         src/cpsw/include \
         src/csl \
         src/udma

# Use CSL stubs for some unit test config
ifeq ($(MCAL_CONFIG),$(filter $(MCAL_CONFIG), 2 3))
	SRCDIR += $(mcal_test_PATH)/eth_test/testLib/stubs/csl
	INCDIR += $(mcal_test_PATH)/eth_test
else
	SRCDIR += src/csl
endif

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
INCLUDE_EXTERNAL_INTERFACES = autosarBSW autosarConfig autosarCompiler

# List all the internal submodules whose interface header files
# need to be included for this module
INCLUDE_INTERNAL_INTERFACES = eth ethtrcv bsw_stubs
INCDIR += $(mcal_PATH)
ifeq ($(SOC),$(filter $(SOC), j721e j7200 j784s4))
  INCLUDE_INTERNAL_INTERFACES += cdd_ipc
endif

MODULE_SRC_BASE_PATH := $(mcal_PATH)/Eth

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON += Eth.c \
               Eth_Irq.c \
               Eth_Priv.c \
               Eth_Packet.c \
               Eth_IrqPacing.c

ifeq ($(SOC),$(filter $(SOC), j721e j7200 j784s4))
SRCS_COMMON += Eth_Rpc.c \
               Eth_VirtMac.c
endif

# CPSW Helper files
SRCS_COMMON += Cpsw_Dma.c \
               Cpsw_Port.c \
               Cpsw_Mac.c \
               Cpsw_Stats.c \
               Cpsw_Mdio.c \
               Cpsw_Ale.c \
               Cpsw_Cpts.c

# UDMA Helper files
SRCS_COMMON += Udma_Ring_LL.c \
               Udma_LL.c

# CSL Helper files
SRCS_COMMON += csl_cpsw.c \
               csl_cpgmac_sl.c \
               csl_mdio.c \
               csl_cpts.c

ifeq ($(SOC),$(filter $(SOC), j721e j7200 j721s2 j784s4))
SRCS_COMMON += csl_cpsw_ale_4g_tblcfg.c
else ifeq ($(SOC),$(filter $(SOC), am62x am62ax am62px j722s))
SRCS_COMMON += csl_cpsw_ale_9g_tblcfg.c
endif

PACKAGE_SRCS_COMMON = .
CFLAGS_LOCAL_COMMON = $(MCUSW_CFLAGS)
ifeq ($(MCUSW_ETH_WRAPPER_ENABLE),TRUE)
  CFLAGS_LOCAL_COMMON += -DETH_WRAPPER_ENABLED=STD_ON
  INCLUDE_INTERNAL_INTERFACES += eth_wrapper
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
