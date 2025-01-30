include $(MCUSW_INSTALL_PATH)/build/Rules.make

ifeq ($(BUILD_OS_TYPE),freertos)
  APP_NAME = mcusw_ex01_multicore_demo_app
else ifeq ($(BUILD_OS_TYPE),baremetal)
  APP_NAME = mcusw_baremetal_ex01_multicore_demo_app
endif

SRCDIR = $(app_utils_PATH)/src

# List all the external libs required by the application
# Required for sciclient required by UDMA driver
EXT_LIB_LIST_COMMON += $(sciclient_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(sciclient_LIBNAME).$(LIBEXT)
# Required for uart
EXT_LIB_LIST_COMMON += $(uart_LIBPATH)/$(SOC)/$(ISA)/$(BUILD_PROFILE_$(CORE))/$(uart_LIBNAME).$(LIBEXT)
#Board module
EXT_LIB_LIST_COMMON += $(board_LIBPATH)/$(BOARD)/$(ISA)/$(BUILD_PROFILE_$(CORE))/$(board_LIBNAME).$(LIBEXT)
# Required by board
EXT_LIB_LIST_COMMON += $(i2c_LIBPATH)/$(SOC)/$(ISA)/$(BUILD_PROFILE_$(CORE))/$(i2c_LIBNAME).$(LIBEXT)
ifeq ($(BUILD_OS_TYPE),freertos)
  EXT_LIB_LIST_COMMON += $(osal_freertos_LIBPATH)/$(SOC)/$(ISA)/$(BUILD_PROFILE_$(CORE))/$(osal_freertos_LIBNAME).$(LIBEXT)
  EXT_LIB_LIST_COMMON += $(freertos_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(freertos_LIBNAME).$(LIBEXT)
endif
ifeq ($(BUILD_OS_TYPE),baremetal)
  EXT_LIB_LIST_COMMON += $(osal_nonos_LIBPATH)/$(SOC)/$(ISA)/$(BUILD_PROFILE_$(CORE))/$(osal_nonos_LIBNAME).$(LIBEXT)
endif
EXT_LIB_LIST_COMMON += $(csl_LIBPATH)/$(SOC)/$(ISA)/$(BUILD_PROFILE_$(CORE))/$(csl_LIBNAME).$(LIBEXT)

ifeq ($(BUILD_OS_TYPE),freertos)
COMP_LIST_COMMON = osal_freertos
SRCS_COMMON = main_rtos.c
else ifeq ($(BUILD_OS_TYPE),baremetal)
COMP_LIST_COMMON = osal_nonos
SRCS_COMMON = main_baremetal.c
ifeq ($(ISA),$(filter $(ISA), a53 a72))
  LNKFLAGS_LOCAL_$(CORE) += --entry Entry
endif
endif




ifneq ($(CORE),$(filter $(CORE), c7x_1 c66xdsp_1 c66xdsp_2))
  EXT_LIB_LIST_COMMON += $(csl_init_LIBPATH)/$(SOC)/$(ISA)/$(BUILD_PROFILE_$(CORE))/$(csl_init_LIBNAME).$(LIBEXT)
endif

ifeq ($(CORE),$(filter $(CORE), c66xdsp_1 c66xdsp_2))
  EXT_LIB_LIST_COMMON += $(csl_intc_LIBPATH)/$(SOC)/$(ISA)/$(BUILD_PROFILE_$(CORE))/$(csl_intc_LIBNAME).$(LIBEXT)
endif

CFLAGS_LOCAL_COMMON = $(MCUSW_CFLAGS)
LNKFLAGS_LOCAL_COMMON = $(MCUSW_LNKFLAGS)

ifeq ($(MCUSW_UART_ENABLE),TRUE)
    CFLAGS_LOCAL_COMMON += -DUART_ENABLED
ifeq ($(UART_PORT),)
    CFLAGS_LOCAL_COMMON += -DUART_WRITE_PORT=1
else
    CFLAGS_LOCAL_COMMON += -DUART_WRITE_PORT=$(UART_PORT)
endif
endif

include ../common/makefile.mk

