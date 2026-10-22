include $(sdl_PATH)/Rules.make

APP_NAME = dm_firmware

# must be set at call as Rules.make does already use parts of it... FIXME add checks?!
#BUILD_OS_TYPE = freertos
#SOC = j721s2
#BOARD = phyboard_izar
#BUILD_CORE = mcu1_0

 # also add 'csl sciclient_direct osal board uart ipc ...' here ?! Seems to work without
INCLUDE_EXTERNAL_INTERFACES = pdk

COMP_LIST_COMMON = sdl_api sdl_ip sdl_osal r5f_startup r5f_core

EXT_LIB_LIST_COMMON += $(board_LIBPATH)/$(BOARD)/$(ISA_EXT)/$(PROFILE)/$(board_LIBNAME).$(LIBEXT)
EXT_LIB_LIST_COMMON += $(sciclient_direct_LIBPATH)/$(BOARD)/$(BUILD_CORE)/$(PROFILE)/$(sciclient_direct_LIBNAME).$(LIBEXT)
EXT_LIB_LIST_COMMON += $(rm_pm_hal_LIBPATH)/$(SOC)/$(BUILD_CORE)/$(PROFILE)/$(rm_pm_hal_LIBNAME).$(LIBEXT)
EXT_LIB_LIST_COMMON += $(uart_LIBPATH)/$(BOARD)/$(ISA_EXT)/$(PROFILE)/$(uart_LIBNAME).$(LIBEXT)
EXT_LIB_LIST_COMMON += $(i2c_LIBPATH)/$(SOC)/$(ISA_EXT)/$(PROFILE)/$(i2c_LIBNAME).$(LIBEXT)

#Path for including common files
DMFW_COMMON_PATH = common

INCDIR = $(DMFW_COMMON_PATH)/$(SOC)/$(BUILD_OS_TYPE)/
SRCDIR = .
SRCS_COMMON = ipc_echo_test.c

PACKAGE_SRCS_COMMON = .
ECHO_TEST_BTCM = 1
# FIXME there is as -Dphyboard_izar missing here !!! Otherwise the board_cfg.h includes are not resolved correctly!!! 
# (dm_firmware_BOARD_DEPENDENCY does not help !?) -> this should be done automatically by setting 'dm_firmware_BOARD_DEPENDENCY'
CFLAGS_LOCAL_COMMON = $(SDL_CFLAGS) -DA72_LINUX_OS -D$(BOARD)=$(BOARD) #-DA72_LINUX_OS_IPC_ATTACH

$(info #Building 'dm_firmware' for $(SOC) / $(BUILD_CORE) / $(BOARD) / $(BUILD_OS_TYPE))
include $(DMFW_COMMON_PATH)/makefile.mk
