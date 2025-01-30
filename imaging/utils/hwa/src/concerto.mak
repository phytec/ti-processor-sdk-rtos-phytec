ifeq ($(TARGET_CPU),$(filter $(TARGET_CPU), x86_64 A72 A53 R5F))

include $(PRELUDE)

TARGET      := app_utils_hwa
TARGETTYPE  := library

ifeq ($(TARGET_CPU),$(filter $(TARGET_CPU), x86_64))
CSOURCES += app_hwa_api_x86.c
endif

ifeq ($(TARGET_CPU),$(filter $(TARGET_CPU), A72 A53))
CSOURCES    := app_hwa_api.c
endif

ifeq ($(TARGET_CPU),R5F)
CSOURCES    := app_hwa.c app_csi.c
endif

ifeq ($(RTOS_SDK), mcu_plus_sdk)
IDIRS       += $(MCU_PLUS_SDK_PATH)/source
endif

IDIRS += $(HOST_ROOT)/kernels/include

include $(FINALE)

endif
