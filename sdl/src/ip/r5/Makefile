MODULE_NAME = r5f_core

SRCDIR= src
INCDIR= . $(sdl_PATH)/src/sdl

INCLUDE_INTERNAL_INTERFACES = r5f_core

# MPU Files
SRCS_ASM_COMMON += sdl_arm_r5_mpu.asm

# PMU Files
SRCS_ASM_COMMON += sdl_arm_r5_pmu.asm

# CCMR5 Files
SRCS_COMMON += sdl_mcu_armss_ccmr5.c

# VIM Files
SRCS_COMMON += sdl_vim.c


PACKAGE_SRCS_COMMON = .

MODULE_SRC_BASE_PATH = $(sdl_PATH)/src/ip/r5

# Include common make files
ifeq ($(MAKERULEDIR), )
#Makerule path not defined, define this and assume relative path from ROOTDIR
  MAKERULEDIR := $(ROOTDIR)/build/makerules
  export MAKERULEDIR
endif
include $(MAKERULEDIR)/common.mk
