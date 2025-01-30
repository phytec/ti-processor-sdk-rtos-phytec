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


# File: env.mk. This file contains all the paths and other ENV variables

#
# Module paths
#

# Destination root directory.
#   - specify the directory where you want to place the object, archive/library,
#     binary and other generated files in a different location than source tree
#   - or leave it blank to place then in the same tree as the source
ifeq ($(MCAL_CONFIG),0)
  DEST_ROOT = $(MCUSW_INSTALL_PATH)/binary
else
  DEST_ROOT = $(MCUSW_INSTALL_PATH)/binary/config_$(MCAL_CONFIG)
endif

# Utilities directory. This is required only if the build machine is Windows.
#   - specify the installation directory of utility which supports POSIX commands
#     (eg: Cygwin installation or MSYS installation).
UTILS_INSTALL_DIR ?= $(utils_PATH)

# Set path separator, etc based on the OS
ifeq ($(OS),Windows_NT)
  PATH_SEPARATOR = ;
  UTILSPATH = $(UTILS_INSTALL_DIR)/
  export SHELL := $(UTILSPATH)sh.exe
else
  # else, assume it is linux
  PATH_SEPARATOR = :
  UTILSPATH = /bin/
endif

# Autosar framework include files
export autosarBSW_INCLUDE = $(autosarBSWInc_PATH)
#Autosar specific compiler files include path
export autosarCompiler_INCLUDE = $(autosarCompilerTypes_PATH)
#Autosar generated files include path
autosarConfig_INCLUDE = $(autosarConfig_PATH)

# pdk drivers
export pdk_INCLUDE = $(pdk_PATH)

# MCAL
export mcal_INCLUDE = $(mcal_PATH)


include $(MCUSW_INSTALL_PATH)/build/makerules/component.mk

# Package Relative path and destination directory
export PACKAGE_RELPATH = mcusw
export PACKAGE_ROOT = $(DEST_ROOT)/package/$(PACKAGE_SELECT)/$(PACKAGE_RELPATH)

# Commands commonly used within the make files
RM = $(UTILSPATH)rm -f
MV = $(UTILSPATH)mv
RMDIR = $(UTILSPATH)rm -rf
MKDIR = $(UTILSPATH)mkdir
ECHO = @$(UTILSPATH)echo

#emake provides detailed info for build analysis.
EMAKE = emake --emake-emulation=gmake --emake-gen-subbuild-db=1 --emake-annodetail=waiting

ifeq ($(OS),Windows_NT)
  MAKE = gmake
#  MAKE =  $(EMAKE)
else
  MAKE = make
endif
EGREP = $(UTILSPATH)egrep
CP = $(UTILSPATH)cp
ifeq ($(OS),Windows_NT)
  CHMOD = $(UTILSPATH)echo
else
  CHMOD = $(UTILSPATH)chmod
endif

ifeq ($(BUILD_OS_TYPE),freertos)
  ifeq ($(SOC),$(filter $(SOC), j721e j7200 j721s2 j784s4 am62x am62ax am62px j722s))
    ifeq ($(CONFIG_BLD_XDC_r5f),)
      CONFIG_BLD_XDC_r5f = $(pdk_PATH)/ti/build/$(SOC)/config_$(SOC)_r5f.bld
      CONFIG_BLD_LNK_r5f = $(MCUSW_INSTALL_PATH)/build/$(SOC)/$(CORE)/linker_r5_freertos.lds
    endif
  endif
else
  ifeq ($(SOC),$(filter $(SOC), j721e j7200 j721s2 j784s4 am62x am62ax am62px j722s))
    ifeq ($(CONFIG_BLD_LNK_r5f),)
      CONFIG_BLD_LNK_r5f   = $(MCUSW_INSTALL_PATH)/build/$(SOC)/$(CORE)/linker_r5.lds
    endif
  endif
endif


CGTOOLS = $(TOOLCHAIN_PATH_R5)
export CGTOOLS
export CGTOOLS_A53 = $(TOOLCHAIN_PATH_A53)
export CGTOOLS_A72 = $(TOOLCHAIN_PATH_A72)

# Nothing beyond this point
