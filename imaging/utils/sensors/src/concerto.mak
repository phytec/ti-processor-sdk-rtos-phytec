ifeq ($(TARGET_OS),$(filter $(TARGET_OS),SYSBIOS FREERTOS SAFERTOS))
ifeq ($(TARGET_CPU),R5F)

include $(PRELUDE)
TARGET      := app_utils_sensors
TARGETTYPE  := library

CSOURCES    := app_sensors.c

include $(FINALE)

endif
endif
