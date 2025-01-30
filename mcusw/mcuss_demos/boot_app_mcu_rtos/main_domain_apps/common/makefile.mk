#
# This file is the makefile for building main domain apps for freeRTOS
#
SRCDIR += . ../common/src
INCDIR += . ../common/src

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk

# List all the components required by the application
COMP_LIST_COMMON += csl sciclient

COMP_LIST_COMMON += board i2c uart

ifeq ($(BUILD_OS_TYPE),freertos)
INCLUDE_EXTERNAL_INTERFACES += freertos
endif

ifeq ($(SOC),$(filter $(SOC), j721e j7200))
ifeq ($(BUILD_OS_TYPE),freertos)
	EXTERNAL_LNKCMD_FILE_LOCAL = ../common/$(SOC)/linker_$(ISA)_$(CORE)_freertos.lds
endif
ifeq ($(BUILD_OS_TYPE),baremetal)
	EXTERNAL_LNKCMD_FILE_LOCAL = ../common/$(SOC)/linker_$(ISA)_$(CORE)_baremetal.lds
endif
endif

# Common source files and CFLAGS across all platforms and cores
PACKAGE_SRCS_COMMON = . ../common ../../common
SRCS_COMMON += main_domain_utils.c
SRCS_COMMON += mcu_timer_read_multicore.c

CFLAGS_LOCAL_COMMON += $(PDK_CFLAGS)

# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
include $(MCUSW_INSTALL_PATH)/build/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk
#     and need not be explicitly specified here

# Nothing beyond this point
