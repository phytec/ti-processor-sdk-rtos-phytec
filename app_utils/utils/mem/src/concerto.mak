ifneq ($(TARGET_PLATFORM),PC)

include $(PRELUDE)
TARGET      := app_utils_mem
TARGETTYPE  := library

ifeq ($(TARGET_OS),$(filter $(TARGET_OS),FREERTOS SAFERTOS))
CSOURCES    := app_mem_free_rtos.c
IDIRS       += $(VISION_APPS_PATH)/platform/$(SOC)/rtos
endif

ifeq ($(TARGET_OS),LINUX)
CSOURCES     := app_mem_linux_dma_heap.c
endif

ifeq ($(TARGET_OS),QNX)
IDIRS += $(PSDK_QNX_PATH)/qnx/sharedmemallocator/usr/public
IDIRS += $(PSDK_QNX_PATH)/qnx/sharedmemallocator/resmgr/public
CSOURCES    := app_mem_qnx.c
endif

include $(FINALE)

endif

ifeq ($(TARGET_CPU),x86_64)

include $(PRELUDE)
TARGET      := app_utils_mem
TARGETTYPE  := library

CSOURCES    := app_mem_pc.c

include $(FINALE)

endif
