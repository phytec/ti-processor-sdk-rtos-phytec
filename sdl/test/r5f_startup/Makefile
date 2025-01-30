MODULE_NAME = r5f_startup

SRCDIR= .
INCDIR= . $(sdl_PATH)/src/sdl $(sdl_PATH)/src/ip/r5

INCLUDE_INTERNAL_INTERFACES = r5f_startup

# Startup Files
SRCS_COMMON += startup.c
SRCS_ASM_COMMON += boot.asm r5_startup.asm resetvecs.asm

# R5F Core Files
SRCS_COMMON += interrupt.c interrupt_handlers.c interrupt_register.c sdl_cache.c sdl_r5.c
SRCS_ASM_COMMON += sdl_arm_r5.asm sdl_arm_r5_hard_err_cache.asm handlers.asm

PACKAGE_SRCS_COMMON = .

MODULE_SRC_BASE_PATH = $(sdl_PATH)/test/r5f_startup

# Include common make files
ifeq ($(MAKERULEDIR), )
#Makerule path not defined, define this and assume relative path from ROOTDIR
  MAKERULEDIR := $(ROOTDIR)/build/makerules
  export MAKERULEDIR
endif
include $(MAKERULEDIR)/common.mk
