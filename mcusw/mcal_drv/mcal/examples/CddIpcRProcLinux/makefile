include $(MCUSW_INSTALL_PATH)/build/Rules.make

#Application name
APP_NAME = cdd_ipc_app_rc_linux

INCDIR =  $(mcal_PATH)
SRCDIR = ./
SRCDIR = ./soc/$(SOC)/$(CORE) $(autosarConfigSrc_PATH)/CddIpcLinux_Demo_Cfg/output/generated/soc/$(SOC)/$(CORE)/src $(app_utils_PATH)/src soc/$(SOC)
INCDIR += ./
INCDIR += $(autosarConfigSrc_PATH)/IntRtr_Cfg/soc/$(SOC)/$(CORE)/

# Compiler path
ifeq ($(SOC),$(filter $(SOC), am62x))
COMPILER_PATH = $(TOOLCHAIN_PATH_R5)
endif

#Path to .c source files
ifeq ($(SOC),$(filter $(SOC), am62ax am62x am62px j722s))
SRCDIR += $(mcal_PATH)/examples/CddIpcRProcLinux/soc/$(SOC)/Generated_Dpl
endif

#Path to .h source files

ifeq ($(SOC),$(filter $(SOC), am62ax am62x am62px j722s))
INCDIR += $(mcal_PATH)/examples/CddIpcRProcLinux/soc/$(SOC)/Generated_Dpl
INCDIR += $(mcu_plus_sdk_INSTALL_PATH)/source
endif
ifeq ($(SOC),$(filter $(SOC), am62ax am62px j722s))
INCDIR +=  $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/sciclient
INCDIR +=  $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/sciclient/include
INCDIR +=  $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/drivers/hw_include
endif
ifeq ($(SOC),$(filter $(SOC), am62x))
INCDIR += $(COMPILER_PATH)/include
INCDIR += $(MCUSW_INSTALL_PATH)/mcal_drv/mcal
INCDIR += $(MCUSW_INSTALL_PATH)/mcal_drv/mcal/CddIpc/include
INCDIR += $(MCUSW_INSTALL_PATH)/mcal_drv/mcal/examples/utils
INCDIR += $(MCUSW_INSTALL_PATH)/mcal_drv/mcal/Bsw_Stubs/Os/inc
INCDIR += $(MCUSW_INSTALL_PATH)/mcal_drv/mcal/Bsw_Stubs/AsrGnrl/inc
INCDIR += $(MCUSW_INSTALL_PATH)/mcal_drv/mcal/Bsw_Stubs/Rte/inc
INCDIR += $(MCUSW_INSTALL_PATH)/mcal_drv/mcal/Bsw_Stubs/Det/inc
INCDIR += $(MCUSW_INSTALL_PATH)/mcal_drv/mcal/Bsw_Stubs/Dem/inc
INCDIR += $(MCUSW_INSTALL_PATH)/mcal_drv/mcal/Bsw_Stubs/MemMap/inc
INCDIR += $(mcu_plus_sdk_INSTALL_PATH)/source/kernel/freertos/FreeRTOS-Kernel/include
INCDIR += $(mcu_plus_sdk_INSTALL_PATH)/source/kernel/freertos/config/am62x/r5f
INCDIR += $(mcu_plus_sdk_INSTALL_PATH)/source/kernel/freertos/portable/TI_ARM_CLANG/ARM_CR5F
endif

# List all the external components/interfaces, whose interface header files
# need to be included for this component
ifeq ($(SOC),$(filter $(SOC), j721e j7200 j721s2 j784s4))
 INCLUDE_EXTERNAL_INTERFACES = pdk autosarBSW autosarConfig autosarCompiler
INCLUDE_EXTERNAL_INTERFACES += ipc_baremetal
endif



INCLUDE_INTERNAL_INTERFACES = cdd_ipc bsw_stubs app_utils

# List all the components required by the application
COMP_LIST_COMMON = cdd_ipc bsw_stubs app_utils
# List all the components required by the application
ifeq ($(SOC),$(filter $(SOC), am62ax am62x am62px j722s))
INCLUDE_EXTERNAL_INTERFACES = autosarBSW autosarConfig autosarCompiler
COMP_LIST_COMMON = cdd_ipc bsw_stubs app_utils hw_init
endif
# Required for prints
ifeq ($(SOC),$(filter $(SOC), am62ax am62px j722s))
COMP_LIST_COMMON +=  generate_dpl 
endif
# List all the external libs required by the application
EXT_LIB_LIST_COMMON = $(csl_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(csl_LIBNAME).$(LIBEXT)
EXT_LIB_LIST_COMMON += $(csl_init_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(csl_init_LIBNAME).$(LIBEXT)
# Link with the baremetal IPC
EXT_LIB_LIST_COMMON += $(ipc_baremetal_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(ipc_baremetal_LIBNAME).$(LIBEXT)

# Required for uart
EXT_LIB_LIST_COMMON += $(uart_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(uart_LIBNAME).$(LIBEXT)
EXT_LIB_LIST_COMMON += $(board_LIBPATH)/$(BOARD)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(board_LIBNAME).$(LIBEXT)
# Required by board
EXT_LIB_LIST_COMMON += $(i2c_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(i2c_LIBNAME).$(LIBEXT)


# Common source files and CFLAGS across all platforms and cores
ifeq ($(SOC),$(filter $(SOC), j721e j7200 j721s2 j784s4))
SRCS_COMMON = CddIpcAppRProcLinux.c Cdd_IpcCfg.c CddIpcR5Mpu.c CddIpcAppStartup.c IpcTrace.c
endif

ifeq ($(SOC),$(filter $(SOC), am62x am62ax am62px j722s))
SRCS_COMMON = CddIpcAppRProcLinux.c
SRCS_COMMON += Cdd_IpcCfg.c
SRCS_COMMON += CddIpcAppStartup.c
SRCS_COMMON += IpcTrace.c
SRCS_COMMON += ti_drivers_config.c
SRCS_COMMON += ti_drivers_open_close.c
SRCS_COMMON += ti_board_config.c
SRCS_COMMON += ti_board_open_close.c
SRCS_COMMON += ti_dpl_config.c
SRCS_COMMON += ti_pinmux_config.c
SRCS_COMMON += ti_power_clock_config.c


endif
# App utils file includes memory sections defined in this module
# which is used to check memory section corruption
SRCS_COMMON += app_utils_cdd_ipc.c

PACKAGE_SRCS_COMMON = .

EXTERNAL_LNKCMD_FILE_LOCAL = $(MCUSW_INSTALL_PATH)/mcal_drv/mcal/examples/CddIpcRProcLinux/soc/$(SOC)/$(CORE)/linker_r5_rc_linux.lds

CFLAGS_LOCAL_COMMON = $(MCUSW_CFLAGS)
LNKFLAGS_LOCAL_COMMON = $(MCUSW_LNKFLAGS)
ifeq ($(MCUSW_UART_ENABLE),TRUE)
    CFLAGS_LOCAL_COMMON += -DUART_ENABLED
endif
# Core/SoC/platform specific source files and CFLAGS
# Example:
# SRCS_<core/SoC/platform-name> =
# CFLAGS_LOCAL_<core/SoC/platform-name> =
#Application specific libraries
ifeq ($(SOC),$(filter $(SOC), am62ax))
  ifeq ($(CORE),mcu0_0)
   EXT_LIB_LIST_COMMON = $(mcu_plus_sdk_INSTALL_PATH)/source/kernel/nortos/lib/nortos.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
   EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/board/lib/board.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
   EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/lib/drivers.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
   EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/device_manager/rm_pm_hal/lib/rm_pm_hal.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
   EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/device_manager/rm_pm_hal/lib/rm_pm_hal.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
   EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/device_manager/self_reset/lib/self_reset.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
  endif
endif
ifeq ($(SOC),$(filter $(SOC), am62x))
  ifeq ($(CORE),mcu0_0)
EXT_LIB_LIST_COMMON = $(mcu_plus_sdk_INSTALL_PATH)/source/board/lib/board.am62x.r5f.ti-arm-clang.$(BUILD_PROFILE).lib
EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/device_manager/dm_stub/lib/dm_stub.am62x.r5f.ti-arm-clang.$(BUILD_PROFILE).lib
EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/lib/drivers.am62x.r5f.ti-arm-clang.$(BUILD_PROFILE).lib
EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/device_manager/rm_pm_hal/lib/rm_pm_hal.am62x.r5f.ti-arm-clang.$(BUILD_PROFILE).lib
EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/device_manager/sciclient_direct/lib/sciclient_direct.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/device_manager/self_reset/lib/self_reset.am62x.r5f.ti-arm-clang.$(BUILD_PROFILE).lib
EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/kernel/freertos/lib/freertos.am62x.r5f.ti-arm-clang.$(BUILD_PROFILE).lib
EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/device_manager/sciserver/lib/sciserver.am62x.r5f.ti-arm-clang.$(BUILD_PROFILE).lib
EXT_LIB_LIST_COMMON += $(CGT_PATH)/lib/libc.a
EXT_LIB_LIST_COMMON += $(CGT_PATH)/lib/libsysbm.a
EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/device_manager/dm_stub/lib/dm_stub.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/device_manager/sciserver/lib/sciserver.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
  endif
CFLAGS_LOCAL_COMMON += -mcpu=cortex-r5
CFLAGS_LOCAL_COMMON += -mfloat-abi=hard
CFLAGS_LOCAL_COMMON += -mfpu=vfpv3-d16
CFLAGS_LOCAL_COMMON += -mthumb
CFLAGS_LOCAL_COMMON += -Wall
CFLAGS_LOCAL_COMMON += -Werror
CFLAGS_LOCAL_COMMON += -g
CFLAGS_LOCAL_COMMON += -Wno-gnu-variable-sized-type-not-at-end
CFLAGS_LOCAL_COMMON += -Wno-unused-function

LNKFLAGS_LOCAL_COMMON += -Wl,--diag_suppress=10063
LNKFLAGS_LOCAL_COMMON += -Wl,--ram_model
LNKFLAGS_LOCAL_COMMON += -Wl,--reread_libs

endif
ifeq ($(SOC),$(filter $(SOC), am62px j722s))
	ifeq ($(CORE),mcu0_0)
      EXT_LIB_LIST_COMMON = $(mcu_plus_sdk_INSTALL_PATH)/source/kernel/nortos/lib/nortos.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
      EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/board/lib/board.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
      EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/lib/drivers.$(SOC).mcu-$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
      EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/device_manager/rm_pm_hal/lib/rm_pm_hal.$(SOC).wkup-$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
      EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/device_manager/self_reset/lib/self_reset.$(SOC).wkup-$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
	endif
endif
ifeq ($(SOC),$(filter $(SOC), j721e j7200 j721s2 j784s4))
  ifeq ($(CORE),mcu1_0)
  EXT_LIB_LIST_COMMON += $(csl_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(csl_LIBNAME).$(LIBEXT)
  EXT_LIB_LIST_COMMON += $(sciclient_direct_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(sciclient_direct_LIBNAME).$(LIBEXT)
  EXT_LIB_LIST_COMMON += $(sciserver_tirtos_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(sciserver_tirtos_LIBNAME).$(LIBEXT)
  EXT_LIB_LIST_COMMON += $(rm_pm_hal_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(rm_pm_hal_LIBNAME).$(LIBEXT)
  # Required for uart
  EXT_LIB_LIST_COMMON += $(uart_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(uart_LIBNAME).$(LIBEXT)
  #Board module
  EXT_LIB_LIST_COMMON += $(board_LIBPATH)/$(BOARD)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(board_LIBNAME).$(LIBEXT)
  # Required by board
	EXT_LIB_LIST_COMMON += $(osal_freertos_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(osal_freertos_LIBNAME).$(LIBEXT)
	EXT_LIB_LIST_COMMON += $(freertos_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(freertos_LIBNAME).$(LIBEXT)
	EXT_LIB_LIST_COMMON += $(csl_init_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(csl_init_LIBNAME).$(LIBEXT)
  else
  EXT_LIB_LIST_COMMON += $(sciclient_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(sciclient_LIBNAME).$(LIBEXT)
  EXT_LIB_LIST_COMMON += $(osal_nonos_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(osal_nonos_LIBNAME).$(LIBEXT)
endif
endif
# Include common make files
include $(MCUSW_INSTALL_PATH)/build/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk
# and need not be explicitly specified here

# Nothing beyond this point