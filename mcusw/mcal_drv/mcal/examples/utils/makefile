
include $(MCUSW_INSTALL_PATH)/build/Rules.make

MODULE_NAME = app_utils

ifeq ($(MAKERULEDIR), )
#Makerule path not defined, define this and assume relative path from ROOTDIR
  MAKERULEDIR := $(MCUSW_INSTALL_PATH)/build
  export MAKERULEDIR
endif
include $(MAKERULEDIR)/platform.mk

#Accommodating Changes for introducing multiple Tool Chains
SRCDIR = src
INCDIR += . $(mcal_PATH)
INCDIR += .$(mcal_PATH)/hw_include/arch/r5
INCDIR += .$(mcal_PATH)/hw_include

ifeq ($(SOC),$(filter $(SOC), am62x am62ax am62px j722s))
SRCDIR += .$(mcal_PATH)/hw_include/arch/r5
INCDIR += .$(mcu_plus_sdk_INSTALL_PATH)/source/kernel/dpl
INCDIR += . $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/sciclient
INCDIR += .$(mcu_plus_sdk_INSTALL_PATH)/source/drivers/sciclient/include
INCDIR += .$(mcal_PATH)/hw_include/arch/r5
INCDIR += . $(mcu_plus_sdk_INSTALL_PATH)/source 
INCDIR += .$(mcu_plus_sdk_INSTALL_PATH)/source/drivers
endif


# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk autosarCompiler
INCLUDE_INTERNAL_INTERFACES = app_utils
UtilsPriv_INCLUDE =

SRCS_COMMON += app_utils.c app_r5f_Pmu.c rti.c

CFLAGS_LOCAL_COMMON = $(MCUSW_CFLAGS)
LNKFLAGS_LOCAL_COMMON = $(MCUSW_LNKFLAGS)

ifeq ($(MCUSW_UART_ENABLE),TRUE)
ifeq ($(SOC),$(filter $(SOC), am62x am62ax am62px j722s))
    CFLAGS_LOCAL_COMMON += -DUART_DISABLED
else
    CFLAGS_LOCAL_COMMON += -DUART_ENABLED
endif
endif
# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =


  MAKERULEDIR := $(MCUSW_INSTALL_PATH)/build/makerules
  export MAKERULEDIR

include $(MCUSW_INSTALL_PATH)/build/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk
#     and need not be explicitly specified here

# Nothing beyond this point
