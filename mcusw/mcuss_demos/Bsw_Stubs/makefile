
include $(MCUSW_INSTALL_PATH)/build/Rules.make

MODULE_NAME = bsw_stubs_rtos

include $(MCUSW_INSTALL_PATH)/build/platform.mk

#Accommodating Changes for introducing multiple Tool Chains
SRCDIR = Det/src Dem/src Rte/src CanIf/src Os/src EcuM/src
INCDIR = $(mcal_PATH)

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk autosarBSW autosarCompiler
ifeq ($(BUILD_OS_TYPE), tirtos)
    INCLUDE_EXTERNAL_INTERFACES += xdc bios
endif

INCLUDE_INTERNAL_INTERFACES = bsw_stubs_rtos app_utils
UtilsPriv_INCLUDE =

SRCS_COMMON += Det.c Dem.c Os.c
SRCS_COMMON += SchM_Spi.c SchM_Can.c SchM_Dio.c
SRCS_COMMON += SchM_Eth.c SchM_EthTrcv.c SchM_Gpt.c
SRCS_COMMON += EcuM_Cbk.c
SRCS_COMMON += CanIf_Cbk.c CanIf.c
SRCS_COMMON += SchM_Cdd_Ipc.c

# List all the external ibs required by the application
ifeq ($(BUILD_OS_TYPE), tirtos)
EXT_LIB_LIST_COMMON += $(osal_tirtos_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(osal_tirtos_LIBNAME).$(LIBEXT)
endif

PACKAGE_SRCS_COMMON = .
CFLAGS_LOCAL_COMMON = $(MCUSW_CFLAGS)

# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
include $(MCUSW_INSTALL_PATH)/build/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk
#     and need not be explicitly specified here

# Nothing beyond this point
