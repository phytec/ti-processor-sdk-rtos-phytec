ifneq ($(TARGET_PLATFORM),PC)
ifeq ($(TARGET_OS),$(filter $(TARGET_OS),SYSBIOS FREERTOS SAFERTOS))

include $(PRELUDE)
TARGET      := app_utils_udma
TARGETTYPE  := library

CSOURCES    := app_udma.c

ifeq ($(SOC),$(filter $(SOC),j721e j721s2 j784s4))
CSOURCES    += app_udma_utils.c
CSOURCES    += app_udma_test.c
endif

ifeq ($(RTOS_SDK),pdk)
IDIRS       += $(PDK_PATH)/packages/ti/drv/udma
endif


ifeq ($(SOC),am62a)
ifeq ($(TARGET_CPU),R5F)
SKIPBUILD=1
endif
endif

include $(FINALE)

endif
endif
