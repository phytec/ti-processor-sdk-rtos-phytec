#******************************************************************************
#*      Copyright (C) 2015 Texas Instruments Incorporated.                    *
#*                      All Rights Reserved                                   *
#******************************************************************************

ifeq ($(TARGET_CPU),C66)
TARGETFOLDER := c66
else ifeq ($(TARGET_CPU),C7100)
TARGETFOLDER := c7100
else ifeq ($(TARGET_CPU),C7120)
TARGETFOLDER := c7120
else ifeq ($(TARGET_CPU),RESERVED)
TARGETFOLDER := RESERVED
else ifeq ($(TARGET_CPU),RESERVED)
TARGETFOLDER := RESERVED
else ifeq ($(TARGET_CPU),RESERVED)
TARGETFOLDER := RESERVED
else ifeq ($(TARGET_CPU),RESERVED)
TARGETFOLDER := RESERVED
else
TARGETFOLDER := c7100 # got to C7100 by default
endif

# Copies the proper linker file to the proper output directory
_MODULE = linker_cmd
include $(PRELUDE)
TARGET = lnk
TARGETTYPE = prebuilt
PREBUILT = $(call PATH_CONV,concerto/$(TARGETFOLDER)/lnk.cmd)
include $(FINALE)

# Copies the proper linker file to the proper output directory
_MODULE = regress_linker_cmd
include $(PRELUDE)
TARGET = lnk_reg
TARGETTYPE = prebuilt
PREBUILT = lnk_reg.cmd
include $(FINALE)

_MODULE := regress
include $(PRELUDE)
TARGET = $(_MODULE)_$(TARGET_CPU)
TARGETTYPE = exe
STATIC_LIBS := mmalib_d_$(TARGET_CPU) mmalib_$(TARGET_CPU) mmalib_cn_$(TARGET_CPU) common_$(TARGET_CPU) test_$(TARGET_CPU)
#ifneq ($(TARGET_SCPU),)
#STATIC_LIBS += c6xsim_$(TARGET_CPU)_$(TARGET_SCPU)
#endif
CSOURCES := main_c7x.c
LINKER_FILES := lnk.cmd
include $(FINALE)
ifeq ($(TARGET_SCPU),)
_MODULE := mma_self_test
include $(PRELUDE)
DEFS += C7X_MMA_BIST_STANDALONE
TARGET = $(_MODULE)_$(TARGET_CPU)
TARGETTYPE = exe
STATIC_LIBS := common_$(TARGET_CPU)
CSOURCES := main_c7x.c
LINKER_FILES := lnk.cmd
include $(FINALE)
endif
