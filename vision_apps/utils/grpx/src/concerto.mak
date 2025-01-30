ifneq ($(TARGET_PLATFORM),PC)
ifeq ($(TARGET_OS),$(filter $(TARGET_OS),LINUX QNX))

include $(PRELUDE)
TARGET      := app_utils_grpx
TARGETTYPE  := library

CSOURCES    := app_grpx.c
CSOURCES    += app_grpx_cpu_load.c
CSOURCES    += app_grpx_hwa_load.c
CSOURCES    += app_grpx_ddr_load.c

include $(FINALE)

endif
endif

ifeq ($(TARGET_PLATFORM),PC)
ifeq ($(TARGET_OS),LINUX)


include $(PRELUDE)
TARGET      := app_utils_grpx
TARGETTYPE  := library

CSOURCES    := app_grpx_null.c

include $(FINALE)

endif
endif
