ifeq ($(TARGET_CPU), $(filter $(TARGET_CPU), X86 x86_64 A72 A53 R5F))

include $(PRELUDE)
TARGET      := ti_2a_wrapper

TARGETTYPE  := library

CSOURCES    := $(call all-c-files)

IDIRS       += $(HOST_ROOT)/algos/awb/include
IDIRS       += $(HOST_ROOT)/algos/ae/include
IDIRS       += $(HOST_ROOT)/kernels/include
IDIRS       += $(TIOVX_PATH)/include
IDIRS       += $(HOST_ROOT)/ti_2a_wrapper/include

IDIRS       += $(HOST_ROOT)/kernels/aewb/arm
IDIRS       += $(HOST_ROOT)/algos/dcc/include
IDIRS       += $(HOST_ROOT)/sensor_drv/include
IDIRS       += $(HOST_ROOT)/itt_server_remote/include
IDIRS       += $(APP_UTILS_PATH)/utils/remote_service/include
IDIRS       += $(APP_UTILS_PATH)/utils/ipc/include

include $(FINALE)

endif
