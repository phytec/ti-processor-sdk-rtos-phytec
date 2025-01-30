# This file contains a list of extension kernel specific static libraries
# to be included in the PC executables.  It is put in this separate file
# to make it easier to add/extend kernels without needing to modify
# several concerto.mak files which depend on kernel libraries.

ifeq ($(TARGET_CPU),x86_64)

STATIC_LIBS += vx_vxu

STATIC_LIBS += vx_utils
STATIC_LIBS += vx_tiovx_tidl_tests
STATIC_LIBS += vx_kernels_tidl vx_target_kernels_tidl vx_target_kernels_ivision_common tidl_algo tidl_priv_algo tidl_obj_algo tidl_custom tidl_avx_kernels

ifeq ($(SOC), j722s)
STATIC_LIBS += C7524-MMA2_256-host-emulation
else
STATIC_LIBS += $(C7X_VERSION)-host-emulation
endif

# Uncomment below to link to TIDL/MMALIB in host emulation mode instead of natural C mode on PC
# STATIC_LIBS += mmalib_x86_64 mmalib_cn_x86_64 common_x86_64
# ADDITIONAL_STATIC_LIBS += dmautils.lib udma.lib sciclient.lib ti.csl.lib ti.osal.lib

SYS_SHARED_LIBS += rt dl png z

LDIRS+=$(CGT7X_ROOT)/host_emulation
LDIRS+=$(MMALIB_PATH)/lib/$(C7X_VERSION)/$(TARGET_BUILD)
LDIRS+=$(TIDL_PATH)/ti_dl/lib/$(TARGET_SOC)/$(TARGET_PLATFORM)/algo/$(TARGET_BUILD)

ifeq ($(RTOS_SDK), mcu_plus_sdk)
LDIRS+= $(MCU_PLUS_SDK_PATH)/source/drivers/dmautils/lib/
else
LDIRS+= $(PDK_PATH)/packages/ti/drv/udma/lib/$(SOC)_hostemu/c7x-hostemu/$(TARGET_BUILD)
LDIRS+= $(PDK_PATH)/packages/ti/csl/lib/$(SOC)/c7x-hostemu/$(TARGET_BUILD)
LDIRS+= $(PDK_PATH)/packages/ti/drv/sciclient/lib/$(SOC)_hostemu/c7x-hostemu/$(TARGET_BUILD)
LDIRS+= $(PDK_PATH)/packages/ti/osal/lib/nonos/$(SOC)/c7x-hostemu/$(TARGET_BUILD)
endif

LDIRS       += $(APP_UTILS_PATH)/lib/PC/$(TARGET_CPU)/$(TARGET_OS)/$(TARGET_BUILD)
STATIC_LIBS += app_utils_mem
LDIRS       += $(VISION_APPS_PATH)/lib/PC/$(TARGET_CPU)/$(TARGET_OS)/$(TARGET_BUILD)
STATIC_LIBS += app_utils_init

MCU_PLUS_SDK_LIBS =
ifeq ($(SOC), am62a)
  MCU_PLUS_SDK_LIBS += dmautils.am62ax.c75x.ti-c7x-hostemu.$(TARGET_BUILD).lib
endif
ifeq ($(SOC), j722s)
  MCU_PLUS_SDK_LIBS += dmautils.j722s.c75ssx-0.ti-c7x-hostemu.$(TARGET_BUILD).lib
endif

PDK_LIBS =
ifeq ($(SOC),$(filter $(SOC), j721e j721s2 j784s4))
PDK_LIBS += udma.lib
PDK_LIBS += sciclient.lib
PDK_LIBS += ti.osal.lib
PDK_LIBS += dmautils.lib
PDK_LIBS += ti.csl.lib
endif

MMA_LIBS =
MMA_LIBS += mmalib_cn_x86_64
MMA_LIBS += mmalib_x86_64
MMA_LIBS += common_x86_64

ADDITIONAL_STATIC_LIBS += $(PDK_LIBS)
ADDITIONAL_STATIC_LIBS += $(MCU_PLUS_SDK_LIBS)

STATIC_LIBS += $(MMA_LIBS)

endif #ifeq ($(TARGET_CPU),x86_64)