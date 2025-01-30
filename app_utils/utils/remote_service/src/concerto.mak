ifneq ($(TARGET_PLATFORM),PC)

include $(PRELUDE)
TARGET      := app_utils_remote_service
TARGETTYPE  := library

CSOURCES := app_remote_service_test.c
ifeq ($(TARGET_OS),$(filter $(TARGET_OS),SYSBIOS FREERTOS SAFERTOS QNX))
CSOURCES += app_remote_service.c
endif
ifeq ($(TARGET_OS),LINUX)
CSOURCES += app_remote_service_linux.c
endif

ifeq ($(RTOS_SDK),mcu_plus_sdk)
IDIRS       += $(VISION_APPS_PATH)/platform/$(SOC)/rtos
endif

include $(FINALE)

endif

