include $(MCUSW_INSTALL_PATH)/build/Rules.make

MODULE_NAME = cdd_ipc

ifeq ($(SOC),$(filter $(SOC), am62x am62ax j721e j7200 am62px j722s))
SRCDIR = src $(mcal_PATH) $(mcal_PATH)/CddIpc/ipc_baremetal_hw/src $(mcal_PATH)/CddIpc/ipc_baremetal_hw/soc/V1 $(mcal_PATH)/CddIpc/ipc_baremetal_hw/mailbox_hw/V0/priv
endif
ifeq ($(SOC), j721s2)
SRCDIR = src $(mcal_PATH) $(mcal_PATH)/CddIpc/ipc_baremetal_hw/src $(mcal_PATH)/CddIpc/ipc_baremetal_hw/soc/V2 $(mcal_PATH)/CddIpc/ipc_baremetal_hw/mailbox_hw/V0/priv
endif
ifeq ($(SOC), j784s4)
SRCDIR = src $(mcal_PATH) $(mcal_PATH)/CddIpc/ipc_baremetal_hw/src $(mcal_PATH)/CddIpc/ipc_baremetal_hw/soc/V3 $(mcal_PATH)/CddIpc/ipc_baremetal_hw/mailbox_hw/V0/priv
endif
INCDIR = src $(mcal_PATH) $(mcal_PATH)/CddIpc/ipc_baremetal_hw
ifeq ($(SOC),$(filter $(SOC), am62x am62ax am62px j722s))
INCDIR +=  $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/hw_include
INCDIR +=  $(mcu_plus_sdk_INSTALL_PATH)/source/drivers/sciclient/include
endif
# List all the external components/interfaces, whose interface header files
# need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = autosarBSW autosarConfig autosarCompiler pdk
# List all the internal submodules whose interface header files
# need to be included for this module
INCLUDE_INTERNAL_INTERFACES = cdd_ipc bsw_stubs

MODULE_SRC_BASE_PATH := $(mcal_PATH)/CddIpc

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON += Cdd_Ipc.c Cdd_IpcIrq.c
SRCS_COMMON += ipc_mp.c ipc_utils.c ipc_mailbox.c ipc_virtio.c ipc_api.c ipc_soc.c hw_mailbox.c

PACKAGE_SRCS_COMMON = .
CFLAGS_LOCAL_COMMON = $(MCUSW_CFLAGS)
CFLAGS_LOCAL_COMMON += $(IPC_CFLAGS) -DIPC_EXCLUDE_CTRL_TASKS -DIPC_ONE_CONTEXT_FOR_HISRGATE_HWIGATE -DIPC_SUPPORT_SCICLIENT -DIPC_EXCLUDE_POLLED_RX -DMAILBOX_INTERRUPT_MODE
# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
  MAKERULEDIR := $(MCUSW_INSTALL_PATH)/build/makerules
  export MAKERULEDIR
include $(MCUSW_INSTALL_PATH)/build/common.mk
# OBJs and libraries are built by using rule defined in rules_<target>.mk
#     and need not be explicitly specified here

# Nothing beyond this point
