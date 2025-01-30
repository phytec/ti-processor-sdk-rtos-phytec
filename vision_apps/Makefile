
include tools_path.mak

# Edit below file to change default build options
include build_flags.mak

# Project specific build defs (don't change across different combos):
BUILD_DEFS :=
ifneq ($(APP_CONFIG_BASE_PATH),)
BUILD_DEFS += APP_CONFIG_BASE_PATH
endif

BUILD_DEFS += $(SOC_DEF)

DIRECTORIES :=
ifeq ($(SOC),am62a)
DIRECTORIES += utils/app_init
DIRECTORIES += utils/sciserver

DIRECTORIES += platform/$(SOC)

DIRECTORIES += modules
DIRECTORIES += apps/basic_demos/app_c7x_kernel
DIRECTORIES += apps/basic_demos/app_fd_exchange
DIRECTORIES += apps/tests/app_vx_conformance
DIRECTORIES += apps/tests/app_vx_conformance_core
DIRECTORIES += apps/tests/app_vx_conformance_hwa
DIRECTORIES += apps/tests/app_vx_conformance_tidl
DIRECTORIES += apps/utilities

DIRECTORIES += kernels/fileio
DIRECTORIES += kernels/img_proc

else
DIRECTORIES += utils
DIRECTORIES += kernels
DIRECTORIES += modules
DIRECTORIES += applibs
DIRECTORIES += platform/$(SOC)

ifeq ($(BUILD_EDGEAI),no)
	DIRECTORIES += apps
else
	DIRECTORIES += apps/basic_demos/app_c7x_kernel
	DIRECTORIES += apps/utilities
	DIRECTORIES += apps/tests
endif
endif

TARGET_COMBOS :=

ifeq ($(BUILD_TARGET_MODE),yes)
  ifeq ($(PROFILE), $(filter $(PROFILE), debug all))
	ifeq ($(BUILD_ISA_R5F),yes)
	TARGET_COMBOS += $(TARGET_SOC):$(RTOS):R5F:3:debug:TIARMCGT_LLVM
	endif
	ifeq ($(BUILD_ISA_C6x),yes)
	TARGET_COMBOS += $(TARGET_SOC):$(RTOS):C66:2:debug:CGT6X
	endif
	ifeq ($(BUILD_ISA_C7x),yes)
	TARGET_COMBOS += $(TARGET_SOC):$(RTOS):$(C7X_TARGET):1:debug:CGT7X
	endif
	ifeq ($(BUILD_ISA_MPU),yes)
		ifeq ($(BUILD_LINUX_MPU),yes)
		TARGET_COMBOS += $(TARGET_SOC):LINUX:$(MPU_CPU):1:debug:GCC_LINUX_ARM
		endif
		ifeq ($(BUILD_QNX_MPU),yes)
		TARGET_COMBOS += $(TARGET_SOC):QNX:$(MPU_CPU):1:debug:GCC_QNX_ARM
		endif
	endif
  endif

  ifeq ($(PROFILE), $(filter $(PROFILE), release all))
	ifeq ($(BUILD_ISA_R5F),yes)
	TARGET_COMBOS += $(TARGET_SOC):$(RTOS):R5F:3:release:TIARMCGT_LLVM
	endif
	ifeq ($(BUILD_ISA_C6x),yes)
	TARGET_COMBOS += $(TARGET_SOC):$(RTOS):C66:2:release:CGT6X
	endif
	ifeq ($(BUILD_ISA_C7x),yes)
	TARGET_COMBOS += $(TARGET_SOC):$(RTOS):$(C7X_TARGET):1:release:CGT7X
	endif
	ifeq ($(BUILD_ISA_MPU),yes)
		ifeq ($(BUILD_LINUX_MPU),yes)
		TARGET_COMBOS += $(TARGET_SOC):LINUX:$(MPU_CPU):1:release:GCC_LINUX_ARM
		endif
		ifeq ($(BUILD_QNX_MPU),yes)
		TARGET_COMBOS += $(TARGET_SOC):QNX:$(MPU_CPU):1:release:GCC_QNX_ARM
		endif
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

CONCERTO_ROOT ?= $(PSDK_BUILDER_PATH)/concerto
BUILD_MULTI_PROJECT := 1
BUILD_TARGET := target.mak
BUILD_PLATFORM :=

include $(CONCERTO_ROOT)/rules.mak

# Project specific rules

.PHONY: all
all:

doxy_docs:
	-$(Q)$(MKDIR) docs/user_guide/ $(QUIET) || true
	-$(Q)$(COPY) tools/3d_calibration_tool/PSDK_RTOS_UserGuide_3D_SurroundView_Manual_CalibTool.pdf docs/user_guide/
	-$(Q)$(COPY) tools/3d_calibration_tool/poster_calib_chart.pdf docs/user_guide/
	$(DOXYGEN) internal_docs/doxy_cfg_user_guide/user_guide_linux_$(SOC).cfg 2> internal_docs/doxy_cfg_user_guide/doxy_warnings.txt
	-$(Q)$(COPY) internal_docs/doxy_cfg_user_guide/theme/*.png docs/user_guide/
	$(COPY) vision_apps_release_notes_$(SOC).html vision_apps_release_notes.html

doxy_design_docs:
	$(DOXYGEN) internal_docs/doxy_cfg_design/design_guide.cfg 2> internal_docs/doxy_cfg_design/doxy_warnings.txt

doxy_datasheet_docs:
	-$(Q)$(MKDIR) docs/datasheet/ $(QUIET) || true
	$(DOXYGEN) internal_docs/doxy_cfg_datasheet/datasheet_$(SOC).cfg 2> internal_docs/doxy_cfg_datasheet/doxy_warnings.txt
