BUILD_OS_TYPE=tirtos
include $(MCUSW_INSTALL_PATH)/build/Rules.make

APP_NAME = can_profile_xip_fota_app

SRCDIR = src $(mcal_PATH)
INCDIR = $(mcal_PATH)
SRCDIR += . $(autosarConfigSrc_PATH)/Can_Demo_Cfg/output/generated/soc/$(SOC)/$(CORE)/src $(autosarConfigSrc_PATH)/Dio_Demo_Cfg/output/generated/soc/$(SOC)/$(CORE)/src
SRCDIR += $(app_utils_PATH)/src soc/$(SOC)/$(CORE)/
INCDIR += . $(autosarConfigSrc_PATH)/IntRtr_Cfg/soc/$(SOC)/$(CORE)/ soc/$(SOC)/$(CORE)/

# List all the external components/interfaces, whose interface header files
# need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk autosarBSW autosarConfig autosarCompiler $(PDK_INSTALL_PATH)/ti/drv/spi

INCLUDE_INTERNAL_INTERFACES = can bsw_stubs_rtos app_utils dio demo_utils

# List all the components required by the application
COMP_LIST_COMMON = can bsw_stubs_rtos app_utils dio demo_utils $(PDK_INSTALL_PATH)/ti/drv/spi board

# List all the external ibs required by the application
EXT_LIB_LIST_COMMON += $(csl_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(csl_LIBNAME).$(LIBEXT)
# Required for sciclient required by UDMA driver
ifeq ($(SOC),$(filter $(SOC), j721e j7200))
  ifeq ($(CORE),mcu1_0)
  EXT_LIB_LIST_COMMON += $(sciclient_direct_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(sciclient_direct_LIBNAME).$(LIBEXT)
  EXT_LIB_LIST_COMMON += $(sciserver_tirtos_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(sciserver_tirtos_LIBNAME).$(LIBEXT)
  EXT_LIB_LIST_COMMON += $(rm_pm_hal_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(rm_pm_hal_LIBNAME).$(LIBEXT)
  else
  EXT_LIB_LIST_COMMON += $(sciclient_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(sciclient_LIBNAME).$(LIBEXT)
  endif
else
  EXT_LIB_LIST_COMMON += $(sciclient_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(sciclient_LIBNAME).$(LIBEXT)
endif
# Required for uart
EXT_LIB_LIST_COMMON += $(uart_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(uart_LIBNAME).$(LIBEXT)
#Board module
EXT_LIB_LIST_COMMON += $(board_LIBPATH)/$(BOARD)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(board_LIBNAME).$(LIBEXT)
# Required by board
EXT_LIB_LIST_COMMON += $(i2c_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(i2c_LIBNAME).$(LIBEXT)

SRCS_COMMON = main_rtos.c fota_writer.c fota_monitor.c can_profile.c CanApp_Startup.c

ifeq ($(BUILD_OS_TYPE), tirtos)
  INCLUDE_EXTERNAL_INTERFACES += xdc bios
  EXT_LIB_LIST_COMMON += $(osal_tirtos_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(osal_tirtos_LIBNAME).$(LIBEXT)
  # Enable XDC build for application by providing XDC CFG File per core
  XDC_CFG_FILE_$(CORE) = overrides/$(SOC)/$(CORE)/sysbios_$(ISA).cfg
  XDC_CFG_UPDATE_$(CORE)=$(MCUSW_INSTALL_PATH)/mcuss_demos/demo_utils/overrides/$(SOC)/utils_prf_ut.cfg
  EXTERNAL_LNKCMD_FILE_LOCAL = overrides/$(SOC)/$(CORE)/linker_r5_sysbios.lds
endif

ifeq ($(BUILD_OS_TYPE), freertos)
  INCLUDE_EXTERNAL_INTERFACES += freertos
  SRCS_COMMON += overrides/$(SOC)/$(CORE)/r5_mpu_freertos.c
  EXT_LIB_LIST_COMMON += $(osal_freertos_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(osal_freertos_LIBNAME).$(LIBEXT)
  EXT_LIB_LIST_COMMON += $(freertos_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(freertos_LIBNAME).$(LIBEXT)
  EXT_LIB_LIST_COMMON += $(csl_init_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(csl_init_LIBNAME).$(LIBEXT)
  EXTERNAL_LNKCMD_FILE_LOCAL = overrides/$(SOC)/$(CORE)/linker_r5_freertos.lds
endif

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON += Can_Cfg.c Can_PBcfg.c Dio_Lcfg.c

# App utils file includes memory sections defined in this module
# which is used to check memory section corruption
SRCS_COMMON += app_utils_can.c

PACKAGE_SRCS_COMMON = .

CFLAGS_LOCAL_COMMON = $(MCUSW_CFLAGS) -UMAKEFILE_BUILD
LNKFLAGS_LOCAL_COMMON = $(MCUSW_LNKFLAGS)
ifeq ($(MCUSW_UART_ENABLE),TRUE)
    CFLAGS_LOCAL_COMMON += -DUART_ENABLED
endif
ifeq ($(REGION), P2)
  CFLAGS_LOCAL_COMMON += -DBUILD_P2
else
  CFLAGS_LOCAL_COMMON += -DBUILD_P1
endif
ifeq ($(BUILD_OS_TYPE), freertos)
  CFLAGS_LOCAL_COMMON += -DBUILD_FREERTOS
else
  CFLAGS_LOCAL_COMMON += -DBUILD_TIRTOS
endif

# Core/SoC/platform specific source files and CFLAGS
# Example:
# SRCS_<core/SoC/platform-name> =
# CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
include $(MCUSW_INSTALL_PATH)/build/common.mk