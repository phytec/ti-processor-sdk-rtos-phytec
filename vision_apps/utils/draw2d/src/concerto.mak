
ifeq ($(TARGET_CPU),$(filter $(TARGET_CPU), x86_64 A72 A53))

include $(PRELUDE)
TARGET      := app_utils_draw2d
TARGETTYPE  := library

CSOURCES    := $(call all-c-files)

IDIRS       := $(VISION_APPS_PATH)

include $(FINALE)

endif
