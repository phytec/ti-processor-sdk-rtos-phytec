
include tools_path.mak
include build_flags.mak

# Project specific build defs (don't change across different combos):
BUILD_DEFS :=

BUILD_DEFS += $(SOC_DEF)
BUILD_DEFS += $(VPAC_VERSION)

ifeq ($(SOC), am62a)
DIRECTORIES :=
DIRECTORIES += utils/hwa
DIRECTORIES += utils/iss
DIRECTORIES += utils/itt_server
DIRECTORIES += utils/network_api

DIRECTORIES += algos
DIRECTORIES += itt_server_remote
DIRECTORIES += kernels
DIRECTORIES += sensor_drv
DIRECTORIES += ti_2a_wrapper

else
DIRECTORIES := .
endif

TARGET_COMBOS :=

ifeq ($(BUILD_TARGET_MODE),yes)
  ifeq ($(PROFILE), $(filter $(PROFILE), debug all))
    TARGET_COMBOS += $(TARGET_SOC):$(RTOS):R5F:1:debug:TIARMCGT_LLVM
    ifeq ($(TARGET_SOC),J721E)
        TARGET_COMBOS += $(TARGET_SOC):$(RTOS):C66:1:debug:CGT6X
    else
        TARGET_COMBOS += $(TARGET_SOC):$(RTOS):$(C7X_TARGET):1:debug:CGT7X
    endif
    ifeq ($(BUILD_LINUX_MPU),yes)
      TARGET_COMBOS += $(TARGET_SOC):LINUX:$(MPU_CPU):1:debug:GCC_LINUX_ARM
    endif
    ifeq ($(BUILD_QNX_MPU),yes)
      TARGET_COMBOS += $(TARGET_SOC):QNX:$(MPU_CPU):1:debug:GCC_QNX_ARM
    endif
  endif

  ifeq ($(PROFILE), $(filter $(PROFILE), release all))
    TARGET_COMBOS += $(TARGET_SOC):$(RTOS):R5F:1:release:TIARMCGT_LLVM
    ifeq ($(TARGET_SOC),J721E)
        TARGET_COMBOS += $(TARGET_SOC):$(RTOS):C66:1:release:CGT6X
    else
        TARGET_COMBOS += $(TARGET_SOC):$(RTOS):$(C7X_TARGET):1:release:CGT7X
    endif
    ifeq ($(BUILD_LINUX_MPU),yes)
      TARGET_COMBOS += $(TARGET_SOC):LINUX:$(MPU_CPU):1:release:GCC_LINUX_ARM
    endif
    ifeq ($(BUILD_QNX_MPU),yes)
      TARGET_COMBOS += $(TARGET_SOC):QNX:$(MPU_CPU):1:release:GCC_QNX_ARM
    endif
  endif

endif

ifeq ($(BUILD_EMULATION_MODE),yes)
  ifeq ($(OS),Windows_NT)
    ifeq ($(PROFILE), $(filter $(PROFILE), debug all))
        ifeq ($(BUILD_EMULATION_ARCH), $(filter $(BUILD_EMULATION_ARCH), x86_64 all))
            TARGET_COMBOS += PC:WINDOWS:x86_64:1:debug:GCC_WINDOWS
        endif
        ifeq ($(BUILD_EMULATION_ARCH), $(filter $(BUILD_EMULATION_ARCH), X86 all))
            TARGET_COMBOS += PC:WINDOWS:X86:1:debug:GCC_WINDOWS
        endif
    endif

    ifeq ($(PROFILE), $(filter $(PROFILE), release all))
        ifeq ($(BUILD_EMULATION_ARCH), $(filter $(BUILD_EMULATION_ARCH), x86_64 all))
            TARGET_COMBOS += PC:WINDOWS:x86_64:1:release:GCC_WINDOWS
        endif
        ifeq ($(BUILD_EMULATION_ARCH), $(filter $(BUILD_EMULATION_ARCH), X86 all))
            TARGET_COMBOS += PC:WINDOWS:X86:1:release:GCC_WINDOWS
        endif
    endif
  else
    ifeq ($(PROFILE), $(filter $(PROFILE), debug all))
        ifeq ($(BUILD_EMULATION_ARCH), $(filter $(BUILD_EMULATION_ARCH), x86_64 all))
            TARGET_COMBOS += PC:LINUX:x86_64:1:debug:GCC_LINUX
        endif
        ifeq ($(BUILD_EMULATION_ARCH), $(filter $(BUILD_EMULATION_ARCH), X86 all))
            TARGET_COMBOS += PC:LINUX:X86:1:debug:GCC_LINUX
        endif
    endif

    ifeq ($(PROFILE), $(filter $(PROFILE), release all))
        ifeq ($(BUILD_EMULATION_ARCH), $(filter $(BUILD_EMULATION_ARCH), x86_64 all))
            TARGET_COMBOS += PC:LINUX:x86_64:1:release:GCC_LINUX
        endif
        ifeq ($(BUILD_EMULATION_ARCH), $(filter $(BUILD_EMULATION_ARCH), X86 all))
            TARGET_COMBOS += PC:LINUX:X86:1:release:GCC_LINUX
        endif
    endif
  endif
endif

CONCERTO_ROOT ?= concerto
BUILD_MULTI_PROJECT := 1
BUILD_TARGET := target.mak
BUILD_PLATFORM :=

include $(CONCERTO_ROOT)/rules.mak
