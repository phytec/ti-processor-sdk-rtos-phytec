ifeq ($(RTOS),FREERTOS)
	ifeq ($(RTOS_SDK),pdk)
		LINKER_CMD_FILES +=  $($(_MODULE)_SDIR)/$(SOC)_linker_freertos.cmd
	else
		LINKER_CMD_FILES +=  $($(_MODULE)_SDIR)/$(SOC)_linker_freertos_mcuplus.cmd
	endif
endif

LINKER_CMD_FILES +=  $($(_MODULE)_SDIR)/linker_mem_map.cmd

IDIRS+=$(VISION_APPS_PATH)/platform/$(SOC)/rtos

ifeq ($(RTOS),FREERTOS)
	LDIRS += $(PDK_PATH)/packages/ti/kernel/lib/$(SOC)/c7x_1/$(TARGET_BUILD)/
endif


include $($(_MODULE)_SDIR)/../concerto_c7x_inc.mak

# CPU instance specific libraries
STATIC_LIBS += app_rtos_common_c7x_1
ifeq ($(RTOS),FREERTOS)
	STATIC_LIBS += app_rtos
endif

#
# Suppress this warning, 10063-D: entry-point symbol other than "_c_int00" specified
# c7x boots in secure mode and to switch to non-secure mode we need to start at a special entry point '_c_int00_secure'
# and later after switching to non-secure mode, sysbios jumps to usual entry point of _c_int00
# Hence we need to suppress this warning
CFLAGS+=--diag_suppress=10063
