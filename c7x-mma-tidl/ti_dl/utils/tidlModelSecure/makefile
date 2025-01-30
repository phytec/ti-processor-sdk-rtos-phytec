#******************************************************************************
#*      Copyright (C) 2024 Texas Instruments Incorporated.               *
#*                      All Rights Reserved                                   *
#******************************************************************************

##############################################################
TARGET_PLATFORM = PC

ALGBASE_PATH ?= $(abspath ../../../)
include $(ALGBASE_PATH)/makerules/config.mk
COMMON_DIR= ../../../common
SUBDIRS= ./src
CFILES := $(foreach dir,$(SUBDIRS),$(wildcard $(dir)/*.c))
CFILES += ../../../ti_dl/algo/src/priv/tidl_model_play.c

OUTFILE= ./out/tidl_model_secure.out
##############################################################
include $(ALGBASE_PATH)/makerules/common.mk
include $(ALGBASE_PATH)/makerules/rules.mk
#used inside makerules, but okay to define it afterwards
CFLAGS+= -I .
CFLAGS+= -I ./src
CFLAGS+= -I ../../inc
CFLAGS+= -I $(COMMON_DIR)
CFLAGS+= -I $(IVISION_PATH)
CFLAGS+= -I $(DSP_COMMON_DIR)
##############################################################
OUTDIR =  "./out"
$(OUTFILE) : outfile
##############################################################

