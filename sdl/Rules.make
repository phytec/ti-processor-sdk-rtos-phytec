ifeq ($(Rules_make_include), )

#Default build environment, windows or linux
ifeq ($(OS), )
  OS := Linux
endif

#Default build OS type. Possible options (baremetal, freertos, safertos)
ifeq ($(BUILD_OS_TYPE), )
  BUILD_OS_TYPE ?= baremetal
endif

ifeq ($(BUILD_OS_TYPE),freertos)
  ifeq ($(CONFIG_BLD_XDC_r5f),)
      CONFIG_BLD_LNK_r5f   = $(sdl_PATH)/build/$(SOC)/linker_r5_freertos.lds
  endif
endif
ifeq ($(BUILD_OS_TYPE),safertos)
  ifeq ($(CONFIG_BLD_XDC_r5f),)
      CONFIG_BLD_LNK_r5f   = $(sdl_PATH)/build/$(SOC)/linker_r5_safertos.lds
  endif
endif
ifeq ($(BUILD_OS_TYPE),baremetal)
  ifeq ($(CONFIG_BLD_XDC_r5f),)
      CONFIG_BLD_LNK_r5f   = $(sdl_PATH)/build/$(SOC)/linker_r5.lds
  endif
endif

#PDK is needed for building J721E and J7200 test applications
export PDK_INSTALL_PATH ?= $(abspath ../)/pdk_j721s2_09_02_00_30/packages
export pdk_PATH  := $(PDK_INSTALL_PATH)

#Safe RTOS install path need for building examples that support SafeRTOS
#Note: If path does not exist, SafeRTOS example builds are skipped
export SAFERTOS_VERSION ?= R5_r20211027
export SAFERTOS_KERNEL_INSTALL_PATH   ?= $(abspath ../)/SafeRTOS_Jacinto_$(SAFERTOS_VERSION)

sdl_RELPATH = sdl

sdl_PATH ?= $(abspath ..)/$(sdl_RELPATH)

ifeq ($(OS),Windows_NT)
#Paths for windows machine
  TOOLCHAIN_PATH_R5 ?= $(abspath ../)/ti-cgt-armllvm_3.2.1.LTS
endif

ifeq ($(OS),Linux)
#Paths for linux machine
  TOOLCHAIN_PATH_R5 ?= $(abspath ../)/ti-cgt-armllvm_3.2.1.LTS
endif

# Utilities directory. This is required only if the build machine is Windows.
#   - specify the installation directory of utility which supports POSIX commands
#     (eg: Cygwin installation or MSYS installation).
ifeq ($(OS),Windows_NT)
ifeq ($(UTILS_INSTALL_DIR),)
  UTILS_INSTALL_DIR := C:/ti/ccs1031/ccs/utils/cygwin
endif
  GMAKE_DIR := C:/ti/ccs1031/ccs/utils/cygwin
endif

ROOTDIR ?= $(sdl_PATH)

# Default platform
# Supported values: j721e

ifeq ($(SOC),)
  SOC := j721e
endif

# Default core
# Supported values: r5f
ifeq ($(CORE),)
  CORE := r5f
endif

# Default Profile
# Supported Values: debug | release
ifeq ($(PROFILE), )
  PROFILE := debug
endif

# Default compile mode for R5 is thumb
# Supported Values: thumb | arm
ifeq ($(COMPILE_MODE), )
  export COMPILE_MODE := thumb
endif

# Set Core Profile depending on PROFILE value
ifeq ($(PROFILE_$(CORE)), )
  PROFILE_$(CORE) := $(PROFILE)
endif

ifeq ($(TREAT_WARNINGS_AS_ERROR), )
  TREAT_WARNINGS_AS_ERROR := yes
endif

export OS
export BUILD_OS_TYPE
export SOC
export CORE
export PROFILE_$(CORE)
export TOOLCHAIN_PATH_R5
export sdl_RELPATH
export sdl_PATH
export ROOTDIR
export UTILS_INSTALL_DIR
export GMAKE_DIR
export TREAT_WARNINGS_AS_ERROR

Rules_make_include := 1
endif

ifeq ($(MAKERULEDIR), )
  MAKERULEDIR := $(ROOTDIR)/build/makerules
  export MAKERULEDIR
endif

include $(MAKERULEDIR)/build_config.mk
include $(MAKERULEDIR)/platform.mk
include $(MAKERULEDIR)/env.mk
include $(MAKERULEDIR)/component.mk