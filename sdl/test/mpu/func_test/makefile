#
# This file is the makefile for building SDL MPU Test App.
#
#   Copyright (c) Texas Instruments Incorporated 2021
#
include $(sdl_PATH)/Rules.make

APP_NAME = mpu_func_test_app
BUILD_OS_TYPE=baremetal
SRCDIR = $(sdl_PATH)/test/mpu/func_test
INCDIR = .

SRCDIR += mpu/
INCDIR += mpu/ $(sdl_PATH)/src/sdl
INCDIR += mpu/ $(sdl_PATH)/src/ip
INCDIR += $(sdl_PATH) $(sdl_PATH)/src/sdl $(sdl_PATH)/include $(sdl_PATH)/include/soc/$(SOC)

# External SDK includes needed by test app
SRCDIR += $(sdl_PATH)/test/unity/src $(sdl_PATH)/test/unity/config
INCDIR += $(sdl_PATH)/test/unity/src $(sdl_PATH)/test/unity/config

# Include the common test OSAL implementatoin
SRCDIR += $(sdl_PATH)/test/osal/src
INCDIR += $(sdl_PATH)/test/osal

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk

# List all the components required by the application
COMP_LIST_COMMON += sdl_api sdl_ip r5f_startup r5f_core sdl_osal  

INCDIR += $(sdl_PATH)/src/ip/r5
SRCS_COMMON += interrupt_handlers.c sdl_vim.c interrupt.c sdl_r5.c 

SRCDIR += $(sdl_PATH)/src/ip/r5/src
SRCS_ASM_COMMON += resetvecs.asm sdl_arm_r5.asm


# J721E depends on PDK modules for the test
ifeq ($(SOC),$(filter $(SOC), j721e j7200 j721s2 j784s4))
BUILD_CORE=mcu1_0
EXT_LIB_LIST_COMMON += $(board_LIBPATH)/$(BOARD)/$(ISA_EXT)/$(PROFILE)/$(board_LIBNAME).$(LIBEXT)
EXT_LIB_LIST_COMMON += $(i2c_LIBPATH)/$(SOC)/$(ISA_EXT)/$(PROFILE)/$(i2c_LIBNAME).$(LIBEXT)
EXT_LIB_LIST_COMMON += $(sciclient_direct_LIBPATH)/$(SOC)/$(BUILD_CORE)/$(PROFILE)/$(sciclient_direct_LIBNAME).$(LIBEXT)
EXT_LIB_LIST_COMMON += $(rm_pm_hal_LIBPATH)/$(SOC)/$(BUILD_CORE)/$(PROFILE)/$(rm_pm_hal_LIBNAME).$(LIBEXT)
EXT_LIB_LIST_COMMON += $(osal_nonos_LIBPATH)/$(SOC)/$(ISA_EXT)/$(PROFILE)/$(osal_nonos_LIBNAME).$(LIBEXT)
EXT_LIB_LIST_COMMON += $(uart_LIBPATH)/$(SOC)/$(ISA_EXT)/$(PROFILE)/$(uart_LIBNAME).$(LIBEXT)
EXT_LIB_LIST_COMMON += $(csl_LIBPATH)/$(SOC)/$(ISA_EXT)/$(PROFILE)/$(csl_LIBNAME).$(LIBEXT)
EXT_LIB_LIST_COMMON += $(csl_init_LIBPATH)/$(SOC)/$(ISA_EXT)/$(PROFILE)/$(csl_init_LIBNAME).$(LIBEXT)
endif

# Common source files and CFLAGS across all platforms and cores
PACKAGE_SRCS_COMMON = .
CFLAGS_LOCAL_COMMON = $(SDL_CFLAGS) -DUNITY_INCLUDE_CONFIG_H

SRCS_COMMON += sdl_mpu_funcTest.c sdl_mpu_default.c main.c sdtf_exception.c
PACKAGE_SRCS_COMMON += main.h sdtf_exception.h
SRCS_COMMON += osal_interface.c


SRCS_COMMON += unity.c unity_config.c

ifeq ($(SOC),$(filter $(SOC), j721e j7200 j721s2 j784s4))
EXTERNAL_LNKCMD_FILE_LOCAL = $(sdl_PATH)/test/mpu/func_test/$(SOC)/linker_r5.lds
endif
	
# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
ifeq ($(MAKERULEDIR), )
#Makerule path not defined, define this and assume relative path from ROOTDIR
  MAKERULEDIR := $(ROOTDIR)/build/makerules
  export MAKERULEDIR
endif
include $(MAKERULEDIR)/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk
#     and need not be explicitly specified here

# Nothing beyond this point
