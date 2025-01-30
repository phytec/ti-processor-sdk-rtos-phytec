include $(MCUSW_INSTALL_PATH)/build/Rules.make

# Application name
APP_NAME = cdd_ipc_app

# Path to .c source files
SRCDIR = . ./soc/$(SOC)/$(CORE) $(autosarConfigSrc_PATH)/CddIpc_Demo_Cfg/output/generated/soc/$(SOC)/$(CORE)/src $(app_utils_PATH)/src soc/$(SOC)
ifeq ($(SOC),$(filter $(SOC), am62x))
SRCDIR += $(app_utils_PATH)/src
SRCDIR += $(mcal_PATH)/examples/CddIpc/soc/$(SOC)/Generated_Dpl
SRCDIR += $(MCUSW_INSTALL_PATH)/mcal_drv/mcal/CddIpc/src
SRCDIR += $(MCUSW_INSTALL_PATH)/mcal_drv/mcal/CddIpc/ipc_baremetal_hw/src
SRCDIR += $(MCUSW_INSTALL_PATH)/mcal_drv/mcal/hw_include/arch/r5/src
endif

# Path to .h source files
INCDIR = . $(autosarConfigSrc_PATH)/IntRtr_Cfg/soc/$(SOC)/$(CORE)/
INCDIR += $(mcal_PATH)
ifeq ($(SOC),$(filter $(SOC), am62x))
INCDIR += . $(mcu_plus_sdk_INSTALL_PATH)/source
INCDIR += . $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/sciclient
INCDIR +=  $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/sciclient/include
INCDIR +=  $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/drivers/hw_include
INCDIR += $(MCUSW_INSTALL_PATH)/mcal_drv/mcal/CddIpc/include
INCDIR += $(MCUSW_INSTALL_PATH)/mcal_drv/mcal/CddIpc/ipc_baremetal_hw/include
INCDIR += $(MCUSW_INSTALL_PATH)/mcal_drv/mcal/hw_include/arch/r5
INCDIR += $(mcal_PATH)/examples/CddIpc/soc/$(SOC)/Generated_Dpl
endif

# List all the external components/interfaces, whose interface header files
# need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk autosarBSW autosarConfig autosarCompiler

INCLUDE_INTERNAL_INTERFACES = cdd_ipc bsw_stubs app_utils

# List all the components required by the application
COMP_LIST_COMMON = cdd_ipc bsw_stubs app_utils
ifeq ($(SOC),$(filter $(SOC), am62x))
COMP_LIST_COMMON +=  generate_dpl 
endif

ifeq ($(SOC),$(filter $(SOC), j721e j7200 j721s2 j784s4))
# List all the external libs required by the application
EXT_LIB_LIST_COMMON = $(csl_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(csl_LIBNAME).$(LIBEXT)
EXT_LIB_LIST_COMMON += $(csl_init_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(csl_init_LIBNAME).$(LIBEXT)
# Required for sciclient required by IPC CDD driver
EXT_LIB_LIST_COMMON += $(sciclient_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(sciclient_LIBNAME).$(LIBEXT)

# Required for sciclient required by IPC CDD driver
EXT_LIB_LIST_COMMON += $(osal_nonos_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(osal_nonos_LIBNAME).$(LIBEXT)
# Required for uart
EXT_LIB_LIST_COMMON += $(uart_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(uart_LIBNAME).$(LIBEXT)
EXT_LIB_LIST_COMMON += $(board_LIBPATH)/$(BOARD)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(board_LIBNAME).$(LIBEXT)
# Required by board
EXT_LIB_LIST_COMMON += $(i2c_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(i2c_LIBNAME).$(LIBEXT)
endif
ifeq ($(SOC),$(filter $(SOC), am62x))
EXT_LIB_LIST_COMMON = $(mcu_plus_sdk_INSTALL_PATH)/source/board/lib/board.am62x.r5f.ti-arm-clang.$(BUILD_PROFILE).lib
EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/device_manager/dm_stub/lib/dm_stub.am62x.r5f.ti-arm-clang.$(BUILD_PROFILE).lib
EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/lib/drivers.am62x.r5f.ti-arm-clang.$(BUILD_PROFILE).lib
EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/kernel/nortos/lib/nortos.am62x.r5f.ti-arm-clang.$(BUILD_PROFILE).lib
EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/device_manager/rm_pm_hal/lib/rm_pm_hal.am62x.r5f.ti-arm-clang.$(BUILD_PROFILE).lib
EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/device_manager/sciclient_direct/lib/sciclient_direct.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/device_manager/self_reset/lib/self_reset.am62x.r5f.ti-arm-clang.$(BUILD_PROFILE).lib
EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/device_manager/dm_stub/lib/dm_stub.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/device_manager/sciserver/lib/sciserver.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
endif

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON =  CddIpcApp.c   
SRCS_COMMON += Cdd_IpcCfg.c
SRCS_COMMON += CddIpcAppStartup.c
ifeq ($(SOC),$(filter $(SOC), j721e j7200 j721s2 j784s4))
SRCS_COMMON += CddIpcR5Mpu.c
endif
ifeq ($(SOC),$(filter $(SOC), am62x))
SRCS_COMMON += ti_drivers_config.c
SRCS_COMMON += ti_drivers_open_close.c
SRCS_COMMON += ti_board_config.c
SRCS_COMMON += ti_board_open_close.c
SRCS_COMMON += ti_dpl_config.c
SRCS_COMMON += ti_pinmux_config.c
SRCS_COMMON += ti_power_clock_config.c
SRCS_COMMON += lld_cache.c
SRCS_ASM_COMMON = lld_arm_r5.asm
endif
# App utils file includes memory sections defined in this module
# which is used to check memory section corruption
SRCS_COMMON += app_utils_cdd_ipc.c

PACKAGE_SRCS_COMMON = .

CFLAGS_LOCAL_COMMON = $(MCUSW_CFLAGS)
LNKFLAGS_LOCAL_COMMON = $(MCUSW_LNKFLAGS)
ifeq ($(MCUSW_UART_ENABLE),TRUE)
    CFLAGS_LOCAL_COMMON += -DUART_ENABLED
endif
# Core/SoC/platform specific source files and CFLAGS
# Example:
# SRCS_<core/SoC/platform-name> =
# CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
include $(MCUSW_INSTALL_PATH)/build/common.mk


# OBJs and libraries are built by using rule defined in rules_<target>.mk
# and need not be explicitly specified here

# Nothing beyond this point
