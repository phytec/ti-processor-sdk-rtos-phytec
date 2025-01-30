include $(MCUSW_INSTALL_PATH)/build/Rules.make

ifeq ($(CORE), mcu1_0)
BUILD_OS_TYPE = freertos
endif

APP_NAME = eth_virtmac_app

SRCDIR += $(app_utils_PATH)/src
INCDIR += $(mcal_PATH)
INCDIR += $(mcal_PATH)/hw_include

INCDIR += $(mcal_PATH)/examples/Eth/utils
SRCDIR += $(mcal_PATH)/examples/Eth/utils
INCDIR += $(mcal_PATH)/examples/Eth/utils/soc
INCDIR += $(mcal_PATH)/examples/Eth/utils/soc/$(SOC)/$(CORE)
SRCDIR += $(mcal_PATH)/examples/Eth/utils/soc/$(SOC)
SRCDIR += $(mcal_PATH)/examples/Eth/utils/soc/$(SOC)/$(CORE)

ifeq ($(CORE), mcu1_0)
  ifeq ($(BUILD_VARIANT), linktime)
    export ETH_SPECIFIC_CONFIG_PATH ?= $(mcal_PATH)/examples_config/Eth_Demo_Lcfg/output/generated/soc/$(SOC)/mcu1_0_virtmac
  else ifeq ($(BUILD_VARIANT), postbuild)
    export ETH_SPECIFIC_CONFIG_PATH ?= $(mcal_PATH)/examples_config/Eth_Demo_PBcfg/output/generated/soc/$(SOC)/mcu1_0_virtmac
  else
    export ETH_SPECIFIC_CONFIG_PATH ?= $(mcal_PATH)/examples_config/Eth_Demo_Cfg/output/generated/soc/$(SOC)/mcu1_0_virtmac
  endif
endif

ifeq ($(ETH_SPECIFIC_CONFIG_PATH), )
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
else
  INCDIR += $(ETH_SPECIFIC_CONFIG_PATH)/include
  SRCDIR += $(ETH_SPECIFIC_CONFIG_PATH)/src
endif
SRCDIR += $(autosarConfigSrc_PATH)/EthTrcv_Demo_Cfg/output/generated/soc/$(SOC)/$(CORE)/src


INCDIR += $(mcal_PATH)/hw_include

# Cdd-IPC Config
# - Use the cdd-ipc config made specifically for this eth_virtmac application
INCDIR += $(mcal_PATH)/examples/Eth/utils/CddIpc_Demo_Cfg/output/generated/soc/$(SOC)/$(CORE)/include
SRCDIR += $(mcal_PATH)/examples/Eth/utils/CddIpc_Demo_Cfg/output/generated/soc/$(SOC)/$(CORE)/src
INCDIR += $(autosarConfigSrc_PATH)/IntRtr_Cfg/soc/$(SOC)/$(CORE)


# List all the external components/interfaces, whose interface header files
# need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk autosarBSW autosarConfig autosarCompiler
INCLUDE_INTERNAL_INTERFACES = eth ethtrcv bsw_stubs app_utils cdd_ipc

# List all the components required by the application
COMP_LIST_COMMON = eth ethtrcv bsw_stubs app_utils cdd_ipc hw_init

# List all the external ibs required by the application
INCDIR += $(PDK_INSTALL_PATH) $(PDK_INSTALL_PATH)/ti/drv/sciclient
EXT_LIB_LIST_COMMON = $(csl_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(csl_LIBNAME).$(LIBEXT)
EXT_LIB_LIST_COMMON += $(csl_init_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(csl_init_LIBNAME).$(LIBEXT)
EXT_LIB_LIST_COMMON += $(udma_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(udma_LIBNAME).$(LIBEXT)

ifeq ($(BUILD_OS_TYPE), freertos)
INCLUDE_EXTERNAL_INTERFACES += freertos
COMP_LIST_COMMON += osal_freertos freertos
EXT_LIB_LIST_COMMON += $(osal_freertos_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(osal_freertos_LIBNAME).$(LIBEXT)
EXT_LIB_LIST_COMMON += $(freertos_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(freertos_LIBNAME).$(LIBEXT)
else
EXT_LIB_LIST_COMMON += $(osal_nonos_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(osal_nonos_LIBNAME).$(LIBEXT)
endif

ifeq ($(SOC),$(filter $(SOC), j721e j7200 j784s4))
  ifeq ($(CORE),mcu1_0)
    EXT_LIB_LIST_COMMON += $(sciclient_direct_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(sciclient_direct_LIBNAME).$(LIBEXT)
    EXT_LIB_LIST_COMMON += $(sciserver_tirtos_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(sciserver_tirtos_LIBNAME).$(LIBEXT)
    EXT_LIB_LIST_COMMON += $(rm_pm_hal_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(rm_pm_hal_LIBNAME).$(LIBEXT)
    EXT_LIB_LIST_COMMON += $(pm_lib_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(pm_lib_LIBNAME).$(LIBEXT)
  else
    EXT_LIB_LIST_COMMON += $(sciclient_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(sciclient_LIBNAME).$(LIBEXT)
  endif
else
  INCDIR += $(mcu_plus_sdk_INSTALL_PATH)/source
  INCDIR += $(mcu_plus_sdk_INSTALL_PATH)/source \
            $(mcu_plus_sdk_INSTALL_PATH)/source/drivers
  EXT_LIB_LIST_COMMON += $(sciclient_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(sciclient_LIBNAME).$(LIBEXT)
endif
# Required for uart
EXT_LIB_LIST_COMMON += $(uart_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(uart_LIBNAME).$(LIBEXT)
# Required by board
EXT_LIB_LIST_COMMON += $(board_LIBPATH)/$(BOARD)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(board_LIBNAME).$(LIBEXT)
EXT_LIB_LIST_COMMON += $(i2c_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(i2c_LIBNAME).$(LIBEXT)

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON = EthUtils.c \
              EthVirtmacApp.c \
              Eth_Cfg.c \
              Eth_Lcfg.c \
              Eth_PBcfg.c \
              EthTrcv_Cfg.c \
              Cdd_IpcCfg.c \
              CddIpcR5Mpu.c

SRCS_COMMON += EthUtils_Target.c EthUtils_VirtMac.c  board.c


# App utils file includes memory sections defined in this module
# which is used to check memory section corruption
SRCS_COMMON += app_utils_eth.c

# App utils file includes memory sections defined in this module
# which is used to check memory section corruption
SRCS_COMMON += app_utils_cdd_ipc.c

ifeq ($(BUILD_OS_TYPE),tirtos)
EXTERNAL_LNKCMD_FILE_LOCAL = $(mcal_PATH)/examples/Eth/utils/build/$(SOC)/$(CORE)/linker_r5_sysbios.lds
else ifeq ($(BUILD_OS_TYPE),freertos)
EXTERNAL_LNKCMD_FILE_LOCAL = $(mcal_PATH)/examples/Eth/utils/build/$(SOC)/$(CORE)/linker_r5_freertos.lds
else
EXTERNAL_LNKCMD_FILE_LOCAL = $(mcal_PATH)/examples/Eth/utils/build/$(SOC)/$(CORE)/linker_r5.lds
endif

PACKAGE_SRCS_COMMON = .

CFLAGS_LOCAL_COMMON = $(MCUSW_CFLAGS)
LNKFLAGS_LOCAL_COMMON = $(MCUSW_LNKFLAGS)
ifeq ($(MCUSW_UART_ENABLE),TRUE)
    CFLAGS_LOCAL_COMMON += -DUART_ENABLED
endif

ifeq ($(ETH_CPSW_RECOVERY_TEST),TRUE)
    CFLAGS_LOCAL_COMMON += -DETH_CPSW_RECOVERY_TEST=STD_ON
else
    CFLAGS_LOCAL_COMMON += -DETH_CPSW_RECOVERY_TEST=STD_OFF
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
