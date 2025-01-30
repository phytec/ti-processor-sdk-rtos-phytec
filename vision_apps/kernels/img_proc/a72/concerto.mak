
ifeq ($(TARGET_CPU), $(filter $(TARGET_CPU), A72 A53 x86_64))

include $(PRELUDE)
TARGET      := vx_target_kernels_img_proc_a72
TARGETTYPE  := library

CSOURCES    := vx_kernels_img_proc_target.c
CSOURCES    += vx_img_hist_target.c

IDIRS       += $(VISION_APPS_PATH)/kernels/img_proc/include
IDIRS       += $(VISION_APPS_PATH)/kernels/img_proc/host
IDIRS       += $(TIOVX_PATH)/kernels/ivision/include

include $(FINALE)

endif
