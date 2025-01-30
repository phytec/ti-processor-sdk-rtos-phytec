include $(MCUSW_INSTALL_PATH)/build/Rules.make

APP_NAME = eth_load_app

SRCDIR += $(app_utils_PATH)/src
INCDIR += $(mcal_PATH)
INCDIR += $(mcal_PATH)/hw_include

INCDIR += $(mcal_PATH)/examples/Eth/utils
INCDIR += $(mcal_PATH)/examples/Eth/utils/soc
INCDIR += $(mcal_PATH)/examples/Eth/utils/soc/$(SOC)/$(CORE)
SRCDIR += $(mcal_PATH)/examples/Eth/utils
SRCDIR += $(mcal_PATH)/examples/Eth/utils/soc/$(SOC)
SRCDIR += $(mcal_PATH)/examples/Eth/utils/soc/$(SOC)/$(CORE)


# Configuration Variants
# - By default precompile configuration is used and is already included
#   in INCLUDE_EXTERNAL_INTERFACES = autosarConfig
ifeq ($(BUILD_VARIANT), linktime)
  INCDIR += $(autosarConfigSrc_PATH)/Eth_Demo_Lcfg/output/generated/soc/$(SOC)/$(CORE)/include
  SRCDIR += $(autosarConfigSrc_PATH)/Eth_Demo_Lcfg/output/generated/soc/$(SOC)/$(CORE)/src
else ifeq ($(BUILD_VARIANT), postbuild)
  INCDIR += $(autosarConfigSrc_PATH)/Eth_Demo_PBcfg/output/generated/soc/$(SOC)/$(CORE)/include
  SRCDIR += $(autosarConfigSrc_PATH)/Eth_Demo_PBcfg/output/generated/soc/$(SOC)/$(CORE)/src
else
  SRCDIR += $(autosarConfigSrc_PATH)/Eth_Demo_Cfg/output/generated/soc/$(SOC)/$(CORE)/src
endif
SRCDIR += $(autosarConfigSrc_PATH)/EthTrcv_Demo_Cfg/output/generated/soc/$(SOC)/$(CORE)/src
INCDIR += $(MCUSW_INSTALL_PATH)/mcal_drv/mcal/examples_config/EthTrcv_Demo_Cfg/output/generated/soc/$(SOC)/$(CORE)/include
SRCDIR += $(MCUSW_INSTALL_PATH)/mcal_drv/mcal/examples_config/EthTrcv_Demo_Cfg/output/generated/soc/$(SOC)/$(CORE)/src

# List all the external components/interfaces, whose interface header files
# need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = autosarBSW autosarConfig autosarCompiler
INCLUDE_INTERNAL_INTERFACES = eth ethtrcv bsw_stubs app_utils

# List all the components required by the application
COMP_LIST_COMMON = eth ethtrcv bsw_stubs app_utils hw_init

# List all the external ibs required by the application
# List all the external libs required by the application
ifeq ($(SOC),$(filter $(SOC), j721e j7200 j721s2 j784s4))
  COMP_LIST_COMMON += freertos osal_freertos
  ifeq ($(SOC),$(filter $(SOC), j721e j7200 j784s4))
  SRCS_COMMON = Cdd_IpcCfg.c app_utils_cdd_ipc.c CddIpcR5Mpu.c EthUtils_VirtMac.c
  COMP_LIST_COMMON += cdd_ipc
  INCLUDE_INTERNAL_INTERFACES += cdd_ipc
  endif
  INCDIR += $(mcal_PATH)/examples/Eth/utils/CddIpc_Demo_Cfg/output/generated/soc/$(SOC)/$(CORE)/include
  SRCDIR += $(mcal_PATH)/examples/Eth/utils/CddIpc_Demo_Cfg/output/generated/soc/$(SOC)/$(CORE)/src
  INCDIR += $(autosarConfigSrc_PATH)/IntRtr_Cfg/soc/$(SOC)/$(CORE)
  INCDIR += $(PDK_INSTALL_PATH) $(PDK_INSTALL_PATH)/ti/drv/sciclient
  INCDIR += $(PDK_INSTALL_PATH)/ti/kernel/freertos/FreeRTOS-LTS/FreeRTOS-Kernel/include
  INCDIR += $(PDK_INSTALL_PATH)/ti/kernel/freertos/portable/TI_CGT/r5f
  INCDIR += $(PDK_INSTALL_PATH)/ti/kernel/freertos/config/$(SOC)/r5f

  EXT_LIB_LIST_COMMON = $(csl_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(csl_LIBNAME).$(LIBEXT)
  EXT_LIB_LIST_COMMON += $(csl_init_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(csl_init_LIBNAME).$(LIBEXT)
  EXT_LIB_LIST_COMMON += $(osal_freertos_LIBPATH)/$(SOC)/$(ISA)/$(BUILD_PROFILE_$(CORE))/$(osal_freertos_LIBNAME).$(LIBEXT)
  EXT_LIB_LIST_COMMON += $(freertos_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(freertos_LIBNAME).$(LIBEXT)
  ifeq ($(CORE),mcu1_0)
    EXT_LIB_LIST_COMMON += $(sciclient_direct_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(sciclient_direct_LIBNAME).$(LIBEXT)
    EXT_LIB_LIST_COMMON += $(sciserver_tirtos_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(sciserver_tirtos_LIBNAME).$(LIBEXT)
    EXT_LIB_LIST_COMMON += $(rm_pm_hal_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(rm_pm_hal_LIBNAME).$(LIBEXT)
  else
    EXT_LIB_LIST_COMMON += $(sciclient_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(sciclient_LIBNAME).$(LIBEXT)
  endif
  # Required for uart
  EXT_LIB_LIST_COMMON += $(uart_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(uart_LIBNAME).$(LIBEXT)
  # Required by board
  EXT_LIB_LIST_COMMON += $(board_LIBPATH)/$(BOARD)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(board_LIBNAME).$(LIBEXT)
  EXT_LIB_LIST_COMMON += $(i2c_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(i2c_LIBNAME).$(LIBEXT)
else ifeq ($(SOC),$(filter $(SOC), am62x am62ax am62px j722s))
  INCDIR += $(mcu_plus_sdk_INSTALL_PATH)/source
  INCDIR += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers
  INCDIR += ${mcu_plus_sdk_INSTALL_PATH}/source/kernel/freertos/FreeRTOS-Kernel/include
  INCDIR +=  ${mcu_plus_sdk_INSTALL_PATH}/source/kernel/freertos/portable/TI_ARM_CLANG/ARM_CR5F
  INCDIR +=  ${mcu_plus_sdk_INSTALL_PATH}/source/kernel/freertos/config/$(SOC)/r5f
  EXT_LIB_LIST_COMMON = $(mcu_plus_sdk_INSTALL_PATH)/source/kernel/freertos/lib/freertos.$(SOC).r5f.ti-arm-clang.$(BUILD_PROFILE_$(CORE)).lib
  ifeq ($(CORE),mcu0_0)
    EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/board/lib/board.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
    ifeq ($(SOC),$(filter $(SOC), am62px j722s))
      EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/lib/drivers.$(SOC).mcu-$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
    else
      EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/lib/drivers.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
    endif
    ifeq ($(SOC),$(filter $(SOC), am62x))
      EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/device_manager/rm_pm_hal/lib/rm_pm_hal.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
      EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/device_manager/self_reset/lib/self_reset.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
      EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/device_manager/sciserver/lib/sciserver.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
      EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/device_manager/sciclient_direct/lib/sciclient_direct.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
      EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/device_manager/dm_stub/lib/dm_stub.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
    endif
  endif
endif

# Common source files and CFLAGS across all platforms and cores

# App utils file includes memory sections defined in this module
# which is used to check memory section corruption
SRCS_COMMON += app_utils_eth.c

SRCS_COMMON += EthUtils.c EthLoadApp.c Eth_Cfg.c Eth_Lcfg.c Eth_PBcfg.c EthTrcv_Cfg.c

SRCS_COMMON += EthUtils_Target.c

ifeq ($(SOC),$(filter $(SOC), j721e j7200 j721s2 j784s4))
    SRCS_COMMON += main.c board.c
else ifeq ($(SOC),$(filter $(SOC), am62x am62ax am62px j722s))
  ifeq ($(CORE),mcu0_0)
    SRCS_COMMON += main.c board.c
    SRCS_COMMON += ti_board_config.c \
                   ti_board_open_close.c \
                   ti_dpl_config.c \
                   ti_drivers_config.c \
                   ti_drivers_open_close.c \
                   ti_pinmux_config.c \
                   ti_power_clock_config.c
  endif
endif

SRCS_COMMON += os_wrapper.c

PACKAGE_SRCS_COMMON = .

CFLAGS_LOCAL_COMMON = $(MCUSW_CFLAGS)
LNKFLAGS_LOCAL_COMMON = $(MCUSW_LNKFLAGS)
ifeq ($(MCUSW_UART_ENABLE),TRUE)
    CFLAGS_LOCAL_COMMON += -DBUILD_MCU -DUART_ENABLED
endif

CFLAGS_LOCAL_COMMON += -Dfreertos

EXTERNAL_LNKCMD_FILE_LOCAL = $(mcal_PATH)/examples/Eth/utils/build/$(SOC)/$(CORE)/linker_r5_freertos.lds

# Core/SoC/platform specific source files and CFLAGS
# Example:
# SRCS_<core/SoC/platform-name> =
# CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
include $(MCUSW_INSTALL_PATH)/build/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk
# and need not be explicitly specified here

# Nothing beyond this point
