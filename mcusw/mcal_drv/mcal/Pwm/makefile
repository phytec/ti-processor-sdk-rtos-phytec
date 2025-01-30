include $(MCUSW_INSTALL_PATH)/build/Rules.make

MODULE_NAME = pwm

ifeq ($(SOC),$(filter $(SOC), j7200 j721e j721s2 j784s4))
PACKAGE_SRCS_COMMON += .
SRCDIR += . src $(mcal_PATH)/Pwm/epwm_hw/V0_1/priv
SRCDIR += $(mcal_PATH)/Pwm/epwm_hw/V0_1/priv $(mcal_PATH)/Pwm/epwm_hw/V0/priv
INCDIR += . src $(mcal_PATH)
else
SRCDIR = src $(mcal_PATH)
INCDIR = src $(mcal_PATH)
endif

# List all the external components/interfaces, whose interface header files
# need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = autosarBSW autosarConfig autosarCompiler
# List all the internal submodules whose interface header files
# need to be included for this module
INCLUDE_INTERNAL_INTERFACES = pwm bsw_stubs

MODULE_SRC_BASE_PATH := $(mcal_PATH)/Pwm

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON += Pwm.c Pwm_Irq.c Pwm_Priv.c
SRCS_COMMON += Pwm_Gptimer.c Pwm_Ehrpwm.c
ifeq ($(SOC),$(filter $(SOC), j7200 j721e j721s2 j784s4))
SRCS_COMMON += timer.c lld_hrpwm.c lld_epwm.c
endif

PACKAGE_SRCS_COMMON = .
CFLAGS_LOCAL_COMMON = $(MCUSW_CFLAGS)

# Core/SoC/platform specific source files and CFLAGS
# Example:
#   SRCS_<core/SoC/platform-name> =
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
include $(MCUSW_INSTALL_PATH)/build/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk
#     and need not be explicitly specified here

# Nothing beyond this point
