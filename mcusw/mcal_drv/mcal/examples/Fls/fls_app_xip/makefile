include $(MCUSW_INSTALL_PATH)/build/Rules.make

APP_NAME = fls_app_xip

BUILD_OS_TYPE=baremetal

SRCDIR = . $(autosarConfigSrc_PATH)/Fls_Demo_Cfg/output/generated/soc/$(SOC)/$(CORE)/src $(app_utils_PATH)/src soc/$(SOC)/$(CORE)
INCDIR = . soc/$(SOC)/$(CORE) $(MCUSW_INSTALL_PATH)/mcal_drv/mcal/Fls
INCDIR += $(autosarConfigSrc_PATH)/IntRtr_Cfg/soc/$(SOC)/$(CORE)/
INCDIR += $(mcal_PATH)

# List all the external components/interfaces, whose interface header files
# need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = autosarBSW autosarConfig autosarCompilers pdk
INCLUDE_INTERNAL_INTERFACES = bsw_stubs app_utils fls brd
# List all the components required by the application
COMP_LIST_COMMON = fls
COMP_LIST_COMMON += brd
# To be replaced with actual AUTOSAR components (such as Os, Dem, Det)
COMP_LIST_COMMON += bsw_stubs
# Required for prints
COMP_LIST_COMMON += app_utils
# List all the external ibs required by the application
# Required for CSL macros used by the driver
EXT_LIB_LIST_COMMON = $(csl_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(csl_LIBNAME).$(LIBEXT)
EXT_LIB_LIST_COMMON += $(csl_init_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(csl_init_LIBNAME).$(LIBEXT)
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
EXT_LIB_LIST_COMMON += $(osal_nonos_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(osal_nonos_LIBNAME).$(LIBEXT)
# Required for uart
EXT_LIB_LIST_COMMON += $(uart_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(uart_LIBNAME).$(LIBEXT)
EXT_LIB_LIST_COMMON += $(board_LIBPATH)/$(BOARD)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(board_LIBNAME).$(LIBEXT)
# Required by board
EXT_LIB_LIST_COMMON += $(i2c_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(i2c_LIBNAME).$(LIBEXT)
# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON = Fls_Cfg_xip.c Fls_PBcfg.c FlsApp.c FlsApp_Startup.c
# App utils file includes memory sections defined in this module
# which is used to check memory section corruption
SRCS_COMMON += app_utils_fls.c

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
