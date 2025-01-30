PSDK_PATH ?= $(abspath ..)

# Compilers
PSDK_TOOLS_PATH ?= $(HOME)/ti
ifeq ($(SOC),am62a)
  TIARMCGT_LLVM_ROOT ?= $(PSDK_TOOLS_PATH)/ti-cgt-armllvm_3.2.2.LTS
else
  TIARMCGT_LLVM_ROOT ?= $(PSDK_TOOLS_PATH)/ti-cgt-armllvm_3.2.1.LTS
endif
CGT7X_ROOT ?= $(PSDK_TOOLS_PATH)/ti-cgt-c7000_4.1.0.LTS
CGT6X_ROOT ?= $(PSDK_TOOLS_PATH)/ti-cgt-c6000_8.3.7
GCC_LINUX_ARM_ROOT ?= $(PSDK_TOOLS_PATH)/arm-gnu-toolchain-11.3.rel1-x86_64-aarch64-none-linux-gnu

# SDK RTOS Components
LINUX_SYSROOT_ARM ?= $(PSDK_PATH)/targetfs
PSDK_BUILDER_PATH ?= $(PSDK_PATH)/sdk_builder
CONCERTO_ROOT ?= $(PSDK_BUILDER_PATH)/concerto
PDK_PATH ?= $(PSDK_PATH)/pdk_j721s2_09_02_00_30
MCU_PLUS_SDK_PATH ?= $(PSDK_PATH)/mcu_plus_sdk
PDK_QNX_PATH ?= $(PSDK_PATH)/psdkqa/pdk
VXLIB_PATH ?= $(PSDK_PATH)/vxlib
MATHLIB_PATH ?= $(PSDK_PATH)/mathlib_c66x_3_1_2_1
MMALIB_PATH ?= $(PSDK_PATH)/mmalib_09_02_00_08
IVISION_PATH ?= $(PSDK_PATH)/ivision
TIDL_PATH ?= $(PSDK_PATH)/c7x-mma-tidl
APP_UTILS_PATH ?= $(PSDK_PATH)/app_utils
TIOVX_PATH ?= $(PSDK_PATH)/tiovx
ifeq ($(SOC), j721e)
    VPAC_C_MODELS_PATH ?= $(PSDK_PATH)/vhwa_c_models/vpac1
else ifeq ($(SOC), $(filter $(SOC), j721s2 j784s4))
    VPAC_C_MODELS_PATH ?= $(PSDK_PATH)/vhwa_c_models/vpac3
else ifeq ($(SOC), $(filter $(SOC), am62a j722s))
    VPAC_C_MODELS_PATH ?= $(PSDK_PATH)/vhwa_c_models/vpac3l
endif
ifeq ($(SOC), $(filter $(SOC), j721e j721s2 j784s4 j722s))
    DMPAC_C_MODELS_PATH ?= $(PSDK_PATH)/vhwa_c_models/dmpac
endif
IMAGING_PATH ?= $(PSDK_PATH)/imaging
VIDEO_IO_PATH ?= $(PSDK_PATH)/video_io
VISION_APPS_PATH ?= $(PSDK_PATH)/vision_apps
MCUSW_PATH ?= $(PSDK_PATH)/mcusw
PTK_PATH ?= $(PSDK_PATH)/ti-perception-toolkit
TIADALG_PATH ?= $(PSDK_PATH)/tiadalg
GLM_PATH ?= $(PSDK_PATH)/glm
ETHFW_PATH ?= $(PSDK_PATH)/ethfw
TI_SECURE_DEV_PKG ?= $(PSDK_PATH)/core-secdev-k3

# This is required to be set when pulling in the safertos_version
BOARD=$(SOC)_evm

ifeq ($(SOC), $(filter $(SOC), j721e j721s2 j784s4))
ifneq ($(wildcard $(PDK_PATH)),)
  include $(PDK_PATH)/packages/ti/build/safertos_version.mk

  ifeq ($(SAFERTOS_$(SOC)_r5f_INSTALL_PATH),)
    export SAFERTOS_KERNEL_INSTALL_PATH_r5f = $(PSDK_PATH)/safertos_$(SOC)_r5f_$(SAFERTOS_VERSION_r5f)
  else
    export SAFERTOS_KERNEL_INSTALL_PATH_r5f = $(SAFERTOS_$(SOC)_r5f_INSTALL_PATH)
  endif
  ifeq ($(SAFERTOS_$(SOC)_c7x_INSTALL_PATH),)
    export SAFERTOS_KERNEL_INSTALL_PATH_c7x = $(PSDK_PATH)/safertos_$(SOC)_c7x_$(SAFERTOS_VERSION_c7x)
  else
    export SAFERTOS_KERNEL_INSTALL_PATH_c7x = $(SAFERTOS_$(SOC)_c7x_INSTALL_PATH)
  endif
  ifeq ($(SOC),j721e)
    ifeq ($(SAFERTOS_$(SOC)_c66_INSTALL_PATH),)
      export SAFERTOS_KERNEL_INSTALL_PATH_c66 = $(PSDK_PATH)/safertos_$(SOC)_c66_$(SAFERTOS_VERSION_c66)
    else
      export SAFERTOS_KERNEL_INSTALL_PATH_c66 = $(SAFERTOS_$(SOC)_c66_INSTALL_PATH)
    endif
  endif
endif
endif

BUILD_OS ?= Linux

ifeq ($(BUILD_OS),Linux)
GCC_LINUX_ROOT ?= /usr/
endif

LINUX_FS_PATH ?= $(PSDK_PATH)/targetfs/
LINUX_FS_BOOT_PATH ?= $(PSDK_PATH)/bootfs/
LINUX_SD_FS_ROOT_PATH ?= /media/$(USER)/rootfs
LINUX_SD_FS_BOOT_PATH ?= /media/$(USER)/BOOT

ifeq ($(SOC),j721e)
  export PSDK_LINUX_PATH ?= $(HOME)/ti-processor-sdk-linux-adas-j721e-evm-09_02_00_05
else ifeq ($(SOC),j721s2)
  export PSDK_LINUX_PATH ?= $(HOME)/ti-processor-sdk-linux-adas-j721s2-evm-09_02_00_05
else ifeq ($(SOC),j784s4)
  export PSDK_LINUX_PATH ?= $(HOME)/ti-processor-sdk-linux-adas-j784s4-evm-09_02_00_05
else ifeq ($(SOC),j722s)
  export PSDK_LINUX_PATH ?= $(HOME)/ti-processor-sdk-linux-adas-j722s-evm-09_02_00_05
else ifeq ($(SOC),am62a)
  export PSDK_LINUX_PATH ?= $(HOME)/ti-processor-sdk-linux-edgeai-am62axx-evm-09_02_00_05
endif

# QNX Paths
export PSDK_QNX_PATH ?= $(PSDK_PATH)/psdkqa

export QNX_SDP_VERSION ?= 710
ifeq ($(QNX_SDP_VERSION),800)
  export QNX_BASE ?= $(HOME)/qnx800
  export QNX_CROSS_COMPILER_TOOL ?= aarch64-unknown-nto-qnx8.0.0-
  export QNX_TARGET ?= $(QNX_BASE)/target/qnx
else
  export QNX_BASE ?= $(HOME)/qnx710
  export QNX_CROSS_COMPILER_TOOL ?= aarch64-unknown-nto-qnx7.1.0-
  # Adding this path for QNX SDP 7.1 which has a need to set the path
  # variable for the g++ tool to properly invloke the ld tool
  PATH := $(QNX_BASE)/host/linux/x86_64/usr/bin:$(PATH)
  export QNX_TARGET ?= $(QNX_BASE)/target/qnx7
endif
export QNX_HOST ?= $(QNX_BASE)/host/linux/x86_64
export GCC_QNX_ROOT ?= $(QNX_HOST)/usr/bin
export GCC_QNX_ARM_ROOT ?= $(QNX_HOST)/usr/bin
export GCC_QNX_ARM ?= $(QNX_HOST)/usr/bin
QNX_BOOT_PATH ?= $(PSDK_QNX_PATH)/bootfs/
QNX_FS_PATH ?= $(PSDK_QNX_PATH)/qnxfs/
QNX_SD_FS_ROOT_PATH ?= /media/$(USER)/rootfs
QNX_SD_FS_QNX_PATH ?= /media/$(USER)/qnxfs
QNX_SD_FS_BOOT_PATH ?= /media/$(USER)/boot
