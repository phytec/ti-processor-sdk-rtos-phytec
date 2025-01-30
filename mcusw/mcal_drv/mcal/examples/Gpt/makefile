include $(MCUSW_INSTALL_PATH)/build/Rules.make

APP_NAME = gpt_app

SRCDIR = . ./soc/$(SOC)/$(CORE) $(autosarConfigSrc_PATH)/Gpt_Demo_Cfg/output/generated/soc/$(SOC)/$(CORE)/src $(app_utils_PATH)/src
INCDIR = . $(autosarConfigSrc_PATH)/IntRtr_Cfg/soc/$(SOC)/$(CORE)/ ./soc/$(SOC)/$(CORE)
INCDIR += $(mcal_PATH)

ifeq ($(SOC),$(filter $(SOC), am62x am62ax am62px j722s))
	INCDIR += . $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/sciclient/include
	INCDIR += .$(mcu_plus_sdk_INSTALL_PATH)/source/kernel/dpl
	INCDIR += . $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/sciclient
	INCDIR += .$(mcu_plus_sdk_INSTALL_PATH)/source/drivers/drivers/hw_include
	INCDIR += . $(mcu_plus_sdk_INSTALL_PATH)/source/
	INCDIR += . $(mcal_PATH)/examples/Gpt/soc/$(SOC)/Generated_Dpl
	SRCDIR += . $(mcal_PATH)/examples/Gpt/soc/$(SOC)/Generated_Dpl
endif

# List all the external components/interfaces, whose interface header files
# need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk autosarBSW autosarConfig autosarCompiler
INCLUDE_INTERNAL_INTERFACES = gpt bsw_stubs app_utils
ifeq ($(SOC),$(filter $(SOC), am62x am62ax am62px j722s))
	IMCLUDE_INTERNAL_INTERFACES += hw_init
endif

# List all the components required by the application
COMP_LIST_COMMON = gpt
# To be replaced with actual AUTOSAR components (such as Os, Dem, Det)
COMP_LIST_COMMON += bsw_stubs
# Required for prints
COMP_LIST_COMMON += app_utils
ifeq ($(SOC),$(filter $(SOC), am62x am62ax am62px j722s))
	COMP_LIST_COMMON += hw_init
endif

# List all the external ibs required by the application
# Required for CSL macros used by the driver
ifeq ($(SOC),$(filter $(SOC), j721e j7200 j721s2 j784s4))
	# List all the external ibs required by the application
	# Required for CSL macros used by the driver
	EXT_LIB_LIST_COMMON = $(csl_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(csl_LIBNAME).$(LIBEXT)
	EXT_LIB_LIST_COMMON += $(csl_init_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(csl_init_LIBNAME).$(LIBEXT)
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
# Required for sciclient required by UDMA driver
EXT_LIB_LIST_COMMON += $(osal_nonos_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(osal_nonos_LIBNAME).$(LIBEXT)
# Required for OS less interrupt handling
# Required for uart
EXT_LIB_LIST_COMMON += $(uart_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(uart_LIBNAME).$(LIBEXT)
EXT_LIB_LIST_COMMON += $(board_LIBPATH)/$(BOARD)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(board_LIBNAME).$(LIBEXT)
# Required by board
EXT_LIB_LIST_COMMON += $(i2c_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(i2c_LIBNAME).$(LIBEXT)
else ifeq ($(SOC),$(filter $(SOC), am62x))
	ifeq ($(CORE),mcu0_0)
	  EXT_LIB_LIST_COMMON = $(mcu_plus_sdk_INSTALL_PATH)/source/kernel/nortos/lib/nortos.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
	  EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/board/lib/board.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
	  EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/lib/drivers.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
	  EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/device_manager/rm_pm_hal/lib/rm_pm_hal.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
	  EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/device_manager/sciclient_direct/lib/sciclient_direct.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
	  EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/device_manager/rm_pm_hal/lib/rm_pm_hal.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
	  EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/device_manager/self_reset/lib/self_reset.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
 	  EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/device_manager/sciserver/lib/sciserver.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
	  EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/device_manager/dm_stub/lib/dm_stub.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
	endif
else ifeq ($(SOC),$(filter $(SOC), am62ax))
	ifeq ($(CORE),mcu0_0)
	  EXT_LIB_LIST_COMMON = $(mcu_plus_sdk_INSTALL_PATH)/source/kernel/nortos/lib/nortos.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
	  EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/board/lib/board.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
	  EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/lib/drivers.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
	endif
else ifeq ($(SOC),$(filter $(SOC), am62px j722s))
	ifeq ($(CORE),mcu0_0)
	  EXT_LIB_LIST_COMMON = $(mcu_plus_sdk_INSTALL_PATH)/source/kernel/nortos/lib/nortos.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
	  EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/board/lib/board.$(SOC).$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
	  EXT_LIB_LIST_COMMON += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/lib/drivers.$(SOC).mcu-$(ISA_EXT).ti-arm-clang.$(BUILD_PROFILE).lib
	endif
endif
# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON = GptApp.c GptApp_Startup.c Gpt_PBcfg.c Gpt_Cfg.c
# App utils file includes memory sections defined in this module
# which is used to check memory section corruption
SRCS_COMMON += app_utils_gpt.c
ifeq ($(SOC),$(filter $(SOC), am62x am62ax am62px j722s))
	SRCS_COMMON += ti_drivers_config.c ti_drivers_open_close.c ti_board_config.c ti_board_open_close.c ti_dpl_config.c ti_pinmux_config.c ti_power_clock_config.c
endif

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
