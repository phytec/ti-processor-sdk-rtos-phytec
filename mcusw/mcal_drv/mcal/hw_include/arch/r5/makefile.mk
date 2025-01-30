
include $(MCUSW_INSTALL_PATH)/build/Rules.make

MODULE_NAME = hw_init

ifeq ($(MAKERULEDIR), )
#Makerule path not defined, define this and assume relative path from ROOTDIR
  MAKERULEDIR := $(MCUSW_INSTALL_PATH)/build
  export MAKERULEDIR
endif
include $(MCUSW_INSTALL_PATH)/build/platform.mk

#Accommodating Changes for introducing multiple Tool Chains
SRCDIR = src $(mcal_PATH)/hw_include
INCDIR = .  $(mcal_PATH) $(mcal_PATH)/hw_include

ifeq ($(SOC),$(filter $(SOC), am62x j722s))
INCDIR += . $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/sciclient/include
INCDIR += . $(mcu_plus_sdk_INSTALL_PATH)/source/kernel/dpl
INCDIR += . $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/sciclient
INCDIR += . $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/sciclient/include
INCDIR += . $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/drivers/hw_include
INCDIR += . $(mcu_plus_sdk_INSTALL_PATH)/source/
INCDIR += $(mcu_plus_sdk_INSTALL_PATH)/mcu_plus_sdk
endif
# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = autosarCompiler #pdk

# List all the internal submodules whose interface header files
# need to be included for this module
INCLUDE_INTERNAL_INTERFACES =

# Common source files and CFLAGS across all platforms and cores

#Include src files for only the modules defined under csl_intc_COMP_LIST
SRCS_COMMON =

PACKAGE_SRCS_COMMON += makefile_init.mk
PACKAGE_SRCS_COMMON += src_files_arch.mk
PACKAGE_SRCS_COMMON += src_files_r5.mk

SRCS_COMMON +=   interrupt.c lld_r5.c lld_vim.c lld_cache.c
SRCS_ASM_COMMON += lld_arm_r5.asm

CFLAGS_LOCAL_COMMON = $(MCUSW_CFLAGS)
LNKFLAGS_LOCAL_COMMON = $(MCUSW_LNKFLAGS)
# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
  MAKERULEDIR := $(PDK_INSTALL_PATH)/build
  export MAKERULEDIR
include $(MCUSW_INSTALL_PATH)/build/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk
#     and need not be explicitly specified here

# Nothing beyond this point
