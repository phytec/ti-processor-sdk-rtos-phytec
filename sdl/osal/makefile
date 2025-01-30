MODULE_NAME = sdl_osal

SRCDIR = . src/
INCDIR = .

INCDIR += $(sdl_PATH)/src/sdl $(sdl_PATH)/include $(sdl_PATH)/include/soc/$(SOC)

INCLUDE_INTERNAL_INTERFACES = sdl_osal

SRCS_COMMON += sdl_osal.c
PACKAGE_SRCS_COMMON += sdl_osal.h sdl_osal.c

MODULE_SRC_BASE_PATH = $(sdl_PATH)/osal

# Include common make files
ifeq ($(MAKERULEDIR), )
#Makerule path not defined, define this and assume relative path from ROOTDIR
  MAKERULEDIR := $(ROOTDIR)/build/makerules
  export MAKERULEDIR
endif
include $(MAKERULEDIR)/common.mk
