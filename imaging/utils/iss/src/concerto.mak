ifeq ($(TARGET_CPU),$(filter $(TARGET_CPU), x86_64 A72 A53 R5F))

include $(PRELUDE)

TARGET      := app_utils_iss
TARGETTYPE  := library
IDIRS += $(HOST_ROOT)/sensor_drv/include
IDIRS += $(HOST_ROOT)/kernels/include

ifeq ($(TARGET_PLATFORM),PC)

CSOURCES    := app_iss_x86.c app_iss_common.c

else

ifeq ($(TARGET_CPU),$(filter $(TARGET_CPU), A72 A53))
CSOURCES    := app_iss_common.c app_iss.c
endif

ifeq ($(TARGET_CPU),$(filter $(TARGET_CPU), R5F))
CSOURCES    := app_iss.c
endif

endif

include $(FINALE)

endif
