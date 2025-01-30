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
ifeq ($(mcusw_component_make_include), )

DEFAULT_RTOS_LIST = freertos

############################
# device mcusw package
# List of components included under mcusw lib
# The components included here are built and will be part of mcusw lib
############################
mcusw_LIB_LIST =

############################
# device mcusw firmware
# List of components included under mcusw firmware
# The components included here are built and will be part of mcusw firmware
############################
mcusw_FIRM_LIST =

############################
# device mcusw application utility packages
# List of application utilities under mcusw
# The components included here are built and will be part of mcusw app_lib
############################
mcusw_APP_LIB_LIST =

############################
# device mcusw examples
# List of examples under mcusw
# All the tests mentioned in list are built when test target is called
# List below all examples for allowed values
############################
mcusw_EXAMPLE_LIST =

############################
# device mcusw unittest
# List of UT's under mcusw
# All the tests mentioned in list are built when test target is called
# List below all examples for allowed values
############################
mcusw_test_EXAMPLE_LIST =

############################
# Duplicate example list which should not be built with "all" (since this is
# duplicate -j option will build both and result in .obj correuption) but needs
# to be still exposed to top level makefile so that user can individually call
# them. Also these duplicate targets doesn't support package option and hence
# should not be included when packaging
############################
mcusw_DUP_EXAMPLE_LIST =

# Components included

#include each module component makefile
-include $(MCUSW_INSTALL_PATH)/mcal_drv/mcusw_mcal_drv.mk
ifeq ($(BUILD_OS_TYPE),baremetal)
  ifneq ($(mcusw_mcal_drv_LIB_LIST),)
    mcusw_LIB_LIST += $(mcusw_mcal_drv_LIB_LIST)
  endif
  ifneq ($(mcusw_mcal_drv_EXAMPLE_LIST),)
    mcusw_EXAMPLE_LIST += $(mcusw_mcal_drv_EXAMPLE_LIST)
  endif
  ifneq ($(mcusw_mcal_dup_drv_EXAMPLE_LIST),)
    mcusw_DUP_EXAMPLE_LIST += $(mcusw_mcal_dup_drv_EXAMPLE_LIST)
  endif
endif

#include each module component makefile
-include $(MCUSW_INSTALL_PATH)/mcal_drv/mcusw_mcal_drv.mk
ifeq ($(BUILD_OS_TYPE),baremetal)
  ifneq ($(mcusw_mcal_drv_test_EXAMPLE_LIST),)
    mcusw_test_EXAMPLE_LIST += $(mcusw_mcal_drv_test_EXAMPLE_LIST)
  endif
endif

-include $(MCUSW_INSTALL_PATH)/mcal_drv/mcusw_mcal_drv.mk

ifneq ($(BUILD_OS_TYPE),baremetal)
-include $(MCUSW_INSTALL_PATH)/mcuss_demos/mcusw_mcuss_demos.mk
endif
ifeq ($(BUILD_OS_TYPE),freertos)
  ifneq ($(mcusw_mcuss_demo_LIB_LIST),)
    mcusw_LIB_LIST += $(mcusw_mcuss_demo_LIB_LIST)
  endif
  ifneq ($(mcusw_mcuss_demo_EXAMPLE_LIST),)
    mcusw_EXAMPLE_LIST += $(mcusw_mcuss_demo_EXAMPLE_LIST)
  endif
else ifeq ($(BUILD_OS_TYPE),baremetal)
  ifneq ($(mcusw_mcuss_demo_LIB_LIST),)
    mcusw_LIB_LIST += $(mcusw_mcuss_demo_LIB_LIST)
  endif
  ifneq ($(mcusw_baremetal_mcuss_demo_EXAMPLE_LIST),)
    mcusw_EXAMPLE_LIST += $(mcusw_baremetal_mcuss_demo_EXAMPLE_LIST)
  endif
endif

#include PDK modules
-include $(PDK_INSTALL_PATH)/ti/build/comp_paths.mk
-include $(PDK_CSL_COMP_PATH)/csl_component.mk
-include $(PDK_UDMA_COMP_PATH)/udma_component.mk
-include $(PDK_OSAL_COMP_PATH)/osal_component.mk
-include $(PDK_FREERTOS_COMP_PATH)/freertos_component.mk
-include $(PDK_SCICLIENT_COMP_PATH)/sciclient_component.mk
-include $(PDK_IPC_COMP_PATH)/ipc_component.mk
-include $(PDK_SBL_COMP_PATH)/sbl_component.mk
-include $(PDK_I2C_COMP_PATH)/i2c_component.mk
-include $(PDK_FATFS_COMP_PATH)/fatfs_component.mk
-include $(PDK_MMCSD_COMP_PATH)/mmcsd_component.mk
-include $(PDK_UDMA_COMP_PATH)/udma_component.mk
-include $(PDK_ENET_COMP_PATH)/enet_component.mk

ifneq ($(csl_LIB_LIST),)
  mcusw_pdk_LIB_LIST += csl
  ifneq ($(CORE),$(filter $(CORE), c7x_1 c66xdsp_1 c66xdsp_2))
    mcusw_pdk_LIB_LIST += csl_init
  endif
  ifeq ($(CORE),$(filter $(CORE), c66xdsp_1 c66xdsp_2))
    mcusw_pdk_LIB_LIST += csl_intc
  endif
endif
ifneq ($(udma_LIB_LIST),)
  mcusw_pdk_LIB_LIST += udma
endif
ifneq ($(osal_LIB_LIST),)
  ifeq ($(BUILD_OS_TYPE),freertos)
    mcusw_pdk_LIB_LIST += osal_freertos
  endif
  ifeq ($(BUILD_OS_TYPE),baremetal)
    mcusw_pdk_LIB_LIST += osal_nonos
  endif
endif
ifeq ($(BUILD_OS_TYPE),freertos)
		mcusw_pdk_LIB_LIST += freertos
endif

-include $(PDK_SCICLIENT_COMP_PATH)/sciclient_component.mk
ifeq ($(SOC),$(filter $(SOC), j721e j7200 j721s2 j784s4 am62x am62ax am62px j722s))
ifeq ($(CORE),mcu1_0)
ifneq ($(sciclient_LIB_LIST),)
  mcusw_pdk_LIB_LIST += sciclient_direct
  mcusw_pdk_LIB_LIST += rm_pm_hal
endif
ifeq ($(BUILD_OS_TYPE),freertos)
    ifneq ($(sciclient_LIB_LIST),)
      mcusw_pdk_LIB_LIST += sciserver_tirtos
    endif
else
    ifneq ($(sciclient_LIB_LIST),)
      mcusw_pdk_LIB_LIST += sciserver_tirtos
    endif
endif
else
  ifneq ($(sciclient_LIB_LIST),)
     mcusw_pdk_LIB_LIST += sciclient
  endif
endif
else
  ifneq ($(sciclient_LIB_LIST),)
     mcusw_pdk_LIB_LIST += sciclient
  endif
endif

ifneq ($(ipc_LIB_LIST),)
ifeq ($(SOC),$(filter $(SOC), j721e j7200 j721s2 j784s4))
  ifeq ($(BUILD_OS_TYPE),baremetal)
    mcusw_pdk_LIB_LIST += ipc_baremetal
endif
endif
	ifeq ($(BUILD_OS_TYPE),freertos)
		mcusw_pdk_LIB_LIST += ipc
    mcusw_pdk_LIB_LIST += ipc_baremetal
  endif
endif

ifneq ($(drvi2c_LIB_LIST),)
  mcusw_pdk_LIB_LIST += $(i2c_LIB_LIST)
endif

ifneq ($(sbl_LIB_LIST),)
  mcusw_pdk_LIB_LIST += $(sbl_LIB_LIST)
endif

ifneq ($(fatfs_LIB_LIST),)
  mcusw_pdk_LIB_LIST += $(fatfs_LIB_LIST)
endif

ifneq ($(mmcsd_LIB_LIST),)
  mcusw_pdk_LIB_LIST += $(mmcsd_LIB_LIST)
endif

ifneq ($(enet_LIB_LIST),)
  mcusw_pdk_LIB_LIST += enet
  mcusw_pdk_LIB_LIST += enetsoc
endif

#include board
-include $(PDK_INSTALL_PATH)/ti/build/comp_paths.mk
-include $(PDK_BOARD_COMP_PATH)/board_component.mk
ifneq ($(board_LIB_LIST),)
  mcusw_pdk_LIB_LIST += board
endif

#include uart
-include $(PDK_INSTALL_PATH)/ti/build/comp_paths.mk
-include $(PDK_UART_COMP_PATH)/uart_component.mk
ifneq ($(uart_LIB_LIST),)
  mcusw_pdk_LIB_LIST += uart
endif

#include PM module
-include $(PDK_INSTALL_PATH)/ti/build/comp_paths.mk
-include $(PDK_PM_COMP_PATH)/pm_component.mk
ifneq ($(pm_LIB_LIST),)
  mcusw_pdk_LIB_LIST += pm_lib
endif

#include PDK SPI Drv module

mcusw_PKG_LIST_ALL = $(mcusw_EXAMPLE_LIST) $(mcusw_LIB_LIST) $(mcusw_APP_LIB_LIST) $(mcusw_test_EXAMPLE_LIST)

# Component specific CFLAGS
MCUSW_CFLAGS =
MCUSW_LNKFLAGS =

#Default to all if not set from rules.make or command line
ifeq ($(PACKAGE_SELECT), )
  PACKAGE_SELECT := all
endif

ifeq ($(CORE),mcu0_0)
  MCUSW_CFLAGS += -DBUILD_MCU0_0 -DBUILD_MCU
  MCUSW_LNKFLAGS += -Xlinker --define=BUILD_MCU0_0 -Xlinker --define=BUILD_MCU  -Xlinker --diag_suppress=10068
  LIB_ENDIAN_LIST = little
endif

ifeq ($(CORE),mcu1_0)
  MCUSW_CFLAGS += -DBUILD_MCU1_0 -DBUILD_MCU
  MCUSW_LNKFLAGS += -Xlinker --define=BUILD_MCU1_0 -Xlinker --define=BUILD_MCU  -Xlinker --diag_suppress=10068
  LIB_ENDIAN_LIST = little
endif

ifeq ($(CORE),mcu1_1)
  MCUSW_CFLAGS += -DBUILD_MCU1_1 -DBUILD_MCU
  MCUSW_LNKFLAGS += -Xlinker --define=BUILD_MCU1_1 -Xlinker --define=BUILD_MCU  -Xlinker --diag_suppress=10068
  LIB_ENDIAN_LIST = little
endif

ifeq ($(CORE),mcu2_0)
  MCUSW_CFLAGS += -DBUILD_MCU2_0 -DBUILD_MCU
  MCUSW_LNKFLAGS += -Xlinker --define=BUILD_MCU2_0 -Xlinker --define=BUILD_MCU  -Xlinker --diag_suppress=10068
  LIB_ENDIAN_LIST = little
endif

ifeq ($(CORE),mcu2_1)
  MCUSW_CFLAGS += -DBUILD_MCU2_1 -DBUILD_MCU
  MCUSW_LNKFLAGS += -Xlinker --define=BUILD_MCU2_1 -Xlinker --define=BUILD_MCU  -Xlinker --diag_suppress=10068
  LIB_ENDIAN_LIST = little
endif

ifeq ($(CORE),mcu3_0)
  MCUSW_CFLAGS += -DBUILD_MCU3_0 -DBUILD_MCU
  MCUSW_LNKFLAGS += -Xlinker --define=BUILD_MCU3_0 -Xlinker --define=BUILD_MCU  -Xlinker --diag_suppress=10068
  LIB_ENDIAN_LIST = little
endif

ifeq ($(CORE),mcu3_1)
  MCUSW_CFLAGS += -DBUILD_MCU3_1 -DBUILD_MCU
  MCUSW_LNKFLAGS += -Xlinker --define=BUILD_MCU3_1 -Xlinker --define=BUILD_MCU  -Xlinker --diag_suppress=10068
  LIB_ENDIAN_LIST = little
endif

ifeq ($(CORE),$(filter $(CORE), c66x c66xdsp_1 c66xdsp_2))
  ifeq ($(CORE),$(filter $(CORE), c66xdsp_1))
    MCUSW_CFLAGS += -DBUILD_DSP_1 -DBUILD_C66X_1
    MCUSW_LNKFLAGS += --define=BUILD_DSP_1 --define=BUILD_C66X_1
  endif
  ifeq ($(CORE),$(filter $(CORE), c66xdsp_2))
    MCUSW_CFLAGS += -DBUILD_DSP_2 -DBUILD_C66X_2
    MCUSW_LNKFLAGS += --define=BUILD_DSP_2 --define=BUILD_C66X_2
  endif
endif

ifeq ($(CORE),$(filter $(CORE), c7x_1 c7x-hostemu))
  MCUSW_CFLAGS += -DBUILD_C7X_1
  MCUSW_LNKFLAGS += --define=BUILD_C7X_1
endif

# These defines will be removed once all modules are ported to AUTOSAR 4.3.1
ifeq ($(AUTOSAR_VERSION),421)
    MCUSW_CFLAGS += -DAUTOSAR_421
endif

ifeq ($(AUTOSAR_VERSION),431)
    MCUSW_CFLAGS += -DAUTOSAR_431
endif

export MCUSW_CFLAGS
export MCUSW_LNKFLAGS

mcusw_component_make_include := 1
endif
