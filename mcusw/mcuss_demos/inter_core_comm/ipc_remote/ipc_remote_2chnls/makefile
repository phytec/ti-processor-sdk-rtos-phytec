include $(MCUSW_INSTALL_PATH)/build/Rules.make

APP_NAME = ipc_remote_2chnls_app_$(BUILD_OS_TYPE)

CFLAGS_LOCAL_COMMON = $(MCUSW_CFLAGS)
LNKFLAGS_LOCAL_COMMON = $(MCUSW_LNKFLAGS)

SRCDIR = . $(autosarConfigSrc_PATH)/CddIpc_Demo_Cfg/output/generated/src
SRCDIR += $(app_utils_PATH)/src ./overrides/$(SOC) $(MCUSW_INSTALL_PATH)/mcuss_demos/inter_core_comm/ipc_remote
SRCDIR += src $(mcal_PATH)
INCDIR = .
INCDIR += $(mcal_PATH)

# List all the external components/interfaces, whose interface header files
# need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk autosarBSW autosarConfig autosarCompiler ipc i2c

INCLUDE_INTERNAL_INTERFACES = bsw_stubs_rtos demo_utils

# List all the components required by the application
COMP_LIST_COMMON = ipc bsw_stubs_rtos demo_utils

# List all the external libs required by the application
EXT_LIB_LIST_COMMON = $(csl_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(csl_LIBNAME).$(LIBEXT)
EXT_LIB_$(CORE) = $(csl_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(csl_LIBNAME).$(LIBEXT)
# Required for sciclient required by UDMA driver
ifeq ($(SOC),$(filter $(SOC), j721e j7200 j721s2 j784s4))
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
EXT_LIB_$(CORE) += $(sciclient_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(sciclient_LIBNAME).$(LIBEXT)
# Required for uart
EXT_LIB_LIST_COMMON += $(uart_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(uart_LIBNAME).$(LIBEXT)
EXT_LIB_$(CORE) += $(uart_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(uart_LIBNAME).$(LIBEXT)
EXT_LIB_LIST_COMMON += $(board_LIBPATH)/$(BOARD)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(board_LIBNAME).$(LIBEXT)
EXT_LIB_$(CORE) += $(board_LIBPATH)/$(BOARD)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(board_LIBNAME).$(LIBEXT)
# Required by board
EXT_LIB_LIST_COMMON += $(i2c_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(i2c_LIBNAME).$(LIBEXT)

# Only supported with TI RTOS
SRCS_COMMON = main_1remote_2chnls_rtos.c ipc_utils.c
ifeq ($(BUILD_OS_TYPE), freertos)
	SRCS_COMMON += $(MCUSW_INSTALL_PATH)/build/$(SOC)/r5_mpu_freertos.c
endif

ifeq ($(SOC), j721e)
  SRCS_COMMON += proc_depends_j721e.c
endif

ifeq ($(SOC), j7200)
  SRCS_COMMON += proc_depends_j7200.c
endif

ifeq ($(SOC), j721s2)
  SRCS_COMMON += proc_depends_j721s2.c
endif

ifeq ($(SOC), j784s4)
  SRCS_COMMON += proc_depends_j784s4.c
endif

# Enable copy of vectors
ifeq ($(ISA),$(filter $(ISA), r5f))
  SRCS_ASM_COMMON += utilsCopyVecs2ATcm.asm
endif

ifeq ($(BUILD_OS_TYPE), freertos)
	CFLAGS_LOCAL_COMMON += -DFREERTOS
	INCLUDE_EXTERNAL_INTERFACES += freertos
	EXT_LIB_LIST_COMMON += $(csl_init_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(csl_init_LIBNAME).$(LIBEXT)
	EXT_LIB_LIST_COMMON += $(osal_freertos_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(osal_freertos_LIBNAME).$(LIBEXT)
	EXT_LIB_LIST_COMMON += $(freertos_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(freertos_LIBNAME).$(LIBEXT)
endif

ifeq ($(BUILD_OS_TYPE), freertos)
EXTERNAL_LNKCMD_FILE_LOCAL = $(MCUSW_INSTALL_PATH)/mcuss_demos/inter_core_comm/ipc_remote/overrides/$(SOC)/linker_$(ISA)_$(CORE)_freertos.lds
else
EXTERNAL_LNKCMD_FILE_LOCAL = $(MCUSW_INSTALL_PATH)/mcuss_demos/inter_core_comm/ipc_remote/overrides/$(SOC)/linker_$(ISA)_$(CORE)_sysbios.lds
endif

PACKAGE_SRCS_COMMON = .


ifeq ($(MCUSW_UART_ENABLE),TRUE)
    CFLAGS_LOCAL_COMMON += -DUART_ENABLED
endif
# Core/SoC/platform specific source files and CFLAGS
# Example:
# SRCS_<core/SoC/platform-name> =
# CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
include $(MCUSW_INSTALL_PATH)/build/common.mk
