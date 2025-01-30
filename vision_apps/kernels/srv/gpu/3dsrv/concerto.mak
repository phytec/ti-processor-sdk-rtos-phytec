ifeq ($(TARGET_OS),$(filter $(TARGET_OS), LINUX QNX))
ifeq ($(TARGET_CPU),$(filter $(TARGET_CPU), A72 A53))

include $(PRELUDE)
TARGET      := vx_srv_render_utils
TARGETTYPE  := library
ifeq ($(TARGET_OS),LINUX)
IDIRS       += $(LINUX_FS_PATH)/usr/include
IDIRS       += $(LINUX_FS_PATH)/usr/include/drm
CFLAGS      += -DEGL_NO_X11
endif
ifeq ($(TARGET_OS),QNX)
IDIRS       += $(QNX_TARGET)/usr/include
endif
IDIRS       += $(VISION_APPS_PATH)/kernels/srv/gpu/3dsrv/Tools
IDIRS       += $(VISION_APPS_PATH)/kernels/srv/gpu/3dsrv/Tools/OGLES2
IDIRS       += $(VISION_APPS_PATH)
IDIRS       += $(GLM_PATH)
CFLAGS      += -DGL_ES
#CFLAGS      += -DGL_ES -DSTANDALONE
CFLAGS      += -Wno-pragmas
CFLAGS      += -Wno-int-to-pointer-cast

CSOURCES    := $(call all-c-files)
CPPSOURCES  := $(call all-cpp-files)

ifeq ($(SOC),$(filter $(SOC), j722s am62a))
SKIPBUILD=1
endif

include $(FINALE)

endif
endif

ifeq ($(TARGET_CPU),x86_64)

include $(PRELUDE)
TARGET      := vx_srv_render_utils
TARGETTYPE  := library
IDIRS       += $(VISION_APPS_PATH)
IDIRS       += $(VISION_APPS_PATH)/kernels/srv/gpu/3dsrv/Tools
IDIRS       += $(VISION_APPS_PATH)/kernels/srv/gpu/3dsrv/Tools/OGLES2
IDIRS       += $(GLM_PATH)
CFLAGS      += -DGL_ES -DSTANDALONE
CFLAGS      += -Wno-pragmas
CFLAGS      += -Wno-int-to-pointer-cast
CFLAGS      += -Wno-strict-aliasing

CSOURCES    := $(call all-c-files)
CPPSOURCES  := $(call all-cpp-files)

ifeq ($(SOC),$(filter $(SOC), j722s am62a))
SKIPBUILD=1
endif

include $(FINALE)

endif

