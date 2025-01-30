ifeq ($(TARGET_OS), $(filter $(TARGET_OS), QNX FREERTOS SAFERTOS))

include $(PRELUDE)

TARGET      := app_utils_rtos
TARGETTYPE  := library

ifeq ($(TARGET_OS), $(filter $(TARGET_OS), FREERTOS SAFERTOS))
ifeq ($(RTOS_SDK),mcu_plus_sdk)
CSOURCES    := app_rtos_mcu_plus_sdk.c
else
CSOURCES    := app_rtos_pdk.c
endif
else
CSOURCES    := app_rtos_pdk.c
endif

include $(FINALE)

endif
