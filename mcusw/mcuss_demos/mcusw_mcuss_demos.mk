#
#
# Copyright (c) 2024 Texas Instruments Incorporated
#
# All rights reserved not granted herein.
#
# Limited License.
#
# Texas Instruments Incorporated grants a world-wide, royalty-free, non-exclusive
# license under copyrights and patents it now or hereafter owns or controls to make,
# have made, use, import, offer to sell and sell ("Utilize") this software subject to the
# terms herein.  With respect to the foregoing patent license, such license is granted
# solely to the extent that any such patent is necessary to Utilize the software alone.
# The patent license shall not apply to any combinations which include this software,
# other than combinations with devices manufactured by or for TI ("TI Devices").
# No hardware patent is licensed hereunder.
#
# Redistributions must preserve existing copyright notices and reproduce this license
# (including the above copyright notice and the disclaimer and (if applicable) source
# code license limitations below) in the documentation and/or other materials provided
# with the distribution
#
# Redistribution and use in binary form, without modification, are permitted provided
# that the following conditions are met:
#
# *       No reverse engineering, decompilation, or disassembly of this software is
# permitted with respect to any software provided in binary form.
#
# *       any redistribution and use are licensed by TI for use only with TI Devices.
#
# *       Nothing shall obligate TI to provide you with source code for the software
# licensed and provided to you in object code.
#
# If software source code is provided to you, modification and redistribution of the
# source code are permitted provided that the following conditions are met:
#
# *       any redistribution and use of the source code, including any resulting derivative
# works, are licensed by TI for use only with TI Devices.
#
# *       any redistribution and use of any object code compiled from the source code
# and any resulting derivative works, are licensed by TI for use only with TI Devices.
#
# Neither the name of Texas Instruments Incorporated nor the names of its suppliers
#
# may be used to endorse or promote products derived from this software without
# specific prior written permission.
#
# DISCLAIMER.
#
# THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
# OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
# OF THE POSSIBILITY OF SUCH DAMAGE.
#


# File: component.mk
#       This file is component include make file of MCUSW.
# List of variables set in this file and their purpose:
# <mod>_RELPATH        - This is the relative path of the module, typically from
#                        top-level directory of the package
# <mod>_PATH           - This is the absolute path of the module. It derives from
#                        absolute path of the top-level directory (set in env.mk)
#                        and relative path set above
# <mod>_INCLUDE        - This is the path that has interface header files of the
#                        module. This can be multiple directories (space separated)
# <mod>_PKG_LIST       - Names of the modules (and sub-modules) that are a part
#                        part of this module, including itself.
# <mod>_BOARD_DEPENDENCY    - "yes": means the code for this module depends on
#                             board and the compiled obj/lib has to be kept
#                             under <board> directory
#                             "no" or "" or if this variable is not defined: means
#                             this module has no board dependent code and hence
#                             the obj/libs are not kept under <board> dir.
# <mod>_CORE_DEPENDENCY     - "yes": means the code for this module depends on
#                             core and the compiled obj/lib has to be kept
#                             under <core> directory
#                             "no" or "" or if this variable is not defined: means
#                             this module has no core dependent code and hence
#                             the obj/libs are not kept under <core> dir.
# <mod>_APP_STAGE_FILES     - List of source files that belongs to the module
#                             <mod>, but that needs to be compiled at application
#                             build stage (in the context of the app). This is
#                             primarily for link time configurations or if the
#                             source file is dependent on options/defines that are
#                             application dependent. This can be left blank or
#                             not defined at all, in which case, it means there
#                             no source files in the module <mod> that are required
#                             to be compiled in the application build stage.
# <mod>_FP_DEPENDENCY       - "yes": means the code for this module depends on
#                             target toolchain floating point support.  Enabling
#                             this option will enable floating point print
#                             support for toolchains which link out floating
#                             point print support to save memory.
#
ifeq ($(mcusw_mcuss_demos_make_include), )

mcusw_demos_default_SOCLIST          = j721e j7200 j721s2 j784s4
mcusw_demos_default_BOARDLIST        = j721e_sim j721e_evm j7200_evm j721s2_evm j784s4_evm
mcusw_demos_default_j721e_CORELIST   = mcu1_0 mpu1_0 mcu1_1
mcusw_demos_default_j7200_CORELIST   = mcu1_0 mpu1_0 mcu1_1
mcusw_demos_default_j721s2_CORELIST   = mcu1_0 mpu1_0 mcu1_1
mcusw_demos_default_j784s4_CORELIST   = mcu1_0 mpu1_0 mcu1_1

############################
# Demo Library
# List of components included under demo lib
# The components included here are built and will be part of demo lib
############################
mcusw_mcuss_demo_LIB_LIST =

############################
# mcal example
# List of components included under mcal examples
# The components included here are built and will be part of mcal lib
############################
mcusw_mcuss_demo_EXAMPLE_LIST =

############################
# mcal example
# List of baremetal examples included under mcal examples
# The components included here are built and will be part of mcal lib
############################
mcusw_baremetal_mcuss_demo_EXAMPLE_LIST =


# Library
# BSW_STUBS (TI RTOS)
bsw_stubs_rtos_COMP_LIST = bsw_stubs_rtos
bsw_stubs_rtos_RELPATH = mcuss_demos/Bsw_Stubs
bsw_stubs_rtos_PATH = $(mcuss_demo_PATH)/Bsw_Stubs
bsw_stubs_rtos_MAKEFILE = -fmakefile
export bsw_stubs_rtos_MAKEFILE
bsw_stubs_rtos_BOARD_DEPENDENCY = no
bsw_stubs_rtos_CORE_DEPENDENCY = no
export bsw_stubs_rtos_COMP_LIST
export bsw_stubs_rtos_BOARD_DEPENDENCY
export bsw_stubs_rtos_CORE_DEPENDENCY
bsw_stubs_rtos_PKG_LIST = bsw_stubs_rtos
export bsw_stubs_rtos_PKG_LIST
bsw_stubs_rtos_INCLUDE = $(bsw_stubs_rtos_PATH)/Det/inc
bsw_stubs_rtos_INCLUDE += $(bsw_stubs_rtos_PATH)/MemMap/inc
bsw_stubs_rtos_INCLUDE += $(bsw_stubs_rtos_PATH)/Os/inc
bsw_stubs_rtos_INCLUDE += $(bsw_stubs_rtos_PATH)/Dem/inc
bsw_stubs_rtos_INCLUDE += $(bsw_stubs_rtos_PATH)/Rte/inc
bsw_stubs_rtos_INCLUDE += $(bsw_stubs_rtos_PATH)/EcuM/inc
bsw_stubs_rtos_INCLUDE += $(bsw_stubs_rtos_PATH)/EthIf/inc
bsw_stubs_rtos_INCLUDE += $(bsw_stubs_rtos_PATH)/AsrGnrl/inc
bsw_stubs_rtos_INCLUDE += $(bsw_stubs_rtos_PATH)/CanIf/inc
bsw_stubs_rtos_INCLUDE += $(bsw_stubs_rtos_PATH)/WdgIf/inc
bsw_stubs_rtos_SOCLIST = $(mcusw_mcal_default_SOCLIST)
export bsw_stubs_rtos_SOCLIST
bsw_stubs_rtos_$(SOC)_CORELIST = $(mcusw_mcal_default_$(SOC)_CORELIST)
export bsw_stubs_rtos_$(SOC)_CORELIST
mcusw_mcuss_demo_LIB_LIST += bsw_stubs_rtos

# Utilities

# example utilities
demo_utils_COMP_LIST = demo_utils
demo_utils_RELPATH = mcuss_demos/demo_utils
demo_utils_PATH = $(mcuss_demo_PATH)/demo_utils
demo_utils_BOARD_DEPENDENCY = yes
demo_utils_CORE_DEPENDENCY = no
demo_utils_MAKEFILE = -fmakefile
export demo_utils_COMP_LIST
export demo_utils_BOARD_DEPENDENCY
export demo_utils_CORE_DEPENDENCY
export demo_utils_MAKEFILE
demo_utils_PKG_LIST = demo_utils
demo_utils_INCLUDE = $(demo_utils_PATH)
demo_utils_BOARDLIST = j721e_evm j7200_evm j721s2_evm j784s4_evm
export demo_utils_INCLUDE
export demo_utils_BOARDLIST
ifeq ($(BOARD),$(filter $(BOARD), j721e_evm j7200_evm j721s2_evm j784s4_evm))
demo_utils_$(SOC)_CORELIST = mcu1_0
endif
export demo_utils_$(SOC)_CORELIST
mcusw_mcuss_demo_LIB_LIST += demo_utils

# Demos

# Can Profiling
can_profile_app_COMP_LIST = can_profile_app
can_profile_app_RELPATH = can
can_profile_app_PATH = $(mcuss_demo_PATH)/profiling/can
can_profile_app_BOARD_DEPENDENCY = yes
can_profile_app_CORE_DEPENDENCY = no
export can_profile_app_COMP_LIST
export can_profile_app_BOARD_DEPENDENCY
export can_profile_app_CORE_DEPENDENCY
can_profile_app_PKG_LIST = can_profile_app
can_profile_app_INCLUDE = $(can_profile_app_PATH)
can_profile_app_BOARDLIST = $(mcusw_demos_default_BOARDLIST)
export can_profile_app_BOARDLIST
ifeq ($(BOARD),$(filter $(BOARD), j721e_evm j7200_evm j721s2_evm j784s4_evm))
can_profile_app_$(SOC)_CORELIST = mcu1_0 mcu2_1
endif
export can_profile_app_$(SOC)_CORELIST
can_profile_app_SBL_APPIMAGEGEN = yes
export can_profile_app_SBL_APPIMAGEGEN
ifeq ($(BUILD_OS_TYPE),$(filter $(BUILD_OS_TYPE), freertos))
    mcusw_mcuss_demo_EXAMPLE_LIST += can_profile_app
endif

# Boot loader, CAN response/profile app (tirtos)
can_boot_app_mcu_rtos_COMP_LIST = can_boot_app_mcu_rtos
can_boot_app_mcu_rtos_RELPATH = boot_app_mcu_rtos
can_boot_app_mcu_rtos_PATH = $(mcuss_demo_PATH)/boot_app_mcu_rtos
can_boot_app_mcu_rtos_BOARD_DEPENDENCY = yes
can_boot_app_mcu_rtos_CORE_DEPENDENCY = no
ifeq ($(BUILD_OS_TYPE), tirtos)
can_boot_app_mcu_rtos_XDC_CONFIGURO = yes
endif
export can_boot_app_mcu_rtos_COMP_LIST
export can_boot_app_mcu_rtos_BOARD_DEPENDENCY
export can_boot_app_mcu_rtos_CORE_DEPENDENCY
can_boot_app_mcu_rtos_PKG_LIST = can_boot_app_mcu_rtos
can_boot_app_mcu_rtos_INCLUDE = $(can_boot_app_mcu_rtos_PATH)
can_boot_app_mcu_rtos_BOARDLIST = j721e_evm j7200_evm j721s2_evm j784s4_evm
export can_boot_app_BOARDLIST
ifeq ($(BOARD),$(filter $(BOARD), j721e_evm j7200_evm j721s2_evm j784s4_evm))
can_boot_app_mcu_rtos_$(SOC)_CORELIST = mcu1_0
endif
export can_boot_app_mcu_rtos_$(SOC)_CORELIST
can_boot_app_mcu_rtos_SBL_APPIMAGEGEN = yes
export can_boot_app_mcu_rtos_SBL_APPIMAGEGEN

ifeq ($(BUILD_OS_TYPE),$(filter $(BUILD_OS_TYPE), tirtos freertos))
    mcusw_mcuss_demo_EXAMPLE_LIST += can_boot_app_mcu_rtos
endif

# Main Domain applications for can_boot_app_mcu_rtos (freertos), loaded by can_boot_app_mcu_rtos application
mcusw_ex01_multicore_demo_app_COMP_LIST = mcusw_ex01_multicore_demo_app
mcusw_ex01_multicore_demo_app_RELPATH = boot_app_mcu_rtos/main_domain_apps/ex01_multicore_demo_app
mcusw_ex01_multicore_demo_app_BINPATH = $(mcuss_demo_PATH)/boot_app_mcu_rtos/main_domain_apps/binary/bin
export mcusw_ex01_multicore_demo_app_OBJPATH = $(mcuss_demo_PATH)/boot_app_mcu_rtos/main_domain_apps/binary/obj
mcusw_ex01_multicore_demo_app_PATH = $(mcuss_demo_PATH)/boot_app_mcu_rtos/main_domain_apps/ex01_multicore_demo_app
mcusw_ex01_multicore_demo_app_MAKEFILE = -f$(mcuss_demo_PATH)/boot_app_mcu_rtos/main_domain_apps/ex01_multicore_demo_app/makefile BUILD_OS_TYPE=freertos
export mcusw_ex01_multicore_demo_app_MAKEFILE
mcusw_ex01_multicore_demo_app_BOARD_DEPENDENCY = yes
mcusw_ex01_multicore_demo_app_CORE_DEPENDENCY = yes
export mcusw_ex01_multicore_demo_app_COMP_LIST
export mcusw_ex01_multicore_demo_app_BOARD_DEPENDENCY
export mcusw_ex01_multicore_demo_app_CORE_DEPENDENCY
export mcusw_ex01_multicore_demo_app_XDC_CONFIGURO
mcusw_ex01_multicore_demo_app_PKG_LIST = mcusw_ex01_multicore_demo_app
mcusw_ex01_multicore_demo_app_INCLUDE = $(mcusw_ex01_multicore_demo_app_PATH)
mcusw_ex01_multicore_demo_app_BOARDLIST = j721e_evm j7200_evm
export mcusw_ex01_multicore_demo_app_BOARDLIST
# TODO - add mpu1_1 to the build
ifeq ($(BOARD),$(filter $(BOARD), j721e_evm))
mcusw_ex01_multicore_demo_app_$(SOC)_CORELIST = mcu2_0 mcu3_0 mcu2_1 mcu3_1 c66xdsp_1 c66xdsp_2 c7x_1
endif
ifeq ($(BOARD),$(filter $(BOARD), j7200_evm))
mcusw_ex01_multicore_demo_app_$(SOC)_CORELIST = mcu2_0 mcu2_1
endif
export mcusw_ex01_multicore_demo_app_$(SOC)_CORELIST

ifeq ($(BUILD_OS_TYPE), freertos)
    mcusw_mcuss_demo_EXAMPLE_LIST += mcusw_ex01_multicore_demo_app
endif

# Main Domain application for can_boot_app_mcu_rtos (baremetal), loaded by can_boot_app_mcu_rtos application
mcusw_baremetal_ex01_multicore_demo_app_COMP_LIST = mcusw_baremetal_ex01_multicore_demo_app
mcusw_baremetal_ex01_multicore_demo_app_RELPATH = boot_app_mcu_rtos/main_domain_apps/ex01_multicore_demo_app
mcusw_baremetal_ex01_multicore_demo_app_BINPATH = $(mcuss_demo_PATH)/boot_app_mcu_rtos/main_domain_apps/binary/bin
export mcusw_baremetal_ex01_multicore_demo_app_OBJPATH = $(mcuss_demo_PATH)/boot_app_mcu_rtos/main_domain_apps/binary/obj
mcusw_baremetal_ex01_multicore_demo_app_PATH = $(mcuss_demo_PATH)/boot_app_mcu_rtos/main_domain_apps/ex01_multicore_demo_app
mcusw_baremetal_ex01_multicore_demo_app_MAKEFILE = -f$(mcuss_demo_PATH)/boot_app_mcu_rtos/main_domain_apps/ex01_multicore_demo_app/makefile BUILD_OS_TYPE=baremetal

mcusw_baremetal_ex01_multicore_demo_app_CORE_DEPENDENCY = yes
export mcusw_baremetal_ex01_multicore_demo_app_COMP_LIST
export mcusw_baremetal_ex01_multicore_demo_app_BOARD_DEPENDENCY
export mcusw_baremetal_ex01_multicore_demo_app_CORE_DEPENDENCY
export mcusw_baremetal_ex01_multicore_demo_app_XDC_CONFIGURO
mcusw_baremetal_ex01_multicore_demo_app_PKG_LIST = mcusw_baremetal_ex01_multicore_demo_app
mcusw_baremetal_ex01_multicore_demo_app_INCLUDE = $(mcusw_baremetal_ex01_multicore_demo_app_PATH)
mcusw_baremetal_ex01_multicore_demo_app_BOARDLIST = j721e_evm j7200_evm
export mcusw_baremetal_ex01_multicore_demo_app_BOARDLIST

mcusw_baremetal_ex01_multicore_demo_app_$(SOC)_CORELIST = mpu1_0

export mcusw_baremetal_ex01_multicore_demo_app_$(SOC)_CORELIST

mcusw_baremetal_mcuss_demo_EXAMPLE_LIST += mcusw_baremetal_ex01_multicore_demo_app

# IPC PDK Demo with MCAL CDD IPC
ipc_remote_app_COMP_LIST = ipc_remote_app
ipc_remote_app_RELPATH = ipc_remote
ipc_remote_app_PATH = $(mcuss_demo_PATH)/inter_core_comm/ipc_remote
ipc_remote_app_BOARD_DEPENDENCY = yes
ipc_remote_app_CORE_DEPENDENCY = yes
export ipc_remote_app_COMP_LIST
export ipc_remote_app_BOARD_DEPENDENCY
export ipc_remote_app_CORE_DEPENDENCY
ipc_remote_app_PKG_LIST = ipc_remote_app
ipc_remote_app_INCLUDE = $(ipc_remote_app_PATH)
ipc_remote_app_BOARDLIST = j721e_evm j7200_evm j721s2_evm j784s4_evm
export ipc_remote_app_BOARDLIST
ifeq ($(BOARD),$(filter $(BOARD), j721e_evm))
ipc_remote_app_$(SOC)_CORELIST = mpu1_0 mcu1_1 mcu2_0 mcu2_1 mcu3_0 mcu1_0
endif
ifeq ($(BOARD),$(filter $(BOARD), j7200_evm))
ipc_remote_app_$(SOC)_CORELIST = mpu1_0 mcu1_1 mcu2_0 mcu2_1 mcu1_0
endif
ifeq ($(BOARD),$(filter $(BOARD), j721s2_evm))
ipc_remote_app_$(SOC)_CORELIST = mcu2_0 mcu2_1
endif
ifeq ($(BOARD),$(filter $(BOARD), j784s4_evm))
ipc_remote_app_$(SOC)_CORELIST = mcu2_0 mcu2_1
endif
export ipc_remote_app_$(SOC)_CORELIST
ipc_remote_app_SBL_APPIMAGEGEN = yes
export ipc_remote_app_SBL_APPIMAGEGEN
ifeq ($(BUILD_OS_TYPE),$(filter $(BUILD_OS_TYPE), freertos))
    mcusw_mcuss_demo_EXAMPLE_LIST += ipc_remote_app
endif

# IPC PDK Demo with MCAL CDD IPC
ipc_remote_test1_COMP_LIST = ipc_remote_test1
ipc_remote_test1_RELPATH = ipc_remote_test1
ipc_remote_test1_PATH = $(mcuss_demo_PATH)/inter_core_comm/ipc_remote/ipc_remote_test1
ipc_remote_test1_BOARD_DEPENDENCY = yes
ipc_remote_test1_CORE_DEPENDENCY = yes
export ipc_remote_test1_COMP_LIST
export ipc_remote_test1_BOARD_DEPENDENCY
export ipc_remote_test1_CORE_DEPENDENCY
ipc_remote_test1_PKG_LIST = ipc_remote_test1
ipc_remote_test1_INCLUDE = $(ipc_remote_test1_PATH)
ipc_remote_test1_BOARDLIST = j721e_evm j7200_evm j721s2_evm j784s4_evm
export ipc_remote_test1_BOARDLIST
ifeq ($(BOARD),$(filter $(BOARD), j721e_evm))
ipc_remote_test1_$(SOC)_CORELIST = mpu1_0 mcu1_1 mcu2_0 mcu2_1 mcu3_0 mcu1_0
endif
ifeq ($(BOARD),$(filter $(BOARD), j7200_evm))
ipc_remote_test1_$(SOC)_CORELIST = mpu1_0 mcu1_1 mcu2_0 mcu2_1 mcu1_0
endif
ifeq ($(BOARD),$(filter $(BOARD), j721s2_evm))
ipc_remote_test1_$(SOC)_CORELIST = mcu2_0 mcu2_1
endif
ifeq ($(BOARD),$(filter $(BOARD), j784s4_evm))
ipc_remote_test1_$(SOC)_CORELIST = mcu2_0 mcu2_1
endif
export ipc_remote_test1_$(SOC)_CORELIST
ipc_remote_test1_SBL_APPIMAGEGEN = yes
export ipc_remote_test1_SBL_APPIMAGEGEN
ifeq ($(BUILD_OS_TYPE),$(filter $(BUILD_OS_TYPE), freertos))
    mcusw_mcuss_demo_EXAMPLE_LIST += ipc_remote_test1
endif

# IPC PDK Demo for same remote multi channel end points with MCAL CDD IPC
ipc_remote_2chnls_app_COMP_LIST = ipc_remote_2chnls_app
ipc_remote_2chnls_app_RELPATH = ipc_remote_2chnls
ipc_remote_2chnls_app_PATH = $(mcuss_demo_PATH)/inter_core_comm/ipc_remote/ipc_remote_2chnls
ipc_remote_2chnls_app_BOARD_DEPENDENCY = yes
ipc_remote_2chnls_app_CORE_DEPENDENCY = yes
export ipc_remote_2chnls_app_COMP_LIST
export ipc_remote_2chnls_app_BOARD_DEPENDENCY
export ipc_remote_2chnls_app_CORE_DEPENDENCY
ipc_remote_2chnls_app_PKG_LIST = ipc_remote_2chnls_app
ipc_remote_2chnls_app_INCLUDE = $(ipc_remote_2chnls_app_PATH)
ipc_remote_2chnls_app_BOARDLIST = j721e_evm j7200_evm j721s2_evm j784s4_evm
export ipc_remote_2chnls_app_BOARDLIST
ifeq ($(BOARD),$(filter $(BOARD), j721e_evm))
ipc_remote_2chnls_app_$(SOC)_CORELIST = mpu1_0 mcu1_1 mcu2_0 mcu2_1 mcu3_0 mcu1_0
endif
ifeq ($(BOARD),$(filter $(BOARD), j7200_evm))
ipc_remote_2chnls_app_$(SOC)_CORELIST = mpu1_0 mcu1_1 mcu2_0 mcu2_1 mcu1_0
endif
ifeq ($(BOARD),$(filter $(BOARD), j721s2_evm))
ipc_remote_2chnls_app_$(SOC)_CORELIST = mpu1_0 mcu1_1 mcu2_0 mcu2_1 mcu3_0 mcu1_0
endif
ifeq ($(BOARD),$(filter $(BOARD), j784s4_evm))
ipc_remote_2chnls_app_$(SOC)_CORELIST = mpu1_0 mcu1_1 mcu2_0 mcu2_1 mcu3_0 mcu1_0
endif
export ipc_remote_2chnls_app_$(SOC)_CORELIST
ipc_remote_2chnls_app_SBL_APPIMAGEGEN = yes
export ipc_remote_2chnls_app_SBL_APPIMAGEGEN
ifeq ($(BUILD_OS_TYPE),$(filter $(BUILD_OS_TYPE), freertos))
    mcusw_mcuss_demo_EXAMPLE_LIST += ipc_remote_2chnls_app
endif

# CDD IPC Profiling
cdd_ipc_profile_app_COMP_LIST = cdd_ipc_profile_app
cdd_ipc_profile_app_RELPATH = cddIpc
cdd_ipc_profile_app_PATH = $(mcuss_demo_PATH)/profiling/cddIpc
cdd_ipc_profile_app_BOARD_DEPENDENCY = yes
cdd_ipc_profile_app_CORE_DEPENDENCY = no
export cdd_ipc_profile_app_COMP_LIST
export cdd_ipc_profile_app_BOARD_DEPENDENCY
export cdd_ipc_profile_app_CORE_DEPENDENCY
cdd_ipc_profile_app_PKG_LIST = cdd_ipc_profile_app
cdd_ipc_profile_app_INCLUDE = $(cdd_ipc_profile_app_PATH)
cdd_ipc_profile_app_BOARDLIST = j721e_evm j7200_evm
ifeq ($(BOARD),$(filter $(BOARD), j721e_evm j7200_evm))
cdd_ipc_profile_app_$(SOC)_CORELIST = mcu1_0
endif
export cdd_ipc_profile_app_BOARDLIST
export cdd_ipc_profile_app_$(SOC)_CORELIST
cdd_ipc_profile_app_SBL_APPIMAGEGEN = yes
export cdd_ipc_profile_app_SBL_APPIMAGEGEN
ifeq ($(BUILD_OS_TYPE),$(filter $(BUILD_OS_TYPE), freertos))
    mcusw_mcuss_demo_EXAMPLE_LIST += cdd_ipc_profile_app
endif

# CDD IPC Profiling with Remoteproc Linux
cdd_ipc_profile_app_rc_linux_COMP_LIST = cdd_ipc_profile_app_rc_linux
cdd_ipc_profile_app_rc_linux_RELPATH = cddIpcRProcLinux
cdd_ipc_profile_app_rc_linux_PATH = $(mcuss_demo_PATH)/profiling/cddIpcRProcLinux
cdd_ipc_profile_app_rc_linux_BOARD_DEPENDENCY = yes
cdd_ipc_profile_app_rc_linux_CORE_DEPENDENCY = no
ifeq ($(BUILD_OS_TYPE), tirtos)
cdd_ipc_profile_app_rc_linux_XDC_CONFIGURO = yes
endif
export cdd_ipc_profile_app_rc_linux_COMP_LIST
export cdd_ipc_profile_app_rc_linux_BOARD_DEPENDENCY
export cdd_ipc_profile_app_rc_linux_CORE_DEPENDENCY
cdd_ipc_profile_app_rc_linux_PKG_LIST = cdd_ipc_profile_app_rc_linux
cdd_ipc_profile_app_rc_linux_INCLUDE = $(cdd_ipc_profile_app_rc_linux_PATH)
cdd_ipc_profile_app_rc_linux_BOARDLIST = j721e_evm j7200_evm
export cdd_ipc_profile_app_rc_linux_BOARDLIST
ifeq ($(BOARD),$(filter $(BOARD), j721e_evm j7200_evm))
cdd_ipc_profile_app_rc_linux_$(SOC)_CORELIST = mcu1_0
endif
export cdd_ipc_profile_app_rc_linux_$(SOC)_CORELIST
cdd_ipc_profile_app_rc_linux_SBL_APPIMAGEGEN = yes
export cdd_ipc_profile_app_rc_linux_SBL_APPIMAGEGEN
ifeq ($(BUILD_OS_TYPE), tirtos)
    mcusw_mcuss_demo_EXAMPLE_LIST += cdd_ipc_profile_app_rc_linux
endif

# Can Profiling App in XIP mode
can_profile_xip_app_COMP_LIST = can_profile_xip_app
can_profile_xip_app_RELPATH = can_profile_xip
can_profile_xip_app_PATH = $(mcuss_demo_PATH)/profiling/can_profile_xip
can_profile_xip_app_BOARD_DEPENDENCY = yes
can_profile_xip_app_CORE_DEPENDENCY = no
ifeq ($(BUILD_OS_TYPE), tirtos)
can_profile_xip_app_XDC_CONFIGURO = yes
endif
export can_profile_xip_app_COMP_LIST
export can_profile_xip_app_BOARD_DEPENDENCY
export can_profile_xip_app_CORE_DEPENDENCY
can_profile_xip_app_PKG_LIST = can_profile_xip_app
can_profile_xip_app_INCLUDE = $(can_profile_xip_app_PATH)
can_profile_xip_app_BOARDLIST = j721e_evm j7200_evm
export can_profile_xip_app_BOARDLIST
ifeq ($(BOARD),$(filter $(BOARD), j721e_evm j7200_evm))
can_profile_xip_app_$(SOC)_CORELIST = mcu1_0
endif
export can_profile_xip_app_$(SOC)_CORELIST
can_profile_xip_app_SBL_APPIMAGEGEN = yes
can_profile_xip_app_$(BUILD_OS_TYPE)_SBL_XIP_APPIMAGEGEN = yes
export can_profile_xip_app_SBL_APPIMAGEGEN
export can_profile_xip_app_$(BUILD_OS_TYPE)_SBL_XIP_APPIMAGEGEN

ifeq ($(BUILD_OS_TYPE), $(filter $(BUILD_OS_TYPE), tirtos freertos))
    mcusw_mcuss_demo_EXAMPLE_LIST += can_profile_xip_app
endif

# Can Profiling App in XIP mode
can_profile_xip_fota_app_COMP_LIST = can_profile_xip_fota_app
can_profile_xip_fota_app_RELPATH = can_profile_xip_fota
can_profile_xip_fota_app_PATH = $(mcuss_demo_PATH)/profiling/can_profile_xip_fota
can_profile_xip_fota_app_BOARD_DEPENDENCY = yes
can_profile_xip_fota_app_CORE_DEPENDENCY = no
can_profile_xip_fota_app_XDC_CONFIGURO = yes
export can_profile_xip_fota_app_COMP_LIST
export can_profile_xip_fota_app_BOARD_DEPENDENCY
export can_profile_xip_fota_app_CORE_DEPENDENCY
can_profile_xip_fota_app_PKG_LIST = can_profile_xip_fota_app
can_profile_xip_fota_app_INCLUDE = $(can_profile_xip_fota_app_PATH)
can_profile_xip_fota_app_BOARDLIST = j721e_evm j7200_evm
export can_profile_xip_fota_app_BOARDLIST
ifeq ($(BOARD),$(filter $(BOARD), j721e_evm j7200_evm))
can_profile_xip_fota_app_$(SOC)_CORELIST = mcu1_0
endif
export can_profile_xip_fota_app_$(SOC)_CORELIST
can_profile_xip_fota_app_SBL_APPIMAGEGEN = yes
can_profile_xip_fota_app_SBL_XIP_APPIMAGEGEN = yes
export can_profile_xip_fota_app_SBL_APPIMAGEGEN
export can_profile_xip_fota_app_SBL_XIP_APPIMAGEGEN

ifeq ($(BUILD_OS_TYPE),$(filter $(BUILD_OS_TYPE), tirtos freertos))
    mcusw_mcuss_demo_EXAMPLE_LIST += can_profile_xip_fota_app
endif

export mcusw_mcuss_demo_LIB_LIST
export mcusw_mcuss_demo_EXAMPLE_LIST

mcusw_mcuss_demos_make_include := 1
endif
