#
#
# Copyright (c) 2024 Texas Instruments Incorporated
#
# All rights reserved not granted herein.
#
# Limited License.
#
# Texas Instruments Incorporated grants a world-wide, royalty-free, non-exclusive
# license under copyrights and patents it now or hereafter owns or controls to make,
# have made, use, import, offer to sell and sell ("Utilize") this software subject to the
# terms herein.  With respect to the foregoing patent license, such license is granted
# solely to the extent that any such patent is necessary to Utilize the software alone.
# The patent license shall not apply to any combinations which include this software,
# other than combinations with devices manufactured by or for TI ("TI Devices").
# No hardware patent is licensed hereunder.
#
# Redistributions must preserve existing copyright notices and reproduce this license
# (including the above copyright notice and the disclaimer and (if applicable) source
# code license limitations below) in the documentation and/or other materials provided
# with the distribution
#
# Redistribution and use in binary form, without modification, are permitted provided
# that the following conditions are met:
#
# *       No reverse engineering, decompilation, or disassembly of this software is
# permitted with respect to any software provided in binary form.
#
# *       any redistribution and use are licensed by TI for use only with TI Devices.
#
# *       Nothing shall obligate TI to provide you with source code for the software
# licensed and provided to you in object code.
#
# If software source code is provided to you, modification and redistribution of the
# source code are permitted provided that the following conditions are met:
#
# *       any redistribution and use of the source code, including any resulting derivative
# works, are licensed by TI for use only with TI Devices.
#
# * any redistribution and use of any object code compiled from the source code
# and any resulting derivative works, are licensed by TI for use only with TI Devices.
#
# Neither the name of Texas Instruments Incorporated nor the names of its suppliers
#
# may be used to endorse or promote products derived from this software without
# specific prior written permission.
#
# DISCLAIMER.
#
# THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
# OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
# OF THE POSSIBILITY OF SUCH DAMAGE.
#

################################################################################
# Build Environment (Edit Below)
# Default build environment, Supported options (Windows_NT / linux)
################################################################################
export OS ?= linux

################################################################################
# MCUSW Configurations (Edit Below)
################################################################################
# Select to build MCAL & its example applications (baremetal)
# Select to MCUSS demo applications (freertos)
# Possible options (freertos/baremetal)
export BUILD_OS_TYPE ?= baremetal

# Utilities directory. This is required only if the build machine is Windows.
#   - specify the installation directory of utility which supports POSIX commands
#     (eg: Cygwin installation or MSYS installation).
# This could be in CCS install directory as in c:/ti/ccsv7/utils/cygwin or
# the XDC install bin folder $(XDC_INSTALL_PATH)/bin
ifeq ($(OS),Windows_NT)
  export utils_PATH ?= $(XDC_INSTALL_PATH)/bin
endif

# Enable MCUSW to log messages in console (UART) or CCS console
export MCUSW_UART_ENABLE ?= TRUE


export SDK_INSTALL_PATH  ?= $(abspath ../../)


################################################################################
# MCUSW Configurations Ends
################################################################################

TOOLS_INSTALL_PATH ?=  $(SDK_INSTALL_PATH)
CCS_INSTALL_PATH ?=  $(TOOLS_INSTALL_PATH)/ccs1220

# Default board
# Supported values are printed in "make -s help" option.
#
# default board and soc
export BOARD ?= j721e_evm
export SOC   ?= j721e

#Default AUTOSAR Version
AUTOSAR_VERSION ?= 431

################################################################################
# Other user configurable variables
################################################################################

# Default to r5 build depending on BOARD selected!!
ifeq ($(BOARD),$(filter $(BOARD), j721e_sim j721e_evm j7200_evm j721s2_evm j784s4_evm))
  CORE ?= mcu1_0
endif
ifeq ($(BOARD),$(filter $(BOARD), am62x_evm am62ax_evm am62px_evm j722s_evm))
  CORE ?= mcu0_0
endif
export CORE

# Default to jacinto build!!
ifeq ($(SOC),$(filter $(SOC), j721e j7200 j721s2 j784s4))
  device_family ?= JACINTO_FAM
endif
ifeq ($(SOC),$(filter $(SOC), am62x am62ax am62px j722s))
  device_family ?= SMPU_FAM
endif
export device_family
# Default Build Profile
# Supported Values: debug | release
export BUILD_PROFILE ?= release

# Default Build Variant
# Supported Values: precompile | linktime | postbuild
export BUILD_VARIANT ?= precompile

# Default compile mode for R5 is thumb
# Supported Values: thumb
export COMPILE_MODE ?= thumb

# Treat compiler warning as error
# Supported Values: yes | no
export TREAT_WARNINGS_AS_ERROR ?= yes

################################################################################
# Configure toolchain paths
################################################################################

# J72x common tool versions
  CGT_ARM_VERSION=20.2.0.LTS
   CGT_ARMLLVM_VERSION=3.2.1.LTS
  CG_XML_VERSION=2_61_00
  GCC_ARCH64_VERSION=9.2-2019.12
  XDC_VERSION=3_62_01_16_core
ifeq ($(BOARD),$(filter $(BOARD), am62x_evm))
	AM62X_MCU_SDK_VERSION=am62x_09_00_00_19
else ifeq ($(BOARD),$(filter $(BOARD), am62ax_evm))
	AM62AX_MCU_SDK_VERSION=am62ax_09_00_00_19
else ifeq ($(BOARD),$(filter $(BOARD), am62px_evm))
	AM62PX_MCU_SDK_VERSION=am62px_09_00_01_17
else ifeq ($(BOARD),$(filter $(BOARD), j722s_evm))
	J722S_MCU_SDK_VERSION=j722s_09_02_00_17
endif

################################################################################
# Dependent toolchain paths variables
################################################################################
export TOOLCHAIN_PATH_R5        ?= $(TOOLS_INSTALL_PATH)/ti-cgt-armllvm_$(CGT_ARMLLVM_VERSION)
export MCUSW_INSTALL_PATH       ?= $(SDK_INSTALL_PATH)/mcusw
ifeq ($(BOARD),$(filter $(BOARD), am62x_evm))
	export mcu_plus_sdk_INSTALL_PATH ?= $(SDK_INSTALL_PATH)/mcu_plus_sdk_$(AM62X_MCU_SDK_VERSION)
else ifeq ($(BOARD),$(filter $(BOARD), am62ax_evm))
	export mcu_plus_sdk_INSTALL_PATH ?= $(SDK_INSTALL_PATH)/mcu_plus_sdk_$(AM62AX_MCU_SDK_VERSION)
else ifeq ($(BOARD),$(filter $(BOARD), am62px_evm))
	export mcu_plus_sdk_INSTALL_PATH ?= $(SDK_INSTALL_PATH)/mcu_plus_sdk_$(AM62PX_MCU_SDK_VERSION)
else ifeq ($(BOARD),$(filter $(BOARD), j722s_evm))
	export mcu_plus_sdk_INSTALL_PATH ?= $(SDK_INSTALL_PATH)/mcu_plus_sdk_$(J722S_MCU_SDK_VERSION)
else
  	export PDK_INSTALL_PATH         ?= $(SDK_INSTALL_PATH)/pdk_j721s2_09_02_00_30/packages
	export SDL_INSTALL_PATH		?= $(SDK_INSTALL_PATH)/sdl
endif
export CG_XML_BIN_INSTALL_PATH  ?= $(TOOLS_INSTALL_PATH)/cg_xml_$(CG_XML_VERSION)/bin

export XDC_INSTALL_PATH ?= $(CCS_INSTALL_PATH)/xdctools_$(XDC_VERSION)

ifeq ($(OS),Windows_NT)
  #Paths for windows machine
  export TOOLCHAIN_PATH_GCC_ARCH64 ?= $(TOOLS_INSTALL_PATH)/gcc-linaro-$(GCC_ARCH64_VERSION)-i686-mingw32_aarch64-elf
else
  #Paths for linux machine
  export TOOLCHAIN_PATH_GCC_ARCH64 ?= $(TOOLS_INSTALL_PATH)/gcc-arm-$(GCC_ARCH64_VERSION)-x86_64-aarch64-none-elf
endif
  export TOOLCHAIN_PATH_A53        ?= $(TOOLCHAIN_PATH_GCC_ARCH64)
  export TOOLCHAIN_PATH_A72        ?= $(TOOLCHAIN_PATH_GCC_ARCH64)
  export GCC_ARCH64_BIN_PREFIX     ?= $(GCC_ARCH64_BIN_PREFIX_STR)

ifeq ($(AUTOSAR_VERSION), 421)
export AUTOSAR_VERSION   ?= 421
endif

ifeq ($(AUTOSAR_VERSION), 431)
export AUTOSAR_VERSION   ?= 431
endif

################################################################################
# Other advanced configurable variables
################################################################################

#use <module>_PATH variable as makefile internally expects PATH variable this way for external component path
export pdk_PATH  := $(PDK_INSTALL_PATH)
export mcal_PATH := $(MCUSW_INSTALL_PATH)/mcal_drv/mcal
export mcal_test_PATH := $(MCUSW_INSTALL_PATH)/mcal_drv/mcal_test
ifeq ($(BUILD_OS_TYPE),baremetal)
  export autosarConfigSrc_PATH ?= $(MCUSW_INSTALL_PATH)/mcal_drv/mcal/examples_config
else
  export autosarConfigSrc_PATH ?= $(MCUSW_INSTALL_PATH)/mcuss_demos/mcal_config
endif
export bios_PATH := $(BIOS_INSTALL_PATH)
export xdc_PATH := $(XDC_INSTALL_PATH)
XDCPATH = $(bios_PATH)/packages;$(xdc_PATH)/packages;$(pdk_PATH);
export XDCPATH

export mcuss_demo_PATH := $(MCUSW_INSTALL_PATH)/mcuss_demos
ifeq ($(BOARD),$(filter $(BOARD), am62x_evm am62ax_evm am62px_evm j722s_evm))
	export ROOTDIR := $(mcu_plus_sdk_INSTALL_PATH)
else
	export ROOTDIR := $(PDK_INSTALL_PATH)
endif

#Autosar basic software include path provided by AUTOSAR stack
ifeq ($(BUILD_OS_TYPE),baremetal)
  export autosarBSWInc_PATH        ?= $(MCUSW_INSTALL_PATH)/mcal_drv/mcal/Bsw_Stubs
  export autosarCompilerTypes_PATH ?= $(MCUSW_INSTALL_PATH)/mcal_drv/mcal/Bsw_Stubs/AsrGnrl/inc
  export McuSDK_Path               ?= $(SDK_INSTALL_PATH)/source/drivers/sciclient/include
else
  export autosarBSWInc_PATH        ?= $(MCUSW_INSTALL_PATH)/mcuss_demos/Bsw_Stubs
  export autosarCompilerTypes_PATH ?= $(MCUSW_INSTALL_PATH)/mcuss_demos/Bsw_Stubs/AsrGnrl/inc
endif

# Set Core Build Profile depending on BUILD_PROFILE value
export BUILD_PROFILE_$(CORE) ?= $(BUILD_PROFILE)

# Default PACKAGE_SELECT build flag
# Supported values: all
export PACKAGE_SELECT ?= all

# Disable recursive building of example dependencies
export DISABLE_RECURSE_DEPS ?= no

# Default C++ build flag, yes or no
export CPLUSPLUS_BUILD ?= no

# ospi vs qspi flash build
export FLS_PROTOCOL ?= ospi

#Default MCAL config - use base directory
MCAL_CONFIG ?= 0

#Variable to build Linux application
CDD_IPC_LINUX_BUILD ?= no

ifeq ($(CDD_IPC_LINUX_BUILD),no)
# Get config path for each component
MCAL_COMP_DIR_LIST = Mcu Adc Can Dio Eth EthTrcv Gpt Spi Wdg Pwm CddIpc Fls Icu
define GET_MCAL_COMP_CONFIG_PATH
  ifeq ($(wildcard $(MCUSW_INSTALL_PATH)/mcal_drv/mcal_test/ut_config/$(1)/soc/$(SOC)/$(CORE)/config_$(MCAL_CONFIG)/.*),)
    # Specified config directory not found, default to base config directory
    # Also j721s2 qspi and ospi flash have different configurations
    # please check mcusw_mcal_component.mk for exact value of autosarConfigSrc_PATH for each fls applications
      $(1)_CONFIG_PATH += $(autosarConfigSrc_PATH)/$(1)_Demo_Cfg/output/generated/soc/$(SOC)/$(CORE)/include
  else
    $(1)_CONFIG_PATH += $(MCUSW_INSTALL_PATH)/mcal_drv/mcal_test/ut_config/$(1)/soc/$(SOC)/$(CORE)/config_$(MCAL_CONFIG)
  endif
endef
else
MCAL_COMP_DIR_LIST = CddIpc
define GET_MCAL_COMP_CONFIG_PATH
    $(1)_CONFIG_PATH += $(autosarConfigSrc_PATH)/CddIpcLinux_Demo_Cfg/output/generated/soc/$(SOC)/$(CORE)/include
  endef
endif
$(foreach MCAL_COMP,$(MCAL_COMP_DIR_LIST),$(eval $(call GET_MCAL_COMP_CONFIG_PATH,$(MCAL_COMP))))

#Autosar generated config include files
#Can be list of directories separated by space.
#Should include path to each modules pre compile header path
#Should include path to each modules <Mod>_Memmap.h
export autosarConfig_PATH = $(foreach MCAL_COMP,$(MCAL_COMP_DIR_LIST),$($(MCAL_COMP)_CONFIG_PATH))

#Default SECTTI SIZE INFORMATION
export SECTTI_SIZE_INFO ?= no

#Default SECTTI tool
export SECTTI ?= $(CG_XML_BIN_INSTALL_PATH)/sectti

# include other dependent files

include $(MCUSW_INSTALL_PATH)/build/build_config.mk
include $(MCUSW_INSTALL_PATH)/build/platform.mk
include $(MCUSW_INSTALL_PATH)/build/makerules/env.mk
