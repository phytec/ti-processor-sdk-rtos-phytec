ifneq ($(TARGET_PLATFORM),PC)

include $(PRELUDE)
TARGET      := app_utils_ipc
TARGETTYPE  := library

ifeq ($(TARGET_OS),$(filter $(TARGET_OS),SYSBIOS FREERTOS SAFERTOS))
CSOURCES    := app_ipc_rtos.c app_ipc_rtos_echo_test.c
endif

ifeq ($(RTOS_SDK),mcu_plus_sdk)
IDIRS       += $(VISION_APPS_PATH)/platform/$(SOC)/rtos
endif

ifeq ($(TARGET_OS),LINUX)
CSOURCES    := app_ipc_linux.c
CSOURCES    += app_ipc_linux_hw_spinlock.c

#
# Select either of one below (rpmsg_proto or rpmsg_char)
# to choose the protocol to use for user space IPC.
# Once rpmsg_char is tested, rpmsg_proto will be removed.
#
CSOURCES    += app_ipc_linux_rpmsg_char.c

endif

ifeq ($(TARGET_OS), QNX)
IDIRS += $(PDK_QNX_PATH)/packages/ti/drv/ipc/
IDIRS += $(PDK_QNX_PATH)/packages/
CSOURCES := app_ipc_qnx.c
endif

include $(FINALE)

endif
