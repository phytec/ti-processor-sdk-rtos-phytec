#
# module name       :TIDL
#
# module descripton :TI Deep learning Library module is TIs CNN/DNN
#                    acceleration on EVE and DSP
#
# Copyright (C) 2016-2017 Texas Instruments Incorporated - http://www.ti.com/
#
# ALL RIGHTS RESERVED
#
#
#
ALGBASE_PATH ?= $(abspath ../../../../)
PSDK_INSTALL_PATH ?= $(abspath ../../../../../)
include $(ALGBASE_PATH)/makerules/config.mk

##############################################################
DSP_COMMON_DIR= ../../../../common

##############################################################


##############################################################

SUBDIRS = ./inc
SUBDIRS+= .

CFILES= $(foreach dir,$(SUBDIRS),$(wildcard $(dir)/*.c))
HFILES = $(foreach dir,$(SUBDIRS),$(wildcard $(dir)/*.h))

CMD_FILE= ../../tidl_alg.cmd

LIB_DIR=algo

ifeq ($(TIDL_BUILD_PATHS), LEGACY)
ifeq ($(TARGET_PLATFORM) , PC)
LIBDIR  = ../../../lib/PC/$(CORE)/$(LIB_DIR)/$(TARGET_BUILD)
else
LIBDIR  = ../../../lib/$(CORE)/$(LIB_DIR)/$(TARGET_BUILD)
endif
else
ifeq ($(TARGET_PLATFORM) , PC)
LIBDIR  = ../../../lib/$(TARGET_SOC)/PC/$(LIB_DIR)/$(TARGET_BUILD)
else
LIBDIR  = ../../../lib/$(TARGET_SOC)/$(CORE)/$(LIB_DIR)/$(TARGET_BUILD)
endif
endif

ifeq ($(TARGET_PLATFORM) , PC)
ifdef SystemRoot
OUTFILE=$(LIBDIR)/tidl_priv_algo.lib
else
OUTFILE=$(LIBDIR)/libtidl_priv_algo.a
endif
else
OUTFILE=$(LIBDIR)/tidl_priv_algo.lib
endif

##############################################################


##############################################################
include $(ALGBASE_PATH)/makerules/rules.mk
include $(ALGBASE_PATH)/makerules/common.mk
#used inside makerules, but okay to define it afterwards
INCLUDE_PATHS+= -I .
INCLUDE_PATHS+= -I ../../inc
INCLUDE_PATHS+= -I ../../../inc
INCLUDE_PATHS+= -I $(DSP_COMMON_DIR)
INCLUDE_PATHS+= -I ../../../utils/perfsim
INCLUDE_PATHS+= -I src/tidsp/inc
INCLUDE_PATHS+= -I src/tidsp/inc_priv

CFLAGS += $(INCLUDE_PATHS)

##############################################################


##############################################################
LDFLAGS= -l "../..//tidl_alg.cmd"
$(OUTFILE) : libfile

##############################################################

