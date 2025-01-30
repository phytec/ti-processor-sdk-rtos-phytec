TARGET      := vx_tidl_rt
TARGETTYPE  := dsmo
ifeq ($(BUILD_WITH_OPENACC), 1)
CFLAGS += -fPIC #-Wno-int-to-pointer-cast -Wno-stringop-truncation -Wno-format-overflow
else
CFLAGS += -fPIC -Wno-int-to-pointer-cast -Wno-stringop-truncation -Wno-format-overflow
endif
CPPFLAGS += -fPIC --std=c++11

CSOURCES    += ../tidl_rt_ovx.c
CSOURCES    += ../tidl_rt_utils.c
CSOURCES    += ../tidl_rt_ovx_debug_utils.c
CPPSOURCES  += ../tidl_rt_ovx_datamove.cpp

ifneq ($(TARGET_PLATFORM), PC)
ifeq ($(TARGET_SOC), $(filter $(TARGET_SOC), J721E j721e J721S2 j721s2 J784S4 j784s4 AM62A am62a J722S j722s))
CSOURCES    += ../tvm_rt_ovx.c
endif
endif

IDIRS :=
IDIRS += $(IVISION_PATH)
IDIRS += $(TIOVX_PATH)/include
IDIRS += $(TIOVX_PATH)/kernels/include
IDIRS += $(TIDL_PATH)/arm-tidl/tiovx_kernels/include
IDIRS += $(TIOVX_PATH)/utils/include
IDIRS += $($(_MODULE)_SDIR)/../../../inc
IDIRS += $($(_MODULE)_SDIR)/../../../rt/inc
IDIRS += $(VISION_APPS_PATH)
IDIRS += $(APP_UTILS_PATH)

