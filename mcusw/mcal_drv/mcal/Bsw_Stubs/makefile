
include $(MCUSW_INSTALL_PATH)/build/Rules.make

MODULE_NAME = bsw_stubs

ifeq ($(MAKERULEDIR), )
#Makerule path not defined, define this and assume relative path from ROOTDIR
  MAKERULEDIR := $(MCUSW_INSTALL_PATH)/build/makerules
  export MAKERULEDIR
endif
include $(MCUSW_INSTALL_PATH)/build/platform.mk

#Accomodating Changes for introducing multiple Tool Chains
SRCDIR = Det/src Dem/src Rte/src CanIf/src Os/src EcuM/src Fee/src MemIf/src EthIf/src
INCDIR += . $(mcal_PATH)

ifeq ($(SOC),$(filter $(SOC), am62x am62ax am62px j722s))
INCDIR += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/sciclient
INCDIR += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/sciclient/include
INCDIR += $(mcu_plus_sdk_INSTALL_PATH)/source/kernel/dpl
INCDIR += $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/drivers/hw_include
INCDIR += $(mcu_plus_sdk_INSTALL_PATH)/source/
INCDIR += $(mcu_plus_sdk_INSTALL_PATH)
endif

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk autosarBSW autosarCompiler
INCLUDE_INTERNAL_INTERFACES = bsw_stubs app_utils
UtilsPriv_INCLUDE =

SRCS_COMMON += Det.c Dem.c Os.c
SRCS_COMMON += SchM_Spi.c SchM_Can.c SchM_Dio.c SchM_Fls.c SchM_Mcu.c
SRCS_COMMON += SchM_Gpt.c SchM_Pwm.c SchM_Adc.c SchM_Icu.c
SRCS_COMMON += SchM_Eth.c SchM_EthTrcv.c
SRCS_COMMON += SchM_Cdd_Ipc.c
SRCS_COMMON += EcuM_Cbk.c
SRCS_COMMON += CanIf_Cbk.c Fee_Cbk.c
SRCS_COMMON += EthIf_Cbk.c

PACKAGE_SRCS_COMMON = .
CFLAGS_LOCAL_COMMON = $(MCUSW_CFLAGS)
ifeq ($(MCUSW_ETH_WRAPPER_ENABLE),TRUE)
  CFLAGS_LOCAL_COMMON += -DETH_WRAPPER_ENABLED=STD_ON
else
  CFLAGS_LOCAL_COMMON += -DETH_WRAPPER_ENABLED=STD_OFF
endif

# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
#Makerule path not defined, define this and assume relative path from ROOTDIR
  MAKERULEDIR := $(MCUSW_INSTALL_PATH)/build/makerules
  export MAKERULEDIR

include $(MCUSW_INSTALL_PATH)/build/common.mk

# Nothing beyond this point
