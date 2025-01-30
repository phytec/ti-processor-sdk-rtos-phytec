# Copyright (C) 2011 Texas Insruments, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

ifdef QCONF_OVERRIDE
    include $(QCONF_OVERRIDE)
endif
VISION_ROOT ?= $(HOST_ROOT)
#SRC_DIR ?= vx

SYSDEFS  :=
SYSIDIRS :=
SYSLDIRS :=


# wanted this here since it is common amongst all compilers, but it didn't work...
#DEFS += J7ES=1
#DEFS += J7AEP=2
#DEFS += RESERVED=3
#DEFS += RESERVED=4
#DEFS += RESERVED=5
#DEFS += RESERVED=6
#DEFS += RESERVED=7
#ifeq ($(SOC),J7ES)
#DEFS += SOC=$(SOC)
#else ifeq ($(SOC),J7AEP)
#DEFS += SOC=$(SOC)
#else ifeq ($(SOC),RESERVED)
#DEFS += SOC=$(SOC)
#else ifeq ($(SOC),RESERVED)
#DEFS += SOC=$(SOC)
#else ifeq ($(SOC),RESERVED)
#DEFS += SOC=$(SOC)
#else ifeq ($(SOC),RESERVED)
#DEFS += SOC=$(SOC)
#else ifeq ($(SOC),RESERVED)
#DEFS += SOC=$(SOC)
#else
#DEFS += SOC=J7ES
#endif


ifeq ($(TARGET_PLATFORM),PC)
    TARGET_OS=$(HOST_OS)
    TARGET_CPU?=X86
    ifneq ($(TARGET_SCPU),)
        SYSDEFS+=_HOST_BUILD
        ifeq ($(TARGET_SCPU),$(filter $(TARGET_SCPU),C7100))
            ifndef CGT7X_ROOT
            $(error You must define CGT7X_ROOT!)
            endif
            SYSDEFS+=__C7100__
            SYSIDIRS += $(CGT7X_ROOT)/host_emulation/include
            SYSIDIRS += $(CGT7X_ROOT)/host_emulation/include/C7100
            SYSLDIRS += $(CGT7X_ROOT)/host_emulation
            SYS_STATIC_LIBS += C7100-host-emulation
        else ifeq ($(TARGET_SCPU),C7120)
            ifndef CGT7X_ROOT
            $(error You must define CGT7X_ROOT!)
            endif
            SYSDEFS+=__C7120__
            SYSIDIRS += $(CGT7X_ROOT)/host_emulation/include
            SYSIDIRS += $(CGT7X_ROOT)/host_emulation/include/C7120
            SYSLDIRS += $(CGT7X_ROOT)/host_emulation
            SYS_STATIC_LIBS += C7120-host-emulation
        else ifeq ($(TARGET_SCPU),RESERVED)
            ifndef CGT7X_ROOT
            $(error You must define CGT7X_ROOT!)
            endif
            SYSDEFS+=__RESERVED__
            SYSIDIRS += $(CGT7X_ROOT)/host_emulation/include
            SYSIDIRS += $(CGT7X_ROOT)/host_emulation/include/RESERVED
            SYSLDIRS += $(CGT7X_ROOT)/host_emulation
            SYS_STATIC_LIBS += RESERVED-host-emulation
        else ifeq ($(TARGET_SCPU),RESERVED)
            ifndef CGT7X_ROOT
            $(error You must define CGT7X_ROOT!)
            endif
            SYSDEFS+=__RESERVED__
            SYSIDIRS += $(CGT7X_ROOT)/host_emulation/include
            SYSIDIRS += $(CGT7X_ROOT)/host_emulation/include/RESERVED
            SYSLDIRS += $(CGT7X_ROOT)/host_emulation
            SYS_STATIC_LIBS += RESERVED-host-emulation
        else ifeq ($(TARGET_SCPU),RESERVED)
            ifndef CGT7X_ROOT
            $(error You must define CGT7X_ROOT!)
            endif
            SYSDEFS+=__RESERVED__
            SYSIDIRS += $(CGT7X_ROOT)/host_emulation/include
            SYSIDIRS += $(CGT7X_ROOT)/host_emulation/include/RESERVED-MMA2_256
            SYSLDIRS += $(CGT7X_ROOT)/host_emulation
            SYS_STATIC_LIBS += RESERVED-MMA2_256-host-emulation
        else ifeq ($(TARGET_SCPU),RESERVED)
            ifndef CGT7X_ROOT
            $(error You must define CGT7X_ROOT!)
            endif
            SYSDEFS+=__RESERVED__
            SYSIDIRS += $(CGT7X_ROOT)/host_emulation/include
            SYSIDIRS += $(CGT7X_ROOT)/host_emulation/include/RESERVED
            SYSLDIRS += $(CGT7X_ROOT)/host_emulation
            SYS_STATIC_LIBS += RESERVED-host-emulation
        else
            $(error TARGET_SCPU=$(TARGET_SCPU) is not supported!)
        endif
    else
        $(error TARGET_SCPU should be defined when TARGET_PLATFORM=PC)
    endif
    ifeq ($(TARGET_OS),LINUX)
        INSTALL_LIB := /usr/lib
        INSTALL_BIN := /usr/bin
        INSTALL_INC := /usr/include
        TARGET_NUM_CORES:=$(shell cat /proc/cpuinfo | grep processor | wc -l)
        SYSIDIRS += /usr/include
        SYSLDIRS += /usr/lib
        SYSDEFS+=_XOPEN_SOURCE=700 _DEFAULT_SOURCE=1 _GNU_SOURCE=1
        ifneq ($(TARGET_CPU),$(HOST_CPU))
            ifeq ($(TARGET_CPU),X86)
                SYSLDIRS+=/usr/lib32
            endif
        endif
    else ifeq ($(TARGET_OS),DARWIN)
        INSTALL_LIB := /opt/local/lib
        INSTALL_BIN := /opt/local/bin
        INSTALL_INC := /opt/local/include
        TARGET_NUM_CORES ?= 2
        SYSDEFS+=_XOPEN_SOURCE=700 _DEFAULT_SOURCE=1 _GNU_SOURCE=1
    else ifeq ($(TARGET_OS),CYGWIN)
        INSTALL_LIB := /usr/lib
        INSTALL_BIN := /usr/bin
        INSTALL_INC := /usr/include
        TARGET_NUM_CORES=1
        SYSIDIRS+=/usr/include
        SYSLDIRS+=/usr/libs
        SYSDEFS+=_XOPEN_SOURCE=700 _DEFAULT_SOURCE=1 _GNU_SOURCE=1 WINVER=0x501
    else ifeq ($(TARGET_OS),Windows_NT)
        INSTALL_LIB := "${windir}\\system32"
        INSTALL_BIN := "${windir}\\system32"
        INSTALL_INC :=
        TARGET_NUM_CORES := $(NUMBER_OF_PROCESSORS)
        SYSIDIRS+=$(VISION_ROOT)/include/win32
        SYSDEFS+=WIN32_LEAN_AND_MEAN WIN32 _WIN32 _CRT_SECURE_NO_DEPRECATE WINVER=0x0501 _WIN32_WINNT=0x0501
    endif
# This appears in both the if and else clause; should be moved to appropriate location but I don't know what that is
    EXTENDED_TEST_CASE ?= -1
    TEST_CASE ?= -1
    TEST_CATEGORY ?= -1
else
    TARGET_CPU ?= C7100
    ifneq ($(TARGET_CPU),$(filter $(TARGET_CPU),C7100 C7120 RESERVED RESERVED RESERVED RESERVED))
        $(error TARGET_CPU=$(TARGET_CPU) is not supported!)
    endif
    MMA_IMPLEMENTATION ?= HARDWARE
    SOC ?= J7ES
    TEST_ENV ?= LOKI
    USE_PRINTF ?= YES
    EXTENDED_TEST_CASE ?= -1
    TEST_CASE ?= -1
    TEST_CATEGORY ?= -1
    TARGET_OS := NO_OS
    HOST_COMPILER := CGT7X
    ifeq ($(TARGET_CPU),$(filter $(TARGET_CPU),C64P C674 C66))
        HOST_COMPILER := CGT6X
    endif
    ifneq ($(C6X_MIGRATION),)
        SYSDEFS+=C6X_MIGRATION
    endif
endif

ifeq ($(TARGET_OS),LINUX)
    PLATFORM_LIBS := dl pthread rt
else ifeq ($(TARGET_OS),DARWIN)
    PLATFORM_LIBS :=
else ifeq ($(TARGET_OS),Windows_NT)
    PLATFORM_LIBS := Ws2_32 user32
else ifeq ($(TARGET_OS),__QNX__)
    PLATFORM_LIBS := screen socket
else ifeq ($(TARGET_OS),CYGWIN)
    PLATFORM_LIBS := c pthread
endif

ifeq ($(TARGET_CPU),X86)
    TARGET_ARCH=32
else ifeq ($(TARGET_CPU),X64)
    TARGET_ARCH=64
else ifeq ($(TARGET_CPU),x86_64)
    TARGET_ARCH=64
else ifeq ($(TARGET_CPU),ARM)
    TARGET_ARCH=32
else ifeq ($(TARGET_CPU),i386)
    TARGET_ARCH=32
endif

# Override default out directory
TARGET_OUT ?= $(HOST_ROOT)/$(BUILD_OUTPUT)/$(TARGET_CPU)/$(TARGET_BUILD)

$(info TARGET_CPU=$(TARGET_CPU))
$(info TARGET_BUILD=$(TARGET_BUILD))

ifeq ($(HOST_OS),Windows_NT)
    MT=gmake
    NEWLINE=@echo. 2>&1
    q=
else
    MT=make
    NEWLINE=@echo
    q="
endif

help:
	$(NEWLINE)
	@echo $(q)Please set/export the following variables, or include them on the command line:$(q)
	@echo $(q)   TARGET_CPU         - Target CPU to build.$(q)
	@echo $(q)                           Available options: C7100, C7120, RESERVED, RESERVED, x86_64 (Linux). [Default=C7100]$(q)
	@echo $(q)   TARGET_SCPU        - When TARGET_CPU==x86_64, this is the simulated target CPU to build.$(q)
	@echo $(q)                           Available options: C7100, C7120, RESERVED, RESERVED. [Default=C7100]$(q)
	@echo $(q)   MMA_IMPLEMENTATION - Target MMA to build.$(q)
	@echo $(q)                           Available options: HARDWARE, J7_QT. [Default=HARDWARE]$(q)
	@echo $(q)   SOC                - Target SoC for build; required for linker command file when creating stand-alone tests.$(q)
	@echo $(q)                           Available options: J7ES, J7AEP, RESERVED, RESERVED. [Default=J7ES]$(q)
	@echo $(q)   TEST_ENV           - Target test environment to build the MMA kernel for.$(q)
	@echo $(q)                           Available options: LOKI, QT, RTL, VMIN, EVM [Default=LOKI]$(q)
	@echo $(q)   NO_PRINTF          - Disable all printf statements in MMALIB builds. Expected to be used when TEST_ENV is QT or RTL.$(q)
	@echo $(q)                           Setting to '1' enables this flag.  Default is disabled.$(q)
	@echo $(q)   PERFORMANCE_TEST   - Disable test driver calls to natural-c computation. This disables comparison between natural-c and optimized-c implementations.$(q)
	@echo $(q)                           Setting to '1' enables this flag.  Default is disabled.$(q)
	@echo $(q)   TEST_CASE          - Number (id) of the test case that will be included in the MMA kernel .out file.$(q)
	@echo $(q)                           For test case numbers, refer to the the *_idat.c file for the kernel.$(q)
	@echo $(q)                           Available options: 0 to maximum test case number (id) [Default=all test cases will be included]$(q)
	@echo $(q)   TEST_CATEGORY      - Category of the test cases that will be included in the MMA kernel .out file. $(q)
	@echo $(q)                           For test cateogories, refer to the the *_idat.c file for the kernel.$(q)
	@echo $(q)                           Available options: 0 to maximum test category [Default=all test cases will be included]$(q)
	@echo $(q)   TARGET_PLATFORM    - Normally used to specify several settings in a platform.  For this project,$(q)
	@echo $(q)                           this is not defined by default, and should only be used for enabling PC simulator.$(q)
	@echo $(q)                           When TARGET_PLATFORM=PC is defined, the make system expects TARGET_SCPU to be defined.$(q)
	@echo $(q)                           Available options: PC. [Default=not defined]$(q)
	@echo $(q)   CGT7X_ROOT         - Directory where the C7X compiler is located.$(q)
	@echo $(q)                           This is only required if C71xx target is being compiled.$(q)
	@echo $(q)                           If not set when this target are selected, a build error will indicate that$(q)
	@echo $(q)                           this variable should be set.$(q)
	@echo $(q)   TARGET_BUILD       - Target configuration to build.$(q)
	@echo $(q)                           Available options: debug, release. [Default=release]$(q)
	@echo $(q)                           Choosing debug also sets CHECKPARAMS=1.$(q)
	@echo $(q)   BUILD_DEBUG        - Flag to enable more verbose print outs during build.  Useful to see the$(q)
	@echo $(q)                           the full commands used for compiling/linking each file.$(q)
	@echo $(q)                           Setting to '1' enables this flag.  Default is disabled.$(q)
	@echo $(q)   KEEP_ASM           - Flag to tell the compiler to keep the generated asm files for$(q)
	@echo $(q)                           debug and to facilitate optimization activities.$(q)
	@echo $(q)                           Setting to '1' enables this flag.  Default is disabled.$(q)
	@echo $(q)   FLAT_MEM           - Flag to link using a large, flat memory structure, assumed to all be at the L2 level.$(q)
	@echo $(q)                           Intended to be used in C7x simulation and test environments without higher level memory.$(q)
	@echo $(q)                           Setting to '1' enables this flag.  Default is disabled(FLAT_MEM=not defined).$(q)
	@echo $(q)   FIXED_SEED         - Seed value to initialize the random number generator used for test generation in MMALIB.$(q)
	@echo $(q)                           Setting this value uses the number specified to initialize the random number generator$(q)
	@echo $(q)                           and thus create repeatable test data.  If not defined, a time-based seed is used.$(q)
	@echo $(q)   AUTOTEST           - Flag to link each unit test against the mmalib and mmalib_cn libraries instead of $(q)
	@echo $(q)                           building only the kernel code.  This is primarily used for automated testing.$(q)
	@echo $(q)                           Setting to '1' enables this flag.  Default is disabled.$(q)
	@echo $(q)   CHECKPARAMS        - Flag to enable parameter checking within the kernel function $(q)
	@echo $(q)                           This is primarily used during library integration.$(q)
	@echo $(q)                           Setting to '1' enables this flag.  Default is disabled.$(q)
	$(NEWLINE)
	@echo $(q)Available make rules$(q)
	@echo $(q)   $(MT) -j(threads)   - Build all.$(q)
	@echo $(q)                           (threads) should be at least as many CPU cores you have on$(q)
	@echo $(q)                           your PC to dramatically reduce the build time.$(q)
	@echo $(q)                           NOTE: All generated files will be put in the 'out' folder.$(q)
	@echo $(q)   $(MT) (module)      - Build a module.$(q)
	@echo $(q)                           (module) is the name of the target module that will be build, along$(q)
	@echo $(q)                           with any dependencies$(q)
	@echo $(q)                           NOTE: All generated files will be put in the 'out' folder.$(q)
	@echo $(q)   $(MT) clean         - Clean this config$(q)
	@echo $(q)                           This deletes the specific configuration that is set in the out folder.$(q)
	@echo $(q)   $(MT) scrub         - Clean all configs$(q)
	@echo $(q)                           This deletes the entire out folder.$(q)
	@echo $(q)   $(MT) help          - This message$(q)
	@echo $(q)   $(MT) info          - Variable settings$(q)
	@echo $(q)                           This reports what each environment variable is set to.$(q)
	$(NEWLINE)
	@echo $(q)Example invocation variations (not exclusive):$(q)
	@echo $(q)   $(MT)$(q)
	@echo $(q)   $(MT) -j8$(q)
	@echo $(q)   $(MT) -j8 mmalib$(q)
	@echo $(q)   $(MT) -j8 linalg_c7xmma/MMALIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX$(q)
	@echo $(q)   $(MT) -j12 cnn_c7xmma/MMALIB_CNN_convolve_col_smallNo_ixX_ixX_oxX TEST_CATEGORY=200 TARGET_CPU=x86_64 TARGET_SCPU=C7100 TARGET_PLATFORM=PC TARGET_BUILD=release$(q)
	@echo $(q)   $(MT) -j8 cnn_c7xmma/MMALIB_CNN_convolve_row_ixX_ixX_oxX TEST_ENV=QT PERFORMANCE_TEST=1 NO_PRINTF=1 TEST_CASE=195$(q)
	$(NEWLINE)

info:
	$(NEWLINE)
	@echo TARGET_CPU   := $(TARGET_CPU)
	@echo TARGET_SCPU  := $(TARGET_SCPU)
	@echo SOC          := $(SOC)
	@echo CGT7X_ROOT   := $(CGT7X_ROOT)
	@echo TARGET_BUILD := $(TARGET_BUILD)
	@echo BUILD_DEBUG  := $(BUILD_DEBUG)
	@echo TARGET_OUT   := $(TARGET_OUT)
	$(NEWLINE)
