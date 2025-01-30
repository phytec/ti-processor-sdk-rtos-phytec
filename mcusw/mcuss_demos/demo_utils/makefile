#
# This file is the makefile for utilities of mcuss demos
#
include $(MCUSW_INSTALL_PATH)/build/Rules.make

MODULE_NAME = demo_utils

SRCDIR = src
SRCDIR += src $(mcal_PATH)
INCDIR = .
INCDIR += $(mcal_PATH) $(mcal_PATH)/hw_include

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk
INCLUDE_INTERNAL_INTERFACES = demo_utils

MODULE_SRC_BASE_PATH := $(mcuss_demo_PATH)/demo_utils

PACKAGE_SRCS_COMMON = .
CFLAGS_LOCAL_COMMON = $(MCUSW_CFLAGS)
LNKFLAGS_LOCAL_COMMON = $(MCUSW_LNKFLAGS)

# List all the components required by this module
COMP_LIST_COMMON =

ifeq ($(BUILD_OS_TYPE), freertos)
  INCLUDE_EXTERNAL_INTERFACES +=  freertos
  COMP_LIST_COMMON += osal_freertos
  SRCS_COMMON += utils_prf_rtos.c
endif

# Include common make files
include $(MCUSW_INSTALL_PATH)/build/common.mk

# Nothing beyond this point
