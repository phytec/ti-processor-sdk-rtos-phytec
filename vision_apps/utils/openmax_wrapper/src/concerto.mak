ifneq ($(TARGET_PLATFORM),PC)

ifeq ($(TARGET_OS),$(filter $(TARGET_OS), QNX))
ifeq ($(TARGET_PLATFORM), $(filter $(TARGET_PLATFORM), J721E J784S4 J721S2))
include $(PRELUDE)

TARGET      := app_utils_omax_wrapper
TARGETTYPE  := library

CSOURCES    := $(call all-c-files)

ifeq ($(TARGET_PLATFORM), $(filter $(TARGET_PLATFORM), J784S4 J721S2))
IDIRS += $(PSDK_QNX_PATH)/qnx/codec/vpu/OpenMAXIL/khronos/openmaxil/
IDIRS += $(PSDK_QNX_PATH)/qnx/codec/vpu/OpenMAXIL/core/public/khronos/openmaxil
IDIRS += $(PSDK_QNX_PATH)/qnx/sharedmemallocator/usr/public
IDIRS += $(PSDK_QNX_PATH)/qnx/codec/vpu/tivpucodec/vpulib/public
IDIRS += $(PSDK_QNX_PATH)/qnx/codec/vpu/OpenMAXIL/utility
IDIRS += $(PSDK_QNX_PATH)/qnx/codec/vpu/OpenMAXIL/core
IDIRS += $(PSDK_QNX_PATH)/qnx/codec/vpu/tivpucodec/common
IDIRS += $(PSDK_QNX_PATH)/qnx/codec/vpu/tivpucodec/encoder
IDIRS += $(PSDK_QNX_PATH)/qnx/codec/vpu/tivpucodec/helper
IDIRS += $(PSDK_QNX_PATH)/qnx/codec/vpu/tivpucodec/helper/misc
IDIRS += $(PSDK_QNX_PATH)/qnx/codec/vpu/tivpucodec/helper/yuv
IDIRS += $(PSDK_QNX_PATH)/qnx/codec/vpu/resmgrlib

CFLAGS      += -DCODEC_USE_HIGHMEM
endif
ifeq ($(TARGET_PLATFORM), $(filter $(TARGET_PLATFORM), J721E))
IDIRS += $(PSDK_QNX_PATH)/qnx/codec/img/qnx/OpenMAXIL/khronos/openmaxil/
IDIRS += $(PSDK_QNX_PATH)/qnx/codec/img/qnx/OpenMAXIL/core/public/khronos/openmaxil
endif

ifeq ($(SOC),j722s)
SKIPBUILD=1
endif

include $(FINALE)

endif
endif

endif
