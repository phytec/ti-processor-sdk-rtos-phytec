ifeq ($(TARGET_CPU),$(filter $(TARGET_CPU), A72 A53))
ifeq ($(TARGET_OS), $(filter $(TARGET_OS), LINUX QNX))

include $(PRELUDE)

TARGET      := tivision_apps
TARGETTYPE  := dsmo
VERSION     := $(PSDK_VERSION)

include $(VISION_APPS_PATH)/apps/concerto_mpu_inc.mak

STATIC_LIBS     += $(IMAGING_LIBS)
STATIC_LIBS     += $(PTK_LIBS)
STATIC_LIBS     += $(VISION_APPS_KERNELS_LIBS)
STATIC_LIBS     += $(VISION_APPS_MODULES_LIBS)
STATIC_LIBS     += $(TEST_LIBS)

ifeq ($(SOC), $(filter $(SOC), j721e j721s2 j784s4 j722s))
STATIC_LIBS     += $(VISION_APPS_OPENGL_UTILS_LIBS)
STATIC_LIBS     += $(VISION_APPS_SAMPLE_LIBS)
STATIC_LIBS     += $(VISION_APPS_STEREO_LIBS)
ifeq ($(SOC), $(filter $(SOC), j721e j721s2 j784s4))
STATIC_LIBS     += $(VISION_APPS_SRV_LIBS)
endif

SHARED_LIBS += GLESv2 EGL

ifeq ($(TARGET_OS),LINUX)
SHARED_LIBS += rt gbm
endif
endif

ifeq ($(TARGET_OS),QNX)
SYS_SHARED_LIBS += screen
endif

include $(FINALE)

endif
endif
