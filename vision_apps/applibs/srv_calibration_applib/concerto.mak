ifeq ($(TARGET_CPU), $(filter $(TARGET_CPU), x86_64 A72 A53))

include $(PRELUDE)
TARGET      := vx_applib_srv_calibration
TARGETTYPE  := library
CSOURCES    := $(call all-c-files)

IDIRS       += $(VISION_APPS_PATH)/kernels/srv/include

ifeq ($(SOC),j722s)
SKIPBUILD=1
endif

include $(FINALE)

endif
